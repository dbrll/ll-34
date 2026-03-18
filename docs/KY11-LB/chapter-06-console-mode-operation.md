# Chapter 6 -- Console Mode Operation

## 6.1 Introduction

This chapter is a recapitulation of all console key and indicator functions. Operation, use, and examples of the utilization of each key are presented. Key operations are divided into console mode and maintenance mode. Examples of console sequences to demonstrate the proper use of the KY11-LB are presented together with further notes and hints on operation.

## 6.2 Console Key Operations

This section describes the operation of each key in a step-by-step procedure. The reader is assumed to have read earlier chapters as some descriptions include the use of keys previously described.

A notation for each key will be introduced in each description and is enclosed in angle brackets < >. These notations are used extensively in Paragraph 6.4 to describe various sequences of key operations.

### \<CLR\>

Used to clear an incorrect entry or existing data.

1. Press and release the CLR key.
2. The display (six digits) will be all zeros.
3. Clears the SR DISP, BUS ERR, or MAINT indicators if on.

### Numerics 0-7

Used to key in an octal numeric digit (0 through 7).

1. Press a numeric key (0 through 7).
2. The corresponding digit will be left shifted into the 6-digit octal display with the previously displayed digits also being left shifted.
3. Release the numeric key.

To enter the number \<xxxxxx\>, e.g., 123456:

1. Press and release the CLR key (000000 will be displayed).
2. Press and release the 1 key (000001 will be displayed).
3. Press and release the 2 key (000012 will be displayed).
4. Press and release the 3 key (000123 will be displayed).
5. Press and release the 4 key (001234 will be displayed).
6. Press and release the 5 key (012345 will be displayed).
7. Press and release the 6 key (123456 will be displayed).

The number has now been entered. Leading zeros do not have to be entered.

### \<LSR\>

Used to load the switch register (accessible as Unibus address 777570).

1. Press and release the LSR key.
2. The display will show the data loaded and the SR DISP indicator will be on.

To load the number 777 i.e., \<LSR 777\>, into the switch register:

1. Press and release the CLR key.
2. Key in the number 777.
3. Press and release the LSR key.
4. 777 will be displayed and the SR DISP indicator will be on.

### \<LAD\>

Used to load the Unibus address pointer prior to performing an EXAMINE, DEPOSIT, or START.

1. Press and release the LAD key.
2. Display will be all zeros.

To load address 200, i.e., \<LAD 200\>:

1. Press and release the CLR key.
2. Key in 200.
3. Press and release the LAD key.
4. Display is all zeros.

### \<DIS AD\>

Displays the current contents of the Unibus address pointer.

1. Press and release the DIS AD key.
2. Display will show the current Unibus address pointer.

To perform the sequence:

1. Press and release the CLR key.
2. Key in 400.
3. Press and release the LAD key (display is all zeros).
4. Press and release the DIS AD key.
5. Display shows the 400 from the Unibus address pointer.

### \<DEP\>

Used to deposit a number into the location pointed to by the Unibus address pointer.

1. Processor must be halted (RUN indicator off); otherwise key is ignored.
2. Press and release the DEP key.
3. Display shows the data deposited.

To deposit \<DEP xxxx\> (e.g., 5252) into location 1000, the sequence is as follows:

1. Press and release the CLR key.
2. Key in 1000.
3. Press and release the LAD key.
4. Key in 5252.
5. Press and release the DEP key.

### \<EXAM\>

Used to examine the contents of a location pointed to by the Unibus address pointer.

1. Machine must be halted.
2. Press and release the EXAM key.
3. Display shows the contents of the location examined.

To examine general-purpose register R7 (program counter) at Unibus address 777707, the following sequence is used:

1. Press and release the CLR key.
2. Key in 777707.
3. Press and release the LAD key.
4. Press and release the EXAM key.
5. The contents of R7 will be displayed.

### \<CNTRL\>

The CNTRL key is always used in conjunction with some other key. When it is used it must be pressed and held down while the second key is pressed and released.

### \<CNTRL-HLT/SS\>

Used to halt the processor.

