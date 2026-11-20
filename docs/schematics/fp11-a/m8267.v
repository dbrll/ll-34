`default_nettype none
// Verilog transcription of the M8267 FP11-A floating point board
// From DEC FP11-A FIELD MAINTENANCE PRINT SET, sheets FP1 to FP10
// http://wwcm.synology.me/pdf/MP00189%20FP11-A%20Field%20Maintenance%20Print%20Set.pdf

module fp11_a_m8267 (
    (* edge_contact = "DA1" *) inout wire B_PORT_0_H,
    (* edge_contact = "DS1" *) inout wire B_PORT_1_H,
    (* edge_contact = "DU1" *) inout wire B_PORT_2_H,
    (* edge_contact = "CM1" *) inout wire B_PORT_3_H,
    (* edge_contact = "EL2" *) inout wire A_PORT_0_H,
    (* edge_contact = "DR1" *) inout wire A_PORT_1_H,
    (* edge_contact = "DP1" *) inout wire A_PORT_2_H,
    (* edge_contact = "DB1" *) inout wire A_PORT_3_H,
    (* edge_contact = "CE1" *) inout wire F_SRC_0_H,
    (* edge_contact = "CF1" *) inout wire F_SRC_1_H,
    (* edge_contact = "CK1" *) inout wire F_SRC_2_H,
    (* edge_contact = "EM2" *) inout wire F_ALU_0_H,
    (* edge_contact = "ER2" *) inout wire F_ALU_1_H,
    (* edge_contact = "EM1" *) inout wire F_ALU_2_H,
    (* edge_contact = "FM2" *) inout wire F_I6_H,
    (* edge_contact = "FR2" *) inout wire F_I7_H,
    (* edge_contact = "FS2" *) inout wire F_I8_H,
    (* edge_contact = "EN1" *) inout wire BYTE_0_ENB_L,
    (* edge_contact = "DV1" *) inout wire BYTE_7_ENB_L,
    (* edge_contact = "DM1" *) inout wire EX_ALU_0_H,
    (* edge_contact = "DV2" *) inout wire EX_ALU_1_H,
    (* edge_contact = "DU2" *) inout wire EX_ALU_2_H,
    (* edge_contact = "DN1" *) inout wire EX_I6_H,
    (* edge_contact = "DJ1" *) inout wire EX_I7_H,
    (* edge_contact = "DC1" *) inout wire EX_I8_H,
    (* edge_contact = "FR1" *) inout wire BYTE_2_ENB_L,
    (* edge_contact = "ES2" *) inout wire BYTE_1_ENB_L,
    (* edge_contact = "FH2" *) inout wire BYTE_4_ENB_L,
    (* edge_contact = "FV2" *) inout wire BYTE_3_ENB_L,
    (* edge_contact = "FU2" *) inout wire BYTE_6_ENB_L,
    (* edge_contact = "FK2" *) inout wire BYTE_5_ENB_L,
    (* edge_contact = "AA2,BA2,CA2,DA2,EA2,FA2", connector_contact = "J5-K" *) inout wire P5V,
    (* edge_contact = "AC2,BC2,CC2,DC2,EC2,FC2,AT1,BT1,CT1,DT1,ET1,FT1", connector_contact = "J4-C,J4-H,J5-H,J1-D,J1-J,J1-N,J1-R,J1-X,J1-BB,J1-HH,J1-PP,J1-SS,J1-VV" *) inout wire GND,
    (* connector_contact = "J1-E" *) inout wire A_MUX_0_H,
    (* connector_contact = "J1-P" *) inout wire A_MUX_1_H,
    (* connector_contact = "J1-K" *) inout wire A_MUX_2_H,
    (* connector_contact = "J1-TT" *) inout wire A_MUX_3_H,
    (* connector_contact = "J1-Y" *) inout wire A_MUX_4_H,
    (* connector_contact = "J1-U" *) inout wire A_MUX_5_H,
    (* connector_contact = "J1-AA" *) inout wire A_MUX_6_H,
    (* connector_contact = "J1-LL" *) inout wire A_MUX_7_H,
    (* connector_contact = "J1-H" *) inout wire A_MUX_8_H,
    (* connector_contact = "J1-W" *) inout wire A_MUX_9_H,
    (* connector_contact = "J1-A" *) inout wire A_MUX_10_H,
    (* connector_contact = "J1-C" *) inout wire A_MUX_11_H,
    (* connector_contact = "J1-NN" *) inout wire A_MUX_12_H,
    (* connector_contact = "J1-JJ" *) inout wire A_MUX_13_H,
    (* connector_contact = "J1-CC" *) inout wire A_MUX_14_H,
    (* connector_contact = "J1-KK" *) inout wire A_MUX_15_H,
    (* connector_contact = "J2-B,J3-B" *) inout wire MPC_0_L,
    (* connector_contact = "J2-C,J3-C" *) inout wire MPC_1_L,
    (* connector_contact = "J2-E,J3-E" *) inout wire MPC_2_L,
    (* connector_contact = "J2-H,J3-H" *) inout wire MPC_3_L,
    (* connector_contact = "J2-A,J3-A" *) inout wire MPC_4_L,
    (* connector_contact = "J2-D,J3-D" *) inout wire MPC_5_L,
    (* connector_contact = "J2-F,J3-F" *) inout wire MPC_6_L,
    (* connector_contact = "J2-J,J3-J" *) inout wire MPC_7_L,
    (* connector_contact = "J2-K,J3-K" *) inout wire J2_K,
    (* connector_contact = "J2-L,J3-L" *) inout wire J2_L,
    (* connector_contact = "J4-A,J5-A" *) inout wire MPC_8_L,
    (* connector_contact = "J4-B,J5-B" *) inout wire MPC_9_L,
    (* connector_contact = "J4-D" *) inout wire SERVICE_BR_PFAIL_L,
    (* connector_contact = "J4-F" *) inout wire LOAD_IR_L,
    (* connector_contact = "J4-J" *) inout wire PFAIL_BR_PEND_H,
    (* connector_contact = "J1-RR" *) inout wire K15_1134_CLK_L,
    (* connector_contact = "J1-EE" *) inout wire PROC_INIT_L,
    (* connector_contact = "J1-S" *) inout wire TRI_STATE_AMUX_L,
    (* edge_contact = "BB2" *) inout wire BUT_DIS_L,
    (* edge_contact = "DS2,DT2" *) inout wire BG_4_IN_H,
    (* edge_contact = "DP2,DR2" *) inout wire BG_5_IN_H,
    (* edge_contact = "DM2,DN2" *) inout wire BG_6_IN_H,
    (* edge_contact = "DK2,DL2" *) inout wire BG_7_IN_H,
    (* edge_contact = "CA1,CB1" *) inout wire NPG_IN_H,
    (* edge_contact = "AB2" *) inout wire BYTE_DIS_L
);

    wire A_MUX_IN_L;
    wire A_REG_0_H;
    wire A_REG_1_H;
    wire A_REG_2_H;
    wire A_SEL_H;
    wire B_PROC_CLK_H;
    wire B_PROC_CLK_L;
    wire B_PROC_INIT_H;
    wire B_PROC_INIT_L;
    wire B_REG_0_H;
    wire B_REG_1_H;
    wire B_REG_2_H;
    wire B_SEL_0_H;
    wire B_SEL_1_H;
    wire BUT_0_H;
    wire BUT_1_H;
    wire BUT_2_H;
    wire BUT_3_H;
    wire BUT_4_H;
    wire BUT_5_H;
    wire C_IN_47_H;
    wire C_OUT_63_H;
    wire CIN_0_H;
    wire CIN_15_H;
    wire CIN_31_H;
    wire E10_6;
    wire E10_8;
    wire E13_6;
    wire E17_1;
    wire E18_10;
    wire E18_15;
    wire E19_1;
    wire E19_2;
    wire E19_3;
    wire E19_4;
    wire E19_5;
    wire E19_6;
    wire E19_7;
    wire E19_9;
    wire E21_1;
    wire E21_2;
    wire E21_3;
    wire E21_4;
    wire E21_5;
    wire E21_6;
    wire E21_7;
    wire E21_9;
    wire E22_11;
    wire E22_3;
    wire E22_6;
    wire E24_1;
    wire E24_10;
    wire E24_13;
    wire E26_10;
    wire E26_14;
    wire E26_2;
    wire E26_7;
    wire E27_2;
    wire E30_6;
    wire E30_8;
    wire E33_4;
    wire E34_10;
    wire E35_10;
    wire E36_10;
    wire E36_11;
    wire E36_12;
    wire E36_9;
    wire E37_10;
    wire E37_11;
    wire E37_12;
    wire E37_9;
    wire E38_10;
    wire E38_11;
    wire E38_12;
    wire E38_9;
    wire E4_11;
    wire E4_12;
    wire E4_3;
    wire E40_10;
    wire E40_11;
    wire E40_12;
    wire E40_9;
    wire E41_10;
    wire E41_11;
    wire E41_12;
    wire E41_9;
    wire E42_10;
    wire E42_11;
    wire E42_12;
    wire E42_9;
    wire E43_10;
    wire E43_11;
    wire E43_12;
    wire E43_9;
    wire E44_10;
    wire E44_11;
    wire E44_12;
    wire E44_9;
    wire E45_10;
    wire E45_11;
    wire E45_12;
    wire E45_9;
    wire E46_10;
    wire E46_11;
    wire E46_12;
    wire E46_9;
    wire E47_10;
    wire E47_11;
    wire E47_12;
    wire E47_9;
    wire E48_10;
    wire E48_11;
    wire E48_12;
    wire E48_9;
    wire E49_10;
    wire E49_2;
    wire E49_5;
    wire E49_7;
    wire E53_10;
    wire E53_12;
    wire E53_2;
    wire E53_5;
    wire E53_7;
    wire E61_16;
    wire E61_32;
    wire E61_35;
    wire E62_21;
    wire E62_29;
    wire E62_32;
    wire E62_35;
    wire E62_9;
    wire E63_10;
    wire E63_7;
    wire E77_32;
    wire E77_35;
    wire E78_21;
    wire E78_29;
    wire E78_32;
    wire E78_35;
    wire E78_9;
    wire E79_21;
    wire E79_29;
    wire E79_32;
    wire E79_35;
    wire E79_9;
    wire E80_29;
    wire E80_32;
    wire E80_35;
    wire E80_9;
    wire E81_21;
    wire E81_29;
    wire E81_32;
    wire E81_35;
    wire E81_9;
    wire E82_21;
    wire E82_29;
    wire E82_32;
    wire E82_35;
    wire E82_9;
    wire E83_21;
    wire E83_29;
    wire E83_32;
    wire E83_35;
    wire E83_9;
    wire E84_16;
    wire E84_32;
    wire E84_35;
    wire E85_32;
    wire E85_35;
    wire E86_21;
    wire E86_29;
    wire E86_32;
    wire E86_35;
    wire E86_9;
    wire E87_21;
    wire E87_29;
    wire E87_32;
    wire E87_35;
    wire E87_9;
    wire E88_29;
    wire E88_32;
    wire E88_35;
    wire E88_9;
    wire E89_21;
    wire E89_29;
    wire E89_32;
    wire E89_35;
    wire E89_9;
    wire E90_16;
    wire E90_21;
    wire E90_29;
    wire E90_32;
    wire E90_35;
    wire E90_9;
    wire EN_BIT_H;
    wire EX_SRC_0_H;
    wire EX_SRC_1_H;
    wire EX_SRC_2_H;
    wire EZ_BIT_H;
    wire F_INSERT_0_L;
    wire FAST_COUT_H;
    wire FD_1_H;
    wire FL_1_H;
    wire FP9_1OR0_SEL;
    wire FPS_CLK_L;
    wire FPX_L;
    wire FZA_BIT_H;
    wire FZB_BIT_H;
    wire FZC_BIT_H;
    wire G_31_H;
    wire G_47_H;
    wire G_63_H;
    wire G15_H;
    wire IN_H_ENB_L;
    wire IN_L_ENB_L;
    wire IR_0_H;
    wire IR_1_H;
    wire IR_10_H;
    wire IR_11_H;
    wire IR_2_H;
    wire IR_3_H;
    wire IR_4_H;
    wire IR_5_H;
    wire IR_6_H;
    wire IR_7_H;
    wire IR_8_H;
    wire IR_9_H;
    wire MISC_1_H;
    wire MISC_2_H;
    wire MISC_3_H;
    wire MISC_4_H;
    wire MPC_0_H;
    wire MPC_1_H;
    wire MPC_2_H;
    wire MPC_3_H;
    wire MPC_4_H;
    wire MPC_5_H;
    wire MPC_6_H;
    wire MPC_7_H;
    wire MPC_8_H;
    wire N_63_BIT_H;
    wire OUT_ENB_L;
    wire P_31_H;
    wire P_47_H;
    wire P_63_H;
    wire P15_H;
    wire P3V;
    wire Q_31_H;
    wire Q_47_H;
    wire Q15_H;
    wire QN_8_40_H;
    wire QN40_H;
    wire QN8_H;
    wire RAM_0_H;
    wire RAM_15_H;
    wire RAM_31_H;
    wire RAM_47_H;
    wire RAM_63_H;
    wire ROTATE_L_L;
    wire ROTATE_R_L;
    wire SECTOR_0_CLK_L;
    wire SECTOR_1_CLK_L;
    wire SECTOR_2_CLK_L;
    wire SECTOR_3_CLK_L;
    wire SHIFT_L_DOUB_L;
    wire SHIFT_L_L;
    wire SHIFT_L_SING_L;
    wire SHIFT_R_L;
    wire T_BUS_0_H;
    wire T_BUS_1_H;
    wire T_BUS_10_H;
    wire T_BUS_11_H;
    wire T_BUS_12_H;
    wire T_BUS_13_H;
    wire T_BUS_14_H;
    wire T_BUS_15_H;
    wire T_BUS_2_H;
    wire T_BUS_3_H;
    wire T_BUS_4_H;
    wire T_BUS_5_H;
    wire T_BUS_6_H;
    wire T_BUS_7_H;
    wire T_BUS_8_H;
    wire T_BUS_9_H;
    wire T_BUS_OUT_H;
    wire T_BUS_OUT_L;
    wire Z_BIT_H;

// FP1 FLOATING POINT PROCESSOR, BYTES 7 AND 0, PDF page 16

    // E82 am2901, zone B7
    am2901 E82 (
        .p1(A_PORT_3_H),             // A3
        .p2(A_PORT_2_H),             // A2
        .p3(A_PORT_1_H),             // A1
        .p4(A_PORT_0_H),             // A0
        .p5(F_I6_H),                 // I6
        .p6(F_I8_H),                 // I8
        .p7(F_I7_H),                 // I7
        .p8(RAM_15_H),               // RAM3
        .p9(E82_9),                  // RAM0
        .p10(P5V),                   // VCC
        .p11(FZA_BIT_H),             // F_ZERO
        .p12(F_SRC_0_H),             // I0
        .p13(F_SRC_1_H),             // I1
        .p14(F_SRC_2_H),             // I2
        .p15(SECTOR_0_CLK_L),        // CP
        .p16(Q15_H),                 // Q3
        .p17(B_PORT_0_H),            // B0
        .p18(B_PORT_1_H),            // B1
        .p19(B_PORT_2_H),            // B2
        .p20(B_PORT_3_H),            // B3
        .p21(E82_21),                // Q0
        .p22(T_BUS_7_H),             // D3
        .p23(T_BUS_6_H),             // D2
        .p24(T_BUS_5_H),             // D1
        .p25(T_BUS_4_H),             // D0
        .p26(F_ALU_0_H),             // I3
        .p27(F_ALU_2_H),             // I5
        .p28(F_ALU_1_H),             // I4
        .p29(E82_29),                // CN
        .p30(GND),                   // GND
        .p31(),                      // F3
        .p32(E82_32),                // G_N
        .p33(),                      // CN4
        .p34(),                      // OVR
        .p35(E82_35),                // P_N
        .p36(T_BUS_4_H),             // Y0
        .p37(T_BUS_5_H),             // Y1
        .p38(T_BUS_6_H),             // Y2
        .p39(T_BUS_7_H),             // Y3
        .p40(BYTE_0_ENB_L)           // OE_N
    );

    // E80 am2901, zone B5
    am2901 E80 (
        .p1(A_PORT_3_H),             // A3
        .p2(A_PORT_2_H),             // A2
        .p3(A_PORT_1_H),             // A1
        .p4(A_PORT_0_H),             // A0
        .p5(F_I6_H),                 // I6
        .p6(F_I8_H),                 // I8
        .p7(F_I7_H),                 // I7
        .p8(E82_9),                  // RAM3
        .p9(E80_9),                  // RAM0
        .p10(P5V),                   // VCC
        .p11(FZB_BIT_H),             // F_ZERO
        .p12(F_SRC_0_H),             // I0
        .p13(F_SRC_1_H),             // I1
        .p14(F_SRC_2_H),             // I2
        .p15(SECTOR_0_CLK_L),        // CP
        .p16(E82_21),                // Q3
        .p17(B_PORT_0_H),            // B0
        .p18(B_PORT_1_H),            // B1
        .p19(B_PORT_2_H),            // B2
        .p20(B_PORT_3_H),            // B3
        .p21(QN8_H),                 // Q0
        .p22(T_BUS_3_H),             // D3
        .p23(T_BUS_2_H),             // D2
        .p24(T_BUS_1_H),             // D1
        .p25(T_BUS_0_H),             // D0
        .p26(F_ALU_0_H),             // I3
        .p27(F_ALU_2_H),             // I5
        .p28(F_ALU_1_H),             // I4
        .p29(E80_29),                // CN
        .p30(GND),                   // GND
        .p31(),                      // F3
        .p32(E80_32),                // G_N
        .p33(),                      // CN4
        .p34(),                      // OVR
        .p35(E80_35),                // P_N
        .p36(T_BUS_0_H),             // Y0
        .p37(T_BUS_1_H),             // Y1
        .p38(T_BUS_2_H),             // Y2
        .p39(T_BUS_3_H),             // Y3
        .p40(BYTE_0_ENB_L)           // OE_N
    );

    // E62 am2901, zone B4
    am2901 E62 (
        .p1(A_PORT_3_H),             // A3
        .p2(A_PORT_2_H),             // A2
        .p3(A_PORT_1_H),             // A1
        .p4(A_PORT_0_H),             // A0
        .p5(EX_I6_H),                // I6
        .p6(EX_I8_H),                // I8
        .p7(EX_I7_H),                // I7
        .p8(E80_9),                  // RAM3
        .p9(E62_9),                  // RAM0
        .p10(P5V),                   // VCC
        .p11(EZ_BIT_H),              // F_ZERO
        .p12(EX_SRC_0_H),            // I0
        .p13(EX_SRC_1_H),            // I1
        .p14(EX_SRC_2_H),            // I2
        .p15(SECTOR_3_CLK_L),        // CP
        .p16(QN8_H),                 // Q3
        .p17(B_PORT_0_H),            // B0
        .p18(B_PORT_1_H),            // B1
        .p19(B_PORT_2_H),            // B2
        .p20(B_PORT_3_H),            // B3
        .p21(E62_21),                // Q0
        .p22(T_BUS_15_H),            // D3
        .p23(T_BUS_14_H),            // D2
        .p24(T_BUS_13_H),            // D1
        .p25(T_BUS_12_H),            // D0
        .p26(EX_ALU_0_H),            // I3
        .p27(EX_ALU_2_H),            // I5
        .p28(EX_ALU_1_H),            // I4
        .p29(E62_29),                // CN
        .p30(GND),                   // GND
        .p31(EN_BIT_H),              // F3
        .p32(E62_32),                // G_N
        .p33(),                      // CN4
        .p34(),                      // OVR
        .p35(E62_35),                // P_N
        .p36(T_BUS_12_H),            // Y0
        .p37(T_BUS_13_H),            // Y1
        .p38(T_BUS_14_H),            // Y2
        .p39(T_BUS_15_H),            // Y3
        .p40(BYTE_7_ENB_L)           // OE_N
    );

    // E61 am2901, zone B2
    // Pin 21 Q0 is drawn as a stub with no destination.
    am2901 E61 (
        .p1(A_PORT_3_H),             // A3
        .p2(A_PORT_2_H),             // A2
        .p3(A_PORT_1_H),             // A1
        .p4(A_PORT_0_H),             // A0
        .p5(EX_I6_H),                // I6
        .p6(EX_I8_H),                // I8
        .p7(EX_I7_H),                // I7
        .p8(E62_9),                  // RAM3
        .p9(RAM_0_H),                // RAM0
        .p10(P5V),                   // VCC
        .p11(EZ_BIT_H),              // F_ZERO
        .p12(EX_SRC_0_H),            // I0
        .p13(EX_SRC_1_H),            // I1
        .p14(EX_SRC_2_H),            // I2
        .p15(SECTOR_3_CLK_L),        // CP
        .p16(E61_16),                // Q3
        .p17(B_PORT_0_H),            // B0
        .p18(B_PORT_1_H),            // B1
        .p19(B_PORT_2_H),            // B2
        .p20(B_PORT_3_H),            // B3
        .p21(),                      // Q0
        .p22(T_BUS_11_H),            // D3
        .p23(T_BUS_10_H),            // D2
        .p24(T_BUS_9_H),             // D1
        .p25(T_BUS_8_H),             // D0
        .p26(EX_ALU_0_H),            // I3
        .p27(EX_ALU_2_H),            // I5
        .p28(EX_ALU_1_H),            // I4
        .p29(CIN_0_H),               // CN
        .p30(GND),                   // GND
        .p31(),                      // F3
        .p32(E61_32),                // G_N
        .p33(),                      // CN4
        .p34(),                      // OVR
        .p35(E61_35),                // P_N
        .p36(T_BUS_8_H),             // Y0
        .p37(T_BUS_9_H),             // Y1
        .p38(T_BUS_10_H),            // Y2
        .p39(T_BUS_11_H),            // Y3
        .p40(BYTE_7_ENB_L)           // OE_N
    );

    // E72 ttl_74s182, zone A5
    ttl_74s182 E72 (
        .p1(E62_32),                 // G1_N
        .p2(E62_35),                 // P1_N
        .p3(E61_32),                 // G0_N
        .p4(E61_35),                 // P0_N
        .p5(E82_32),                 // G3_N
        .p6(E82_35),                 // P3_N
        .p7(P15_H),                  // P_N
        .p8(GND),                    // GND
        .p9(E82_29),                 // CNZ
        .p10(G15_H),                 // G_N
        .p11(E80_29),                // CNY
        .p12(E62_29),                // CNX
        .p13(CIN_0_H),               // CN
        .p14(E80_32),                // G2_N
        .p15(E80_35),                // P2_N
        .p16(P5V)                    // VCC
    );

    // E51 dm8093, zone B4
    dm8093 E51 (
        .p1(SHIFT_L_DOUB_L),         // C1
        .p2(FAST_COUT_H),            // A1
        .p3(E62_21),                 // Y1
        .p4(SHIFT_L_L),              // C2
        .p5(GND),                    // A2
        .p6(RAM_0_H),                // Y2
        .p7(GND),                    // GND
        .p8(E80_9),                  // Y3
        .p9(GND),                    // A3
        .p10(F_INSERT_0_L),          // C3
        .p11(E61_16),                // Y4
        .p12(E62_21),                // A4
        .p13(SHIFT_R_L),             // C4
        .p14(P5V)                    // VCC
    );

    // E56 dm8093, zone A1, FP3 A3
    dm8093 E56 (
        .p1(SHIFT_L_SING_L),         // C1, FP3
        .p2(FAST_COUT_H),            // A1, FP3
        .p3(E79_21),                 // Y1, FP3
        .p4(ROTATE_L_L),             // C2
        .p5(RAM_63_H),               // A2
        .p6(RAM_0_H),                // Y2
        .p7(GND),                    // GND
        .p8(E79_21),                 // Y3, FP3
        .p9(E84_16),                 // A3, FP3
        .p10(SHIFT_L_DOUB_L),        // C3, FP3
        .p11(E84_16),                // Y4, FP3
        .p12(E79_21),                // A4, FP3
        .p13(SHIFT_R_L),             // C4, FP3
        .p14(P5V)                    // VCC
    );

    // R2 470 ohm, zone B3
    resistor R2 (
        .p1(P5V),                    // END1
        .p2(EZ_BIT_H)                // END2
    );

// FP2 FLOATING POINT PROCESSOR, BYTES 1 AND 2, PDF page 17

    // E89 am2901, zone B7
    am2901 E89 (
        .p1(A_PORT_3_H),             // A3
        .p2(A_PORT_2_H),             // A2
        .p3(A_PORT_1_H),             // A1
        .p4(A_PORT_0_H),             // A0
        .p5(F_I6_H),                 // I6
        .p6(F_I8_H),                 // I8
        .p7(F_I7_H),                 // I7
        .p8(RAM_31_H),               // RAM3
        .p9(E89_9),                  // RAM0
        .p10(P5V),                   // VCC
        .p11(FZA_BIT_H),             // F_ZERO
        .p12(F_SRC_0_H),             // I0
        .p13(F_SRC_1_H),             // I1
        .p14(F_SRC_2_H),             // I2
        .p15(SECTOR_1_CLK_L),        // CP
        .p16(Q_31_H),                // Q3
        .p17(B_PORT_0_H),            // B0
        .p18(B_PORT_1_H),            // B1
        .p19(B_PORT_2_H),            // B2
        .p20(B_PORT_3_H),            // B3
        .p21(E89_21),                // Q0
        .p22(T_BUS_7_H),             // D3
        .p23(T_BUS_6_H),             // D2
        .p24(T_BUS_5_H),             // D1
        .p25(T_BUS_4_H),             // D0
        .p26(F_ALU_0_H),             // I3
        .p27(F_ALU_2_H),             // I5
        .p28(F_ALU_1_H),             // I4
        .p29(E89_29),                // CN
        .p30(GND),                   // GND
        .p31(),                      // F3
        .p32(E89_32),                // G_N
        .p33(),                      // CN4
        .p34(),                      // OVR
        .p35(E89_35),                // P_N
        .p36(T_BUS_4_H),             // Y0
        .p37(T_BUS_5_H),             // Y1
        .p38(T_BUS_6_H),             // Y2
        .p39(T_BUS_7_H),             // Y3
        .p40(BYTE_2_ENB_L)           // OE_N
    );

    // E87 am2901, zone B5
    am2901 E87 (
        .p1(A_PORT_3_H),             // A3
        .p2(A_PORT_2_H),             // A2
        .p3(A_PORT_1_H),             // A1
        .p4(A_PORT_0_H),             // A0
        .p5(F_I6_H),                 // I6
        .p6(F_I8_H),                 // I8
        .p7(F_I7_H),                 // I7
        .p8(E89_9),                  // RAM3
        .p9(E87_9),                  // RAM0
        .p10(P5V),                   // VCC
        .p11(FZB_BIT_H),             // F_ZERO
        .p12(F_SRC_0_H),             // I0
        .p13(F_SRC_1_H),             // I1
        .p14(F_SRC_2_H),             // I2
        .p15(SECTOR_1_CLK_L),        // CP
        .p16(E89_21),                // Q3
        .p17(B_PORT_0_H),            // B0
        .p18(B_PORT_1_H),            // B1
        .p19(B_PORT_2_H),            // B2
        .p20(B_PORT_3_H),            // B3
        .p21(E87_21),                // Q0
        .p22(T_BUS_3_H),             // D3
        .p23(T_BUS_2_H),             // D2
        .p24(T_BUS_1_H),             // D1
        .p25(T_BUS_0_H),             // D0
        .p26(F_ALU_0_H),             // I3
        .p27(F_ALU_2_H),             // I5
        .p28(F_ALU_1_H),             // I4
        .p29(E87_29),                // CN
        .p30(GND),                   // GND
        .p31(),                      // F3
        .p32(E87_32),                // G_N
        .p33(),                      // CN4
        .p34(),                      // OVR
        .p35(E87_35),                // P_N
        .p36(T_BUS_0_H),             // Y0
        .p37(T_BUS_1_H),             // Y1
        .p38(T_BUS_2_H),             // Y2
        .p39(T_BUS_3_H),             // Y3
        .p40(BYTE_2_ENB_L)           // OE_N
    );

    // E78 am2901, zone B4
    am2901 E78 (
        .p1(A_PORT_3_H),             // A3
        .p2(A_PORT_2_H),             // A2
        .p3(A_PORT_1_H),             // A1
        .p4(A_PORT_0_H),             // A0
        .p5(F_I6_H),                 // I6
        .p6(F_I8_H),                 // I8
        .p7(F_I7_H),                 // I7
        .p8(E87_9),                  // RAM3
        .p9(E78_9),                  // RAM0
        .p10(P5V),                   // VCC
        .p11(FZC_BIT_H),             // F_ZERO
        .p12(F_SRC_0_H),             // I0
        .p13(F_SRC_1_H),             // I1
        .p14(F_SRC_2_H),             // I2
        .p15(SECTOR_0_CLK_L),        // CP
        .p16(E87_21),                // Q3
        .p17(B_PORT_0_H),            // B0
        .p18(B_PORT_1_H),            // B1
        .p19(B_PORT_2_H),            // B2
        .p20(B_PORT_3_H),            // B3
        .p21(E78_21),                // Q0
        .p22(T_BUS_15_H),            // D3
        .p23(T_BUS_14_H),            // D2
        .p24(T_BUS_13_H),            // D1
        .p25(T_BUS_12_H),            // D0
        .p26(F_ALU_0_H),             // I3
        .p27(F_ALU_2_H),             // I5
        .p28(F_ALU_1_H),             // I4
        .p29(E78_29),                // CN
        .p30(GND),                   // GND
        .p31(),                      // F3
        .p32(E78_32),                // G_N
        .p33(),                      // CN4
        .p34(),                      // OVR
        .p35(E78_35),                // P_N
        .p36(T_BUS_12_H),            // Y0
        .p37(T_BUS_13_H),            // Y1
        .p38(T_BUS_14_H),            // Y2
        .p39(T_BUS_15_H),            // Y3
        .p40(BYTE_1_ENB_L)           // OE_N
    );

    // E77 am2901, zone B2
    // Pin 11 is labeled FP2 FZC BIT H, FP3 and FP8 use FP1 FZC BIT H. Taken
    // as one net, pulled up by R9.
    am2901 E77 (
        .p1(A_PORT_3_H),             // A3
        .p2(A_PORT_2_H),             // A2
        .p3(A_PORT_1_H),             // A1
        .p4(A_PORT_0_H),             // A0
        .p5(F_I6_H),                 // I6
        .p6(F_I8_H),                 // I8
        .p7(F_I7_H),                 // I7
        .p8(E78_9),                  // RAM3
        .p9(RAM_15_H),               // RAM0
        .p10(P5V),                   // VCC
        .p11(FZC_BIT_H),             // F_ZERO
        .p12(F_SRC_0_H),             // I0
        .p13(F_SRC_1_H),             // I1
        .p14(F_SRC_2_H),             // I2
        .p15(SECTOR_0_CLK_L),        // CP
        .p16(E78_21),                // Q3
        .p17(B_PORT_0_H),            // B0
        .p18(B_PORT_1_H),            // B1
        .p19(B_PORT_2_H),            // B2
        .p20(B_PORT_3_H),            // B3
        .p21(Q15_H),                 // Q0
        .p22(T_BUS_11_H),            // D3
        .p23(T_BUS_10_H),            // D2
        .p24(T_BUS_9_H),             // D1
        .p25(T_BUS_8_H),             // D0
        .p26(F_ALU_0_H),             // I3
        .p27(F_ALU_2_H),             // I5
        .p28(F_ALU_1_H),             // I4
        .p29(CIN_15_H),              // CN
        .p30(GND),                   // GND
        .p31(),                      // F3
        .p32(E77_32),                // G_N
        .p33(),                      // CN4
        .p34(),                      // OVR
        .p35(E77_35),                // P_N
        .p36(T_BUS_8_H),             // Y0
        .p37(T_BUS_9_H),             // Y1
        .p38(T_BUS_10_H),            // Y2
        .p39(T_BUS_11_H),            // Y3
        .p40(BYTE_1_ENB_L)           // OE_N
    );

    // E71 ttl_74s182, zone A4
    ttl_74s182 E71 (
        .p1(E78_32),                 // G1_N
        .p2(E78_35),                 // P1_N
        .p3(E77_32),                 // G0_N
        .p4(E77_35),                 // P0_N
        .p5(E89_32),                 // G3_N
        .p6(E89_35),                 // P3_N
        .p7(P_31_H),                 // P_N
        .p8(GND),                    // GND
        .p9(E89_29),                 // CNZ
        .p10(G_31_H),                // G_N
        .p11(E87_29),                // CNY
        .p12(E78_29),                // CNX
        .p13(CIN_15_H),              // CN
        .p14(E87_32),                // G2_N
        .p15(E87_35),                // P2_N
        .p16(P5V)                    // VCC
    );

    // R9 470 ohm, zone B3
    resistor R9 (
        .p1(P5V),                    // END1
        .p2(FZC_BIT_H)               // END2
    );

// FP3 FLOATING POINT PROCESSOR, BYTES 3 AND 4, PDF page 18

    // E83 am2901, zone B7
    am2901 E83 (
        .p1(A_PORT_3_H),             // A3
        .p2(A_PORT_2_H),             // A2
        .p3(A_PORT_1_H),             // A1
        .p4(A_PORT_0_H),             // A0
        .p5(F_I6_H),                 // I6
        .p6(F_I8_H),                 // I8
        .p7(F_I7_H),                 // I7
        .p8(RAM_47_H),               // RAM3
        .p9(E83_9),                  // RAM0
        .p10(P5V),                   // VCC
        .p11(FZA_BIT_H),             // F_ZERO
        .p12(F_SRC_0_H),             // I0
        .p13(F_SRC_1_H),             // I1
        .p14(F_SRC_2_H),             // I2
        .p15(SECTOR_2_CLK_L),        // CP
        .p16(Q_47_H),                // Q3
        .p17(B_PORT_0_H),            // B0
        .p18(B_PORT_1_H),            // B1
        .p19(B_PORT_2_H),            // B2
        .p20(B_PORT_3_H),            // B3
        .p21(E83_21),                // Q0
        .p22(T_BUS_7_H),             // D3
        .p23(T_BUS_6_H),             // D2
        .p24(T_BUS_5_H),             // D1
        .p25(T_BUS_4_H),             // D0
        .p26(F_ALU_0_H),             // I3
        .p27(F_ALU_2_H),             // I5
        .p28(F_ALU_1_H),             // I4
        .p29(E83_29),                // CN
        .p30(GND),                   // GND
        .p31(),                      // F3
        .p32(E83_32),                // G_N
        .p33(),                      // CN4
        .p34(),                      // OVR
        .p35(E83_35),                // P_N
        .p36(T_BUS_4_H),             // Y0
        .p37(T_BUS_5_H),             // Y1
        .p38(T_BUS_6_H),             // Y2
        .p39(T_BUS_7_H),             // Y3
        .p40(BYTE_4_ENB_L)           // OE_N
    );

    // E88 am2901, zone B5
    am2901 E88 (
        .p1(A_PORT_3_H),             // A3
        .p2(A_PORT_2_H),             // A2
        .p3(A_PORT_1_H),             // A1
        .p4(A_PORT_0_H),             // A0
        .p5(F_I6_H),                 // I6
        .p6(F_I8_H),                 // I8
        .p7(F_I7_H),                 // I7
        .p8(E83_9),                  // RAM3
        .p9(E88_9),                  // RAM0
        .p10(P5V),                   // VCC
        .p11(FZB_BIT_H),             // F_ZERO
        .p12(F_SRC_0_H),             // I0
        .p13(F_SRC_1_H),             // I1
        .p14(F_SRC_2_H),             // I2
        .p15(SECTOR_2_CLK_L),        // CP
        .p16(E83_21),                // Q3
        .p17(B_PORT_0_H),            // B0
        .p18(B_PORT_1_H),            // B1
        .p19(B_PORT_2_H),            // B2
        .p20(B_PORT_3_H),            // B3
        .p21(QN40_H),                // Q0
        .p22(T_BUS_3_H),             // D3
        .p23(T_BUS_2_H),             // D2
        .p24(T_BUS_1_H),             // D1
        .p25(T_BUS_0_H),             // D0
        .p26(F_ALU_0_H),             // I3
        .p27(F_ALU_2_H),             // I5
        .p28(F_ALU_1_H),             // I4
        .p29(E88_29),                // CN
        .p30(GND),                   // GND
        .p31(),                      // F3
        .p32(E88_32),                // G_N
        .p33(),                      // CN4
        .p34(),                      // OVR
        .p35(E88_35),                // P_N
        .p36(T_BUS_0_H),             // Y0
        .p37(T_BUS_1_H),             // Y1
        .p38(T_BUS_2_H),             // Y2
        .p39(T_BUS_3_H),             // Y3
        .p40(BYTE_4_ENB_L)           // OE_N
    );

    // E79 am2901, zone B4
    am2901 E79 (
        .p1(A_PORT_3_H),             // A3
        .p2(A_PORT_2_H),             // A2
        .p3(A_PORT_1_H),             // A1
        .p4(A_PORT_0_H),             // A0
        .p5(F_I6_H),                 // I6
        .p6(F_I8_H),                 // I8
        .p7(F_I7_H),                 // I7
        .p8(E88_9),                  // RAM3
        .p9(E79_9),                  // RAM0
        .p10(P5V),                   // VCC
        .p11(FZC_BIT_H),             // F_ZERO
        .p12(F_SRC_0_H),             // I0
        .p13(F_SRC_1_H),             // I1
        .p14(F_SRC_2_H),             // I2
        .p15(SECTOR_1_CLK_L),        // CP
        .p16(QN40_H),                // Q3
        .p17(B_PORT_0_H),            // B0
        .p18(B_PORT_1_H),            // B1
        .p19(B_PORT_2_H),            // B2
        .p20(B_PORT_3_H),            // B3
        .p21(E79_21),                // Q0
        .p22(T_BUS_15_H),            // D3
        .p23(T_BUS_14_H),            // D2
        .p24(T_BUS_13_H),            // D1
        .p25(T_BUS_12_H),            // D0
        .p26(F_ALU_0_H),             // I3
        .p27(F_ALU_2_H),             // I5
        .p28(F_ALU_1_H),             // I4
        .p29(E79_29),                // CN
        .p30(GND),                   // GND
        .p31(),                      // F3
        .p32(E79_32),                // G_N
        .p33(),                      // CN4
        .p34(),                      // OVR
        .p35(E79_35),                // P_N
        .p36(T_BUS_12_H),            // Y0
        .p37(T_BUS_13_H),            // Y1
        .p38(T_BUS_14_H),            // Y2
        .p39(T_BUS_15_H),            // Y3
        .p40(BYTE_3_ENB_L)           // OE_N
    );

    // E84 am2901, zone B2
    am2901 E84 (
        .p1(A_PORT_3_H),             // A3
        .p2(A_PORT_2_H),             // A2
        .p3(A_PORT_1_H),             // A1
        .p4(A_PORT_0_H),             // A0
        .p5(F_I6_H),                 // I6
        .p6(F_I8_H),                 // I8
        .p7(F_I7_H),                 // I7
        .p8(E79_9),                  // RAM3
        .p9(RAM_31_H),               // RAM0
        .p10(P5V),                   // VCC
        .p11(FZB_BIT_H),             // F_ZERO
        .p12(F_SRC_0_H),             // I0
        .p13(F_SRC_1_H),             // I1
        .p14(F_SRC_2_H),             // I2
        .p15(SECTOR_1_CLK_L),        // CP
        .p16(E84_16),                // Q3
        .p17(B_PORT_0_H),            // B0
        .p18(B_PORT_1_H),            // B1
        .p19(B_PORT_2_H),            // B2
        .p20(B_PORT_3_H),            // B3
        .p21(Q_31_H),                // Q0
        .p22(T_BUS_11_H),            // D3
        .p23(T_BUS_10_H),            // D2
        .p24(T_BUS_9_H),             // D1
        .p25(T_BUS_8_H),             // D0
        .p26(F_ALU_0_H),             // I3
        .p27(F_ALU_2_H),             // I5
        .p28(F_ALU_1_H),             // I4
        .p29(CIN_31_H),              // CN
        .p30(GND),                   // GND
        .p31(),                      // F3
        .p32(E84_32),                // G_N
        .p33(),                      // CN4
        .p34(),                      // OVR
        .p35(E84_35),                // P_N
        .p36(T_BUS_8_H),             // Y0
        .p37(T_BUS_9_H),             // Y1
        .p38(T_BUS_10_H),            // Y2
        .p39(T_BUS_11_H),            // Y3
        .p40(BYTE_3_ENB_L)           // OE_N
    );

    // E73 ttl_74s182, zone A6
    ttl_74s182 E73 (
        .p1(E79_32),                 // G1_N
        .p2(E79_35),                 // P1_N
        .p3(E84_32),                 // G0_N
        .p4(E84_35),                 // P0_N
        .p5(E83_32),                 // G3_N
        .p6(E83_35),                 // P3_N
        .p7(P_47_H),                 // P_N
        .p8(GND),                    // GND
        .p9(E83_29),                 // CNZ
        .p10(G_47_H),                // G_N
        .p11(E88_29),                // CNY
        .p12(E79_29),                // CNX
        .p13(CIN_31_H),              // CN
        .p14(E88_32),                // G2_N
        .p15(E88_35),                // P2_N
        .p16(P5V)                    // VCC
    );

// FP4 FLOATING POINT PROCESSOR, BYTES 5 AND 6, PDF page 19

    // E90 am2901, zone B7
    am2901 E90 (
        .p1(A_PORT_3_H),             // A3
        .p2(A_PORT_2_H),             // A2
        .p3(A_PORT_1_H),             // A1
        .p4(A_PORT_0_H),             // A0
        .p5(F_I6_H),                 // I6
        .p6(F_I8_H),                 // I8
        .p7(F_I7_H),                 // I7
        .p8(RAM_63_H),               // RAM3
        .p9(E90_9),                  // RAM0
        .p10(P5V),                   // VCC
        .p11(FZA_BIT_H),             // F_ZERO
        .p12(F_SRC_0_H),             // I0
        .p13(F_SRC_1_H),             // I1
        .p14(F_SRC_2_H),             // I2
        .p15(SECTOR_3_CLK_L),        // CP
        .p16(E90_16),                // Q3
        .p17(B_PORT_0_H),            // B0
        .p18(B_PORT_1_H),            // B1
        .p19(B_PORT_2_H),            // B2
        .p20(B_PORT_3_H),            // B3
        .p21(E90_21),                // Q0
        .p22(T_BUS_7_H),             // D3
        .p23(T_BUS_6_H),             // D2
        .p24(T_BUS_5_H),             // D1
        .p25(T_BUS_4_H),             // D0
        .p26(F_ALU_0_H),             // I3
        .p27(F_ALU_2_H),             // I5
        .p28(F_ALU_1_H),             // I4
        .p29(E90_29),                // CN
        .p30(GND),                   // GND
        .p31(N_63_BIT_H),            // F3
        .p32(E90_32),                // G_N
        .p33(C_OUT_63_H),            // CN4
        .p34(),                      // OVR
        .p35(E90_35),                // P_N
        .p36(T_BUS_4_H),             // Y0
        .p37(T_BUS_5_H),             // Y1
        .p38(T_BUS_6_H),             // Y2
        .p39(T_BUS_7_H),             // Y3
        .p40(BYTE_6_ENB_L)           // OE_N
    );

    // E81 am2901, zone B5
    am2901 E81 (
        .p1(A_PORT_3_H),             // A3
        .p2(A_PORT_2_H),             // A2
        .p3(A_PORT_1_H),             // A1
        .p4(A_PORT_0_H),             // A0
        .p5(F_I6_H),                 // I6
        .p6(F_I8_H),                 // I8
        .p7(F_I7_H),                 // I7
        .p8(E90_9),                  // RAM3
        .p9(E81_9),                  // RAM0
        .p10(P5V),                   // VCC
        .p11(FZA_BIT_H),             // F_ZERO
        .p12(F_SRC_0_H),             // I0
        .p13(F_SRC_1_H),             // I1
        .p14(F_SRC_2_H),             // I2
        .p15(SECTOR_3_CLK_L),        // CP
        .p16(E90_21),                // Q3
        .p17(B_PORT_0_H),            // B0
        .p18(B_PORT_1_H),            // B1
        .p19(B_PORT_2_H),            // B2
        .p20(B_PORT_3_H),            // B3
        .p21(E81_21),                // Q0
        .p22(T_BUS_3_H),             // D3
        .p23(T_BUS_2_H),             // D2
        .p24(T_BUS_1_H),             // D1
        .p25(T_BUS_0_H),             // D0
        .p26(F_ALU_0_H),             // I3
        .p27(F_ALU_2_H),             // I5
        .p28(F_ALU_1_H),             // I4
        .p29(E81_29),                // CN
        .p30(GND),                   // GND
        .p31(),                      // F3
        .p32(E81_32),                // G_N
        .p33(),                      // CN4
        .p34(),                      // OVR
        .p35(E81_35),                // P_N
        .p36(T_BUS_0_H),             // Y0
        .p37(T_BUS_1_H),             // Y1
        .p38(T_BUS_2_H),             // Y2
        .p39(T_BUS_3_H),             // Y3
        .p40(BYTE_6_ENB_L)           // OE_N
    );

    // E86 am2901, zone B4
    am2901 E86 (
        .p1(A_PORT_3_H),             // A3
        .p2(A_PORT_2_H),             // A2
        .p3(A_PORT_1_H),             // A1
        .p4(A_PORT_0_H),             // A0
        .p5(F_I6_H),                 // I6
        .p6(F_I8_H),                 // I8
        .p7(F_I7_H),                 // I7
        .p8(E81_9),                  // RAM3
        .p9(E86_9),                  // RAM0
        .p10(P5V),                   // VCC
        .p11(FZB_BIT_H),             // F_ZERO
        .p12(F_SRC_0_H),             // I0
        .p13(F_SRC_1_H),             // I1
        .p14(F_SRC_2_H),             // I2
        .p15(SECTOR_2_CLK_L),        // CP
        .p16(E81_21),                // Q3
        .p17(B_PORT_0_H),            // B0
        .p18(B_PORT_1_H),            // B1
        .p19(B_PORT_2_H),            // B2
        .p20(B_PORT_3_H),            // B3
        .p21(E86_21),                // Q0
        .p22(T_BUS_15_H),            // D3
        .p23(T_BUS_14_H),            // D2
        .p24(T_BUS_13_H),            // D1
        .p25(T_BUS_12_H),            // D0
        .p26(F_ALU_0_H),             // I3
        .p27(F_ALU_2_H),             // I5
        .p28(F_ALU_1_H),             // I4
        .p29(E86_29),                // CN
        .p30(GND),                   // GND
        .p31(),                      // F3
        .p32(E86_32),                // G_N
        .p33(),                      // CN4
        .p34(),                      // OVR
        .p35(E86_35),                // P_N
        .p36(T_BUS_12_H),            // Y0
        .p37(T_BUS_13_H),            // Y1
        .p38(T_BUS_14_H),            // Y2
        .p39(T_BUS_15_H),            // Y3
        .p40(BYTE_5_ENB_L)           // OE_N
    );

    // E85 am2901, zone B2
    am2901 E85 (
        .p1(A_PORT_3_H),             // A3
        .p2(A_PORT_2_H),             // A2
        .p3(A_PORT_1_H),             // A1
        .p4(A_PORT_0_H),             // A0
        .p5(F_I6_H),                 // I6
        .p6(F_I8_H),                 // I8
        .p7(F_I7_H),                 // I7
        .p8(E86_9),                  // RAM3
        .p9(RAM_47_H),               // RAM0
        .p10(P5V),                   // VCC
        .p11(FZB_BIT_H),             // F_ZERO
        .p12(F_SRC_0_H),             // I0
        .p13(F_SRC_1_H),             // I1
        .p14(F_SRC_2_H),             // I2
        .p15(SECTOR_2_CLK_L),        // CP
        .p16(E86_21),                // Q3
        .p17(B_PORT_0_H),            // B0
        .p18(B_PORT_1_H),            // B1
        .p19(B_PORT_2_H),            // B2
        .p20(B_PORT_3_H),            // B3
        .p21(Q_47_H),                // Q0
        .p22(T_BUS_11_H),            // D3
        .p23(T_BUS_10_H),            // D2
        .p24(T_BUS_9_H),             // D1
        .p25(T_BUS_8_H),             // D0
        .p26(F_ALU_0_H),             // I3
        .p27(F_ALU_2_H),             // I5
        .p28(F_ALU_1_H),             // I4
        .p29(C_IN_47_H),             // CN
        .p30(GND),                   // GND
        .p31(),                      // F3
        .p32(E85_32),                // G_N
        .p33(),                      // CN4
        .p34(),                      // OVR
        .p35(E85_35),                // P_N
        .p36(T_BUS_8_H),             // Y0
        .p37(T_BUS_9_H),             // Y1
        .p38(T_BUS_10_H),            // Y2
        .p39(T_BUS_11_H),            // Y3
        .p40(BYTE_5_ENB_L)           // OE_N
    );

    // E74 ttl_74s182, zone A4
    ttl_74s182 E74 (
        .p1(E86_32),                 // G1_N
        .p2(E86_35),                 // P1_N
        .p3(E85_32),                 // G0_N
        .p4(E85_35),                 // P0_N
        .p5(E90_32),                 // G3_N
        .p6(E90_35),                 // P3_N
        .p7(P_63_H),                 // P_N
        .p8(GND),                    // GND
        .p9(E90_29),                 // CNZ
        .p10(G_63_H),                // G_N
        .p11(E81_29),                // CNY
        .p12(E86_29),                // CNX
        .p13(C_IN_47_H),             // CN
        .p14(E81_32),                // G2_N
        .p15(E81_35),                // P2_N
        .p16(P5V)                    // VCC
    );

    // E58 dm8093, zone B8, FP9 B6
    // On FP4 the line from pin 6 to the RAM3 line of E90 is faint.
    dm8093 E58 (
        .p1(SHIFT_R_L),              // C1
        .p2(GND),                    // A1
        .p3(E90_16),                 // Y1
        .p4(SHIFT_R_L),              // C2
        .p5(FP9_1OR0_SEL),           // A2
        .p6(RAM_63_H),               // Y2
        .p7(GND),                    // GND
        .p8(RAM_63_H),               // Y3
        .p9(RAM_0_H),                // A3
        .p10(ROTATE_R_L),            // C3
        .p11(BUT_DIS_L),             // Y4, FP9
        .p12(GND),                   // A4, FP9
        .p13(FPX_L),                 // C4, FP9
        .p14(P5V)                    // VCC
    );

    // R10 470 ohm, zone B6
    resistor R10 (
        .p1(P5V),                    // END1
        .p2(FZA_BIT_H)               // END2
    );

    // R3 470 ohm, zone B2
    resistor R3 (
        .p1(P5V),                    // END1
        .p2(FZB_BIT_H)               // END2
    );

// FP5 FLOATING POINT PROCESSOR, LOW BYTE BUFFER, PDF page 20

    // E16 dm8097, zone D6, FP5 D5
    dm8097 E16 (
        .p1(T_BUS_OUT_L),            // G1_N_(A1_through_A4)
        .p2(T_BUS_2_H),              // A1
        .p3(A_MUX_2_H),              // Y1
        .p4(T_BUS_1_H),              // A2
        .p5(A_MUX_1_H),              // Y2
        .p6(T_BUS_0_H),              // A3
        .p7(A_MUX_0_H),              // Y3
        .p8(GND),                    // GND
        .p9(),                       // Y4
        .p10(),                      // A4
        .p11(T_BUS_1_H),             // Y5
        .p12(A_MUX_1_H),             // A5
        .p13(T_BUS_2_H),             // Y6
        .p14(A_MUX_2_H),             // A6
        .p15(A_MUX_IN_L),            // G2_N_(A5_and_A6)
        .p16(P5V)                    // VCC
    );

    // E6 dm8097, zone D5, FP5 C6
    dm8097 E6 (
        .p1(A_MUX_IN_L),             // G1_N_(A1_through_A4)
        .p2(A_MUX_0_H),              // A1
        .p3(T_BUS_0_H),              // Y1
        .p4(A_MUX_4_H),              // A2
        .p5(T_BUS_4_H),              // Y2
        .p6(A_MUX_5_H),              // A3
        .p7(T_BUS_5_H),              // Y3
        .p8(GND),                    // GND
        .p9(T_BUS_3_H),              // Y4
        .p10(A_MUX_3_H),             // A4
        .p11(A_MUX_3_H),             // Y5
        .p12(T_BUS_3_H),             // A5
        .p13(A_MUX_4_H),             // Y6
        .p14(T_BUS_4_H),             // A6
        .p15(T_BUS_OUT_L),           // G2_N_(A5_and_A6)
        .p16(P5V)                    // VCC
    );

    // E9 dm8097, zone B6, FP5 B5
    dm8097 E9 (
        .p1(T_BUS_OUT_L),            // G1_N_(A1_through_A4)
        .p2(T_BUS_7_H),              // A1
        .p3(A_MUX_7_H),              // Y1
        .p4(T_BUS_6_H),              // A2
        .p5(A_MUX_6_H),              // Y2
        .p6(T_BUS_5_H),              // A3
        .p7(A_MUX_5_H),              // Y3
        .p8(GND),                    // GND
        .p9(),                       // Y4
        .p10(),                      // A4
        .p11(T_BUS_6_H),             // Y5
        .p12(A_MUX_6_H),             // A5
        .p13(T_BUS_7_H),             // Y6
        .p14(A_MUX_7_H),             // A6
        .p15(A_MUX_IN_L),            // G2_N_(A5_and_A6)
        .p16(P5V)                    // VCC
    );

    // E25 ttl_74173, zone D2
    ttl_74173 E25 (
        .p1(OUT_ENB_L),              // M
        .p2(GND),                    // N
        .p3(T_BUS_3_H),              // Q1
        .p4(T_BUS_2_H),              // Q2
        .p5(T_BUS_1_H),              // Q3
        .p6(T_BUS_0_H),              // Q4
        .p7(B_PROC_CLK_L),           // CLK
        .p8(GND),                    // GND
        .p9(IN_L_ENB_L),             // G1_N
        .p10(GND),                   // G2_N
        .p11(T_BUS_0_H),             // D4
        .p12(T_BUS_1_H),             // D3
        .p13(T_BUS_2_H),             // D2
        .p14(T_BUS_3_H),             // D1
        .p15(B_PROC_INIT_H),         // CLR
        .p16(P5V)                    // VCC
    );

    // E15 ttl_74173, zone B2
    // Pin 9 is labeled FP7 IN H ENB L, E25 on the same byte uses FP7 IN L ENB
    // L.
    ttl_74173 E15 (
        .p1(OUT_ENB_L),              // M
        .p2(GND),                    // N
        .p3(T_BUS_7_H),              // Q1
        .p4(T_BUS_6_H),              // Q2
        .p5(T_BUS_5_H),              // Q3
        .p6(T_BUS_4_H),              // Q4
        .p7(B_PROC_CLK_L),           // CLK
        .p8(GND),                    // GND
        .p9(IN_H_ENB_L),             // G1_N
        .p10(GND),                   // G2_N
        .p11(T_BUS_4_H),             // D4
        .p12(T_BUS_5_H),             // D3
        .p13(T_BUS_6_H),             // D2
        .p14(T_BUS_7_H),             // D1
        .p15(B_PROC_INIT_H),         // CLR
        .p16(P5V)                    // VCC
    );

    // R1 220 ohm, zone B1
    resistor R1 (
        .p1(P5V),                    // END1
        .p2(P3V)                     // END2
    );

    // R6 750 ohm, zone B1
    resistor R6 (
        .p1(P3V),                    // END1
        .p2(GND)                     // END2
    );

// FP6 FLOATING POINT PROCESSOR, HIGH BYTE BUFFER, PDF page 21

    // E7 dm8097, zone D7, FP6 D6
    dm8097 E7 (
        .p1(T_BUS_OUT_L),            // G1_N_(A1_through_A4)
        .p2(T_BUS_10_H),             // A1
        .p3(A_MUX_10_H),             // Y1
        .p4(T_BUS_9_H),              // A2
        .p5(A_MUX_9_H),              // Y2
        .p6(T_BUS_8_H),              // A3
        .p7(A_MUX_8_H),              // Y3
        .p8(GND),                    // GND
        .p9(),                       // Y4
        .p10(),                      // A4
        .p11(T_BUS_9_H),             // Y5
        .p12(A_MUX_9_H),             // A5
        .p13(T_BUS_10_H),            // Y6
        .p14(A_MUX_10_H),            // A6
        .p15(A_MUX_IN_L),            // G2_N_(A5_and_A6)
        .p16(P5V)                    // VCC
    );

    // E5 dm8097, zone D6, FP6 C7
    dm8097 E5 (
        .p1(A_MUX_IN_L),             // G1_N_(A1_through_A4)
        .p2(A_MUX_8_H),              // A1
        .p3(T_BUS_8_H),              // Y1
        .p4(A_MUX_12_H),             // A2
        .p5(T_BUS_12_H),             // Y2
        .p6(A_MUX_13_H),             // A3
        .p7(T_BUS_13_H),             // Y3
        .p8(GND),                    // GND
        .p9(T_BUS_11_H),             // Y4
        .p10(A_MUX_11_H),            // A4
        .p11(A_MUX_11_H),            // Y5
        .p12(T_BUS_11_H),            // A5
        .p13(A_MUX_12_H),            // Y6
        .p14(T_BUS_12_H),            // A6
        .p15(T_BUS_OUT_L),           // G2_N_(A5_and_A6)
        .p16(P5V)                    // VCC
    );

    // E8 dm8097, zone B7, FP6 B6
    dm8097 E8 (
        .p1(T_BUS_OUT_L),            // G1_N_(A1_through_A4)
        .p2(T_BUS_15_H),             // A1
        .p3(A_MUX_15_H),             // Y1
        .p4(T_BUS_14_H),             // A2
        .p5(A_MUX_14_H),             // Y2
        .p6(T_BUS_13_H),             // A3
        .p7(A_MUX_13_H),             // Y3
        .p8(GND),                    // GND
        .p9(),                       // Y4
        .p10(),                      // A4
        .p11(T_BUS_14_H),            // Y5
        .p12(A_MUX_14_H),            // A5
        .p13(T_BUS_15_H),            // Y6
        .p14(A_MUX_15_H),            // A6
        .p15(A_MUX_IN_L),            // G2_N_(A5_and_A6)
        .p16(P5V)                    // VCC
    );

    // E3 ttl_74173, zone D3
    ttl_74173 E3 (
        .p1(OUT_ENB_L),              // M
        .p2(GND),                    // N
        .p3(T_BUS_11_H),             // Q1
        .p4(T_BUS_10_H),             // Q2
        .p5(T_BUS_9_H),              // Q3
        .p6(T_BUS_8_H),              // Q4
        .p7(B_PROC_CLK_L),           // CLK
        .p8(GND),                    // GND
        .p9(IN_H_ENB_L),             // G1_N
        .p10(GND),                   // G2_N
        .p11(T_BUS_8_H),             // D4
        .p12(T_BUS_9_H),             // D3
        .p13(T_BUS_10_H),            // D2
        .p14(T_BUS_11_H),            // D1
        .p15(B_PROC_INIT_H),         // CLR
        .p16(P5V)                    // VCC
    );

    // E14 ttl_74173, zone B3
    ttl_74173 E14 (
        .p1(OUT_ENB_L),              // M
        .p2(GND),                    // N
        .p3(T_BUS_15_H),             // Q1
        .p4(T_BUS_14_H),             // Q2
        .p5(T_BUS_13_H),             // Q3
        .p6(T_BUS_12_H),             // Q4
        .p7(B_PROC_CLK_L),           // CLK
        .p8(GND),                    // GND
        .p9(IN_H_ENB_L),             // G1_N
        .p10(GND),                   // G2_N
        .p11(T_BUS_12_H),            // D4
        .p12(T_BUS_13_H),            // D3
        .p13(T_BUS_14_H),            // D2
        .p14(T_BUS_15_H),            // D1
        .p15(B_PROC_INIT_H),         // CLR
        .p16(P5V)                    // VCC
    );

// FP7 IR REGISTER AND DECODE, BRANCH LOGIC, PDF page 22

    // E22 ttl_74s00, zone D7, FP8 A5, FP8 B2
    ttl_74s00 E22 (
        .p1(QN8_H),                  // A1, FP8
        .p2(FD_1_H),                 // B1, FP8
        .p3(E22_3),                  // Y1, FP8
        .p4(E13_6),                  // A2, FP8
        .p5(QN40_H),                 // B2, FP8
        .p6(E22_6),                  // Y2, FP8
        .p7(GND),                    // GND
        .p8(FPS_CLK_L),              // Y3
        .p9(B_PROC_CLK_H),           // A3
        .p10(E17_1),                 // B3
        .p11(E22_11),                // Y4, FP8
        .p12(MPC_9_L),               // A4, FP8
        .p13(E35_10),                // B4, FP8
        .p14(P5V)                    // VCC
    );

    // E17 prom_82s123, zone D8
    prom_82s123 E17 (
        .p1(E17_1),                  // O1
        .p2(A_MUX_IN_L),             // O2
        .p3(),                       // O3
        .p4(IN_L_ENB_L),             // O4
        .p5(IN_H_ENB_L),             // O5
        .p6(OUT_ENB_L),              // O6
        .p7(SERVICE_BR_PFAIL_L),     // O7
        .p8(GND),                    // GND
        .p9(),                       // O8
        .p10(MISC_1_H),              // A0
        .p11(MISC_2_H),              // A1
        .p12(MISC_3_H),              // A2
        .p13(MISC_4_H),              // A3
        .p14(GND),                   // A4
        .p15(GND),                   // CE_N
        .p16(P5V)                    // VCC
    );

    // E65 ttl_74s174, zone C6
    ttl_74s174 E65 (
        .p1(B_PROC_INIT_L),          // CLR_N
        .p2(IR_0_H),                 // Q1
        .p3(T_BUS_0_H),              // D1
        .p4(T_BUS_1_H),              // D2
        .p5(IR_1_H),                 // Q2
        .p6(T_BUS_2_H),              // D3
        .p7(IR_2_H),                 // Q3
        .p8(GND),                    // GND
        .p9(E24_10),                 // CLK
        .p10(IR_3_H),                // Q4
        .p11(T_BUS_3_H),             // D4
        .p12(IR_4_H),                // Q5
        .p13(T_BUS_4_H),             // D5
        .p14(T_BUS_5_H),             // D6
        .p15(IR_5_H),                // Q6
        .p16(P5V)                    // VCC
    );

    // E67 ttl_74s174, zone B6
    ttl_74s174 E67 (
        .p1(B_PROC_INIT_L),          // CLR_N
        .p2(IR_6_H),                 // Q1
        .p3(T_BUS_6_H),              // D1
        .p4(T_BUS_7_H),              // D2
        .p5(IR_7_H),                 // Q2
        .p6(T_BUS_8_H),              // D3
        .p7(IR_8_H),                 // Q3
        .p8(GND),                    // GND
        .p9(E24_10),                 // CLK
        .p10(IR_9_H),                // Q4
        .p11(T_BUS_9_H),             // D4
        .p12(IR_10_H),               // Q5
        .p13(T_BUS_10_H),            // D5
        .p14(T_BUS_11_H),            // D6
        .p15(IR_11_H),               // Q6
        .p16(P5V)                    // VCC
    );

    // E24 ttl_74s02, zone B7, FP8 A6, FP9 C3
    ttl_74s02 E24 (
        .p1(E24_1),                  // Y1, FP9
        .p2(T_BUS_OUT_H),            // A1, FP9
        .p3(T_BUS_OUT_H),            // B1, FP9
        .p4(),                       // Y2
        .p5(),                       // A2
        .p6(),                       // B2
        .p7(GND),                    // GND
        .p8(B_PROC_CLK_L),           // A3
        .p9(LOAD_IR_L),              // B3
        .p10(E24_10),                // Y3
        .p11(E10_8),                 // A4, FP8
        .p12(LOAD_IR_L),             // B4, FP8
        .p13(E24_13),                // Y4, FP8
        .p14(P5V)                    // VCC
    );

    // E21 prom_82s123, zone D4
    prom_82s123 E21 (
        .p1(E21_1),                  // O1
        .p2(E21_2),                  // O2
        .p3(E21_3),                  // O3
        .p4(E21_4),                  // O4
        .p5(E21_5),                  // O5
        .p6(E21_6),                  // O6
        .p7(E21_7),                  // O7
        .p8(GND),                    // GND
        .p9(E21_9),                  // O8
        .p10(BUT_0_H),               // A0
        .p11(BUT_1_H),               // A1
        .p12(BUT_2_H),               // A2
        .p13(BUT_3_H),               // A3
        .p14(BUT_4_H),               // A4
        .p15(GND),                   // CE_N
        .p16(P5V)                    // VCC
    );

    // E19 prom_82s123, zone C4
    prom_82s123 E19 (
        .p1(E19_1),                  // O1
        .p2(E19_2),                  // O2
        .p3(E19_3),                  // O3
        .p4(E19_4),                  // O4
        .p5(E19_5),                  // O5
        .p6(E19_6),                  // O6
        .p7(E19_7),                  // O7
        .p8(GND),                    // GND
        .p9(E19_9),                  // O8
        .p10(BUT_0_H),               // A0
        .p11(BUT_1_H),               // A1
        .p12(BUT_2_H),               // A2
        .p13(BUT_3_H),               // A3
        .p14(BUT_4_H),               // A4
        .p15(GND),                   // CE_N
        .p16(P5V)                    // VCC
    );

    // E64 prom_82s136, zone B4
    prom_82s136 E64 (
        .p1(IR_5_H),                 // A6
        .p2(IR_4_H),                 // A5
        .p3(IR_3_H),                 // A4
        .p4(IR_2_H),                 // A3
        .p5(FL_1_H),                 // A0
        .p6(IR_0_H),                 // A1
        .p7(IR_1_H),                 // A2
        .p8(E30_6),                  // CE1_N
        .p9(GND),                    // GND
        .p10(E30_6),                 // CE2_N
        .p11(MPC_3_L),               // O4
        .p12(MPC_2_L),               // O3
        .p13(MPC_1_L),               // O2
        .p14(MPC_0_L),               // O1
        .p15(BUT_2_H),               // A9
        .p16(BUT_1_H),               // A8
        .p17(BUT_0_H),               // A7
        .p18(P5V)                    // VCC
    );

    // E66 prom_82s136, zone A4
    prom_82s136 E66 (
        .p1(IR_11_H),                // A6
        .p2(IR_10_H),                // A5
        .p3(IR_9_H),                 // A4
        .p4(IR_8_H),                 // A3
        .p5(FD_1_H),                 // A0
        .p6(IR_6_H),                 // A1
        .p7(IR_7_H),                 // A2
        .p8(BUT_5_H),                // CE1_N
        .p9(GND),                    // GND
        .p10(BUT_5_H),               // CE2_N
        .p11(MPC_3_L),               // O4
        .p12(MPC_2_L),               // O3
        .p13(MPC_1_L),               // O2
        .p14(MPC_0_L),               // O1
        .p15(BUT_2_H),               // A9
        .p16(BUT_1_H),               // A8
        .p17(BUT_0_H),               // A7
        .p18(P5V)                    // VCC
    );

    // E30 ttl_74s10, zone B4, FP10 B3
    ttl_74s10 E30 (
        .p1(),                       // A1
        .p2(),                       // B1
        .p3(BUT_3_H),                // A2
        .p4(BUT_4_H),                // B2
        .p5(BUT_5_H),                // C2
        .p6(E30_6),                  // Y2
        .p7(GND),                    // GND
        .p8(E30_8),                  // Y3, FP10
        .p9(IR_2_H),                 // A3, FP10
        .p10(IR_1_H),                // B3, FP10
        .p11(IR_0_H),                // C3, FP10
        .p12(),                      // Y1
        .p13(),                      // C1
        .p14(P5V)                    // VCC
    );

    // E32 ttl_74s03, zone D2, FP7 B2
    ttl_74s03 E32 (
        .p1(E19_1),                  // A1
        .p2(T_BUS_0_H),              // B1
        .p3(MPC_1_L),                // Y1
        .p4(EN_BIT_H),               // A2
        .p5(E21_9),                  // B2
        .p6(MPC_8_L),                // Y2
        .p7(GND),                    // GND
        .p8(MPC_7_L),                // Y3
        .p9(E21_7),                  // A3
        .p10(EZ_BIT_H),              // B3
        .p11(MPC_1_L),               // Y4
        .p12(E21_1),                 // A4
        .p13(T_BUS_14_H),            // B4
        .p14(P5V)                    // VCC
    );

    // E23 ttl_74s03, zone D2
    ttl_74s03 E23 (
        .p1(PFAIL_BR_PEND_H),        // A1
        .p2(E21_6),                  // B1
        .p3(MPC_6_L),                // Y1
        .p4(Z_BIT_H),                // A2
        .p5(E21_4),                  // B2
        .p6(MPC_4_L),                // Y2
        .p7(GND),                    // GND
        .p8(MPC_3_L),                // Y3
        .p9(E21_3),                  // A3
        .p10(QN_8_40_H),             // B3
        .p11(MPC_5_L),               // Y4
        .p12(E21_5),                 // A4
        .p13(N_63_BIT_H),            // B4
        .p14(P5V)                    // VCC
    );

    // E11 ttl_74s03, zone C2
    ttl_74s03 E11 (
        .p1(FAST_COUT_H),            // A1
        .p2(E21_2),                  // B1
        .p3(MPC_2_L),                // Y1
        .p4(T_BUS_6_H),              // A2
        .p5(E19_9),                  // B2
        .p6(MPC_8_L),                // Y2
        .p7(GND),                    // GND
        .p8(MPC_7_L),                // Y3
        .p9(E19_7),                  // A3
        .p10(T_BUS_5_H),             // B3
        .p11(MPC_6_L),               // Y4
        .p12(E19_6),                 // A4
        .p13(T_BUS_11_H),            // B4
        .p14(P5V)                    // VCC
    );

    // E20 ttl_74s03, zone C2
    ttl_74s03 E20 (
        .p1(T_BUS_1_H),              // A1
        .p2(E19_2),                  // B1
        .p3(MPC_2_L),                // Y1
        .p4(T_BUS_9_H),              // A2
        .p5(E19_4),                  // B2
        .p6(MPC_4_L),                // Y2
        .p7(GND),                    // GND
        .p8(MPC_3_L),                // Y3
        .p9(E19_3),                  // A3
        .p10(T_BUS_8_H),             // B3
        .p11(MPC_5_L),               // Y4
        .p12(E19_5),                 // A4
        .p13(T_BUS_10_H),            // B4
        .p14(P5V)                    // VCC
    );

// FP8 MICROPROGRAM ROM AND REGISTERS, PDF page 23

    // E34 ttl_74s04, zone D7, FP10 B3
    ttl_74s04 E34 (
        .p1(MPC_6_L),                // A1
        .p2(MPC_6_H),                // Y1
        .p3(MPC_4_L),                // A2
        .p4(MPC_4_H),                // Y2
        .p5(MPC_0_L),                // A3
        .p6(MPC_0_H),                // Y3
        .p7(GND),                    // GND
        .p8(MPC_2_H),                // Y4
        .p9(MPC_2_L),                // A4
        .p10(E34_10),                // Y5, FP10
        .p11(BYTE_DIS_L),            // A5, FP10
        .p12(MPC_8_H),               // Y6
        .p13(MPC_8_L),               // A6
        .p14(P5V)                    // VCC
    );

    // E35 ttl_74s04, zone D7, FP8 A6
    ttl_74s04 E35 (
        .p1(MPC_7_L),                // A1
        .p2(MPC_7_H),                // Y1
        .p3(MPC_5_L),                // A2
        .p4(MPC_5_H),                // Y2
        .p5(MPC_3_L),                // A3
        .p6(MPC_3_H),                // Y3
        .p7(GND),                    // GND
        .p8(Z_BIT_H),                // Y4
        .p9(E10_6),                  // A4
        .p10(E35_10),                // Y5
        .p11(E24_13),                // A5
        .p12(MPC_1_H),               // Y6
        .p13(MPC_1_L),               // A6
        .p14(P5V)                    // VCC
    );

    // E45 prom_82s131, zone D5
    prom_82s131 E45 (
        .p1(MPC_6_H),                // A6
        .p2(MPC_5_H),                // A5
        .p3(MPC_4_H),                // A4
        .p4(MPC_3_H),                // A3
        .p5(MPC_0_H),                // A0
        .p6(MPC_1_H),                // A1
        .p7(MPC_2_H),                // A2
        .p8(GND),                    // GND
        .p9(E45_9),                  // O4
        .p10(E45_10),                // O3
        .p11(E45_11),                // O2
        .p12(E45_12),                // O1
        .p13(GND),                   // CE_N
        .p14(MPC_8_H),               // A8
        .p15(MPC_7_H),               // A7
        .p16(P5V)                    // VCC
    );

    // E44 prom_82s131, zone C5
    prom_82s131 E44 (
        .p1(MPC_6_H),                // A6
        .p2(MPC_5_H),                // A5
        .p3(MPC_4_H),                // A4
        .p4(MPC_3_H),                // A3
        .p5(MPC_0_H),                // A0
        .p6(MPC_1_H),                // A1
        .p7(MPC_2_H),                // A2
        .p8(GND),                    // GND
        .p9(E44_9),                  // O4
        .p10(E44_10),                // O3
        .p11(E44_11),                // O2
        .p12(E44_12),                // O1
        .p13(GND),                   // CE_N
        .p14(MPC_8_H),               // A8
        .p15(MPC_7_H),               // A7
        .p16(P5V)                    // VCC
    );

    // E43 prom_82s131, zone C5
    prom_82s131 E43 (
        .p1(MPC_6_H),                // A6
        .p2(MPC_5_H),                // A5
        .p3(MPC_4_H),                // A4
        .p4(MPC_3_H),                // A3
        .p5(MPC_0_H),                // A0
        .p6(MPC_1_H),                // A1
        .p7(MPC_2_H),                // A2
        .p8(GND),                    // GND
        .p9(E43_9),                  // O4
        .p10(E43_10),                // O3
        .p11(E43_11),                // O2
        .p12(E43_12),                // O1
        .p13(GND),                   // CE_N
        .p14(MPC_8_H),               // A8
        .p15(MPC_7_H),               // A7
        .p16(P5V)                    // VCC
    );

    // E38 prom_82s131, zone B5
    prom_82s131 E38 (
        .p1(MPC_6_H),                // A6
        .p2(MPC_5_H),                // A5
        .p3(MPC_4_H),                // A4
        .p4(MPC_3_H),                // A3
        .p5(MPC_0_H),                // A0
        .p6(MPC_1_H),                // A1
        .p7(MPC_2_H),                // A2
        .p8(GND),                    // GND
        .p9(E38_9),                  // O4
        .p10(E38_10),                // O3
        .p11(E38_11),                // O2
        .p12(E38_12),                // O1
        .p13(GND),                   // CE_N
        .p14(MPC_8_H),               // A8
        .p15(MPC_7_H),               // A7
        .p16(P5V)                    // VCC
    );

    // E57 ttl_74s174, zone D3
    ttl_74s174 E57 (
        .p1(FPX_L),                  // CLR_N
        .p2(F_SRC_0_H),              // Q1
        .p3(E44_10),                 // D1
        .p4(E44_9),                  // D2
        .p5(F_SRC_1_H),              // Q2
        .p6(E45_12),                 // D3
        .p7(F_SRC_2_H),              // Q3
        .p8(GND),                    // GND
        .p9(B_PROC_CLK_L),           // CLK
        .p10(F_ALU_0_H),             // Q4
        .p11(E45_11),                // D4
        .p12(F_ALU_1_H),             // Q5
        .p13(E45_10),                // D5
        .p14(E45_9),                 // D6
        .p15(F_ALU_2_H),             // Q6
        .p16(P5V)                    // VCC
    );

    // E52 ttl_74s174, zone C3
    ttl_74s174 E52 (
        .p1(FPX_L),                  // CLR_N
        .p2(EX_SRC_0_H),             // Q1
        .p3(E43_12),                 // D1
        .p4(E43_11),                 // D2
        .p5(EX_SRC_1_H),             // Q2
        .p6(E43_10),                 // D3
        .p7(EX_SRC_2_H),             // Q3
        .p8(GND),                    // GND
        .p9(B_PROC_CLK_L),           // CLK
        .p10(EX_ALU_0_H),            // Q4
        .p11(E43_9),                 // D4
        .p12(EX_ALU_1_H),            // Q5
        .p13(E44_12),                // D5
        .p14(E44_11),                // D6
        .p15(EX_ALU_2_H),            // Q6
        .p16(P5V)                    // VCC
    );

    // E27 ttl_74s174, zone B3
    ttl_74s174 E27 (
        .p1(B_PROC_INIT_L),          // CLR_N
        .p2(E27_2),                  // Q1
        .p3(E24_13),                 // D1
        .p4(E22_11),                 // D2
        .p5(FPX_L),                  // Q2
        .p6(E38_12),                 // D3
        .p7(A_SEL_H),                // Q3
        .p8(GND),                    // GND
        .p9(B_PROC_CLK_L),           // CLK
        .p10(B_SEL_0_H),             // Q4
        .p11(E38_11),                // D4
        .p12(B_SEL_1_H),             // Q5
        .p13(E38_10),                // D5
        .p14(E38_9),                 // D6
        .p15(CIN_0_H),               // Q6
        .p16(P5V)                    // VCC
    );

    // E10 ttl_74s20, zone A7
    ttl_74s20 E10 (
        .p1(EZ_BIT_H),               // A1
        .p2(FZA_BIT_H),              // B1
        .p3(),                       // NC
        .p4(FZB_BIT_H),              // C1
        .p5(FZC_BIT_H),              // D1
        .p6(E10_6),                  // Y1
        .p7(GND),                    // GND
        .p8(E10_8),                  // Y2
        .p9(T_BUS_15_H),             // A2
        .p10(T_BUS_14_H),            // B2
        .p11(),                      // NC
        .p12(T_BUS_13_H),            // C2
        .p13(T_BUS_12_H),            // D2
        .p14(P5V)                    // VCC
    );

    // E12 ttl_74s37, zone A4, FP8 B1
    ttl_74s37 E12 (
        .p1(K15_1134_CLK_L),         // A1
        .p2(P3V),                    // B1
        .p3(B_PROC_CLK_H),           // Y1
        .p4(B_PROC_CLK_H),           // A2
        .p5(P3V),                    // B2
        .p6(B_PROC_CLK_L),           // Y2
        .p7(GND),                    // GND
        .p8(B_PROC_INIT_H),          // Y3
        .p9(PROC_INIT_L),            // A3
        .p10(P3V),                   // B3
        .p11(QN_8_40_H),             // Y4
        .p12(E22_3),                 // A4
        .p13(E22_6),                 // B4
        .p14(P5V)                    // VCC
    );

    // E33 ttl_74s05, zone A3, FP9 D5, FP9 C3
    ttl_74s05 E33 (
        .p1(E27_2),                  // A1
        .p2(MPC_9_L),                // Y1
        .p3(T_BUS_OUT_H),            // A2, FP9
        .p4(E33_4),                  // Y2, FP9
        .p5(B_PROC_INIT_H),          // A3
        .p6(B_PROC_INIT_L),          // Y3
        .p7(GND),                    // GND
        .p8(MPC_8_L),                // Y4
        .p9(E27_2),                  // A4
        .p10(TRI_STATE_AMUX_L),      // Y5, FP9
        .p11(E4_3),                  // A5, FP9
        .p12(MPC_9_L),               // Y6, FP9
        .p13(E53_2),                 // A6, FP9
        .p14(P5V)                    // VCC
    );

    // R5 330 ohm, zone A3
    // Drawn 330, the parts list gives 1K.
    resistor R5 (
        .p1(P5V),                    // END1
        .p2(B_PROC_INIT_L)           // END2
    );

    // E13 ttl_74s74, zone B2
    ttl_74s74 E13 (
        .p1(B_PROC_INIT_L),          // CLR1_N
        .p2(T_BUS_7_H),              // D1
        .p3(FPS_CLK_L),              // CLK1
        .p4(P3V),                    // PRE1_N
        .p5(FD_1_H),                 // Q1
        .p6(E13_6),                  // Q1_N
        .p7(GND),                    // GND
        .p8(),                       // Q2_N
        .p9(FL_1_H),                 // Q2
        .p10(P3V),                   // PRE2_N
        .p11(FPS_CLK_L),             // CLK2
        .p12(T_BUS_6_H),             // D2
        .p13(B_PROC_INIT_L),         // CLR2_N
        .p14(P5V)                    // VCC
    );

// FP9 A PORT MUX, B PORT MUX, BUT, PDF page 24

    // E42 prom_82s131, zone D7
    prom_82s131 E42 (
        .p1(MPC_6_H),                // A6
        .p2(MPC_5_H),                // A5
        .p3(MPC_4_H),                // A4
        .p4(MPC_3_H),                // A3
        .p5(MPC_0_H),                // A0
        .p6(MPC_1_H),                // A1
        .p7(MPC_2_H),                // A2
        .p8(GND),                    // GND
        .p9(E42_9),                  // O4
        .p10(E42_10),                // O3
        .p11(E42_11),                // O2
        .p12(E42_12),                // O1
        .p13(GND),                   // CE_N
        .p14(MPC_8_H),               // A8
        .p15(MPC_7_H),               // A7
        .p16(P5V)                    // VCC
    );

    // E41 prom_82s131, zone C7
    prom_82s131 E41 (
        .p1(MPC_6_H),                // A6
        .p2(MPC_5_H),                // A5
        .p3(MPC_4_H),                // A4
        .p4(MPC_3_H),                // A3
        .p5(MPC_0_H),                // A0
        .p6(MPC_1_H),                // A1
        .p7(MPC_2_H),                // A2
        .p8(GND),                    // GND
        .p9(E41_9),                  // O4
        .p10(E41_10),                // O3
        .p11(E41_11),                // O2
        .p12(E41_12),                // O1
        .p13(GND),                   // CE_N
        .p14(MPC_8_H),               // A8
        .p15(MPC_7_H),               // A7
        .p16(P5V)                    // VCC
    );

    // E40 prom_82s131, zone B7
    prom_82s131 E40 (
        .p1(MPC_6_H),                // A6
        .p2(MPC_5_H),                // A5
        .p3(MPC_4_H),                // A4
        .p4(MPC_3_H),                // A3
        .p5(MPC_0_H),                // A0
        .p6(MPC_1_H),                // A1
        .p7(MPC_2_H),                // A2
        .p8(GND),                    // GND
        .p9(E40_9),                  // O4
        .p10(E40_10),                // O3
        .p11(E40_11),                // O2
        .p12(E40_12),                // O1
        .p13(GND),                   // CE_N
        .p14(MPC_8_H),               // A8
        .p15(MPC_7_H),               // A7
        .p16(P5V)                    // VCC
    );

    // E53 ttl_74s174, zone D6
    // Pin 1 is labeled FP9 FPX L, the only source is FP8 FPX L from E27.
    ttl_74s174 E53 (
        .p1(FPX_L),                  // CLR_N
        .p2(E53_2),                  // Q1
        .p3(E41_10),                 // D1
        .p4(E41_9),                  // D2
        .p5(E53_5),                  // Q2
        .p6(E42_12),                 // D3
        .p7(E53_7),                  // Q3
        .p8(GND),                    // GND
        .p9(B_PROC_CLK_L),           // CLK
        .p10(E53_10),                // Q4
        .p11(E42_11),                // D4
        .p12(E53_12),                // Q5
        .p13(E42_10),                // D5
        .p14(E42_9),                 // D6
        .p15(T_BUS_OUT_H),           // Q6
        .p16(P5V)                    // VCC
    );

    // E31 ttl_74s174, zone C6
    ttl_74s174 E31 (
        .p1(BUT_DIS_L),              // CLR_N
        .p2(BUT_0_H),                // Q1
        .p3(E40_12),                 // D1
        .p4(E40_11),                 // D2
        .p5(BUT_1_H),                // Q2
        .p6(E40_10),                 // D3
        .p7(BUT_2_H),                // Q3
        .p8(GND),                    // GND
        .p9(B_PROC_CLK_L),           // CLK
        .p10(BUT_3_H),               // Q4
        .p11(E40_9),                 // D4
        .p12(BUT_4_H),               // Q5
        .p13(E41_12),                // D5
        .p14(E41_11),                // D6
        .p15(BUT_5_H),               // Q6
        .p16(P5V)                    // VCC
    );

    // E55 prom_82s123, zone D4
    prom_82s123 E55 (
        .p1(ROTATE_R_L),             // O1
        .p2(FP9_1OR0_SEL),           // O2
        .p3(F_INSERT_0_L),           // O3
        .p4(SHIFT_L_L),              // O4
        .p5(ROTATE_L_L),             // O5
        .p6(SHIFT_R_L),              // O6
        .p7(SHIFT_L_SING_L),         // O7
        .p8(GND),                    // GND
        .p9(SHIFT_L_DOUB_L),         // O8
        .p10(E53_5),                 // A0
        .p11(E53_7),                 // A1
        .p12(E53_10),                // A2
        .p13(E53_12),                // A3
        .p14(FD_1_H),                // A4
        .p15(GND),                   // CE_N
        .p16(P5V)                    // VCC
    );

    // E54 prom_82s123, zone C4
    prom_82s123 E54 (
        .p1(EX_I6_H),                // O1
        .p2(EX_I7_H),                // O2
        .p3(EX_I8_H),                // O3
        .p4(F_I6_H),                 // O4
        .p5(F_I7_H),                 // O5
        .p6(F_I8_H),                 // O6
        .p7(),                       // O7
        .p8(GND),                    // GND
        .p9(),                       // O8
        .p10(E53_5),                 // A0
        .p11(E53_7),                 // A1
        .p12(E53_10),                // A2
        .p13(E53_12),                // A3
        .p14(GND),                   // A4
        .p15(GND),                   // CE_N
        .p16(P5V)                    // VCC
    );

    // R8 330 ohm, zone B6
    resistor R8 (
        .p1(P5V),                    // END1
        .p2(BUT_DIS_L)               // END2
    );

    // E70 ttl_74s157, zone B4
    ttl_74s157 E70 (
        .p1(A_SEL_H),                // S
        .p2(GND),                    // A1
        .p3(P3V),                    // B1
        .p4(A_PORT_3_H),             // Y1
        .p5(GND),                    // A2
        .p6(A_REG_2_H),              // B2
        .p7(A_PORT_2_H),             // Y2
        .p8(GND),                    // GND
        .p9(A_PORT_1_H),             // Y3
        .p10(A_REG_1_H),             // B3
        .p11(IR_7_H),                // A3
        .p12(A_PORT_0_H),            // Y4
        .p13(A_REG_0_H),             // B4
        .p14(IR_6_H),                // A4
        .p15(GND),                   // G_N
        .p16(P5V)                    // VCC
    );

    // E63 ttl_74s182, zone B6
    ttl_74s182 E63 (
        .p1(G_31_H),                 // G1_N
        .p2(P_31_H),                 // P1_N
        .p3(G15_H),                  // G0_N
        .p4(P15_H),                  // P0_N
        .p5(G_63_H),                 // G3_N
        .p6(P_63_H),                 // P3_N
        .p7(E63_7),                  // P_N
        .p8(GND),                    // GND
        .p9(C_IN_47_H),              // CNZ
        .p10(E63_10),                // G_N
        .p11(CIN_31_H),              // CNY
        .p12(CIN_15_H),              // CNX
        .p13(CIN_0_H),               // CN
        .p14(G_47_H),                // G2_N
        .p15(P_47_H),                // P2_N
        .p16(P5V)                    // VCC
    );

    // E50 ttl_74s182, zone B5
    ttl_74s182 E50 (
        .p1(),                       // G1_N
        .p2(),                       // P1_N
        .p3(E63_10),                 // G0_N
        .p4(E63_7),                  // P0_N
        .p5(),                       // G3_N
        .p6(),                       // P3_N
        .p7(),                       // P_N
        .p8(GND),                    // GND
        .p9(),                       // CNZ
        .p10(),                      // G_N
        .p11(),                      // CNY
        .p12(FAST_COUT_H),           // CNX
        .p13(CIN_0_H),               // CN
        .p14(),                      // G2_N
        .p15(),                      // P2_N
        .p16(P5V)                    // VCC
    );

    // R4 470 ohm, zone C3
    resistor R4 (
        .p1(P5V),                    // END1
        .p2(E33_4)                   // END2
    );

    // R11 100 ohm, zone C3
    resistor R11 (
        .p1(E33_4),                  // END1
        .p2(E4_12)                   // END2
    );

    // C36 82 pF, zone C3
    capacitor C36 (
        .p1(E4_12),                  // END1
        .p2(GND)                     // END2
    );

    // E4 ttl_74132, zone C2
    ttl_74132 E4 (
        .p1(E24_1),                  // A1
        .p2(E4_12),                  // B1
        .p3(E4_3),                   // Y1
        .p4(),                       // A2
        .p5(),                       // B2
        .p6(),                       // Y2
        .p7(GND),                    // GND
        .p8(T_BUS_OUT_L),            // Y3
        .p9(T_BUS_OUT_H),            // A3
        .p10(E4_11),                 // B3
        .p11(E4_11),                 // Y4
        .p12(E4_12),                 // A4
        .p13(P3V),                   // B4
        .p14(P5V)                    // VCC
    );

    // E69 ttl_74s153, zone B2
    ttl_74s153 E69 (
        .p1(GND),                    // G1_N
        .p2(B_SEL_1_H),              // B
        .p3(B_REG_1_H),              // C3_1
        .p4(IR_7_H),                 // C2_1
        .p5(IR_1_H),                 // C1_1
        .p6(IR_7_H),                 // C0_1
        .p7(B_PORT_1_H),             // Y1
        .p8(GND),                    // GND
        .p9(B_PORT_0_H),             // Y2
        .p10(P3V),                   // C0_2
        .p11(IR_0_H),                // C1_2
        .p12(IR_6_H),                // C2_2
        .p13(B_REG_0_H),             // C3_2
        .p14(B_SEL_0_H),             // A
        .p15(GND),                   // G2_N
        .p16(P5V)                    // VCC
    );

    // E59 ttl_74s153, zone A2
    ttl_74s153 E59 (
        .p1(GND),                    // G1_N
        .p2(B_SEL_1_H),              // B
        .p3(P3V),                    // C3_1
        .p4(GND),                    // C2_1
        .p5(GND),                    // C1_1
        .p6(GND),                    // C0_1
        .p7(B_PORT_3_H),             // Y1
        .p8(GND),                    // GND
        .p9(B_PORT_2_H),             // Y2
        .p10(GND),                   // C0_2
        .p11(IR_2_H),                // C1_2
        .p12(GND),                   // C2_2
        .p13(B_REG_2_H),             // C3_2
        .p14(B_SEL_0_H),             // A
        .p15(GND),                   // G2_N
        .p16(P5V)                    // VCC
    );

// FP10 CONSTANTS, BYTE ENABLE, CLOCKS, PDF page 25

    // E37 prom_82s131, zone D7
    prom_82s131 E37 (
        .p1(MPC_6_H),                // A6
        .p2(MPC_5_H),                // A5
        .p3(MPC_4_H),                // A4
        .p4(MPC_3_H),                // A3
        .p5(MPC_0_H),                // A0
        .p6(MPC_1_H),                // A1
        .p7(MPC_2_H),                // A2
        .p8(GND),                    // GND
        .p9(E37_9),                  // O4
        .p10(E37_10),                // O3
        .p11(E37_11),                // O2
        .p12(E37_12),                // O1
        .p13(GND),                   // CE_N
        .p14(MPC_8_H),               // A8
        .p15(MPC_7_H),               // A7
        .p16(P5V)                    // VCC
    );

    // E36 prom_82s131, zone C7
    prom_82s131 E36 (
        .p1(MPC_6_H),                // A6
        .p2(MPC_5_H),                // A5
        .p3(MPC_4_H),                // A4
        .p4(MPC_3_H),                // A3
        .p5(MPC_0_H),                // A0
        .p6(MPC_1_H),                // A1
        .p7(MPC_2_H),                // A2
        .p8(GND),                    // GND
        .p9(E36_9),                  // O4
        .p10(E36_10),                // O3
        .p11(E36_11),                // O2
        .p12(E36_12),                // O1
        .p13(GND),                   // CE_N
        .p14(MPC_8_H),               // A8
        .p15(MPC_7_H),               // A7
        .p16(P5V)                    // VCC
    );

    // E48 prom_82s131, zone C7
    prom_82s131 E48 (
        .p1(MPC_6_H),                // A6
        .p2(MPC_5_H),                // A5
        .p3(MPC_4_H),                // A4
        .p4(MPC_3_H),                // A3
        .p5(MPC_0_H),                // A0
        .p6(MPC_1_H),                // A1
        .p7(MPC_2_H),                // A2
        .p8(GND),                    // GND
        .p9(E48_9),                  // O4
        .p10(E48_10),                // O3
        .p11(E48_11),                // O2
        .p12(E48_12),                // O1
        .p13(GND),                   // CE_N
        .p14(MPC_8_H),               // A8
        .p15(MPC_7_H),               // A7
        .p16(P5V)                    // VCC
    );

    // E47 prom_82s131, zone B7
    prom_82s131 E47 (
        .p1(MPC_6_H),                // A6
        .p2(MPC_5_H),                // A5
        .p3(MPC_4_H),                // A4
        .p4(MPC_3_H),                // A3
        .p5(MPC_0_H),                // A0
        .p6(MPC_1_H),                // A1
        .p7(MPC_2_H),                // A2
        .p8(GND),                    // GND
        .p9(E47_9),                  // O4
        .p10(E47_10),                // O3
        .p11(E47_11),                // O2
        .p12(E47_12),                // O1
        .p13(GND),                   // CE_N
        .p14(MPC_8_H),               // A8
        .p15(MPC_7_H),               // A7
        .p16(P5V)                    // VCC
    );

    // E46 prom_82s131, zone A7
    // Outputs are drawn D1 pin 12 at the top down to D4 pin 9.
    prom_82s131 E46 (
        .p1(MPC_6_H),                // A6
        .p2(MPC_5_H),                // A5
        .p3(MPC_4_H),                // A4
        .p4(MPC_3_H),                // A3
        .p5(MPC_0_H),                // A0
        .p6(MPC_1_H),                // A1
        .p7(MPC_2_H),                // A2
        .p8(GND),                    // GND
        .p9(E46_9),                  // O4
        .p10(E46_10),                // O3
        .p11(E46_11),                // O2
        .p12(E46_12),                // O1
        .p13(GND),                   // CE_N
        .p14(MPC_8_H),               // A8
        .p15(MPC_7_H),               // A7
        .p16(P5V)                    // VCC
    );

    // E26 ttl_74s175, zone D3
    ttl_74s175 E26 (
        .p1(FPX_L),                  // CLR_N
        .p2(E26_2),                  // Q1
        .p3(),                       // Q1_N
        .p4(E37_12),                 // D1
        .p5(E37_11),                 // D2
        .p6(),                       // Q2_N
        .p7(E26_7),                  // Q2
        .p8(GND),                    // GND
        .p9(B_PROC_CLK_L),           // CLK
        .p10(E26_10),                // Q3
        .p11(),                      // Q3_N
        .p12(E37_10),                // D3
        .p13(E37_9),                 // D4
        .p14(E26_14),                // Q4_N
        .p15(),                      // Q4
        .p16(P5V)                    // VCC
    );

    // E18 ttl_74s175, zone C3
    ttl_74s175 E18 (
        .p1(FPX_L),                  // CLR_N
        .p2(MISC_3_H),               // Q1
        .p3(),                       // Q1_N
        .p4(E36_12),                 // D1
        .p5(E36_11),                 // D2
        .p6(),                       // Q2_N
        .p7(MISC_4_H),               // Q2
        .p8(GND),                    // GND
        .p9(B_PROC_CLK_L),           // CLK
        .p10(E18_10),                // Q3
        .p11(),                      // Q3_N
        .p12(E36_10),                // D3
        .p13(E36_9),                 // D4
        .p14(),                      // Q4_N
        .p15(E18_15),                // Q4
        .p16(P5V)                    // VCC
    );

    // E28 ttl_74s174, zone C5
    ttl_74s174 E28 (
        .p1(FPX_L),                  // CLR_N
        .p2(B_REG_2_H),              // Q1
        .p3(E47_10),                 // D1
        .p4(E47_9),                  // D2
        .p5(A_REG_0_H),              // Q2
        .p6(E48_12),                 // D3
        .p7(A_REG_1_H),              // Q3
        .p8(GND),                    // GND
        .p9(B_PROC_CLK_L),           // CLK
        .p10(A_REG_2_H),             // Q4
        .p11(E48_11),                // D4
        .p12(MISC_1_H),              // Q5
        .p13(E48_10),                // D5
        .p14(E48_9),                 // D6
        .p15(MISC_2_H),              // Q6
        .p16(P5V)                    // VCC
    );

    // E49 ttl_74s174, zone B5
    ttl_74s174 E49 (
        .p1(FPX_L),                  // CLR_N
        .p2(E49_2),                  // Q1
        .p3(E46_9),                  // D1
        .p4(E46_10),                 // D2
        .p5(E49_5),                  // Q2
        .p6(E46_11),                 // D3
        .p7(E49_7),                  // Q3
        .p8(GND),                    // GND
        .p9(B_PROC_CLK_L),           // CLK
        .p10(E49_10),                // Q4
        .p11(E46_12),                // D4
        .p12(B_REG_0_H),             // Q5
        .p13(E47_12),                // D5
        .p14(E47_11),                // D6
        .p15(B_REG_1_H),             // Q6
        .p16(P5V)                    // VCC
    );

    // E60 ttl_74s00, zone B4, FP10 A4
    // FP10 generates only SECTOR n CLK L. FP1 to FP4 label four of the clock
    // inputs SECTOR n CLK H, taken as the same nets.
    ttl_74s00 E60 (
        .p1(E49_10),                 // A1
        .p2(B_PROC_CLK_H),           // B1
        .p3(SECTOR_0_CLK_L),         // Y1
        .p4(E49_7),                  // A2
        .p5(B_PROC_CLK_H),           // B2
        .p6(SECTOR_1_CLK_L),         // Y2
        .p7(GND),                    // GND
        .p8(SECTOR_2_CLK_L),         // Y3
        .p9(E49_5),                  // A3
        .p10(B_PROC_CLK_H),          // B3
        .p11(SECTOR_3_CLK_L),        // Y4
        .p12(B_PROC_CLK_H),          // A4
        .p13(E49_2),                 // B4
        .p14(P5V)                    // VCC
    );

    // R7 330 ohm, zone B4
    resistor R7 (
        .p1(P5V),                    // END1
        .p2(BYTE_DIS_L)              // END2
    );

    // E76 prom_82s123, zone C2
    prom_82s123 E76 (
        .p1(BYTE_7_ENB_L),           // O1
        .p2(BYTE_0_ENB_L),           // O2
        .p3(BYTE_1_ENB_L),           // O3
        .p4(BYTE_2_ENB_L),           // O4
        .p5(BYTE_3_ENB_L),           // O5
        .p6(BYTE_4_ENB_L),           // O6
        .p7(BYTE_5_ENB_L),           // O7
        .p8(GND),                    // GND
        .p9(BYTE_6_ENB_L),           // O8
        .p10(E18_10),                // A0
        .p11(E18_15),                // A1
        .p12(E26_2),                 // A2
        .p13(E26_7),                 // A3
        .p14(E26_14),                // A4
        .p15(E34_10),                // CE_N
        .p16(P5V)                    // VCC
    );

    // E68 prom_74s471, zone B2
    prom_74s471 E68 (
        .p1(FL_1_H),                 // A0
        .p2(FD_1_H),                 // A1
        .p3(E18_10),                 // A2
        .p4(E18_15),                 // A3
        .p5(E26_2),                  // A4
        .p6(T_BUS_0_H),              // OUT1
        .p7(T_BUS_1_H),              // OUT2
        .p8(T_BUS_2_H),              // OUT3
        .p9(T_BUS_3_H),              // OUT4
        .p10(GND),                   // GND
        .p11(T_BUS_4_H),             // OUT5
        .p12(T_BUS_5_H),             // OUT6
        .p13(T_BUS_6_H),             // OUT7
        .p14(T_BUS_7_H),             // OUT8
        .p15(E26_14),                // EN1_N
        .p16(GND),                   // EN2_N
        .p17(E26_7),                 // A5
        .p18(E26_10),                // A6
        .p19(E30_8),                 // A7
        .p20(P5V)                    // VCC
    );

    // E75 prom_74s471, zone A2
    // Pin 19 is labeled A1, it is A7.
    prom_74s471 E75 (
        .p1(FL_1_H),                 // A0
        .p2(FD_1_H),                 // A1
        .p3(E18_10),                 // A2
        .p4(E18_15),                 // A3
        .p5(E26_2),                  // A4
        .p6(T_BUS_8_H),              // OUT1
        .p7(T_BUS_9_H),              // OUT2
        .p8(T_BUS_10_H),             // OUT3
        .p9(T_BUS_11_H),             // OUT4
        .p10(GND),                   // GND
        .p11(T_BUS_12_H),            // OUT5
        .p12(T_BUS_13_H),            // OUT6
        .p13(T_BUS_14_H),            // OUT7
        .p14(T_BUS_15_H),            // OUT8
        .p15(E26_14),                // EN1_N
        .p16(GND),                   // EN2_N
        .p17(E26_7),                 // A5
        .p18(E26_10),                // A6
        .p19(GND),                   // A7
        .p20(P5V)                    // VCC
    );

endmodule
`default_nettype wire
