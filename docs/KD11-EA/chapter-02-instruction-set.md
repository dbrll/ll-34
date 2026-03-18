# Chapter 2 — Instruction Set

Source: EK-KD11E-TM-001, KD11-E Central Processor Maintenance Manual, December 1976, Chapter 2.

## 2.1 Introduction

The KD11-E is defined by its instruction set. The sequences of processor operations are selected according to the instruction decoding. The following describes the PDP-11/34 instructions and instruction set addressing modes along with instruction set differences from those of the KD11-A, KD11-B, and KD11-D.

## 2.2 Addressing Modes

Data stored in memory must be accessed and manipulated. Data handling is specified by a PDP-11/34 instruction (MOV, ADD, etc.), which usually indicates:

1. The function (operation code)
2. A general-purpose register to be used when locating the source operand and/or locating the destination operand
3. An addressing mode (to specify how the selected register(s) is to be used)

Because a large portion of the data handled by a computer is usually structured (in character strings, in arrays, in lists, etc.), the PDP-11/34 has been designed to handle structured data efficiently and flexibly. The general registers may be used with an instruction in any of the following ways:

1. **As accumulators.** The data to be manipulated resides within the register.
2. **As pointers.** The contents of the register are the address of the operand, rather than the operand itself.
3. **As pointers, which automatically step through core locations.** Automatically stepping forward through consecutive core locations is known as autoincrement addressing; automatically stepping backward is known as autodecrement addressing. These modes are particularly useful for processing tabular data.
4. **As index registers.** In this instance the contents of the register and the word following the instruction are summed to produce the address of the operand. This allows easy access to variable entries in a list.

PDP-11/34s also have instruction addressing mode combinations that facilitate temporary data storage structures for convenient handling of data which must be frequently accessed. This is known as the "stack."

In the PDP-11/34, any register can be used as a "stack pointer" under program control; however, certain instructions associated with subroutine linkage and interrupt service automatically use Register 6 as a "hardware stack pointer." For this reason, R6 is frequently referred to as the "SP."

R7 is used by the processor as its program counter (PC).

Two types of instructions utilize the addressing modes: single-operand and double-operand. Figure 2-1 shows the formats of these two types of instructions. The addressing modes are listed in Table 2-1.

![Addressing Mode Instruction Formats](../cpu/figures/fig2-1_instruction_formats.png)

### Table 2-1 — Addressing Modes

#### Direct Modes

| Mode | Binary Code | Name | Assembler Syntax | Function |
|------|-------------|------|------------------|----------|
| 0 | 000 | Register | Rn | Register contains operand. |
| 2 | 010 | Autoincrement | (Rn)+ | Register contains address of operand. Register contents incremented after reference. |
| 4 | 100 | Autodecrement | -(Rn) | Register contents decremented before reference. Register contains address of operand. |
| 6 | 110 | Index | X(Rn) | Value X (stored in a word following the instruction) is added to (Rn) to produce address of operand. Neither X nor (Rn) is modified. |

#### Deferred Modes

| Mode | Binary Code | Name | Assembler Syntax | Function |
|------|-------------|------|------------------|----------|
| 1 | 001 | Register Deferred | @Rn or (Rn) | Register contains the address of the operand. |
| 3 | 011 | Autoincrement Deferred | @(Rn)+ | Register is first used as a pointer to a word containing the address of the operand, then incremented (always by two, even for byte instructions). |
| 5 | 101 | Autodecrement Deferred | @-(Rn) | Register is decremented (always by two, even for byte instructions) and then used as a pointer to a word containing the address of the operand. |
| 7 | 111 | Index Deferred | @X(Rn) | Value X (stored in the memory word following the instruction) and (Rn) are added and the sum is used as a pointer to a word containing the address of the operand. Neither X nor (Rn) is modified. |

#### PC Addressing Modes

| Mode | Binary Code | Name | Assembler Syntax | Function |
|------|-------------|------|------------------|----------|
| 2 | 010 | Immediate | #n | Operand follows instruction. |
| 3 | 011 | Absolute | @#A | Absolute address follows instruction. |
| 6 | 110 | Relative | A | Address of A, relative to the instruction, follows the instruction. |
| 7 | 111 | Relative Deferred | @A | Address of location containing address of A, relative to the instruction, follows the instruction. |

Note: Rn = Register; X, n, A = next program counter (PC) word (constant).

## 2.3 PDP-11/34 Instructions

The PDP-11/34 instructions can be divided into five groups:

1. Single-Operand Instructions (shifts, multiple precision instructions, rotations)
2. Double-Operand Instructions (arithmetic and logical instructions)
3. Program Control Instructions (branches, subroutines, traps)
4. Operate Group Instructions (processor control operations)
5. Condition Code Operators (processor status word bit instructions)

Tables 2-2 through 2-6 list each instruction, including byte instructions for the respective instruction groups. Figure 2-2 shows the six different instruction formats of the instruction set, and the individual instructions in each format.

![PDP-11 Instruction Formats](../cpu/figures/fig2-2_pdp11_instruction_formats.png)

### Table 2-2 — Single Operand Instructions

