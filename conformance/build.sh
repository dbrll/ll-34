#!/bin/sh
# Assemble the programs and the capture programs into build/, with macro11
# (Krehbiel, Hoppe, Seibert). MACRO11 is its path if it is not in PATH.
set -e
cd "$(dirname "$0")"
D=$(pwd)
M=${MACRO11:-macro11}
mkdir -p build
rc=0
for src in programs/confrm.mac programs/confrm_mmu.mac programs/confrm_e62.mac \
           programs/confrm_mmu_detail.mac programs/cfint.mac programs/cfint_detail.mac \
           captures/mpc/mpc_sweep.mac captures/traps/traps.mac \
           timing/timing.mac captures/e107/e107_sweep.mac; do
    name=$(basename "$src" .mac)
    lst=$D/build/$name.lst
    # .INCLUDE looks in the current directory
    (cd "$(dirname "$src")" && "$M" "$name.mac" -o "$D/build/$name.obj" -l "$lst") \
        >/dev/null 2>&1 || true
    err=$(grep -c 'ERROR' "$lst" || true)
    # The loaders read words only: no byte line.
    bytes=$(awk 'length($0) > 20 && substr($0, 10, 6) ~ /^[0-7]{6}$/ &&
        substr($0, 17, 4) ~ /^[0-7]{3} $/' "$lst" | wc -l | tr -d ' ')
    if [ "$err" != 0 ] || [ "$bytes" != 0 ]; then
        echo "$name: $err errors, $bytes byte lines" >&2; rc=1
    fi
done
exit $rc