1. Press and hold down the CNTRL key.
2. Press and release the HLT/SS key.
3. Display will show the current contents of R7 (program counter) and the RUN indicator will be off.
4. Release the CNTRL key.

If the processor is already halted, the use of the HLT/SS key will single-instruction step the processor. (The CNTRL key is not required to single-instruction step the processor once halted.)

1. Press and release the HLT/SS key.
2. Processor will perform one instruction and halt.
3. Display will show the new current contents of R7 (program counter).

### \<CNTRL-CONT\>

Used to allow the processor to begin running from a halt.

1. Press and hold down the CNTRL key.
2. Press and release the CONT key.
3. Processor will run unless a program halt instruction is encountered. The RUN and SR DISP indicators should be on and the contents of the switch register should be displayed. If a halt instruction was encountered, the indicators will be off and the program counter will be displayed.
4. Release the CNTRL key.

> **NOTE:** If the processor is already running, use of \<CNTRL-CONT\> will result in the switch register being displayed; there will be no other effect on the processor.

### \<CNTRL-BOOT\>

Used to initiate running of M9301 Bootstrap program.

1. Processor must be halted.
2. Press and hold down the CNTRL key.
3. Press and release the BOOT key.
4. Processor should start running (RUN indicator on) the bootstrap program selected on the M9301.
5. Release the CNTRL key.

> **NOTE:** For more information concerning the M9301 Bootstrap program, consult the system users guide.

### \<CNTRL-START\>

Used to start the processor running a program from a given starting address.

1. Processor must be halted; otherwise key is ignored.
2. Press and hold down CNTRL key.
3. Press and release the START key.
4. RUN indicator will be on unless a halt instruction is encountered. The SR DISP indicator should also be on and the contents of the switch register should be displayed.
5. Release the CNTRL key.

To start running a program at location 1000, the following sequence is used:

1. Press and release the CLR key.
2. Key in 1000.
3. Press and release the LAD key.
4. Press and hold down the CNTRL key.
5. Press and release the START key.
6. Release the CNTRL key.

### \<CNTRL-INIT\>

Generates a Bus Initialize without the processor starting.

1. Processor must be halted.
2. Press and hold down the CNTRL key.
3. Press and release the INIT key.
4. Bus Initialize will be generated for 150 ms.
5. Release the CNTRL key.

### \<CNTRL-7\>

Used to calculate the correct address when a mode 6 or 7 register R7 instruction is encountered.

1. Press and hold down the CNTRL key.
2. Press and release the 7 key.
3. Display will show the new temporary register which contains the old temporary register plus the Unibus address pointer plus 2.

See Paragraph 6.4 for an example.

### \<CNTRL-6\>

Used to calculate the offset address when mode 6 or 7 instructions other than register R7 are encountered.

1. Press and hold down the CNTRL key.
2. Press and release the 6 key.
3. Display shows the new temporary register, which contains the old temporary plus the switch register.
4. Release the CNTRL key.

See Paragraph 6.4 for an example.

### \<CNTRL-1\>

Used to enter the console into maintenance mode. Maintenance mode should only be used as an aid to troubleshooting hardware problems. Maintenance mode provides no help in debugging software problems.

1. Press and hold down the CNTRL key.
2. Press and release the 1 key.
3. MAINT indicator will be on and the MPC (microprogram counter) will be sampled and displayed.
4. Release the CNTRL key.

## 6.3 Notes on Operation

An erroneous display will result if, while the processor is running and the switch register is being displayed, a numeric key is pressed. Although the SR DISP indicator will remain on, the display no longer reflects the actual contents of the switch register. If at any time while the processor is running, it is desired that the switch register contents be displayed, the CNTRL-CONT keys should be used.

As a general practice, prior to entering a new 6-digit number and if the display is nonzero, the CLR key should be used to initially zero the display.

In order to single-instruction step the processor from a given starting address, the program counter (R7) must be loaded with the starting address using the Unibus address of R7 (777707) i.e., to single-instruction step from the beginning of a program starting at location 1000, the following sequence is necessary:

