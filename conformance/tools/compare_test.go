package main

import (
	"archive/zip"
	"bytes"
	"encoding/binary"
	"fmt"
	"os"
	"path/filepath"
	"runtime"
	"strconv"
	"strings"
	"testing"
)

func projectRoot() string {
	_, file, _, _ := runtime.Caller(0)
	return filepath.Clean(filepath.Join(filepath.Dir(file), "../.."))
}
func makeSession(names []string, words []uint64) *session {
	s := &session{names: names, unit: 2, hz: 50e6, raw: make([]byte, 2*len(words))}
	for i, w := range words {
		binary.LittleEndian.PutUint16(s.raw[2*i:], uint16(w))
	}
	return s
}
func clone(s *session) *session {
	c := *s
	c.names = append([]string{}, s.names...)
	c.raw = append([]byte{}, s.raw...)
	return &c
}
func toggle(s *session, sample, ch int) { s.raw[sample*s.unit+ch/8] ^= 1 << uint(ch%8) }
func reversed(s *session) *session {
	c := clone(s)
	for i := range c.names {
		c.names[len(c.names)-1-i] = s.names[i]
	}
	for i := 0; i < s.count(); i++ {
		w := s.word(i)
		var v uint64
		for b := range s.names {
			v |= ((w >> b) & 1) << uint(len(s.names)-1-b)
		}
		for b := 0; b < c.unit; b++ {
			c.raw[i*c.unit+b] = byte(v >> uint(b*8))
		}
	}
	return c
}

func TestNames(t *testing.T) {
	s := &session{names: []string{"E107-5  │ K2-5 CC CODE 00 H", "E38-9 |K1-5 PROC CLK L", "E53-9/E59:9 | K2-6 IR CODE 02 L", "K2:E52:14"}}
	for query, want := range map[string]int{"E107:5": 0, " k2-5   cc code 00 h ": 0, "K1-5 PROC CLK L": 1, "E59-9": 2, "E52-14": 3} {
		got, err := s.channel(query)
		if err != nil || got != want {
			t.Fatalf("%q: %d %v", query, got, err)
		}
	}
	if _, err := s.channel("E99-1"); err == nil {
		t.Fatal("missing channel accepted")
	}
	s.names = append(s.names, "E38-9 | duplicate")
	if _, err := s.channel("E38-9"); err == nil {
		t.Fatal("duplicate channel accepted")
	}
}

func TestSessionZIP(t *testing.T) {
	for _, method := range []uint16{zip.Store, zip.Deflate} {
		t.Run(fmt.Sprint(method), func(t *testing.T) {
			path := filepath.Join(t.TempDir(), "test.sr")
			var buf bytes.Buffer
			z := zip.NewWriter(&buf)
			entries := []struct{ name, data string }{
				{"logic-1-2", string([]byte{0x78, 0x56})},
				{"metadata", "[global]\nsigrok version=0.5.2\n[device 1]\ntotal probes=16\nunitsize=2\nsamplerate=50 MHz\nprobe1=E52-5 │ TBIT\n"},
				{"version", "2"}, {"logic-1-1", string([]byte{0x34, 0x12})},
			}
			for _, e := range entries {
				f, err := z.CreateHeader(&zip.FileHeader{Name: e.name, Method: method})
				if err != nil {
					t.Fatal(err)
				}
				if _, err = f.Write([]byte(e.data)); err != nil {
					t.Fatal(err)
				}
			}
			if err := z.Close(); err != nil {
				t.Fatal(err)
			}
			if err := os.WriteFile(path, buf.Bytes(), 0644); err != nil {
				t.Fatal(err)
			}
			s, err := readSession(path)
			if err != nil {
				t.Fatal(err)
			}
			if s.hz != 50e6 || s.word(0) != 0x1234 || s.word(1) != 0x5678 {
				t.Fatalf("incorrect chunk decoding: %+v", s)
			}
			// ZIP CRC must reject a damaged stored logic byte.
			if method == zip.Store {
				bad := append([]byte{}, buf.Bytes()...)
				i := bytes.Index(bad, []byte{0x34, 0x12})
				if i < 0 {
					t.Fatal("payload missing")
				}
				bad[i] ^= 1
				os.WriteFile(path, bad, 0644)
				if _, err := readSession(path); err == nil {
					t.Fatal("damaged ZIP accepted")
				}
			}
		})
	}
}

func TestAlignment(t *testing.T) {
	ref := []event{{key: 1}, {key: 2}, {key: 1}, {key: 2}, {key: 3}, {key: 4}, {key: 5}}
	cap := append(append([]event{}, ref[3:]...), ref[:3]...)
	i, j, err := align(cap, ref, 2)
	if err != nil || i != 0 || j != 3 {
		t.Fatalf("cyclic anchor %d %d %v", i, j, err)
	}
	if _, _, err := align([]event{{key: 99}, {key: 98}, {key: 97}}, ref, 2); err == nil {
		t.Fatal("unrelated sequence aligned")
	}
}

