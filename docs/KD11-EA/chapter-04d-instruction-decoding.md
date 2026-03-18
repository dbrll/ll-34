# Instruction Decoding

Source: EK-KD11E-TM-001, Chapter 4, Sections 4.5–4.6

## 4.5.1 General Description

Two methods are used to control instruction decoding, one using microroutine
selection and the other using auxiliary ALU control. Dual control is required
because of the large number of instructions that require source/destination
calculations. Auxiliary ALU control is evoked whenever the microcode executes the
action X = Y OP B as a result of a specific instruction.

The following facts pertain to the KD11-E/PDP-11 instruction set:

1. In general, the PDP-11 operation code is variable from 4 to 16 bits.
2. A number of instructions require two address calculations; an even larger
   number require only one address calculation. There are also a number of
   instructions that require address calculations, but do not operate on data.
3. All op codes that are not implemented in the KD11-E processor must be trapped.
4. There are illegal combinations of instructions and address modes that must be
   trapped.
5. There exists a list of exceptions in the execution of instructions having to
   do with both the treatment of data and the setting of condition codes in the
   processor status word.

## 4.5.2 Instruction Register

Each PDP-11 instruction obtained from memory is stored in the 16-bit instruction
register (IR). This register consists of three 6-bit D-type 74174 registers
(E55, E65, and E66 on K2-5) and one 74S74 D-type flip-flop (E33). The purpose
of the IR is to store the instruction for the complete instruction cycle so that
the IR Decode and Auxiliary ALU Control circuits can decode the correct control
signals throughout the instruction cycle.

The IR latches data from the SSMUX 00-15 lines on K2-7 LOAD IR L and the leading
edge of K1-5 PROC CLK L.

On the trailing edge of K2-9 BUT SERVICE (1) H, all the IR bits except K2-5 IR15
(1) H are cleared. [K2-5 IR15 (1) H is set by the same signal transition.] This
means that the IR Decode circuit will see a conditional branch instruction in the
IR after every service microstep. This action prevents the processor from
decoding a HLT instruction after an Initialize condition.

If a bus error (BE) occurs while the Control Store output signal Enable Double
Bus Error (K2-8 ENAB DBE L) is asserted, the whole IR is cleared (PDP-11 Halt),
causing the processor to halt automatically. Bus errors occurring without the
K2-8 ENAB DBE L signal have no effect on the IR. K2-8 ENAB DBE L is only
asserted during certain microwords in the trap sequence to prevent the
possibility of a second bus error occurring (Double Bus Error), which would cause
the trap sequence to be re-entered before it is completed. For example, if R6
(Stack Pointer) were an odd address, the first bus reference using the stack in
the trap routine would cause another trap (Odd Address), a sequence that could
tie up the CPU indefinitely if not for the Halt and Double Bus Error facilities.
In short, any bus error during the four memory references of the trap sequence is
fatal.

## 4.5.3 Instruction Decoder

### 4.5.3.1 Instruction Decoder Circuitry

The Instruction Decode (prints K2-5 and K2-6) and Control Store (prints K2-7
through K2-10) circuitry could be thought of as an internal microprocessor that
interprets PDP-11 instructions and translates them into a set of
microinstructions, each consisting of 40 control signals. These control signals
then determine the operation of the data path and Unibus control circuitry.

A block diagram of this internal microprocessor is shown in Figure 4-1. Note that
all outputs of the Control Store ROMs (K2-7 through K2-10) are latched in hex
D-type registers (74174s).

Nine of these latched signals (K2-7 MPC 08 H through K2-7 MPC 00 H) are fed back
to the inputs of the Control Store ROM as the next microinstruction address (and
can then be called the micro-PC). The wired-OR capability of these lines allows
the IR Decode circuitry to force microbranching addresses on certain enabling
conditions. The actual microbranch address will depend on the instruction being
decoded, the instruction mode used (modes 0-7), and the operand required (source
or destination).

The IR Decode circuitry is shown on prints K2-5 and K2-6. It consists of one
512 x 4 ROM, ten 256 x 4 ROMs, and two 32 x 8 ROMs, and 74H01, 7402, 7400, and
7410 logic gates.

### 4.5.3.2 Double-Operand Instructions

