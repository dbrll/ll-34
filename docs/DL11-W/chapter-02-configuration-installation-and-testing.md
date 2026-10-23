# Chapter 2: Configuration, Installation, and Testing

> Addresses and vectors are octal. Switch numbers and pin numbers are decimal. The original contains several wiring and cross-reference inconsistencies; these are marked in source notes, not silently resolved. Verify the engineering drawings before wiring hardware.

<!-- Source PDF page 19; printed page 2-1 -->

## 2.1 Configuration

The DL11-W includes an M7856 quad module, either of two distribution panels (BC27C or H3009), and associated interconnecting cables. Also included is an adaptor bracket for use in cabinets which do not require compliance with FCC regulations for electromagnetic interference (EMI) suppression. Table 2-1 lists the option specific components. Figure 2-1 shows all of the parts associated with the DL11-W interface.

The M7856 quad module includes five dip-mounted switch packs. Each pack contains either eight or ten individual slide or toggle switches. The packs are labeled S1 through S5 on the board; each switch on the packs is numbered 1 through 8 or 10. Positions for on and off are clearly indicated on the hardware. “SX-Y” is the convention used in this manual to refer to specific switches where X indicates the switch pack number and Y indicates the particular switch on that switch pack. For example, “S2-9” refers to switch number 9 on switch pack 2.

Switch selections on the DL11-W interface provide the flexibility needed to handle a variety of functions. For example, the user can set up switches so that the DL11-W can interface to a Teletypewriter or to a high-speed CRT terminal. The user has a choice of speeds, character size, stop code length, parity, error detection, 20 mA current loop or EIA, addresses and vectors, active or passive modes, and the specific type of interface which the DL11-W is to replace.

### 2.1.1 Baud Rates

Table 2-2 lists the eight different baud rates available on the DL11-W. Completely independent split-speed operation is provided so that the receiver and transmitter may operate at different rates. The user should be careful to set the correct speeds when replacing other interface modules (DL11-A, DL11-B, and so on).

### 2.1.2 Address and Vector Selection

The DL11-W interface is addressed through the address selection logic, and its interrupt vector is determined by the interrupt control logic. Each DL11-W interface within a system has a unique address and a unique vector. These are determined by the switches on the module. However, the line clock address and vector are fixed at `777546` and `100`, respectively. Figure 2-2 shows the relation of specific switches to the address and vector of the device used (Teletypewriter and so on).

Thus, for address selection, switch S5-3 corresponds to address bit 10, and it indicates a logical 1 when turned off. For vector selection, on the other hand, switch S2-3 corresponds to vector bit 5, and it indicates a logical 1 when it is on.

All PDP-11 systems have enough I/O addresses reserved to handle up to 47 devices. Each one of these devices could be a DL11-W. However, only one DL11-W per system is allowed to have the LTC enabled. The LTC sections of other DL11-Ws can be disabled by turning switch S5-9 ON and S5-10 OFF. See Table 4-2 for more specific address configuration information.

<!-- Source PDF page 20; printed page 2-2 -->

![Figure 2-1 — DL11-W parts diagram](figures/fig-2-1-parts.png)

> Source note: The cable-length caution reference printed in Figure 2-1 is “3.5.2.1”; the relevant caution is in §2.3.2.1 below. The 7008519 external cable must be ordered separately.

<!-- Source PDF page 21; printed page 2-3 -->

**Table 2-1 — Option Configurations**

| Configuration | Module | Cable         | Distribution panel |
| ------------- | ------ | ------------- | ------------------ |
| DL11-W        | M7856  | None          | None               |
| DL11-WA       | M7856  | 7008360-9     | H3009              |
| DL11-WB       | M7856  | BC05C*; BC22E | BC27C              |

_A BC05C cable and BC27C panel comprise the BC27C cable/panel assembly._

**Table 2-2 — DL11-W Baud Rates**