```
LAD 777707
DEP 1000
CNTRL-INIT (if desired)
HLT/SS
HLT/SS
etc.
```

The console requires an 18-bit address. This is especially important to remember when accessing device registers (i.e., 777560 instead of 177560). Otherwise an erroneous access to memory or to a nonexistent address will occur.

The Unibus addresses for the general-purpose registers can only be used by the console. A PDP-11 program using the Unibus addresses for the general-purpose registers will trap as a nonexistent address. Also, internal registers R10 through R17, which are used for various purposes (depending upon processor), may be accessed by the console through Unibus addresses 777710 through 777717.

The BUS ERR indicator on the console reflects a bus error by the console only. The indicator will not reflect bus errors due to other devices such as the processor.

## 6.4 Examples of Console Sequences

This section combines key operations with example sequences to demonstrate the proper use of the KY11-LB Programmer's Console.

The following sequences use the notations for key operations as described in Paragraph 6.2.

The angle brackets < > will be used in the sequences to identify the display contents after the operation is performed, i.e., LAD 200 \<0\>

### Example 1

This sequence uses the examine function and the switch register Unibus address 777570 to read the contents of the switch register.

```
LSR 123456     <123456>
LAD 777570     <0>
EXAM           <123456>
DIS AD         <777570>
LSR 777        <777>
EXAM           <777>
```

### Example 2

This sequence demonstrates the use of the following keys: LAD, DIS AD, LSR, EXAM, DEP, CNTRL-START, CNTRL-CONT, and CNTRL-HLT/SS.

This example loads the following program into memory, which is then run to demonstrate various operations.

```
Program Memory  Location/Contents

1000/13737      ;Move the contents of
1002/177570     ;the switch register to
1004/1014       ;memory location 1014
1006/0000       ;Halt
1010/137        ;Jump to location 1000
1012/1000
1014/0000
```

**Sequence:**

```
LAD 1000       <0>
DEP 13737      <13737>
DEP 177570     <177570>
DEP 1014       <1014>
DEP 0          <0>
DEP 137        <137>
DEP 1000       <1000>
DEP 0          <0>
LAD 1000       <0>
EXAM           <13737>
EXAM           <177570>
EXAM           <1014>
EXAM           <0>
EXAM           <137>
EXAM           <1000>
EXAM           <0>
DIS AD         <1014>
LSR 123456     <123456>
LAD 1000       <0>
CNTRL-START    <1010>
LAD 1014       <0>
EXAM           <123456>
DEP 0          <0>
EXAM           <0>
DIS AD         <1014>
LSR 125252     <125252>
CNTRL-CONT     <1010>
LAD 1014       <0>
EXAM           <125252>
DEP 0          <0>
LAD 1006       <0>
DEP 240        <240>
EXAM           <240>
LAD 1000       <0>
CNTRL-START    <125252>
LSR 70707      <10707>
CNTRL-HLT/SS
LAD 1014       <0>
EXAM           <10707>
HLT/SS
HLT/SS
HLT/SS
LSR 05252      <052525>
HLT/SS
LAD 1014       <0>
EXAM           <052525>
```

### Example 3

This sequence demonstrates the use of CNTRL-7 and CNTRL-6. The following data are loaded into memory:

```
1000/177
1002/100
1004/000
1006/5060
1010/1020

1104/1006

R0 = 777760
```

The sequence to load the data is as follows:

```
LAD 1000       <0>
DEP 177        <177>
DEP 100        <100>
DEP 0          <0>
DEP 5060       <5060>
DEP 1020       <1020>
LAD 1104       <0>
DEP 1006       <1006>
LAD 777700     <0>
DEP 777760     <777760>
```

**Sequence:**

```
LAD 1000       <0>
EXAM           <177>
EXAM           <100>
CNTRL-7        <1104>
LAD            <0>
EXAM           <1006>
LAD            <0>
EXAM           <5060>
EXAM           <1020>
LSR            <1020>
LAD 777700     <0>
EXAM           <777760>
CNTRL-6        <100000>
LAD            <0>
EXAM           <177>
```