| Mnemonic | Op Code | Operation | Condition Codes | Description |
|----------|---------|-----------|-----------------|-------------|
| CLR / CLRB | 0050DD / 1050DD | (dst) ← 0 | N: cleared; Z: set; V: cleared; C: cleared | Contents of specified destination are replaced with zeroes. |
| COM / COMB | 0051DD / 1051DD | (dst) ← ~(dst) | N: set if MSB of result is 0; Z: set if result is 0; V: cleared; C: set | Replaces the contents of the destination address by their logical complement (each bit equal to 0 set and each bit equal to 1 cleared). |
| INC / INCB | 0052DD / 1052DD | (dst) ← (dst) + 1 | N: set if result < 0; Z: set if result is 0; V: set if (dst) was 077777; C: not affected | Add 1 to the contents of the destination. |
| DEC / DECB | 0053DD / 1053DD | (dst) ← (dst) - 1 | N: set if result < 0; Z: set if result is 0; V: set if (dst) was 100000; C: not affected | Subtract 1 from the contents of the destination. |
| NEG / NEGB | 0054DD / 1054DD | (dst) ← -(dst) | N: set if result < 0; Z: set if result is 0; V: set if result is 100000; C: cleared if result is 0 | Replaces the contents of the destination address by its 2's complement. Note that 100000 is replaced by itself. |
| ADC / ADCB | 0055DD / 1055DD | (dst) ← (dst) + C | N: set if result < 0; Z: set if result is 0; V: set if (dst) is 077777 and C is 1; C: set if (dst) is 177777 and C is 1 | Adds the contents of the C-bit into the destination. This permits the carry from the addition of the low-order words/bytes to be carried into the high-order results. |
| SBC / SBCB | 0056DD / 1056DD | (dst) ← (dst) - C | N: set if result < 0; Z: set if result is 0; V: set if (dst) was 100000; C: cleared if (dst) is 0 and C is 1 | Subtracts the contents of the C-bit from the destination. This permits the carry from the subtraction of the low-order words/bytes to be subtracted from the high-order part of the result. |
| TST / TSTB | 0057DD / 1057DD | (dst) ← (dst) | N: set if result < 0; Z: set if result is 0; V: cleared; C: cleared | Sets the condition codes N and Z according to the contents of the destination address. |
| ROR / RORB | 0060DD / 1060DD | (dst) ← (dst) rotate right one place | N: set if high-order bit of result is set; Z: set if all bits of result are 0; V: loaded with exclusive-OR of N-bit and C-bit as set by ROR; C: loaded from low-order bit of destination | Rotates all bits of the destination right one place. The low-order bit is loaded into the C-bit and the previous contents of the C-bit are loaded into the high-order bit of the destination. |
| ROL / ROLB | 0061DD / 1061DD | (dst) ← (dst) rotate left one place | N: set if high-order bit of result word is set (result < 0); cleared otherwise; Z: set if all bits of result word = 0; cleared otherwise; V: loaded with exclusive-OR of N-bit and C-bit (as set by the completion of the rotate operation); C: loaded with high-order bit of the destination | Rotate all bits of the destination left one place. The high-order bit is loaded into the C-bit of the status word and the previous contents of the C-bit are loaded into the low-order bit of the destination. |
| ASR / ASRB | 0062DD / 1062DD | (dst) ← (dst) shifted one place to the right | N: set if high-order bit of result is set (result < 0); cleared otherwise; Z: set if result = 0; cleared otherwise; V: loaded from exclusive-OR of N-bit and C-bit (as set by the completion of the shift operation); C: loaded from low-order bit of the destination | Shifts all bits of the destination right one place. The high-order bit is replicated. The C-bit is loaded from the low-order bit of the destination. ASR performs signed division of the destination by two. |
| ASL / ASLB | 0063DD / 1063DD | (dst) ← (dst) shifted one place to the left | N: set if high-order bit of result set (result < 0); cleared otherwise; Z: set if result = 0; cleared otherwise; V: loaded with exclusive-OR of N-bit and C-bit (as set by the completion of the shift operation); C: loaded with high-order bit of the destination | Shifts all bits of the destination left one place. The low-order bit is loaded with a 0. The C-bit of the status word is loaded from the high-order bit of the destination. ASL performs a signed multiplication of the destination by 2 with overflow indication. |
| ASH | 072RSS | R ← R shifted arithmetically NN places to right or left, where NN = (src) | N: set if result < 0; cleared otherwise; Z: set if result = 0; cleared otherwise; V: set if sign of register changed during shift; cleared otherwise; C: loaded from last bit shifted out of register | The contents of the register are shifted right or left the number of times specified by the source operand. The shift count is taken as the low-order 6 bits of the source operand. This number ranges from -32 to +31. Negative is a right shift and positive is a left shift. |
| ASHC | 073RSS | R, Rv1 ← R, Rv1 shifted NN places to the right or left, where NN = (src) | N: set if result < 0; cleared otherwise; Z: set if result = 0; cleared otherwise; V: set if sign bit changes during the shift; cleared otherwise; C: loaded with high-order bit when right shift (loaded with the last bit shifted out of the 32-bit operand) | The contents of the register and the register ORed with one are treated as one 32-bit word. Rv1 (bits 0-15) and R (bits 16-31) are shifted right or left the number of times specified by the shift count. The shift count is taken as the low-order 6 bits of the source operand. This number ranges from -32 to +31. Negative is a right shift and positive is a left shift. When the register chosen is an odd number, the register and the register ORed with one are the same. In this case, the right shift becomes a rotate. The 16-bit word is rotated right the number of bits specified by the shift count. |
| SXT | 0067DD | (dst) ← 0 if N bit is clear; (dst) ← -1 if N bit is set | N: unaffected; Z: set if N bit clear; V: cleared; C: unaffected | If the condition code bit N is set then a -1 is placed in the destination operand; if N bit is clear, then a 0 is placed in the destination operand. This instruction is particularly useful in multiple precision arithmetic because it permits the sign to be extended through multiple words. |
| SWAB | 0003DD | Byte 1/Byte 0 ↔ Byte 0/Byte 1 | N: set if high-order bit of low-order byte (bit 7) of result is set; cleared otherwise; Z: set if low-order byte of result = 0; cleared otherwise; V: cleared; C: cleared | Exchanges high-order byte and low-order byte of the destination word (destination must be a word address). |

### Table 2-3 — Double Operand Instructions