Double-operand instructions require two address calculations, one for the source
and one for the destination operand. The microbranch to the sequence of
microinstructions that determine the source operand is initiated by the Control
Store output signal K2-6 IR DECODE (1) H. When this signal is enabled, the IR
Decode ROMs DOP Decode (E68 and E69 on print K2-6) check the instruction in the
IR (op code bits IR15-12). If the instruction is a double-operand type, the ROM
outputs are asserted as follows:

| Type Instruction | K2-6 IR Code 00 L | K2-7 MPC07 L | K2-7 MPC06 L | K2-7 MPC05 L | K2-7 MPC04 L | K2-7 MPC03 L |
|-----------------|-------------------|-------------|-------------|-------------|-------------|-------------|
| MOV (SM0\*DM0) | | | 0 | 0 | 0 | |
| DOP (MOV+SUB) MOD (SM0\*DM0) (ADD, BIC, BIS) | | 0 | | | 0 | 0 |
| SUB (SM0\*DM0) | | | 0 | | 0 | 0 |
| DOP (SM0\*DM0) | | 0 | | 0 | 0 | |
| Illegal Instructions | 0 | 0 | 0 | 0 | 0 | 0 |
| DOP NONMOD (SM0\*DM0) (CMP, BIT) | | | 0 | | | |

NOTE: Ground on the MPC lines represents a logic "1."

Coupled with the microprocessor outputs of the DOP DEC ROM are the outputs of a
set of type 74H01 gates on K2-6. These gates, when enabled, place the contents
of the source mode field (IR11:09) of the PDP-11 instruction being decoded onto
the MPC 00:02 lines. These gates are enabled by the K2-6 SRCH ROM output only
when the instruction being decoded is of the double-operand type, the K2-6 IR
DECODE (1) L signal is asserted, and the instruction is not reserved (K2-6 IR
CODE 00 L unasserted).

Source microaddresses summary:

| Instruction | Source Mode | Octal Microbranch Address |
|-------------|-------------|--------------------------|
| DOP (SM0\*DM0) | 0 | 110 |
| | 1 | 111 |
| | 2 | 112 |
| | 3 | 113 |
| | 4 | 114 |
| | 5 | 115 |
| | 6 | 116 |
| | 7 | 117 |
| Reserved DOP | | 00 |

NOTE: A ground on the MPC lines represents a logic 1.

The DOP DEC ROMs described above are also used to decode the microprocessor
address for the various Control Store destination operand routines. When the K2-7
BUT DEST L input is asserted by the miscellaneous control field circuitry of the
Control Store, the DOP DEC ROMs decode the instructions, determine whether it is
a modifying or nonmodifying instruction, and generate the following micro-PC
addresses.

| Type Instruction | K2-7 MPC07 L | K2-7 MPC06 L | K2-7 MPC05 L | K2-7 MPC04 L | K2-7 MPC03 L |
|-----------------|-------------|-------------|-------------|-------------|-------------|
| Move (SM0\*DM0) | 0 | 0 | | 0 | |
| Modify (ADD BIS BIC but not MOV or SUB) | 0 | 0 | | | |
| Nonmodify (CMP BIT) | 0 | 0 | | | 0 |
| SUB | 0 | 0 | 1 | 0 | 0 |

The circuitry used to decode the destination mode field of the instruction being
decoded is similar to that described above for microaddressing the source
operand routine. A set of 74H01 gates on K2-6 is used to place the contents of
K2-5 IR 05 (1) H through K2-5 IR 03 (1) H on the lines when enabled. For
double-operand instructions, enabling occurs when the MPC miscellaneous control
field asserts K2-7 BUT DEST L.

ROM E73 on print K2-6 is also considered to be part of the DOP Decoder circuitry.
This ROM decodes all Extended Instruction Set (EIS) instructions, generating the
following micro-PC addresses when K2-6 IR DECODE (1) H is asserted:

| Type Instruction | K2-6 IR Code 00 L | K2-7 MPC07 L | K2-7 MPC06 L | K2-7 MPC05 L | K2-7 MPC04 L | K2-7 MPC03 L |
|-----------------|-------------------|-------------|-------------|-------------|-------------|-------------|
| Multiply or Divide (MUL, DIV) | | | 0 | 0 | | 0 |
| Arithmetic Shift or Arithmetic Shift Combined (ASH, ASHC) | | | 0 | 0 | | |
| SOP | | | 0 | | | 0 |
| XOR | | 0 | | 0 | 0 | |
| Reserved | 0 | 0 | 0 | 0 | 0 | 0 |

