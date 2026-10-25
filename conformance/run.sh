#!/bin/sh
# Run the capture programs and the self-checking programs in ll-34, and
# compare the results with the reference PDP-11/34A.
# Requires a C11 compiler, make, macro11 and Go 1.18 or later.
#
# Generated sessions, logs and JSON reports in conformance/build/.
#
set -eu
R=$(CDPATH= cd -- "$(dirname -- "$0")/.." && pwd)
cd "$R"
B=conformance/build
mkdir -p "$B"

fail_log() {
    printf '%s\n' "$1" >&2
    tail -n 30 "$2" >&2
    exit 1
}
command -v go >/dev/null 2>&1 || { echo 'Go 1.18 or later is required' >&2; exit 1; }
sh conformance/build.sh >"$B/assemble.log" 2>&1 ||
    fail_log 'Assembly failed (is macro11 installed, or MACRO11 set?)' "$B/assemble.log"
make -s >"$B/build.log" 2>&1 || fail_log 'll-34 build failed' "$B/build.log"
go build -o "$B/compare" conformance/tools/compare.go >"$B/tools.log" 2>&1 ||
    fail_log 'Comparator build failed' "$B/tools.log"
${CC:-cc} -std=c11 -O2 -Wall -Wextra -I. -o "$B/test_probe_depth" \
    conformance/tools/test_probe_depth.c pdp11/probe/probe.c >"$B/probe-test.log" 2>&1 ||
    fail_log 'Probe test build failed' "$B/probe-test.log"
"$B/test_probe_depth" >>"$B/probe-test.log" 2>&1 ||
    fail_log 'Probe depth test failed' "$B/probe-test.log"
go test -count=1 conformance/tools/compare.go conformance/tools/compare_test.go >"$B/tools-test.log" 2>&1 ||
    fail_log 'Comparator tests failed' "$B/tools-test.log"

for kind in mpc datapath e107 E52 E53 E54 E80; do
    wiring=
    case "$kind" in
        mpc)
            listing=$B/mpc_sweep.lst
            probe=conformance/ll34-probes/mpc.probe
            machine=conformance/captures/mpc/MPC.sr ;;
        datapath)
            listing=$B/mpc_sweep.lst
            probe=conformance/ll34-probes/datapath.probe
            machine=conformance/captures/datapath/Datapath.sr ;;
        e107)
            listing=$B/e107_sweep.lst
            probe=conformance/ll34-probes/e107.probe
            machine=conformance/captures/e107/e107.sr ;;
        *)
            listing=$B/traps.lst
            probe=conformance/ll34-probes/$kind.probe
            machine=conformance/captures/traps/$kind.sr
            if [ "$kind" = E80 ]; then wiring=--wiring=e80-real; fi ;;
    esac
    # A failed export must never leave a stale reference to compare against.
    rm -f "$B/ll34-$kind.sr" "$B/$kind.json"
    "$B/compare" run ./ll-34 --timing aged --rtc 50 --lst "$listing" \
        --start 1000 --probe "$probe" </dev/null >"$B/$kind.log" 2>&1 ||
        fail_log "$kind: ll-34 run failed" "$B/$kind.log"
    "$B/compare" ${wiring:+"$wiring"} --report "$B/$kind.json" \
        "$kind" "$machine" "$B/ll34-$kind.sr"
done

# Reordered channels, missing channels and corruptions of every measured
# signal are tested in memory. No capture or ROM image is rewritten.
LL34_CONFORMANCE_INTEGRATION=1 go test -count=1 \
    conformance/tools/compare.go conformance/tools/compare_test.go >"$B/fault-tests.log" 2>&1 ||
    fail_log 'Capture fault-injection tests failed' "$B/fault-tests.log"

# Self-checking programs: their console output must match the one of the
# PDP-11/34A in programs/reference, except the line clock ticks (column 4).
P=conformance/programs
# Run a program in ll-34 until it has printed its passes, or for at most
# 300 s. The programs never stop by themselves.
run_program() {
    out=$B/ll34-$1.txt
    ./ll-34 --unpaced --timing aged --rtc 50 --lst "$B/$1.lst" --start 1000 \
        </dev/null >"$out.raw" 2>"$B/$1.log" &
    pid=$!
    t=0
    while kill -0 $pid 2>/dev/null && [ $t -lt 300 ]; do
        [ "$(tr -d '\r' <"$out.raw" | grep -c '^END')" -ge "$2" ] && break
        sleep 1; t=$((t + 1))
    done
    kill $pid 2>/dev/null || true
    wait $pid 2>/dev/null || true
    tr -d '\r' <"$out.raw" | awk -v n="$2" '{ print } /^END/ && ++e == n { exit }' >"$out"
}
SPECS='confrm:1 confrm_mmu:1 confrm_e62:1 confrm_mmu_detail:1 cfint:3'
for spec in $SPECS; do
    run_program "${spec%%:*}" "${spec#*:}" &
done
wait
KEY='NF { print $1, $2, $3, (NF > 4 ? $5 " " $6 " " $7 " " $8 " " $9 : "") }'
for spec in $SPECS; do
    name=${spec%%:*}
    awk "$KEY" "$P/reference/$name.txt" >"$B/$name.ref"
    awk "$KEY" "$B/ll34-$name.txt" >"$B/$name.out"
    if [ -s "$B/$name.out" ] && cmp -s "$B/$name.ref" "$B/$name.out"; then
        echo "$name: OK, $(wc -l <"$B/$name.out" | tr -d ' ') lines identical"
    else
        echo "$name: DIFFERS from the PDP-11/34A (see $B/ll34-$name.txt)" >&2
        exit 1
    fi
done
