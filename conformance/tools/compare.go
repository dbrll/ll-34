// Compare the pin levels captured on the machine with those exported by ll-34.

package main

import (
	"archive/zip"
	"bytes"
	"context"
	"encoding/binary"
	"encoding/json"
	"errors"
	"flag"
	"fmt"
	"io"
	"math"
	"os"
	"os/exec"
	"strconv"
	"strings"
	"time"
)

type session struct {
	raw   []byte
	names []string
	unit  int
	hz    float64
}

func readSession(path string) (*session, error) {
	z, err := zip.OpenReader(path)
	if err != nil {
		return nil, err
	}
	defer z.Close()
	files := map[string]*zip.File{}
	for _, f := range z.File {
		if files[f.Name] != nil {
			return nil, fmt.Errorf("duplicate ZIP entry %q", f.Name)
		}
		files[f.Name] = f
	}
	read := func(f *zip.File, limit int64) ([]byte, error) {
		if f == nil {
			return nil, errors.New("missing ZIP entry")
		}
		r, e := f.Open()
		if e != nil {
			return nil, e
		}
		defer r.Close()
		b, e := io.ReadAll(io.LimitReader(r, limit+1))
		if int64(len(b)) > limit {
			return nil, errors.New("ZIP entry exceeds size limit")
		}
		return b, e
	}
	version, err := read(files["version"], 16)
	if err != nil || strings.TrimSpace(string(version)) != "2" {
		return nil, errors.New("sigrok session version 2 required")
	}
	meta, err := read(files["metadata"], 65536)
	if err != nil {
		return nil, err
	}
	cfg := map[string]string{}
	device := false
	devices := 0
	for _, line := range strings.Split(string(meta), "\n") {
		line = strings.TrimSpace(line)
		if strings.HasPrefix(line, "[") {
			device = line == "[device 1]"
			if strings.HasPrefix(line, "[device ") {
				devices++
			}
			continue
		}
		if device {
			p := strings.SplitN(line, "=", 2)
			if len(p) == 2 {
				k := strings.TrimSpace(p[0])
				if _, exists := cfg[k]; exists {
					return nil, fmt.Errorf("duplicate metadata key %s", k)
				}
				cfg[k] = strings.TrimSpace(p[1])
			}
		}
	}
	if devices != 1 {
		return nil, errors.New("exactly one logic device required")
	}
	unit, err := strconv.Atoi(cfg["unitsize"])
	if err != nil || (unit != 1 && unit != 2 && unit != 4 && unit != 8) {
		return nil, errors.New("invalid sample width")
	}
	nch, err := strconv.Atoi(cfg["total probes"])
	if err != nil || nch < 1 || nch > 64 || nch > 8*unit {
		return nil, errors.New("invalid channel count")
	}
	fields := strings.Fields(cfg["samplerate"])
	if len(fields) != 2 {
		return nil, errors.New("invalid sample rate")
	}
	rate, err := strconv.ParseFloat(fields[0], 64)
	mul := map[string]float64{"Hz": 1, "kHz": 1e3, "MHz": 1e6, "GHz": 1e9}[fields[1]]
	if err != nil || mul == 0 || rate <= 0 || math.IsNaN(rate) || math.IsInf(rate*mul, 0) || rate*mul > 1e12 {
		return nil, errors.New("invalid sample rate")
	}
	s := &session{unit: unit, hz: rate * mul, names: make([]string, nch)}
	for i := range s.names {
		s.names[i] = cfg[fmt.Sprintf("probe%d", i+1)]
	}
	chunks := map[int]*zip.File{}
	for name, f := range files {
		if !strings.HasPrefix(name, "logic-1-") {
			continue
		}
		n, e := strconv.Atoi(strings.TrimPrefix(name, "logic-1-"))
		if e != nil || n < 1 || chunks[n] != nil {
			return nil, errors.New("invalid logic chunk name")
		}
		chunks[n] = f
	}
	if len(chunks) == 0 {
		return nil, errors.New("no logic chunks")
	}
	for n := 1; n <= len(chunks); n++ {
		b, e := read(chunks[n], 256<<20)
		if e != nil {
			return nil, fmt.Errorf("logic chunk %d: %w", n, e)
		}
		if len(b)%unit != 0 {
			return nil, errors.New("truncated logic sample")
		}
		if len(s.raw)+len(b) > 512<<20 {
			return nil, errors.New("session exceeds 512 MiB")
		}
		s.raw = append(s.raw, b...)
	}
	if len(s.raw) == 0 {
		return nil, errors.New("empty logic capture")
	}
	return s, nil
}

