#!/usr/bin/env bash
set -euo pipefail
# Xiaomi Pad 5 / nabu-v-oss source tree; isolated out-of-tree build.
# Requires GNU make, bc, flex, bison, Python 3 and Clang/LLD 20.1.8.
SRC="$(cd "$(dirname "${BASH_SOURCE[0]}")/../.." && pwd)"
PROFILE="$(cat "$SRC/recovery/BUILD_PROFILE")"
OUT="${OUT:-$SRC/../out-nabu-$PROFILE}"
: "${LLVM_BIN:?Provide absolute path to Clang/LLD 20.1.8 /bin directory}"
export PATH="$LLVM_BIN:$PATH"
[[ "$(clang --version | head -1)" == *'20.1.8'* ]] || { echo 'ERROR: requires Clang 20.1.8' >&2; exit 1; }
[[ "$(ld.lld --version)" == *'20.1.8'* ]] || { echo 'ERROR: requires LLD 20.1.8' >&2; exit 1; }
mkdir -p "$OUT" "$OUT/firmware"
# Android vendor source stores embedded firmware as Intel HEX. For O= builds,
# pre-convert it into the output tree: .gen.S references O=/firmware/*.bin.
# Do NOT disable a Kconfig feature to work around a missing binary.
for ihex in "$SRC"/firmware/*.bin.ihex; do
    [[ -e "$ihex" ]] || continue
    fw="$(basename "$ihex" .ihex)"
    if [[ ! -s "$OUT/firmware/$fw" || "$ihex" -nt "$OUT/firmware/$fw" ]]; then
        llvm-objcopy -Iihex -Obinary "$ihex" "$OUT/firmware/$fw"
    fi
done
# Vendor Kbuild 4.14 invokes asm-generic submakes from O= and can miss
# $(srctree) include directories. Generate official asm-generic wrappers
# explicitly from Kbuild definitions before building kernel/bounds.s.
for sub in uapi/asm asm; do
    make -s -C "$OUT" -I "$SRC" -f "$SRC/scripts/Makefile.asm-generic" \
        srctree="$SRC" SRCARCH=arm64 src="$sub" \
        obj="arch/arm64/include/generated/$sub"
done
cp "$SRC/recovery/profile-reference.config" "$OUT/.config"
ARGS=(O="$OUT" ARCH=arm64 CC=clang LD=ld.lld AR=llvm-ar NM=llvm-nm
      OBJCOPY=llvm-objcopy OBJDUMP=llvm-objdump STRIP=llvm-strip
      LLVM_IAS=1 CROSS_COMPILE=aarch64-linux-gnu- CLANG_TRIPLE=aarch64-linux-gnu-)
if [[ -f "$SRC/recovery/BUILD_METADATA.env" ]]; then
    # Literal tracked values. Caller can explicitly override timestamp/version.
    # shellcheck disable=SC1091
    source "$SRC/recovery/BUILD_METADATA.env"
else
    export KBUILD_BUILD_USER="${KBUILD_BUILD_USER:-perezchilli}"
    export KBUILD_BUILD_HOST="${KBUILD_BUILD_HOST:-build-host}"
fi
make -C "$SRC" "${ARGS[@]}" olddefconfig
# Preserve a diff: Kconfig may remove unsupported/hidden symbols.
diff -u "$SRC/recovery/profile-reference.config" "$OUT/.config" > "$OUT/reference-config.diff" || true
make -C "$SRC" "${ARGS[@]}" -j"${JOBS:-$(nproc)}" Image.gz-dtb
printf 'Build output: %s\nConfiguration drift: %s\n' "$OUT" "$OUT/reference-config.diff"