func TestQualification(t *testing.T) {
	l := layout{addr: []string{"A0"}, outs: []string{"O1"}, ce: []string{"CE"}}
	words := make([]uint64, 0, 80)
	// Capture-edge plateaus are excluded. A settling glitch in the first
	// 80 ns is ignored. An output change in the qualified window is not.
	for _, w := range []uint64{0, 3, 0} {
		for j := 0; j < 20; j++ {
			words = append(words, w)
		}
	}
	s := makeSession([]string{"A0", "O1", "CE"}, words)
	r, err := qualify(s, l)
	if err != nil {
		t.Fatal(err)
	}
	if r.Stable != 1 || r.Cells[1][1] != 1 {
		t.Fatalf("unexpected cells: %+v", r)
	}
	toggle(s, 21, 1)
	r, _ = qualify(s, l)
	if r.Stable != 1 {
		t.Fatal("settling glitch rejected")
	}
	toggle(s, 30, 1)
	r, _ = qualify(s, l)
	if r.Unstable != 1 || r.Stable != 0 {
		t.Fatal("unstable output accepted")
	}
	toggle(s, 30, 1)
	for i := 20; i < 40; i++ {
		toggle(s, i, 2)
	}
	r, _ = qualify(s, l)
	if r.Disabled != 1 {
		t.Fatal("disabled chip accepted")
	}
}

func TestE80Wiring(t *testing.T) {
	s := &session{unit: 2, names: make([]string, 16)}
	for i := range s.names {
		s.names[i] = strconv.Itoa(i)
	}
	if err := nameE80(s); err != nil {
		t.Fatal(err)
	}
	if ch, err := s.channel("E80-6"); err != nil || ch != 12 {
		t.Fatal("incorrect VBA00 AND BDT channel")
	}
	if err := nameE80(s); err == nil {
		t.Fatal("numeric profile silently accepted named metadata")
	}
}

// run.sh enables these checks after it has exported fresh ll-34 captures.
// All corruption is in memory, never in the recorded captures or images.
func TestIntegrationFaults(t *testing.T) {
	if os.Getenv("LL34_CONFORMANCE_INTEGRATION") == "" {
		t.Skip("ll-34 captures not requested")
	}
	root := projectRoot()
	for _, kind := range []string{"mpc", "datapath", "e107", "E52", "E53", "E54", "E80"} {
		t.Run(kind, func(t *testing.T) {
			rel := map[string]string{"mpc": "mpc/MPC", "datapath": "datapath/Datapath", "e107": "e107/e107"}[kind]
			if rel == "" {
				rel = "traps/" + kind
			}
			hw, err := readSession(filepath.Join(root, "conformance/captures", rel+".sr"))
			if err != nil {
				t.Fatal(err)
			}
			sim, err := readSession(filepath.Join(root, "conformance/build", "ll34-"+kind+".sr"))
			if err != nil {
				t.Fatal(err)
			}
			cells := strings.HasPrefix(kind, "E")
			wiring := ""
			if kind == "E80" {
				wiring = "e80-real"
			}
			compare := func(h, s *session) error {
				if cells {
					_, e := compareCells(kind, clone(h), s, wiring)
					return e
				}
				_, e := compareCycles(kind, h, s)
				return e
			}
			if err := compare(hw, sim); err != nil {
				t.Fatal(err)
			}
			if err := compare(hw, reversed(sim)); err != nil {
				t.Fatalf("reordered ll-34 capture failed: %v", err)
			}
			if kind != "E80" {
				if err := compare(reversed(hw), sim); err != nil {
					t.Fatalf("reordered acquisition failed: %v", err)
				}
			} else {
				ignored := clone(hw)
				for k := 0; k < ignored.count(); k++ {
					for _, ch := range []int{1, 11, 13, 14, 15} {
						toggle(ignored, k, ch)
					}
				}
				if err := compare(ignored, sim); err != nil {
					t.Fatalf("unconnected E80 channels affected result: %v", err)
				}
			}
			missing := clone(sim)
			missing.names[0] = "not connected"
			if err := compare(hw, missing); err == nil {
				t.Fatal("missing ll-34 channel accepted")
			}
			if cells {
				l, _ := trapLayout(kind)
				named := clone(hw)
				if kind == "E80" {
					if err := nameE80(named); err != nil {
						t.Fatal(err)
					}
				}
				for _, name := range l.outs {
					if name == "" {
						continue
					}
					ch, e := named.channel(name)
					if e != nil {
						t.Fatal(e)
					}
					bad := clone(hw)
					for k := 0; k < bad.count(); k++ {
						toggle(bad, k, ch)
					}
					if err := compare(bad, sim); err == nil {
						t.Fatalf("corrupted %s accepted", name)
					}
				}
			} else {
				events, e := decode(hw, kind, false)
				if e != nil {
					t.Fatal(e)
				}
				index := len(events) / 2
				if kind == "e107" {
					index = 20000
				}
				edge := events[index].sample
				names := []string{}
				if kind == "mpc" {
					names = []string{"E97-1", "E97-2", "E97-3", "E97-4", "E97-6", "E97-16", "E97-5", "E97-7", "E97-17", "E97-14", "E97-13", "E38-12", "E38-2", "E38-7"}
				}
				if kind == "datapath" {
					for _, p := range []int{1, 2, 3, 4, 5, 6, 7, 9} {
						names = append(names, fmt.Sprintf("E82-%d", p))
					}
					for p := 1; p <= 4; p++ {
						names = append(names, fmt.Sprintf("E87-%d", p))
					}
				}
				if kind == "e107" {
					for _, p := range []int{5, 6, 7, 4, 3, 2, 1, 15, 9, 10} {
						names = append(names, fmt.Sprintf("E107-%d", p))
					}
				}
				for _, name := range names {
					ch, e := hw.channel(name)
					if e != nil {
						t.Fatal(e)
					}
					sample := edge - 1
					if kind == "mpc" && strings.HasPrefix(name, "E38-") {
						sample = edge + 2
					}
					bad := clone(hw)
					toggle(bad, sample, ch)
					if err := compare(bad, sim); err == nil {
						t.Fatalf("corrupted %s accepted", name)
					}
				}
			}
		})
	}
}