The K2-6 DEST L output of the EIS Decoder ROM (E73) allows the 74H01 (E64) on
print K2-6 to place the contents of the destination mode field of the instruction
being decoded onto the micro-PC (MPC00-MPC02) lines. This microbranching
technique is similar to that described above for microaddressing the source
operand routine. Use of the EIS instructions does not degrade processor timing or
affect NPR latency.

### 4.5.3.3 Single-Operand Instructions

Unlike double-operand instructions, single-operand instructions only require one
address calculation to obtain the necessary operand. Complete SOP instruction
decoding is done with the two 256 x 4-bit ROMs (E59 and E58).

The SOP Microbranch ROM (E59) monitors the necessary IR input lines and asserts
the correct micro-PC address on lines K2-7 MPC03 L through K2-7 MPC 06 L when
the K2-6 IR DECODE (1) L signal is asserted and the SOP enable signal K2-5
IR 12-14=0 H is true. The K2-6 DEST L output is also activated when an SOP
instruction is decoded. This signal enables the destination mode monitoring
circuitry described in the double-operand instruction decoding section.

Microaddresses for SOP instructions:

| Instruction | Base Microbranch Address |
|-------------|------------------------|
| SOP Modify (CLR, COM, INC, DEC) | 040 |
| SOP Non-Modify (TST) | 160 |
| NEG | 150 |
| Rotate and Shift | 170 |
| JSR | 150 |
| JMP | 020 |
| MARK | 030 |
| SWAB | 030 |
| MFPI(D) | 100 |
| MTPI(D) | 250 |
| MFPS | 130 |
| MTPS | 120 |

The SOP Microbranch ROM (E59) is also used to decode JSR instructions. This
decoding is performed in the same manner as that for SOP instructions. The K2-6
DMO H input to the ROM is used to detect the illegal instruction JMP or JSR
destination mode 0. When this occurs, no micro-PC address is allowed on the ROM
outputs.

The SOP Decode ROM (E58) monitors the same input signals as the SOP Microbranch
ROM. Its purpose, however, is to decode illegal, reserved, and trap instructions.
The three output signals K2-6 IR CODE 00 L through K2-6 IR CODE 02 L are enabled
as follows:

| Instructions | IR Code 02 | IR Code 01 | IR Code 00 |
|-------------|-----------|-----------|-----------|
| Reserved | 1 | 1 | 0 |
| Illegal (JMP or JSR Mode 0) | 1 | 0 | 1 |
| EMT | 0 | 1 | 0 |
| Trap | 0 | 0 | 1 |

The fourth output signal of the SOP Decode ROM enables the destination mode
monitoring circuitry described in the double-operand instruction decoding section.

### 4.5.3.4 Branch Instructions

Conditional branch instructions are completely decoded by the Branch DEC ROM (E71
on print K2-6). This ROM is enabled when bits IR11:IR14 are all low and the K2-6
IR DECODE (1) L signal is active. The input lines monitored are the four
condition code bits (N, Z, V, and C) and four IR bits (IR15, 10, 9, and 8). When
a branch is decoded, the K2-7 MPC 07 L output signal is enabled. The branch
instruction microcode routine in the Control Store will sign-extend the branch
offset and shift it left one place.

### 4.5.3.5 Operate Instructions

There are three 256 x 4-bit ROMs in the instruction-decoding circuitry for
decoding PDP-11 operate instructions. These ROMs are the Reset/Trap Decode, Trap
Decode, and Op Branch ROMs (E62), all found on K2-6.

**Op Branch ROM (E62).** Monitors IR output lines IR00:IR07. It is enabled when
IR08 and IR15 are low and K2-6 IR DECODE (1) L is active. The PDP-11 operate
instructions are decoded into the following micro-PC addresses on the ROM outputs
K2-7 MPC 00 L through K2-7 MPC 03 L.

| Instruction | Microbranch Address |
|-------------|-------------------|
| Reset | 003 |
| RTI/RTT | 011 |
| Set Condition Codes | 007 |
| Clear Condition Codes | 006 |
| RTS | 004 |
| Wait | 014 |

**Reset/Trap Decode ROM (E53).** Decodes Reset, RTT, and RTI instructions and
activates the outputs K2-6 START RESET H and K2-6 ENAB TBIT H accordingly. This
ROM also allows the lower PSW bits (K2-6 DISABLE LOAD PSW H) to be loaded only
from the stack when the processor is operating in User mode (memory management
restriction). It also treats a Reset instruction as a Nop in User mode.