func (s *session) count() int { return len(s.raw) / s.unit }
func (s *session) word(i int) uint64 {
	b := s.raw[i*s.unit:]
	switch s.unit {
	case 1:
		return uint64(b[0])
	case 2:
		return uint64(binary.LittleEndian.Uint16(b))
	case 4:
		return uint64(binary.LittleEndian.Uint32(b))
	default:
		return binary.LittleEndian.Uint64(b)
	}
}
func normalize(s string) string { return strings.ToUpper(strings.Join(strings.Fields(s), " ")) }
func pin(s string) string {
	s = normalize(s)
	if strings.HasPrefix(s, "K1:") || strings.HasPrefix(s, "K2:") {
		s = s[3:]
	}
	return strings.ReplaceAll(s, ":", "-")
}
func (s *session) channel(query string) (int, error) {
	found := -1
	for i, name := range s.names {
		parts := strings.FieldsFunc(name, func(r rune) bool { return r == '|' || r == '│' })
		match := normalize(name) == normalize(query)
		for _, part := range parts {
			if normalize(part) == normalize(query) {
				match = true
			}
		}
		// Driver locations can contain a slash-separated list of wired-OR pins.
		if len(parts) > 0 {
			for _, loc := range strings.Split(parts[0], "/") {
				if pin(loc) == pin(query) {
					match = true
				}
			}
		}
		if match {
			if found >= 0 {
				return -1, fmt.Errorf("ambiguous channel %q", query)
			}
			found = i
		}
	}
	if found < 0 {
		return -1, fmt.Errorf("missing channel %q", query)
	}
	return found, nil
}
func channels(s *session, names []string) ([]int, error) {
	ch := make([]int, len(names))
	for i, name := range names {
		if name == "" {
			ch[i] = -1
			continue
		}
		n, err := s.channel(name)
		if err != nil {
			return nil, err
		}
		ch[i] = n
	}
	return ch, nil
}
func bits(w uint64, ch []int) uint16 {
	var v uint16
	for b, c := range ch {
		if c >= 0 {
			v |= uint16((w>>c)&1) << b
		}
	}
	return v
}

type event struct {
	key, data uint16
	sample    int
}

func decode(s *session, kind string, ll34 bool) ([]event, error) {
	var names []string
	clock := "E38-9"
	switch kind {
	case "mpc":
		pins := []int{1, 2, 3, 4, 6, 16, 5, 7, 17}
		for i, p := range pins {
			name := fmt.Sprintf("E97-%d", p)
			if ll34 {
				name = fmt.Sprintf("K2-7 MPC %02d H", i)
			}
			names = append(names, name)
		}
		names = append(names, "E38-12", "E38-2", "E38-7", "E97-14", "E97-13")
	case "datapath":
		for _, p := range []int{1, 2, 3, 4, 5, 6, 7, 9} {
			names = append(names, fmt.Sprintf("E82-%d", p))
		}
		for _, p := range []int{1, 2, 3, 4} {
			names = append(names, fmt.Sprintf("E87-%d", p))
		}
	case "e107":
		for _, p := range []int{5, 6, 7, 4, 3, 2, 1, 15, 9, 10} {
			names = append(names, fmt.Sprintf("E107-%d", p))
		}
		clock = "E97-9"
		if ll34 {
			names = []string{"K2-5 CC CODE 00 H", "K2-5 CC CODE 01 H", "K2-5 CC CODE 02 H", "K1-1 C BIT (1) H", "K2-5 ROT CBIT (1) H", "E107-2", "E107-1", "K1-10 CC N H", "E107-9", "E107-10"}
			clock = "K1-1 PSW CLK H"
		}
	default:
		return nil, errors.New("unknown cycle comparison")
	}
	if ll34 && kind != "e107" {
		clock = "K1-5 PROC CLK L"
	}
	ch, err := channels(s, names)
	if err != nil {
		return nil, err
	}
	clk, err := s.channel(clock)
	if err != nil {
		return nil, err
	}
	end := s.count()
	if kind == "mpc" {
		end -= 2
	}
	var out []event
	prev := s.word(0)
	for k := 1; k < end; k++ {
		w := s.word(k)
		if (prev>>clk)&1 == 0 && (w>>clk)&1 != 0 {
			v := bits(prev, ch)
			e := event{sample: k}
			switch kind {
			case "mpc":
				e.key = v & 511
				e.data = bits(s.word(k+2), ch[9:12]) | ((v >> 12) << 3)
			case "datapath":
				e.key = v
			case "e107":
				e.key = v & 64
				e.data = v
			}
			out = append(out, e)
		}
		prev = w
	}
	if len(out) == 0 {
		return nil, errors.New("no clock edges")
	}
	return out, nil
}

