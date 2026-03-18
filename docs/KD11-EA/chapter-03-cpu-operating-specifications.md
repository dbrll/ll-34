# Chapter 3 — CPU Operating Specifications

Source: EK-KD11E-TM-001, Chapter 3

| Parameter | Value |
|-----------|-------|
| Operating Temperature | 5° to 50° C (41° to 122° F) |
| Relative Humidity | 20 to 95% (without condensation) |
| Input Power | +5 Vdc ±5% at 4.5 A (typical) per module (M7265 and M7266) |
| Physical Size | Two hex modules (8-1/2 × 15 in.) |
| Interface Requirements | All I/O signals are available on connectors A and B. These signals are pin-compatible with modified Unibus pinout as shown in Table 3-1. The bus loading on each of these Unibus lines is equivalent to one bus load. |
| Power and Ground Pinouts | +5 V: pins AA2, BA2, CA2, DA2, EA2 |
| | GND: pins AC2, AT1, BC2, BT1, CC2, CT1, DC2, DT1, EC2, ET1, FC2, FT1 |
| Number of Integrated Circuits | 231 (M7265 = 120; M7266 = 111) |

## Table 3-1 — Standard and Modified Unibus Pin Assignments

| Pin | Standard Signal | Modified Signal | | Pin | Standard Signal | Modified Signal | | Pin | Standard Signal | Modified Signal |
|-----|-----------------|-----------------|---|-----|-----------------|-----------------|---|-----|-----------------|-----------------|
| AA1 | INIT L | INIT L | | AP1 | GROUND | P0* | | BH1 | A01 L | A01 L |
| AA2 | +5 V | +5 V | | AP2 | BBSY L | BBSY L | | BH2 | A00 L | A00 L |
| AB1 | INTR L | INTR L | | AR1 | GROUND | BAT BACKUP +15 V | | BJ1 | A03 L | A03 L |
| AB2 | GROUND | TEST POINT | | AR2 | SACK L | SACK L | | BJ2 | A02 L | A02 L |
| AC1 | D00 L | D00 L | | AS1 | GROUND | BAT BACKUP +15 V | | BK1 | A05 L | A05 L |
| AC2 | GROUND | GROUND | | AS2 | NPR L | NPR L | | BK2 | A04 L | A04 L |
| AD1 | D02 L | D02 L | | AT1 | GROUND | GROUND | | BL1 | A07 L | A07 L |
| AD2 | D01 L | D01 L | | AT2 | BR7 L | BR7 L | | BL2 | A06 L | A06 L |
| AE1 | D04 L | D04 L | | AU1 | NPG H | +20 V | | BM1 | A09 L | A09 L |
| AE2 | D03 L | D03 L | | AU2 | BR6 L | BR6 L | | BM2 | A08 L | A08 L |
| AF1 | D06 L | D06 L | | AV1 | BG7 H | +20 V | | BN1 | A11 L | A11 L |
| AF2 | D05 L | D05 L | | AV2 | GROUND | +20 V | | BN2 | A10 L | A10 L |
| AH1 | D08 L | D08 L | | BA1 | BG6 H | SPARE | | BP1 | A13 L | A13 L |
| AH2 | D07 L | D07 L | | BA2 | +5 V | +5 V | | BP2 | A12 L | A12 L |
| AJ1 | D10 L | D10 L | | BB1 | BG5 H | SPARE | | BR1 | A15 L | A15 L |
| AJ2 | D09 L | D09 L | | BB2 | GROUND | TEST POINT | | BR2 | A14 L | A14 L |
| AK1 | D12 L | D12 L | | BC1 | BR5 L | BR5 L | | BS1 | A17 L | A17 L |
| AK2 | D11 L | D11 L | | BC2 | GROUND | GROUND | | BS2 | A16 L | A16 L |
| AL1 | D14 L | D14 L | | BD1 | GROUND | BAT BACKUP +5 V | | BT1 | GROUND | GROUND |
| AL2 | D13 L | D13 L | | BD2 | BR4 L | BR4 L | | BT2 | C1 L | C1 L |
| AM1 | PA L | PA L | | BE1 | GROUND | INT SSYN* | | BU1 | SSYN L | SSYN L |
| AM2 | D15 L | D15 L | | BE2 | BG4 H | PAR. DET* | | BU2 | C0 L | C0 L |
| AN1 | GROUND | GROUND | | BF1 | ACLO L | ACLO L | | BV1 | MSYN L | MSYN L |
| AN2 | PB L | PB L | | BF2 | DCLO L | DCLO L | | BV2 | GROUND | -5 V |

\*Pins used by parity control module.