| Mnemonic | Op Code | Operation | Condition Codes | Description |
|----------|---------|-----------|-----------------|-------------|
| MOV / MOVB | 01SSDD / 11SSDD | (dst) ← (src) | N: set if (src) < 0; cleared otherwise; Z: set if (src) = 0; cleared otherwise; V: cleared; C: not affected | Word: Moves the source operand to the destination location. The previous contents of the destination are lost. The source operand is not affected. Byte: Same as MOV. The MOVB to a register (unique among byte instructions) extends the most significant bit of the low-order byte (sign extension). Otherwise MOVB operates on bytes exactly as MOV operates on words. |
| CMP / CMPB | 02SSDD / 12SSDD | (src) - (dst) [in detail, (src) + ~(dst) + 1] | N: set if result < 0; cleared otherwise; Z: set if result = 0; cleared otherwise; V: set if there was arithmetic overflow (i.e., operands were of opposite signs and the sign of the destination was the same as the sign of the result); cleared otherwise; C: cleared if there was a carry from the MSB of the result; set otherwise | Compares the source and destination operands and sets the condition codes which may then be used for arithmetic and logical conditional branches. Both operands are unaffected. The only action is to set the condition codes. The compare is customarily followed by a conditional branch instruction. Note that unlike the subtract instruction the order of operation is (src) - (dst), not (dst) - (src). |
| BIT / BITB | 03SSDD / 13SSDD | (src) ∧ (dst) | N: set if high-order bit of result set; cleared otherwise; Z: set if result = 0; cleared otherwise; V: cleared; C: not affected | Performs logical AND comparison of the source and destination operands and modifies condition codes accordingly. Neither the source nor destination operands are affected. The BIT instruction may be used to test whether any of the corresponding bits that are set in the destination are clear in the source. |
| BIC / BICB | 04SSDD / 14SSDD | (dst) ← ~(src) ∧ (dst) | N: set if high-order bit of result set; cleared otherwise; Z: set if result = 0; cleared otherwise; V: cleared; C: not affected | Clears each bit in the destination that corresponds to a set bit in the source. The original contents of the destination are lost. The contents of the source are unaffected. |
| BIS / BISB | 05SSDD / 15SSDD | (dst) ← (src) ∨ (dst) | N: set if high-order bit of result set; cleared otherwise; Z: set if result = 0; cleared otherwise; V: cleared; C: not affected | Performs inclusive-OR operation between the source and destination operands and leaves the result at the destination address; i.e., corresponding bits set in the destination. The contents of the destination are lost. |
| ADD | 06SSDD | (dst) ← (src) + (dst) | N: set if result < 0; cleared otherwise; Z: set if result = 0; cleared otherwise; V: set if there was arithmetic overflow as a result of the operation (that is, both operands were of the same sign and the result was of the opposite sign); cleared otherwise; C: set if there was a carry from the MSB of the result; cleared otherwise | Adds the source operand to the destination operand and stores the result at the destination address. The original contents of the destination are lost. The contents of the source are not affected. Two's complement addition is performed. |
| SUB | 16SSDD | (dst) ← (dst) - (src) [in detail, (dst) + ~(src) + 1] | N: set if result < 0; cleared otherwise; Z: set if result = 0; cleared otherwise; V: set if there was arithmetic overflow as a result of the operation (i.e., if operands were of opposite signs and the sign of the source was the same as the sign of the result); cleared otherwise; C: cleared if there was a carry from the MSB of the result; set otherwise | Subtracts the source operand from the destination operand and leaves the result at the destination address. The original contents of the destination are lost. The contents of the source are not affected. In double precision arithmetic, the C-bit, when set, indicates a borrow. |
| MUL | 070RSS | R, Rv1 ← R × (src) | N: set if product is < 0; cleared otherwise; Z: set if product is 0; cleared otherwise; V: cleared; C: set if the result is less than -2^15 or greater than or equal to 2^15 - 1 | The contents of the destination register and source taken as two's complement integers are multiplied and stored in the destination register and the succeeding register (if R is even). If R is odd, only the low-order product is stored. Assembler syntax is: MUL S,R. (Note that the actual destination is R, Rv1 which reduces to just R when R is odd.) |
| DIV | 071RSS | R, Rv1 ← R, Rv1 / (src) | N: set if quotient < 0; cleared otherwise; Z: set if quotient = 0; cleared otherwise; V: set if source = 0 or if the absolute value of the register is larger than the absolute value of the source (in this case the instruction is aborted because the quotient would exceed 15 bits); C: set if divide 0 attempted; cleared otherwise | The 32-bit two's complement integer in R and Rv1 is divided by the source operand. The quotient is left in R; the remainder is of the same sign as the dividend. R must be even. |
| XOR | 074RDD | (dst) ← R ⊕ (dst) | N: set if result < 0; cleared otherwise; Z: set if result = 0; cleared otherwise; V: cleared; C: unaffected | The exclusive OR of the register and destination operand is stored in the destination address. Contents of register are unaffected. Assembler format is XOR R,D. |

### Table 2-4 — Program Control Instructions

#### Branch Instructions

