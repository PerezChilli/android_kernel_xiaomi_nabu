/* SPDX-License-Identifier: GPL-2.0-only */
/*
 * Experimental Android GpuWork reporting for Adreno A640 (nabu).
 * Raw CP_ALWAYS_ON_CONTEXT interval ticks were validated on this device.
 * Approximate busy by summing serial context-active command durations for
 * every UID (including multiple processes belonging to one UID).
 * Do NOT use submit->retire latency or the absolute ctx counter as time.
 */
#include <linux/kernel.h>
#include <linux/ktime.h>
#include <linux/math64.h>
#include <linux/spinlock.h>
#include <linux/workqueue.h>
#include <linux/jiffies.h>
#include <linux/types.h>

#include "kgsl_trace_power.h"
#include "kgsl_gpu_work_period.h"

#define GPUWORK_GPU_ID 1U
#define GPUWORK_UID_SLOTS 128
#define GPUWORK_FLUSH_MS 500
#define GPUWORK_MAX_WINDOW_NS 900000000ULL
/* Hardware XO context clock: 19,200,000 ticks/s. */
#define GPUWORK_MAX_TICKS 19200000ULL

struct gpuwork_uid_slot {
	u32 uid;
	bool used;
	u64 begin_ns;
	u64 active_ns;
	u64 last_seen_ns;
};

static DEFINE_SPINLOCK(gpuwork_lock);
static struct gpuwork_uid_slot gpuwork_uids[GPUWORK_UID_SLOTS];
static bool gpuwork_scheduled;
static void gpuwork_flush(struct work_struct *work);
static DECLARE_DELAYED_WORK(gpuwork_work, gpuwork_flush);

/* No pointers to KGSL processes or contexts escape into the workqueue. */
static struct gpuwork_uid_slot *gpuwork_get_slot(u32 uid, u64 now)
{
	struct gpuwork_uid_slot *candidate = NULL;
	struct gpuwork_uid_slot *oldest = NULL;
	int i;

	for (i = 0; i < GPUWORK_UID_SLOTS; i++) {
		struct gpuwork_uid_slot *p = &gpuwork_uids[i];

		if (p->used && p->uid == uid)
			return p;
		if (!p->used && !candidate)
			candidate = p;
		if (p->used && !p->active_ns &&
		    (!oldest || p->last_seen_ns < oldest->last_seen_ns))
			oldest = p;
	}

	if (!candidate)
		candidate = oldest;
	if (!candidate)
		return NULL; /* Busy with 128 concurrent UIDs: drop safely. */

	candidate->uid = uid;
	candidate->used = true;
	candidate->begin_ns = now;
	candidate->active_ns = 0;
	candidate->last_seen_ns = now;
	return candidate;
}

void kgsl_gpuwork_account(u32 uid, u64 ctx_start, u64 ctx_end)
{
	struct gpuwork_uid_slot *slot;
	unsigned long flags;
	u64 ticks, active_ns, now;
	bool start_worker = false;

	/* Drop invalid/wrapped samples instead of producing fake GPU time. */
	if (uid == (u32)-1 || !ctx_start || ctx_end <= ctx_start)
		return;
	ticks = ctx_end - ctx_start;
	if (ticks > GPUWORK_MAX_TICKS)
		return;

	active_ns = div64_u64(ticks * 10000ULL, 192ULL);
	if (!active_ns)
		return;
	now = ktime_get_raw_ns();

	spin_lock_irqsave(&gpuwork_lock, flags);
	slot = gpuwork_get_slot(uid, now);
	if (slot) {
		/* Treat a long idle gap as a new reporting window. */
		if (!slot->active_ns && now > slot->begin_ns &&
		    now - slot->begin_ns > GPUWORK_MAX_WINDOW_NS)
			slot->begin_ns = now;
		/* Saturate the value; emission also clamps to window time. */
		if (U64_MAX - slot->active_ns < active_ns)
			slot->active_ns = U64_MAX;
		else
			slot->active_ns += active_ns;
		slot->last_seen_ns = now;
		if (!gpuwork_scheduled) {
			gpuwork_scheduled = true;
			start_worker = true;
		}
	}
	spin_unlock_irqrestore(&gpuwork_lock, flags);

	if (start_worker)
		schedule_delayed_work(&gpuwork_work,
			msecs_to_jiffies(GPUWORK_FLUSH_MS));
}

static void gpuwork_flush(struct work_struct *work)
{
	u64 now = ktime_get_raw_ns();
	unsigned long flags;
	bool again = false;
	int i;

	for (i = 0; i < GPUWORK_UID_SLOTS; i++) {
		struct gpuwork_uid_slot *slot = &gpuwork_uids[i];
		u32 uid = 0;
		u64 start = 0, active = 0;

		spin_lock_irqsave(&gpuwork_lock, flags);
		if (slot->used && slot->active_ns && now > slot->begin_ns) {
			uid = slot->uid;
			start = slot->begin_ns;
			active = slot->active_ns;
			slot->active_ns = 0;
			slot->begin_ns = now;
			/* Guarantee the Android maximum one-second period ABI. */
			if (now - start > GPUWORK_MAX_WINDOW_NS)
				start = now - GPUWORK_MAX_WINDOW_NS;
			if (active > now - start)
				active = now - start;
		}
		spin_unlock_irqrestore(&gpuwork_lock, flags);

		/* Never trace while holding the UID bookkeeping spinlock. */
		if (active && start < now)
			trace_gpu_work_period(GPUWORK_GPU_ID, uid, start,
				now, active);
	}

	spin_lock_irqsave(&gpuwork_lock, flags);
	for (i = 0; i < GPUWORK_UID_SLOTS; i++) {
		if (gpuwork_uids[i].active_ns) {
			again = true;
			break;
		}
	}
	gpuwork_scheduled = again;
	spin_unlock_irqrestore(&gpuwork_lock, flags);

	if (again)
		schedule_delayed_work(&gpuwork_work,
			msecs_to_jiffies(GPUWORK_FLUSH_MS));
}
