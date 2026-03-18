# Schematic Index — MP00190 Print Set

Source: MP00190, 1134A Field Maintenance Print Set. Drawing numbers D-CS-M8265-0-1 (Data Path) and D-CS-M8266-0-1 (Control).

PDF file: `KD11-CPU/MP00190_1134A_KD11-EA-newocr.pdf` (49 pages).

## PDF Page Map

### Front Matter (Pages 1–15)

| PDF Page | Content                                                                          |
| -------- | -------------------------------------------------------------------------------- |
| 1        | Table of Contents / Drawing Directory (1134A)                                    |
| 2        | Shipping List / Documentation                                                    |
| 3–15     | Other modules: M9391 Unibus Terminator, Bootstrap (M9312), KY11-LB Console, etc. |

### M8265 Data Path Module (Pages 16–29)

Drawing: D-CS-M8265-0-1, 10 sheets (K1-1 through K1-10).

| PDF Page | Sheet | Title                                     | Key Components / Function                                                                                                                                                               |
| -------- | ----- | ----------------------------------------- | --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- |
| 16       | —     | M8265 Drawing Directory                   | B-DD-M8265-0-0. Design data base, next higher assembly.                                                                                                                                 |
| 17       | —     | M8265 Assembly Drawing                    | Component placement (image only, no useful OCR).                                                                                                                                        |
| 18       | —     | M8265 Etch/Fabrication                    | Board artwork reference.                                                                                                                                                                |
| 19       | —     | M8265 Parts List                          | D-UA-M8265-0-0. Insertion parts list with IC part numbers.                                                                                                                              |
| 20       | K1-1  | Data Path (K1-1) — Bits 03:00             | 4-bit ALU slice (E1 74S181), carry gen (E13 74S182 shared), scratchpad slice (E19 8x5x68), B REG / BX REG slice, BMUX, AMUX, SSMUX, PSW MUX (E96), PSW bits C and V, OUTPUT STORE.      |
| 21       | K1-2  | Data Path (K1-2) — Bits 07:04             | 4-bit ALU slice (E2 74S181), scratchpad slice (E20 8x5x68), B REG / BX REG slice, BMUX, AMUX, SSMUX. Signal K1-2 4-7=0 H for CC Z.                                                      |
| 22       | K1-3  | Data Path (K1-3) — Bits 11:08             | 4-bit ALU slice (E3 74S181), carry gen (E14 74S182 shared), scratchpad slice (E21 8x5x68), B REG / BX REG slice, BMUX, AMUX, SSMUX. Signal K1-3 8-11=0 H.                               |
| 23       | K1-4  | Data Path (K1-4) — Bits 15:12             | 4-bit ALU slice (E4 74S181), scratchpad slice (E22 8x5x68), B REG / BX REG slice, BMUX, AMUX (E90 quad 2-to-1 MUX for bits 13/14 SSMUX reroute), SSMUX, PSW bits N and Z, SP 15 signal. |
| 24       | K1-5  | Data Path (K1-5) — Processor Clock        | Processor clock generation (PROC CLK L), timing taps (TAP 30 H), REG CLK H, LOAD BAR L, ALLOW MSYN H. Bus control timing.                                                               |
| 25       | K1-6  | Data Path (K1-6) — Bus Address            | Virtual Bus Address register (VBA 00:17), Physical Bus Address register (BA 06:17), VBA 00 for odd address detection. 8641 bus drivers for UNIBUS address lines.                        |
| 26       | K1-7  | Data Path (K1-7) — Page Address Registers | Memory management PAR (Page Address Registers). Three scratchpad memories (E76, E77, E78). PAR/PDR Address MUX (E89). PAR output to KT MUX.                                             |
| 27       | K1-8  | Data Path (K1-8) — Page Descriptor / SR0  | Memory management PDR (Page Descriptor Registers), SR0 (Status Register 0), page length comparator (E60, E61), KT FAULT L, error flags (NR, PL, RO), RELOCATE H.                        |
| 28       | K1-9  | Data Path (K1-9) — KT MUX / SR2           | KT MUX (tri-state multiplexer channeling PAR data onto scratchpad output lines), SR2 (Status Register 2, loaded with 16-bit virtual address).                                           |
| 29       | K1-10 | Data Path (K1-10) — Condition Code Logic  | BYTE MUX (E106), C and V Decode ROM (E105), SHIFT MUX (E117), CC Z H (combined zero detect), CC V H, CC C H, AMUX S0 H, ENAB GR L, PAR & PDR LOW L.                                     |

### Flow Diagrams (Pages 30–31)

