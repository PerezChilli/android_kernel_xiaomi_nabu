# Xiaomi Pad 5 (nabu) — Linux 4.14.180

This branch provides **BakaSU**, with kernel release **`4.14.180-bpf-os3-bk`**, targeting Android 16 / HyperOS 3.

| Branch | Root features |
| --- | --- |
| `public/no-root` | None |
| `public/bk` | BakaSU 35262 |
| `public/bks` | BakaSU 35262, SUSFS 2.3.0 NON-GKI |

The branches share the kernel's BPF, KGSL/Adreno GPU telemetry, display, battery and network changes. Each branch retains its own tracked Kconfig; differences beyond root support should be assessed directly against those profiles.

- [Sources and upstream attribution](docs/SOURCES.md)
- [Changes and technical rationale](docs/CHANGES.md)

## Required BakaSU Android manager

For this kernel, use **BakaSU Manager v4.2.0-rc3 (versionCode 35262)** for **ARM64 / arm64-v8a**:

`BakaSU_v4.2.0-rc3_35262-arm64-v8a-release.apk`

This matches the integrated BakaSU kernel revision `7b65bde` (`v4.2.0-rc3-7b65bde-offline`, KernelSU API / UAPI **6**). Do not substitute a generic KernelSU manager or a different BakaSU build unless its compatibility has been verified. The Android manager APK is **not** bundled with this kernel source repository; obtain it from the corresponding BakaSU release and verify the publisher before installing it.

See [BakaSU manager compatibility](docs/BAKASU-MANAGER.md) for details.
