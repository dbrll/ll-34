# Appendix B -- IC Descriptions

## 7493 Binary Counter

The 7493 is a 4-bit binary counter with separate clock inputs and reset capability. When used as a 4-bit ripple-through counter, output RO(1) is connected to input CLK0.

**Truth Table** (4-bit ripple-through counter):

| Input Pulse | R0(1) | R1(1) | R2(1) | R3(1) |
|---|---|---|---|---|
| 0 | 0 | 0 | 0 | 0 |
| 1 | 1 | 0 | 0 | 0 |
| 2 | 0 | 1 | 0 | 0 |
| 3 | 1 | 1 | 0 | 0 |
| 4 | 0 | 0 | 1 | 0 |
| 5 | 1 | 0 | 1 | 0 |
| 6 | 0 | 1 | 1 | 0 |
| 7 | 1 | 1 | 1 | 0 |
| 8 | 0 | 0 | 0 | 1 |
| 9 | 1 | 0 | 0 | 1 |
| 10 | 0 | 1 | 0 | 1 |
| 11 | 1 | 1 | 0 | 1 |
| 12 | 0 | 0 | 1 | 1 |
| 13 | 1 | 0 | 1 | 1 |
| 14 | 0 | 1 | 1 | 1 |
| 15 | 1 | 1 | 1 | 1 |

**Notes:**
1. Truth table applies when 7493 is used as 4-bit ripple-through counter.
2. Output RO(1) connected to input CLK0.
3. To reset all outputs to logical 0, both pins 02 and 03 inputs must be high.
4. Either (or both) reset inputs RO(1) (pins 02 and 03) must be low to count.

**Pinout:** VCC = PIN 05, GND = PIN 10

## 8641 Quad Bus Transceiver

The 8641 consists of four identical receiver/drivers and a single enabling gate in one package for interfacing with the PDP-11 Unibus. The transceiver drivers are enabled when ENABLE A and ENABLE B are both low. The other input of each driver is connected to the data to be sent to the Unibus. For example, when enabled, DATA IN 1 (pin 2) is read to the Unibus via BUS 1 (pin 1). During a write operation, data comes from the Unibus as BUS 1 (pin 1) and is passed through the receiver to the device as DATA OUT 1 (pin 3).

**Pinout:**

| Pin | Signal | Pin | Signal |
|---|---|---|---|
| 1 | BUS 1 | 16 | VCC |
| 2 | DATA IN 1 | 15 | BUS 4 |
| 3 | DATA OUT 1 | 14 | DATA IN 4 |
| 4 | BUS 2 | 13 | DATA OUT 4 |
| 5 | DATA IN 2 | 12 | BUS 3 |
| 6 | DATA OUT 2 | 11 | DATA IN 3 |
| 7 | ENABLE A | 10 | DATA OUT 3 |
| 8 | GROUND | 9 | ENABLE B |

## 74154 4-Line to 16-Line Decoder

The 74154 4-Line to 16-Line Decoder decodes four binary-coded inputs into one of 16 mutually-exclusive outputs when both strobe inputs (G1 and G2) are low. The decoding function is performed by using the four input lines to address the output line, passing data from one of the strobe inputs with the other strobe input low. When either strobe input is high, all outputs are high.

**Notes for Demultiplexing:**
- Inputs used to address output line.
- Data passed from one strobe input with other strobe held low.
- Either strobe high gives all high outputs.

**Pinout:** VCC = PIN 24, GND = PIN 12

## 74175 Quad Storage Register

The 74175 is a quad D-type flip-flop with complementary outputs. Data is loaded on the positive edge of the clock pulse. A master reset input clears all flip-flops.

**Truth Table:**

| Input D | Outputs |
|---|---|
| (at time th) | R(1) at th+1 | R(0) at th+1 |
| H | H | L |
| L | L | H |

Where th = bit time before clock pulse, th+1 = bit time after clock pulse.

**Pinout:** VCC = PIN 16, GND = PIN 8