| PDF Page | Sheet | Title                                                                                                          |
| -------- | ----- | -------------------------------------------------------------------------------------------------------------- |
| 30       | —     | KD11-EA Flow Diagrams (D-FD-KD11-EA), sheet 1. Microcode flow: trap service, bus errors, stack overflow, halt. |
| 31       | —     | KD11-EA Flow Diagrams, sheet 2 (or design data page).                                                          |

### M8266 Control Module (Pages 32–49)

Drawing: D-CS-M8266-0-1, 10 sheets (K2-1 through K2-10).

| PDF Page | Sheet | Title                                     | Key Components / Function                                                                                                                                                                                                                            |
| -------- | ----- | ----------------------------------------- | ---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- |
| 32       | —     | M8266 Drawing Directory                   | B-DD-M8266-0-0.                                                                                                                                                                                                                                      |
| 33       | —     | M8266 Assembly Drawing                    | Component placement.                                                                                                                                                                                                                                 |
| 34       | —     | M8266 Rework Instructions                 | ECO rework information.                                                                                                                                                                                                                              |
| 35       | —     | M8266 Parts List (sheet 1/5)              | D-UA-M8266-0-0. Insertion parts list — standard ICs.                                                                                                                                                                                                 |
| 36       | —     | M8266 Parts List (sheet 2/5)              | IC part numbers (74193, 7408, 74123, 74S60, 74S153, etc.)                                                                                                                                                                                            |
| 37       | —     | M8266 Parts List (sheet 3/5)              | IC part numbers (74S174, 74175, 7427, 74S175, 8837, etc.)                                                                                                                                                                                            |
| 38       | —     | M8266 Parts List (sheet 4/5)              | ROM part numbers (82S131 256×4, 82S137 512×4, 82S123 32×8, etc.)                                                                                                                                                                                     |
| 39       | —     | M8266 Parts List (sheet 5/5)              | ROM part numbers (82S181 1024×4, 82S137 512×4, 82S123 32×8, etc.)                                                                                                                                                                                    |
| 40       | K2-1  | Control (K2-1) — Bus Control / DAT TRAN   | DAT TRAN logic, MSYN/SSYN control, BBSY, BUS INUSE, DATIP, START TRAN H, UBUS code, CACHE AIT, parity (BUS PA L, BUS PB L), INT MSYN.                                                                                                                |
| 41       | K2-2  | Control (K2-2) — Priority Arbitration     | BR priority arbitration (BR4–BR7), BG grant logic, BG INH, SACK, NPG, NO SACK timeout, PROC INIT, Start Reset flip-flop (E54), 100 ms INIT one-shot.                                                                                                 |
| 42       | K2-3  | Control (K2-3) — Service / Trap Logic     | Power fail (PFAIL, AC LO, DC LO), auto restart, service ROMs (E50, E51), trap vector generation, stack overflow detection, double bus error (DBE), START POWN, USER MODE H.                                                                          |
| 43       | K2-4  | Control (K2-4) — Scratchpad Address MUX   | SPAM (Scratchpad Address MUX, E38 dual 74S153), SPA select lines (SPA 00–SPA 03), SPA SRC SEL / SPA DST SEL, DISABLE MSYN+1 L, FORCE KERNEL H, SET BEL, odd address detect MUX (E38), DISABLE BIT 0.                                                 |
| 44       | K2-5  | Control (K2-5) — IR and Auxiliary Control | Instruction Register (E55, E65, E66 74174 + E33 74S74), Categorizing ROM (E67), BYTE decode (BYTE L, BYTE H), AUX DOP ROM (E81), AUX SOP ROM (E60), ROT/SHFT ROM (E61), SERIAL SHIFT H, CC CODE, LOAD IR H, IR bit outputs.                          |
| 45       | K2-6  | Control (K2-6) — Instruction Decode       | DOP Decoder ROMs (E68, E69, E73), SOP Decoder ROMs (E59, E58), Branch Decoder ROM (E71), Operate Decoders (E62, E53, E52), EIS Decoder, DEST L, IR DECODE H/L. Source/destination mode field gating (74H01 gates).                                   |
| 46       | K2-7  | Control (K2-7) — Control Store (part 1)   | Micro-PC (MPC 00–08), MPC latches, LOAD IR L, branch logic, micro-branch control. MPC address lines to ROM array.                                                                                                                                    |
| 47       | K2-8  | Control (K2-8) — Control Store (part 2)   | Control Store ROMs (portion), output register latches. ALU control fields (ALU S0–S3, ALU MODE, ALU CIN), B LEG fields, BX MODE, B MODE, SHIFT MUX, AUX CONTROL, ENAB OVX, FUNC CODE, BUF DAT TRAN, odd address ROM (E78), DISABLE MSYN, ENAB MAINT. |
| 48       | K2-9  | Control (K2-9) — Control Store (part 3)   | Control Store ROMs (portion), output register latches. SSMUX select, AMUX S0/S1 select, SPA DST SEL, SPA SRC SEL, FORCE RS+1, SS fields.                                                                                                             |
| 49       | K2-10 | Control (K2-10) — Control Store (part 4)  | Control Store ROMs (portion), output register latches. ROM SPA select lines, COUNT fields, EIS counter logic.                                                                                                                                        |

