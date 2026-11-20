#!/usr/bin/env bash
set -euo pipefail
# Build all three Xiaomi Pad 5 kernel variants as ONE numbered kit.
# Layout: parent/{no-root,bk,bks} are sibling Git worktrees/checkouts.
# The counter increases once after ALL THREE builds successfully finish.
ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/../../.." && pwd)"
STATE="${KIT_COUNTER_FILE:-$ROOT/kit-counter.state}"
if [[ -f "$STATE" ]]; then
  read -r LAST < "$STATE"
else
  # Use the already shipped version as the base for a new kit.
  LAST=$(sed -nE 's/.*KBUILD_BUILD_VERSION:-([0-9]+).*/\1/p' "$ROOT/no-root/recovery/BUILD_METADATA.env" | head -1)
  [[ -n "$LAST" ]] || { echo 'Cannot determine last successful build number' >&2; exit 2; }
fi
[[ "$LAST" =~ ^[0-9]+$ ]] || { echo 'Invalid kit counter' >&2; exit 2; }
NEXT=$((LAST+1))
: "${LLVM_BIN:?Provide LLVM 20.1.8 bin path}"
export KBUILD_BUILD_VERSION="$NEXT"
export KBUILD_BUILD_USER="${KBUILD_BUILD_USER:-perezchilli}"
export KBUILD_BUILD_HOST="${KBUILD_BUILD_HOST:-build-host}"
export KBUILD_BUILD_TIMESTAMP="${KBUILD_BUILD_TIMESTAMP:-$(LC_ALL=C date -u '+%a %b %-d %T UTC %Y')}"
OUT_ROOT="${OUT_ROOT:-$ROOT/out}"
mkdir -p "$OUT_ROOT"
echo "Building full kit #${NEXT}, timestamp: $KBUILD_BUILD_TIMESTAMP"
for variant in no-root bk bks; do
  SRC="$ROOT/$variant"
  [[ -f "$SRC/recovery/scripts/build-recovery.sh" ]] || { echo "Missing source for $variant: $SRC" >&2; exit 3; }
  export OUT="$OUT_ROOT/$variant"
  bash "$SRC/recovery/scripts/build-recovery.sh"
  grep -Fq "#define UTS_VERSION \"#${NEXT} " "$OUT/include/generated/compile.h"
  test -s "$OUT/arch/arm64/boot/Image"
  cmp "$SRC/recovery/profile-reference.config" "$OUT/.config"
  echo "$variant finished: #${NEXT}"
done
printf '%s\n' "$NEXT" > "$STATE"
echo "COMPLETE kit #${NEXT}; next run will use #$((NEXT+1))"