func keys(events []event) []byte {
	b := make([]byte, len(events)*2)
	for i, e := range events {
		binary.LittleEndian.PutUint16(b[2*i:], e.key)
	}
	return b
}
func findEven(hay, needle []byte, start int) int {
	for start <= len(hay) {
		k := bytes.Index(hay[start:], needle)
		if k < 0 {
			return -1
		}
		k += start
		if k%2 == 0 {
			return k
		}
		start = k + 1
	}
	return -1
}

// Growing unique-window alignment follows the reference scripts. MPC uses
// addresses and E107 uses S sign only. With no address channels in the
// datapath clip, its E82/E87 output sequence is the alignment key.
func align(capture, reference []event, window int) (int, int, error) {
	p := len(reference)
	if p == 0 {
		return 0, 0, errors.New("empty reference")
	}
	ref := keys(reference)
	hay := append(append([]byte{}, ref...), ref...)
	hay = hay[:len(hay)-2]
	seq := keys(capture)
	for i := 0; i < len(capture)-window; i++ {
		for w := window; i+w <= len(capture); w *= 2 {
			needle := seq[2*i : 2*(i+w)]
			k := findEven(hay, needle, 0)
			if k < 0 {
				break
			}
			second := findEven(hay, needle, k+2)
			// The duplicate at phase+p is the same cyclic alignment, not
			// a second candidate. Count only starts in the first loop.
			if second < 0 || second/2 >= p || w >= p {
				return i, k / 2, nil
			}
		}
	}
	return 0, 0, errors.New("no unique input-sequence alignment")
}

func compareCycles(kind string, hw, sim *session) (map[string]any, error) {
	cap, err := decode(hw, kind, false)
	if err != nil {
		return nil, err
	}
	ref, err := decode(sim, kind, true)
	if err != nil {
		return nil, err
	}
	// These are the periods of the supplied programs, not sample durations.
	period, window := 97984, 48
	if kind == "datapath" {
		window = 200
	}
	if kind == "e107" {
		period = 34488
	}
	if len(ref) < period+48 {
		return nil, errors.New("the ll-34 capture does not cover a complete loop and its repeated anchor")
	}
	for i := period; i < len(ref); i++ {
		if ref[i].key != ref[i%period].key || ref[i].data != ref[i%period].data {
			return nil, fmt.Errorf("the ll-34 capture is not periodic at event %d (expected period %d)", i, period)
		}
	}
	fullRef := ref
	ref = ref[:period]
	if kind == "e107" {
		if len(cap) < period {
			return nil, errors.New("E107 capture shorter than one event loop")
		}
		cap = cap[:period] // Matches the independent reference comparator.
	}
	start, phase, err := align(cap, ref, window)
	if err != nil {
		return nil, err
	}
	if kind == "e107" && start != 0 {
		return nil, errors.New("E107 must align from its first event")
	}
	mismatch := make([]int, 16)
	bad, count := 0, len(cap)-start
	cells := map[uint16]bool{}
	measured := map[uint16]map[uint16]int{}
	var realNS, ll34NS float64
	for t := 0; t < count; t++ {
		a, b := cap[start+t], ref[(phase+t)%period]
		var diff uint16
		switch kind {
		case "mpc":
			diff = (a.key ^ b.key) | ((a.data ^ b.data) << 9)
			cells[a.key] = true
		case "datapath":
			diff = a.key ^ b.key
		case "e107":
			diff = a.data ^ b.data
			address := a.data & 255
			cells[address] = true
			if measured[address] == nil {
				measured[address] = map[uint16]int{}
			}
			// The physical E107 image places C on O3 and V on O4.
			value := ((a.data>>9)&1)<<2 | ((a.data>>8)&1)<<3
			measured[address][value]++
		}
		if diff != 0 {
			bad++
			if bad <= 8 {
				fmt.Fprintf(os.Stderr, "%s event %d: machine %03o/%04o, ll-34 %03o/%04o\n", kind, t, a.key, a.data, b.key, b.data)
			}
		}
		for bit := range mismatch {
			if (diff>>bit)&1 != 0 {
				mismatch[bit]++
			}
		}
		if kind == "mpc" && t+1 < count {
			realNS += float64(cap[start+t+1].sample-a.sample) * 1e9 / hw.hz
			j := (phase + t) % period
			// The next edge interval executes the word latched at the current edge.
			ll34NS += float64(fullRef[j+1].sample-fullRef[j].sample) * 1e9 / sim.hz
		}
	}
	r := map[string]any{"kind": kind, "capture_start": start, "ll34_phase": phase, "period": period, "compared": count, "mismatching_events": bad, "mismatches_per_bit": mismatch, "distinct_addresses": len(cells)}
	if kind == "mpc" {
		r["timing_intervals"] = count - 1
		r["real_ns"] = realNS
		r["ll34_ns"] = ll34NS
		r["timing_percent"] = 100 * (ll34NS - realNS) / realNS
	}
	if kind == "e107" {
		r["measured_cells"], r["data_mask"], r["cells"] = len(cells), 12, measured
		if len(cells) != 101 {
			return r, fmt.Errorf("E107 measured %d cells, expected 101", len(cells))
		}
		for address, values := range measured {
			if len(values) != 1 {
				return r, fmt.Errorf("conflicting E107 outputs at address %03o", address)
			}
		}
	}
	if bad != 0 {
		return r, fmt.Errorf("%d mismatching events", bad)
	}
	return r, nil
}

