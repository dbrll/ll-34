CC     = cc
CFLAGS = -Wall -Wextra -O3 -flto -std=c11 -I.
LDFLAGS =

TARGET = ll-34

SRCS = frontends/cli/main.c pdp11/machine.c pdp11/unibus/unibus.c pdp11/unibus/ram.c pdp11/unibus/loader.c \
       pdp11/kd11ea/kd11ea.c pdp11/kd11ea/clockgen.c pdp11/kd11ea/clock.c pdp11/kd11ea/int.c pdp11/kd11ea/debug.c pdp11/kd11ea/mmu.c \
       pdp11/m9301/rom.c \
       pdp11/dl11/dl11.c \
       pdp11/rk11/rk11.c \
       pdp11/rl11/rl11.c \
       pdp11/kw11/kw11.c \
       pdp11/pc11/pc11.c \
       pdp11/ky11/console_sim.c \
       pdp11/probe/probe.c pdp11/probe/probe_export.c \
       frontends/tty/tty_stdio.c frontends/tty/tty_tcp.c frontends/tty/tty_pty.c \
       pdp11/debug/disasm.c pdp11/debug/memdump.c

OBJS = $(SRCS:.c=.o)

ROM_FILES = $(wildcard pdp11/kd11ea/roms/M8265/*.bin pdp11/kd11ea/roms/M8266/*.bin)
ROM_HEADER = pdp11/kd11ea/roms/rom_images.h
ROM_GENERATOR = pdp11/kd11ea/roms/gen_rom_header.c

HEADERS = pdp11/machine.h pdp11/unibus/unibus.h pdp11/unibus/ram.h pdp11/unibus/loader.h pdp11/trace.h \
          pdp11/kd11ea/kd11ea.h pdp11/kd11ea/rom_wiring.h pdp11/kd11ea/bus_wiring.h pdp11/kd11ea/ucode_rom.h $(ROM_HEADER) pdp11/kd11ea/clockgen.h pdp11/kd11ea/clock.h pdp11/kd11ea/int.h pdp11/kd11ea/debug.h pdp11/kd11ea/mmu.h \
          pdp11/m9301/rom.h pdp11/m9301/m9301_yf_rom.h \
          pdp11/dl11/dl11.h \
          pdp11/rk11/rk11.h \
          pdp11/rl11/rl11.h \
          pdp11/kw11/kw11.h \
          pdp11/pc11/pc11.h \
          pdp11/ky11/console.h \
          pdp11/probe/probe.h \
          frontends/tty/tty.h \
          pdp11/debug/disasm.h pdp11/debug/memdump.h

all: $(TARGET)

$(ROM_HEADER): $(ROM_GENERATOR) $(ROM_FILES)
	$(CC) -std=c11 -O2 -o pdp11/kd11ea/roms/.gen_rom_header $(ROM_GENERATOR)
	@trap 'rm -f pdp11/kd11ea/roms/.gen_rom_header' EXIT; \
	    pdp11/kd11ea/roms/.gen_rom_header

roms: $(ROM_HEADER)

$(TARGET): $(OBJS)
	$(CC) $(LDFLAGS) -o $@ $(OBJS)

%.o: %.c $(HEADERS)
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	rm -f $(OBJS) $(TARGET) pdp11/kd11ea/roms/.gen_rom_header pdp11/kd11ea/roms/rom_images.h.tmp

# ----------------------------------------------------------------
# WebAssembly target  (requires Emscripten: source emsdk/emsdk_env.sh)
# ----------------------------------------------------------------
EMCC = emcc
WASM_CFLAGS = -Wall -Wextra -O2 -std=c11 -I.

# Same sources as native, minus:
#   frontends/cli/main.c       -> replaced by frontends/wasm/main_wasm.c
#   frontends/tty/tty_stdio.c  -> replaced by frontends/tty/tty_wasm.c
#   frontends/tty/tty_tcp.c    -> not applicable in browser
#   frontends/tty/tty_pty.c    -> not applicable in browser
WASM_DIR  = frontends/wasm
WASM_SRCS = $(filter-out frontends/cli/main.c pdp11/kd11ea/clock.c frontends/tty/tty_stdio.c frontends/tty/tty_tcp.c frontends/tty/tty_pty.c, $(SRCS)) \
            frontends/tty/tty_wasm.c $(WASM_DIR)/main_wasm.c

WASM_EXPORTS = '["_main",\
"_wasm_rx_push","_wasm_tx_poll",\
"_wasm_set_odt","_wasm_load_rk_image","_wasm_load_rl_image","_wasm_load_lda","_wasm_load_asc",\
"_wasm_reset","_wasm_power_on","_wasm_power_off",\
"_wasm_set_timing_profile","_wasm_get_timing_profile",\
"_wasm_console_key","_wasm_get_display_addr","_wasm_get_display_data","_wasm_get_leds",\
"_wasm_get_reg","_wasm_get_psw","_wasm_get_ir","_wasm_get_us",\
"_wasm_is_halted","_wasm_halt","_wasm_run","_wasm_step",\
"_wasm_ustep_n","_wasm_poll_halt",\
"_wasm_examine","_wasm_deposit","_wasm_ram_ptr","_wasm_ram_words",\
"_wasm_disasm","_wasm_disasm_len",\
"_wasm_get_mpc","_wasm_get_mpc_label","_wasm_get_mpc_desc","_wasm_get_ba","_wasm_get_b_reg","_wasm_get_bx_reg",\
"_wasm_get_alu_out","_wasm_get_alu_cout","_wasm_get_udata",\
"_wasm_get_mmu_sr0","_wasm_get_mmu_sr2","_wasm_get_mmu_par","_wasm_get_mmu_pdr",\
"_wasm_bp_set","_wasm_bp_del","_wasm_bp_clear","_wasm_bp_count","_wasm_bp_addr",\
"_wasm_get_trace_count","_wasm_get_trace_pc","_wasm_get_trace_ir","_wasm_trace_clear",\
"_wasm_probe_sig_count","_wasm_probe_sig_name","_wasm_probe_sig_desc",\
"_wasm_probe_add","_wasm_probe_rm","_wasm_probe_clear_sigs",\
"_wasm_probe_sel_count","_wasm_probe_sel_name",\
"_wasm_probe_set_trigger","_wasm_probe_set_depth","_wasm_probe_set_rate","_wasm_probe_set_pos",\
"_wasm_probe_arm","_wasm_probe_disarm","_wasm_probe_state",\
"_wasm_probe_sample_count","_wasm_probe_read",\
"_wasm_probe_sel_is_1bit","_wasm_probe_read_ns",\
"_wasm_probe_sig_alias","_wasm_probe_sig_loc","_wasm_probe_sig_group","_wasm_probe_chip_name","_wasm_probe_sig_is_1bit",\
"_wasm_probe_clear_trigger","_wasm_probe_set_trigger_edge","_wasm_probe_trigger_index"]'

$(WASM_DIR)/ll-34.js: $(WASM_SRCS) $(HEADERS)
	$(EMCC) $(WASM_CFLAGS) -o $(WASM_DIR)/ll-34.js $(WASM_SRCS) \
	    -s EXPORTED_FUNCTIONS=$(WASM_EXPORTS) \
	    -s EXPORTED_RUNTIME_METHODS='["ccall","cwrap","FS","HEAPU16"]' \
	    -s ALLOW_MEMORY_GROWTH=1 \
	    -s INITIAL_MEMORY=67108864

wasm: $(WASM_DIR)/ll-34.js

clean-wasm:
	rm -f $(WASM_DIR)/ll-34.js $(WASM_DIR)/ll-34.wasm

.PHONY: all clean roms wasm clean-wasm
