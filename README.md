# Xiaomi Pad 5 (nabu) — Linux 4.14.180

Linux kernel source tree for **Xiaomi Pad 5 (nabu)**, adapted for **HyperOS 3 / Android 16**. The repository maintains three kernel configurations with a shared set of Android compatibility and device-specific changes.

## Branches

| Branch | Kernel configuration | Root support |
| --- | --- | --- |
| [`nabu-w-oss`](https://github.com/PerezChilli/android_kernel_xiaomi_nabu/tree/nabu-w-oss) | No-root | Disabled |
| [`nabu-w-oss-bk`](https://github.com/PerezChilli/android_kernel_xiaomi_nabu/tree/nabu-w-oss-bk) | BakaSU | BakaSU v4.2.0-rc3 |
| [`nabu-w-oss-bks`](https://github.com/PerezChilli/android_kernel_xiaomi_nabu/tree/nabu-w-oss-bks) | BakaSU + SUSFS | BakaSU v4.2.0-rc3 and SUSFS v2.3.0 (non-GKI) |
| [`nabu-v-oss`](https://github.com/PerezChilli/android_kernel_xiaomi_nabu/tree/nabu-v-oss) | Base kernel source snapshot | Reference baseline |

**Current branch:** No-root (`nabu-w-oss`)
**Kernel release configuration:** `4.14.180-bpf-os3`

## Kernel changes

- Android 16-oriented **eBPF backport**, including network, socket and cgroup interfaces and supporting kernel dependencies.
- **KGSL / Adreno A640** telemetry and experimental profiling work; hardware-counter accuracy is not yet established for every path.
- Display DSI/ESD interrupt initialization adjustments.
- Battery fuel-gauge cycle-count consistency and power-supply uevent safeguards.
- IPv4 routing race fix adapted from Linux upstream.
- Optional BakaSU and SUSFS integration, selected by branch and Kconfig.

See [technical changes](docs/CHANGES.md) for implementation details. EROFS was already present in the starting `nabu-v-oss` configuration and is **not** claimed as a new addition.

## BakaSU Android manager

This is the **no-root** configuration. An Android root manager is not required and cannot enable root access when root support is disabled in the kernel. For rooted builds, use the `nabu-w-oss-bk` or `nabu-w-oss-bks` branch.

## Building

Build on **Linux** (Ubuntu/Debian or another environment with a case-sensitive filesystem). A typical macOS APFS checkout is case-insensitive and cannot hold every source file correctly (`xt_DSCP.c` and `xt_dscp.c` are different files); do not build from such a checkout. A Linux VM or WSL2 with files stored in the Linux filesystem is suitable.

The kernel variants are configured for **Clang/LLD 20.1.8**. The tracked script uses the selected branch's `recovery/BUILD_PROFILE` and `recovery/profile-reference.config` (there is no need to substitute a generic defconfig). It checks the compiler versions before building.

**1. Install host dependencies (Ubuntu/Debian example):**

```bash
sudo apt update
sudo apt install -y git build-essential bc bison flex python3 \
  libssl-dev libelf-dev libncurses-dev \
  gcc-aarch64-linux-gnu binutils-aarch64-linux-gnu
```

Install **Clang and LLD 20.1.8** separately and make sure the same toolchain directory also contains `llvm-ar`, `llvm-nm`, `llvm-objcopy`, `llvm-objdump` and `llvm-strip`.

**2. Clone the repository and select a kernel variant:**

```bash
git clone https://github.com/PerezChilli/android_kernel_xiaomi_nabu.git
cd android_kernel_xiaomi_nabu
git switch nabu-w-oss
```

Use `nabu-w-oss` for **no-root**, `nabu-w-oss-bk` for **BakaSU**, or `nabu-w-oss-bks` for **BakaSU + SUSFS**. Each branch has its own tracked kernel configuration.

**3. Build the kernel:**

```bash
export LLVM_BIN=/absolute/path/to/clang-20.1.8/bin
"$LLVM_BIN/clang" --version
"$LLVM_BIN/ld.lld" --version
JOBS="$(nproc)" bash recovery/scripts/build-recovery.sh
```

The script performs an out-of-tree build, prepares the required firmware and generated headers, restores the branch-specific configuration, and builds `Image.gz-dtb`.

Some older vendor drivers may report Clang 20.1.8 `-Werror=unused-but-set-variable` errors. Treat these as build issues to investigate, not a reason to disable required Kconfig options. Earlier recovery experiments used `KCFLAGS=-Wno-error=unused-but-set-variable` as a narrowly scoped diagnostic workaround; such builds still require full verification.

**4. Inspect the result:**

```bash
PROFILE="$(cat recovery/BUILD_PROFILE)"
OUT="../out-nabu-$PROFILE"
ls -lh "$OUT/arch/arm64/boot/Image.gz-dtb"
cat "$OUT/reference-config.diff"
```

By default, build output is stored in `../out-nabu-<profile>/`. The `reference-config.diff` file should be inspected for changes introduced by `olddefconfig`; a nonempty diff may indicate configuration drift. Override the output path with `OUT=/absolute/output/directory` if needed.

> **Important:** this procedure builds the kernel (`Image.gz-dtb`), **not** a flashable Android `boot.img`. Packaging a boot image requires matching boot-header parameters, device-specific boot components/ramdisk and separate verification. Do not treat a successful compilation as a successful device boot test.

For build-system details, see [`recovery/scripts/build-recovery.sh`](recovery/scripts/build-recovery.sh) and the tracked [`recovery/`](recovery/) files.

## Source provenance and credits

The base is the `nabu-v-oss` source snapshot distributed by [sticpaper](https://github.com/sticpaper/sticpaper_kernel_source/tree/nabu-v-oss), **not** a wholesale replacement with an agate kernel. eBPF backport references and the SUSFS 4.14 port were adapted from the [souta-lab agate project](https://github.com/souta-lab/agate-bakasu-susfs), itself building on the [Scaffold47 agate tree](https://github.com/Scaffold47/agate-ksun-susfs). BakaSU and SUSFS retain their respective upstream authorship.

Full source lineage, explicit contributors and roles, and additional upstream fixes are listed in [**Sources, credits and licensing**](docs/SOURCES.md). Independent hosting and nabu-specific integration do not imply original authorship of imported code.

## License and redistribution notice

The Linux kernel source tree is distributed under **GNU GPL-2.0-only** (see [`COPYING`](COPYING)); copyright and license notices of the original authors remain applicable. Kernel-side BakaSU code also contains GPL-2.0 licensing. **SUSFS upstream is marked GPL-3.0**, while this kernel tree uses GPL-2.0-only components. The compatibility of the combined `nabu-w-oss-bks` source/binary distribution has **not** been established, and this repository does not claim a relicensing grant or special exception from SUSFS rights holders.

**Before redistributing combined SUSFS/kernel sources or binaries, independently review license compatibility and obtain any permissions that may be required.** The existence of this public repository or other published ports is not proof of such permission. This notice does not replace or override any upstream license terms.

**No warranty:** these sources and any derived images are provided as-is, without a guarantee of functionality, safety, compatibility or fitness for a particular purpose. This is a source-development repository, not a stable firmware release.

For attribution or copyright concerns, please open a GitHub issue. See [source and license details](docs/SOURCES.md).
