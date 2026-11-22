# Kernel changes and rationale

## Android 16 / HyperOS 3 eBPF support

- **eBPF backport:** adapt the agate BPF implementation and its network, cgroup, VFS and API dependencies to Linux 4.14.180, including Android network BPF attachment, socket option call sites and the `CGROUP_INET_SOCK_RELEASE` hook during INET socket teardown. Implement required behavior instead of returning success from stub functions.

## GPU and display

- **KGSL/Adreno A640:** integrate GPU memory tracepoints, per-UID accounting and experimental A640 context profiling / GPU work-period instrumentation for Android performance telemetry. The same GPU-related configuration is selected in all three variants; hardware measurement accuracy requires runtime validation.
- **Display DSI/ESD IRQ:** restore IRQ disable-on-registration behavior (`IRQ_NOAUTOEN`) to avoid enabling the ESD interrupt before the driver is ready.

## Battery and power supplies

- **Fuel Gauge:** read all eight battery cycle buckets under one mutex, keeping the reported counter snapshot consistent.
- **Battery uevent:** bound optional BMS properties against the available uevent environment space, reserving capacity for the mandatory trailing `SEQNUM` and preserving essential battery fields. Keep the kernel overflow diagnostics enabled rather than suppressing warnings.
- **Power-supply sysfs permissions:** set the relevant attribute mode to `0644` rather than world-writable `0666`.

## Networking

- **Route and socket destination lifetime (R22–R24):** port upstream IPv4 fix `5018c59607a5`, and retire replaced FNHE routes to the uncached route list so an in-flight RCU reader does not lose the device reference prematurely. Reconcile the `dst_ops::negative_advice` callback contract across IPv4, IPv6, XFRM and DECnet: invalidate `sk_dst_cache` before releasing its socket-owned destination reference, avoiding stale cached routes after negative advice. The corrections target identified lifetime hazards; absence of all kernel memory-corruption paths is not yet established.

## Optional root features

- **BK / BKS:** integrate BakaSU v4.2.0-rc3 (35262) with `faccessat` and `stat` hook pointer types matching the actual VFS ABI.
- **BKS only:** integrate SUSFS 2.3.0 NON-GKI, fix the inappropriate hot-section placement of `susfs_is_inode_sus_kstat`, retain the relevant kallsyms filtering and apply the SUS_MAP check in `show_map()` as well as `show_smap()`.
- **No-root:** omit BakaSU and SUSFS while preserving the common BPF, GPU, display, battery and network functionality.

The EROFS configuration was enabled in the `nabu-v-oss` base and is not claimed as an added feature.