| Baud rate | Transmit S4-10 | Transmit S3-1 | Transmit S3-4 | Receive S3-2 | Receive S3-3 | Receive S3-5 |
| --------- | -------------- | ------------- | ------------- | ------------ | ------------ | ------------ |
| 110       | ON             | ON            | ON            | OFF          | OFF          | OFF          |
| 150       | OFF            | ON            | ON            | ON           | OFF          | OFF          |
| 300       | ON             | OFF           | OFF           | OFF          | ON           | ON           |
| 600       | ON             | OFF           | ON            | OFF          | ON           | OFF          |
| 1200      | ON             | ON            | OFF           | OFF          | OFF          | ON           |
| 2400      | OFF            | OFF           | OFF           | ON           | ON           | ON           |
| 4800      | OFF            | OFF           | ON            | ON           | ON           | OFF          |
| 9600      | OFF            | ON            | OFF           | ON           | OFF          | ON           |

![Figure 2-2 — Address and vector selection; address 1 = OFF, vector 1 = ON](figures/fig-2-2-address-vector.png)

<!-- Source PDF page 22; printed page 2-4 -->

### 2.1.3 Address Selection Modes

The DL11-W can be operated in any of three different address selection modes. Normally, a DL11-W used as console terminal control would operate in the first mode, whereas additional DL11s would be operated in the second mode. The third mode is not normally used, but is discussed here for completeness.

- **Mode 1:** Both the serial line unit and the line clock sections can be addressed. Due to common address selection logic, operation in this mode requires that the serial line unit addresses be restricted to `77756X`. The line clock address is `777546`.
- **Mode 2:** Only the serial line unit section can be addressed. Address selection ranges from `774000` to `777776`. The line clock is disabled and does not respond to address `777546`.
- **Mode 3:** Only the line clock section can be addressed at `777546`. The serial line unit section does not respond to any address.

Table 2-3 indicates the correct switch setting for selection of the desired address and address mode.

**Table 2-3 — Address and Mode Selection**

| Mode | A10 S5-3 | A09 S5-2 | A08 S5-1 | A07 S5-4 | A06 S5-5 | A05 S5-6 | A04 S5-8 | A03 S5-7 | LTC S5-9 | LTC S5-10 |
| ---- | -------- | -------- | -------- | -------- | -------- | -------- | -------- | -------- | -------- | --------- |
| 1    | OFF      | OFF      | OFF      | ON       | OFF      | OFF      | OFF      | ON       | OFF      | ON        |
| 2*   | OFF      | OFF      | OFF      | ON       | OFF      | OFF      | OFF      | ON       | ON       | OFF       |
| 3    | OFF      | OFF      | OFF      | ON       | OFF      | OFF      | ON       | ON       | ON       | ON        |

_Address `77756X` is selected for the serial line interface. Other addresses may be selected using switches shown in Figure 2-2, where OFF = 1 and ON = 0._

### 2.1.4 Active and Passive Modes

Two switch-selectable modes of operation are available for the 20 mA current loop. In the active mode, the DL11-W is the source for the 20 mA of current; in the passive mode, the external device must provide the current. As an example, two processing systems could be connected using two DL11-Ws via the 20 mA current loop. One DL11-W would be the active device. The other DL11-W would be passive. Table 2-4 shows the appropriate switch settings. Normal configuration is in the active mode.

### 2.1.5 Data Format

The data format (Figure 2-3) consists of a START bit, five to eight DATA bits, a PARITY bit or no PARITY bit, and one, one and one-half, or two STOP bits.

When less than eight DATA bits are selected, the hardware justifies the bits into the least significant bit positions for characters received by the interface. When transmitting characters, the program provides the justification into the least significant bits. The PARITY bit may be either on or off; when on, it can be selected for checking either odd or even parity when receiving and for providing an extra PARITY bit during transmission.

<!-- Source PDF page 23; printed page 2-5 -->

**Table 2-4 — Switch Settings**

**Transmitter**

| Mode    | S1-1 | S1-2 | S1-3 | S1-6 | S1-7 |
| ------- | ---- | ---- | ---- | ---- | ---- |
| Active  | ON   | ON   | OFF  | OFF  | ON   |
| Passive | OFF  | OFF  | ON   | ON   | OFF  |

