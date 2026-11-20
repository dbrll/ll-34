`default_nettype none
// Verilog transcription of the M7859 KY11-LB interface board
// From DEC 1134A FIELD MAINTENANCE PRINT SET, sheets KY-1 to KY-9
// http://bitsavers.informatik.uni-stuttgart.de/pdf/dec/pdp11/1134/MP00190_1134A_KD11-EA.pdf

module ky11_lb_m7859 (
    (* edge_contact = "AN1" *) inout wire BUS_DC_LO_L,
    (* edge_contact = "AA2,BA2,CA2,DA2", connector_contact = "J1-B,J3-K" *) inout wire PLUS_5V,
    (* edge_contact = "AC2,BC2,CC2,DC2,AT1,BT1,CT1,DT1", connector_contact = "J1-A,J3-H" *) inout wire GND,
    (* edge_contact = "AP1" *) inout wire HALT_RQST_L,
    (* edge_contact = "BL1" *) inout wire BUS_INIT_L,
    (* edge_contact = "CE1" *) inout wire BUS_MSYN_L,
    (* edge_contact = "CJ1" *) inout wire BUS_SSYN_L,
    (* edge_contact = "AR1" *) inout wire HALT_GRANT_H,
    (* edge_contact = "DD1" *) inout wire BUS_BUSY_L,
    (* edge_contact = "DT2" *) inout wire BUS_SACK_L,
    (* edge_contact = "BK2,BL2" *) inout wire BUS_BG4_H,
    (* edge_contact = "BM2,BN2" *) inout wire BUS_BG5_H,
    (* edge_contact = "BP2,BR2" *) inout wire BUS_BG6_H,
    (* edge_contact = "BS2,BT2" *) inout wire BUS_BG7_H,
    (* edge_contact = "CE2" *) inout wire BUS_A16_L,
    (* edge_contact = "CD1" *) inout wire BUS_A17_L,
    (* edge_contact = "CF2" *) inout wire BUS_C1_L,
    (* edge_contact = "CH2" *) inout wire BUS_A00_L,
    (* edge_contact = "CH1" *) inout wire BUS_A01_L,
    (* edge_contact = "CF1" *) inout wire BUS_A02_L,
    (* edge_contact = "CV2" *) inout wire BUS_A03_L,
    (* edge_contact = "CU2" *) inout wire BUS_A04_L,
    (* edge_contact = "CV1" *) inout wire BUS_A05_L,
    (* edge_contact = "CU1" *) inout wire BUS_A06_L,
    (* edge_contact = "CP2" *) inout wire BUS_A07_L,
    (* edge_contact = "CN2" *) inout wire BUS_A08_L,
    (* edge_contact = "CR1" *) inout wire BUS_A09_L,
    (* edge_contact = "CP1" *) inout wire BUS_A10_L,
    (* edge_contact = "CL1" *) inout wire BUS_A11_L,
    (* edge_contact = "CC1" *) inout wire BUS_A12_L,
    (* edge_contact = "CK2" *) inout wire BUS_A13_L,
    (* edge_contact = "CK1" *) inout wire BUS_A14_L,
    (* edge_contact = "CD2" *) inout wire BUS_A15_L,
    (* edge_contact = "AS2" *) inout wire BUS_D00_L,
    (* edge_contact = "AR2" *) inout wire BUS_D01_L,
    (* edge_contact = "AU2" *) inout wire BUS_D02_L,
    (* edge_contact = "AT2" *) inout wire BUS_D03_L,
    (* edge_contact = "AN2" *) inout wire BUS_D04_L,
    (* edge_contact = "AP2" *) inout wire BUS_D05_L,
    (* edge_contact = "AV2" *) inout wire BUS_D06_L,
    (* edge_contact = "AM2" *) inout wire BUS_D07_L,
    (* edge_contact = "AL2" *) inout wire BUS_D08_L,
    (* edge_contact = "AK2" *) inout wire BUS_D09_L,
    (* edge_contact = "AJ2" *) inout wire BUS_D10_L,
    (* edge_contact = "AH1" *) inout wire BUS_D11_L,
    (* edge_contact = "AH2" *) inout wire BUS_D12_L,
    (* edge_contact = "AF2" *) inout wire BUS_D13_L,
    (* edge_contact = "AE2" *) inout wire BUS_D14_L,
    (* edge_contact = "AD2" *) inout wire BUS_D15_L,
    (* edge_contact = "BB2" *) inout wire MINUS_15V
);

    wire ADR_CLR_L;
    wire ADRD_0_H;
    wire ADRD_0_L;
    wire ADRD_1_H;
    wire ADRD_1_L;
    wire ADRD_10_H;
    wire ADRD_11_H;
    wire ADRD_12_H;
    wire ADRD_13_H;
    wire ADRD_2_H;
    wire ADRD_2_L;
    wire ADRD_3_H;
    wire ADRD_3_L;
    wire ADRD_4_H;
    wire ADRD_5_H;
    wire ADRD_6_H;
    wire ADRD_7_H;
    wire ADRD_8_H;
    wire ADRD_9_H;
    wire BB_A00_H;
    wire BB_A01_H;
    wire BB_A02_H;
    wire BB_A03_H;
    wire BB_A04_H;
    wire BB_A05_H;
    wire BB_A06_H;
    wire BB_A07_H;
    wire BB_A08_H;
    wire BB_A09_H;
    wire BB_A10_H;
    wire BB_A11_H;
    wire BB_A12_H;
    wire BB_A13_H;
    wire BB_A14_H;
    wire BB_A15_H;
    wire BB_A16_H;
    wire BB_A17_H;
    wire BB_C1_H;
    wire BB_D00_H;
    wire BB_D01_H;
    wire BB_D02_H;
    wire BB_D03_H;
    wire BB_D04_H;
    wire BB_D05_H;
    wire BB_D06_H;
    wire BB_D07_H;
    wire BB_D08_H;
    wire BB_D09_H;
    wire BB_D10_H;
    wire BB_D11_H;
    wire BB_D12_H;
    wire BB_D13_H;
    wire BB_D14_H;
    wire BB_D15_H;
    wire BB_SSYN_H;
    wire BOOT_L;
    wire BUS_ER_L;
    wire C80_BOTTOM;
    wire C80_TOP;
    wire CK2_H;
    wire CK2_L;
    wire CLR_BUS_L;
    wire CONN_0_H;
    wire CONN_1_H;
    wire CONN_10_H;
    wire CONN_11_H;
    wire CONN_2_H;
    wire CONN_3_H;
    wire CONN_4_H;
    wire CONN_5_H;
    wire CONN_6_H;
    wire CONN_7_H;
    wire CONN_8_H;
    wire CONN_9_H;
    wire D_OUT_0_H;
    wire D_OUT_1_H;
    wire D_OUT_2_H;
    wire D_OUT_3_H;
    wire D_OUT_4_H;
    wire D_OUT_5_H;
    wire D_OUT_6_H;
    wire D_OUT_7_H;
    wire D1_ANODE;
    wire DIN_0_H;
    wire DIN_1_H;
    wire DIN_2_H;
    wire DIN_3_H;
    wire DIN_4_H;
    wire DIN_5_H;
    wire DIN_6_H;
    wire DIN_7_H;
    wire DIN_DRIVER_DIS_H;
    wire E1_12;
    wire E1_14;
    wire E1_2;
    wire E12_5;
    wire E12_6;
    wire E12_8;
    wire E15_10;
    wire E15_15;
    wire E15_2;
    wire E15_7;
    wire E18_D0;
    wire E18_D1;
    wire E18_D2;
    wire E18_D3;
    wire E18_D4;
    wire E18_D5;
    wire E18_D6;
    wire E18_D7;
    wire E18_READY;
    wire E18_S0;
    wire E18_S1;
    wire E18_S2;
    wire E18_SYNC;
    wire E19_10;
    wire E19_15;
    wire E19_2;
    wire E19_7;
    wire E2_10;
    wire E2_15;
    wire E2_2;
    wire E2_7;
    wire E22_8;
    wire E24_10;
    wire E24_12;
    wire E24_2;
    wire E24_4;
    wire E24_6;
    wire E26_3;
    wire E26_8;
    wire E29_1;
    wire E29_10;
    wire E29_13;
    wire E29_4;
    wire E30_10;
    wire E30_4;
    wire E32_3;
    wire E34_4;
    wire E34_5;
    wire E34_7;
    wire E34_9;
    wire E36_QA;
    wire E36_QD;
    wire E40_12;
    wire E40_6;
    wire E40_8;
    wire E42_12;
    wire E42_8;
    wire E48_10;
    wire E48_11;
    wire E48_13;
    wire E48_14;
    wire E48_15;
    wire E48_16;
    wire E48_9;
    wire E51_15;
    wire E51_2;
    wire E51_7;
    wire E52_11;
    wire E52_15;
    wire E52_2;
    wire E52_7;
    wire E53_3;
    wire E53_6;
    wire E57_10;
    wire E57_15;
    wire E57_2;
    wire E57_7;
    wire E58_10;
    wire E58_15;
    wire E58_2;
    wire E58_7;
    wire E6_5;
    wire E6_8;
    wire E6_9;
    wire E60_9;
    wire E62_IN;
    wire E63_5;
    wire E64_11;
    wire E64_14;
    wire E66_4;
    wire E67_10;
    wire E67_15;
    wire E67_2;
    wire E67_7;
    wire E68_14;
    wire E68_3;
    wire E68_6;
    wire E73_15;
    wire E75_6;
    wire E76_11;
    wire E76_6;
    wire E76_8;
    wire E9_10;
    wire E9_15;
    wire E9_2;
    wire E9_7;
    wire EN_AR_TO_BUS_H;
    wire EN_AR_TO_BUS_L;
    wire EN_DB_TO_BUS_L;
    wire HALT_BUSY_H;
    wire J2_A;
    wire J2_B;
    wire J2_C;
    wire J2_D;
    wire J2_E;
    wire J2_F;
    wire J2_H;
    wire J2_J;
    wire J3_A;
    wire J3_B;
    wire J3_C;
    wire J3_D;
    wire KEY_1_H;
    wire KEY_2_H;
    wire KEY_3_H;
    wire KEY_4_H;
    wire LD_AD_1_H;
    wire LD_AD_2_H;
    wire LD_REG_0_H;
    wire LD_REG_1_H;
    wire LD_REG_2_H;
    wire LD_REG_3_H;
    wire LD_REG_4_H;
    wire LD_REG_5_H;
    wire MAINT_1_L;
    wire MAINT_MD_L;
    wire MAN_CLK_EN_L;
    wire MAN_CLK_L;
    wire MINUS_9V;
    wire NUM_1_H;
    wire NUM_1_OUT;
    wire NUM_2_H;
    wire NUM_2_OUT;
    wire NUM_3_H;
    wire NUM_3_OUT;
    wire OSC_CLK;
    wire PC_FUN_0_H;
    wire PC_FUN_1_H;
    wire PLUS_3V;
    wire PUP_1_L;
    wire PUP_H;
    wire PUP_L;
    wire RAM_TO_DIN_BUS_L;
    wire RAM_WRITE_H;
    wire READ_INPUT_0_L;
    wire READ_INPUT_1_L;
    wire READ_INPUT_2_L;
    wire READ_INPUT_3_L;
    wire READ_INPUT_4_L;
    wire READ_INPUT_5_L;
    wire READ_INPUT_6_L;
    wire READ_INPUT_7_L;
    wire ROM_1_EN_L;
    wire ROM_2_EN_L;
    wire RUN_L;
    wire SCAN_1_L;
    wire SCAN_1_OUT;
    wire SCAN_2_L;
    wire SCAN_2_OUT;
    wire SCAN_3_L;
    wire SCAN_3_OUT;
    wire SCAN_4_L;
    wire SCAN_4_OUT;
    wire SCAN_5_L;
    wire SCAN_5_OUT;
    wire SCAN_6_L;
    wire SCAN_6_OUT;
    wire SR_DISP_L;
    wire SSYN_REC_H;
    wire SSYN_REC_L;
    wire STOP_L;
    wire SYNC_H;
    wire T1I_L;
    wire TAKE_BUS_L;
    wire TS1_L;
    wire TS2_L;
    wire TS3_L;
    wire TS4_L;
    wire TS5_L;
    wire WAIT_L;

// KY-1, PDF page 7

    // E42 mc4024p, zone 8
    mc4024p E42 (
        .p1(PLUS_5V),                // VCM1
        .p2(),                       // VCX1
        .p3(),                       // CX1_A
        .p4(),                       // CX1_B
        .p5(GND),                    // GND5
        .p6(),                       // OUT1
        .p7(GND),                    // GND7
        .p8(E42_8),                  // OUT2
        .p9(GND),                    // GND9
        .p10(C80_TOP),               // CX2_A
        .p11(C80_BOTTOM),            // CX2_B
        .p12(E42_12),                // VCX2
        .p13(PLUS_5V),               // VCM2
        .p14(PLUS_5V)                // BUFFER_VCC
    );

    // C80 270 pF. Value read from the parts list. The drawing label is blurred., zone 8
    capacitor C80 (
        .p1(C80_TOP),                // END1
        .p2(C80_BOTTOM)              // END2
    );

    // R1 5.6 kohm, zone 8
    resistor R1 (
        .p1(PLUS_5V),                // END1
        .p2(E42_12)                  // END2
    );

    // R2 10K variable. Three terminal trimmer drawn as a rheostat, zone 8
    resistor R2 (
        .p1(PLUS_5V),                // END1
        .p2(E42_12)                  // END2
    );

    // R3 6.8 kohm, zone 8
    resistor R3 (
        .p1(E42_12),                 // END1
        .p2(GND)                     // END2
    );

    // W1 jumper, zone 8
    jumper W1 (
        .p1(E42_8),                  // END1
        .p2(OSC_CLK)                 // END2
    );

    // E12 ttl_7474, zone 7
    ttl_7474 E12 (
        .p1(PLUS_3V),                // CLR1_N
        .p2(E12_6),                  // D1
        .p3(OSC_CLK),                // CLK1
        .p4(PLUS_3V),                // PRE1_N
        .p5(E12_5),                  // Q1
        .p6(E12_6),                  // Q1_N
        .p7(GND),                    // GND
        .p8(E12_8),                  // Q2_N
        .p9(),                       // Q2
        .p10(E29_4),                 // PRE2_N
        .p11(E36_QD),                // CLK2
        .p12(GND),                   // D2
        .p13(PLUS_3V),               // CLR2_N
        .p14(PLUS_5V)                // VCC
    );

    // E30 ttl_7402, zone 7
    ttl_7402 E30 (
        .p1(DIN_DRIVER_DIS_H),       // Y1
        .p2(E29_13),                 // A1
        .p3(E29_13),                 // B1
        .p4(E30_4),                  // Y2
        .p5(BB_SSYN_H),              // A2
        .p6(E24_12),                 // B2
        .p7(GND),                    // GND
        .p8(OSC_CLK),                // A3
        .p9(E12_5),                  // B3
        .p10(E30_10),                // Y3
        .p11(E12_6),                 // A4
        .p12(OSC_CLK),               // B4
        .p13(CK2_H),                 // Y4
        .p14(PLUS_5V)                // VCC
    );

    // E40 ttl_7404, zone 6
    ttl_7404 E40 (
        .p1(CK2_H),                  // A1
        .p2(CK2_L),                  // Y1
        .p3(E24_10),                 // A2
        .p4(SYNC_H),                 // Y2
        .p5(BB_A03_H),               // A3
        .p6(E40_6),                  // Y3
        .p7(GND),                    // GND
        .p8(E40_8),                  // Y4
        .p9(BB_C1_H),                // A4
        .p10(PUP_L),                 // Y5
        .p11(PUP_H),                 // A5
        .p12(E40_12),                // Y6
        .p13(E48_16),                // A6
        .p14(PLUS_5V)                // VCC
    );

    // E18 intel_8008, zone 6
    intel_8008 E18 (
        .p1(MINUS_9V),               // VDD
        .p2(E18_D7),                 // D7
        .p3(E18_D6),                 // D6
        .p4(E18_D5),                 // D5
        .p5(E18_D4),                 // D4
        .p6(E18_D3),                 // D3
        .p7(E18_D2),                 // D2
        .p8(E18_D1),                 // D1
        .p9(E18_D0),                 // D0
        .p10(PLUS_5V),               // VCC
        .p11(E18_S2),                // S2
        .p12(E18_S1),                // S1
        .p13(E18_S0),                // S0
        .p14(E18_SYNC),              // SYNC
        .p15(CK2_H),                 // PHI2
        .p16(E30_10),                // PHI1
        .p17(E18_READY),             // READY
        .p18(E12_8)                  // INTERRUPT
    );

    // R5 1 kohm, zone 6
    resistor R5 (
        .p1(PLUS_5V),                // END1
        .p2(E18_READY)               // END2
    );

    // D3 D664, zone 5
    diode D3 (
        .p1(GND),                    // ANODE
        .p2(E18_S0)                  // CATHODE
    );

    // D4 D664, zone 5
    diode D4 (
        .p1(GND),                    // ANODE
        .p2(E18_S1)                  // CATHODE
    );

    // D5 D664, zone 6
    diode D5 (
        .p1(GND),                    // ANODE
        .p2(E18_S2)                  // CATHODE
    );

    // D6 D664, zone 6
    diode D6 (
        .p1(GND),                    // ANODE
        .p2(E18_SYNC)                // CATHODE
    );

    // D1 D664, zone 5
    diode D1 (
        .p1(D1_ANODE),               // ANODE
        .p2(GND)                     // CATHODE
    );

    // D2 D664, zone 5
    diode D2 (
        .p1(MINUS_9V),               // ANODE
        .p2(D1_ANODE)                // CATHODE
    );

    // C79 6.8 uF, zone 5
    polar_capacitor C79 (
        .p1(GND),                    // POSITIVE
        .p2(MINUS_9V)                // NEGATIVE
    );

    // R4 120 ohm, zone 5
    resistor R4 (
        .p1(MINUS_9V),               // END1
        .p2(MINUS_15V)               // END2
    );

    // E16 dm8833, zone 5
    dm8833 E16 (
        .p1(E18_D7),                 // BUS_A
        .p2(DIN_7_H),                // IN_A
        .p3(D_OUT_7_H),              // OUT_A
        .p4(E18_D6),                 // BUS_B
        .p5(DIN_6_H),                // IN_B
        .p6(D_OUT_6_H),              // OUT_B
        .p7(GND),                    // RECEIVER_DISABLE
        .p8(GND),                    // GND
        .p9(DIN_DRIVER_DIS_H),       // DRIVER_DISABLE
        .p10(D_OUT_5_H),             // OUT_C
        .p11(DIN_5_H),               // IN_C
        .p12(E18_D5),                // BUS_C
        .p13(D_OUT_4_H),             // OUT_D
        .p14(DIN_4_H),               // IN_D
        .p15(E18_D4),                // BUS_D
        .p16(PLUS_5V)                // VCC
    );

    // E17 dm8833, zone 5
    dm8833 E17 (
        .p1(E18_D3),                 // BUS_A
        .p2(DIN_3_H),                // IN_A
        .p3(D_OUT_3_H),              // OUT_A
        .p4(E18_D2),                 // BUS_B
        .p5(DIN_2_H),                // IN_B
        .p6(D_OUT_2_H),              // OUT_B
        .p7(GND),                    // RECEIVER_DISABLE
        .p8(GND),                    // GND
        .p9(DIN_DRIVER_DIS_H),       // DRIVER_DISABLE
        .p10(D_OUT_1_H),             // OUT_C
        .p11(DIN_1_H),               // IN_C
        .p12(E18_D1),                // BUS_C
        .p13(D_OUT_0_H),             // OUT_D
        .p14(DIN_0_H),               // IN_D
        .p15(E18_D0),                // BUS_D
        .p16(PLUS_5V)                // VCC
    );

    // E24 dm8837, zone 5
    dm8837 E24 (
        .p1(E18_S0),                 // IN4
        .p2(E24_2),                  // OUT4
        .p3(E18_S1),                 // IN5
        .p4(E24_4),                  // OUT5
        .p5(E18_S2),                 // IN6
        .p6(E24_6),                  // OUT6
        .p7(GND),                    // DISABLE_B
        .p8(GND),                    // GND
        .p9(GND),                    // DISABLE_A
        .p10(E24_10),                // OUT3
        .p11(E18_SYNC),              // IN3
        .p12(E24_12),                // OUT2
        .p13(BUS_BUSY_L),            // IN2
        .p14(BB_SSYN_H),             // OUT1
        .p15(BUS_SSYN_L),            // IN1
        .p16(PLUS_5V)                // VCC
    );

    // E23 ttl_7442, zone 5
    ttl_7442 E23 (
        .p1(TS4_L),                  // Y0_N
        .p2(STOP_L),                 // Y1_N
        .p3(TS5_L),                  // Y2_N
        .p4(TS3_L),                  // Y3_N
        .p5(T1I_L),                  // Y4_N
        .p6(TS1_L),                  // Y5_N
        .p7(TS2_L),                  // Y6_N
        .p8(GND),                    // GND
        .p9(WAIT_L),                 // Y7_N
        .p10(),                      // Y8_N
        .p11(),                      // Y9_N
        .p12(GND),                   // D
        .p13(E24_2),                 // C
        .p14(E24_4),                 // B
        .p15(E24_6),                 // A
        .p16(PLUS_5V)                // VCC
    );

    // E22 ttl_7427, zone 6
    ttl_7427 E22 (
        .p1(CK2_L),                  // A1
        .p2(SYNC_H),                 // B1
        .p3(SYNC_H),                 // A2
        .p4(CK2_L),                  // B2
        .p5(TS2_L),                  // C2
        .p6(LD_AD_2_H),              // Y2
        .p7(GND),                    // GND
        .p8(E22_8),                  // Y3
        .p9(SYNC_H),                 // A3
        .p10(TS3_L),                 // B3
        .p11(CK2_L),                 // C3
        .p12(LD_AD_1_H),             // Y1
        .p13(TS1_L),                 // C1
        .p14(PLUS_5V)                // VCC
    );

    // E1 dm8837, zone 8
    dm8837 E1 (
        .p1(HALT_GRANT_H),           // IN4
        .p2(E1_2),                   // OUT4
        .p3(BUS_DC_LO_L),            // IN5
        .p4(PUP_H),                  // OUT5
        .p5(PUP_H),                  // IN6
        .p6(PUP_1_L),                // OUT6
        .p7(GND),                    // DISABLE_B
        .p8(GND),                    // GND
        .p9(GND),                    // DISABLE_A
        .p10(),                      // OUT3
        .p11(),                      // IN3
        .p12(E1_12),                 // OUT2
        .p13(E1_14),                 // IN2
        .p14(E1_14),                 // OUT1
        .p15(BUS_INIT_L),            // IN1
        .p16(PLUS_5V)                // VCC
    );

    // E36 ttl_7493, zone 7
    ttl_7493 E36 (
        .p1(E36_QA),                 // CKB
        .p2(PUP_H),                  // R0_1
        .p3(PUP_H),                  // R0_2
        .p4(),                       // NC4
        .p5(PLUS_5V),                // VCC
        .p6(),                       // NC6
        .p7(),                       // NC7
        .p8(),                       // QC
        .p9(),                       // QB
        .p10(GND),                   // GND
        .p11(E36_QD),                // QD
        .p12(E36_QA),                // QA
        .p13(),                      // NC13
        .p14(E6_9)                   // CKA
    );

    // E29 ttl_7402, zone 7
    ttl_7402 E29 (
        .p1(E29_1),                  // Y1
        .p2(STOP_L),                 // A1
        .p3(CK2_L),                  // B1
        .p4(E29_4),                  // Y2
        .p5(PUP_H),                  // A2
        .p6(E29_10),                 // B2
        .p7(GND),                    // GND
        .p8(CK2_L),                  // A3
        .p9(T1I_L),                  // B3
        .p10(E29_10),                // Y3
        .p11(E34_4),                 // A4
        .p12(E26_3),                 // B4
        .p13(E29_13),                // Y4
        .p14(PLUS_5V)                // VCC
    );

    // E6 ttl_7474, zone 7
    ttl_7474 E6 (
        .p1(PLUS_3V),                // CLR1_N
        .p2(GND),                    // D1
        .p3(E29_10),                 // CLK1
        .p4(TS3_L),                  // PRE1_N
        .p5(E6_5),                   // Q1
        .p6(),                       // Q1_N
        .p7(GND),                    // GND
        .p8(E6_8),                   // Q2_N
        .p9(E6_9),                   // Q2
        .p10(PLUS_3V),               // PRE2_N
        .p11(E29_1),                 // CLK2
        .p12(E6_8),                  // D2
        .p13(PUP_1_L),               // CLR2_N
        .p14(PLUS_5V)                // VCC
    );

    // E26 ttl_7408, zone 6
    ttl_7408 E26 (
        .p1(WAIT_L),                 // A1
        .p2(TS3_L),                  // B1
        .p3(E26_3),                  // Y1
        .p4(E34_5),                  // A2
        .p5(E22_8),                  // B2
        .p6(RAM_WRITE_H),            // Y2
        .p7(GND),                    // GND
        .p8(E26_8),                  // Y3
        .p9(E34_9),                  // A3
        .p10(E22_8),                 // B3
        .p11(ADR_CLR_L),             // Y4
        .p12(PUP_1_L),               // A4
        .p13(E6_5),                  // B4
        .p14(PLUS_5V)                // VCC
    );

    // E28 ttl_74175, zone 3
    ttl_74175 E28 (
        .p1(ADR_CLR_L),              // CLR_N
        .p2(PC_FUN_1_H),             // Q1
        .p3(),                       // Q1_N
        .p4(D_OUT_7_H),              // D1
        .p5(D_OUT_6_H),              // D2
        .p6(),                       // Q2_N
        .p7(PC_FUN_0_H),             // Q2
        .p8(GND),                    // GND
        .p9(LD_AD_2_H),              // CLK
        .p10(ADRD_13_H),             // Q3
        .p11(),                      // Q3_N
        .p12(D_OUT_5_H),             // D3
        .p13(D_OUT_4_H),             // D4
        .p14(),                      // Q4_N
        .p15(ADRD_12_H),             // Q4
        .p16(PLUS_5V)                // VCC
    );

    // E10 ttl_74175, zone 3
    ttl_74175 E10 (
        .p1(ADR_CLR_L),              // CLR_N
        .p2(ADRD_11_H),              // Q1
        .p3(),                       // Q1_N
        .p4(D_OUT_3_H),              // D1
        .p5(D_OUT_2_H),              // D2
        .p6(),                       // Q2_N
        .p7(ADRD_10_H),              // Q2
        .p8(GND),                    // GND
        .p9(LD_AD_2_H),              // CLK
        .p10(ADRD_9_H),              // Q3
        .p11(),                      // Q3_N
        .p12(D_OUT_1_H),             // D3
        .p13(D_OUT_0_H),             // D4
        .p14(),                      // Q4_N
        .p15(ADRD_8_H),              // Q4
        .p16(PLUS_5V)                // VCC
    );

    // E4 ttl_74175, zone 3
    ttl_74175 E4 (
        .p1(ADR_CLR_L),              // CLR_N
        .p2(ADRD_7_H),               // Q1
        .p3(),                       // Q1_N
        .p4(D_OUT_7_H),              // D1
        .p5(D_OUT_6_H),              // D2
        .p6(),                       // Q2_N
        .p7(ADRD_6_H),               // Q2
        .p8(GND),                    // GND
        .p9(LD_AD_1_H),              // CLK
        .p10(ADRD_5_H),              // Q3
        .p11(),                      // Q3_N
        .p12(D_OUT_5_H),             // D3
        .p13(D_OUT_4_H),             // D4
        .p14(),                      // Q4_N
        .p15(ADRD_4_H),              // Q4
        .p16(PLUS_5V)                // VCC
    );

    // E5 ttl_74175, zone 2
    ttl_74175 E5 (
        .p1(ADR_CLR_L),              // CLR_N
        .p2(ADRD_3_H),               // Q1
        .p3(ADRD_3_L),               // Q1_N
        .p4(D_OUT_3_H),              // D1
        .p5(D_OUT_2_H),              // D2
        .p6(ADRD_2_L),               // Q2_N
        .p7(ADRD_2_H),               // Q2
        .p8(GND),                    // GND
        .p9(LD_AD_1_H),              // CLK
        .p10(ADRD_1_H),              // Q3
        .p11(ADRD_1_L),              // Q3_N
        .p12(D_OUT_1_H),             // D3
        .p13(D_OUT_0_H),             // D4
        .p14(ADRD_0_L),              // Q4_N
        .p15(ADRD_0_H),              // Q4
        .p16(PLUS_5V)                // VCC
    );

// KY-2, PDF page 8

    // E11 dm85s68, zone 5
    dm85s68 E11 (
        .p1(D_OUT_1_H),              // D2
        .p2(D_OUT_0_H),              // D1
        .p3(ADRD_0_L),               // A0
        .p4(ADRD_2_L),               // A2
        .p5(ADRD_3_L),               // A3
        .p6(ADRD_1_L),               // A1
        .p7(DIN_0_H),                // O1
        .p8(DIN_1_H),                // O2
        .p9(GND),                    // GND
        .p10(DIN_2_H),               // O3
        .p11(DIN_3_H),               // O4
        .p12(RAM_TO_DIN_BUS_L),      // OD
        .p13(PLUS_3V),               // OS_N
        .p14(RAM_WRITE_H),           // CLK
        .p15(GND),                   // WE_N
        .p16(D_OUT_3_H),             // D4
        .p17(D_OUT_2_H),             // D3
        .p18(PLUS_5V)                // VCC
    );

    // E27 dm85s68, zone 6
    dm85s68 E27 (
        .p1(D_OUT_5_H),              // D2
        .p2(D_OUT_4_H),              // D1
        .p3(ADRD_0_L),               // A0
        .p4(ADRD_2_L),               // A2
        .p5(ADRD_3_L),               // A3
        .p6(ADRD_1_L),               // A1
        .p7(DIN_4_H),                // O1
        .p8(DIN_5_H),                // O2
        .p9(GND),                    // GND
        .p10(DIN_6_H),               // O3
        .p11(DIN_7_H),               // O4
        .p12(RAM_TO_DIN_BUS_L),      // OD
        .p13(PLUS_3V),               // OS_N
        .p14(RAM_WRITE_H),           // CLK
        .p15(GND),                   // WE_N
        .p16(D_OUT_7_H),             // D4
        .p17(D_OUT_6_H),             // D3
        .p18(PLUS_5V)                // VCC
    );

    // E3 prom_82s131, zone 4
    prom_82s131 E3 (
        .p1(ADRD_6_H),               // A6
        .p2(ADRD_5_H),               // A5
        .p3(ADRD_4_H),               // A4
        .p4(ADRD_3_H),               // A3
        .p5(ADRD_0_H),               // A0
        .p6(ADRD_1_H),               // A1
        .p7(ADRD_2_H),               // A2
        .p8(GND),                    // GND
        .p9(DIN_3_H),                // O4
        .p10(DIN_2_H),               // O3
        .p11(DIN_1_H),               // O2
        .p12(DIN_0_H),               // O1
        .p13(ROM_1_EN_L),            // CE_N
        .p14(ADRD_8_H),              // A8
        .p15(ADRD_7_H),              // A7
        .p16(PLUS_5V)                // VCC
    );

    // E21 prom_82s131, zone 3
    prom_82s131 E21 (
        .p1(ADRD_6_H),               // A6
        .p2(ADRD_5_H),               // A5
        .p3(ADRD_4_H),               // A4
        .p4(ADRD_3_H),               // A3
        .p5(ADRD_0_H),               // A0
        .p6(ADRD_1_H),               // A1
        .p7(ADRD_2_H),               // A2
        .p8(GND),                    // GND
        .p9(DIN_7_H),                // O4
        .p10(DIN_6_H),               // O3
        .p11(DIN_5_H),               // O2
        .p12(DIN_4_H),               // O1
        .p13(ROM_1_EN_L),            // CE_N
        .p14(ADRD_8_H),              // A8
        .p15(ADRD_7_H),              // A7
        .p16(PLUS_5V)                // VCC
    );

    // E33 prom_82s131, zone 3
    prom_82s131 E33 (
        .p1(ADRD_6_H),               // A6
        .p2(ADRD_5_H),               // A5
        .p3(ADRD_4_H),               // A4
        .p4(ADRD_3_H),               // A3
        .p5(ADRD_0_H),               // A0
        .p6(ADRD_1_H),               // A1
        .p7(ADRD_2_H),               // A2
        .p8(GND),                    // GND
        .p9(DIN_3_H),                // O4
        .p10(DIN_2_H),               // O3
        .p11(DIN_1_H),               // O2
        .p12(DIN_0_H),               // O1
        .p13(ROM_2_EN_L),            // CE_N
        .p14(ADRD_8_H),              // A8
        .p15(ADRD_7_H),              // A7
        .p16(PLUS_5V)                // VCC
    );

    // E39 prom_82s131, zone 3
    prom_82s131 E39 (
        .p1(ADRD_6_H),               // A6
        .p2(ADRD_5_H),               // A5
        .p3(ADRD_4_H),               // A4
        .p4(ADRD_3_H),               // A3
        .p5(ADRD_0_H),               // A0
        .p6(ADRD_1_H),               // A1
        .p7(ADRD_2_H),               // A2
        .p8(GND),                    // GND
        .p9(DIN_7_H),                // O4
        .p10(DIN_6_H),               // O3
        .p11(DIN_5_H),               // O2
        .p12(DIN_4_H),               // O1
        .p13(ROM_2_EN_L),            // CE_N
        .p14(ADRD_8_H),              // A8
        .p15(ADRD_7_H),              // A7
        .p16(PLUS_5V)                // VCC
    );

// KY-3, PDF page 9

    // E35 dm8093, zone 5
    dm8093 E35 (
        .p1(READ_INPUT_2_L),         // C1
        .p2(BB_A00_H),               // A1
        .p3(DIN_0_H),                // Y1
        .p4(READ_INPUT_2_L),         // C2
        .p5(BB_A01_H),               // A2
        .p6(DIN_1_H),                // Y2
        .p7(GND),                    // GND
        .p8(DIN_2_H),                // Y3
        .p9(BB_A02_H),               // A3
        .p10(READ_INPUT_2_L),        // C3
        .p11(DIN_3_H),               // Y4
        .p12(BB_A03_H),              // A4
        .p13(READ_INPUT_2_L),        // C4
        .p14(PLUS_5V)                // VCC
    );

    // E44 dm8093, zone 5
    dm8093 E44 (
        .p1(READ_INPUT_2_L),         // C1
        .p2(BB_A04_H),               // A1
        .p3(DIN_4_H),                // Y1
        .p4(READ_INPUT_2_L),         // C2
        .p5(BB_A05_H),               // A2
        .p6(DIN_5_H),                // Y2
        .p7(GND),                    // GND
        .p8(DIN_6_H),                // Y3
        .p9(BB_A06_H),               // A3
        .p10(READ_INPUT_2_L),        // C3
        .p11(DIN_7_H),               // Y4
        .p12(BB_A07_H),              // A4
        .p13(READ_INPUT_2_L),        // C4
        .p14(PLUS_5V)                // VCC
    );

    // E70 dm8093, zone 5
    dm8093 E70 (
        .p1(READ_INPUT_3_L),         // C1
        .p2(BB_A08_H),               // A1
        .p3(DIN_0_H),                // Y1
        .p4(READ_INPUT_3_L),         // C2
        .p5(BB_A09_H),               // A2
        .p6(DIN_1_H),                // Y2
        .p7(GND),                    // GND
        .p8(DIN_2_H),                // Y3
        .p9(BB_A10_H),               // A3
        .p10(READ_INPUT_3_L),        // C3
        .p11(DIN_3_H),               // Y4
        .p12(BB_A11_H),              // A4
        .p13(READ_INPUT_3_L),        // C4
        .p14(PLUS_5V)                // VCC
    );

    // E46 dm8093, zone 5
    dm8093 E46 (
        .p1(READ_INPUT_3_L),         // C1
        .p2(BB_A12_H),               // A1
        .p3(DIN_4_H),                // Y1
        .p4(READ_INPUT_3_L),         // C2
        .p5(BB_A13_H),               // A2
        .p6(DIN_5_H),                // Y2
        .p7(GND),                    // GND
        .p8(DIN_6_H),                // Y3
        .p9(BB_A14_H),               // A3
        .p10(READ_INPUT_3_L),        // C3
        .p11(DIN_7_H),               // Y4
        .p12(BB_A15_H),              // A4
        .p13(READ_INPUT_3_L),        // C4
        .p14(PLUS_5V)                // VCC
    );

    // E71 dm8093, zone 7
    dm8093 E71 (
        .p1(READ_INPUT_4_L),         // C1
        .p2(BB_A16_H),               // A1
        .p3(DIN_0_H),                // Y1
        .p4(READ_INPUT_4_L),         // C2
        .p5(BB_A17_H),               // A2
        .p6(DIN_1_H),                // Y2
        .p7(GND),                    // GND
        .p8(DIN_2_H),                // Y3
        .p9(),                       // A3
        .p10(READ_INPUT_4_L),        // C3
        .p11(DIN_3_H),               // Y4
        .p12(),                      // A4
        .p13(READ_INPUT_4_L),        // C4
        .p14(PLUS_5V)                // VCC
    );

    // E45 dm8093, zone 5
    dm8093 E45 (
        .p1(READ_INPUT_7_L),         // C1
        .p2(CONN_8_H),               // A1
        .p3(DIN_0_H),                // Y1
        .p4(READ_INPUT_7_L),         // C2
        .p5(CONN_9_H),               // A2
        .p6(DIN_1_H),                // Y2
        .p7(GND),                    // GND
        .p8(DIN_2_H),                // Y3
        .p9(CONN_10_H),              // A3
        .p10(READ_INPUT_7_L),        // C3
        .p11(DIN_3_H),               // Y4
        .p12(CONN_11_H),             // A4
        .p13(READ_INPUT_7_L),        // C4
        .p14(PLUS_5V)                // VCC
    );

    // E74 dm8093, zone 3
    dm8093 E74 (
        .p1(READ_INPUT_5_L),         // C1
        .p2(KEY_1_H),                // A1
        .p3(DIN_0_H),                // Y1
        .p4(READ_INPUT_5_L),         // C2
        .p5(KEY_2_H),                // A2
        .p6(DIN_1_H),                // Y2
        .p7(GND),                    // GND
        .p8(DIN_2_H),                // Y3
        .p9(KEY_3_H),                // A3
        .p10(READ_INPUT_5_L),        // C3
        .p11(DIN_3_H),               // Y4
        .p12(KEY_4_H),               // A4
        .p13(READ_INPUT_5_L),        // C4
        .p14(PLUS_5V)                // VCC
    );

    // E38 dm8093, zone 3
    dm8093 E38 (
        .p1(READ_INPUT_5_L),         // C1
        .p2(SSYN_REC_H),             // A1
        .p3(DIN_4_H),                // Y1
        .p4(READ_INPUT_5_L),         // C2
        .p5(HALT_BUSY_H),            // A2
        .p6(DIN_5_H),                // Y2
        .p7(GND),                    // GND
        .p8(DIN_6_H),                // Y3
        .p9(),                       // A3
        .p10(READ_INPUT_5_L),        // C3
        .p11(DIN_7_H),               // Y4
        .p12(),                      // A4
        .p13(READ_INPUT_5_L),        // C4
        .p14(PLUS_5V)                // VCC
    );

    // E59 dm8093, zone 3
    dm8093 E59 (
        .p1(READ_INPUT_6_L),         // C1
        .p2(CONN_0_H),               // A1
        .p3(DIN_0_H),                // Y1
        .p4(READ_INPUT_6_L),         // C2
        .p5(CONN_1_H),               // A2
        .p6(DIN_1_H),                // Y2
        .p7(GND),                    // GND
        .p8(DIN_2_H),                // Y3
        .p9(CONN_2_H),               // A3
        .p10(READ_INPUT_6_L),        // C3
        .p11(DIN_3_H),               // Y4
        .p12(CONN_3_H),              // A4
        .p13(READ_INPUT_6_L),        // C4
        .p14(PLUS_5V)                // VCC
    );

    // E41 dm8093, zone 3
    dm8093 E41 (
        .p1(READ_INPUT_6_L),         // C1
        .p2(CONN_4_H),               // A1
        .p3(DIN_4_H),                // Y1
        .p4(READ_INPUT_6_L),         // C2
        .p5(CONN_5_H),               // A2
        .p6(DIN_5_H),                // Y2
        .p7(GND),                    // GND
        .p8(DIN_6_H),                // Y3
        .p9(CONN_6_H),               // A3
        .p10(READ_INPUT_6_L),        // C3
        .p11(DIN_7_H),               // Y4
        .p12(CONN_7_H),              // A4
        .p13(READ_INPUT_6_L),        // C4
        .p14(PLUS_5V)                // VCC
    );

// KY-4, PDF page 10

    // E60 ttl_74s133, zone 7
    ttl_74s133 E60 (
        .p1(BB_A17_H),               // A
        .p2(BB_A16_H),               // B
        .p3(BB_A15_H),               // C
        .p4(BB_A14_H),               // D
        .p5(BB_A13_H),               // E
        .p6(BB_A12_H),               // F
        .p7(BB_A11_H),               // G
        .p8(GND),                    // GND
        .p9(E60_9),                  // Y
        .p10(BB_A10_H),              // H
        .p11(BB_A09_H),              // I
        .p12(BB_A08_H),              // J
        .p13(BB_A06_H),              // K
        .p14(BB_A05_H),              // L
        .p15(BB_A04_H),              // M
        .p16(PLUS_5V)                // VCC
    );

    // E32 sp314a, zone 6
    sp314a E32 (
        .p1(GND),                    // GND
        .p2(),                       // NC2
        .p3(E32_3),                  // Y
        .p4(BB_A01_H),               // A
        .p5(BB_A07_H),               // B
        .p6(BB_A02_H),               // C
        .p7(),                       // NC7
        .p8(PLUS_5V),                // VCC
        .p9(E60_9),                  // D
        .p10(BUS_MSYN_L),            // E
        .p11(E40_6),                 // F
        .p12(GND),                   // G
        .p13(),                      // NC13
        .p14()                       // NC14
    );

    // E66 ttl_7417, zone 5
    ttl_7417 E66 (
        .p1(NUM_1_H),                // A1
        .p2(NUM_1_OUT),              // Y1
        .p3(E32_3),                  // A2
        .p4(E66_4),                  // Y2
        .p5(NUM_2_H),                // A3
        .p6(NUM_2_OUT),              // Y3
        .p7(GND),                    // GND
        .p8(NUM_3_OUT),              // Y4
        .p9(NUM_3_H),                // A4
        .p10(EN_DB_TO_BUS_L),        // Y5
        .p11(E52_11),                // A5
        .p12(RUN_L),                 // Y6
        .p13(HALT_BUSY_H),           // A6
        .p14(PLUS_5V)                // VCC
    );

    // R6 1 kohm, zone 5
    resistor R6 (
        .p1(PLUS_5V),                // END1
        .p2(E66_4)                   // END2
    );

    // R20 100 ohm, zone 5
    resistor R20 (
        .p1(E66_4),                  // END1
        .p2(E62_IN)                  // END2
    );

    // C81 470 pF, zone 5
    capacitor C81 (
        .p1(E62_IN),                 // END1
        .p2(GND)                     // END2
    );

    // R7 1 kohm, zone 5
    resistor R7 (
        .p1(PLUS_5V),                // END1
        .p2(EN_DB_TO_BUS_L)          // END2
    );

    // E62 signetics_8881, zone 5
    signetics_8881 E62 (
        .p1(BUS_SSYN_L),             // Y1
        .p2(E62_IN),                 // A1
        .p3(E62_IN),                 // B1
        .p4(),                       // Y2
        .p5(),                       // A2
        .p6(),                       // B2
        .p7(GND),                    // GND
        .p8(HALT_BUSY_H),            // A3
        .p9(HALT_BUSY_H),            // B3
        .p10(BUS_BUSY_L),            // Y3
        .p11(E63_5),                 // B4
        .p12(E53_3),                 // A4
        .p13(BUS_SACK_L),            // Y4
        .p14(PLUS_5V)                // VCC
    );

    // E43 signetics_8881, zone 3
    signetics_8881 E43 (
        .p1(HALT_RQST_L),            // Y1
        .p2(E52_2),                  // A1
        .p3(E52_2),                  // B1
        .p4(BUS_INIT_L),             // Y2
        .p5(E52_7),                  // A2
        .p6(E52_7),                  // B2
        .p7(GND),                    // GND
        .p8(E40_8),                  // A3
        .p9(E32_3),                  // B3
        .p10(EN_DB_TO_BUS_L),        // Y3
        .p11(E52_15),                // B4
        .p12(SSYN_REC_L),            // A4
        .p13(BUS_MSYN_L),            // Y4
        .p14(PLUS_5V)                // VCC
    );

    // E34 prom_82s123, zone 7
    prom_82s123 E34 (
        .p1(ROM_1_EN_L),             // O1
        .p2(ROM_2_EN_L),             // O2
        .p3(),                       // O3
        .p4(E34_4),                  // O4
        .p5(E34_5),                  // O5
        .p6(RAM_TO_DIN_BUS_L),       // O6
        .p7(E34_7),                  // O7
        .p8(GND),                    // GND
        .p9(E34_9),                  // O8
        .p10(ADRD_11_H),             // A0
        .p11(ADRD_12_H),             // A1
        .p12(ADRD_13_H),             // A2
        .p13(PC_FUN_1_H),            // A3
        .p14(PC_FUN_0_H),            // A4
        .p15(GND),                   // CE_N
        .p16(PLUS_5V)                // VCC
    );

    // E48 ttl_74154, zone 5
    ttl_74154 E48 (
        .p1(READ_INPUT_0_L),         // Y0_N
        .p2(READ_INPUT_1_L),         // Y1_N
        .p3(READ_INPUT_2_L),         // Y2_N
        .p4(READ_INPUT_3_L),         // Y3_N
        .p5(READ_INPUT_4_L),         // Y4_N
        .p6(READ_INPUT_5_L),         // Y5_N
        .p7(READ_INPUT_6_L),         // Y6_N
        .p8(READ_INPUT_7_L),         // Y7_N
        .p9(E48_9),                  // Y8_N
        .p10(E48_10),                // Y9_N
        .p11(E48_11),                // Y10_N
        .p12(GND),                   // GND
        .p13(E48_13),                // Y11_N
        .p14(E48_14),                // Y12_N
        .p15(E48_15),                // Y13_N
        .p16(E48_16),                // Y14_N
        .p17(),                      // Y15_N
        .p18(E34_7),                 // G1_N
        .p19(E34_7),                 // G2_N
        .p20(E26_8),                 // D
        .p21(ADRD_11_H),             // C
        .p22(ADRD_10_H),             // B
        .p23(ADRD_9_H),              // A
        .p24(PLUS_5V)                // VCC
    );

    // E47 ttl_7404, zone 4
    ttl_7404 E47 (
        .p1(E48_15),                 // A1
        .p2(LD_REG_5_H),             // Y1
        .p3(E48_14),                 // A2
        .p4(LD_REG_4_H),             // Y2
        .p5(E48_13),                 // A3
        .p6(LD_REG_3_H),             // Y3
        .p7(GND),                    // GND
        .p8(LD_REG_0_H),             // Y4
        .p9(E48_9),                  // A4
        .p10(LD_REG_2_H),            // Y5
        .p11(E48_11),                // A5
        .p12(LD_REG_1_H),            // Y6
        .p13(E48_10),                // A6
        .p14(PLUS_5V)                // VCC
    );

    // E52 ttl_74175, zone 3
    ttl_74175 E52 (
        .p1(PUP_L),                  // CLR_N
        .p2(E52_2),                  // Q1
        .p3(),                       // Q1_N
        .p4(ADRD_0_H),               // D1
        .p5(ADRD_1_H),               // D2
        .p6(),                       // Q2_N
        .p7(E52_7),                  // Q2
        .p8(GND),                    // GND
        .p9(LD_REG_5_H),             // CLK
        .p10(),                      // Q3
        .p11(E52_11),                // Q3_N
        .p12(ADRD_2_H),              // D3
        .p13(ADRD_3_H),              // D4
        .p14(),                      // Q4_N
        .p15(E52_15),                // Q4
        .p16(PLUS_5V)                // VCC
    );

    // E64 ttl_74175, zone 3
    ttl_74175 E64 (
        .p1(PUP_L),                  // CLR_N
        .p2(),                       // Q1
        .p3(TAKE_BUS_L),             // Q1_N
        .p4(ADRD_4_H),               // D1
        .p5(ADRD_5_H),               // D2
        .p6(CLR_BUS_L),              // Q2_N
        .p7(),                       // Q2
        .p8(GND),                    // GND
        .p9(LD_REG_5_H),             // CLK
        .p10(),                      // Q3
        .p11(E64_11),                // Q3_N
        .p12(ADRD_6_H),              // D3
        .p13(ADRD_7_H),              // D4
        .p14(E64_14),                // Q4_N
        .p15(),                      // Q4
        .p16(PLUS_5V)                // VCC
    );

    // E68 ttl_74175, zone 3
    ttl_74175 E68 (
        .p1(PUP_L),                  // CLR_N
        .p2(),                       // Q1
        .p3(E68_3),                  // Q1_N
        .p4(ADRD_4_H),               // D1
        .p5(ADRD_5_H),               // D2
        .p6(E68_6),                  // Q2_N
        .p7(),                       // Q2
        .p8(GND),                    // GND
        .p9(E40_12),                 // CLK
        .p10(),                      // Q3
        .p11(MAINT_1_L),             // Q3_N
        .p12(ADRD_6_H),              // D3
        .p13(ADRD_7_H),              // D4
        .p14(E68_14),                // Q4_N
        .p15(),                      // Q4
        .p16(PLUS_5V)                // VCC
    );

    // E69 ttl_7417, zone 2
    ttl_7417 E69 (
        .p1(E64_14),                 // A1
        .p2(MAN_CLK_L),              // Y1
        .p3(E68_3),                  // A2
        .p4(BUS_ER_L),               // Y2
        .p5(E68_6),                  // A3
        .p6(SR_DISP_L),              // Y3
        .p7(GND),                    // GND
        .p8(MAN_CLK_EN_L),           // Y4
        .p9(E64_11),                 // A4
        .p10(MAINT_MD_L),            // Y5
        .p11(MAINT_1_L),             // A5
        .p12(BOOT_L),                // Y6
        .p13(E68_14),                // A6
        .p14(PLUS_5V)                // VCC
    );

// KY-5, PDF page 11

    // E9 ttl_74175, zone 6
    ttl_74175 E9 (
        .p1(PUP_1_L),                // CLR_N
        .p2(E9_2),                   // Q1
        .p3(),                       // Q1_N
        .p4(ADRD_0_H),               // D1
        .p5(ADRD_1_H),               // D2
        .p6(),                       // Q2_N
        .p7(E9_7),                   // Q2
        .p8(GND),                    // GND
        .p9(LD_REG_3_H),             // CLK
        .p10(E9_10),                 // Q3
        .p11(),                      // Q3_N
        .p12(ADRD_2_H),              // D3
        .p13(ADRD_3_H),              // D4
        .p14(),                      // Q4_N
        .p15(E9_15),                 // Q4
        .p16(PLUS_5V)                // VCC
    );

    // E19 ttl_74175, zone 6
    ttl_74175 E19 (
        .p1(PUP_1_L),                // CLR_N
        .p2(E19_2),                  // Q1
        .p3(),                       // Q1_N
        .p4(ADRD_4_H),               // D1
        .p5(ADRD_5_H),               // D2
        .p6(),                       // Q2_N
        .p7(E19_7),                  // Q2
        .p8(GND),                    // GND
        .p9(LD_REG_3_H),             // CLK
        .p10(E19_10),                // Q3
        .p11(),                      // Q3_N
        .p12(ADRD_6_H),              // D3
        .p13(ADRD_7_H),              // D4
        .p14(),                      // Q4_N
        .p15(E19_15),                // Q4
        .p16(PLUS_5V)                // VCC
    );

    // E2 ttl_74175, zone 6
    ttl_74175 E2 (
        .p1(PUP_1_L),                // CLR_N
        .p2(E2_2),                   // Q1
        .p3(),                       // Q1_N
        .p4(ADRD_0_H),               // D1
        .p5(ADRD_1_H),               // D2
        .p6(),                       // Q2_N
        .p7(E2_7),                   // Q2
        .p8(GND),                    // GND
        .p9(LD_REG_4_H),             // CLK
        .p10(E2_10),                 // Q3
        .p11(),                      // Q3_N
        .p12(ADRD_2_H),              // D3
        .p13(ADRD_3_H),              // D4
        .p14(),                      // Q4_N
        .p15(E2_15),                 // Q4
        .p16(PLUS_5V)                // VCC
    );

    // E15 ttl_74175, zone 6
    ttl_74175 E15 (
        .p1(PUP_1_L),                // CLR_N
        .p2(E15_2),                  // Q1
        .p3(),                       // Q1_N
        .p4(ADRD_4_H),               // D1
        .p5(ADRD_5_H),               // D2
        .p6(),                       // Q2_N
        .p7(E15_7),                  // Q2
        .p8(GND),                    // GND
        .p9(LD_REG_4_H),             // CLK
        .p10(E15_10),                // Q3
        .p11(),                      // Q3_N
        .p12(ADRD_6_H),              // D3
        .p13(ADRD_7_H),              // D4
        .p14(),                      // Q4_N
        .p15(E15_15),                // Q4
        .p16(PLUS_5V)                // VCC
    );

    // E7 ds8641, zone 4
    ds8641 E7 (
        .p1(BUS_D00_L),              // BUS3
        .p2(E9_2),                   // IN3
        .p3(BB_D00_H),               // OUT3
        .p4(BUS_D01_L),              // BUS4
        .p5(E9_7),                   // IN4
        .p6(BB_D01_H),               // OUT4
        .p7(GND),                    // DISABLE_B
        .p8(GND),                    // GND
        .p9(EN_DB_TO_BUS_L),         // DISABLE_A
        .p10(BB_D02_H),              // OUT2
        .p11(E9_10),                 // IN2
        .p12(BUS_D02_L),             // BUS2
        .p13(BB_D03_H),              // OUT1
        .p14(E9_15),                 // IN1
        .p15(BUS_D03_L),             // BUS1
        .p16(PLUS_5V)                // VCC
    );

    // E25 ds8641, zone 4
    ds8641 E25 (
        .p1(BUS_D04_L),              // BUS3
        .p2(E19_2),                  // IN3
        .p3(BB_D04_H),               // OUT3
        .p4(BUS_D05_L),              // BUS4
        .p5(E19_7),                  // IN4
        .p6(BB_D05_H),               // OUT4
        .p7(GND),                    // DISABLE_B
        .p8(GND),                    // GND
        .p9(EN_DB_TO_BUS_L),         // DISABLE_A
        .p10(BB_D06_H),              // OUT2
        .p11(E19_10),                // IN2
        .p12(BUS_D06_L),             // BUS2
        .p13(BB_D07_H),              // OUT1
        .p14(E19_15),                // IN1
        .p15(BUS_D07_L),             // BUS1
        .p16(PLUS_5V)                // VCC
    );

    // E8 ds8641, zone 4
    ds8641 E8 (
        .p1(BUS_D08_L),              // BUS3
        .p2(E2_2),                   // IN3
        .p3(BB_D08_H),               // OUT3
        .p4(BUS_D09_L),              // BUS4
        .p5(E2_7),                   // IN4
        .p6(BB_D09_H),               // OUT4
        .p7(GND),                    // DISABLE_B
        .p8(GND),                    // GND
        .p9(EN_DB_TO_BUS_L),         // DISABLE_A
        .p10(BB_D10_H),              // OUT2
        .p11(E2_10),                 // IN2
        .p12(BUS_D10_L),             // BUS2
        .p13(BB_D11_H),              // OUT1
        .p14(E2_15),                 // IN1
        .p15(BUS_D11_L),             // BUS1
        .p16(PLUS_5V)                // VCC
    );

    // E13 ds8641, zone 4
    ds8641 E13 (
        .p1(BUS_D12_L),              // BUS3
        .p2(E15_2),                  // IN3
        .p3(BB_D12_H),               // OUT3
        .p4(BUS_D13_L),              // BUS4
        .p5(E15_7),                  // IN4
        .p6(BB_D13_H),               // OUT4
        .p7(GND),                    // DISABLE_B
        .p8(GND),                    // GND
        .p9(EN_DB_TO_BUS_L),         // DISABLE_A
        .p10(BB_D14_H),              // OUT2
        .p11(E15_10),                // IN2
        .p12(BUS_D14_L),             // BUS2
        .p13(BB_D15_H),              // OUT1
        .p14(E15_15),                // IN1
        .p15(BUS_D15_L),             // BUS1
        .p16(PLUS_5V)                // VCC
    );

// KY-6, PDF page 12

    // E67 ttl_74175, zone 6
    ttl_74175 E67 (
        .p1(PUP_L),                  // CLR_N
        .p2(E67_2),                  // Q1
        .p3(SCAN_1_L),               // Q1_N
        .p4(ADRD_0_H),               // D1
        .p5(ADRD_1_H),               // D2
        .p6(SCAN_2_L),               // Q2_N
        .p7(E67_7),                  // Q2
        .p8(GND),                    // GND
        .p9(LD_REG_0_H),             // CLK
        .p10(E67_10),                // Q3
        .p11(SCAN_3_L),              // Q3_N
        .p12(ADRD_2_H),              // D3
        .p13(ADRD_3_H),              // D4
        .p14(SCAN_4_L),              // Q4_N
        .p15(E67_15),                // Q4
        .p16(PLUS_5V)                // VCC
    );

    // E57 ttl_74175, zone 6
    ttl_74175 E57 (
        .p1(PUP_L),                  // CLR_N
        .p2(E57_2),                  // Q1
        .p3(SCAN_5_L),               // Q1_N
        .p4(ADRD_4_H),               // D1
        .p5(ADRD_5_H),               // D2
        .p6(SCAN_6_L),               // Q2_N
        .p7(E57_7),                  // Q2
        .p8(GND),                    // GND
        .p9(LD_REG_0_H),             // CLK
        .p10(E57_10),                // Q3
        .p11(),                      // Q3_N
        .p12(ADRD_6_H),              // D3
        .p13(ADRD_7_H),              // D4
        .p14(),                      // Q4_N
        .p15(E57_15),                // Q4
        .p16(PLUS_5V)                // VCC
    );

    // E73 ttl_74175, zone 6
    ttl_74175 E73 (
        .p1(PUP_L),                  // CLR_N
        .p2(NUM_1_H),                // Q1
        .p3(),                       // Q1_N
        .p4(ADRD_0_H),               // D1
        .p5(ADRD_1_H),               // D2
        .p6(),                       // Q2_N
        .p7(NUM_2_H),                // Q2
        .p8(GND),                    // GND
        .p9(LD_REG_1_H),             // CLK
        .p10(NUM_3_H),               // Q3
        .p11(),                      // Q3_N
        .p12(ADRD_2_H),              // D3
        .p13(ADRD_3_H),              // D4
        .p14(),                      // Q4_N
        .p15(E73_15),                // Q4
        .p16(PLUS_5V)                // VCC
    );

    // E58 ttl_74175, zone 6
    ttl_74175 E58 (
        .p1(PUP_L),                  // CLR_N
        .p2(E58_2),                  // Q1
        .p3(),                       // Q1_N
        .p4(ADRD_4_H),               // D1
        .p5(ADRD_5_H),               // D2
        .p6(),                       // Q2_N
        .p7(E58_7),                  // Q2
        .p8(GND),                    // GND
        .p9(LD_REG_1_H),             // CLK
        .p10(E58_10),                // Q3
        .p11(),                      // Q3_N
        .p12(ADRD_6_H),              // D3
        .p13(ADRD_7_H),              // D4
        .p14(),                      // Q4_N
        .p15(E58_15),                // Q4
        .p16(PLUS_5V)                // VCC
    );

    // E61 ds8641, zone 4
    ds8641 E61 (
        .p1(BUS_A00_L),              // BUS3
        .p2(E67_2),                  // IN3
        .p3(BB_A00_H),               // OUT3
        .p4(BUS_A01_L),              // BUS4
        .p5(E67_7),                  // IN4
        .p6(BB_A01_H),               // OUT4
        .p7(GND),                    // DISABLE_B
        .p8(GND),                    // GND
        .p9(EN_AR_TO_BUS_L),         // DISABLE_A
        .p10(BB_A02_H),              // OUT2
        .p11(E67_10),                // IN2
        .p12(BUS_A02_L),             // BUS2
        .p13(BB_A03_H),              // OUT1
        .p14(E67_15),                // IN1
        .p15(BUS_A03_L),             // BUS1
        .p16(PLUS_5V)                // VCC
    );

    // E56 ds8641, zone 4
    ds8641 E56 (
        .p1(BUS_A04_L),              // BUS3
        .p2(E57_2),                  // IN3
        .p3(BB_A04_H),               // OUT3
        .p4(BUS_A05_L),              // BUS4
        .p5(E57_7),                  // IN4
        .p6(BB_A05_H),               // OUT4
        .p7(GND),                    // DISABLE_B
        .p8(GND),                    // GND
        .p9(EN_AR_TO_BUS_L),         // DISABLE_A
        .p10(BB_A06_H),              // OUT2
        .p11(E57_10),                // IN2
        .p12(BUS_A06_L),             // BUS2
        .p13(BB_A07_H),              // OUT1
        .p14(E57_15),                // IN1
        .p15(BUS_A07_L),             // BUS1
        .p16(PLUS_5V)                // VCC
    );

    // E65 ds8641, zone 4
    ds8641 E65 (
        .p1(BUS_A08_L),              // BUS3
        .p2(NUM_1_H),                // IN3
        .p3(BB_A08_H),               // OUT3
        .p4(BUS_A09_L),              // BUS4
        .p5(NUM_2_H),                // IN4
        .p6(BB_A09_H),               // OUT4
        .p7(GND),                    // DISABLE_B
        .p8(GND),                    // GND
        .p9(EN_AR_TO_BUS_L),         // DISABLE_A
        .p10(BB_A10_H),              // OUT2
        .p11(NUM_3_H),               // IN2
        .p12(BUS_A10_L),             // BUS2
        .p13(BB_A11_H),              // OUT1
        .p14(E73_15),                // IN1
        .p15(BUS_A11_L),             // BUS1
        .p16(PLUS_5V)                // VCC
    );

    // E55 ds8641, zone 4
    ds8641 E55 (
        .p1(BUS_A12_L),              // BUS3
        .p2(E58_2),                  // IN3
        .p3(BB_A12_H),               // OUT3
        .p4(BUS_A13_L),              // BUS4
        .p5(E58_7),                  // IN4
        .p6(BB_A13_H),               // OUT4
        .p7(GND),                    // DISABLE_B
        .p8(GND),                    // GND
        .p9(EN_AR_TO_BUS_L),         // DISABLE_A
        .p10(BB_A14_H),              // OUT2
        .p11(E58_10),                // IN2
        .p12(BUS_A14_L),             // BUS2
        .p13(BB_A15_H),              // OUT1
        .p14(E58_15),                // IN1
        .p15(BUS_A15_L),             // BUS1
        .p16(PLUS_5V)                // VCC
    );

// KY-7, PDF page 13

    // E51 ttl_74175, zone 6
    ttl_74175 E51 (
        .p1(PUP_L),                  // CLR_N
        .p2(E51_2),                  // Q1
        .p3(),                       // Q1_N
        .p4(ADRD_0_H),               // D1
        .p5(ADRD_1_H),               // D2
        .p6(),                       // Q2_N
        .p7(E51_7),                  // Q2
        .p8(GND),                    // GND
        .p9(LD_REG_2_H),             // CLK
        .p10(EN_AR_TO_BUS_H),        // Q3
        .p11(EN_AR_TO_BUS_L),        // Q3_N
        .p12(ADRD_2_H),              // D3
        .p13(ADRD_3_H),              // D4
        .p14(),                      // Q4_N
        .p15(E51_15),                // Q4
        .p16(PLUS_5V)                // VCC
    );

    // E50 ds8641, zone 4
    ds8641 E50 (
        .p1(BUS_A16_L),              // BUS3
        .p2(E51_2),                  // IN3
        .p3(BB_A16_H),               // OUT3
        .p4(BUS_A17_L),              // BUS4
        .p5(E51_7),                  // IN4
        .p6(BB_A17_H),               // OUT4
        .p7(GND),                    // DISABLE_B
        .p8(GND),                    // GND
        .p9(EN_AR_TO_BUS_L),         // DISABLE_A
        .p10(),                      // OUT2
        .p11(),                      // IN2
        .p12(),                      // BUS2
        .p13(BB_C1_H),               // OUT1
        .p14(E51_15),                // IN1
        .p15(BUS_C1_L),              // BUS1
        .p16(PLUS_5V)                // VCC
    );

    // E72 ttl_7417, zone 4
    ttl_7417 E72 (
        .p1(SCAN_5_L),               // A1
        .p2(SCAN_5_OUT),             // Y1
        .p3(SCAN_1_L),               // A2
        .p4(SCAN_1_OUT),             // Y2
        .p5(SCAN_2_L),               // A3
        .p6(SCAN_2_OUT),             // Y3
        .p7(GND),                    // GND
        .p8(SCAN_6_OUT),             // Y4
        .p9(SCAN_6_L),               // A4
        .p10(SCAN_3_OUT),            // Y5
        .p11(SCAN_3_L),              // A5
        .p12(SCAN_4_OUT),            // Y6
        .p13(SCAN_4_L),              // A6
        .p14(PLUS_5V)                // VCC
    );

// KY-8, PDF page 14

    // R19 10 kohm, zone 7
    resistor R19 (
        .p1(PLUS_5V),                // END1
        .p2(J2_B)                    // END2
    );

    // R18 10 kohm, zone 7
    resistor R18 (
        .p1(PLUS_5V),                // END1
        .p2(J2_C)                    // END2
    );

    // R17 10 kohm, zone 7
    resistor R17 (
        .p1(PLUS_5V),                // END1
        .p2(J2_E)                    // END2
    );

    // R16 10 kohm, zone 7
    resistor R16 (
        .p1(PLUS_5V),                // END1
        .p2(J2_H)                    // END2
    );

    // R15 10 kohm, zone 7
    resistor R15 (
        .p1(PLUS_5V),                // END1
        .p2(J2_A)                    // END2
    );

    // R14 10 kohm, zone 7
    resistor R14 (
        .p1(PLUS_5V),                // END1
        .p2(J2_D)                    // END2
    );

    // R13 10 kohm, zone 7
    resistor R13 (
        .p1(PLUS_5V),                // END1
        .p2(J2_F)                    // END2
    );

    // R12 10 kohm, zone 7
    resistor R12 (
        .p1(PLUS_5V),                // END1
        .p2(J2_J)                    // END2
    );

    // R11 10 kohm, zone 7
    resistor R11 (
        .p1(PLUS_5V),                // END1
        .p2(J3_A)                    // END2
    );

    // R10 10 kohm, zone 7
    resistor R10 (
        .p1(PLUS_5V),                // END1
        .p2(J3_B)                    // END2
    );

    // R9 10 kohm, zone 7
    resistor R9 (
        .p1(PLUS_5V),                // END1
        .p2(J3_C)                    // END2
    );

    // R8 10 kohm, zone 7
    resistor R8 (
        .p1(PLUS_5V),                // END1
        .p2(J3_D)                    // END2
    );

    // E54 dm8837, zone 6
    dm8837 E54 (
        .p1(J2_B),                   // IN4
        .p2(CONN_0_H),               // OUT4
        .p3(J2_C),                   // IN5
        .p4(CONN_1_H),               // OUT5
        .p5(J2_E),                   // IN6
        .p6(CONN_2_H),               // OUT6
        .p7(GND),                    // DISABLE_B
        .p8(GND),                    // GND
        .p9(GND),                    // DISABLE_A
        .p10(CONN_3_H),              // OUT3
        .p11(J2_H),                  // IN3
        .p12(CONN_4_H),              // OUT2
        .p13(J2_A),                  // IN2
        .p14(CONN_5_H),              // OUT1
        .p15(J2_D),                  // IN1
        .p16(PLUS_5V)                // VCC
    );

    // E49 dm8837, zone 6
    dm8837 E49 (
        .p1(J2_F),                   // IN4
        .p2(CONN_6_H),               // OUT4
        .p3(J2_J),                   // IN5
        .p4(CONN_7_H),               // OUT5
        .p5(J3_A),                   // IN6
        .p6(CONN_8_H),               // OUT6
        .p7(GND),                    // DISABLE_B
        .p8(GND),                    // GND
        .p9(GND),                    // DISABLE_A
        .p10(CONN_9_H),              // OUT3
        .p11(J3_B),                  // IN3
        .p12(CONN_10_H),             // OUT2
        .p13(J3_C),                  // IN2
        .p14(CONN_11_H),             // OUT1
        .p15(J3_D),                  // IN1
        .p16(PLUS_5V)                // VCC
    );

    // E53 ttl_7408, zone 4
    ttl_7408 E53 (
        .p1(CLR_BUS_L),              // A1
        .p2(E1_12),                  // B1
        .p3(E53_3),                  // Y1
        .p4(E1_2),                   // A2
        .p5(E30_4),                  // B2
        .p6(E53_6),                  // Y2
        .p7(GND),                    // GND
        .p8(),                       // Y3
        .p9(),                       // A3
        .p10(),                      // B3
        .p11(),                      // Y4
        .p12(),                      // A4
        .p13(),                      // B4
        .p14(PLUS_5V)                // VCC
    );

    // E63 ttl_7474, zone 3
    ttl_7474 E63 (
        .p1(E53_3),                  // CLR1_N
        .p2(GND),                    // D1
        .p3(GND),                    // CLK1
        .p4(E1_2),                   // PRE1_N
        .p5(E63_5),                  // Q1
        .p6(),                       // Q1_N
        .p7(GND),                    // GND
        .p8(),                       // Q2_N
        .p9(HALT_BUSY_H),            // Q2
        .p10(TAKE_BUS_L),            // PRE2_N
        .p11(E53_6),                 // CLK2
        .p12(E63_5),                 // D2
        .p13(E53_3),                 // CLR2_N
        .p14(PLUS_5V)                // VCC
    );

    // R21 220 ohm, zone 4
    resistor R21 (
        .p1(PLUS_5V),                // END1
        .p2(PLUS_3V)                 // END2
    );

    // R22 750 ohm, zone 4
    resistor R22 (
        .p1(PLUS_3V),                // END1
        .p2(GND)                     // END2
    );

// KY-9, PDF page 15

    // E75 ttl_7474, zone 7
    ttl_7474 E75 (
        .p1(E76_11),                 // CLR1_N
        .p2(BB_SSYN_H),              // D1
        .p3(OSC_CLK),                // CLK1
        .p4(PLUS_3V),                // PRE1_N
        .p5(),                       // Q1
        .p6(E75_6),                  // Q1_N
        .p7(GND),                    // GND
        .p8(SSYN_REC_H),             // Q2_N
        .p9(SSYN_REC_L),             // Q2
        .p10(E76_11),                // PRE2_N
        .p11(OSC_CLK),               // CLK2
        .p12(E76_8),                 // D2
        .p13(PLUS_3V),               // CLR2_N
        .p14(PLUS_5V)                // VCC
    );

    // E76 ttl_7408, zone 6
    ttl_7408 E76 (
        .p1(),                       // A1
        .p2(),                       // B1
        .p3(),                       // Y1
        .p4(MAINT_1_L),              // A2
        .p5(E75_6),                  // B2
        .p6(E76_6),                  // Y2
        .p7(GND),                    // GND
        .p8(E76_8),                  // Y3
        .p9(SSYN_REC_L),             // A3
        .p10(E75_6),                 // B3
        .p11(E76_11),                // Y4
        .p12(EN_AR_TO_BUS_H),        // A4
        .p13(PUP_1_L),               // B4
        .p14(PLUS_5V)                // VCC
    );

    // E20 ttl_74173, zone 3
    ttl_74173 E20 (
        .p1(GND),                    // M
        .p2(READ_INPUT_0_L),         // N
        .p3(DIN_3_H),                // Q1
        .p4(DIN_2_H),                // Q2
        .p5(DIN_1_H),                // Q3
        .p6(DIN_0_H),                // Q4
        .p7(OSC_CLK),                // CLK
        .p8(GND),                    // GND
        .p9(SSYN_REC_H),             // G1_N
        .p10(E76_6),                 // G2_N
        .p11(BB_D00_H),              // D4
        .p12(BB_D01_H),              // D3
        .p13(BB_D02_H),              // D2
        .p14(BB_D03_H),              // D1
        .p15(GND),                   // CLR
        .p16(PLUS_5V)                // VCC
    );

    // E31 ttl_74173, zone 3
    ttl_74173 E31 (
        .p1(GND),                    // M
        .p2(READ_INPUT_0_L),         // N
        .p3(DIN_7_H),                // Q1
        .p4(DIN_6_H),                // Q2
        .p5(DIN_5_H),                // Q3
        .p6(DIN_4_H),                // Q4
        .p7(OSC_CLK),                // CLK
        .p8(GND),                    // GND
        .p9(SSYN_REC_H),             // G1_N
        .p10(E76_6),                 // G2_N
        .p11(BB_D04_H),              // D4
        .p12(BB_D05_H),              // D3
        .p13(BB_D06_H),              // D2
        .p14(BB_D07_H),              // D1
        .p15(GND),                   // CLR
        .p16(PLUS_5V)                // VCC
    );

    // E14 ttl_74173, zone 3
    ttl_74173 E14 (
        .p1(GND),                    // M
        .p2(READ_INPUT_1_L),         // N
        .p3(DIN_3_H),                // Q1
        .p4(DIN_2_H),                // Q2
        .p5(DIN_1_H),                // Q3
        .p6(DIN_0_H),                // Q4
        .p7(OSC_CLK),                // CLK
        .p8(GND),                    // GND
        .p9(SSYN_REC_H),             // G1_N
        .p10(E76_6),                 // G2_N
        .p11(BB_D08_H),              // D4
        .p12(BB_D09_H),              // D3
        .p13(BB_D10_H),              // D2
        .p14(BB_D11_H),              // D1
        .p15(GND),                   // CLR
        .p16(PLUS_5V)                // VCC
    );

    // E37 ttl_74173, zone 3
    ttl_74173 E37 (
        .p1(GND),                    // M
        .p2(READ_INPUT_1_L),         // N
        .p3(DIN_7_H),                // Q1
        .p4(DIN_6_H),                // Q2
        .p5(DIN_5_H),                // Q3
        .p6(DIN_4_H),                // Q4
        .p7(OSC_CLK),                // CLK
        .p8(GND),                    // GND
        .p9(SSYN_REC_H),             // G1_N
        .p10(E76_6),                 // G2_N
        .p11(BB_D12_H),              // D4
        .p12(BB_D13_H),              // D3
        .p13(BB_D14_H),              // D2
        .p14(BB_D15_H),              // D1
        .p15(GND),                   // CLR
        .p16(PLUS_5V)                // VCC
    );

endmodule
`default_nettype wire
