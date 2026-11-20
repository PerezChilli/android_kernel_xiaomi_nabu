# Sources, contributors and licensing

This document records the provenance and author roles of code incorporated into the **Xiaomi Pad 5 (nabu), Linux 4.14.180** source trees. It distinguishes **original upstream authors**, **intermediate port maintainers** and the **nabu integration maintainer**. A GitHub repository owner, importer or synthetic Git commit author is not necessarily the author of the underlying code.

## Code contributors and upstream projects

| Component | Contributors / upstream project | Role in this repository |
| --- | --- | --- |
| Linux kernel | [Linux kernel contributors](https://www.kernel.org/) (see also [`CREDITS`](../CREDITS) and [`MAINTAINERS`](../MAINTAINERS)) | Core Linux 4.14 kernel code, infrastructure and upstream fixes |
| Xiaomi Pad 5 vendor/device support | Xiaomi, Qualcomm and individual vendor contributors; [`sticpaper/sticpaper_kernel_source`, `nabu-v-oss`](https://github.com/sticpaper/sticpaper_kernel_source/tree/nabu-v-oss) | Original device-specific sources and published base snapshot; `sticpaper` is credited as source publisher/maintainer, not sole author of the tree |
| eBPF backport reference | Upstream Linux/BPF contributors; [`Scaffold47/agate-ksun-susfs`](https://github.com/Scaffold47/agate-ksun-susfs); [`souta-lab/agate-bakasu-susfs`](https://github.com/souta-lab/agate-bakasu-susfs) | Linux 4.14.336 agate code lineage used as a donor/reference for BPF, UAPI, socket/cgroup/network dependencies |
| BakaSU / KernelSU lineage | [`Baka-SU/BakaSU` contributors](https://github.com/Baka-SU/BakaSU); [KernelSU project contributors](https://github.com/tiann/KernelSU) | Optional integrated kernel-side root code for `bk` and `bks`; locally included BakaSU revision `7b65bde0710b5e7c6f755545cb15a4abd417f3a8`, versionCode 35262, API/UAPI 6 |
| Original SUSFS | [`simonpunk/susfs4ksu`](https://gitlab.com/simonpunk/susfs4ksu) and its contributors | Original SUSFS project and functionality |
| Non-GKI SUSFS reference | [`wxx9248/susfs4ksu`](https://github.com/wxx9248/susfs4ksu) | Non-GKI Linux 4.9 reference used in the agate port lineage |
| Linux 4.14 SUSFS adaptation | [`souta-lab/agate-bakasu-susfs`](https://github.com/souta-lab/agate-bakasu-susfs/tree/feature/susfs-2.3.4.14-port) and upstream contributors | Intermediate Linux 4.14.336 / agate SUSFS 2.3.0 port adapted for nabu; not the original SUSFS author |
| IPv4 route-race correction | [Linux upstream fix `5018c59607a5`](https://git.zx2c4.com/linux-rng/commit/?id=5018c59607a511cdee743b629c76206d9c9e6d7b) and original upstream contributors | Backported networking correction |
| nabu integration and maintained repository | [`PerezChilli`](https://github.com/PerezChilli) | Reconstructed R22 Git/source trees; selection, adaptation and integration of kernel changes, project maintenance and device testing; no claim of exclusive authorship over upstream code |

This list is a **project-level attribution overview**, not a replacement for original copyright headers and full contributor histories. Original notices in individual source files remain authoritative. The project uses recovered source snapshots and reconstructed commits; those commit author fields must not be interpreted as an exhaustive or original attribution record.

## Provenance notes

- The initial nabu tree came from an archived `sticpaper_kernel_source-nabu-v-oss` source snapshot. The reconstructed baseline Git commit is an **import commit**, not the original upstream commit SHA.
- eBPF-related code was adapted to Linux 4.14.180 from the agate Linux 4.14.336 line, with modifications to BPF internals, headers/UAPI, networking, socket/cgroup hooks and dependencies. This does **not** make the agate tree the base kernel.
- The SUSFS chain is `simonpunk/susfs4ksu` → `wxx9248` (non-GKI reference) → `souta-lab` (agate 4.14 adaptation) → nabu adaptation. The separate upstream author and intermediate port maintainers are credited individually.
- In the `base` snapshot, EROFS already exists and is enabled; it is not claimed as an independent local feature.
- A successful build, older on-device functional tests or reconstructed version metadata do not prove a later R22 kernel is an exact reproduction of an earlier released binary.

## License compatibility notice

The Linux kernel source is generally under **GPL-2.0-only**, with its applicable per-file exceptions/notices. The integrated kernel-side BakaSU sources include GPL-2.0 licensing. Upstream SUSFS is presented as **GPL-3.0**; no verified GPL-2.0 relicensing permission or special compatibility exception for the specific imported SUSFS files has been established by this project.

Because the `bks` branch incorporates SUSFS into a GPL-2.0 kernel, **there is an unresolved license-compatibility concern for combined distribution**. Before redistributing combined sources or built images, review the precise file licenses and rights holders, and obtain additional permission where necessary. The presence of another public combined kernel project does not itself settle this question. Adding a GPL-2.0 header to imported SUSFS code would not resolve the underlying rights question.

All upstream license files and copyright notices should be retained. Nothing in this document changes the license selected by the original copyright holders, asserts new rights over imported contributions, or constitutes legal advice.

## Related project documentation

- [Changes and technical rationale](CHANGES.md)
- [BakaSU Manager compatibility](BAKASU-MANAGER.md) (`bk` / `bks`)
- [Linux kernel COPYING](../COPYING)
