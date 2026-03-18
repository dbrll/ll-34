# Chapter 5 -- Extended Addressing

## 5.1 General

This chapter applies to use of the M9301-YA, -YB, -YE, -YF, and -YJ in PDP-11 systems which have no console. When the memory of a PDP-11 system is extended beyond 28K, the processor is able to access upper memory through the memory management system. However, the console emulator normally allows the user to access only the lower 28K of memory. This chapter provides an explanation of the method by which the user can gain access to upper memory in order to read or modify the contents of any location. The reader should be familiar with the concepts of memory management in the KD11-E processor.

## 5.2 Virtual and Physical Addresses

Addresses generated in the processor are called virtual addresses, and will be 16 bits in length. Physical addresses refer to actual locations in memory. They are asserted on the Unibus and may be up to 18 bits in length (for 128K memories).

## 5.3 Address Mapping Without Memory Management

With memory management disabled (as is the case following depression of the boot switch), a simple hardware mapping scheme converts virtual addresses to physical addresses. Virtual addresses in the 0 to 28K range are mapped directly into physical addresses in the range from 0 to 28K. Virtual addresses on the I/O page, in the range from 28K to 32K (160000=177776), are mapped into physical addresses in the range from 124K to 128K.

## 5.4 Address Mapping with Memory Management

With memory management enabled, a different mapping scheme is used. In this scheme, a relocation constant is added to the virtual address to create a physical or "relocated" address.

Virtual address space consists of eight 4K banks where each bank can be relocated by the relocation constant associated with that bank. The procedure specified in this section allows the user to:

1. Create a virtual address to type into the load address command.
2. Determine the relocation constant required to relocate the calculated virtual address into the desired physical address.
3. Enable or disable the memory management hardware.

## 5.5 Creation of a Virtual Address

The easiest way to create a virtual address is to divide the 18-bit physical address into two separate fields -- a virtual address and a physical bank number. The virtual address is represented by the lower 13 bits and the physical bank by the upper 5 bits. The lower 3 bits of the physical bank number (bits 13, 14, 15) represent the virtual bank number (Table 5-1). Thus if bits 13, 14, and 15 are all 0s, the virtual bank selected is 0. The user should calculate the relocation constant according to Table 5-2. He can then deposit this constant in the relocation register associated with virtual bank 0 (Table 5-1).

**Table 5-1 Unibus Address Assignments**

| Virtual Address | Virtual Bank | Relocation Register | Descriptor Register |
|---|---|---|---|
| 160000-177776 | 7 | 172356 | 172316 |
| 140000-157776 | 6 | 172354 | 172314 |
| 120000-137776 | 5 | 172352 | 172312 |
| 100000-117776 | 4 | 172350 | 172310 |
| 060000-077776 | 3 | 172346 | 172306 |
| 040000-057776 | 2 | 172344 | 172304 |
| 020000-037776 | 1 | 172342 | 172302 |
| 000000-017776 | 0 | 172340 | 172300 |

**Table 5-2 Relocation Constants**

| Physical Bank Number | Relocation Constant | Physical Bank Number | Relocation Constant |
|---|---|---|---|
| 37 | 007600 | 17 | 003600 |
| 36 | 007400 | 16 | 003400 |
| 35 | 007200 | 15 | 003200 |
| 34 | 007000 | 14 | 003000 |
| 33 | 006600 | 13 | 002600 |
| 32 | 006400 | 12 | 002400 |
| 31 | 006200 | 11 | 002200 |
| 30 | 006000 | 10 | 002000 |
| 27 | 005600 | 7 | 001600 |
| 26 | 005400 | 6 | 001400 |
| 25 | 005200 | 5 | 001200 |
| 24 | 005000 | 4 | 001000 |
| 23 | 004600 | 3 | 000600 |
| 22 | 004400 | 2 | 000400 |
| 21 | 004200 | 1 | 000200 |
| 20 | 004000 | 0 | 000000 |

The memory management logic also provides various forms of protection against unauthorized access. The corresponding descriptor register must be set up along with the relocation register to allow access anywhere within the 4K bank.

For example, assume a user wishes to access location 533720(8). The normal access capability of the console is 0 to 28K. This address (533720) is between the 28K limit and the I/O page (760000-777776), and consequently must be accessed as a relocated virtual address, with memory management enabled. The virtual address is 13720 in physical bank 25 and is derived as follows.

All locations in bank 25 may be accessed through the virtual addresses 000000-017776. The relocation and descriptor registers in the processor are still accessible since their addresses are within the I/O page. (Note that access to the I/O page is not automatically relocated with memory management, while access to the I/O page is automatically relocated when memory management is enabled.)

The relocation constant for physical bank 25 is 005200. This constant is added in the relocation unit to the virtual address, as shown, yielding 533720.

|   | Octal |
|---|---|
| Virtual address | 013720 |
| Relocated constant (Table 5-2) | 520000 |
| Physical address | 533720 |

The Unibus addresses of the relocation registers and the descriptor registers are given in Table 5-1. The relocation constant to be loaded into the relocation register for each 4K bank is provided in Table 5-2. The data to be loaded in the descriptor register to provide read/write access to the full 4K is always 077406.

The Unibus address of the control register to enable memory management is 177572. This register is loaded with the value 000001 to enable memory management, and loaded with 0 to disable it.

To complete the example previously described (accessing location 533720), the console routine would be as follows:

```
$L      172340          /Access relocation register for virtual bank 0
$D      5200            /Deposit code for physical bank 25.
$L      172356          /Access relocation register for virtual bank 7.
$D      7600            /Deposit code for the I/O page.
$L      172300          /Access descriptor register, virtual bank 0.
$D      77406           /Deposit code for read/write access to 4K.
$L      172316          /Access descriptor register, virtual bank 7.
$D      77406           /Deposit code for read/write access to 4K.
$L      177572          /Access control register.
$D      1               /Enable memory management.
$L      13720           /Load virtual address of location desired.
$E                      /Examine the data in location 533720.
                        /Data will be displayed.
```

## 5.6 Constraints

Loading a new relocation constant into the relocation register for virtual bank 0 will cause virtual addresses 000000-017776 to access the new physical bank. A second bank can be made accessible by loading the relocation constant and descriptor data into the relocation and descriptor registers for virtual bank 1 and accessing the location through virtual address 020000-037776. Seven banks are accessible in this manner, by loading the proper constants, setting up the descriptor data, and selecting the proper virtual address. Bank 7 (I/O page) must remain relocated to physical bank 37 as it is accessed by the CPU to execute the console emulator routine.

Memory management is disabled by clearing (loading with 0s) the control register 177572. It should always be disabled prior to typing a boot command.

The start command automatically disables memory management and the CPU begins executing at the physical address corresponding to the address specified by the previous load address command. Pressing the boot switch automatically disables memory management. The contents of the relocation registers are not modified.

The HALT/CONT switch has no effect on memory management.
