# Conformance

This directory documents the verification of ll-34 against my own PDP-11/34A. The tests cover the reconstructed decode and service ROMs, the original microcode, the arithmetic and logic unit (ALU), condition codes, memory management, interrupts, and exceptions.

Verification uses two complementary methods:

- Captures on my PDP-11 with a DSLogic Pro16 logic analyzer, mostly at the end of a signal chain (MPC lines, ALU control, condition codes), so that one capture checks every ROM upstream. The original captures are in the `captures` folder, while the `ll34-probes` folder contains the probe configuration for ll-34, to compare against the hardware reference.
- Test programs and diagnostics run on both systems for direct comparisons of results. The tests range from individual instructions to complex cases that involve memory, exceptions, and interrupts. These measurements also helped calibrate the emulator's timing.

Run the conformance tests with `./run.sh`. These tests require Golang and MACRO11.

## Conformance summary

| Area                                             | Validation                                                     | Result                                                                 |
| ------------------------------------------------ | -------------------------------------------------------------- | ---------------------------------------------------------------------- |
| Microcode sequencing, decode ROMs, E102          | `captures/mpc`: MPC lines and E38 latch, 506 399 micro-cycles  | identical, cycle by cycle                                              |
| ALU control (E82, E87, E61/E83 chain)            | `captures/datapath`: E82 and E87 outputs, 506 527 micro-cycles | identical, cycle by cycle                                              |
| Condition codes (E68, E107)                      | `captures/e107`: E107 inputs and outputs, 34 488 events        | 101 E107 cells measured, ll-34 replay identical                        |
| Trap decode (E52, E53, E54, E80)                 | `captures/traps`                                               | cells measured and used in the images: E52 17, E53 236, E54 75, E80 35 |
| Instruction set, every opcode 000000-167777      | `programs/confrm`                                              | 28/28 blocks                                                           |
| Single operand instructions, odd registers (E62) | `programs/confrm_e62`                                          | 4/4 blocks                                                             |
| Memory management                                | `programs/confrm_mmu`, `confrm_mmu_detail`                     | 16/16 blocks, 1021/1021 tests                                          |
| Traps and interrupts (E29, E51, E52)             | `programs/cfint`                                               | 16/16 blocks, 3 passes                                                 |
| DEC diagnostics FKAAC0, FKABD1, FKACA0, FKTHB0   | `diagnostics`                                                  | pass on both                                                           |
| Timing                                           | `timing`, `captures/mpc`                                       | within 0.2 % over the capture, 1.4 % mean on timing                    |