type layout struct {
	addr, outs, ce []string
	mask, expected int
}

func trapLayout(kind string) (layout, error) {
	switch kind {
	case "E52":
		return layout{[]string{"E52-5", "E52-6", "E52-7", "E52-4", "E52-3", "E52-2", "E52-1", "E52-17", "E52-16", "E52-15"}, []string{"E52-14", "E52-13", "E52-12", "E52-11"}, []string{"E52-8"}, 15, 17}, nil
	case "E53":
		return layout{[]string{"E53-5", "E53-6", "E53-7", "E53-4", "E53-3", "E53-2", "E53-1", "E53-15"}, []string{"E53-12", "E52-16", "E52-17", "E52-1"}, []string{"E53-13", "E53-14"}, 15, 236}, nil
	case "E54":
		return layout{[]string{"E53-5", "E53-13", "E54-7", "E53-4", "E53-3", "E53-7", "E54-1", "E54-15"}, []string{"", "E54-11", "E54-10", "E54-9"}, nil, 14, 75}, nil
	case "E80":
		return layout{[]string{"E80-5", "E80-6", "E80-7", "E80-4", "E80-3", "E80-2", "", "E80-15"}, []string{"E80-12", "E80-11", "E80-10", "E80-9"}, nil, 15, 35}, nil
	}
	return layout{}, errors.New("unknown trap comparison")
}

// E80 alone has no signal names in its acquisition. This opt-in wiring
// profile is a declared pin map, not a fallback for missing metadata.
func nameE80(s *session) error {
	if s.unit != 2 || len(s.names) != 16 {
		return errors.New("E80 wiring requires 16 uint16 channels")
	}
	for i, name := range s.names {
		if name != strconv.Itoa(i) {
			return fmt.Errorf("E80 wiring requires original numeric metadata at D%d", i)
		}
	}
	for c, name := range map[int]string{0: "E80-5", 2: "E80-7", 3: "E80-4", 4: "E80-3", 5: "E80-2", 6: "E80-9", 7: "E80-15", 8: "E80-12", 9: "E80-11", 10: "E80-10", 12: "E80-6"} {
		s.names[c] = name
	}
	return nil
}

type cellRun struct{ address, data uint16 }
type cellResult struct {
	Cells    map[uint16]map[uint16]int `json:"cells"`
	Runs     []cellRun                 `json:"-"`
	Stable   int                       `json:"stable_intervals"`
	Disabled int                       `json:"disabled_intervals"`
	Short    int                       `json:"short_intervals"`
	Unstable int                       `json:"unstable_intervals"`
}