## Drawing Numbers

| Drawing Number | Description                                  | Module                           |
| -------------- | -------------------------------------------- | -------------------------------- |
| D-CS-M8265-0-1 | KD11E-A Data Path circuit schematics         | M8265 (10 sheets, K1-1 to K1-10) |
| D-CS-M8266-0-1 | KD11E-A Control circuit schematics           | M8266 (10 sheets, K2-1 to K2-10) |
| D-UA-M8265-0-0 | KD11E-A Data Path unit assembly / parts list | M8265                            |
| D-UA-M8266-0-0 | KD11E-A Control unit assembly / parts list   | M8266                            |
| B-DD-M8265-0-0 | KD11E-A Data Path drawing directory          | M8265                            |
| B-DD-M8266-0-0 | KD11E-A Control drawing directory            | M8266                            |
| D-FD-KD11-EA   | KD11E-A Flow Diagrams (microcode flow)       | —                                |

## Key IC Cross-Reference by Sheet

### M8265 Data Path (K1-1 through K1-10)

| IC                 | Type                     | Function                           | Sheet                                            |
| ------------------ | ------------------------ | ---------------------------------- | ------------------------------------------------ |
| E1, E2, E3, E4     | 74S181                   | 4-bit ALU slices                   | K1-1, K1-2, K1-3, K1-4                           |
| E13, E14           | 74S182                   | Look-Ahead Carry Generators        | K1-1 (shared K1-1/K1-2), K1-3 (shared K1-3/K1-4) |
| E19, E20, E21, E22 | 8x5x68 (=85S68)          | Scratchpad memory (16×4 tri-state) | K1-1, K1-2, K1-3, K1-4                           |
| E90                | 74S153 (quad 2-to-1 MUX) | SSMUX reroute bits 13/14           | K1-4                                             |
| E96                | —                        | PSW MUX                            | K1-1                                             |
| E105               | 82S131 (256×4 OC)        | C and V Decode ROM                 | K1-10                                            |
| E106               | —                        | BYTE MUX                           | K1-10                                            |
| E117               | —                        | SHIFT MUX                          | K1-10                                            |
| E76, E77, E78      | —                        | PAR scratchpad memories            | K1-7                                             |
| E89                | —                        | PAR/PDR Address MUX                | K1-7                                             |
| E60, E61 (on K1-8) | —                        | Page length comparator network     | K1-8                                             |

### M8266 Control (K2-1 through K2-10)

| IC            | Type               | Function                                           | Sheet              |
| ------------- | ------------------ | -------------------------------------------------- | ------------------ |
| E31           | —                  | DAT TRAN flip-flop                                 | K2-1               |
| E54           | —                  | Start Reset flip-flop                              | K2-2               |
| E50, E51      | 82S131 (256×4 OC)  | Service / Trap ROMs                                | K2-3               |
| E38           | 74S153             | Scratchpad Address MUX / odd address detect        | K2-4               |
| E55, E65, E66 | 74174              | Instruction Register (6-bit D-type, ×3 = 18 bits)  | K2-5               |
| E33           | 74S74              | IR flip-flop (additional bit)                      | K2-5               |
| E67           | 82S123 (32×8 OC)   | Categorizing ROM                                   | K2-5               |
| E81           | 82S131 (256×4 OC)  | AUX DOP ROM                                        | K2-5               |
| E60 (on K2-5) | 82S131 (256×4 OC)  | AUX SOP ROM                                        | K2-5               |
| E61 (on K2-5) | 82S131 (256×4 OC)  | ROT/SHFT ROM                                       | K2-5               |
| E68, E69      | 82S137 (512×4 OC)  | DOP Decoder ROMs                                   | K2-6               |
| E73           | 82S137 (512×4 OC)  | DOP Decoder ROM / EIS Decoder                      | K2-6               |
| E59, E58      | 82S137 (512×4 OC)  | SOP Decoder ROMs                                   | K2-6               |
| E71           | 82S131 (256×4 OC)  | Branch Decoder ROM                                 | K2-6               |
| E62, E53, E52 | 82S131 (256×4 OC)  | Operate Decoder ROMs                               | K2-6               |
| E78 (on K2-8) | 82S123 (32×8 OC)   | Odd Address Detect ROM                             | K2-8               |
| E97–E108      | 82S181 (1024×4 TS) | Control Store ROMs (12 × 512×4 = 48-bit microword) | K2-7 through K2-10 |
| E109, E110    | 82S181 (1024×4 TS) | Control Store Expansion ROMs                       | K2-7 through K2-10 |