**Receiver**

| Mode    | S3-6 | S3-7 | S3-8 | S3-9 | S3-10 |
| ------- | ---- | ---- | ---- | ---- | ----- |
| Active  | ON   | OFF  | ON   | OFF  | ON    |
| Passive | OFF  | ON   | OFF  | ON   | OFF   |

**Paper Tape Reader Enable**

| Mode    | S1-4 | S1-5 | S1-8 | S1-9 | S1-10 |
| ------- | ---- | ---- | ---- | ---- | ----- |
| Active  | ON   | OFF  | ON   | OFF  | ON    |
| Passive | OFF  | ON   | OFF  | ON   | OFF   |

![Figure 2-3 — DL11-W data format](figures/fig-2-3-data-format.png)

All variable items within any data format are selected by switches on the DL11-W module. None of the variables can be controlled by the program. These switches are listed in Table 2-5 and described more fully in Chapter 4.

Figure 2-4 shows typical switch settings for a DL11-W when interfacing with a standard DIGITAL terminal (console device only).

Table 2-6 gives a complete listing of the switches and their functions.

### 2.1.6 Cabling

Figure 2-5 illustrates the proper cabling configuration for selecting and connecting cables between the DL11-W and various peripheral devices.

## 2.2 Preinstallation and Set-Up Procedures

Before installing the DL11-W, assign device and vector addresses in accordance with Section 2.1.2.

<!-- Source PDF page 24; printed page 2-6 -->

**Table 2-5 — Data Format Switches**

| Name                | Switch     | UART pin | Function                                                                                                                                                                                                                                                                                                |
| ------------------- | ---------- | -------- | ------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- |
| No Parity           | S4-6       | 35       | Enables or disables the parity bit in the data character. When enabled, its value depends on the odd/even parity selected by S4-2. When disabled, STOP bits immediately follow the last DATA bit during transmission; the receiver does not check parity. **ON: parity enabled. OFF: parity disabled.** |
| Even Parity         | S4-2       | 39       | Determines whether odd or even parity is used. The receiver checks the incoming character for appropriate parity; the transmitter inserts the appropriate parity value. **ON: odd parity. OFF: even parity.**                                                                                           |
| STOP Bit            | S4-5       | 36       | Selects the number of stop bits. **ON: one STOP bit. OFF: two STOP bits**, except that five DATA bits select one and one-half STOP bits.                                                                                                                                                                |
| Number of DATA bits | S4-3; S4-4 | 38; 37   | Used together to select the number of DATA bits, as below.                                                                                                                                                                                                                                              |

| S4-4 | S4-3 | Number of DATA bits |
| ---- | ---- | ------------------- |
| ON   | ON   | 5                   |
| ON   | OFF  | 6                   |
| OFF  | ON   | 7                   |
| OFF  | OFF  | 8                   |

<!-- Source PDF page 25; printed page 2-7 -->

![Figure 2-4 — Typical switch settings for standard DEC console terminals; N = ON, F = OFF, dash = unused](figures/fig-2-4-switch-settings.png)

**Table 2-6 — DL11-W Switch Functions**

| Switch pack | Switch number(s) | Function                                          |
| ----------- | ---------------- | ------------------------------------------------- |
| 1           | 1, 2, 3          | Transmitter (active/passive mode of 20 mA loop)   |
| 1           | 4, 5             | Reader enable (active/passive mode of 20 mA loop) |
| 1           | 6, 7             | Transmitter (active/passive mode of 20 mA loop)   |
| 1           | 8, 9, 10         | Reader enable (active/passive mode of 20 mA loop) |
| 2           | 1, 2             | Not functional                                    |
| 2           | 3, 4, 5, 6, 7, 8 | Vector address                                    |

<!-- Source PDF page 26; printed page 2-8 -->

**Table 2-6 — DL11-W Switch Functions (continued)**

