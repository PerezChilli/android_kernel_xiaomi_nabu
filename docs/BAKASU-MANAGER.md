# BakaSU Manager compatibility — Xiaomi Pad 5 (nabu)

Applies to the [`nabu-w-oss-bk`](https://github.com/PerezChilli/android_kernel_xiaomi_nabu/tree/nabu-w-oss-bk) (BakaSU) and [`nabu-w-oss-bks`](https://github.com/PerezChilli/android_kernel_xiaomi_nabu/tree/nabu-w-oss-bks) (BakaSU + SUSFS 2.3.0) branches.

| Item | Required / matched value |
| --- | --- |
| Android manager | **BakaSU Manager v4.2.0-rc3** |
| Android APK versionCode | **35262** |
| APK ABI | **arm64-v8a** (ARM64) |
| Expected APK filename | `BakaSU_v4.2.0-rc3_35262-arm64-v8a-release.apk` |
| Bundled kernel-side BakaSU source | Revision `7b65bde` (`7b65bde0710b5e7c6f755545cb15a4abd417f3a8`) |
| Kernel-side version string | `v4.2.0-rc3-7b65bde-offline` |
| KernelSU API/UAPI | **6** |
| `nabu-w-oss` (no-root) branch | No Android root manager required or useful |

The kernel-side values are defined in `KernelSU/kernel/Kbuild` and were corroborated against recorded kernel logs for previously working builds. Use the matching **BakaSU** manager instead of a generic KernelSU APK; other releases are not validated for this reconstruction.

The APK is intentionally **not** included in the source tree. Obtain the matching official BakaSU APK from a trusted upstream source and verify its authenticity. This note does not verify availability or signatures of external APK files, nor does it certify R22 runtime behavior.