| Mnemonic | Op Code | Branch Condition | Description |
|----------|---------|------------------|-------------|
| BR | 000400+offset | Unconditional | Provides a way of transferring program control within a range of -128 to +127 words with a one word instruction. It is an unconditional branch. |
| BNE | 001000+offset | Z = 0 | Tests the state of the Z-bit and causes a branch if the Z-bit is clear. BNE is the complementary operation to BEQ. It is used to test inequality following a CMP, to test that some bits set in the destination were also in the source following a BIT, and generally, to test that the result of the previous operation was not 0. |
| BEQ | 001400+offset | Z = 1 | Tests the state of the Z-bit and causes a branch if Z is set. Used to test equality following a CMP operation, to test that no bits set in the destination were also set in the source following a BIT operation, and generally, to test that the result of the previous operation was 0. |
| BGE | 002000+offset | N ⊕ V = 0 | Causes a branch if N and V are either both clear or both set. BGE is the complementary operation to BLT. Thus, BGE always causes a branch when it follows an operation that caused addition of two positive numbers. BGE also causes a branch on a 0 result. |
| BLT | 002400+offset | N ⊕ V = 1 | Causes a branch if the exclusive-OR of the N- and V-bits are 1. Thus, BLT always branches following an operation that added two negative numbers, even if overflow occurred. In particular, BLT always causes a branch if it follows a CMP instruction operating on a negative source and a positive destination (even if overflow occurred). Further, BLT never causes a branch when it follows a CMP instruction operating on a positive source and negative destination. BLT does not cause a branch if the result of the previous operation was 0 (without overflow). |
| BGT | 003000+offset | Z ∨ (N ⊕ V) = 0 | Operation of BGT is similar to BGE, except BGT does not cause a branch on a 0 result. |
| BLE | 003400+offset | Z ∨ (N ⊕ V) = 1 | Operation is similar to BLT, but in addition will cause a branch if the result of the previous operation was 0. |
| BPL | 100000+offset | N = 0 | Tests the state of the N-bit and causes a branch if N is clear. BPL is the complementary operation of BMI. |
| BMI | 100400+offset | N = 1 | Tests the state of the N-bit and causes a branch if N is set. It is used to test the sign (most significant bit) of the result of the previous operation. |
| BHI | 101000+offset | C ∨ Z = 0 | Causes a branch if the previous operation causes neither a carry nor a 0 result. This will happen in comparison (CMP) operations as long as the source has a higher unsigned value than the destination. |
| BLOS | 101400+offset | C ∨ Z = 1 | Causes a branch if the previous operation caused either a carry or a 0 result. BLOS is the complementary operation to BHI. The branch occurs in comparison operations as long as the source is equal to or has a lower unsigned value than the destination. |
| BVC | 102000+offset | V = 0 | Tests the state of the V-bit and causes a branch if the V-bit is clear. BVC is the complementary operation to BVS. |
| BVS | 102400+offset | V = 1 | Tests the state of the V-bit (overflow) and causes a branch if the V-bit is set. BVS is used to detect arithmetic overflow in the previous operation. |
| BCC / BHIS | 103000+offset | C = 0 | Tests the state of the C-bit and causes a branch if C is clear. BCC is the complementary operation to BCS. |
| BCS / BLO | 103400+offset | C = 1 | Tests the state of the C-bit and causes a branch if C is set. It is used to test for a carry in the result of a previous operation. |

All branch instructions: condition codes unaffected. Branch operation: PC ← PC + (2 × offset).

#### Jump and Subroutine Instructions

| Mnemonic | Op Code | Operation | Condition Codes | Description |
|----------|---------|-----------|-----------------|-------------|
| JMP | 0001DD | PC ← (dst) | Unaffected | JMP provides more flexible program branching than provided with the branch instruction. Control may be transferred to any location in memory (no range limitation) and can be accomplished with the full flexibility of the addressing modes, with the exception of register mode 0. Execution of a jump with mode 0 will cause an illegal instruction condition. (Program control cannot be transferred to a register.) Register deferred mode is legal and will cause program control to be transferred to the address held in the specified register. Note that instructions are word data and must therefore be fetched from an even-numbered address. A boundary error trap condition will result when the processor attempts to fetch an instruction from an odd address. |
| JSR | 004RDD | (tmp) ← (dst); ↓(SP) ← reg; reg ← PC; PC ← (tmp) | Unaffected | In execution of the JSR, the old contents of the specified register (the linkage pointer) are automatically pushed onto the processor stack and new linkage information placed in the register. Thus, subroutines nested within subroutines to any depth may all be called with the same linkage register. There is no need either to plan the maximum depth at which any particular subroutine will be called or to include instructions in each routine to save and restore the linkage pointer. Further, since all linkages are saved in a re-entrant manner on the processor stack, execution of a subroutine may be interrupted, and the same subroutine re-entered and executed by an interrupt service routine. Execution of the initial subroutine can then be resumed when other requests are satisfied. This process (called nesting) can proceed to any level. JSR PC, dst is a special case of the PDP-11 subroutine call suitable for subroutine calls that transmit parameters. |
| RTS | 00020R | PC ← (reg); (reg) ← (SP)↑ | Unaffected | Loads contents of register into PC and pops the top element of the processor stack into the specified register. Return from a non-re-entrant subroutine is typically made through the same register that was used in its call. Thus, a subroutine called with a JSR PC, dst exits with an RTS PC, and a subroutine called with a JSR R5, dst may pick up parameters with addressing modes (R5)+, X(R5), or @X(R5) and finally exit with an RTS R5. |
| MARK | 0064NN | SP ← SP + 2×NN; PC ← R5; R5 ← (SP)↑ | Unaffected | Used as part of the standard PDP-11 subroutine return convention. MARK facilitates the stack cleanup procedures involved in subroutine exit. Assembler format is: MARK N. NN = number of parameters. |
| SOB | 077R00+offset | R ← R - 1; if result ≠ 0 then PC ← PC - (2 × offset) | Unaffected | The register is decremented. If it is not equal to 0, twice the offset is subtracted from the PC (now pointing to the following word). The offset is interpreted as a six-bit positive number. This instruction provides a fast, efficient method of loop control. Assembler syntax is: SOB R,A where A is the address to which transfer is to be made if the decremented R is not equal to 0. Note that the SOB instruction cannot be used to transfer control in the forward direction. |

#### Trap Instructions

