# BakaSU Android manager compatibility — Xiaomi Pad 5 (nabu)

Applies to source branches `public/bk` (BakaSU) and `public/bks` (BakaSU + SUSFS 2.3.0).

| Item | Required / matched value |
| --- | --- |
| Android manager | **BakaSU Manager v4.2.0-rc3** |
| Android APK versionCode | **35262** |
| APK ABI | **arm64-v8a** (ARM64) |
| Expected APK filename | `BakaSU_v4.2.0-rc3_35262-arm64-v8a-release.apk` |
| Bundled kernel-side BakaSU source | Revision `7b65bde` (`7b65bde0710b5e7c6f755545cb15a4abd417f3a8`) |
| Kernel-side version string | `v4.2.0-rc3-7b65bde-offline` |
| KernelSU API/UAPI | **6** |
| No-root branch | No Android manager required or useful |

The kernel-side values are defined in `KernelSU/kernel/Kbuild` and were corroborated against the recorded R20 kernel logs. Use the matching **BakaSU** manager, rather than a generic KernelSU APK; other releases are not validated for this reconstruction.

The APK is intentionally **not** included in the public source tree. Obtain the exact matching, officially published BakaSU APK from a trusted upstream release and check its authenticity. This repository's source-branch compatibility guidance does **not** itself verify an external APK's signature or availability.

This note specifies the Android management application only. It does not change kernel build steps, root features, or SUSFS behavior.