| Switch pack | Switch number(s)       | Function                                     |
| ----------- | ---------------------- | -------------------------------------------- |
| 3           | 1                      | Transmitter baud rate                        |
| 3           | 2, 3                   | Receiver baud rate                           |
| 3           | 4                      | Transmitter baud rate                        |
| 3           | 5                      | Receiver baud rate                           |
| 3           | 6, 7, 8, 9, 10         | Receiver (active/passive mode of 20 mA loop) |
| 4           | 1                      | Break enable                                 |
| 4           | 2                      | Parity select (odd or even)                  |
| 4           | 3, 4                   | Number of DATA bits                          |
| 4           | 5                      | Number of STOP bits                          |
| 4           | 6                      | Parity enable                                |
| 4           | 7                      | Error bit enable                             |
| 4           | 8, 9                   | Not functional                               |
| 4           | 10                     | Transmitter baud select                      |
| 5           | 1, 2, 3, 4, 5, 6, 7, 8 | Device address                               |
| 5           | 9, 10                  | Line clock enable                            |

<!-- Source PDF page 27; printed page 2-9 -->

![Figure 2-5 — DL11-W cable connections, EIA and 20 mA current loop](figures/fig-2-5-cable-connections.png)

## 2.3 Installation

This section identifies the installation procedures for the DL11-W. Installation is broken down into M7856 module installation and distribution panel installation.

> **WARNING — When performing any installation procedures, turn all power OFF.**

### 2.3.1 M7856 Module Installation

The DL11-W can be installed in any small peripheral controller (SPC) slot of the PDP-11 processor. Figure 2-6 illustrates a typical 9-slot backplane configuration (DD11-D).

1. Plug the female Berg connector (P-1) of the desired internal cable (see Figure 2-5) into the Berg connector (J1) of the M7856 module. The Berg connector is shown in Figure 1-1.
2. Install the M7856 module into the system unit.
3. Perform resistance checks between the backplane voltage sources and ground to ensure that no short circuit conditions exist on the M7856 module. Refer to the engineering print set _DL11-W SLU/RTC Option Engineering Drawings_ for pin assignments.
4. Proceed to Section 2.3.2.

> Source note: Step 1 prints “P-1” for the cable's Berg connector. Figure 2-5 and Tables 2-9 and 2-11 instead identify the module-side Berg connector as P2. Check the actual cable and drawings before connecting it.

### 2.3.2 Distribution Panel Installation

Because the installation procedures for installing the BC27C and H3009 distribution panels are similar, the following instructions pertain to both panels.

<!-- Source PDF page 28; printed page 2-10 -->

![Figure 2-6 — DD11-D backplane, module-side view and bus grant direction](figures/fig-2-6-dd11-d.png)

Two different approaches for installing distribution panel assemblies are included in this manual.

Most new installations utilize I/O bulkheads to comply with FCC regulations limiting EMI leakage. For installations utilizing an I/O bulkhead, follow the steps outlined in Section 2.3.2.1.

Alternate instructions are included for those cabinets that do not require I/O bulkheads and thus require a slightly modified installation procedure. If the system does not incorporate an I/O bulkhead, follow the steps outlined in Section 2.3.2.2.

#### 2.3.2.1 Installation in Cabinets with an I/O Bulkhead

Though there may be differences in the positioning of the I/O bulkheads of the PDP-11 kernel cabinet, the universal expansion cabinet, and other cabinets, the installation concept is the same. Once the BC27C or H3009 distribution panel is installed, there should be no openings left (panels omitted) in the I/O frame on the rear of the cabinet which could permit EMI leakage. For this reason, it is important to tighten all mounting screws in the distribution panel. Figures 2-7 and 2-8 show the various I/O bulkhead types and illustrate the correct approach to each.

**1.** Gain access to the I/O bulkhead through the door on the rear of the system cabinet and remove one of the 4.57 cm (2 in) wide panels from the bulkhead. This is where the distribution panel is mounted.

> Source note: The original prints “4.57 cm (2 in)” here and in Step 4. The units disagree: 2 in = 5.08 cm. The original dimension is retained; confirm the physical panel dimensions.

<!-- Source PDF page 29; printed page 2-11 -->