| Mnemonic | Op Code | Operation | Condition Codes | Description |
|----------|---------|-----------|-----------------|-------------|
| BPT | 000003 | ↓(SP) ← PS; ↓(SP) ← PC; PC ← (14); PS ← (16) | Loaded from trap vector | Performs a trap sequence with a trap vector address of 14. Used to call debugging aids. The user is cautioned against employing code 000003 in programs run under these debugging aids. |
| IOT | 000004 | ↓(SP) ← PS; ↓(SP) ← PC; PC ← (20); PS ← (22) | Loaded from trap vector | Performs a trap sequence with a trap vector address of 20. Used to call the I/O executive routine IOX in the paper-tape software system and for error reporting in the disk operating system. |
| EMT | 104000–104377 | ↓(SP) ← PS; ↓(SP) ← PC; PC ← (30); PS ← (32) | Loaded from trap vector | All operation codes from 104000 to 104377 are EMT instructions and may be used to transmit information to the emulating routine (e.g., function to be performed). The trap vector for EMT is at address 30; the new central processor status (PS) is taken from the word at address 32. **CAUTION:** EMT is used frequently by DEC system software and is therefore not recommended for general use. |
| TRAP | 104400–104777 | ↓(SP) ← PS; ↓(SP) ← PC; PC ← (34); PS ← (36) | Loaded from trap vector | Operation codes from 104400 to 104777 are TRAP instructions. TRAPs and EMTs are identical in operation, except that the trap vector for TRAP is at address 34. **NOTE:** Since DEC software makes frequent use of EMT, the TRAP instruction is recommended for general use. |

### Table 2-5 — Miscellaneous Instructions

| Mnemonic | Op Code | Operation | Condition Codes | Description |
|----------|---------|-----------|-----------------|-------------|
| RTI | 000002 | PC ← (SP)↑; PSW ← (SP)↑ | Loaded from processor stack | Used to exit from an interrupt or trap service routine. The PC and PSW are restored (popped) from the processor stack. If the RTI sets the T-bit in the PSW, a trace trap will occur prior to executing the next instruction. |
| RTT | 000006 | PC ← (SP)↑; PS ← (SP)↑ | Loaded from processor stack | This is the same as the RTI instruction, except that it inhibits a trace trap, while RTI permits a trace trap. If a trace trap is pending, the first instruction after the RTT will be executed prior to the next "T" trap. In the case of the RTI instruction, the "T" trap will occur immediately after the RTI. |
| MFPI / MFPD | 0065SS / 1065SS | (temp) ← (src); ↓(SP) ← (temp) | N: set if source < 0; otherwise cleared; Z: set if source = 0; otherwise cleared; V: cleared; C: unaffected | This instruction pushes a word onto the current stack from an address in previous space. Processor Status (bits 13, 12). The source address is computed using the current registers and memory map. |
| MTPI / MTPD | 0066SS / 1066SS | (temp) ← (SP)↑; (dst) ← (temp) | N: set if source < 0; otherwise cleared; Z: set if source = 0; otherwise cleared; V: cleared; C: unaffected | This instruction pops a word off the current stack determined by PS (bits 15, 14) and stores that word into an address in previous space PS (bits 13, 12). The destination address is computed using the current registers and memory map. |
| MFPS | 1067DD | (dst) ← PSW (lower 8 bits) | N: set if PSW bit 7 = 1; otherwise cleared; Z: set if PS[0:7] = 0; otherwise cleared; V: cleared; C: not affected | The 8-bit contents of the PS are moved to the effective destination. If destination is mode 0, PS bit 7 is sign-extended through upper byte of the register, and destination operand is treated as a byte address. |
| MTPS | 1064SS | PSW ← (src) | Set according to effective src operand bits 0-3 | The 8 bits of the effective operand replace the current contents of the PSW. The source operand address is treated as a byte address. Note that PSW bit 4 cannot be set with this instruction. The src operand remains unchanged. Because there is no hardware to prevent execution of these instructions in User mode, it is necessary for the system software to prevent any reference to the PSW address by a user. |
| HALT | 000000 | — | Unaffected | Causes the processor operation to cease. The console is given control of the processor. The console data lights display the address of the HALT instruction plus two. Transfers on the Unibus are terminated immediately. The PC points to the next instruction to be executed. Pressing the CON key on the console causes processor operation to resume. No INIT signal is given. |
| WAIT | 000001 | — | Unaffected | Provides a way for the processor to relinquish use of the bus while it waits for an external interrupt. Having been given a WAIT command, the processor will not compete for bus by fetching instructions or operands from memory. This permits higher transfer rates between device and memory, as no processor-induced latencies will be encountered by bus requests from the device. In WAIT, as in all instructions, the PC points to the next instruction following the WAIT operation. Thus, when an interrupt causes the PC and PS to be pushed onto the stack, the address of the next instruction following the WAIT is saved. The exit from the interrupt routine (i.e., execution of an RTI instruction) will cause resumption of the interrupted process at the instruction following the WAIT. |
| RESET | 000005 | — | Unaffected | Sends INIT on the Unibus for 100 ms. All devices on the Unibus are reset to their state at power-up. |

### Table 2-6 — Condition Code Operators

| Mnemonic | Op Code | Instruction |
|----------|---------|-------------|
| CLC | 000241 | Clear condition code C |
| CLV | 000242 | Clear condition code V |
| CLZ | 000244 | Clear condition code Z |
| CLN | 000250 | Clear condition code N |
| CCC | 000257 | Clear all condition code bits |
| SEC | 000261 | Set condition code C |
| SEV | 000262 | Set condition code V |
| SEZ | 000264 | Set condition code Z |
| SEN | 000270 | Set condition code N |
| SCC | 000277 | Set all condition code bits |

**NOTE:** Selectable combinations of condition code bits may be cleared or set together. The status of bit 4 controls the way in which bits 0, 1, 2, and 3 are to be modified. If bit 4 = 1, the specified bits are set; if bit 4 = 0, the specified bits are cleared.

## 2.4 Instruction Execution Time

The execution time for an instruction depends on the instruction itself, the modes of addressing used, and the type of memory being referenced. In the most general case, the instruction execution time is the sum of a source address (SRC) time, a destination address (DST) time, and an execute, fetch (EF) time.

```
Instr Time = SRC Time + DST Time + EF Time
```

Some of the instructions require only some of these times, and are so noted in Paragraph 2.4.1. All timing information is in microseconds, unless otherwise noted. Times are typical; processor timing can vary ±10%.

### 2.4.1 Basic Instruction Set Timing

Timing formulas:
- Double-Operand (all instructions): Instr Time = SRC Time + DST Time + EF Time
- Single-Operand (all instructions): Instr Time = DST Time + EF Time
- Branch, Jump, Control, Trap, and Miscellaneous (all instructions): Instr Time = EF Time

