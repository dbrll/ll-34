# ll/34

**ll/34 is a circuit-level emulator for the PDP-11/34A (1976), running a virtual CPU reverse-engineered from the schematics, microcode, and behavior of an actual PDP-11/34A.**

It was originally designed as a digital replica of the real CPU to assist with troubleshooting at the signal level.
The virtual KD11-EA CPU essentially consists of a C translation of the schematics, and lookups to the ROM tables. All combinational ROMs are reverse-engineered, and the clock generator is precisely modeled.

Circuit-level (ROM truth tables + combinatorial logic) was chosen other gate-level (Verilog) because it is low-level enough to reproduce hardware bugs, yet fast enough to run programs.


## Emulated devices

- UNIBUS backplane (timing and signal accurate)
- KD11-EA CPU (circuit-level)
- M9301 boot card with original boot and diagnostic ROMs
- DL11 serial card (timing accurate)
- KW11 line clock (50/60 Hz)
- Programmer Console (including maintenance mode for CPU troubleshooting)
- RK05 drives (high-level emulation of the RK11 controller)
- RL01 and RL02 drives (high-level emulation of the RL11 controller. NB: the M9301-YF has no bootstrap for RL11 drives, a separate bootstrap is provided)
- Tape reader
- VT100 terminal with stdio, TCP port or PTY modes

## Architecture

All devices are plugged into the UNIBUS backplane (unibus.c), which performs address decoding, timing, and bus cycles.

Internally, the CPU is composed of the following components:

| File           | Description                                                                                                                                                                                                                                                                                   |
| -------------- | --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- |
| `ucode_rom.h`  | 512x48-bit microcode store (bipolar PROMs), based on [Bitsavers](https://bitsavers.org/pdf/dec/pdp11/1134/) Verilog and dumps. Each micro-word controls the ALU, scratchpad, bus interface, and branching for one micro-cycle.                                                                |
| `combo_roms.h` | Combinational ROMs (E51-E83, E107), IR decode (E59/E60/E63/E69/E70/E71/E74), ALU function select (E61/E82/E83), trap/service priority (E52/E53), and condition code logic (E68/E107), all reconstituted from the schematics and logic captures. Open-collector ROMs wire-OR onto the MPC bus. |
| `kd11ea.c`     | Datapath engine. Steps through the microcode one micro-word per cycle. Basic 74xx gates are simulated using C logic operators (`!`, `&&`, `>>` etc.). The 74S181 bit-slice ALU is fully modeled (4 slices, 16-bit).                                                                           |
| `mmu.c`        | Memory management. Kernel/user address spaces, 18-bit relocation (3x 74S283 adders), 16 PAR/PDR pairs, SR0/SR2 abort registers with freeze logic.                                                                                                                                             |
| `clockgen.c`   | Clock generator. Models the E106 delay line (TAP 30/90/120 feedback, TRAN INH bus wait). Short cycles: 180 ns, long cycles: 240 ns, bus transfers stretch until SSYN returns.                                                                                                                 |
| `clock.c`      | Real-time pacing. Keeps simulated time in sync with wall-clock time.                                                                                                                                                                                                                          |
| `int.c`        | Interrupt arbiter. BR4-BR7 priority queue, approximates the UNIBUS daisy-chain grant order.                                                                                                                                                                                                   |


## Programmer Console (Ctrl-P)

Emulates the front panel switches and indicators of the PDP-11/34A. Active when the CPU is halted (power-up, HALT instruction, or Ctrl-P).

It allows initializing the bus, booting the CPU, single-stepping through code, examining and depositing data at arbitrary addresses, and quitting the emulator.

The frontend is decoupled from the engine, so a photorealistic front panel GUI is also provided in WebAssembly.

## Debug Console (Ctrl-E)

The Debug Console provides an interactive debugger for both microcode-level and instruction-level code:

- `s [n]`: single-step n micro-instructions with full state dump (MPC, ALU, bus, registers)
- `b addr`: set PC breakpoint (octal), `bl` to list, `bd n` to delete
- `g`: resume execution
- `u [addr] [n]`: disassemble instructions
- `c [addr] [n]`: memory dump (octal)
- `mmu`: show MMU state (kernel + user PAR/PDR, current mode marked)
- `r`: register dump

## Logic Analyzer (Ctrl-L)

Troubleshooting ll-34 during its development turned out to be so similar to troubleshooting the real hardware that an internal logic analyzer was implemented to trace the signals and probe the datapath. This proved instrumental in tracking down subtle ROM and timing bugs in the virtual CPU. It can also be used as a reference to troubleshoot the real hardware.

The logic analyzer allows the probing of 102 points on major CPU signals, mapped to physical chip pins (KD1:Exx:pin notation matching the DEC schematics K1-5 through K2-9). Logical aliases (MPC, ALU_OUT, IR, PSW…) are provided for convenience.

Sample rate is the real-time CPU clock (5,555,556 Hz) with a resolution of 180 ns per sample. A configurable divider reduces the sample rate for longer capture windows.

Captures use a ring buffer (up to 64K samples) with configurable trigger on any signal, adjustable trigger position (pre/post ratio), and CSV export for offline analysis.

<figure>
<p align="center">
<img src="https://github.com/user-attachments/assets/5d07fc3d-dbe9-4cfe-9ef9-567232176dae" width="50%">
<figcaption>
</p>   
<p align="center">
<i>Logic captures were used liberally throughout the project to map and debug poorly understood multiplexing paths.</i></figcaption>
</p>   
</figure>


## Sample Programs

ll-34 comes with a few programs and systems to try: a Game of Life, V6 UNIX, RT-11 V3, and a small trainable Transformer with self-attention.

## Building

`make` + a C11 compiler, there are no other dependencies.

The result is a standalone binary of approximately 90 KB. Tested on Linux (x86_64 and aarch64) with both musl and glibc, macOS aarch64, and NetBSD 10 aarch64.

A standalone WebAssembly version with a photo-realistic GUI is also available here: https://dbrll.github.io/ll-34
