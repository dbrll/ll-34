# Chapter 7 -- Maintenance Mode Operation

## 7.1 Introduction

This chapter covers the keypad facilities of the programmer's console available for hardware maintenance of the processor.

## 7.2 Maintenance Mode Key Operations

The following definitions apply to a subset of the same keys used in console mode; however the functions and operations differ from those in console mode. In general, console mode functions are not available while in maintenance mode, and many keys have no function in maintenance mode.

> **NOTE:** Maintenance mode operation is indicated by the MAINT indicator being on.

In order to use the hardware maintenance features available in maintenance mode, the maintenance cable (11/04) or cables (11/34) must be connected between the KY11-LB interface board (M7859) and the corresponding processor board (M7263-11/04, M7266-11/34, M8266-11/34A, or M8267-FP11A). An exception to this is the 5 (maintenance mode) operation which allows the console to examine or deposit into memory or device registers without the processor being either present or functional.

### DIS AD (Maintenance Mode)

Used to display Unibus address lines.

1. Press and release the DIS AD key.
2. Unibus address lines will be sampled (read once) and displayed, i.e., display will not be updated as address lines change.

### EXAM (Maintenance Mode)

Used to display Unibus data lines.

1. Press and release the EXAM key.
2. Unibus data lines will be sampled and displayed.

### HLT/SS (Maintenance Mode)

Asserts manual clock enable and displays MPC (microprogram counter).

1. Press and release the HLT/SS key.
2. Manual clock enable will be asserted.
3. MPC will be sampled and displayed.

### CONT (Maintenance Mode)

Single microsteps the processor through one microstate and displays the MPC.

1. Press and release the CONT key.
2. Manual clock will be pulsed.
3. New MPC will be sampled and displayed.

### BOOT (Maintenance Mode)

Boots the M9301. If manual clock enable is asserted, the M9301 routine will not be entered but because the M9301 simulates a power fail the processor will power up through location 24.

1. Press and release the BOOT key.
2. The display is not affected. If manual clock enable is asserted, the MPC is now at the beginning of the power-up sequence. To see the new MPC, use the HLT/SS key.

### START (Maintenance Mode)

Drops manual clock enable.

1. Press and release the START key.
2. Manual clock enable is released.
3. MPC will be sampled and displayed.

### CLR (Maintenance Mode)

Returns console to console mode operation.

1. Press and release the CLR key.
2. MAINT indicator is off.
3. Processor should halt.
4. Program counter should be displayed.

### 5 (Maintenance Mode)

Allows the console to take control of the Unibus if a processor is not in the system.

1. Press and release the 5 key.
2. The MAINT indicator will be off (console mode operation now).
3. Console attempts to read the program counter which is not present and therefore the BUS ERR indicator will be on.

## 7.3 Notes on Operation

If the single-microstep feature in maintenance mode is to be used, it is preferable that the processor be halted prior to entering maintenance mode, if it is possible. This is because the assertion of manual clock enable, which turns off the processor clock if it is running, cannot be synchronized with the processor clock. Therefore, if the processor is not halted, the clock may be running and the assertion of manual clock enable may cause an erroneous condition to occur.

In order to single-microstep the processor from the beginning of the power-up sequence, the following steps may be used:

1. Halt the processor if possible.
2. Use CNTRL-1 to enter maintenance mode.
3. Use HLT/SS to assert manual clock enable (RUN indicator should come on).
4. Use BOOT to generate a simulated power-fail (will not work if M9301 is not present in the system).
5. Use HLT/SS to display the MPC (microprogram counter) for the first microstep in the power-up routine.
6. Use CONT to single-microstep the processor through the power-up routine. (The new MPC will be displayed at each step.)
7. Unibus address lines and Unibus data (see NOTE below) lines may be examined at any microstep by using DIS AD and EXAM, respectively. Use of these keys does not advance the microprogram. To redisplay the current MPC without advancing the microprogram, use the HLT/SS key.
8. To return from maintenance mode, use the CLR key.
9. To single-microstep through a program, the program counter (R7) must first be loaded with the starting address of the program as in single-instruction stepping the processor prior to entering maintenance mode.

> **NOTE:** Because the data transfer occurs asynchronously with the processor clock, Unibus data will not be displayed on DATI in maintenance mode when using the console with an 11/04 processor. Unibus data on DATO on the 11/04 and both DATI and DATO on the 11/34 will be displayed.

> Due to hardware changes, the M8266 module will gate the AMUX lines onto the Unibus when manual clock enable is asserted and a Unibus transaction is not occurring.