NOTES:
1. The times specified apply to both word and byte instructions, whether odd or even byte.
2. Timing is given without regard for NPR or BR servicing.
3. If the memory management is enabled, instruction execution times increase by 0.12 μs for each memory cycle used.
4. All timing is based on memory with the following performance characteristics:

| Memory | Access Time (μs) | Cycle Time (μs) |
|--------|-------------------|------------------|
| Core (MM11-DP) | 0.510 | 1.1 |
| MOS (MS11-JP) | 0.635 | 0.920 |

### Table 2-7 — PDP-11/34 Instruction Set Timing

#### Source Address Time

| Source Mode | Memory Cycles | Core (MM11-DP) μs | MOS (MS11-JP) μs |
|-------------|---------------|--------------------|-------------------|
| 0 | 0 | 0.00 | 0.00 |
| 1 | 1 | 1.13 | 1.26 |
| 2 | 1 | 1.33 | 1.46 |
| 3 | 2 | 2.37 | 2.62 |
| 4 | 1 | 1.28 | 1.41 |
| 5 | 2 | 2.57 | 2.82 |
| 6 | 2 | 2.57 | 2.82 |
| 7 | 3 | 3.80 | 4.18 |

#### Destination Address Time

**Modifying Single-Operand and Modifying Double-Operand** (except MOV, SWAB, ROR, ROL, ASR, ASL):

| Dest Mode | Memory Cycles | Core μs | MOS μs |
|-----------|---------------|---------|--------|
| 0 | 0 | 0.00 | 0.00 |
| 1 | 2 | 1.62 | 1.74 |
| 2 | 2 | 1.77 | 1.89 |
| 3 | 3 | 2.90 | 3.15 |
| 4 | 2 | 1.77 | 1.89 |
| 5 | 3 | 3.00 | 3.25 |
| 6 | 3 | 3.10 | 3.35 |
| 7 | 4 | 4.29 | 4.66 |

**MOV:**

| Dest Mode | Memory Cycles | Core μs | MOS μs |
|-----------|---------------|---------|--------|
| 0 | 0 | 0.00 | 0.00 |
| 1 | 1 | 0.93 | 0.93 |
| 2 | 1 | 0.93 | 0.93 |
| 3 | 2 | 2.17 | 2.29 |
| 4 | 1 | 1.13 | 1.13 |
| 5 | 2 | 2.22 | 2.34 |
| 6 | 2 | 2.37 | 2.49 |
| 7 | 3 | 3.50 | 3.75 |

**MTPS:**

| Dest Mode | Memory Cycles | Core μs | MOS μs |
|-----------|---------------|---------|--------|
| 0 | 0 | 0.00 | 0.00 |
| 1 | 1 | 0.95 | 0.95 |
| 2 | 1 | 1.13 | 1.26 |
| 3 | 2 | 2.26 | 2.51 |
| 4 | 1 | 1.13 | 1.26 |
| 5 | 2 | 2.26 | 2.51 |
| 6 | 2 | 2.44 | 2.69 |
| 7 | 3 | 3.57 | 4.20 |

**MFPS:**

| Dest Mode | Memory Cycles | Core μs | MOS μs |
|-----------|---------------|---------|--------|
| 0 | 0 | 0.00 | 0.00 |
| 1 | 1 | 0.64 | 0.64 |
| 2 | 1 | 0.64 | 0.64 |
| 3 | 2 | 1.95 | 2.08 |
| 4 | 1 | 0.82 | 0.82 |
| 5 | 2 | 1.95 | 2.08 |
| 6 | 2 | 2.13 | 2.26 |
| 7 | 3 | 3.26 | 3.51 |

**Non-modifying Single-Operand and Double-Operand (SWAB, ROR, ROL, ASR, ASL):**

| Dest Mode | Memory Cycles | Core μs | MOS μs |
|-----------|---------------|---------|--------|
| 0 | 0 | 0.00 | 0.00 |
| 1 | 2 | 1.42 | 1.54 |
| 2 | 2 | 1.57 | 1.69 |
| 3 | 3 | 2.70 | 2.95 |
| 4 | 2 | 1.62 | 1.74 |
| 5 | 3 | 2.80 | 3.05 |
| 6 | 3 | 2.90 | 3.15 |
| 7 | 4 | 4.09 | 4.46 |

**Non-modifying Single-Operand and Double-Operand:**

| Dest Mode | Memory Cycles | Core μs | MOS μs |
|-----------|---------------|---------|--------|
| 0 | 0 | 0.00 | 0.00 |
| 1 | 1 | 1.13 | 1.26 |
| 2 | 1 | 1.28 | 1.41 |
| 3 | 2 | 2.42 | 2.67 |
| 4 | 1 | 1.33 | 1.46 |
| 5 | 2 | 2.52 | 2.77 |
| 6 | 2 | 2.62 | 2.87 |
| 7 | 3 | 3.80 | 4.18 |

#### Execute, Fetch Time

**Double Operand:**

| Instruction | Memory Cycles | Core μs | MOS μs |
|-------------|---------------|---------|--------|
| ADD, SUB, CMP, BIT, BIC, BIS, XOR | 1 | 2.03 | 2.16 |
| MOV | 1 | 1.83 | 1.96 |

**Single Operand:**

| Instruction | Memory Cycles | Core μs | MOS μs |
|-------------|---------------|---------|--------|
| CLR, COM, INC, DEC, ADC, SBC, TST | 1 | 1.83 | 1.96 |
| SWAB, NEG | 1 | 2.03 | 2.16 |
| ROR, ROL, ASR, ASL | 1 | 2.18 | 2.31 |
| MTPS | 2 | 2.99 | 3.12 |
| MFPS | 2 | 1.99 | 2.12 |

**EIS Instructions** (use with DST times):

