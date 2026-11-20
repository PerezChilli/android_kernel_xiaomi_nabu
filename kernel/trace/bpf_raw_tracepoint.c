/* SPDX-License-Identifier: GPL-2.0 */
/* Backport of BPF raw tracepoint transport from the Agate 4.14.336 donor.
 * This file is intentionally separate from the older Perf bpf_trace.c.
 */
#include <linux/kernel.h>
#include <linux/bpf.h>
#include <linux/filter.h>
#include <linux/trace_events.h>
#include <linux/tracepoint.h>
#include <linux/syscalls.h>
#include <linux/perf_event.h>
#include <linux/module.h>
#include <linux/mutex.h>
#include <linux/rcupdate.h>
#include <linux/slab.h>
#include <linux/init.h>

static DEFINE_MUTEX(bpf_raw_tp_mutex);

#ifdef CONFIG_MODULES
struct bpf_raw_tp_module {
    struct list_head list;
    struct module *module;
};
static LIST_HEAD(bpf_raw_tp_modules);
static DEFINE_MUTEX(bpf_raw_tp_module_mutex);

static struct bpf_raw_event_map *bpf_raw_tp_find_module(const char *name)
{
    struct bpf_raw_tp_module *entry;
    struct bpf_raw_event_map *map;
    unsigned int i;

    mutex_lock(&bpf_raw_tp_module_mutex);
    list_for_each_entry(entry, &bpf_raw_tp_modules, list) {
        for (i = 0; i < entry->module->num_bpf_raw_events; i++) {
            map = &entry->module->bpf_raw_events[i];
            if (!strcmp(map->tp->name, name)) {
                if (!try_module_get(entry->module))
                    continue;
                mutex_unlock(&bpf_raw_tp_module_mutex);
                return map;
            }
        }
    }
    mutex_unlock(&bpf_raw_tp_module_mutex);
    return NULL;
}

static int bpf_raw_tp_module_notify(struct notifier_block *nb,
                                    unsigned long action, void *data)
{
    struct module *mod = data;
    struct bpf_raw_tp_module *entry, *tmp;
    int ret = 0;

    if (!mod->num_bpf_raw_events ||
        (action != MODULE_STATE_COMING && action != MODULE_STATE_GOING))
        return NOTIFY_DONE;

    mutex_lock(&bpf_raw_tp_module_mutex);
    if (action == MODULE_STATE_COMING) {
        entry = kzalloc(sizeof(*entry), GFP_KERNEL);
        if (!entry) {
            ret = -ENOMEM;
        } else {
            entry->module = mod;
            list_add(&entry->list, &bpf_raw_tp_modules);
        }
    } else {
        list_for_each_entry_safe(entry, tmp, &bpf_raw_tp_modules, list) {
            if (entry->module == mod) {
                list_del(&entry->list);
                kfree(entry);
                break;
            }
        }
    }
    mutex_unlock(&bpf_raw_tp_module_mutex);
    return notifier_from_errno(ret);
}

static struct notifier_block bpf_raw_tp_module_nb = {
    .notifier_call = bpf_raw_tp_module_notify,
};

static int __init bpf_raw_tp_module_init(void)
{
    return register_module_notifier(&bpf_raw_tp_module_nb);
}
fs_initcall(bpf_raw_tp_module_init);
#else
static struct bpf_raw_event_map *bpf_raw_tp_find_module(const char *name)
{
    return NULL;
}
#endif

extern struct bpf_raw_event_map __start__bpf_raw_tp[];
extern struct bpf_raw_event_map __stop__bpf_raw_tp[];

struct bpf_raw_event_map *bpf_get_raw_tracepoint(const char *name)
{
    struct bpf_raw_event_map *map;

    for (map = __start__bpf_raw_tp; map < __stop__bpf_raw_tp; map++)
        if (!strcmp(map->tp->name, name))
            return map;

    return bpf_raw_tp_find_module(name);
}

void bpf_put_raw_tracepoint(struct bpf_raw_event_map *map)
{
#ifdef CONFIG_MODULES
    struct module *owner;

    preempt_disable();
    owner = __module_address((unsigned long)map);
    module_put(owner);
    preempt_enable();
#endif
}

int bpf_probe_register(struct bpf_raw_event_map *map, struct bpf_prog *prog)
{
    int ret;

    if (prog->aux->max_ctx_offset > map->num_args * sizeof(u64) ||
        prog->aux->max_tp_access > map->writable_size)
        return -EINVAL;
    mutex_lock(&bpf_raw_tp_mutex);
    /* Perf 4.14's register API rejects duplicates; it provides the
     * same underlying tracepoint lifetime and ref counting. */
    ret = tracepoint_probe_register(map->tp, map->bpf_func, prog);
    mutex_unlock(&bpf_raw_tp_mutex);
    return ret;
}

int bpf_probe_unregister(struct bpf_raw_event_map *map, struct bpf_prog *prog)
{
    int ret;

    mutex_lock(&bpf_raw_tp_mutex);
    ret = tracepoint_probe_unregister(map->tp, map->bpf_func, prog);
    mutex_unlock(&bpf_raw_tp_mutex);
    return ret;
}

static __always_inline void bpf_trace_execute(struct bpf_prog *prog, u64 *args)
{
    /* Paired with BPF program release; no sleepable BPF handling here. */
    rcu_read_lock();
    (void)BPF_PROG_RUN(prog, args);
    rcu_read_unlock();
}

