# Chapter 4 -- Microprocessor Instruction Description

## 4.1 Data and Instruction Formats

Data in the CPU is stored in the form of 8-bit binary integers. All data transfers to the system data bus will be in the same format.

```
| D7 D6 D5 D4 D3 D2 D1 D0 |    DATA WORD
```

The program instructions may be one, two, or three bytes in length. Multiple byte instructions must be stored in successive words in program memory. The instruction formats then depend on the particular operation executed.

**One Byte Instructions:**
```
| D7 D6 D5 D4 D3 D2 D1 D0 |    OP CODE    Register to register, memory reference
                                             I/O arithmetic or logical, rotate or
                                             return instructions.
```

**Two Byte Instructions:**
```
| D7 D6 D5 D4 D3 D2 D1 D0 |    OP CODE
| D7 D6 D5 D4 D3 D2 D1 D0 |    OPERAND    Immediate mode instructions.
```

**Three Byte Instructions:**
```
| D7 D6 D5 D4 D3 D2 D1 D0 |    OP CODE
| D7 D6 D5 D4 D3 D2 D1 D0 |    LOW ADDRESS    Jump or Call instructions.
| X  X  D5 D4 D3 D2 D1 D0 |    HIGH ADDRESS*
```

\* For the third byte of this instruction, D6 and D7 are "don't care" bits.

## 4.2 Microprocessor Instructions

### 4.2.1 Index Register Instructions

The load instructions do not affect the flag flip-flops. The increment and decrement instructions affect all flip-flops except the carry.

| Mnemonic | Min States | Code D7..D0 | Description |
|---|---|---|---|
| Lr,r₂ | 5 | 11 DDD SSS | Load index register r₁ with the content of index register r₂. |
| LrM | 8 | 11 DDD 111 | Load index register r with the content of memory register M. |
| LMr | 7 | 11 111 SSS | Load memory register M with the content of index register r. |
| LrI | 8 | 00 DDD 110 / BBBBBBBB | Load index register r with data B...B. |
| LMI | 9 | 00 111 110 / BBBBBBBB | Load memory register M with data B...B. |
| INr | 5 | 00 DDD 000 | Increment the content of index register r (r≠A). |
| DCr | 5 | 00 DDD 001 | Decrement the content of index register r (r≠A). |

### 4.2.2 Accumulator Group Instructions

The result of the ALU instructions affect all of the flag flip-flops. The rotate instructions affect only the carry flip-flop.

| Mnemonic | Min States | Code D7..D0 | Description |
|---|---|---|---|
| ADr | 5 | 10 000 SSS | Add the content of index register r, memory register M, or data B...B to the accumulator. An overflow (carry) sets the carry flip-flop. |
| ADM | 8 | 10 000 111 | (same as above, memory operand) |
| ADI | 8 | 00 000 100 / BBBBBBBB | (same as above, immediate operand) |
| ACr | 5 | 10 001 SSS | Add with carry. |
| ACM | 8 | 10 001 111 | (same, memory operand) |
| ACI | 8 | 00 001 100 / BBBBBBBB | (same, immediate operand) |
| SUr | 5 | 10 010 SSS | Subtract the content of index register r, memory register M, or data B...B from the accumulator. An underflow (borrow) sets the carry flip-flop. |
| SUM | 8 | 10 010 111 | (same, memory operand) |
| SUI | 8 | 00 010 100 / BBBBBBBB | (same, immediate operand) |
| SBr | 5 | 10 011 SSS | Subtract with borrow. |
| SBM | 8 | 10 011 111 | (same, memory operand) |
| SBI | 8 | 00 011 100 / BBBBBBBB | (same, immediate operand) |
| NDr | 5 | 10 100 SSS | Compute the logical AND of the content of index register r, memory register M, or data B...B with the accumulator. |
| NDM | 8 | 10 100 111 | (same, memory operand) |
| NDI | 8 | 00 100 100 / BBBBBBBB | (same, immediate operand) |
| XRr | 5 | 10 101 SSS | Compute the Exclusive OR. |
| XRM | 8 | 10 101 111 | (same, memory operand) |
| XRI | 8 | 00 101 100 / BBBBBBBB | (same, immediate operand) |
| ORr | 5 | 10 110 SSS | Compute the Inclusive OR. |
| ORM | 8 | 10 110 111 | (same, memory operand) |
| ORI | 8 | 00 110 100 / BBBBBBBB | (same, immediate operand) |
| CPr | 5 | 10 111 SSS | Compare the content of index register r, memory register M, or data B...B with the accumulator. The content of the accumulator is unchanged. |
| CPM | 8 | 10 111 111 | (same, memory operand) |
| CPI | 8 | 00 111 100 / BBBBBBBB | (same, immediate operand) |
| RLC | 5 | 00 000 010 | Rotate the content of the accumulator left. |
| RRC | 5 | 00 001 010 | Rotate the content of the accumulator right. |
| RAL | 5 | 00 010 010 | Rotate the content of the accumulator left through the carry. |
| RAR | 5 | 00 011 010 | Rotate the content of the accumulator right through the carry. |