| Instruction | Memory Cycles | Core μs | MOS μs |
|-------------|---------------|---------|--------|
| MUL | 1 | 8.82* | 8.95* |
| DIV (overflow) | 1 | 2.78 | 2.91 |
| DIV | 1 | 12.48 | 12.61 |
| ASH | 1 | 4.18** | 4.31** |
| ASHC | 1 | 4.18** | 4.31** |

\* Add 200 ns for each bit transition in serial data from LSB to MSB.
\*\* Add 200 ns per shift.

**Memory Management Instructions:**

| Instruction | Memory Cycles | Core μs | MOS μs |
|-------------|---------------|---------|--------|
| MFPI(D) | 2 | 3.07 | 3.14 |
| MTPI(D) | 2 | 3.37 | 3.34 |

**MFPI(D) / MTPI(D) Destination Time:**

| Dest Mode | Memory Cycles | Core μs | MOS μs |
|-----------|---------------|---------|--------|
| 0 | 0 | 0.00 | 0.00 |
| 1 | 1 | 0.98 | 1.24 |
| 2 | 1 | 1.32 | 1.44 |
| 3 | 2 | 2.20 | 2.45 |
| 4 | 1 | 1.18 | 1.44 |
| 5 | 2 | 2.20 | 2.45 |
| 6 | 2 | 2.40 | 2.65 |
| 7 | 3 | 3.59 | 3.96 |

**Branch Instructions:**

| Instruction | Memory Cycles | Core μs | MOS μs |
|-------------|---------------|---------|--------|
| BR, BNE, BEQ, BPL, BMI, BVC, BVS, BCC, BCS, BGE, BLT, BGT, BLE, BHI, BLOS, BHIS, BLO (Branch taken) | 1 | 2.18 | 2.31 |
| (No branch) | 1 | 1.63 | 1.76 |
| SOB (Branch taken) | 1 | 2.38 | 2.51 |
| SOB (No branch) | 1 | 1.98 | 2.11 |

**Jump Instructions:**

| Instruction / Dest Mode | Memory Cycles | Core μs | MOS μs |
|-------------------------|---------------|---------|--------|
| JMP mode 1 | 1 | 1.83 | 1.96 |
| JMP mode 2 | 1 | 2.18 | 2.31 |
| JMP mode 3 | 2 | 3.12 | 3.37 |
| JMP mode 4 | 1 | 2.03 | 2.16 |
| JMP mode 5 | 2 | 3.07 | 3.32 |
| JMP mode 6 | 2 | 3.07 | 3.32 |
| JMP mode 7 | 3 | 4.25 | 4.78 |
| JSR mode 1 | 2 | 3.32 | 3.44 |
| JSR mode 2 | 2 | 3.47 | 3.59 |
| JSR mode 3 | 3 | 4.40 | 4.65 |
| JSR mode 4 | 2 | 3.32 | 3.44 |
| JSR mode 5 | 3 | 4.40 | 4.65 |
| JSR mode 6 | 3 | 4.60 | 4.85 |
| JSR mode 7 | 4 | 5.69 | 6.06 |

**Miscellaneous:**

| Instruction | Memory Cycles | Core μs | MOS μs |
|-------------|---------------|---------|--------|
| RTS | 2 | 3.32 | 3.57 |
| MARK | 2 | 4.27 | 4.52 |
| RTI, RTT | 3 | 4.60 | 4.98 |
| Set or Clear C, V, N, Z | 1 | 2.03 | 2.16 |
| HALT | 1 | 1.68 | 1.81 |
| WAIT | 1 | 1.68 | 1.81 |
| RESET | — | 100 ms | 100 ms |
| IOT, EMT, TRAP, BPT | 5 | 7.32 | 7.70 |

### 2.4.2 Bus Latency Times

Interrupts (BR requests) are acknowledged at the end of the current instruction. For a typical instruction, with an instruction execution time of 4 μs, the average time to request acknowledgement would be 2 μs.

Interrupt service time, which is the time from BR acknowledgement to the first subroutine instruction, is 7.32 μs max for core, and 7.7 μs for MOS.

NPR (DMA) latency, which is the time from request to bus mastership for the first NPR device, is 2.5 μs max.

## 2.5 Extended Instruction Set

The Extended Instruction Set (EIS) provides the user with the capability of extended manipulation of fixed-point numbers. Use of the EIS instructions does not degrade processor timing or affect NPR latency. Interrupts are serviced at the end of an EIS instruction.

The EIS instructions are:

| Mnemonic | Instruction | Op Code |
|----------|-------------|---------|
| MUL | Multiply | 070RSS |
| DIV | Divide | 071RSS |
| ASH | Shift arithmetically | 072RSS |
| ASHC | Arithmetic shift combined | 073RSS |

Number formats: See Figure 2-3.