func qualify(s *session, l layout) (*cellResult, error) {
	addr, err := channels(s, l.addr)
	if err != nil {
		return nil, err
	}
	outs, err := channels(s, l.outs)
	if err != nil {
		return nil, err
	}
	ce, err := channels(s, l.ce)
	if err != nil {
		return nil, err
	}
	var keymask, outmask uint64
	for _, c := range append(append([]int{}, addr...), ce...) {
		if c >= 0 {
			keymask |= uint64(1) << c
		}
	}
	for _, c := range outs {
		if c >= 0 {
			outmask |= uint64(1) << c
		}
	}
	pre, post := int(math.Ceil(80*s.hz/1e9)), int(math.Ceil(20*s.hz/1e9))
	minimum := int(math.Ceil(160 * s.hz / 1e9))
	if minimum < pre+post+1 {
		minimum = pre + post + 1
	}
	r := &cellResult{Cells: map[uint16]map[uint16]int{}}
	start := 0
	k := s.word(0) & keymask
	for end := 1; end <= s.count(); end++ {
		var next uint64
		if end < s.count() {
			next = s.word(end) & keymask
		}
		if end < s.count() && next == k {
			continue
		}
		if start != 0 && end != s.count() {
			switch {
			case bits(k, ce) != 0:
				r.Disabled++
			case end-start < minimum:
				r.Short++
			default:
				d := s.word(start+pre) & outmask
				stable := true
				for j := start + pre + 1; j < end-post; j++ {
					if s.word(j)&outmask != d {
						stable = false
						break
					}
				}
				if !stable {
					r.Unstable++
				} else {
					a, v := bits(k, addr), bits(d, outs)
					if r.Cells[a] == nil {
						r.Cells[a] = map[uint16]int{}
					}
					r.Cells[a][v]++
					r.Runs = append(r.Runs, cellRun{a, v})
					r.Stable++
				}
			}
		}
		start, k = end, next
	}
	return r, nil
}
func collapse(runs []cellRun) []event {
	var seq []event
	for _, r := range runs {
		if len(seq) == 0 || seq[len(seq)-1].key != r.address {
			seq = append(seq, event{key: r.address})
		}
	}
	return seq
}
func anchor(capture, reference []event) (int, int, bool) {
	if len(capture) < 12 || len(reference) < 12 {
		return 0, 0, false
	}
	lookup := map[string]int{}
	for j := 0; j+12 <= len(reference); j++ {
		k := string(keys(reference[j : j+12]))
		if _, exists := lookup[k]; !exists {
			lookup[k] = j
		}
	}
	for i := 0; i+12 <= len(capture); i++ {
		if j, ok := lookup[string(keys(capture[i:i+12]))]; ok {
			return i, j, true
		}
	}
	return 0, 0, false
}
func compareCells(kind string, hw, sim *session, wiring string) (map[string]any, error) {
	l, err := trapLayout(kind)
	if err != nil {
		return nil, err
	}
	if kind == "E80" {
		if wiring != "e80-real" {
			return nil, errors.New("E80 requires --wiring=e80-real for its unnamed capture")
		}
		if err = nameE80(hw); err != nil {
			return nil, err
		}
	} else if wiring != "" {
		return nil, errors.New("wiring profile applies only to E80")
	}
	a, err := qualify(hw, l)
	if err != nil {
		return nil, err
	}
	// Shared IR CODE wires are named at their drivers in ll-34.
	if kind == "E53" {
		l.outs = []string{"E53-12", "K2-6 IR CODE 00 L", "K2-6 IR CODE 01 L", "K2-6 IR CODE 02 L"}
	}
	b, err := qualify(sim, l)
	if err != nil {
		return nil, err
	}
	bad, missing, conflicts := 0, 0, 0
	for _, result := range []*cellResult{a, b} {
		for address, values := range result.Cells {
			if len(values) != 1 {
				conflicts++
			}
			if kind == "E80" && ((address&64) != 0 || ((address&2) != 0 && (address&16) == 0)) {
				return nil, fmt.Errorf("electrically impossible E80 address %03o", address)
			}
		}
	}
	for address, values := range a.Cells {
		expected := b.Cells[address]
		if len(expected) == 0 {
			missing++
			fmt.Fprintf(os.Stderr, "%s cell %04o not reached in ll-34\n", kind, address)
			continue
		}
		for d, n := range values {
			if expected[d] == 0 {
				bad += n
				fmt.Fprintf(os.Stderr, "%s cell %04o: machine %o, ll-34 %v\n", kind, address, d, expected)
			}
		}
	}
	ci, ri, ok := anchor(collapse(a.Runs), collapse(b.Runs))
	r := map[string]any{"kind": kind, "data_mask": l.mask, "measured_cells": len(a.Cells), "ll34_cells": len(b.Cells), "machine": a, "ll34": b, "mismatching_intervals": bad, "missing_cells": missing, "conflicting_cells": conflicts, "input_anchor_found": ok, "machine_anchor": ci, "ll34_anchor": ri, "alignment_scope": "12-address input-only anchor, not timing validation"}
	if len(a.Cells) != l.expected {
		return r, fmt.Errorf("measured %d cells, expected %d", len(a.Cells), l.expected)
	}
	if bad+missing+conflicts != 0 {
		return r, fmt.Errorf("%d mismatching intervals, %d missing cells, %d conflicts", bad, missing, conflicts)
	}
	// A cell comparison does not require a sequence anchor. Report its absence
	// rather than claiming cycle-by-cycle alignment of asynchronous clips.
	return r, nil
}