#define UNPACK(...) __VA_ARGS__
#define REPEAT_1(FN, DL, X, ...) FN(X)
#define REPEAT_2(FN, DL, X, ...) FN(X) UNPACK DL REPEAT_1(FN, DL, __VA_ARGS__)
#define REPEAT_3(FN, DL, X, ...) FN(X) UNPACK DL REPEAT_2(FN, DL, __VA_ARGS__)
#define REPEAT_4(FN, DL, X, ...) FN(X) UNPACK DL REPEAT_3(FN, DL, __VA_ARGS__)
#define REPEAT_5(FN, DL, X, ...) FN(X) UNPACK DL REPEAT_4(FN, DL, __VA_ARGS__)
#define REPEAT_6(FN, DL, X, ...) FN(X) UNPACK DL REPEAT_5(FN, DL, __VA_ARGS__)
#define REPEAT_7(FN, DL, X, ...) FN(X) UNPACK DL REPEAT_6(FN, DL, __VA_ARGS__)
#define REPEAT_8(FN, DL, X, ...) FN(X) UNPACK DL REPEAT_7(FN, DL, __VA_ARGS__)
#define REPEAT_9(FN, DL, X, ...) FN(X) UNPACK DL REPEAT_8(FN, DL, __VA_ARGS__)
#define REPEAT_10(FN, DL, X, ...) FN(X) UNPACK DL REPEAT_9(FN, DL, __VA_ARGS__)
#define REPEAT_11(FN, DL, X, ...) FN(X) UNPACK DL REPEAT_10(FN, DL, __VA_ARGS__)
#define REPEAT_12(FN, DL, X, ...) FN(X) UNPACK DL REPEAT_11(FN, DL, __VA_ARGS__)
#define REPEAT_13(FN, DL, X, ...) FN(X) UNPACK DL REPEAT_12(FN, DL, __VA_ARGS__)
#define REPEAT_14(FN, DL, X, ...) FN(X) UNPACK DL REPEAT_13(FN, DL, __VA_ARGS__)
#define REPEAT_15(FN, DL, X, ...) FN(X) UNPACK DL REPEAT_14(FN, DL, __VA_ARGS__)
#define REPEAT_16(FN, DL, X, ...) FN(X) UNPACK DL REPEAT_15(FN, DL, __VA_ARGS__)
#define REPEAT_17(FN, DL, X, ...) FN(X) UNPACK DL REPEAT_16(FN, DL, __VA_ARGS__)
#define REPEAT_18(FN, DL, X, ...) FN(X) UNPACK DL REPEAT_17(FN, DL, __VA_ARGS__)
#define REPEAT(X, FN, DL, ...) REPEAT_##X(FN, DL, __VA_ARGS__)
#define SARG(X) u64 arg##X
#define COPY(X) args[X] = arg##X
#define __DL_COM (,)
#define __DL_SEM (;)
#define __SEQ_0_17 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17
#define DEFINE_BPF_RAW_RUN(x) \
    void bpf_trace_run##x(struct bpf_prog *prog, REPEAT(x, SARG, __DL_COM, __SEQ_0_17)) \
    { \
        u64 args[x]; \
        REPEAT(x, COPY, __DL_SEM, __SEQ_0_17); \
        bpf_trace_execute(prog, args); \
    } \
    EXPORT_SYMBOL_GPL(bpf_trace_run##x)
DEFINE_BPF_RAW_RUN(1);
DEFINE_BPF_RAW_RUN(2);
DEFINE_BPF_RAW_RUN(3);
DEFINE_BPF_RAW_RUN(4);
DEFINE_BPF_RAW_RUN(5);
DEFINE_BPF_RAW_RUN(6);
DEFINE_BPF_RAW_RUN(7);
DEFINE_BPF_RAW_RUN(8);
DEFINE_BPF_RAW_RUN(9);
DEFINE_BPF_RAW_RUN(10);
DEFINE_BPF_RAW_RUN(11);
DEFINE_BPF_RAW_RUN(12);
DEFINE_BPF_RAW_RUN(13);
DEFINE_BPF_RAW_RUN(14);
DEFINE_BPF_RAW_RUN(15);
DEFINE_BPF_RAW_RUN(16);
DEFINE_BPF_RAW_RUN(17);
DEFINE_BPF_RAW_RUN(18);

/* TASK_FD_QUERY for tracepoint and syscall perf events. This reports only
 * event classes that this older Perf tree can describe accurately; kprobe
 * and uprobe query support needs a separate integration with trace_probe. */
int bpf_get_perf_event_info(const struct perf_event *event, u32 *prog_id,
                            u32 *fd_type, const char **name,
                            u64 *probe_offset, u64 *probe_addr)
{
    struct trace_event_call *tp;
    bool regular_tp, syscall_tp;

    if (!event->prog)
        return -ENOENT;
    if (event->prog->type == BPF_PROG_TYPE_PERF_EVENT)
        return -EOPNOTSUPP;
    if (event->attr.type != PERF_TYPE_TRACEPOINT || !event->tp_event)
        return -EOPNOTSUPP;

    tp = event->tp_event;
    regular_tp = tp->flags & TRACE_EVENT_FL_TRACEPOINT;
    syscall_tp = is_syscall_trace_event(tp);
    if (!regular_tp && !syscall_tp)
        return -EOPNOTSUPP;

    *prog_id = event->prog->aux->id;
    *fd_type = BPF_FD_TYPE_TRACEPOINT;
    *name = regular_tp ? tp->tp->name : tp->name;
    *probe_offset = 0;
    *probe_addr = 0;
    return 0;
}
