# KD11-EA ROMs

These two folders contain the ROMs from the two CPU boards.

These ROMs are old, fragile, and require specific voltages not supported
by my TL-866+. Rather than desoldering and handling them unnecessarily, their
contents were reconstructed from logic analyzer captures and schematic analysis.

## M8266 control board

## Microcode (MPC)

| ROM  | Micro-word bits | Function                                     |
| ---- | --------------- | -------------------------------------------- |
| E97  | 24–27           | B, BX, OVX and DBE control                   |
| E98  | 28–31           | SSMUX and AMUX control                       |
| E99  | 20–23           | ALU function control                         |
| E100 | 32–35           | Micro-branch selection (`BUT`)               |
| E103 | 40–43           | Mode and service control                     |
| E104 | 36–39           | Scratchpad source and destination selection  |
| E105 | 44–47           | Scratchpad address                           |
| E106 | 16–19           | Bus-address, cycle and auxiliary ALU control |
| E107 | 12–15           | Bus transfer and maintenance control         |
| E108 | 8–11            | Next MPC bit 0 and miscellaneous control     |
| E109 | 4–7             | Next MPC bits 1–4                            |
| E110 | 0–3             | Next MPC bits 5–8                            |

### Combinatorial ROMs

| ROM  | Function                                                 |
| ---- | -------------------------------------------------------- |
| E29  | UNIBUS interrupt priority                                |
| E51  | Service dispatch and trap-vector selection               |
| E52  | Service and trap priority encoder                        |
| E53  | Instruction trap and HALT decoder                        |
| E54  | RESET and trace-trap control                             |
| E59  | Single-operand instruction decode, low part              |
| E60  | Single-operand instruction decode, high part             |
| E61  | Single-operand ALU control                               |
| E62  | Rotate and shift control                                 |
| E63  | Operate, RTS and condition-code instruction decode       |
| E68  | Instruction categorization for condition-code generation |
| E69  | Double-operand instruction decode, low part              |
| E70  | Double-operand instruction decode, high part             |
| E71  | Conditional branch decode                                |
| E74  | Extended Instruction Set decode                          |
| E80  | Byte swapping, sign extension and byte-write control     |
| E82  | Primary microcode ALU-function expansion                 |
| E83  | Double-operand ALU control                               |
| E87  | B/BX shift-register mode expansion                       |
| E102 | Micro-branch (`BUT`) expansion                           |

## M8265 data-path board

| ROM    | Function                                     |
| ------ | -------------------------------------------- |
| E74–75 | General-register address decoding            |
| E76    | PAR/PDR read and multiplexer control         |
| E77    | PAR/PDR write control                        |
| E83    | MMU abort and SR0 source selection           |
| E86    | Console access to the general registers      |
| E107   | Carry and overflow condition-code generation |

**Notes:**

- E74 and E75 share the same content, their input selects a different half in each of them.
- Address wiring, active levels and output decoding are documented in `../ucode_rom.h`, `../ucode_labels.h` and `../rom_wiring.h`.

Transcribing the ROM wiring by hand is boring, tedious and error-prone. For this reason, `rom_wiring.h` was generated with an LLM from pinouts traced from the schematics, then validated with truth-table tests.
