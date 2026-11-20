/* SPDX-License-Identifier: GPL-2.0-only */
#ifndef _KGSL_GPU_WORK_PERIOD_H_
#define _KGSL_GPU_WORK_PERIOD_H_
#include <linux/types.h>
#ifdef CONFIG_QCOM_KGSL_GPU_WORK_PERIOD
void kgsl_gpuwork_account(u32 uid, u64 ctx_start, u64 ctx_end);
#endif
#endif
