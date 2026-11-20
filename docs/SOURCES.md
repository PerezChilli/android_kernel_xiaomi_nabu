# Sources and attribution

The kernel targets **Xiaomi Pad 5 (nabu)**, Linux **4.14.180**, Android 16 / HyperOS 3.

| Component | Source | Use in this kernel |
| --- | --- | --- |
| Base kernel | [sticpaper/sticpaper_kernel_source — nabu-v-oss](https://github.com/sticpaper/sticpaper_kernel_source/tree/nabu-v-oss) | Base nabu kernel sources and device drivers; EROFS was already enabled in the original `nabu_defconfig` |
| eBPF | [souta-lab/agate-bakasu-susfs](https://github.com/souta-lab/agate-bakasu-susfs), from the agate Linux 4.14.336 code lineage including [Scaffold47/agate-ksun-susfs](https://github.com/Scaffold47/agate-ksun-susfs) | Backported and adapted BPF programs, maps, verifier/JIT-related infrastructure, cgroup/socket hooks, UAPI and supporting kernel APIs for nabu 4.14.180 |
| SUSFS 2.3.0 NON-GKI | [souta-lab/agate-bakasu-susfs — feature/susfs-2.3.4.14-port](https://github.com/souta-lab/agate-bakasu-susfs/tree/feature/susfs-2.3.4.14-port) | Linux 4.14 port adapted to nabu VFS, procfs, namespaces, fsnotify, and BakaSU hooks |
| SUSFS upstream | [simonpunk/susfs4ksu](https://gitlab.com/simonpunk/susfs4ksu) and [wxx9248/susfs4ksu](https://github.com/wxx9248/susfs4ksu) | Upstream SUSFS and earlier non-GKI reference used by the agate port |
| BakaSU | [Baka-SU/BakaSU](https://github.com/Baka-SU/BakaSU) | Optional root implementation (BK/BKS), integrated as revision `7b65bde0710b5e7c6f755545cb15a4abd417f3a8`, version 35262 / UAPI 6 |
| IPv4 route race | [Linux upstream fix 5018c59607a5](https://git.zx2c4.com/linux-rng/commit/?id=5018c59607a511cdee743b629c76206d9c9e6d7b) | Ported to the Linux 4.14 IPv4 routing implementation |

The code was adapted for the nabu kernel interfaces; the donor kernel was **not** used as a wholesale replacement for the base tree.