- 16-bit single word: bit 15 is sign (S), bits 14-0 are number.
- 32-bit double word: R has sign (S) in bit 15, bits 14-0 are high number part; Rv1 has bits 15-0 as low number part.
- S = 0 for positive quantities; S = 1 for negative quantities (number is in 2's complement notation).

### EIS Examples

**Multiply Instruction — MUL 070RSS**

Example: 16-bit product (R is odd)
```
000241          ; CLC — Clear carry condition code
012701,400      ; MOV #400,R1
070127,10       ; MUL #10,R1
1034xx          ; BCS ERROR — Carry will be set if product < -2^15 or >= 2^15

Before: (R1) = 000400
After:  (R1) = 004000  ; no significance lost
```

**Divide Instruction — DIV 071RSS**

Example:
```
005000          ; CLR R0
012701,20001    ; MOV #20001,R1
071027,2        ; DIV #2,R0

Before: (R0) = 000000, (R1) = 020001
After:  (R0) = 010000 (Quotient), (R1) = 000001 (Remainder)
```

**Arithmetic Shift Instruction — ASH 072RSS**

Example: ASH R0, R3
```
Before: (R3) = 000003, (R0) = 001234
After:  (R3) = 000003, (R0) = 012340
```

**Arithmetic Shift Combined Instruction — ASHC 073RSS**

Similar to the example for the ASH instruction except that two registers are used.

## 2.6 Instruction Set Differences

Table 2-8 lists the instruction set differences between the PDP-11/34 and other PDP-11 machines.

### Table 2-8 — Programming Differences

#### General Registers (including PC and SP)

| Topic | 11/05 and 11/10 | 11/35 and 11/40 | 11/04 | 11/34 |
|-------|-----------------|-----------------|-------|-------|
| OPR %R,(R)+ or OPR %R,-(R), OPR %R,@(R)+ or OPR %R,@-(R) (using same register as both src and dst) | Initial contents of R are used as the source operand. | Contents of R are incremented by 2 (or decremented by 2), before being used as the source operand. | Same as 11/05 | Same as 11/05 |
| JMP (R)+ or JSR register,(R)+ (jump using autoincrement) | Contents of R are incremented by 2, then used as the new PC address. | Initial contents of R are used as new PC. | Same as 11/40 | Same as 11/40 |
| MOV PC,@#A or MOV PC,A (moving the incremented PC to a memory address referenced by the PC) | Location A will contain PC + 2. | Location A will contain the PC of the move instruction + 4. | Same as 11/05 | Same as 11/05 |
| Stack Pointer (SP), R6 used for referencing | Using the SP for pointing to odd addresses or non-existent memory causes a halt (double bus error). | Odd address or non-existent memory references with SP cause a fatal trap with a new stack created at locations 0 and 2. | Same as 11/05 | Same as 11/05 |
| Stack Overflow | Stack limit fixed at 400₈. Overflow (going lower) checked after modes 4 and 5 using R6, and JSR and traps. Overflow serviced by an overflow trap. No red zone. | Variable limit with stack limit option. Overflow checked after JSR, traps, and address modes 1, 2, 4, and 6. Non-altering references to stack data are always allowed. There is a 16-word yellow (warning) zone. Red zone trap occurs if stack is 16 words below boundary; PS and PC are saved at locations 0 and 2. | Same as 11/05 | Same as 11/05 |

#### Traps and Interrupts

| Topic | 11/05 and 11/10 | 11/35 and 11/40 | 11/04 | 11/34 |
|-------|-----------------|-----------------|-------|-------|
| RTI Instruction | First instruction after RTI instruction is always executed. | If RTI sets the T-bit, the T-bit trap is acknowledged immediately after the RTI instruction. | Same as 11/40 | Same as 11/40 |
| RTT Instruction | Not implemented. | First instruction after RTT is guaranteed to be executed. | Same as 11/40 | Same as 11/40 |
| Processor status odd byte at location 777777 | Odd byte of PS can be addressed without a trap. | Same as 11/05 | Same as 11/05 | Same as 11/05 |
| T-bit of PS | T-bit can be loaded by direct address of PS or from console. | Only RTI, RTT, traps and interrupts can load the T-bit. | Same as 11/05 | Same as 11/40 |

#### Bus Errors

| Topic | 11/05 and 11/10 | 11/35 and 11/40 | 11/04 | 11/34 |
|-------|-----------------|-----------------|-------|-------|
| PC contains odd address | PC unincremented. | Same as 11/05 | Same as 11/05 | Same as 11/05 |
| PC contains an address in non-existent memory | PC incremented. | PC unincremented. | Same as 11/05 | Same as 11/40 |
| Register contains odd address and instruction mode 2 | Register unincremented. | Register incremented. | Same as 11/05 | Same as 11/05 except for MOV mode 2 and MTPI where the register will be incremented. |
| Register contains address in non-existent memory and instruction mode 2 | Register incremented. | Register incremented. | Register unincremented. | Same as 11/04 except for MOV mode 2 destination and MTPI where the register will be incremented. |
| Interrupt service routine | The first instruction will not be executed if another interrupt occurs at a higher priority. | Same as 11/05 | Same as 11/05 | Same as 11/05 |

#### Priority Order of Traps and Interrupts

| 11/05 and 11/10 | 11/35 and 11/40 | 11/04 | 11/34 |
|-----------------|-----------------|-------|-------|
| Odd address | Halt instruction | Halt instruction | Same as 11/40 except no red zone stack overflow |
| Time-out | Odd address | Bus error | |
| Halt instruction | Stack overflow (red) | Trap instruction | |
| Trap instructions | Mem mgt error | Trace trap | |
| Trace trap | Time-out | Stack overflow | |
| Stack overflow | Parity | Power fail | |
| Power fail | Trap instruction | Halt from console | |
| Halt from console | Trace trap | Interrupts | |
| | Stack overflow (yellow) | Next instruction fetch | |
| | Power fail | | |
| | Halt from console | | |

#### Other Differences

| Topic | 11/05 and 11/10 | 11/35 and 11/40 | 11/04 | 11/34 |
|-------|-----------------|-----------------|-------|-------|
| SWAB and V-bit | V-bit is cleared. | Same as 11/05 | Same as 11/05 | Same as 11/05 |
| Instruction set | Basic set. | Basic set and MARK, RTT, SOB, SXT, XOR. EIS adds: MUL, DIV, ASH, ASHC. FIS adds: FADD, FSUB, FMUL, FDIV. KT11-D adds: MTPI, MFPI. | Basic set and RTT. | Basic set and MARK, RTT, SOB, SXT, XOR, MUL, DIV, ASH, ASHC, MTPI, MFPI, MTPS, MFPS. (MTPS and MFPS are new instructions used for LSI-11.) |
| Memory management violation during a trap sequence | Does not apply. | If a mem mgt violation occurs between the first and second push down of the stack during a trap sequence, the status of the CPU before the violation is placed as the PS on the Kernel stack. | Does not apply. | If a mem mgt violation occurs between the first and second push down of the stack during a trap sequence, the status of the vector +2 of the original trap is placed as the PS on the Kernel stack. |