**TRAP DEC ROM (E52).** Has the same inputs as the Op Branch ROM. Its purpose is
to decode Halt, reserved, trap, and illegal instructions, and to enable the
outputs accordingly. The K2-3 USER MODE H input also allows this ROM to treat
Halt instructions as reserved instructions when operating in the memory
management User mode.

| Instruction | IR Code 02 | IR Code 01 | IR Code 00 |
|-------------|-----------|-----------|-----------|
| Reserved | | 1 | 0 |
| Illegal | | 0 | 1 |
| BPT | | 0 | 0 |
| IOT | 0 | 1 | 1 |
| HALT | Enable HLT RQST L | | |

---

## 4.6 Auxiliary ALU Control

The AUX Control circuitry on the KD11-E consists of three bipolar ROMs, shown on
K2-5.

| ROM | Name |
|-----|------|
| 32 x 8-bit | DOP (E81) |
| 256 x 4-bit | SOP (E60) |
| 256 x 4-bit | ROT/SHIFT (E61) |

These ROMs determine the ALU operation to be performed whenever the microcode
executes the action X <- Y OP B, where Y designates a scratchpad register and X
designates either the B REG or a scratchpad register.

### AUX DOP ROM (E81)

Decodes double-operand instructions, and is enabled by K2-8 AUX SETUP H. The
following table expresses the outputs of this ROM as a function of the
instruction being performed. (B represents the B register, A represents any
scratchpad register, and F represents the ALU output.)

| Instruction | ALU Operation | Func Code 03 | Func Code 02 | Func Code 01 | Func Code 00 |
|-------------|--------------|--------------|--------------|--------------|--------------|
| MOV(B) | F <- A | 0 | 1 | 0 | 1 |
| CMP(B) | F <- A minus B | 0 | 1 | 0 | 0 |
| ADD | F <- A plus B | 1 | 0 | 0 | 0 |
| SUB | F <- A minus B | 0 | 1 | 0 | 0 |
| BIT(B) | F <- A AND B | 1 | 0 | 0 | 1 |
| BIC(B) | F <- A AND NOT B | 1 | 0 | 1 | 0 |
| BIS(B) | F <- A OR B | 1 | 0 | 1 | 1 |
| XOR | F <- A XOR B | 1 | 1 | 0 | 0 |

### AUX SOP ROM (E60)

Decodes single-operand instructions, and is enabled by K2-8 AUX SETUP H. The
following table expresses the ROM outputs as a function of the SOP instruction
decoded.

| Instruction | ALU Function | Func Code 03 | Func Code 02 | Func Code 01 | Func Code 00 |
|-------------|-------------|--------------|--------------|--------------|--------------|
| SWAB | F <- A | 0 | 1 | 0 | 1 |
| CLR(B) | F <- ZERO | 0 | 0 | 0 | 0 |
| COM(B) | F <- NOT A | 0 | 0 | 0 | 1 |
| INC(B) | F <- A plus 1 | 0 | 0 | 1 | 0 |
| DEC(B) | F <- A minus 1 | 0 | 0 | 1 | 1 |
| NEG(B) | F <- A minus B | 0 | 1 | 0 | 0 |
| ADC(B) CBIT=0 | F <- A | 0 | | 0 | |
| ADC(B) CBIT=1 | F <- A plus | 0 | 0 | | 0 |
| SBC(B) CBIT=0 | F <- A | 0 | | 0 | |
| SBC(B) CBIT=1 | F <- A minus | 0 | 0 | 1 | 1 |
| TST(B) | F <- A | 0 | 1 | 0 | 1 |
| ROR(B) | F <- B | 0 | 1 | 1 | 0 |
| ROL(B) | F <- B | 0 | 1 | 1 | 0 |
| ASR(B) | F <- B | 0 | 1 | 1 | 0 |
| ASL(B) | F <- B | 0 | 1 | 1 | 0 |
| MARK | N/A | 0 | 0 | 0 | 0 |
| MFPI | F <- A | 0 | 1 | 0 | 1 |
| MTPI | F <- A | 0 | 1 | 0 | 1 |
| SXT NBIT=0 | F <- | 0 | 0 | 0 | 0 |
| SXT NBIT=1 | F <- | 0 | 1 | 1 | 0 |
| MTPS | F <- A | 0 | 1 | 0 | 1 |
| MFPD | F <- A | 0 | 1 | 0 | 1 |
| MTPD | F <- A | 0 | 1 | 0 | 1 |
| MFPS | F <- A | 0 | 1 | 0 | 1 |

