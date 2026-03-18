CC     = cc
CFLAGS = -Wall -Wextra -O2 -std=c11 -I.
LDFLAGS =

TARGET = ll-34

SRCS = main.c unibus/unibus.c unibus/ram.c unibus/loader.c \
       kd11ea/kd11ea.c kd11ea/clockgen.c kd11ea/clock.c kd11ea/int.c kd11ea/debug.c kd11ea/mmu.c \
       m9301/rom.c \
       dl11/dl11.c \
       rk11/rk11.c \
       rl11/rl11.c \
       kw11/kw11.c \
       pc11/pc11.c \
       console/console_sim.c \
       probe/probe.c \
       tty/tty_stdio.c tty/tty_tcp.c tty/tty_pty.c \
       debug/disasm.c debug/memdump.c

OBJS = $(SRCS:.c=.o)

HEADERS = unibus/unibus.h unibus/ram.h unibus/loader.h trace.h \
          kd11ea/kd11ea.h kd11ea/combo_roms.h kd11ea/ucode_rom.h kd11ea/clockgen.h kd11ea/clock.h kd11ea/int.h kd11ea/debug.h kd11ea/mmu.h \
          m9301/rom.h m9301/m9301_yf_rom.h \
          dl11/dl11.h \
          rk11/rk11.h \
          rl11/rl11.h \
          kw11/kw11.h \
          pc11/pc11.h \
          console/console.h \
          probe/probe.h \
          tty/tty.h \
          debug/disasm.h debug/memdump.h

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(LDFLAGS) -o $@ $(OBJS)

%.o: %.c $(HEADERS)
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	rm -f $(OBJS) $(TARGET)

# ----------------------------------------------------------------
# WebAssembly target  (requires Emscripten: source emsdk/emsdk_env.sh)
# ----------------------------------------------------------------
EMCC = emcc
WASM_CFLAGS = -Wall -Wextra -O2 -std=c11 -I.

# Same sources as native, minus:
#   main.c            -> replaced by wasm/main_wasm.c
#   tty/tty_stdio.c   -> replaced by tty/tty_wasm.c
#   tty/tty_tcp.c     -> not applicable in browser
#   tty/tty_pty.c     -> not applicable in browser
WASM_SRCS = $(filter-out main.c kd11ea/clock.c tty/tty_stdio.c tty/tty_tcp.c tty/tty_pty.c, $(SRCS)) \
            tty/tty_wasm.c wasm/main_wasm.c

WASM_EXPORTS = '["_main",\
"_wasm_rx_push","_wasm_tx_poll",\
"_wasm_set_odt","_wasm_load_rk_image","_wasm_load_rl_image","_wasm_load_lda","_wasm_load_asc",\
"_wasm_reset","_wasm_power_on","_wasm_power_off",\
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
"_wasm_probe_sel_is_1bit","_wasm_probe_read_ns"]'

wasm/ll-34.js: $(WASM_SRCS) $(HEADERS)
	$(EMCC) $(WASM_CFLAGS) -o wasm/ll-34.js $(WASM_SRCS) \
	    -s EXPORTED_FUNCTIONS=$(WASM_EXPORTS) \
	    -s EXPORTED_RUNTIME_METHODS='["ccall","cwrap","FS","HEAPU16"]' \
	    -s ALLOW_MEMORY_GROWTH=1 \
	    -s INITIAL_MEMORY=67108864

wasm: wasm/ll-34.js

clean-wasm:
	rm -f wasm/ll-34.js wasm/ll-34.wasm

.PHONY: all clean wasm clean-wasm
