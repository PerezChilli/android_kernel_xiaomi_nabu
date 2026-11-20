# Xiaomi Pad 5 (nabu) — Linux 4.14.180

This branch provides **BakaSU + SUSFS 2.3.0**, with kernel release **`4.14.180-bpf-os3-bks`**, targeting Android 16 / HyperOS 3.

| Branch | Root features |
| --- | --- |
| `public/no-root` | None |
| `public/bk` | BakaSU 35262 |
| `public/bks` | BakaSU 35262, SUSFS 2.3.0 NON-GKI |

The branches share the kernel's BPF, KGSL/Adreno GPU telemetry, display, battery and network changes. Each branch retains its own tracked Kconfig; differences beyond root support should be assessed directly against those profiles.

- [Sources and upstream attribution](docs/SOURCES.md)
- [Changes and technical rationale](docs/CHANGES.md)