![Figure 2-7 — H3009 installation in a horizontally oriented I/O bulkhead](figures/fig-2-7-horizontal-bulkhead.png)

<!-- Source PDF page 30; printed page 2-12 -->

![Figure 2-8 — BC27C installation in a vertically oriented I/O bulkhead](figures/fig-2-8-vertical-bulkhead.png)

**2.** When using the 7008360-9 cable and H3009 panel, plug the connector (P-2) of the free end of the cable into the male connector on the rear of the H3009 panel. When using the BC27C cable/panel assembly, this step may be omitted since the cable and panel are already connected.

**3.** Route the remaining internal cable and distribution panel through the cabinet and through the opening in the I/O bulkhead at the rear of the cabinet. Keep in mind that the cable must be routed and dressed in a manner compatible with existing cabinet cabling.

<!-- Source PDF page 31; printed page 2-13 -->

**4.** Install the distribution panel into the opening of the I/O bulkhead (see Figures 2-7 and 2-8) in place of the 4.57 cm (2 in) wide panel that was removed in Step 1.

> **NOTE** — It is necessary to maintain an interference-free environment outside the cabinet enclosure. Any additional panels that may have been removed to facilitate easier installation of the distribution panel must be replaced.

**5.** Connect the correct external cable to the connector on the rear of the distribution panel (see Section 2.1.6). The cable should exit the cabinet with the other signal cables.

> **CAUTION** — BC22E cable lengths in excess of 7.62 m (25 feet) may violate the maximum capacitance allowed by the RS-232-C specification. Note, however, that up to 15 m (50 feet) provides satisfactory DL11-W performance levels.

**6.** Connect the other end of the external cable to the connector on the peripheral device.

**7.** Turn the power ON.

#### 2.3.2.2 Installation in Cabinets Without an I/O Bulkhead

**1.** Gain access to the rear of the system cabinet and mount the adaptor bracket (Part No. 74-27292) to one of the rear vertical mounting rails as shown in Figure 2-9. Mounting the bracket on either side of the cabinet is permissible.

**2.** When using the 7008360-9 cable and H3009 panel, plug the connector (P-2) of the free end of the cable into the male connector on the rear of the H3009 panel. When using the BC27C cable/panel assembly, this step may be omitted since the cable and panel are already connected.

**3.** Route the remaining internal cable and distribution panel through the cabinet and through the adaptor bracket at the rear of the cabinet. Keep in mind that the cable must be routed and dressed in a manner compatible with existing cabinet cabling.

**4.** Connect the external cable to the connector on the rear of the distribution panel (see Section 2.1.6). The cable should exit the cabinet with the other signal cables.

> **CAUTION** — BC22E cable lengths in excess of 7.62 m (25 feet) may violate the maximum capacitance allowed by the RS-232-C specification. Note, however, that up to 15 m (50 feet) provides satisfactory DL11-W performance levels.

**5.** Connect the other end of the external cable to the connector on the peripheral device.

**6.** Turn the power ON.

<!-- Source PDF page 32; printed page 2-14 -->

![Figure 2-9 — BC27C panel installed in an adaptor bracket](figures/fig-2-9-adaptor-bracket.png)

## 2.4 Pin Interconnection

Table 2-7 lists the signal names and associated pins on the Berg connector mounted on the M7856 module. This table also lists the signals supplied on the 7008360-9/H3009 and BC27C cables.

Table 2-8 provides a quick reference of M7856 input/output signals for TTL, EIA, and 20 mA current loop devices.

Table 2-9 lists connector pin numbers and signals for the 7008360-9 cable.

Table 2-10 lists connector pin numbers and signals for the 7008519 external cable which is used in conjunction with the 7008360-9/H3009 assembly cable.

Table 2-11 lists connector pin numbers for the BC27C cable connectors.

<!-- Source PDF page 33; printed page 2-15 -->

**Table 2-7 — Pin Connections**

Blank entries in the source are represented by a dash; cable signals do not imply that the M7856 implements every modem-control function.