### 4.2.3 Program Counter and Stack Control Instructions

| Mnemonic | Min States | Code D7..D0 | Description |
|---|---|---|---|
| JMP | 11 | 01 XXX 100 / B₇B₆ B₅B₄B₃ B₂B₁B₀ / XX B₅B₄B₃ B₂B₁B₀ | Unconditionally jump to memory address B₁₃...B₈B₇...B₀. |
| JFc | 9 or 11 | 01 0C₁C₀ 000 / (addr) | Jump if condition flip-flop c is false. Otherwise, execute the next instruction in sequence. |
| JTc | 9 or 11 | 01 1C₁C₀ 000 / (addr) | Jump if condition flip-flop c is true. Otherwise, execute the next instruction in sequence. |
| CAL | 11 | 01 XXX 110 / (addr) | Unconditionally call the subroutine. Save the current address (up one level in the stack). |
| CFc | 9 or 11 | 01 0C₁C₀ 010 / (addr) | Call if condition flip-flop c is false. Save the current address (up one level in the stack). |
| CTc | 9 or 11 | 01 1C₁C₀ 010 / (addr) | Call if condition flip-flop c is true. Save the current address (up one level in the stack). |
| RET | 5 | 00 XXX 111 | Unconditionally return (down one level in the stack). |
| RFc | 3 or 5 | 00 0C₁C₀ 011 | Return if condition flip-flop c is false. |
| RTc | 3 or 5 | 00 1C₁C₀ 011 | Return if condition flip-flop c is true. |
| RST | 5 | 00 AAA 101 | Call the subroutine at memory address AAA000 (up one level in the stack). |

### 4.2.4 Input/Output Instructions

| Mnemonic | Min States | Code D7..D0 | Description |
|---|---|---|---|
| INP | 8 | 01 00M MM1 | Read the content of the selected input port (MMM) into the accumulator. |
| OUT | 6 | 01 RRM MM1 | Write the content of the accumulator into the selected output port (RRMMM) (RR ≠ 00). |

### 4.2.5 Machine Instruction

| Mnemonic | Min States | Code D7..D0 | Description |
|---|---|---|---|
| HLT | 4 | 00 000 00X | Enter the STOPPED state and remain there until interrupted. |
| HLT | 4 | 11 111 111 | Enter the STOPPED state and remain there until interrupted. |

### Notes

1. SSS = Source Index Register; DDD = Destination Index Register. These registers, r₁, r₂, are designated A (accumulator--000), B(001), C(010), D(011), E(100), H(101), L(110).
2. Memory registers are addressed by the contents of registers H & L.
3. Additional bytes of instruction are designated by BBBBBBBB.
4. X = "Don't Care".
5. Flag flip-flops are defined by C₁C₀: carry (00--overflow or underflow), zero (01--result is zero), sign (10--MSB of result is "1"), parity (11--parity is even).
