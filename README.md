# ll/34

ll/34 is a circuit-level emulator for the PDP-11/34A (1976), running a virtual CPU reverse-engineered from the schematics, microcode, and logic captures of an actual PDP-11/34A.

It was originally designed as a digital replica of the real CPU to assist with troubleshooting at the signal level.
Circuit-level (ROM truth tables + combinatorial logic) was chosen over gate-level (Verilog) because it is low-level enough to reproduce hardware bugs, yet fast enough to run programs.

## Architecture

The source tree is organized as follows:

| Directory    | Description                                                                                                                         |
| ------------ | ----------------------------------------------------------------------------------------------------------------------------------- |
| `pdp11/`     | The machine: CPU, UNIBUS, peripherals, boot ROM, programmer console model, logic analyzer probe. `machine.c` assembles and runs it. |
| `frontends/` | User interfaces: `cli/` (terminal), `wasm/` (browser GUI), `tty/` (host side of the serial line).                                   |
| `demos/`     | Disk images and programs.                                                                                                           |
| `docs/`      | DEC documentation.                                                                                                                  |

The virtual CPU (`pdp11/kd11ea/`) replicates the clock generator, logic gates and ALU (simulated in C logic), stepping through the original microcode dumped from the ROMs.
It is composed of the following components:

| File           | Description                                                                                                                |
| -------------- | -------------------------------------------------------------------------------------------------------------------------- |
| `roms/`        | ROM dumps (combinational and microcode).                                                                                   |
| `kd11ea.c`     | CPU core. Steps through the microcode one micro-word per cycle. The 74xx gates and 74S181 bit-slice ALUs are modeled here. |
| `rom_wiring.h` | Address wiring and output decoding for the combinatorial ROMs.                                                             |
| `ucode_rom.h`  | Documents and assembles the CPU microcode into 512×48-bit micro-words.                                                     |
| `clockgen.c`   | Clock generator. Models the original delay line pacing the CPU.                                                            |
| `mmu.c`        | Memory management. Kernel/user address spaces, and the 16-bit to 18-bit memory translation.                                |
| `int.c`        | Interrupt arbiter. Implements the priority queue and approximates the UNIBUS daisy-chain grant order.                      |
| `clock.c`      | Internal emulator clock. Keeps simulated time in sync with wall-clock time.                                                |

## Emulated devices

The following devices have been implemented. As the Unibus is asynchronous, not all devices need to be timing or signal accurate.

The Programmer Console, in particular, uses a high-level emulation of its behavior rather than a low-level reconstruction of the Intel 8008 and wiring from its board.

- UNIBUS backplane (timing and signal accurate)
- KD11-EA CPU
- M9301 boot card with original boot and diagnostic ROMs
- DL11 serial card (timing accurate)
- KW11 line clock (50/60 Hz)
- Programmer Console (including maintenance mode for CPU and MPC troubleshooting)
- RK05 drives (RK11 controller)
- RL01 and RL02 drives (RL11 controller)
- Tape reader
- VT100 terminal with stdio, TCP port or PTY modes

Devices are plugged into the UNIBUS backplane (unibus.c), which performs address decoding, timing, and bus cycles.

## Programmer Console (Ctrl-P)

The Programmer Console emulates the front panel switches and indicators of the PDP-11/34A.
It allows initializing the bus, booting the CPU, single-stepping through code, examining and depositing data at arbitrary addresses, and quitting the emulator.

The console automatically spawns whenever the CPU is halted (power-up, HALT instruction, or Ctrl-P).

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

<figure>
<p align="center">
<img width="1650" height="532" alt="ll-34's debug console" src="https://github.com/user-attachments/assets/302479c6-9e11-4456-8ae8-42353e626938" />
<figcaption>
</p>   
<p align="center">
<i>Debug console in the Web Assembly GUI.</i></figcaption>
</p>
</figcaption>
</figure>

## Logic Analyzer (Ctrl-L)

Troubleshooting ll-34 during its development turned out to be so similar to troubleshooting the real hardware that an internal logic analyzer was implemented to trace the signals and probe the datapath. This proved instrumental in tracking down subtle ROM and timing bugs in the virtual CPU.

The logic analyzer allows the probing of 107 signals, including 72 from the DEC schematics, 31 composite signals (registers, buses and flags), and 4 timing values from the emulator. Future versions will allow the capture of most signals from the schematics.

Sample rate is the real-time CPU clock (5,555,556 Hz) with a resolution of 180 ns per sample. A configurable divider reduces the sample rate for longer capture windows.

Captures use a ring buffer (up to 64K samples) with configurable trigger on any signal, adjustable trigger position (pre/post ratio), and CSV export for offline analysis.

<figure>
<p align="center">
<img src="https://github.com/user-attachments/assets/5d07fc3d-dbe9-4cfe-9ef9-567232176dae" width="50%">
<figcaption>
</p>   
<p align="center">
<i>ll-34’s logic analyzer (bottom) can be used to troubleshoot the real hardware (top), just as the real hardware was used to troubleshoot ll-34.</i></figcaption>
</p>
</figcaption>
<img width="1699" height="598" alt="ll-34's logic analyzer" src="https://github.com/user-attachments/assets/f4ed3018-5c5a-47db-a2de-bb2f127fffd1" />
</figure>

## Sample Programs

ll-34 comes with a few programs and systems to try: a Game of Life, V6 UNIX, RT-11 V4 with the original Tetris game, and [ATTN/11](https://github.com/dbrll/ATTN-11/), a small trainable Transformer with self-attention.

### RT-11

```
./ll-34 --rk ./demos/rtv4_rk.dsk
```

The boot ROM will automatically load from the RK11 and start the operating
system. `DIR` will list files, `HELP` will print help, `RUN TETRIS`
will start the Tetris demo (although the `$TERM` will probably be wrong?).
`RUN ADVENT` will start the [Colossal Cave Adventure](https://trmm.net/Advent/),
built on a real PDP-11/34.

### v6 UNIX

```
./ll-34 --rk ./demos/v6bin.rk
```

The bootloader will print a minimal `@` prompt. Type `rkunix` and
hit enters and a few seconds later you should get a `login:` prompt.
Enter the username `root` with no password and you're in!
One note is that `cd` is named `chdir` in early UNIX.

## Building

`make` + a C11 compiler, there are no other dependencies.

Verified to compile with no warnings on Linux (x86_64 and aarch64) with both musl and glibc, macOS aarch64, and NetBSD 10 aarch64.

`make wasm` builds the WebAssembly version (requires Emscripten). `frontends/wasm/serve.sh` serves it on http://localhost:1134, and `frontends/wasm/deploy-pages.sh` publishes it on the `gh-pages` branch.

## Running

Besides the CLI, a standalone WebAssembly version with a photo-realistic GUI is available here: https://dbrll.github.io/ll-34.

**Note**: ll-34 is a resource-intensive emulator. Unlike instruction-level emulators, it steps through the microcode one cycle at a time and runs the ALU, combinatorial ROMs, scratchpad, clock generator, and bus timing at each step.
The host must sustain the 5.5 MHz clock generator pace continuously to remain cycle accurate.

At startup, the emulator will benchmark itself and report its speed relative to a real KD11-EA. A ratio below 1x means the host cannot keep up and timing accuracy is not guaranteed. The benchmark is not available in the WebAssembly version, where browser timer resolution makes it unreliable. As the WASM build is slower than native, running it on a smartphone will typically be too slow.