| Berg pin | M7856 module              | BC27C modem cable         | 7008360-9 cable   |
| -------- | ------------------------- | ------------------------- | ----------------- |
| A        | Ground                    | Ground                    | Ground            |
| B        | Ground                    | Ground                    | —                 |
| C        | —                         | Force Busy                | —                 |
| D        | —                         | Secondary Clear to Send   | —                 |
| E        | Serial Input (TTL)        | Interlock In              | Interlock In      |
| F        | Serial Output (EIA)       | Transmitted Data          | —                 |
| H        | 20 mA Interlock           | —                         | Interlock Out     |
| J        | Serial Input (EIA)        | Received Data             | —                 |
| K        | +Serial Input (20 mA)     | —                         | +Received Data    |
| L        | —                         | External Clock            | —                 |
| M        | EIA Interlock             | Interlock Out             | —                 |
| N        | —                         | Serial Clock Xmit         | —                 |
| P        | —                         | Secondary Request to Send | —                 |
| R        | —                         | Serial Clock Receiver     | —                 |
| S        | −Serial Input (20 mA)     | —                         | −Received Data    |
| T        | —                         | Clear to Send             | —                 |
| U        | —                         | —                         | —                 |
| V        | Request to Send (EIA)     | Request to Send           | —                 |
| W        | —                         | −Power                    | —                 |
| X        | —                         | Ring                      | —                 |
| Y        | —                         | +Power                    | —                 |
| Z        | —                         | Data Set Ready            | —                 |
| AA       | +Serial Output (20 mA)    | —                         | +Transmitted Data |
| BB       | —                         | Carrier                   | —                 |
| CC       | —                         | —                         | —                 |
| DD       | Data Terminal Ready (EIA) | Data Terminal Ready       | —                 |
| EE       | −Reader Run (20 mA)       | —                         | −Reader Run       |
| FF       | —                         | 202 Secondary Transmit    | —                 |
| HH       | —                         | —                         | —                 |
| JJ       | —                         | 202 Secondary Receive     | —                 |
| KK       | −Serial Output (20 mA)    | —                         | −Transmitted Data |
| LL       | —                         | EIA Secondary Transmit    | —                 |
| MM       | —                         | Signal Quality            | —                 |
| NN       | —                         | EIA Secondary Receive     | —                 |
| PP       | +Reader Run (20 mA)       | —                         | +Reader Run       |
| RR       | —                         | Signal Rate               | —                 |
| SS       | —                         | —                         | —                 |
| TT       | +5 V                      | —                         | —                 |
| UU       | Ground                    | Ground                    | Ground            |
| VV       | Ground                    | Ground                    | Ground            |

<!-- Source PDF page 34; printed page 2-16 -->

**Table 2-8 — Input/Output Signals — M7856**

| Type               | Direction | Signal              | Pin |
| ------------------ | --------- | ------------------- | --- |
| TTL                | Input     | Serial Data         | E   |
| 20 mA current loop | Input     | +Serial Data        | K   |
| 20 mA current loop | Input     | −Serial Data        | S   |
| 20 mA current loop | Output    | +Serial Data        | AA  |
| 20 mA current loop | Output    | −Serial Data        | KK  |
| 20 mA current loop | Output    | +Reader Run         | PP  |
| 20 mA current loop | Output    | −Reader Run         | EE  |
| EIA                | Input     | Serial Data         | J   |
| EIA                | Output    | Serial Data         | F   |
| EIA                | Output    | Request to Send     | V   |
| EIA                | Output    | Data Terminal Ready | DD  |

**Table 2-9 — 7008360-9 Connections**

| Twisted pair | Color | Mate-N-Lok connector P1 (to device) | Berg connector P2 (to DL11) | Signal            |
| ------------ | ----- | ----------------------------------- | --------------------------- | ----------------- |
| Black/Red    | Black | 2                                   | KK                          | −Transmitted Data |
| Black/Red    | Red   | 3                                   | S                           | −Received Data    |
| Black/White  | Black | 3 [sic]                             | EE                          | −Reader Run       |
| Black/White  | White | 5                                   | AA                          | +Transmitted Data |
| Black/Green  | Black | 6                                   | PP                          | +Reader Run       |
| Black/Green  | Green | 7                                   | K                           | +Received Data    |
| —            | —     | —                                   | E                           | Interlock In      |
| —            | —     | —                                   | H                           | Interlock Out     |

