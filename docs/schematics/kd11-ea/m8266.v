`default_nettype none
// Verilog transcription of the M8266 KD11-EA control board
// From DEC 1134A FIELD MAINTENANCE PRINT SET, sheets K2-1 to K2-10
// http://bitsavers.informatik.uni-stuttgart.de/pdf/dec/pdp11/1134/MP00190_1134A_KD11-EA.pdf

module kd11_ea_m8266 (
    (* edge_contact = "AA2,BA2,CA2,DA2,EA2,FA2" *) inout wire P5V,
    (* edge_contact = "AC2,AT1,BC2,BT1,CC2,CT1,DC2,DT1,EC2,ET1,FC2,FT1" *) inout wire GND,
    (* edge_contact = "CF2" *) inout wire INT_MSYN_H,
    (* edge_contact = "CE1" *) inout wire CACHE_HIT_L,
    (* edge_contact = "EK2" *) inout wire ASSERT_SSYN_H,
    (* edge_contact = "BU1" *) inout wire BUS_SSYN_L,
    (* edge_contact = "AP2" *) inout wire BUS_BBSY_L,
    (* edge_contact = "BV1" *) inout wire BUS_MSYN_L,
    (* edge_contact = "FB2" *) inout wire ENAB_ADDRS_L,
    (* edge_contact = "CN2" *) inout wire START_TRAN_L,
    (* edge_contact = "CH2" *) inout wire MSYN_1_L,
    (* edge_contact = "CJ1" *) inout wire CLK_MSYN_H,
    (* edge_contact = "DF2" *) inout wire DISABLE_WBIT_L,
    (* edge_contact = "DD2" *) inout wire KT_FAULT_L,
    (* edge_contact = "CK2" *) inout wire TRAN_IN_L,
    (* edge_contact = "CB2" *) inout wire UBUS_C0_1_H,
    (* edge_contact = "CC1" *) inout wire UBUS_C1_1_H,
    (* edge_contact = "CT2" *) inout wire PE_1_H,
    (* edge_contact = "EJ2" *) inout wire KTE_1_H,
    (* edge_contact = "AN2" *) inout wire BUS_PB_L,
    (* edge_contact = "AM1" *) inout wire BUS_PA_L,
    (* edge_contact = "EV2" *) inout wire ALLOW_MSYN_H,
    (* edge_contact = "DE2" *) inout wire TAP_30_H,
    (* edge_contact = "AT2" *) inout wire BUS_BR7_L,
    (* edge_contact = "AU2" *) inout wire BUS_BR6_L,
    (* edge_contact = "BC1" *) inout wire BUS_BR5_L,
    (* edge_contact = "BD2" *) inout wire BUS_BR4_L,
    (* edge_contact = "CP1" *) inout wire BUS_HLT_RQST_L,
    (* edge_contact = "FH2" *) inout wire PSW_07_1_H,
    (* edge_contact = "CM2" *) inout wire PSW_06_1_H,
    (* edge_contact = "CP2" *) inout wire PSW_05_1_H,
    (* edge_contact = "ER1" *) inout wire PROC_CLK_L,
    (* edge_contact = "DK2,DL2" *) inout wire BUS_BG7_H,
    (* edge_contact = "DM2,DN2" *) inout wire BUS_BG6_H,
    (* edge_contact = "DP2,DR2" *) inout wire BUS_BG5_H,
    (* edge_contact = "DS2,DT2" *) inout wire BUS_BG4_H,
    (* edge_contact = "CR1" *) inout wire HLT_GRANT_H,
    (* edge_contact = "FR2" *) inout wire BG_INH_L,
    (* edge_contact = "CR2" *) inout wire SACK_RET_1_H,
    (* edge_contact = "CA1,CB1" *) inout wire BUS_NPG_H,
    (* edge_contact = "CE2" *) inout wire RCD_INIT_L,
    (* edge_contact = "AS2" *) inout wire BUS_NPR_L,
    (* edge_contact = "AR2" *) inout wire BUS_SACK_L,
    (* edge_contact = "AA1" *) inout wire BUS_INIT_L,
    (* edge_contact = "EU1" *) inout wire PROC_INIT_L,
    (* connector_contact = "J2-J" *) inout wire PFAIL_BR_PEND_H,
    (* edge_contact = "BF2" *) inout wire BUS_DC_LO_L,
    (* edge_contact = "BF1" *) inout wire BUS_AC_LO_L,
    (* edge_contact = "FP2" *) inout wire PROC_CLK_H,
    (* edge_contact = "DF1" *) inout wire TBIT_1_H,
    (* edge_contact = "CJ2" *) inout wire K110_8_15_EQ_0_L,
    (* edge_contact = "DJ1" *) inout wire PSW_15_1_L,
    (* edge_contact = "CU1" *) inout wire C4_H,
    (* edge_contact = "CV1" *) inout wire C3_H,
    (* edge_contact = "DB2" *) inout wire C2_H,
    (* edge_contact = "AB1" *) inout wire BUS_INTR_L,
    (* edge_contact = "CV2" *) inout wire INTR_H,
    (* edge_contact = "DK1" *) inout wire INT_VECTOR_L,
    (* connector_contact = "J2-D" *) inout wire FP11_SERVICE_BR_PFAIL_L,
    // Contact label unclear.
    (* edge_contact = "FN2" *) inout wire MODE_01_H,
    (* edge_contact = "DM1" *) inout wire PBA_03_H,
    (* edge_contact = "EE2" *) inout wire PBA_02_H,
    // CH1 is also drawn for ALU S1 H on K2-8, one of the two labels is wrong.
    (* edge_contact = "CH1" *) inout wire PBA_01_H,
    (* edge_contact = "CL2" *) inout wire PBA_00_H,
    (* edge_contact = "CU2" *) inout wire SPA_03_H,
    (* edge_contact = "CS1" *) inout wire SPA_02_H,
    (* edge_contact = "CN1" *) inout wire SPA_01_H,
    (* edge_contact = "CD2" *) inout wire SPA_00_H,
    (* edge_contact = "FV2" *) inout wire SP_WRITE_L,
    (* edge_contact = "EM1" *) inout wire DISABLE_MSYN_PLUS_1_L,
    (* edge_contact = "CD1" *) inout wire SSMUX_15_H,
    (* edge_contact = "EP2" *) inout wire SSMUX_14_H,
    (* edge_contact = "DV2" *) inout wire SSMUX_13_H,
    (* edge_contact = "EB2" *) inout wire SSMUX_12_H,
    (* edge_contact = "DV1" *) inout wire SSMUX_10_H,
    (* edge_contact = "EM2" *) inout wire SSMUX_09_H,
    (* edge_contact = "DJ2" *) inout wire SSMUX_07_H,
    (* edge_contact = "EE1" *) inout wire SSMUX_05_H,
    (* edge_contact = "DH2" *) inout wire SSMUX_04_H,
    (* edge_contact = "DE1" *) inout wire SSMUX_03_H,
    (* edge_contact = "EB1" *) inout wire SSMUX_02_H,
    (* edge_contact = "DH1" *) inout wire SSMUX_01_H,
    (* edge_contact = "EA1" *) inout wire SSMUX_11_H,
    (* edge_contact = "EN2" *) inout wire SSMUX_08_H,
    (* edge_contact = "EK1" *) inout wire SSMUX_06_H,
    (* edge_contact = "FF1" *) inout wire SSMUX_00_H,
    (* edge_contact = "ED2" *) inout wire LOAD_IR_H,
    (* edge_contact = "EH2" *) inout wire BYTE_L,
    (* edge_contact = "EC1" *) inout wire CC_CODE_02_H,
    (* edge_contact = "EU2" *) inout wire CC_CODE_01_H,
    (* edge_contact = "FS1" *) inout wire CC_CODE_00_H,
    (* edge_contact = "DU2" *) inout wire C_BIT_1_H,
    (* edge_contact = "DL1" *) inout wire N_BIT_1_H,
    (* edge_contact = "DN1" *) inout wire BREG_00_1_H,
    (* edge_contact = "FT2" *) inout wire CC_N_H,
    (* edge_contact = "EF2" *) inout wire SERIAL_SHIFT_H,
    (* edge_contact = "FR1" *) inout wire ROT_CBIT_1_H,
    (* connector_contact = "J2-C" *) inout wire FP11_A_ATTACHED_L,
    (* edge_contact = "DC1" *) inout wire DISABLE_LOAD_PSW_H,
    (* connector_contact = "J1-A" *) inout wire MPC_04_L,
    (* connector_contact = "J1-C" *) inout wire MPC_01_L,
    (* connector_contact = "J1-E" *) inout wire MPC_02_L,
    (* connector_contact = "J1-H" *) inout wire MPC_03_L,
    (* edge_contact = "FM1", connector_contact = "J1-K" *) inout wire MAN_CLK_ENAB_L,
    (* connector_contact = "J1-B" *) inout wire MPC_00_L,
    (* connector_contact = "J1-D" *) inout wire MPC_05_L,
    (* connector_contact = "J1-F" *) inout wire MPC_06_L,
    (* connector_contact = "J1-J" *) inout wire MPC_07_L,
    (* edge_contact = "FP1", connector_contact = "J1-L" *) inout wire MAN_CLK_L,
    (* connector_contact = "J2-A" *) inout wire MPC_08_L,
    (* connector_contact = "J2-B" *) inout wire MPC_09_L,
    (* connector_contact = "J2-F" *) inout wire LOAD_IR_L,
    (* edge_contact = "FJ2" *) inout wire LOAD_PSW_L,
    (* edge_contact = "FN1" *) inout wire LOAD_CC_L,
    (* edge_contact = "FS2" *) inout wire ENAB_MAINT_1_H,
    (* edge_contact = "FD1" *) inout wire LOAD_BA_1_H,
    (* edge_contact = "FV1" *) inout wire LONG_CYCLE_1_L,
    (* edge_contact = "DD1" *) inout wire AUX_CONTROL_1_L,
    (* edge_contact = "ER2" *) inout wire ALU_S3_H,
    (* edge_contact = "EF1" *) inout wire ALU_S2_H,
    // CH1 is also drawn for PBA 01 H on K2-4, one of the two labels is wrong.
    (* edge_contact = "CH1" *) inout wire ALU_S1_H,
    (* edge_contact = "FB1" *) inout wire ALU_S0_H,
    (* edge_contact = "FJ1" *) inout wire ALU_MODE_H,
    // Contact label partly hidden by the arrow.
    (* edge_contact = "FE2" *) inout wire ALU_CIN_L,
    (* edge_contact = "FM2" *) inout wire BLEG_01_H,
    (* edge_contact = "FC1" *) inout wire BLEG_00_H,
    (* edge_contact = "FU2" *) inout wire BMODE_01_L,
    (* edge_contact = "FL2" *) inout wire BMODE_00_L,
    (* edge_contact = "FL1" *) inout wire BXMODE_01_L,
    // Contact label partly hidden by the arrow.
    (* edge_contact = "ES1" *) inout wire BXMODE_00_L,
    (* edge_contact = "FU1" *) inout wire SHIFT_MUX_01_L,
    (* edge_contact = "FK2" *) inout wire SHIFT_MUX_00_L,
    // Contact label partly hidden, read ?U1. AU1 or DU1, the other U1
    // contacts carry other signals.
    (* edge_contact = "DU1" *) inout wire ENAB_OVX_L,
    (* edge_contact = "ET2" *) inout wire VBA_00_1_H,
    (* edge_contact = "ES2" *) inout wire SWAP_H,
    (* edge_contact = "EJ1" *) inout wire SEX_H,
    (* edge_contact = "EP1" *) inout wire DISABLE_UPPER_BYTE_WRITE_H,
    (* edge_contact = "FH1" *) inout wire AMUX_S1_1_L,
    (* edge_contact = "FD2" *) inout wire AMUX_S0_1_L,
    (* edge_contact = "CS2" *) inout wire PREVIOUS_MODE_1_L,
    (* edge_contact = "CL1" *) inout wire FORCE_KERNEL_1_H,
    (* edge_contact = "EV1" *) inout wire ALU_COUT_H,
    (* edge_contact = "FA1" *) inout wire BXREG_01_1_H,
    (* edge_contact = "FK1" *) inout wire BXREG_00_1_H,
    (* edge_contact = "FE1" *) inout wire SP_15_1_H,
    (* edge_contact = "EL1" *) inout wire CC_Z_H,
    (* edge_contact = "FF2" *) inout wire Z_BIT_1_H,
    (* edge_contact = "ED1" *) inout wire V_BIT_1_H
);

    wire ABORT_H;
    wire ABORT_RESTART_L;
    wire ALLOW_BG_H;
    wire AUX_CONTROL_1_H;
    wire AUX_SET_UP_H;
    wire BBSY_H;
    wire BE_1_H;
    wire BE_1_L;
    wire BG_1_H;
    wire BG_ENAB_H;
    wire BG4_H;
    wire BG5_H;
    wire BG6_H;
    wire BG7_H;
    wire BUF_C0_1_H;
    wire BUF_C1_1_H;
    wire BUF_DAT_TRAN_1_H;
    wire BUF_DAT_TRAN_L;
    wire BUS_IN_USE_H;
    wire BUT_DEST_L;
    wire BUT_SERVICE_1_H;
    wire BUT_SERVICE_L;
    wire BYTE_H;
    wire CACHE_HIT_H;
    wire CLK_BG_H;
    wire CLK_COUNT_L;
    wire CLR_MPC_L;
    wire COUNT_05_1_H;
    wire DATIP_0_L;
    wire DATIP_1_L;
    wire DBE_H;
    wire DEST_L;
    wire DET_PE_H;
    wire DISAB_MSYN_L;
    wire DM0_H;
    wire E1_10;
    wire E1_14;
    wire E1_2;
    wire E1_4;
    wire E10_4;
    wire E10_9;
    wire E100_11;
    wire E100_12;
    wire E100_13;
    wire E100_14;
    wire E102_1;
    wire E102_2;
    wire E102_3;
    wire E102_4;
    wire E102_5;
    wire E102_6;
    wire E102_7;
    wire E102_9;
    wire E103_11;
    wire E103_12;
    wire E103_13;
    wire E103_14;
    wire E104_11;
    wire E104_12;
    wire E104_13;
    wire E104_14;
    wire E105_11;
    wire E105_12;
    wire E105_13;
    wire E105_14;
    wire E106_11;
    wire E106_12;
    wire E106_14;
    wire E107_11;
    wire E107_12;
    wire E107_13;
    wire E107_14;
    wire E108_11;
    wire E108_12;
    wire E108_13;
    wire E108_14;
    wire E109_11;
    wire E109_12;
    wire E109_13;
    wire E109_14;
    wire E11_3;
    wire E11_6;
    wire E110_11;
    wire E110_12;
    wire E110_13;
    wire E110_14;
    wire E112_10;
    wire E12_8;
    wire E13_3;
    wire E14_12;
    wire E14_13;
    wire E14_14;
    wire E14_15;
    wire E14_4;
    wire E14_6;
    wire E14_7;
    wire E14_RC;
    wire E15_3;
    wire E15_6;
    wire E16_12;
    wire E16_14;
    wire E16_15;
    wire E16_6;
    wire E16_7;
    wire E16_Q_L;
    wire E16_RC;
    wire E17_10;
    wire E17_12;
    wire E17_14;
    wire E17_2;
    wire E17_4;
    wire E17_6;
    wire E18_6;
    wire E18_8;
    wire E19_12;
    wire E19_15;
    wire E19_2;
    wire E19_5;
    wire E2_11;
    wire E2_12;
    wire E2_3;
    wire E2_6;
    wire E2_8;
    wire E20_1;
    wire E20_13;
    wire E20_4;
    wire E20_9;
    wire E21_1;
    wire E21_12;
    wire E21_13;
    wire E21_3;
    wire E21_5;
    wire E21_9;
    wire E22_4;
    wire E23_1;
    wire E23_10;
    wire E23_13;
    wire E24_12;
    wire E24_13;
    wire E24_9;
    wire E25_6;
    wire E25_8;
    wire E26_11;
    wire E27_2;
    wire E28_11;
    wire E28_3;
    wire E28_6;
    wire E28_8;
    wire E3_11;
    wire E3_3;
    wire E3_6;
    wire E30_6;
    wire E30_8;
    wire E31_6;
    wire E35_11;
    wire E35_3;
    wire E35_6;
    wire E37_11;
    wire E37_6;
    wire E37_8;
    wire E39_9;
    wire E39_HOLD;
    wire E4_6;
    wire E40_8;
    wire E41_6;
    wire E41_8;
    wire E42_11;
    wire E42_5;
    wire E42_6;
    wire E43_13;
    wire E44_11;
    wire E44_3;
    wire E44_6;
    wire E45_12;
    wire E45_9;
    wire E47_11;
    wire E47_3;
    wire E47_6;
    wire E47_8;
    wire E5_5;
    wire E5_6;
    wire E50_11;
    wire E50_3;
    wire E50_6;
    wire E52_11;
    wire E52_12;
    wire E52_13;
    wire E52_14;
    wire E57_6;
    wire E57_8;
    wire E58_6;
    wire E58_8;
    wire E59_12;
    wire E6_11;
    wire E6_3;
    wire E6_8;
    wire E62_9;
    wire E65_6;
    wire E7_13;
    wire E7_14;
    wire E7_15;
    wire E7_6;
    wire E7_7;
    wire E7_RC1;
    wire E7_RC2;
    wire E71_10;
    wire E71_12;
    wire E71_15;
    wire E71_2;
    wire E71_5;
    wire E73_13;
    wire E73_4;
    wire E78_12;
    wire E78_13;
    wire E8_13;
    wire E8_4;
    wire E8_8;
    wire E82_1;
    wire E82_2;
    wire E82_4;
    wire E82_5;
    wire E84_11;
    wire E84_3;
    wire E84_8;
    wire E85_12;
    wire E85_6;
    wire E85_8;
    wire E86_11;
    wire E86_3;
    wire E86_6;
    wire E86_8;
    wire E87_1;
    wire E87_2;
    wire E88_10;
    wire E88_12;
    wire E88_15;
    wire E88_2;
    wire E88_5;
    wire E88_7;
    wire E9_7;
    wire E90_10;
    wire E90_12;
    wire E90_15;
    wire E90_2;
    wire E90_5;
    wire E90_7;
    wire E92_10;
    wire E92_12;
    wire E92_15;
    wire E92_2;
    wire E92_5;
    wire E92_7;
    wire E93_10;
    wire E93_12;
    wire E93_5;
    wire E93_7;
    wire E95_1;
    wire E95_10;
    wire E95_13;
    wire E95_4;
    wire E97_11;
    wire E97_12;
    wire E97_13;
    wire E97_14;
    wire E98_11;
    wire E98_12;
    wire ENAB_ABORT_H;
    wire ENAB_DBE_L;
    wire ENAB_EOT_H;
    wire ENAB_GRANTS_H;
    wire ENAB_STOV_L;
    wire ENAB_TBIT_H;
    wire EOT_L;
    wire FORCE_RS_PLUS_1_1_L;
    wire FUNC_CODE_00_H;
    wire FUNC_CODE_01_H;
    wire FUNC_CODE_02_H;
    wire FUNC_CODE_03_H;
    wire GET_BUS_H;
    wire HALT_PEND_L;
    wire HLT_RQST_1_H;
    wire HLT_RQST_H;
    wire IR_00_1_H;
    wire IR_00_1_L;
    wire IR_01_1_H;
    wire IR_02_1_H;
    wire IR_03_1_H;
    wire IR_04_1_H;
    wire IR_05_1_H;
    wire IR_06_1_H;
    wire IR_06_1_L;
    wire IR_07_1_H;
    wire IR_08_1_H;
    wire IR_09_1_H;
    wire IR_10_1_H;
    wire IR_11_1_H;
    wire IR_11_1_L;
    wire IR_12_1_H;
    wire IR_13_1_H;
    wire IR_14_1_H;
    wire IR_15_1_H;
    wire IR_CODE_00;
    wire IR_CODE_00_L;
    wire IR_CODE_01_L;
    wire IR_CODE_02_L;
    wire IR_DECODE_1_H;
    wire IR_DECODE_1_L;
    wire IR12_14_EQ_0_H;
    wire IR12_14_EQ_0_L;
    wire LOAD_COUNT_L;
    wire MOV_L;
    wire MPC_00_H;
    wire MPC_01_H;
    wire MPC_02_H;
    wire MPC_03_H;
    wire MPC_04_H;
    wire MPC_05_H;
    wire MPC_06_H;
    wire MPC_07_H;
    wire MPC_08_H;
    wire MPC_09_H;
    wire MSYN_1_H;
    wire MSYN_H;
    wire NO_SACK_L;
    wire NOSACK_H;
    wire NPG_1_H;
    wire NPG_1_L;
    wire NPG_ENAB_H;
    wire NPR_H;
    wire P3V;
    wire P3VA;
    wire PARITY_ERROR_H;
    wire PFAIL_1_H;
    wire PFAIL_H;
    wire PFAIL_SERV_H;
    wire PROC_INIT_H;
    wire PWR_CLR_L;
    wire R6_PLUS_7_L;
    wire RCD_INIT_H;
    wire RESET_1_H;
    wire RESET_INST_H;
    wire RESET_INST_L;
    wire ROM_SPA_00_1_H;
    wire ROM_SPA_00_1_L;
    wire ROM_SPA_01_1_H;
    wire ROM_SPA_02_1_H;
    wire ROM_SPA_03_1_L;
    wire SACK_RET_1_L;
    wire SERV_IR_1_L;
    wire SERV_IR_H;
    wire SET_BE_L;
    wire SM0_H;
    wire SPA_DST_SEL_0_1_H;
    wire SPA_DST_SEL_1_1_H;
    wire SPA_SRC_SEL_0_1_H;
    wire SPA_SRC_SEL_1_1_H;
    wire SPAM_S0;
    wire SPAM_S1;
    wire SRC_H;
    wire SS_00_H;
    wire SS_01_H;
    wire SSYN_H;
    wire START_PDWN_1_L;
    wire START_RESET_H;
    wire START_TRAN_H;
    wire STOV_SERV_H;
    wire TO_1_H;
    wire TO_1_L;
    wire UBUS_C0_1_L;
    wire USER_MODE_H;

// K2-1 DAT TRAN, PDF page 40

    // R55 470 ohm, zone A7
    resistor R55 (
        .p1(P5V),                    // END1
        .p2(P3V)                     // END2
    );

    // R56 470 ohm, zone A7
    resistor R56 (
        .p1(P5V),                    // END1
        .p2(P3VA)                    // END2
    );

    // E4 8815, zone D7, K2-2 C2
    signetics_8815 E4 (
        .p1(BBSY_H),                 // A1
        .p2(NPR_H),                  // B1
        .p3(),                       // NC
        .p4(NPG_1_H),                // C1
        .p5(NO_SACK_L),              // D1
        .p6(E4_6),                   // Y1
        .p7(GND),                    // GND
        .p8(BG_INH_L),               // Y2, K2-2
        .p9(RESET_1_H),              // A2, K2-2
        .p10(ALLOW_BG_H),            // B2, K2-2
        .p11(),                      // NC
        .p12(SACK_RET_1_H),          // C2, K2-2
        .p13(E22_4),                 // D2, K2-2
        .p14(P5V)                    // VCC
    );

    // E22 7402, zone D7, K2-1 C4, K2-2 C2, K2-2 C4
    ttl_7402 E22 (
        .p1(HLT_GRANT_H),            // Y1, K2-2
        .p2(HALT_PEND_L),            // A1, K2-2
        .p3(E12_8),                  // B1, K2-2
        .p4(E22_4),                  // Y2, K2-2
        .p5(BUT_SERVICE_L),          // A2, K2-2
        .p6(CLK_BG_H),               // B2, K2-2
        .p7(GND),                    // GND
        .p8(DATIP_0_L),              // A3
        .p9(E4_6),                   // B3
        .p10(BUS_IN_USE_H),          // Y3
        .p11(SSYN_H),                // A4
        .p12(E21_12),                // B4
        .p13(CLK_MSYN_H),            // Y4
        .p14(P5V)                    // VCC
    );

    // E31 7474, zone D6, K2-1 C3
    // Pin 9 is named MSYN (1) L on the 1 side and MSYN (0) H on the 0 side.
    ttl_7474 E31 (
        .p1(GET_BUS_H),              // CLR1_N
        .p2(GND),                    // D1
        .p3(GND),                    // CLK1
        .p4(BUS_IN_USE_H),           // PRE1_N
        .p5(),                       // Q1
        .p6(E31_6),                  // Q1_N
        .p7(GND),                    // GND
        .p8(MSYN_1_H),               // Q2_N
        .p9(MSYN_1_L),               // Q2
        .p10(START_TRAN_H),          // PRE2_N
        .p11(CLK_MSYN_H),            // CLK2
        .p12(E44_6),                 // D2
        .p13(P3VA),                  // CLR2_N
        .p14(P5V)                    // VCC
    );

    // R1 150 ohm, zone D6
    resistor R1 (
        .p1(E31_6),                  // END1
        .p2(E42_5)                   // END2
    );

    // C68 100 pF, zone D6
    capacitor C68 (
        .p1(E42_5),                  // END1
        .p2(GND)                     // END2
    );

    // E42 74132, zone D6, K2-1 C4, K2-1 B4
    // Print gap in the E42.8 wire before E33.4.
    ttl_74132 E42 (
        .p1(KT_FAULT_L),             // A1
        .p2(DISABLE_MSYN_PLUS_1_L),  // B1
        .p3(ENAB_ABORT_H),           // Y1
        .p4(DATIP_1_L),              // A2
        .p5(E42_5),                  // B2
        .p6(E42_6),                  // Y2
        .p7(GND),                    // GND
        .p8(SET_BE_L),               // Y3
        .p9(ABORT_H),                // A3
        .p10(E42_11),                // B3
        .p11(E42_11),                // Y4
        .p12(TO_1_L),                // A4
        .p13(DISABLE_MSYN_PLUS_1_L), // B4
        .p14(P5V)                    // VCC
    );

    // E6 74S00, zone D5, K2-1 B7
    ttl_74s00 E6 (
        .p1(MSYN_H),                 // A1
        .p2(SACK_RET_1_H),           // B1
        .p3(E6_3),                   // Y1
        .p4(E6_3),                   // A2
        .p5(MSYN_1_L),               // B2
        .p6(INT_MSYN_H),             // Y2
        .p7(GND),                    // GND
        .p8(E6_8),                   // Y3
        .p9(E6_11),                  // A3
        .p10(E6_11),                 // B3
        .p11(E6_11),                 // Y4
        .p12(ABORT_RESTART_L),       // A4
        .p13(TAP_30_H),              // B4
        .p14(P5V)                    // VCC
    );

    // R54 390 ohm, zone D5
    resistor R54 (
        .p1(P5V),                    // END1
        .p2(CACHE_HIT_L)             // END2
    );

    // E73 74S02, zone D5, K2-5 C7, K2-5 A7, K2-6 C4
    // Pin 1 also marked J2-5, which is not in the J2 table of K2-7.
    ttl_74s02 E73 (
        .p1(LOAD_IR_H),              // Y1, K2-5
        .p2(LOAD_IR_L),              // A1, K2-5
        .p3(PROC_CLK_L),             // B1, K2-5
        .p4(E73_4),                  // Y2, K2-6
        .p5(IR_08_1_H),              // A2, K2-6
        .p6(IR_15_1_H),              // B2, K2-6
        .p7(GND),                    // GND
        .p8(CACHE_HIT_L),            // A3
        .p9(ENAB_ABORT_H),           // B3
        .p10(CACHE_HIT_H),           // Y3
        .p11(DBE_H),                 // A4, K2-5
        .p12(SERV_IR_H),             // B4, K2-5
        .p13(E73_13),                // Y4, K2-5
        .p14(P5V)                    // VCC
    );

    // E44 74S32, zone D5, K2-1 A4, K2-2 D4, K2-2 A7
    ttl_74s32 E44 (
        .p1(KT_FAULT_L),             // A1
        .p2(CLR_MPC_L),              // B1
        .p3(E44_3),                  // Y1
        .p4(CACHE_HIT_H),            // A2
        .p5(ENAB_ABORT_H),           // B2
        .p6(E44_6),                  // Y2
        .p7(GND),                    // GND
        .p8(PFAIL_BR_PEND_H),        // Y3, K2-2
        .p9(E18_8),                  // A3, K2-2
        .p10(PFAIL_1_H),             // B3, K2-2
        .p11(E44_11),                // Y4, K2-2
        .p12(HLT_RQST_H),            // A4, K2-2
        .p13(PWR_CLR_L),             // B4, K2-2
        .p14(P5V)                    // VCC
    );

    // E23 7402, zone D4, K2-1 B4, K2-3 C6, K2-3 C3
    ttl_7402 E23 (
        .p1(E23_1),                  // Y1
        .p2(PARITY_ERROR_H),         // A1
        .p3(TO_1_H),                 // B1
        .p4(PFAIL_H),                // Y2, K2-3
        .p5(START_PDWN_1_L),         // A2, K2-3
        .p6(E45_12),                 // B2, K2-3
        .p7(GND),                    // GND
        .p8(UBUS_C0_1_L),            // A3
        .p9(UBUS_C1_1_H),            // B3
        .p10(E23_10),                // Y3
        .p11(E17_14),                // A4, K2-3
        .p12(E14_13),                // B4, K2-3
        .p13(E23_13),                // Y4, K2-3
        .p14(P5V)                    // VCC
    );

    // E32 7474, zone D3, K2-1 C2
    ttl_7474 E32 (
        .p1(CLR_MPC_L),              // CLR1_N
        .p2(E23_10),                 // D1
        .p3(MSYN_1_H),               // CLK1
        .p4(P3VA),                   // PRE1_N
        .p5(DATIP_0_L),              // Q1
        .p6(DATIP_1_L),              // Q1_N
        .p7(GND),                    // GND
        .p8(TO_1_H),                 // Q2_N
        .p9(TO_1_L),                 // Q2
        .p10(MSYN_1_H),              // PRE2_N
        .p11(E16_Q_L),               // CLK2
        .p12(SSYN_H),                // D2
        .p13(P3VA),                  // CLR2_N
        .p14(P5V)                    // VCC
    );

    // R4 47 ohm, zone D3
    resistor R4 (
        .p1(ASSERT_SSYN_H),          // END1
        .p2(E27_2)                   // END2
    );

    // C58 100 pF, zone D3
    capacitor C58 (
        .p1(E27_2),                  // END1
        .p2(GND)                     // END2
    );

    // E27 8641, zone D2, K2-2 A6
    ds8641 E27 (
        .p1(BUS_SSYN_L),             // BUS3
        .p2(E27_2),                  // IN3
        .p3(SSYN_H),                 // OUT3
        .p4(BUS_INIT_L),             // BUS4, K2-2
        .p5(E25_8),                  // IN4, K2-2
        .p6(RCD_INIT_H),             // OUT4, K2-2
        .p7(GND),                    // DISABLE_B
        .p8(GND),                    // GND
        .p9(GND),                    // DISABLE_A
        .p10(BBSY_H),                // OUT2
        .p11(E42_6),                 // IN2
        .p12(BUS_BBSY_L),            // BUS2
        .p13(MSYN_H),                // OUT1
        .p14(MSYN_1_H),              // IN1
        .p15(BUS_MSYN_L),            // BUS1
        .p16(P5V)                    // VCC
    );

    // E18 7400, zone D2, K2-2 D4, K2-2 C5
    // The E42.6 wire turns beside the DATIP (1) L label of E32.6 and does not
    // reach it.
    ttl_7400 E18 (
        .p1(HLT_RQST_1_H),           // A1, K2-2
        .p2(HLT_RQST_1_H),           // B1, K2-2
        .p3(HALT_PEND_L),            // Y1, K2-2
        .p4(HALT_PEND_L),            // A2, K2-2
        .p5(E30_8),                  // B2, K2-2
        .p6(E18_6),                  // Y2, K2-2
        .p7(GND),                    // GND
        .p8(E18_8),                  // Y3, K2-2
        .p9(E30_8),                  // A3, K2-2
        .p10(E30_8),                 // B3, K2-2
        .p11(ENAB_ADDRS_L),          // Y4
        .p12(E42_6),                 // A4
        .p13(E42_6),                 // B4
        .p14(P5V)                    // VCC
    );

    // E40 74S10, zone C6, K2-1 C4, K2-1 B5
    ttl_74s10 E40 (
        .p1(EOT_L),                  // A1
        .p2(E25_6),                  // B1
        .p3(E42_6),                  // A2
        .p4(START_TRAN_H),           // B2
        .p5(E10_4),                  // C2
        .p6(START_TRAN_L),           // Y2
        .p7(GND),                    // GND
        .p8(E40_8),                  // Y3
        .p9(ENAB_ABORT_H),           // A3
        .p10(START_TRAN_H),          // B3
        .p11(CLK_MSYN_H),            // C3
        .p12(TRAN_IN_L),             // Y1
        .p13(E6_8),                  // C1
        .p14(P5V)                    // VCC
    );

    // E41 74S03, zone C5, K2-1 C3, K2-3 D3, K2-3 A3
    ttl_74s03 E41 (
        .p1(E3_6),                   // A1, K2-3
        .p2(BUT_SERVICE_1_H),        // B1, K2-3
        .p3(MPC_00_L),               // Y1, K2-3
        .p4(START_TRAN_L),           // A2
        .p5(START_TRAN_L),           // B2
        .p6(E41_6),                  // Y2
        .p7(GND),                    // GND
        .p8(E41_8),                  // Y3
        .p9(E40_8),                  // A3
        .p10(E23_1),                 // B3
        .p11(MPC_00_L),              // Y4, K2-3
        .p12(E24_13),                // A4, K2-3
        .p13(E24_13),                // B4, K2-3
        .p14(P5V)                    // VCC
    );

    // R2 1 kohm, zone C5
    resistor R2 (
        .p1(P5V),                    // END1
        .p2(E41_6)                   // END2
    );

    // R3 47 ohm, zone C5
    resistor R3 (
        .p1(E41_6),                  // END1
        .p2(E21_13)                  // END2
    );

    // C56 470 pF, zone C5
    // Drawn as C66, like the capacitor of K2-2. C56 is the only 470 pF part
    // of the parts list not drawn elsewhere.
    capacitor C56 (
        .p1(E21_13),                 // END1
        .p2(GND)                     // END2
    );

    // E21 7414, zone C5, K2-1 C2, K2-1 B2, K2-2 C5
    ttl_7414 E21 (
        .p1(E21_1),                  // A1
        .p2(ABORT_RESTART_L),        // Y1
        .p3(E21_3),                  // A2
        .p4(DISABLE_WBIT_L),         // Y2
        .p5(E21_5),                  // A3, K2-2
        .p6(BG_ENAB_H),              // Y3, K2-2
        .p7(GND),                    // GND
        .p8(NPG_ENAB_H),             // Y4, K2-2
        .p9(E21_9),                  // A4, K2-2
        .p10(BUT_SERVICE_L),         // Y5, K2-2
        .p11(BUT_SERVICE_1_H),       // A5, K2-2
        .p12(E21_12),                // Y6
        .p13(E21_13),                // A6
        .p14(P5V)                    // VCC
    );

    // R8 30 kohm, zone C3
    resistor R8 (
        .p1(P5V),                    // END1
        .p2(E16_15)                  // END2
    );

    // C69 2200 pF, zone C3
    capacitor C69 (
        .p1(E16_14),                 // END1
        .p2(E16_15)                  // END2
    );

    // E16 74123, zone C3, K2-2 B7
    // K2-1: 22 us bus timeout. The wire to E32.11 leaves the package below
    // the 13 and 4 stubs, pin 4 assumed. K2-2: 100 ms reset.
    ttl_74123 E16 (
        .p1(GND),                    // A1_N
        .p2(MSYN_1_H),               // B1
        .p3(START_TRAN_H),           // CLR1_N
        .p4(E16_Q_L),                // Q1_N
        .p5(RESET_1_H),              // Q2, K2-2
        .p6(E16_6),                  // CEXT2, K2-2
        .p7(E16_7),                  // REXT_CEXT2, K2-2
        .p8(GND),                    // GND
        .p9(GND),                    // A2_N, K2-2
        .p10(RESET_INST_H),          // B2, K2-2
        .p11(START_PDWN_1_L),        // CLR2_N, K2-2
        .p12(E16_12),                // Q2_N, K2-2
        .p13(),                      // Q1
        .p14(E16_14),                // CEXT1
        .p15(E16_15),                // REXT_CEXT1
        .p16(P5V)                    // VCC
    );

    // R9 470 ohm, zone C3
    resistor R9 (
        .p1(P5V),                    // END1
        .p2(E41_8)                   // END2
    );

    // R10 150 ohm, zone C3
    resistor R10 (
        .p1(E41_8),                  // END1
        .p2(E21_1)                   // END2
    );

    // C65 470 pF, zone C3
    capacitor C65 (
        .p1(E21_1),                  // END1
        .p2(GND)                     // END2
    );

    // R11 47 ohm, zone B3
    resistor R11 (
        .p1(E41_8),                  // END1
        .p2(E21_3)                   // END2
    );

    // C67 100 pF, zone B3
    capacitor C67 (
        .p1(E21_3),                  // END1
        .p2(GND)                     // END2
    );

    // E50 7408, zone B2, K2-1 B6, K2-2 C6, K2-3 B7
    // Pin 2 is labeled NPG (0) H, which is E12.5.
    ttl_7408 E50 (
        .p1(BUT_SERVICE_1_H),        // A1, K2-2
        .p2(NPG_1_L),                // B1, K2-2
        .p3(E50_3),                  // Y1, K2-2
        .p4(BYTE_H),                 // A2
        .p5(BUF_C0_1_H),             // B2
        .p6(E50_6),                  // Y2
        .p7(GND),                    // GND
        .p8(ABORT_H),                // Y3
        .p9(E41_8),                  // A3
        .p10(DISABLE_WBIT_L),        // B3
        .p11(E50_11),                // Y4, K2-3
        .p12(TBIT_1_H),              // A4, K2-3
        .p13(ENAB_TBIT_H),           // B4, K2-3
        .p14(P5V)                    // VCC
    );

    // E33 74S74, zone B3, K2-5 D7
    ttl_74s74 E33 (
        .p1(PROC_INIT_L),            // CLR1_N
        .p2(GND),                    // D1
        .p3(BUT_SERVICE_L),          // CLK1
        .p4(SET_BE_L),               // PRE1_N
        .p5(BE_1_H),                 // Q1
        .p6(BE_1_L),                 // Q1_N
        .p7(GND),                    // GND
        .p8(),                       // Q2_N, K2-5
        .p9(IR_15_1_H),              // Q2, K2-5
        .p10(SERV_IR_1_L),           // PRE2_N, K2-5
        .p11(LOAD_IR_H),             // CLK2, K2-5
        .p12(SSMUX_15_H),            // D2, K2-5
        .p13(E43_13),                // CLR2_N, K2-5
        .p14(P5V)                    // VCC
    );

    // E37 74S00, zone B4, K2-1 C6, K2-3 B7, K2-4 B4
    ttl_74s00 E37 (
        .p1(SPA_02_H),               // A1, K2-3
        .p2(SPA_01_H),               // B1, K2-3
        .p3(R6_PLUS_7_L),            // Y1, K2-3
        .p4(SSYN_H),                 // A2
        .p5(MSYN_1_L),               // B2
        .p6(E37_6),                  // Y2
        .p7(GND),                    // GND
        .p8(E37_8),                  // Y3
        .p9(ABORT_H),                // A3
        .p10(PARITY_ERROR_H),        // B3
        .p11(E37_11),                // Y4, K2-4
        .p12(SPA_DST_SEL_1_1_H),     // A4, K2-4
        .p13(SPA_DST_SEL_0_1_H),     // B4, K2-4
        .p14(P5V)                    // VCC
    );

    // E34 7474, zone B3, K2-1 A3
    ttl_7474 E34 (
        .p1(PROC_INIT_L),            // CLR1_N
        .p2(GND),                    // D1
        .p3(BUT_SERVICE_L),          // CLK1
        .p4(E37_8),                  // PRE1_N
        .p5(PE_1_H),                 // Q1
        .p6(),                       // Q1_N
        .p7(GND),                    // GND
        .p8(),                       // Q2_N
        .p9(KTE_1_H),                // Q2
        .p10(E44_3),                 // PRE2_N
        .p11(BUT_SERVICE_L),         // CLK2
        .p12(GND),                   // D2
        .p13(PROC_INIT_L),           // CLR2_N
        .p14(P5V)                    // VCC
    );

    // E1 8837, zone C8, K2-1 C6, K2-2 C7, K2-2 C5, K2-3 A5
    // Pin 4 is marked RS on the drawing.
    dm8837 E1 (
        .p1(BUS_SACK_L),             // IN4, K2-2
        .p2(E1_2),                   // OUT4, K2-2
        .p3(BUS_PA_L),               // IN5
        .p4(E1_4),                   // OUT5
        .p5(BUS_INTR_L),             // IN6, K2-3
        .p6(INTR_H),                 // OUT6, K2-3
        .p7(GND),                    // DISABLE_B, K2-3
        .p8(GND),                    // GND
        .p9(GND),                    // DISABLE_A, K2-2
        .p10(E1_10),                 // OUT3
        .p11(DET_PE_H),              // IN3
        .p12(HLT_RQST_H),            // OUT2, K2-2
        .p13(BUS_HLT_RQST_L),        // IN2, K2-2
        .p14(E1_14),                 // OUT1
        .p15(E2_3),                  // IN1
        .p16(P5V)                    // VCC
    );

    // W1 jumper, zone C7
    // Installed.
    jumper W1 (
        .p1(E9_7),                   // END1
        .p2(E1_4)                    // END2
    );

    // R58 1 kohm, zone C7
    resistor R58 (
        .p1(P5V),                    // END1
        .p2(E9_7)                    // END2
    );

    // E9 8640, zone C7, K2-2 C7, K2-2 C4
    // Pin 4 drawn as 9.
    ds8640 E9 (
        .p1(GND),                    // GND
        .p2(DET_PE_H),               // OUT2
        .p3(NPR_H),                  // OUT1, K2-2
        .p4(RCD_INIT_H),             // IN1A, K2-2
        .p5(BUS_NPR_L),              // IN1B, K2-2
        .p6(BUS_PB_L),               // IN2A
        .p7(E9_7),                   // IN2B
        .p8(P5V),                    // VCC
        .p9(E2_11),                  // IN3B, K2-2
        .p10(E1_2),                  // IN3A, K2-2
        .p11(RCD_INIT_H),            // IN4B, K2-2
        .p12(PROC_INIT_H),           // IN4A, K2-2
        .p13(RCD_INIT_L),            // OUT4, K2-2
        .p14(NOSACK_H)               // OUT3, K2-2
    );

    // E2 74132, zone C6, K2-1 B6, K2-2 C4
    ttl_74132 E2 (
        .p1(ENAB_EOT_H),             // A1
        .p2(DET_PE_H),               // B1
        .p3(E2_3),                   // Y1
        .p4(ENAB_EOT_H),             // A2
        .p5(E1_10),                  // B2
        .p6(E2_6),                   // Y2
        .p7(GND),                    // GND
        .p8(E2_8),                   // Y3, K2-2
        .p9(E1_2),                   // A3, K2-2
        .p10(E2_11),                 // B3, K2-2
        .p11(E2_11),                 // Y4, K2-2
        .p12(E2_12),                 // A4, K2-2
        .p13(E2_12),                 // B4, K2-2
        .p14(P5V)                    // VCC
    );

    // E35 74S00, zone B7, K2-10 B4
    // Drawn E55 or E35 on K2-10. E55 is a 74S175.
    ttl_74s00 E35 (
        .p1(E35_11),                 // A1, K2-10
        .p2(NPG_1_L),                // B1, K2-10
        .p3(E35_3),                  // Y1, K2-10
        .p4(MSYN_1_H),               // A2
        .p5(SSYN_H),                 // B2
        .p6(E35_6),                  // Y2
        .p7(GND),                    // GND
        .p8(),                       // Y3
        .p9(),                       // A3
        .p10(),                      // B3
        .p11(E35_11),                // Y4, K2-10
        .p12(HALT_PEND_L),           // A4, K2-10
        .p13(BG_1_H),                // B4, K2-10
        .p14(P5V)                    // VCC
    );

    // E8 8881, zone B7, K2-2 B2, K2-2 C5, K2-10 B2
    signetics_8881 E8 (
        .p1(BUS_NPG_H),              // Y1, K2-2
        .p2(NPG_1_L),                // A1, K2-2
        .p3(NPG_1_L),                // B1, K2-2
        .p4(E8_4),                   // Y2, K2-2
        .p5(E1_2),                   // A2, K2-2
        .p6(E1_2),                   // B2, K2-2
        .p7(GND),                    // GND
        .p8(E8_8),                   // A3, K2-10
        .p9(E14_12),                 // B3, K2-10
        .p10(BUS_SACK_L),            // Y3, K2-10
        .p11(E35_6),                 // B4
        .p12(E35_6),                 // A4
        .p13(E8_13),                 // Y4
        .p14(P5V)                    // VCC
    );

    // R6 750 ohm, zone B7
    resistor R6 (
        .p1(P5V),                    // END1
        .p2(E8_13)                   // END2
    );

    // R7 47 ohm, zone B6
    resistor R7 (
        .p1(E8_13),                  // END1
        .p2(ENAB_EOT_H)              // END2
    );

    // C70 220 pF, zone B6
    capacitor C70 (
        .p1(ENAB_EOT_H),             // END1
        .p2(GND)                     // END2
    );

    // E10 74S157, zone C5
    ttl_74s157 E10 (
        .p1(BUF_C1_1_H),             // S
        .p2(ALLOW_MSYN_H),           // A1
        .p3(E37_6),                  // B1
        .p4(E10_4),                  // Y1
        .p5(E2_6),                   // A2
        .p6(E35_6),                  // B2
        .p7(EOT_L),                  // Y2
        .p8(GND),                    // GND
        .p9(E10_9),                  // Y3
        .p10(E50_6),                 // B3
        .p11(BUF_C0_1_H),            // A3
        .p12(PARITY_ERROR_H),        // Y4
        .p13(GND),                   // B4
        .p14(E1_14),                 // A4
        .p15(BUF_DAT_TRAN_L),        // G_N
        .p16(P5V)                    // VCC
    );

    // E13 74175, zone B5
    // Drawn 74S175, the parts list gives 74175.
    ttl_74175 E13 (
        .p1(PROC_INIT_L),            // CLR_N
        .p2(),                       // Q1
        .p3(E13_3),                  // Q1_N
        .p4(BUF_DAT_TRAN_1_H),       // D1
        .p5(E10_9),                  // D2
        .p6(UBUS_C0_1_L),            // Q2_N
        .p7(UBUS_C0_1_H),            // Q2
        .p8(GND),                    // GND
        .p9(TAP_30_H),               // CLK
        .p10(RESET_INST_H),          // Q3
        .p11(RESET_INST_L),          // Q3_N
        .p12(START_RESET_H),         // D3
        .p13(BUF_C1_1_H),            // D4
        .p14(),                      // Q4_N
        .p15(UBUS_C1_1_H),           // Q4
        .p16(P5V)                    // VCC
    );

    // E25 74S00, zone B6, K2-2 A6, K2-3 B2
    ttl_74s00 E25 (
        .p1(BUF_DAT_TRAN_1_H),       // A1
        .p2(BUF_DAT_TRAN_1_H),       // B1
        .p3(BUF_DAT_TRAN_L),         // Y1
        .p4(CACHE_HIT_H),            // A2
        .p5(CLK_MSYN_H),             // B2
        .p6(E25_6),                  // Y2
        .p7(GND),                    // GND
        .p8(E25_8),                  // Y3, K2-2
        .p9(E44_11),                 // A3, K2-2
        .p10(E16_12),                // B3, K2-2
        .p11(SERV_IR_1_L),           // Y4, K2-3
        .p12(SERV_IR_H),             // A4, K2-3
        .p13(SERV_IR_H),             // B4, K2-3
        .p14(P5V)                    // VCC
    );

    // E26 74S37, zone B5, K2-2 B4
    ttl_74s37 E26 (
        .p1(PWR_CLR_L),              // A1, K2-2
        .p2(E26_11),                 // B1, K2-2
        .p3(PROC_INIT_H),            // Y1, K2-2
        .p4(PROC_INIT_H),            // A2, K2-2
        .p5(PROC_INIT_H),            // B2, K2-2
        .p6(PROC_INIT_L),            // Y2, K2-2
        .p7(GND),                    // GND
        .p8(GET_BUS_H),              // Y3
        .p9(E13_3),                  // A3
        .p10(BUF_DAT_TRAN_L),        // B3
        .p11(E26_11),                // Y4, K2-2
        .p12(RCD_INIT_H),            // A4, K2-2
        .p13(RESET_INST_L),          // B4, K2-2
        .p14(P5V)                    // VCC
    );

// K2-2 PRIORITY ARB, PDF page 41

    // E17 8837, zone D7, K2-3 D7, K2-3 D4
    dm8837 E17 (
        .p1(BUS_AC_LO_L),            // IN4, K2-3
        .p2(E17_2),                  // OUT4, K2-3
        .p3(BUS_BR4_L),              // IN5
        .p4(E17_4),                  // OUT5
        .p5(BUS_BR6_L),              // IN6
        .p6(E17_6),                  // OUT6
        .p7(GND),                    // DISABLE_B, K2-3
        .p8(GND),                    // GND
        .p9(GND),                    // DISABLE_A, K2-3
        .p10(E17_10),                // OUT3
        .p11(BUS_BR7_L),             // IN3
        .p12(E17_12),                // OUT2
        .p13(BUS_BR5_L),             // IN2
        .p14(E17_14),                // OUT1, K2-3
        .p15(BUS_DC_LO_L),           // IN1, K2-3
        .p16(P5V)                    // VCC
    );

    // R12 1 kohm, zone C7
    resistor R12 (
        .p1(P5V),                    // END1
        .p2(BUS_HLT_RQST_L)          // END2
    );

    // E19 74S174, zone C6
    // Pin 14 drawn as 19. The clock is labeled K2-5 PROC CLK L here and K1-5
    // PROC CLK L on K2-3, both on contact ER1.
    ttl_74s174 E19 (
        .p1(SACK_RET_1_L),           // CLR_N
        .p2(E19_2),                  // Q1
        .p3(E17_6),                  // D1
        .p4(E17_4),                  // D2
        .p5(E19_5),                  // Q2
        .p6(HLT_RQST_H),             // D3
        .p7(HLT_RQST_1_H),           // Q3
        .p8(GND),                    // GND
        .p9(PROC_CLK_L),             // CLK
        .p10(PFAIL_1_H),             // Q4
        .p11(PFAIL_H),               // D4
        .p12(E19_12),                // Q5
        .p13(E17_12),                // D5
        .p14(E17_10),                // D6
        .p15(E19_15),                // Q6
        .p16(P5V)                    // VCC
    );

    // E29 82S129, DEC 23-160A2, zone D5
    prom_82s129 E29 (
        .p1(E19_12),                 // A6
        .p2(E19_15),                 // A5
        .p3(E19_5),                  // A4
        .p4(E19_2),                  // A3
        .p5(PSW_07_1_H),             // A0
        .p6(PSW_06_1_H),             // A1
        .p7(HLT_RQST_1_H),           // A2
        .p8(GND),                    // GND
        .p9(BG5_H),                  // O4
        .p10(BG4_H),                 // O3
        .p11(BG7_H),                 // O2
        .p12(BG6_H),                 // O1
        .p13(GND),                   // CE1_N
        .p14(GND),                   // CE2_N
        .p15(PSW_05_1_H),            // A7
        .p16(P5V)                    // VCC
    );

    // E28 7400, zone D2
    ttl_7400 E28 (
        .p1(BG7_H),                  // A1
        .p2(BG_1_H),                 // B1
        .p3(E28_3),                  // Y1
        .p4(BG6_H),                  // A2
        .p5(BG_1_H),                 // B2
        .p6(E28_6),                  // Y2
        .p7(GND),                    // GND
        .p8(E28_8),                  // Y3
        .p9(BG4_H),                  // A3
        .p10(BG_1_H),                // B3
        .p11(E28_11),                // Y4
        .p12(BG5_H),                 // A4
        .p13(BG_1_H),                // B4
        .p14(P5V)                    // VCC
    );

    // E77 8881, zone D2
    // Pin 5 drawn as 6.
    signetics_8881 E77 (
        .p1(BUS_BG4_H),              // Y1
        .p2(E28_8),                  // A1
        .p3(E28_8),                  // B1
        .p4(BUS_BG6_H),              // Y2
        .p5(E28_6),                  // A2
        .p6(E28_6),                  // B2
        .p7(GND),                    // GND
        .p8(E28_3),                  // A3
        .p9(E28_3),                  // B3
        .p10(BUS_BG7_H),             // Y3
        .p11(E28_11),                // B4
        .p12(E28_11),                // A4
        .p13(BUS_BG5_H),             // Y4
        .p14(P5V)                    // VCC
    );

    // R13 178 ohm, zone D2
    resistor R13 (
        .p1(P5V),                    // END1
        .p2(BUS_BG7_H)               // END2
    );

    // R14 178 ohm, zone D2
    resistor R14 (
        .p1(P5V),                    // END1
        .p2(BUS_BG6_H)               // END2
    );

    // R15 178 ohm, zone D2
    resistor R15 (
        .p1(P5V),                    // END1
        .p2(BUS_BG5_H)               // END2
    );

    // R16 178 ohm, zone D2
    resistor R16 (
        .p1(P5V),                    // END1
        .p2(BUS_BG4_H)               // END2
    );

    // E30 8815, zone C5, K2-3 B7
    signetics_8815 E30 (
        .p1(R6_PLUS_7_L),            // A1, K2-3
        .p2(SPA_00_H),               // B1, K2-3
        .p3(),                       // NC
        .p4(ENAB_STOV_L),            // C1, K2-3
        .p5(K110_8_15_EQ_0_L),       // D1, K2-3
        .p6(E30_6),                  // Y1, K2-3
        .p7(GND),                    // GND
        .p8(E30_8),                  // Y2
        .p9(BG5_H),                  // A2
        .p10(BG7_H),                 // B2
        .p11(),                      // NC
        .p12(BG4_H),                 // C2
        .p13(BG6_H),                 // D2
        .p14(P5V)                    // VCC
    );

    // E11 7408, zone C4, K2-2 C3
    // Pin 5 drawn as 6.
    ttl_7408 E11 (
        .p1(NPG_ENAB_H),             // A1
        .p2(NOSACK_H),               // B1
        .p3(E11_3),                  // Y1
        .p4(RCD_INIT_L),             // A2
        .p5(E2_8),                   // B2
        .p6(E11_6),                  // Y2
        .p7(GND),                    // GND
        .p8(ALLOW_BG_H),             // Y3
        .p9(E18_6),                  // A3
        .p10(ENAB_GRANTS_H),         // B3
        .p11(CLK_BG_H),              // Y4
        .p12(BG_ENAB_H),             // A4
        .p13(NOSACK_H),              // B4
        .p14(P5V)                    // VCC
    );

    // E5 7474, zone C6, K2-2 C3
    ttl_7474 E5 (
        .p1(NPR_H),                  // CLR1_N
        .p2(GND),                    // D1
        .p3(GND),                    // CLK1
        .p4(E50_3),                  // PRE1_N
        .p5(E5_5),                   // Q1
        .p6(E5_6),                   // Q1_N
        .p7(GND),                    // GND
        .p8(SACK_RET_1_H),           // Q2_N
        .p9(SACK_RET_1_L),           // Q2
        .p10(NO_SACK_L),             // PRE2_N
        .p11(E12_8),                 // CLK2
        .p12(GND),                   // D2
        .p13(P3V),                   // CLR2_N
        .p14(P5V)                    // VCC
    );

    // R17 150 ohm, zone C5
    resistor R17 (
        .p1(E5_5),                   // END1
        .p2(E21_5)                   // END2
    );

    // C57 470 pF, zone C5
    capacitor C57 (
        .p1(E21_5),                  // END1
        .p2(GND)                     // END2
    );

    // R18 150 ohm, zone C5
    resistor R18 (
        .p1(E5_6),                   // END1
        .p2(E21_9)                   // END2
    );

    // C66 470 pF, zone C5
    capacitor C66 (
        .p1(E21_9),                  // END1
        .p2(GND)                     // END2
    );

    // E12 7474, zone C3, K2-2 B3
    ttl_7474 E12 (
        .p1(P3V),                    // CLR1_N
        .p2(GND),                    // D1
        .p3(E11_3),                  // CLK1
        .p4(E11_6),                  // PRE1_N
        .p5(NPG_1_L),                // Q1
        .p6(NPG_1_H),                // Q1_N
        .p7(GND),                    // GND
        .p8(E12_8),                  // Q2_N
        .p9(BG_1_H),                 // Q2
        .p10(P3V),                   // PRE2_N
        .p11(CLK_BG_H),              // CLK2
        .p12(ALLOW_BG_H),            // D2
        .p13(E11_6),                 // CLR2_N
        .p14(P5V)                    // VCC
    );

    // R22 178 ohm, zone B2
    resistor R22 (
        .p1(P5V),                    // END1
        .p2(BUS_NPG_H)               // END2
    );

    // E15 7400, zone B3, K2-3 D4, K2-3 D3
    // Reference partly illegible on K2-3, E18 pins 4 to 6 are used on K2-2.
    ttl_7400 E15 (
        .p1(E17_2),                  // A1, K2-3
        .p2(E17_2),                  // B1, K2-3
        .p3(E15_3),                  // Y1, K2-3
        .p4(E15_3),                  // A2, K2-3
        .p5(E14_4),                  // B2, K2-3
        .p6(E15_6),                  // Y2, K2-3
        .p7(GND),                    // GND
        .p8(PWR_CLR_L),              // Y3, K2-3
        .p9(E15_6),                  // A3, K2-3
        .p10(E24_13),                // B3, K2-3
        .p11(NO_SACK_L),             // Y4
        .p12(NOSACK_H),              // A4
        .p13(NOSACK_H),              // B4
        .p14(P5V)                    // VCC
    );

    // R20 1 kohm, zone C5
    resistor R20 (
        .p1(P5V),                    // END1
        .p2(E8_4)                    // END2
    );

    // R21 47 ohm, zone C5
    resistor R21 (
        .p1(E8_4),                   // END1
        .p2(E2_12)                   // END2
    );

    // C71 470 pF, zone C5
    capacitor C71 (
        .p1(E2_12),                  // END1
        .p2(GND)                     // END2
    );

    // C60 10 uF, zone B7
    polar_capacitor C60 (
        .p1(E16_RC),                 // POSITIVE
        .p2(E16_6)                   // NEGATIVE
    );

    // R19 30 kohm, zone B7
    resistor R19 (
        .p1(P5V),                    // END1
        .p2(E16_RC)                  // END2
    );

    // D1 D664, zone B7
    diode D1 (
        .p1(E16_RC),                 // ANODE
        .p2(E16_7)                   // CATHODE
    );

// K2-3 PFAIL/AUTO REST BUT SERV, PDF page 42

    // E14 74123, zone D6, K2-10 B3
    // K2-3: 150 ms. K2-10: 22 us.
    ttl_74123 E14 (
        .p1(E17_14),                 // A1_N
        .p2(START_PDWN_1_L),         // B1
        .p3(P3V),                    // CLR1_N
        .p4(E14_4),                  // Q1_N
        .p5(),                       // Q2, K2-10
        .p6(E14_6),                  // CEXT2, K2-10
        .p7(E14_7),                  // REXT_CEXT2, K2-10
        .p8(GND),                    // GND
        .p9(GND),                    // A2_N, K2-10
        .p10(E35_3),                 // B2, K2-10
        .p11(P3V),                   // CLR2_N, K2-10
        .p12(E14_12),                // Q2_N, K2-10
        .p13(E14_13),                // Q1
        .p14(E14_14),                // CEXT1
        .p15(E14_15),                // REXT_CEXT1
        .p16(P5V)                    // VCC
    );

    // C59 15 uF, zone D6
    polar_capacitor C59 (
        .p1(E14_RC),                 // POSITIVE
        .p2(E14_14)                  // NEGATIVE
    );

    // R23 30 kohm, zone D6
    resistor R23 (
        .p1(P5V),                    // END1
        .p2(E14_RC)                  // END2
    );

    // D2 D664, zone D6
    diode D2 (
        .p1(E14_RC),                 // ANODE
        .p2(E14_15)                  // CATHODE
    );

    // E24 7473, zone C6, K2-3 B6
    ttl_7473 E24 (
        .p1(PROC_CLK_H),             // CLK1
        .p2(E23_13),                 // CLR1_N
        .p3(GND),                    // K1
        .p4(P5V),                    // VCC
        .p5(E20_1),                  // CLK2
        .p6(E45_9),                  // CLR2_N
        .p7(E30_6),                  // J2
        .p8(),                       // Q2_N
        .p9(E24_9),                  // Q2
        .p10(GND),                   // K2
        .p11(GND),                   // GND
        .p12(E24_12),                // Q1
        .p13(E24_13),                // Q1_N
        .p14(P3V)                    // J1
    );

    // E7 74123, zone C5, K2-3 C4
    // First half 2.2 ms, second half 6.6 ms.
    ttl_74123 E7 (
        .p1(GND),                    // A1_N
        .p2(E24_12),                 // B1
        .p3(P3V),                    // CLR1_N
        .p4(),                       // Q1_N
        .p5(),                       // Q2
        .p6(E7_6),                   // CEXT2
        .p7(E7_7),                   // REXT_CEXT2
        .p8(GND),                    // GND
        .p9(E7_13),                  // A2_N
        .p10(E15_6),                 // B2
        .p11(P3V),                   // CLR2_N
        .p12(START_PDWN_1_L),        // Q2_N
        .p13(E7_13),                 // Q1
        .p14(E7_14),                 // CEXT1
        .p15(E7_15),                 // REXT_CEXT1
        .p16(P5V)                    // VCC
    );

    // C44 0.68 uF, zone C5
    polar_capacitor C44 (
        .p1(E7_RC1),                 // POSITIVE
        .p2(E7_14)                   // NEGATIVE
    );

    // R24 10 kohm, zone C5
    resistor R24 (
        .p1(P5V),                    // END1
        .p2(E7_RC1)                  // END2
    );

    // D3 D664, zone C5
    diode D3 (
        .p1(E7_RC1),                 // ANODE
        .p2(E7_15)                   // CATHODE
    );

    // C55 0.68 uF, zone C4
    polar_capacitor C55 (
        .p1(E7_RC2),                 // POSITIVE
        .p2(E7_6)                    // NEGATIVE
    );

    // R25 30 kohm, zone C4
    resistor R25 (
        .p1(P5V),                    // END1
        .p2(E7_RC2)                  // END2
    );

    // D4 D664, zone C4
    diode D4 (
        .p1(E7_RC2),                 // ANODE
        .p2(E7_7)                    // CATHODE
    );

    // E45 7473, zone C4, K2-3 A6
    // Drawn E95 on K2-3 C4. The parts list gives 7473 as E24 and E45, and E95
    // as a 74S02.
    ttl_7473 E45 (
        .p1(PROC_CLK_H),             // CLK1
        .p2(PROC_INIT_L),            // CLR1_N
        .p3(GND),                    // K1
        .p4(P5V),                    // VCC
        .p5(PROC_CLK_H),             // CLK2
        .p6(PROC_INIT_L),            // CLR2_N
        .p7(BUT_SERVICE_1_H),        // J2
        .p8(),                       // Q2_N
        .p9(E45_9),                  // Q2
        .p10(STOV_SERV_H),           // K2
        .p11(GND),                   // GND
        .p12(E45_12),                // Q1
        .p13(),                      // Q1_N
        .p14(PFAIL_SERV_H)           // J1
    );

    // E43 7402, zone C2, K2-5 D7, K2-7 A6
    ttl_7402 E43 (
        .p1(CLR_MPC_L),              // Y1, K2-7
        .p2(PROC_INIT_H),            // A1, K2-7
        .p3(ABORT_H),                // B1, K2-7
        .p4(DBE_H),                  // Y2
        .p5(ENAB_DBE_L),             // A2
        .p6(SET_BE_L),               // B2
        .p7(GND),                    // GND
        .p8(BE_1_H),                 // A3
        .p9(PSW_15_1_L),             // B3
        .p10(USER_MODE_H),           // Y3
        .p11(DBE_H),                 // A4, K2-5
        .p12(DBE_H),                 // B4, K2-5
        .p13(E43_13),                // Y4, K2-5
        .p14(P5V)                    // VCC
    );

    // E20 74S02, zone B7, K2-3 B4, K2-3 B3, K2-4 B4
    ttl_74s02 E20 (
        .p1(E20_1),                  // Y1
        .p2(PROC_CLK_L),             // A1
        .p3(USER_MODE_H),            // B1
        .p4(E20_4),                  // Y2
        .p5(BUT_SERVICE_1_H),        // A2
        .p6(PROC_INIT_H),            // B2
        .p7(GND),                    // GND
        .p8(BUT_SERVICE_1_H),        // A3
        .p9(E20_9),                  // B3
        .p10(SERV_IR_H),             // Y3
        .p11(DISAB_MSYN_L),          // A4, K2-4
        .p12(E95_13),                // B4, K2-4
        .p13(E20_13),                // Y4, K2-4
        .p14(P5V)                    // VCC
    );

    // R61 390 ohm, zone A6
    resistor R61 (
        .p1(P5V),                    // END1
        .p2(FP11_SERVICE_BR_PFAIL_L) // END2
    );

    // E52 82S137, DEC 23-013F1, zone B5
    // The pin 15 signal name is faint.
    prom_82s137 E52 (
        .p1(IR_CODE_02_L),           // A6
        .p2(PFAIL_1_H),              // A5
        .p3(KTE_1_H),                // A4
        .p4(PE_1_H),                 // A3
        .p5(E50_11),                 // A0
        .p6(BE_1_H),                 // A1
        .p7(E24_9),                  // A2
        .p8(BUT_SERVICE_L),          // CE1_N
        .p9(GND),                    // GND
        .p10(BUT_SERVICE_L),         // CE2_N
        .p11(E52_11),                // O4
        .p12(E52_12),                // O3
        .p13(E52_13),                // O2
        .p14(E52_14),                // O1
        .p15(FP11_SERVICE_BR_PFAIL_L), // A9
        .p16(IR_CODE_00_L),          // A8
        .p17(IR_CODE_01_L),          // A7
        .p18(P5V)                    // VCC
    );

    // R26 470 ohm, zone B5
    resistor R26 (
        .p1(P5V),                    // END1
        .p2(E52_11)                  // END2
    );

    // R27 470 ohm, zone B5
    resistor R27 (
        .p1(P5V),                    // END1
        .p2(E52_12)                  // END2
    );

    // R28 470 ohm, zone B5
    resistor R28 (
        .p1(P5V),                    // END1
        .p2(E52_13)                  // END2
    );

    // R29 470 ohm, zone B5
    resistor R29 (
        .p1(P5V),                    // END1
        .p2(E52_14)                  // END2
    );

    // E51 82S123, DEC 23-106A1, zone B4
    prom_82s123 E51 (
        .p1(PFAIL_SERV_H),           // O1
        .p2(),                       // O2
        .p3(STOV_SERV_H),            // O3
        .p4(),                       // O4
        .p5(C4_H),                   // O5
        .p6(C3_H),                   // O6
        .p7(C2_H),                   // O7
        .p8(GND),                    // GND
        .p9(ENAB_GRANTS_H),          // O8
        .p10(E52_11),                // A0
        .p11(E52_12),                // A1
        .p12(E52_13),                // A2
        .p13(E52_14),                // A3
        .p14(BUT_SERVICE_1_H),       // A4
        .p15(GND),                   // CE_N
        .p16(P5V)                    // VCC
    );

    // R52 150 ohm, zone B3
    resistor R52 (
        .p1(E20_4),                  // END1
        .p2(E20_9)                   // END2
    );

    // C62 220 pF, zone B3
    capacitor C62 (
        .p1(E20_9),                  // END1
        .p2(GND)                     // END2
    );

    // E3 7400, zone A4
    ttl_7400 E3 (
        .p1(INTR_H),                 // A1
        .p2(INTR_H),                 // B1
        .p3(E3_3),                   // Y1
        .p4(E3_3),                   // A2
        .p5(ENAB_GRANTS_H),          // B2
        .p6(E3_6),                   // Y2
        .p7(GND),                    // GND
        .p8(INT_VECTOR_L),           // Y3
        .p9(E3_6),                   // A3
        .p10(E3_11),                 // B3
        .p11(E3_11),                 // Y4
        .p12(ENAB_GRANTS_H),         // A4
        .p13(ENAB_GRANTS_H),         // B4
        .p14(P5V)                    // VCC
    );

// K2-4 SPA MUX, PDF page 43

    // E58 74S20, zone D7, K2-6 D7
    ttl_74s20 E58 (
        .p1(ROM_SPA_02_1_H),         // A1
        .p2(MODE_01_H),              // B1
        .p3(),                       // NC
        .p4(ROM_SPA_01_1_H),         // C1
        .p5(ROM_SPA_00_1_L),         // D1
        .p6(E58_6),                  // Y1
        .p7(GND),                    // GND
        .p8(E58_8),                  // Y2, K2-6
        .p9(IR_14_1_H),              // A2, K2-6
        .p10(IR_13_1_H),             // B2, K2-6
        .p11(),                      // NC
        .p12(IR_12_1_H),             // C2, K2-6
        .p13(IR_12_1_H),             // D2, K2-6
        .p14(P5V)                    // VCC
    );

    // E47 74S00, zone D6, K2-4 C6, K2-4 B7
    // Drawn E97 or E47. The parts list gives E97 as a microcode PROM.
    ttl_74s00 E47 (
        .p1(IR_06_1_L),              // A1
        .p2(FORCE_RS_PLUS_1_1_L),    // B1
        .p3(E47_3),                  // Y1
        .p4(E57_8),                  // A2
        .p5(E57_8),                  // B2
        .p6(E47_6),                  // Y2
        .p7(GND),                    // GND
        .p8(E47_8),                  // Y3
        .p9(E58_6),                  // A3
        .p10(ROM_SPA_03_1_L),        // B3
        .p11(E47_11),                // Y4
        .p12(E57_6),                 // A4
        .p13(E57_6),                 // B4
        .p14(P5V)                    // VCC
    );

    // E57 74S20, zone D7, K2-4 C7
    ttl_74s20 E57 (
        .p1(IR_07_1_H),              // A1
        .p2(MODE_01_H),              // B1
        .p3(),                       // NC
        .p4(IR_08_1_H),              // C1
        .p5(IR_06_1_L),              // D1
        .p6(E57_6),                  // Y1
        .p7(GND),                    // GND
        .p8(E57_8),                  // Y2
        .p9(IR_02_1_H),              // A2
        .p10(IR_01_1_H),             // B2
        .p11(),                      // NC
        .p12(IR_00_1_L),             // C2
        .p13(MODE_01_H),             // D2
        .p14(P5V)                    // VCC
    );

    // E46 74S153, zone D5
    // Drawn E96, the parts list gives E96 as a 74S174. Select 00 ROM, 01 RS,
    // 10 RD, 11 BA. Pin 10 drawn unclear.
    ttl_74s153 E46 (
        .p1(GND),                    // G1_N
        .p2(SPAM_S1),                // B
        .p3(PBA_03_H),               // C3_1
        .p4(E47_6),                  // C2_1
        .p5(E47_11),                 // C1_1
        .p6(E47_8),                  // C0_1
        .p7(SPA_03_H),               // Y1
        .p8(GND),                    // GND
        .p9(SPA_02_H),               // Y2
        .p10(ROM_SPA_02_1_H),        // C0_2
        .p11(IR_08_1_H),             // C1_2
        .p12(IR_02_1_H),             // C2_2
        .p13(PBA_02_H),              // C3_2
        .p14(SPAM_S0),               // A
        .p15(GND),                   // G2_N
        .p16(P5V)                    // VCC
    );

    // E36 74S153, zone C5
    ttl_74s153 E36 (
        .p1(GND),                    // G1_N
        .p2(SPAM_S1),                // B
        .p3(PBA_01_H),               // C3_1
        .p4(IR_01_1_H),              // C2_1
        .p5(IR_07_1_H),              // C1_1
        .p6(ROM_SPA_01_1_H),         // C0_1
        .p7(SPA_01_H),               // Y1
        .p8(GND),                    // GND
        .p9(SPA_00_H),               // Y2
        .p10(ROM_SPA_00_1_H),        // C0_2
        .p11(E47_3),                 // C1_2
        .p12(IR_00_1_H),             // C2_2
        .p13(PBA_00_H),              // C3_2
        .p14(SPAM_S0),               // A
        .p15(GND),                   // G2_N
        .p16(P5V)                    // VCC
    );

    // E39 74S157, zone B6
    ttl_74s157 E39 (
        .p1(TAP_30_H),               // S
        .p2(SPA_SRC_SEL_0_1_H),      // A1
        .p3(SPA_DST_SEL_0_1_H),      // B1
        .p4(SPAM_S0),                // Y1
        .p5(GND),                    // A2
        .p6(BUF_DAT_TRAN_1_H),       // B2
        .p7(START_TRAN_H),           // Y2
        .p8(GND),                    // GND
        .p9(E39_9),                  // Y3
        .p10(E39_HOLD),              // B3
        .p11(R6_PLUS_7_L),           // A3
        .p12(SPAM_S1),               // Y4
        .p13(SPA_DST_SEL_1_1_H),     // B4
        .p14(SPA_SRC_SEL_1_1_H),     // A4
        .p15(GND),                   // G_N
        .p16(P5V)                    // VCC
    );

    // R63 47 ohm, zone A5
    resistor R63 (
        .p1(E39_9),                  // END1
        .p2(E39_HOLD)                // END2
    );

    // C117 220 pF, zone A5
    capacitor C117 (
        .p1(E39_HOLD),               // END1
        .p2(GND)                     // END2
    );

    // E95 74S02, zone B4, K2-10 B7, K2-10 A7
    ttl_74s02 E95 (
        .p1(E95_1),                  // Y1, K2-10
        .p2(COUNT_05_1_H),           // A1, K2-10
        .p3(COUNT_05_1_H),           // B1, K2-10
        .p4(E95_4),                  // Y2, K2-10
        .p5(PROC_CLK_L),             // A2, K2-10
        .p6(CLK_COUNT_L),            // B2, K2-10
        .p7(GND),                    // GND
        .p8(LOAD_COUNT_L),           // A3, K2-10
        .p9(LOAD_COUNT_L),           // B3, K2-10
        .p10(E95_10),                // Y3, K2-10
        .p11(BUF_DAT_TRAN_1_H),      // A4
        .p12(E39_HOLD),              // B4
        .p13(E95_13),                // Y4
        .p14(P5V)                    // VCC
    );

    // E112 74S04, zone B3, K2-7 D6, K2-7 C2, K2-8 A4
    // Pin 9 also marked J2-1. The J2 table of K2-7 gives MPC 09 L on contact
    // B.
    ttl_74s04 E112 (
        .p1(MPC_02_L),               // A1, K2-7
        .p2(MPC_02_H),               // Y1, K2-7
        .p3(MPC_01_L),               // A2, K2-7
        .p4(MPC_01_H),               // Y2, K2-7
        .p5(E20_13),                 // A3
        .p6(DISABLE_MSYN_PLUS_1_L),  // Y3
        .p7(GND),                    // GND
        .p8(MPC_09_H),               // Y4, K2-7
        .p9(MPC_09_L),               // A4, K2-7
        .p10(E112_10),               // Y5, K2-8
        .p11(E86_11),                // A5, K2-8
        .p12(MPC_00_H),              // Y6, K2-7
        .p13(MPC_00_L),              // A6, K2-7
        .p14(P5V)                    // VCC
    );

    // E91 7405, zone B3, K2-7 C4, K2-8 B2
    ttl_7405 E91 (
        .p1(E87_1),                  // A1, K2-8
        .p2(BMODE_01_L),             // Y1, K2-8
        .p3(E90_7),                  // A2, K2-7
        .p4(MPC_01_L),               // Y2, K2-7
        .p5(E90_5),                  // A3, K2-7
        .p6(MPC_02_L),               // Y3, K2-7
        .p7(GND),                    // GND
        .p8(MPC_00_L),               // Y4, K2-7
        .p9(E90_10),                 // A4, K2-7
        .p10(BMODE_00_L),            // Y5, K2-8
        .p11(E87_2),                 // A5, K2-8
        .p12(SP_WRITE_L),            // Y6
        .p13(E37_11),                // A6
        .p14(P5V)                    // VCC
    );

    // R51 470 ohm, zone B3
    resistor R51 (
        .p1(P5V),                    // END1
        .p2(SP_WRITE_L)              // END2
    );

// K2-5 IR, AUX CNTRL, CC CODE, PDF page 44

    // E66 74S174, zone C6
    ttl_74s174 E66 (
        .p1(E73_13),                 // CLR_N
        .p2(IR_14_1_H),              // Q1
        .p3(SSMUX_14_H),             // D1
        .p4(SSMUX_12_H),             // D2
        .p5(IR_12_1_H),              // Q2
        .p6(SSMUX_10_H),             // D3
        .p7(IR_10_1_H),              // Q3
        .p8(GND),                    // GND
        .p9(LOAD_IR_H),              // CLK
        .p10(IR_09_1_H),             // Q4
        .p11(SSMUX_09_H),            // D4
        .p12(),                      // Q5
        .p13(),                      // D5
        .p14(SSMUX_13_H),            // D6
        .p15(IR_13_1_H),             // Q6
        .p16(P5V)                    // VCC
    );

    // E56 74S174, zone B6
    ttl_74s174 E56 (
        .p1(E73_13),                 // CLR_N
        .p2(IR_07_1_H),              // Q1
        .p3(SSMUX_07_H),             // D1
        .p4(SSMUX_04_H),             // D2
        .p5(IR_04_1_H),              // Q2
        .p6(SSMUX_02_H),             // D3
        .p7(IR_02_1_H),              // Q3
        .p8(GND),                    // GND
        .p9(LOAD_IR_H),              // CLK
        .p10(IR_01_1_H),             // Q4
        .p11(SSMUX_01_H),            // D4
        .p12(IR_03_1_H),             // Q5
        .p13(SSMUX_03_H),            // D5
        .p14(SSMUX_05_H),            // D6
        .p15(IR_05_1_H),             // Q6
        .p16(P5V)                    // VCC
    );

    // E67 74S175, zone A6
    // Pin 14 marked N/C.
    ttl_74s175 E67 (
        .p1(E73_13),                 // CLR_N
        .p2(IR_11_1_H),              // Q1
        .p3(IR_11_1_L),              // Q1_N
        .p4(SSMUX_11_H),             // D1
        .p5(SSMUX_06_H),             // D2
        .p6(IR_06_1_L),              // Q2_N
        .p7(IR_06_1_H),              // Q2
        .p8(GND),                    // GND
        .p9(LOAD_IR_H),              // CLK
        .p10(IR_00_1_H),             // Q3
        .p11(IR_00_1_L),             // Q3_N
        .p12(SSMUX_00_H),            // D3
        .p13(SSMUX_08_H),            // D4
        .p14(),                      // Q4_N
        .p15(IR_08_1_H),             // Q4
        .p16(P5V)                    // VCC
    );

    // E68 82S131, DEC 23-014A9, zone D4
    prom_82s131 E68 (
        .p1(IR_15_1_H),              // A6
        .p2(IR_14_1_H),              // A5
        .p3(IR_06_1_H),              // A4
        .p4(IR_07_1_H),              // A3
        .p5(IR_12_1_H),              // A0
        .p6(IR_09_1_H),              // A1
        .p7(IR_10_1_H),              // A2
        .p8(GND),                    // GND
        .p9(BYTE_L),                 // O4
        .p10(CC_CODE_02_H),          // O3
        .p11(CC_CODE_01_H),          // O2
        .p12(CC_CODE_00_H),          // O1
        .p13(GND),                   // CE_N
        .p14(IR_08_1_H),             // A8
        .p15(IR_13_1_H),             // A7
        .p16(P5V)                    // VCC
    );

    // E64 74S03, zone D4, K2-6 A3
    ttl_74s03 E64 (
        .p1(IR_10_1_H),              // A1, K2-6
        .p2(SRC_H),                  // B1, K2-6
        .p3(MPC_01_L),               // Y1, K2-6
        .p4(IR_DECODE_1_L),          // A2
        .p5(BYTE_L),                 // B2
        .p6(BYTE_H),                 // Y2
        .p7(GND),                    // GND
        .p8(MPC_00_L),               // Y3, K2-6
        .p9(IR_09_1_H),              // A3, K2-6
        .p10(SRC_H),                 // B3, K2-6
        .p11(MPC_02_L),              // Y4, K2-6
        .p12(IR_11_1_H),             // A4, K2-6
        .p13(SRC_H),                 // B4, K2-6
        .p14(P5V)                    // VCC
    );

    // R53 470 ohm, zone D3
    resistor R53 (
        .p1(P5V),                    // END1
        .p2(BYTE_H)                  // END2
    );

    // E83 82S123, DEC 23-107A1, zone C3
    prom_82s123 E83 (
        .p1(FUNC_CODE_02_H),         // O1
        .p2(FUNC_CODE_01_H),         // O2
        .p3(FUNC_CODE_00_H),         // O3
        .p4(),                       // O4
        .p5(),                       // O5
        .p6(),                       // O6
        .p7(),                       // O7
        .p8(GND),                    // GND
        .p9(FUNC_CODE_03_H),         // O8
        .p10(GND),                   // A0
        .p11(IR_12_1_H),             // A1
        .p12(IR_13_1_H),             // A2
        .p13(IR_14_1_H),             // A3
        .p14(IR_15_1_H),             // A4
        .p15(E84_8),                 // CE_N
        .p16(P5V)                    // VCC
    );

    // E84 74S00, zone B4, K2-6 A6, K2-6 B5
    ttl_74s00 E84 (
        .p1(IR_11_1_L),              // A1, K2-6
        .p2(IR12_14_EQ_0_H),         // B1, K2-6
        .p3(E84_3),                  // Y1, K2-6
        .p4(IR12_14_EQ_0_H),         // A2, K2-6
        .p5(IR12_14_EQ_0_H),         // B2, K2-6
        .p6(IR12_14_EQ_0_L),         // Y2, K2-6
        .p7(GND),                    // GND
        .p8(E84_8),                  // Y3
        .p9(AUX_SET_UP_H),           // A3
        .p10(IR12_14_EQ_0_L),        // B3
        .p11(E84_11),                // Y4
        .p12(IR12_14_EQ_0_H),        // A4
        .p13(AUX_SET_UP_H),          // B4
        .p14(P5V)                    // VCC
    );

    // E61 82S129, DEC 23-161A2, zone B3
    prom_82s129 E61 (
        .p1(N_BIT_1_H),              // A6
        .p2(IR_10_1_H),              // A5
        .p3(IR_09_1_H),              // A4
        .p4(IR_08_1_H),              // A3
        .p5(IR_15_1_H),              // A0
        .p6(IR_06_1_H),              // A1
        .p7(IR_07_1_H),              // A2
        .p8(GND),                    // GND
        .p9(FUNC_CODE_03_H),         // O4
        .p10(FUNC_CODE_02_H),        // O3
        .p11(FUNC_CODE_01_H),        // O2
        .p12(FUNC_CODE_00_H),        // O1
        .p13(E84_11),                // CE1_N
        .p14(E84_11),                // CE2_N
        .p15(C_BIT_1_H),             // A7
        .p16(P5V)                    // VCC
    );

    // E75 7427, zone B4, K2-6 B7
    ttl_7427 E75 (
        .p1(IR_14_1_H),              // A1
        .p2(IR_13_1_H),              // B1
        .p3(IR_11_1_H),              // A2, K2-6
        .p4(IR_10_1_H),              // B2, K2-6
        .p5(IR_09_1_H),              // C2, K2-6
        .p6(SM0_H),                  // Y2, K2-6
        .p7(GND),                    // GND
        .p8(DM0_H),                  // Y3, K2-6
        .p9(IR_05_1_H),              // A3, K2-6
        .p10(IR_04_1_H),             // B3, K2-6
        .p11(IR_03_1_H),             // C3, K2-6
        .p12(IR12_14_EQ_0_H),        // Y1
        .p13(IR_12_1_H),             // C1
        .p14(P5V)                    // VCC
    );

    // E62 82S126, DEC 23-170A2, zone A3
    prom_82s126 E62 (
        .p1(IR_07_1_H),              // A6
        .p2(IR_06_1_H),              // A5
        .p3(IR_09_1_H),              // A4
        .p4(CC_N_H),                 // A3
        .p5(IR_10_1_H),              // A0
        .p6(C_BIT_1_H),              // A1
        .p7(BREG_00_1_H),            // A2
        .p8(GND),                    // GND
        .p9(E62_9),                  // O4
        .p10(BMODE_00_L),            // O3
        .p11(BMODE_01_L),            // O2
        .p12(SERIAL_SHIFT_H),        // O1
        .p13(E84_11),                // CE1_N
        .p14(E84_11),                // CE2_N
        .p15(IR_08_1_H),             // A7
        .p16(P5V)                    // VCC
    );

    // R30 470 ohm, zone A3
    resistor R30 (
        .p1(P5V),                    // END1
        .p2(SERIAL_SHIFT_H)          // END2
    );

    // R31 470 ohm, zone A3
    resistor R31 (
        .p1(P5V),                    // END1
        .p2(E62_9)                   // END2
    );

    // E55 74S175, zone A2, K2-6 D7, K2-8 C5
    ttl_74s175 E55 (
        .p1(PROC_INIT_L),            // CLR_N, K2-8
        .p2(IR_DECODE_1_H),          // Q1, K2-6
        .p3(IR_DECODE_1_L),          // Q1_N, K2-6
        .p4(E65_6),                  // D1, K2-6
        .p5(),                       // D2
        .p6(),                       // Q2_N
        .p7(),                       // Q2
        .p8(GND),                    // GND
        .p9(PROC_CLK_L),             // CLK, K2-8
        .p10(AUX_CONTROL_1_H),       // Q3, K2-8
        .p11(AUX_CONTROL_1_L),       // Q3_N, K2-8
        .p12(AUX_SET_UP_H),          // D3, K2-8
        .p13(E62_9),                 // D4
        .p14(),                      // Q4_N
        .p15(ROT_CBIT_1_H),          // Q4
        .p16(P5V)                    // VCC
    );

// K2-6 IR DECODE, PDF page 45

    // E65 74S03, zone D7, K2-6 B3
    ttl_74s03 E65 (
        .p1(E85_8),                  // A1
        .p2(IR_04_1_H),              // B1
        .p3(MPC_01_L),               // Y1
        .p4(LOAD_IR_L),              // A2
        .p5(LOAD_IR_L),              // B2
        .p6(E65_6),                  // Y2
        .p7(GND),                    // GND
        .p8(MPC_00_L),               // Y3
        .p9(IR_03_1_H),              // A3
        .p10(E85_8),                 // B3
        .p11(MPC_02_L),              // Y4
        .p12(IR_05_1_H),             // A4
        .p13(E85_8),                 // B4
        .p14(P5V)                    // VCC
    );

    // R32 1 kohm, zone D7
    resistor R32 (
        .p1(P5V),                    // END1
        .p2(E65_6)                   // END2
    );

    // E74 82S23, DEC 23-110A1, zone D6
    // Pin 6 is labeled IR CODE 00 without polarity.
    prom_82s23 E74 (
        .p1(),                       // O1
        .p2(MPC_03_L),               // O2
        .p3(MPC_04_L),               // O3
        .p4(MPC_05_L),               // O4
        .p5(MPC_06_L),               // O5
        .p6(IR_CODE_00),             // O6
        .p7(DEST_L),                 // O7
        .p8(GND),                    // GND
        .p9(MPC_07_L),               // O8
        .p10(IR_09_1_H),             // A0
        .p11(IR_10_1_H),             // A1
        .p12(IR_11_1_H),             // A2
        .p13(IR_DECODE_1_H),         // A3
        .p14(E58_8),                 // A4
        .p15(IR_15_1_H),             // CE_N
        .p16(P5V)                    // VCC
    );

    // R33 470 ohm, zone D6
    resistor R33 (
        .p1(P5V),                    // END1
        .p2(DEST_L)                  // END2
    );

    // R59 1 kohm, zone C7
    resistor R59 (
        .p1(P5V),                    // END1
        .p2(FP11_A_ATTACHED_L)       // END2
    );

    // E70 82S130, DEC 23-349A9, zone C6
    prom_82s130 E70 (
        .p1(IR_15_1_H),              // A6
        .p2(IR_14_1_H),              // A5
        .p3(BUT_DEST_L),             // A4
        .p4(IR_DECODE_1_H),          // A3
        .p5(IR_12_1_H),              // A0
        .p6(DM0_H),                  // A1
        .p7(SM0_H),                  // A2
        .p8(GND),                    // GND
        .p9(MPC_07_L),               // O4
        .p10(MPC_06_L),              // O3
        .p11(MPC_05_L),              // O2
        .p12(IR_CODE_00),            // O1
        .p13(GND),                   // CE_N
        .p14(FP11_A_ATTACHED_L),     // A8
        .p15(IR_13_1_H),             // A7
        .p16(P5V)                    // VCC
    );

    // E69 82S126, DEC 23-172A2, zone B6
    prom_82s126 E69 (
        .p1(IR_15_1_H),              // A6
        .p2(IR_14_1_H),              // A5
        .p3(BUT_DEST_L),             // A4
        .p4(IR_DECODE_1_H),          // A3
        .p5(IR_12_1_H),              // A0
        .p6(DM0_H),                  // A1
        .p7(SM0_H),                  // A2
        .p8(GND),                    // GND
        .p9(MPC_04_L),               // O4
        .p10(MPC_03_L),              // O3
        .p11(MOV_L),                 // O2
        .p12(SRC_H),                 // O1
        .p13(GND),                   // CE1_N
        .p14(GND),                   // CE2_N
        .p15(IR_13_1_H),             // A7
        .p16(P5V)                    // VCC
    );

    // R34 470 ohm, zone B6
    resistor R34 (
        .p1(P5V),                    // END1
        .p2(MOV_L)                   // END2
    );

    // R35 470 ohm, zone B6
    resistor R35 (
        .p1(P5V),                    // END1
        .p2(SRC_H)                   // END2
    );

    // E72 82S126, DEC 23-A22A2, zone A6
    prom_82s126 E72 (
        .p1(N_BIT_1_H),              // A6
        .p2(Z_BIT_1_H),              // A5
        .p3(V_BIT_1_H),              // A4
        .p4(IR_15_1_H),              // A3
        .p5(IR_08_1_H),              // A0
        .p6(IR_09_1_H),              // A1
        .p7(IR_10_1_H),              // A2
        .p8(GND),                    // GND
        .p9(),                       // O4
        .p10(),                      // O3
        .p11(),                      // O2
        .p12(MPC_07_L),              // O1
        .p13(E84_3),                 // CE1_N
        .p14(IR_DECODE_1_L),         // CE2_N
        .p15(C_BIT_1_H),             // A7
        .p16(P5V)                    // VCC
    );

    // E60 82S126, DEC 23-173A2, zone C5
    prom_82s126 E60 (
        .p1(IR_11_1_H),              // A6
        .p2(IR_10_1_H),              // A5
        .p3(IR_09_1_H),              // A4
        .p4(IR_08_1_H),              // A3
        .p5(DM0_H),                  // A0
        .p6(IR_06_1_H),              // A1
        .p7(IR_07_1_H),              // A2
        .p8(GND),                    // GND
        .p9(MPC_06_L),               // O4
        .p10(MPC_05_L),              // O3
        .p11(MPC_04_L),              // O2
        .p12(MPC_03_L),              // O1
        .p13(IR12_14_EQ_0_L),        // CE1_N
        .p14(IR_DECODE_1_L),         // CE2_N
        .p15(IR_15_1_H),             // A7
        .p16(P5V)                    // VCC
    );

    // E59 82S126, DEC 23-174A2, zone B5
    prom_82s126 E59 (
        .p1(IR_11_1_H),              // A6
        .p2(IR_10_1_H),              // A5
        .p3(IR_09_1_H),              // A4
        .p4(IR_08_1_H),              // A3
        .p5(DM0_H),                  // A0
        .p6(IR_06_1_H),              // A1
        .p7(IR_07_1_H),              // A2
        .p8(GND),                    // GND
        .p9(IR_CODE_02_L),           // O4
        .p10(IR_CODE_01_L),          // O3
        .p11(IR_CODE_00_L),          // O2
        .p12(E59_12),                // O1
        .p13(IR12_14_EQ_0_L),        // CE1_N
        .p14(IR12_14_EQ_0_L),        // CE2_N
        .p15(IR_15_1_H),             // A7
        .p16(P5V)                    // VCC
    );

    // R36 470 ohm, zone B4
    resistor R36 (
        .p1(P5V),                    // END1
        .p2(IR_CODE_02_L)            // END2
    );

    // R37 470 ohm, zone B4
    resistor R37 (
        .p1(P5V),                    // END1
        .p2(IR_CODE_01_L)            // END2
    );

    // R38 470 ohm, zone B4
    resistor R38 (
        .p1(P5V),                    // END1
        .p2(IR_CODE_00_L)            // END2
    );

    // R39 470 ohm, zone B4
    resistor R39 (
        .p1(P5V),                    // END1
        .p2(E59_12)                  // END2
    );

    // E85 74S10, zone B4, K2-6 C3
    ttl_74s10 E85 (
        .p1(E59_12),                 // A1
        .p2(IR12_14_EQ_0_H),         // B1
        .p3(IR12_14_EQ_0_H),         // A2
        .p4(SM0_H),                  // B2
        .p5(E73_4),                  // C2
        .p6(E85_6),                  // Y2
        .p7(GND),                    // GND
        .p8(E85_8),                  // Y3
        .p9(BUT_DEST_L),             // A3
        .p10(DEST_L),                // B3
        .p11(E85_12),                // C3
        .p12(E85_12),                // Y1
        .p13(IR_DECODE_1_H),         // C1
        .p14(P5V)                    // VCC
    );

    // E54 82S129, DEC 23-162A2, zone C2
    // Pin 10 is also named RTT H.
    prom_82s129 E54 (
        .p1(PSW_15_1_L),             // A6
        .p2(IR_07_1_H),              // A5
        .p3(IR_02_1_H),              // A4
        .p4(IR_01_1_H),              // A3
        .p5(IR_00_1_H),              // A0
        .p6(IR_06_1_H),              // A1
        .p7(DM0_H),                  // A2
        .p8(GND),                    // GND
        .p9(START_RESET_H),          // O4
        .p10(ENAB_TBIT_H),           // O3
        .p11(DISABLE_LOAD_PSW_H),    // O2
        .p12(),                      // O1
        .p13(GND),                   // CE1_N
        .p14(GND),                   // CE2_N
        .p15(E85_6),                 // A7
        .p16(P5V)                    // VCC
    );

    // E53 82S126, DEC 23-175A2, zone B2
    prom_82s126 E53 (
        .p1(IR_04_1_H),              // A6
        .p2(IR_03_1_H),              // A5
        .p3(IR_02_1_H),              // A4
        .p4(IR_01_1_H),              // A3
        .p5(IR_00_1_H),              // A0
        .p6(IR_05_1_H),              // A1
        .p7(IR_07_1_H),              // A2
        .p8(GND),                    // GND
        .p9(IR_CODE_02_L),           // O4
        .p10(IR_CODE_01_L),          // O3
        .p11(IR_CODE_00_L),          // O2
        .p12(BUS_HLT_RQST_L),        // O1
        .p13(IR_06_1_H),             // CE1_N
        .p14(E85_6),                 // CE2_N
        .p15(USER_MODE_H),           // A7
        .p16(P5V)                    // VCC
    );

    // E63 82S126, DEC 23-176A2, zone B2
    prom_82s126 E63 (
        .p1(IR_04_1_H),              // A6
        .p2(IR_03_1_H),              // A5
        .p3(IR_02_1_H),              // A4
        .p4(IR_01_1_H),              // A3
        .p5(IR_00_1_H),              // A0
        .p6(IR_05_1_H),              // A1
        .p7(IR_07_1_H),              // A2
        .p8(GND),                    // GND
        .p9(MPC_00_L),               // O4
        .p10(MPC_01_L),              // O3
        .p11(MPC_02_L),              // O2
        .p12(MPC_03_L),              // O1
        .p13(IR_DECODE_1_L),         // CE1_N
        .p14(E85_6),                 // CE2_N
        .p15(USER_MODE_H),           // A7
        .p16(P5V)                    // VCC
    );

// K2-7 MPC BUT FIELD, PDF page 46

    // R60 390 ohm, zone D6
    resistor R60 (
        .p1(P5V),                    // END1
        .p2(MPC_09_L)                // END2
    );

    // E110 82S137, DEC 23-016F1, zone D7
    prom_82s137 E110 (
        .p1(MPC_00_H),               // A6
        .p2(MPC_01_H),               // A5
        .p3(MPC_02_H),               // A4
        .p4(MPC_03_H),               // A3
        .p5(MPC_06_H),               // A0
        .p6(MPC_04_H),               // A1
        .p7(MPC_07_H),               // A2
        .p8(GND),                    // CE1_N
        .p9(GND),                    // GND
        .p10(GND),                   // CE2_N
        .p11(E110_11),               // O4
        .p12(E110_12),               // O3
        .p13(E110_13),               // O2
        .p14(E110_14),               // O1
        .p15(MPC_09_H),              // A9
        .p16(MPC_05_H),              // A8
        .p17(MPC_08_H),              // A7
        .p18(P5V)                    // VCC
    );

    // E109 82S137, DEC 23-017F1, zone C7
    prom_82s137 E109 (
        .p1(MPC_00_H),               // A6
        .p2(MPC_01_H),               // A5
        .p3(MPC_02_H),               // A4
        .p4(MPC_03_H),               // A3
        .p5(MPC_06_H),               // A0
        .p6(MPC_04_H),               // A1
        .p7(MPC_07_H),               // A2
        .p8(GND),                    // CE1_N
        .p9(GND),                    // GND
        .p10(GND),                   // CE2_N
        .p11(E109_11),               // O4
        .p12(E109_12),               // O3
        .p13(E109_13),               // O2
        .p14(E109_14),               // O1
        .p15(MPC_09_H),              // A9
        .p16(MPC_05_H),              // A8
        .p17(MPC_08_H),              // A7
        .p18(P5V)                    // VCC
    );

    // E108 82S137, DEC 23-018F1, zone B7
    prom_82s137 E108 (
        .p1(MPC_00_H),               // A6
        .p2(MPC_01_H),               // A5
        .p3(MPC_02_H),               // A4
        .p4(MPC_03_H),               // A3
        .p5(MPC_06_H),               // A0
        .p6(MPC_04_H),               // A1
        .p7(MPC_07_H),               // A2
        .p8(GND),                    // CE1_N
        .p9(GND),                    // GND
        .p10(GND),                   // CE2_N
        .p11(E108_11),               // O4
        .p12(E108_12),               // O3
        .p13(E108_13),               // O2
        .p14(E108_14),               // O1
        .p15(MPC_09_H),              // A9
        .p16(MPC_05_H),              // A8
        .p17(MPC_08_H),              // A7
        .p18(P5V)                    // VCC
    );

    // E92 74S174, zone D5
    ttl_74s174 E92 (
        .p1(CLR_MPC_L),              // CLR_N
        .p2(E92_2),                  // Q1
        .p3(E109_11),                // D1
        .p4(E109_12),                // D2
        .p5(E92_5),                  // Q2
        .p6(E110_11),                // D3
        .p7(E92_7),                  // Q3
        .p8(GND),                    // GND
        .p9(PROC_CLK_L),             // CLK
        .p10(E92_10),                // Q4
        .p11(E110_14),               // D4
        .p12(E92_12),                // Q5
        .p13(E110_13),               // D5
        .p14(E110_12),               // D6
        .p15(E92_15),                // Q6
        .p16(P5V)                    // VCC
    );

    // E90 74S174, zone B5
    ttl_74s174 E90 (
        .p1(CLR_MPC_L),              // CLR_N
        .p2(E90_2),                  // Q1
        .p3(E108_14),                // D1
        .p4(E109_13),                // D2
        .p5(E90_5),                  // Q2
        .p6(E109_14),                // D3
        .p7(E90_7),                  // Q3
        .p8(GND),                    // GND
        .p9(PROC_CLK_L),             // CLK
        .p10(E90_10),                // Q4
        .p11(E108_11),               // D4
        .p12(E90_12),                // Q5
        .p13(E108_12),               // D5
        .p14(E108_13),               // D6
        .p15(E90_15),                // Q6
        .p16(P5V)                    // VCC
    );

    // E101 7405, zone D4
    ttl_7405 E101 (
        .p1(E92_2),                  // A1
        .p2(MPC_04_L),               // Y1
        .p3(E92_5),                  // A2
        .p4(MPC_03_L),               // Y2
        .p5(E92_7),                  // A3
        .p6(MPC_08_L),               // Y3
        .p7(GND),                    // GND
        .p8(MPC_05_L),               // Y4
        .p9(E92_10),                 // A4
        .p10(MPC_06_L),              // Y5
        .p11(E92_12),                // A5
        .p12(MPC_07_L),              // Y6
        .p13(E92_15),                // A6
        .p14(P5V)                    // VCC
    );

    // R40 390 ohm, zone D3
    resistor R40 (
        .p1(P5V),                    // END1
        .p2(MPC_08_L)                // END2
    );

    // R41 390 ohm, zone D3
    resistor R41 (
        .p1(P5V),                    // END1
        .p2(MPC_07_L)                // END2
    );

    // R42 390 ohm, zone D3
    resistor R42 (
        .p1(P5V),                    // END1
        .p2(MPC_06_L)                // END2
    );

    // R43 390 ohm, zone D3
    resistor R43 (
        .p1(P5V),                    // END1
        .p2(MPC_05_L)                // END2
    );

    // R44 390 ohm, zone C3
    resistor R44 (
        .p1(P5V),                    // END1
        .p2(MPC_04_L)                // END2
    );

    // R45 390 ohm, zone C3
    resistor R45 (
        .p1(P5V),                    // END1
        .p2(MPC_03_L)                // END2
    );

    // R46 390 ohm, zone C3
    resistor R46 (
        .p1(P5V),                    // END1
        .p2(MPC_02_L)                // END2
    );

    // R47 390 ohm, zone C3
    resistor R47 (
        .p1(P5V),                    // END1
        .p2(MPC_01_L)                // END2
    );

    // R48 390 ohm, zone C3
    resistor R48 (
        .p1(P5V),                    // END1
        .p2(MPC_00_L)                // END2
    );

    // E111 74S04, zone D2
    ttl_74s04 E111 (
        .p1(MPC_08_L),               // A1
        .p2(MPC_08_H),               // Y1
        .p3(MPC_03_L),               // A2
        .p4(MPC_03_H),               // Y2
        .p5(MPC_04_L),               // A3
        .p6(MPC_04_H),               // Y3
        .p7(GND),                    // GND
        .p8(MPC_07_H),               // Y4
        .p9(MPC_07_L),               // A4
        .p10(MPC_06_H),              // Y5
        .p11(MPC_06_L),              // A5
        .p12(MPC_05_H),              // Y6
        .p13(MPC_05_L),              // A6
        .p14(P5V)                    // VCC
    );

    // E89 74S138, zone B3
    ttl_74s138 E89 (
        .p1(E90_2),                  // A
        .p2(E90_15),                 // B
        .p3(E90_12),                 // C
        .p4(GND),                    // G2A_N
        .p5(GND),                    // G2B_N
        .p6(P3VA),                   // G1
        .p7(CLK_COUNT_L),            // Y7_N
        .p8(GND),                    // GND
        .p9(LOAD_COUNT_L),           // Y6_N
        .p10(ENAB_STOV_L),           // Y5_N
        .p11(BUT_DEST_L),            // Y4_N
        .p12(LOAD_CC_L),             // Y3_N
        .p13(LOAD_PSW_L),            // Y2_N
        .p14(LOAD_IR_L),             // Y1_N
        .p15(),                      // Y0_N
        .p16(P5V)                    // VCC
    );

// K2-8 MICRO STORE, PDF page 47

    // E107 82S137, DEC 23-019F1, zone D7
    prom_82s137 E107 (
        .p1(MPC_00_H),               // A6
        .p2(MPC_01_H),               // A5
        .p3(MPC_02_H),               // A4
        .p4(MPC_03_H),               // A3
        .p5(MPC_06_H),               // A0
        .p6(MPC_04_H),               // A1
        .p7(MPC_07_H),               // A2
        .p8(GND),                    // CE1_N
        .p9(GND),                    // GND
        .p10(GND),                   // CE2_N
        .p11(E107_11),               // O4
        .p12(E107_12),               // O3
        .p13(E107_13),               // O2
        .p14(E107_14),               // O1
        .p15(MPC_09_H),              // A9
        .p16(MPC_05_H),              // A8
        .p17(MPC_08_H),              // A7
        .p18(P5V)                    // VCC
    );

    // E106 82S137, DEC 23-020F1, zone C7
    prom_82s137 E106 (
        .p1(MPC_00_H),               // A6
        .p2(MPC_01_H),               // A5
        .p3(MPC_02_H),               // A4
        .p4(MPC_03_H),               // A3
        .p5(MPC_06_H),               // A0
        .p6(MPC_04_H),               // A1
        .p7(MPC_07_H),               // A2
        .p8(GND),                    // CE1_N
        .p9(GND),                    // GND
        .p10(GND),                   // CE2_N
        .p11(E106_11),               // O4
        .p12(E106_12),               // O3
        .p13(AUX_SET_UP_H),          // O2
        .p14(E106_14),               // O1
        .p15(MPC_09_H),              // A9
        .p16(MPC_05_H),              // A8
        .p17(MPC_08_H),              // A7
        .p18(P5V)                    // VCC
    );

    // E99 82S137, DEC 23-021F1, zone B7
    prom_82s137 E99 (
        .p1(MPC_00_H),               // A6
        .p2(MPC_01_H),               // A5
        .p3(MPC_02_H),               // A4
        .p4(MPC_03_H),               // A3
        .p5(MPC_06_H),               // A0
        .p6(MPC_04_H),               // A1
        .p7(MPC_07_H),               // A2
        .p8(AUX_SET_UP_H),           // CE1_N
        .p9(GND),                    // GND
        .p10(AUX_SET_UP_H),          // CE2_N
        .p11(FUNC_CODE_03_H),        // O4
        .p12(FUNC_CODE_02_H),        // O3
        .p13(FUNC_CODE_01_H),        // O2
        .p14(FUNC_CODE_00_H),        // O1
        .p15(MPC_09_H),              // A9
        .p16(MPC_05_H),              // A8
        .p17(MPC_08_H),              // A7
        .p18(P5V)                    // VCC
    );

    // E97 82S137, DEC 23-022F1, zone B7
    prom_82s137 E97 (
        .p1(MPC_00_H),               // A6
        .p2(MPC_01_H),               // A5
        .p3(MPC_02_H),               // A4
        .p4(MPC_03_H),               // A3
        .p5(MPC_06_H),               // A0
        .p6(MPC_04_H),               // A1
        .p7(MPC_07_H),               // A2
        .p8(GND),                    // CE1_N
        .p9(GND),                    // GND
        .p10(GND),                   // CE2_N
        .p11(E97_11),                // O4
        .p12(E97_12),                // O3
        .p13(E97_13),                // O2
        .p14(E97_14),                // O1
        .p15(MPC_09_H),              // A9
        .p16(MPC_05_H),              // A8
        .p17(MPC_08_H),              // A7
        .p18(P5V)                    // VCC
    );

    // E96 74S174, zone D5
    // The drawing labels pin 3 D5 and pin 14 D0. In a 74174 pin 3 drives pin
    // 2 and pin 14 drives pin 15, so LONG CYCLE is micro word bit 16 and LOAD
    // BA bit 17.
    ttl_74s174 E96 (
        .p1(PROC_INIT_L),            // CLR_N
        .p2(LONG_CYCLE_1_L),         // Q1
        .p3(E106_11),                // D1
        .p4(E107_14),                // D2
        .p5(ENAB_MAINT_1_H),         // Q2
        .p6(E107_12),                // D3
        .p7(BUF_C1_1_H),             // Q3
        .p8(GND),                    // GND
        .p9(PROC_CLK_L),             // CLK
        .p10(BUF_DAT_TRAN_1_H),      // Q4
        .p11(E107_11),               // D4
        .p12(BUF_C0_1_H),            // Q5
        .p13(E107_13),               // D5
        .p14(E106_12),               // D6
        .p15(LOAD_BA_1_H),           // Q6
        .p16(P5V)                    // VCC
    );

    // E71 74S174, zone C5
    ttl_74s174 E71 (
        .p1(PROC_INIT_L),            // CLR_N
        .p2(E71_2),                  // Q1
        .p3(FUNC_CODE_00_H),         // D1
        .p4(E106_14),                // D2
        .p5(E71_5),                  // Q2
        .p6(),                       // D3
        .p7(),                       // Q3
        .p8(GND),                    // GND
        .p9(PROC_CLK_L),             // CLK
        .p10(E71_10),                // Q4
        .p11(FUNC_CODE_01_H),        // D4
        .p12(E71_12),                // Q5
        .p13(FUNC_CODE_02_H),        // D5
        .p14(FUNC_CODE_03_H),        // D6
        .p15(E71_15),                // Q6
        .p16(P5V)                    // VCC
    );

    // E82 82S123, DEC 23-108A1, zone C4
    prom_82s123 E82 (
        .p1(E82_1),                  // O1
        .p2(E82_2),                  // O2
        .p3(ALU_MODE_H),             // O3
        .p4(E82_4),                  // O4
        .p5(E82_5),                  // O5
        .p6(ALU_CIN_L),              // O6
        .p7(BLEG_01_H),              // O7
        .p8(GND),                    // GND
        .p9(BLEG_00_H),              // O8
        .p10(E71_10),                // A0
        .p11(E71_5),                 // A1
        .p12(E71_12),                // A2
        .p13(E71_15),                // A3
        .p14(E71_2),                 // A4
        .p15(GND),                   // CE_N
        .p16(P5V)                    // VCC
    );

    // E81 7437, zone C3
    ttl_7437 E81 (
        .p1(E82_1),                  // A1
        .p2(E82_1),                  // B1
        .p3(ALU_S3_H),               // Y1
        .p4(E82_4),                  // A2
        .p5(E82_4),                  // B2
        .p6(ALU_S1_H),               // Y2
        .p7(GND),                    // GND
        .p8(ALU_S0_H),               // Y3
        .p9(E82_5),                  // A3
        .p10(E82_5),                 // B3
        .p11(ALU_S2_H),              // Y4
        .p12(E82_2),                 // A4
        .p13(E82_2),                 // B4
        .p14(P5V)                    // VCC
    );

    // E88 74S174, zone B5
    ttl_74s174 E88 (
        .p1(PROC_INIT_L),            // CLR_N
        .p2(E88_2),                  // Q1
        .p3(SS_01_H),                // D1
        .p4(E97_13),                 // D2
        .p5(E88_5),                  // Q2
        .p6(E97_11),                 // D3
        .p7(E88_7),                  // Q3
        .p8(GND),                    // GND
        .p9(PROC_CLK_L),             // CLK
        .p10(E88_10),                // Q4
        .p11(E97_12),                // D4
        .p12(E88_12),                // Q5
        .p13(E97_14),                // D5
        .p14(SS_00_H),               // D6
        .p15(E88_15),                // Q6
        .p16(P5V)                    // VCC
    );

    // E87 82S123, DEC 23-109A1, zone B4
    prom_82s123 E87 (
        .p1(E87_1),                  // O1
        .p2(E87_2),                  // O2
        .p3(BXMODE_01_L),            // O3
        .p4(BXMODE_00_L),            // O4
        .p5(SHIFT_MUX_01_L),         // O5
        .p6(SHIFT_MUX_00_L),         // O6
        .p7(ENAB_OVX_L),             // O7
        .p8(GND),                    // GND
        .p9(ENAB_DBE_L),             // O8
        .p10(GND),                   // A0
        .p11(E88_7),                 // A1
        .p12(E88_5),                 // A2
        .p13(E88_12),                // A3
        .p14(E88_10),                // A4
        .p15(GND),                   // CE_N
        .p16(P5V)                    // VCC
    );

    // R49 470 ohm, zone B2
    resistor R49 (
        .p1(P5V),                    // END1
        .p2(BMODE_01_L)              // END2
    );

    // R50 470 ohm, zone B2
    resistor R50 (
        .p1(P5V),                    // END1
        .p2(BMODE_00_L)              // END2
    );

    // E86 7400, zone A4, K2-10 B6, K2-10 A6
    ttl_7400 E86 (
        .p1(COUNT_05_1_H),           // A1, K2-10
        .p2(E95_4),                  // B1, K2-10
        .p3(E86_3),                  // Y1, K2-10
        .p4(E95_4),                  // A2, K2-10
        .p5(E95_1),                  // B2, K2-10
        .p6(E86_6),                  // Y2, K2-10
        .p7(GND),                    // GND
        .p8(E86_8),                  // Y3, K2-10
        .p9(PROC_CLK_H),             // A3, K2-10
        .p10(E95_10),                // B3, K2-10
        .p11(E86_11),                // Y4
        .p12(VBA_00_1_H),            // A4
        .p13(BUF_DAT_TRAN_1_H),      // B4
        .p14(P5V)                    // VCC
    );

    // E80 82S129, DEC 23-163A2, zone A3
    prom_82s129 E80 (
        .p1(GND),                    // A6
        .p2(E88_2),                  // A5
        .p3(BUF_DAT_TRAN_1_H),       // A4
        .p4(AUX_CONTROL_1_H),        // A3
        .p5(BYTE_H),                 // A0
        .p6(E112_10),                // A1
        .p7(MOV_L),                  // A2
        .p8(GND),                    // GND
        .p9(SWAP_H),                 // O4
        .p10(SEX_H),                 // O3
        .p11(DISAB_MSYN_L),          // O2
        .p12(DISABLE_UPPER_BYTE_WRITE_H), // O1
        .p13(GND),                   // CE1_N
        .p14(GND),                   // CE2_N
        .p15(E88_15),                // A7
        .p16(P5V)                    // VCC
    );

// K2-9 MICRO CODE, PDF page 48

    // E98 82S137, DEC 23-023F1, zone D7
    prom_82s137 E98 (
        .p1(MPC_00_H),               // A6
        .p2(MPC_01_H),               // A5
        .p3(MPC_02_H),               // A4
        .p4(MPC_03_H),               // A3
        .p5(MPC_06_H),               // A0
        .p6(MPC_04_H),               // A1
        .p7(MPC_07_H),               // A2
        .p8(GND),                    // CE1_N
        .p9(GND),                    // GND
        .p10(GND),                   // CE2_N
        .p11(E98_11),                // O4
        .p12(E98_12),                // O3
        .p13(SS_00_H),               // O2
        .p14(SS_01_H),               // O1
        .p15(MPC_09_H),              // A9
        .p16(MPC_05_H),              // A8
        .p17(MPC_08_H),              // A7
        .p18(P5V)                    // VCC
    );

    // E100 82S137, DEC 23-024F1, zone C7
    prom_82s137 E100 (
        .p1(MPC_00_H),               // A6
        .p2(MPC_01_H),               // A5
        .p3(MPC_02_H),               // A4
        .p4(MPC_03_H),               // A3
        .p5(MPC_06_H),               // A0
        .p6(MPC_04_H),               // A1
        .p7(MPC_07_H),               // A2
        .p8(GND),                    // CE1_N
        .p9(GND),                    // GND
        .p10(GND),                   // CE2_N
        .p11(E100_11),               // O4
        .p12(E100_12),               // O3
        .p13(E100_13),               // O2
        .p14(E100_14),               // O1
        .p15(MPC_09_H),              // A9
        .p16(MPC_05_H),              // A8
        .p17(MPC_08_H),              // A7
        .p18(P5V)                    // VCC
    );

    // E104 82S137, DEC 23-025F1, zone B7
    prom_82s137 E104 (
        .p1(MPC_00_H),               // A6
        .p2(MPC_01_H),               // A5
        .p3(MPC_02_H),               // A4
        .p4(MPC_03_H),               // A3
        .p5(MPC_06_H),               // A0
        .p6(MPC_04_H),               // A1
        .p7(MPC_07_H),               // A2
        .p8(GND),                    // CE1_N
        .p9(GND),                    // GND
        .p10(GND),                   // CE2_N
        .p11(E104_11),               // O4
        .p12(E104_12),               // O3
        .p13(E104_13),               // O2
        .p14(E104_14),               // O1
        .p15(MPC_09_H),              // A9
        .p16(MPC_05_H),              // A8
        .p17(MPC_08_H),              // A7
        .p18(P5V)                    // VCC
    );

    // E103 82S137, DEC 23-011F1, zone B7
    prom_82s137 E103 (
        .p1(MPC_00_H),               // A6
        .p2(MPC_01_H),               // A5
        .p3(MPC_02_H),               // A4
        .p4(MPC_03_H),               // A3
        .p5(MPC_06_H),               // A0
        .p6(MPC_04_H),               // A1
        .p7(MPC_07_H),               // A2
        .p8(GND),                    // CE1_N
        .p9(GND),                    // GND
        .p10(GND),                   // CE2_N
        .p11(E103_11),               // O4
        .p12(E103_12),               // O3
        .p13(E103_13),               // O2
        .p14(E103_14),               // O1
        .p15(MPC_09_H),              // A9
        .p16(MPC_05_H),              // A8
        .p17(MPC_08_H),              // A7
        .p18(P5V)                    // VCC
    );

    // E93 74S174, zone D5
    ttl_74s174 E93 (
        .p1(PROC_INIT_L),            // CLR_N
        .p2(AMUX_S0_1_L),            // Q1
        .p3(E98_11),                 // D1
        .p4(E100_14),                // D2
        .p5(E93_5),                  // Q2
        .p6(E100_12),                // D3
        .p7(E93_7),                  // Q3
        .p8(GND),                    // GND
        .p9(PROC_CLK_L),             // CLK
        .p10(E93_10),                // Q4
        .p11(E100_11),               // D4
        .p12(E93_12),                // Q5
        .p13(E100_13),               // D5
        .p14(E98_12),                // D6
        .p15(AMUX_S1_1_L),           // Q6
        .p16(P5V)                    // VCC
    );

    // E49 74S74, zone C5
    // Second half drawn E?9. The parts list gives E99 as a microcode PROM.
    ttl_74s74 E49 (
        .p1(PROC_INIT_L),            // CLR1_N
        .p2(E104_11),                // D1
        .p3(PROC_CLK_L),             // CLK1
        .p4(P3VA),                   // PRE1_N
        .p5(SPA_SRC_SEL_1_1_H),      // Q1
        .p6(),                       // Q1_N
        .p7(GND),                    // GND
        .p8(),                       // Q2_N
        .p9(SPA_SRC_SEL_0_1_H),      // Q2
        .p10(P3VA),                  // PRE2_N
        .p11(PROC_CLK_L),            // CLK2
        .p12(E104_12),               // D2
        .p13(PROC_INIT_L),           // CLR2_N
        .p14(P5V)                    // VCC
    );

    // E38 74S174, zone B5
    ttl_74s174 E38 (
        .p1(PROC_INIT_L),            // CLR_N
        .p2(SPA_DST_SEL_0_1_H),      // Q1
        .p3(E104_14),                // D1
        .p4(E103_12),                // D2
        .p5(PREVIOUS_MODE_1_L),      // Q2
        .p6(E103_13),                // D3
        .p7(BUT_SERVICE_1_H),        // Q3
        .p8(GND),                    // GND
        .p9(PROC_CLK_L),             // CLK
        .p10(FORCE_KERNEL_1_H),      // Q4
        .p11(E103_14),               // D4
        .p12(SPA_DST_SEL_1_1_H),     // Q5
        .p13(E104_13),               // D5
        .p14(E103_11),               // D6
        .p15(FORCE_RS_PLUS_1_1_L),   // Q6
        .p16(P5V)                    // VCC
    );

    // E102 82S123, DEC 23-177A1, zone D4
    prom_82s123 E102 (
        .p1(E102_1),                 // O1
        .p2(E102_2),                 // O2
        .p3(E102_3),                 // O3
        .p4(E102_4),                 // O4
        .p5(E102_5),                 // O5
        .p6(E102_6),                 // O6
        .p7(E102_7),                 // O7
        .p8(GND),                    // GND
        .p9(E102_9),                 // O8
        .p10(P3VA),                  // A0
        .p11(E93_10),                // A1
        .p12(E93_12),                // A2
        .p13(E93_5),                 // A3
        .p14(E93_7),                 // A4
        .p15(GND),                   // CE_N
        .p16(P5V)                    // VCC
    );

    // E94 74S03, zone D2
    ttl_74s03 E94 (
        .p1(E102_1),                 // A1
        .p2(ALU_COUT_H),             // B1
        .p3(MPC_07_L),               // Y1
        .p4(E102_4),                 // A2
        .p5(BXREG_01_1_H),           // B2
        .p6(MPC_05_L),               // Y2
        .p7(GND),                    // GND
        .p8(MPC_04_L),               // Y3
        .p9(BXREG_00_1_H),           // A3
        .p10(E102_5),                // B3
        .p11(MPC_06_L),              // Y4
        .p12(CC_N_H),                // A4
        .p13(E102_2),                // B4
        .p14(P5V)                    // VCC
    );

    // E76 74S03, zone C2
    ttl_74s03 E76 (
        .p1(E102_3),                 // A1
        .p2(SP_15_1_H),              // B1
        .p3(MPC_03_L),               // Y1
        .p4(E102_6),                 // A2
        .p5(COUNT_05_1_H),           // B2
        .p6(MPC_02_L),               // Y2
        .p7(GND),                    // GND
        .p8(MPC_01_L),               // Y3
        .p9(E102_7),                 // A3
        .p10(CC_Z_H),                // B3
        .p11(MPC_00_L),              // Y4
        .p12(E102_9),                // A4
        .p13(IR_09_1_H),             // B4
        .p14(P5V)                    // VCC
    );

// K2-10 ROM SPA COUNTER, PDF page 49

    // E79 74193, zone C6
    ttl_74193 E79 (
        .p1(SSMUX_05_H),             // B
        .p2(COUNT_05_1_H),           // QB
        .p3(),                       // QA
        .p4(E78_13),                 // DOWN
        .p5(E78_12),                 // UP
        .p6(),                       // QC
        .p7(),                       // QD
        .p8(GND),                    // GND
        .p9(GND),                    // D
        .p10(GND),                   // C
        .p11(E86_8),                 // LOAD_N
        .p12(),                      // CARRY_N
        .p13(),                      // BORROW_N
        .p14(PROC_INIT_H),           // CLEAR
        .p15(SSMUX_04_H),            // A
        .p16(P5V)                    // VCC
    );

    // E78 74193, zone B6
    ttl_74193 E78 (
        .p1(SSMUX_01_H),             // B
        .p2(),                       // QB
        .p3(),                       // QA
        .p4(E86_6),                  // DOWN
        .p5(E86_3),                  // UP
        .p6(),                       // QC
        .p7(),                       // QD
        .p8(GND),                    // GND
        .p9(SSMUX_03_H),             // D
        .p10(SSMUX_02_H),            // C
        .p11(E86_8),                 // LOAD_N
        .p12(E78_12),                // CARRY_N
        .p13(E78_13),                // BORROW_N
        .p14(PROC_INIT_H),           // CLEAR
        .p15(SSMUX_00_H),            // A
        .p16(P5V)                    // VCC
    );

    // E105 82S137, DEC 23-027F1, zone C4
    prom_82s137 E105 (
        .p1(MPC_00_H),               // A6
        .p2(MPC_01_H),               // A5
        .p3(MPC_02_H),               // A4
        .p4(MPC_03_H),               // A3
        .p5(MPC_06_H),               // A0
        .p6(MPC_04_H),               // A1
        .p7(MPC_07_H),               // A2
        .p8(GND),                    // CE1_N
        .p9(GND),                    // GND
        .p10(GND),                   // CE2_N
        .p11(E105_11),               // O4
        .p12(E105_12),               // O3
        .p13(E105_13),               // O2
        .p14(E105_14),               // O1
        .p15(MPC_09_H),              // A9
        .p16(MPC_05_H),              // A8
        .p17(MPC_08_H),              // A7
        .p18(P5V)                    // VCC
    );

    // E48 74S175, zone C2
    ttl_74s175 E48 (
        .p1(PROC_INIT_L),            // CLR_N
        .p2(ROM_SPA_03_1_L),         // Q1
        .p3(),                       // Q1_N
        .p4(E105_11),                // D1
        .p5(E105_12),                // D2
        .p6(ROM_SPA_02_1_H),         // Q2_N
        .p7(),                       // Q2
        .p8(GND),                    // GND
        .p9(PROC_CLK_L),             // CLK
        .p10(),                      // Q3
        .p11(ROM_SPA_01_1_H),        // Q3_N
        .p12(E105_13),               // D3
        .p13(E105_14),               // D4
        .p14(ROM_SPA_00_1_H),        // Q4_N
        .p15(ROM_SPA_00_1_L),        // Q4
        .p16(P5V)                    // VCC
    );

    // C115 2200 pF, zone B3
    capacitor C115 (
        .p1(E14_6),                  // END1
        .p2(E14_7)                   // END2
    );

    // R5 30 kohm, zone B3
    resistor R5 (
        .p1(P5V),                    // END1
        .p2(E14_7)                   // END2
    );

    // R62 220 ohm, zone A3
    resistor R62 (
        .p1(E35_3),                  // END1
        .p2(E8_8)                    // END2
    );

    // C116 470 pF, zone A3
    capacitor C116 (
        .p1(E8_8),                   // END1
        .p2(GND)                     // END2
    );

endmodule
`default_nettype wire