func runLL34(args []string) error {
	if len(args) == 0 {
		return errors.New("run requires an executable")
	}
	ctx, cancel := context.WithTimeout(context.Background(), 60*time.Second)
	defer cancel()
	cmd := exec.CommandContext(ctx, args[0], args[1:]...)
	cmd.Stdin, cmd.Stdout, cmd.Stderr = os.Stdin, os.Stdout, os.Stderr
	err := cmd.Run()
	if ctx.Err() != nil {
		return errors.New("ll-34 ran for more than 60 seconds")
	}
	return err
}

func run(args []string) error {
	if len(args) > 0 && args[0] == "run" {
		return runLL34(args[1:])
	}
	flags := flag.NewFlagSet("compare", flag.ContinueOnError)
	report := flags.String("report", "", "write detailed JSON report")
	wiring := flags.String("wiring", "", "explicit acquisition wiring profile")
	dump := flags.String("dump", "", "write decoded capture events as little-endian uint16 values")
	if err := flags.Parse(args); err != nil {
		return err
	}
	a := flags.Args()
	if len(a) != 3 {
		return errors.New("usage: compare [--report FILE] [--wiring e80-real] KIND MACHINE.sr LL34.sr")
	}
	hw, err := readSession(a[1])
	if err != nil {
		return fmt.Errorf("machine capture: %w", err)
	}
	sim, err := readSession(a[2])
	if err != nil {
		return fmt.Errorf("ll-34 capture: %w", err)
	}
	var r map[string]any
	if strings.HasPrefix(a[0], "E") {
		r, err = compareCells(a[0], hw, sim, *wiring)
	} else {
		r, err = compareCycles(a[0], hw, sim)
	}
	if r != nil {
		r["machine_file"], r["ll34_file"] = a[1], a[2]
		r["machine_samples"], r["ll34_samples"] = hw.count(), sim.count()
		r["machine_hz"], r["ll34_hz"] = hw.hz, sim.hz
		r["passed"] = err == nil
		if *report != "" {
			data, e := json.MarshalIndent(r, "", "  ")
			if e != nil {
				return e
			}
			if e = os.WriteFile(*report, append(data, '\n'), 0644); e != nil {
				return e
			}
		}
	}
	if *dump != "" && !strings.HasPrefix(a[0], "E") {
		events, e := decode(hw, a[0], false)
		if e != nil {
			return e
		}
		data := make([]byte, len(events)*2)
		for i, v := range events {
			value := v.key
			if a[0] == "e107" {
				value = v.data
			}
			binary.LittleEndian.PutUint16(data[2*i:], value)
		}
		if e = os.WriteFile(*dump, data, 0644); e != nil {
			return e
		}
	}
	if err != nil {
		return err
	}
	if strings.HasPrefix(a[0], "E") {
		fmt.Printf("%s: OK, %v measured cells, %v qualified intervals, mask 0x%X\n", a[0], r["measured_cells"], r["machine"].(*cellResult).Stable, r["data_mask"])
	} else {
		suffix := ""
		if a[0] == "e107" {
			suffix = ", 101 measured cells"
		}
		fmt.Printf("%s: OK, %v events, period %v, no mismatches%s\n", a[0], r["compared"], r["period"], suffix)
	}
	return nil
}
func main() {
	if err := run(os.Args[1:]); err != nil {
		fmt.Fprintln(os.Stderr, "compare:", err)
		os.Exit(1)
	}
}