**Notes:**

1. Connector on ASR Teletype uses all pins (2–7).
2. Connector on KSR Teletype does not use pins 4 or 6 (Reader Run, − and +).

> Source note: The scan really prints P1 pin **3** for −Reader Run, although its own Note 2 identifies reader-run pins as **4 and 6**. The inconsistency is retained, not a recommended wiring connection. Check the cable drawing before wiring pin EE.

<!-- Source PDF page 35; printed page 2-17 -->

**Table 2-10 — 7008519 Connections**

| 7008360-9 Mate-N-Lok P1 | Mate-N-Lok P2 (to 7008360-9) | Color | Mate-N-Lok P1 (to device) | Signal            |
| ----------------------- | ---------------------------- | ----- | ------------------------- | ----------------- |
| 2                       | 2                            | Black | 2                         | −Transmitted Data |
| 3                       | 3                            | Red   | 3                         | −Received Data    |
| 5                       | 5                            | White | 5                         | +Transmitted Data |
| 7                       | 7                            | Green | 7                         | +Received Data    |

**Table 2-11 — BC27C Connections**

| Color        | Cinch connector P1 (to device) | Berg connector P2 (to DL11) | Signal                    |
| ------------ | ------------------------------ | --------------------------- | ------------------------- |
| Blue/White   | 1                              | A; VV                       | Ground                    |
| White/Blue   | 2                              | F                           | Transmitted Data          |
| Orange/White | 3                              | J                           | Received Data             |
| White/Orange | 4                              | V                           | Request to Send           |
| Green/White  | 5                              | T                           | Clear to Send             |
| White/Green  | 6                              | Z                           | Data Set Ready            |
| Brown/White  | 7                              | B; UU                       | Ground                    |
| White/Brown  | 8                              | BB                          | Carrier                   |
| Slate/White  | 9                              | Y                           | +Power                    |
| White/Slate  | 10                             | W                           | −Power                    |
| Blue/Red     | 11                             | FF                          | 202 Secondary Transmit    |
| Red/Blue     | 12                             | JJ                          | 202 Secondary Receive     |
| Orange/Red   | 13                             | D                           | Secondary Clear to Send   |
| Slate/Red    | 14                             | LL                          | EIA Secondary Transmit    |
| Slate/Green  | 15                             | N                           | Serial Clock Transmit     |
| Red/Brown    | 16                             | NN                          | EIA Secondary Receive     |
| Slate        | 17                             | R                           | Serial Clock Receive      |
| Red/Slate    | 18                             | O [sic]                     | Unassigned                |
| Blue/Black   | 19                             | P                           | Secondary Request to Send |
| Black/Blue   | 20                             | DD                          | Data Terminal Ready       |
| Orange/Black | 21                             | MM                          | Signal Quality            |
| Black/Orange | 22                             | X                           | Ring                      |
| Green/Black  | 23                             | RR                          | Signal Rate               |
| Brown/Red    | 24                             | L                           | External Clock            |
| Red/Orange   | 25                             | C                           | Force Busy                |
| —            | —                              | E                           | Interlock In              |
| —            | —                              | M                           | Interlock Out             |

> Source note: The source prints **O** for the unassigned Cinch pin 18, whereas Table 2-7's Berg pin sequence contains **U**, not O. This unresolved source discrepancy is retained for verification against the cable drawings.

<!-- Source PDF page 36; printed page 2-18 -->

## 2.5 Installation Testing

Installation testing is performed by running the diagnostic programs after the DL11-W interface has been completely installed. The diagnostic programs and their operating instructions are supplied with the DL11-W interface.

The diagnostics supplied with the interface are:

1. `MD-11-CZDLA**`
2. `MD-11-CZDLB**`
3. `DEC/X11-CXDLA**`

Make three error-free passes of each diagnostic to ensure proper DL11 operation.