## Notes

- Each Data Path sheet K1-1 through K1-4 contains one complete 4-bit slice of the data path: ALU, scratchpad, B REG, BX REG, BMUX, AMUX, and SSMUX for that 4-bit range.
- K1-5 through K1-10 contain support functions: clock/timing (K1-5), bus address (K1-6), memory management (K1-7 through K1-9), and condition code logic (K1-10).
- The Control Store spans four sheets (K2-7 through K2-10) because 12 ROMs × 4 output bits = 48 control bits, plus MPC logic, latches, and branching circuitry.
- DEC signal naming convention: `<sheet> <signal name> (<qualifier>) <level>`. Example: `K2-8 ALU S3 H` = sheet K2-8, signal ALU S3, active High.
- IC reference designators (E-numbers) are unique within each module but may overlap between M8265 and M8266. Always specify the module or sheet when referencing an IC.

## M8265 PART LIST (visual check)

E1 8641
E2 74S157
E3 74S253
E4 74S253
E5 74S253
E6 74153
E7 74S181
E8 85S68
E9 74153
E10 74194
E11 74194
E12 8641
E13 74S153
E14 74S253
E15 74S253
E16 74S04
E17 8815
E18 74S181
E19 85S68
E20 74S157
E21 74194
E22 74194
E23 58641
E24 74S157
E25 74S253
E26 74S253
E27 74S182
E28 74S181
E29 85S68
E30 74S157
E31 74194
E32 74194
E33 8641
E34 74S153
E35 74S153
E36 74S253
E37 8815
E38 74S181
E39 85S68
E40 74157
E41 74194
E42 58641
E43 8641
E44 74298
E45 7483
E46 74S253
E47 74S174
E48 74S174
E49 74s174
E50 74174
E51 74174
E52 74194
E53 8641
E54 7430
E55 74298
E56 7483
E57 74LS253
E58 74LS253
E59 74LS253
E60 74LS253
E61 7485
E62 74174
E63 8641
E64 8641
E65 74298
E66 7483
E67 74LS253
E68 74LS253
E69 74LS253
E70 74LS253
E71 74S157
E72 7485
E73 74174
E74 MMI 23-169A2-02
E75 82S216
E76 82S126
E77 MMI 23-167A2-02
E78 85S68
E79 85S68
E80 85S68
E81 85S68
E82 74175
E83 MMI 23-165A2-02
E84 74S37
E85 74S37
E86 MMI 23-166A2-02
E87 7430
E88 85S68
E89 85S68
E90 85S68
E91 74S157
E92 74157
E93 74157
E94 74S10
E95 74S00
E96 74S157
E97 74S75
E98 74S157
E99 74175
E100 74S74
E101 7402
E102 7402
E103 74175
E104 delay line 150ns (clock generator)
E105 74S04
E106 74S08
E107 74S287 (visu) 23-164A2 (schematics)
E108 74S157
E109 74S74
E110 7400
E111 74H01
E112 7408
E113 74S00
E114 7402
E115 7486
E116 7437
E117 7404
E118 74S00
E119 74153
E120 74132
E121 74S11
E122 7432
E123 7410

## M8266 PART LIST (

Part Chips

---

7474 E5,E12,E31,E32,E34
7400 E3,E15,E18,E28,E86
7473 E24,E45
7402 E22,E23,E43
8881 E8,E77
8815 E4,E30
74S03 E41,E64,E65,E94
74193 E78,E79
74S138 E89
7437 E81
7408 E11,E50
74123 E16,E7,E14
74S00 E25,E37,E47,E84,E6,E35
74S04 E111,E112
74S20 E57,E58
74S153 E46,E36
S157 E10,E39
74S174 E19,E38,E66,E71,E88,E90,E92,E96,E56
74175 E13
7427 E75
74S175 E48,E55,E67
8837 E1,E17
7414 E21
8640 E9
8641 E27
74132 E2,E42
74S02 E20,E73,E95
74S37 E26
74S74 E33,E49
74S10 E40,E85
74S32 E44
ROM 32x8 E51
ROM 256x4 E28,E53,E54,E59,E60,E61,E62,E63,E68,E69,E72,E80
ROM 512x4 E70
ROM 32x8 E74,E82,E83,E87,E102
ROM 1024x4 E97,E98,E99,E100,E103,E104,E105,E106,E107,E109,E110,W1
ROM 1025x4 E108