### ROT/SHFT ROM (E61)

Auxiliary control signals are also necessary for performing rotate and shift
operations. The ROT/SHFT ROM (E61) on K2-5 decodes these instructions and
outputs those control signals required to shift the contents of the B REG. Inputs
K1-1 BREG 00 (1) H, K1-10 CC N H, and K1-1 CBIT (1) H also determine the K2-5
SERIAL SHIFT H and K2-5 ROT CBIT (1) H signals. The SERIAL SHIFT H signal is
sent to the BYTE MUX (E106 on K1-10), where it is used in determining the K1-10
SHIFT IN 07 H signal used in the B REG shifting operation. K2-5 ROT CBIT (1) H
is used in the calculation of the new carry condition (C and V Bit ROM - E105 on
K1-10). Note that for all rotate and shift operations, the AUX SETUP is performed
on the B <- B step before each X <- Y OP B step previously mentioned. This is
done to allow the condition codes to be set up without slowing the processor.

### Table 4-9 Auxiliary Control for Binary and Unary Instructions

| Instruction | N and Z | V | C | ALU Function | CIN |
|-------------|---------|---|---|-------------|-----|
| MOV(B) | Load | Cleared | Not affected | A Logical | 0 |
| CMP(B) | Load | Load like Subtract | Load like Subtract | A minus B | 0 |
| BIT(B) | Load | Cleared | Not affected | A AND B Logical | 0 |
| BIC(B) | Load | Cleared | Not affected | NOT A AND B Logical | 0 |
| BIS(B) | Load | Cleared | Not affected | A OR B Logical | 0 |
| ADD | Load | Set if operands are same sign and result different | Set if carry out | A plus B | 0 |
| SUB | Load | Set if there was arithmetic overflow as a result of the operation (i.e., if operands were of opposite signs and the sign of the source was the same as the sign of the result; cleared otherwise | Set if carry | A minus B | 0 |
| XOR | Load | Cleared | Not affected | A XOR B | 0 |
| CLR(B) | Load | Cleared (like Add) | Clear | 0 | 0 |
| COM(B) | Load | Cleared | Set | NOT A | 0 |
| INC(B) | Load | Set if destination held 100000 before operand | Not affected | A plus 1 | +1 |
| DEC(B) | Load | Set if result is 100000 | Not affected | A minus 1 | 1 |
| NEG(B) | Load | Set if result is 100000 | Cleared if result is 0; set otherwise | A minus B | 0 |
| ADC(B) | Load | Set if destination was 077777 and C = 1 | Set if destination was 177777 and C = 1 | A plus CBIT | 0 |
| SBC(B) | Load | Set if destination was 100000 | Set if destination was 0 and C = 1; cleared otherwise | A minus CBIT | 0 |
| TST(B) | Load | Cleared | Cleared | A Logical | 0 |
| ROR(B) | Z <- 1 if (15:00)\*C=0; N <- C | Unaffected | (0) | B Logical | 0 |
| ROL(B) | Z <- 1 if (14:00)\*C=0; N <- (14) | Unaffected | (15); B(7) | B Logical | 0 |
| ASR(B) | Z <- 1 if (15:01)=0; N <- N | Unaffected | 0 <- (15) | B Logical | 0 |
| ASL(B) | Z <- 1 if (14:01)=0; N <- (14) | | C <- (15) | B Logical | 0 |
| SWAB | Load | Cleared | Cleared | A Logical | 0 |
| SXT | Z-Load; N-Unaffected | Cleared | Cleared | 1 | 0 |
| MFPI | Load | Cleared | Unaffected | A Logical | 0 |
| MTPI | Load | Cleared | Unaffected | A Logical | 0 |
| MTPS | Z-Set if SRC(7)=0; N-Set if SRC(7)=1 | Cleared | Unaffected | A Logical | 0 |
| MFPD | Load | Cleared | Unaffected | A Logical | 0 |
| MTPD | Load | Cleared | Unaffected | A Logical | 0 |
| MFPS | Z-Set if PS(7)=0; N-Set if PS(7)=1 | Cleared | Unaffected | A Logical | 0 |
