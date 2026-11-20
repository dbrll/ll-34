`default_nettype none
// Verilog transcription of the M8265 KD11-EA data path board
// From DEC 1134A FIELD MAINTENANCE PRINT SET, sheets K1-1 to K1-10
// http://bitsavers.informatik.uni-stuttgart.de/pdf/dec/pdp11/1134/MP00190_1134A_KD11-EA.pdf

module kd11_ea_m8265 (
    (* edge_contact = "EF1" *) inout wire SP_WRITE_L,
    (* edge_contact = "DD2" *) inout wire SPA_03_H,
    (* edge_contact = "DL1" *) inout wire SPA_02_H,
    (* edge_contact = "DB2" *) inout wire SPA_01_H,
    (* edge_contact = "DF2" *) inout wire SPA_00_H,
    (* edge_contact = "DE1" *) inout wire BMODE_00_L,
    (* edge_contact = "CU2" *) inout wire BMODE_01_L,
    (* edge_contact = "DM1" *) inout wire BREG_00_1_H,
    (* edge_contact = "ER2" *) inout wire BXMODE_00_L,
    (* edge_contact = "ES2" *) inout wire BXMODE_01_L,
    (* edge_contact = "DF1" *) inout wire BXREG_01_1_H,
    (* edge_contact = "CU1" *) inout wire BXREG_00_1_H,
    (* edge_contact = "CS1" *) inout wire C3_H,
    (* edge_contact = "CL1" *) inout wire C2_H,
    (* edge_contact = "CP2" *) inout wire SWAP_H,
    (* edge_contact = "AE2" *) inout wire BUS_D03_L,
    (* edge_contact = "AD1" *) inout wire BUS_D02_L,
    (* edge_contact = "AD2" *) inout wire BUS_D01_L,
    (* edge_contact = "AC1" *) inout wire BUS_D00_L,
    (* edge_contact = "EP2" *) inout wire N_BIT_1_H,
    (* edge_contact = "EV1" *) inout wire Z_BIT_1_H,
    (* edge_contact = "EU2" *) inout wire V_BIT_1_H,
    (* edge_contact = "DV1" *) inout wire C_BIT_1_H,
    (* edge_contact = "FD2", connector_contact = "J1-EE" *) inout wire PROC_INIT_L,
    (* edge_contact = "FF2" *) inout wire LOAD_CC_L,
    (* edge_contact = "EN2" *) inout wire LOAD_PSW_L,
    (* edge_contact = "EF2" *) inout wire ALU_S3_H,
    (* edge_contact = "EH1" *) inout wire ALU_S2_H,
    (* edge_contact = "DD1" *) inout wire ALU_S1_H,
    (* edge_contact = "CV2" *) inout wire ALU_S0_H,
    (* edge_contact = "CK2" *) inout wire ALU_MODE_H,
    (* edge_contact = "CR2" *) inout wire ALU_CIN_L,
    (* edge_contact = "DN1" *) inout wire BLEG_01_H,
    (* edge_contact = "CV1" *) inout wire BLEG_00_H,
    (* edge_contact = "CS2" *) inout wire DISABLE_MSYN_PLUS_1_L,
    (* edge_contact = "ER1" *) inout wire AUX_CONTROL_1_L,
    (* connector_contact = "J1-C" *) inout wire AMUX_11_H,
    (* connector_contact = "J1-A" *) inout wire AMUX_10_H,
    (* connector_contact = "J1-W" *) inout wire AMUX_09_H,
    (* connector_contact = "J1-H" *) inout wire AMUX_08_H,
    (* connector_contact = "J1-TT" *) inout wire AMUX_03_H,
    (* connector_contact = "J1-K" *) inout wire AMUX_02_H,
    (* connector_contact = "J1-P" *) inout wire AMUX_01_H,
    (* connector_contact = "J1-E" *) inout wire AMUX_00_H,
    (* connector_contact = "J1-S" *) inout wire TRI_STATE_AMUX_L,
    (* edge_contact = "EK2" *) inout wire PSW_07_1_H,
    (* edge_contact = "EU1" *) inout wire PSW_06_1_H,
    (* edge_contact = "ET2" *) inout wire PSW_05_1_H,
    (* edge_contact = "DK1" *) inout wire TBIT_1_H,
    (* edge_contact = "FS2" *) inout wire DISABLE_LOAD_PSW_H,
    (* edge_contact = "CE2" *) inout wire SSMUX_07_H,
    (* edge_contact = "CC1" *) inout wire SSMUX_06_H,
    (* edge_contact = "EH2" *) inout wire SSMUX_05_H,
    (* edge_contact = "CD1" *) inout wire SSMUX_04_H,
    (* edge_contact = "ED1" *) inout wire SSMUX_03_H,
    (* edge_contact = "ED2" *) inout wire SSMUX_02_H,
    (* edge_contact = "EE1" *) inout wire SSMUX_01_H,
    (* edge_contact = "EB2" *) inout wire SSMUX_00_H,
    (* edge_contact = "AH2" *) inout wire BUS_D07_L,
    (* edge_contact = "AF1" *) inout wire BUS_D06_L,
    (* edge_contact = "AF2" *) inout wire BUS_D05_L,
    (* edge_contact = "AE1" *) inout wire BUS_D04_L,
    (* edge_contact = "CM2" *) inout wire KTE_1_H,
    (* edge_contact = "CT2" *) inout wire PE_1_H,
    (* edge_contact = "CN2" *) inout wire C4_H,
    (* connector_contact = "J1-KK" *) inout wire AMUX_15_H,
    (* connector_contact = "J1-CC" *) inout wire AMUX_14_H,
    (* connector_contact = "J1-JJ" *) inout wire AMUX_13_H,
    (* connector_contact = "J1-NN" *) inout wire AMUX_12_H,
    (* connector_contact = "J1-LL" *) inout wire AMUX_07_H,
    (* connector_contact = "J1-AA" *) inout wire AMUX_06_H,
    (* connector_contact = "J1-U" *) inout wire AMUX_05_H,
    (* connector_contact = "J1-Y" *) inout wire AMUX_04_H,
    (* edge_contact = "FR1" *) inout wire DISABLE_UPPER_BYTE_WRITE_H,
    (* edge_contact = "CF2" *) inout wire SSMUX_11_H,
    (* edge_contact = "CH2" *) inout wire SSMUX_10_H,
    (* edge_contact = "CJ2" *) inout wire SSMUX_09_H,
    (* edge_contact = "EN1" *) inout wire SSMUX_08_H,
    (* edge_contact = "AK2" *) inout wire BUS_D11_L,
    (* edge_contact = "AJ1" *) inout wire BUS_D10_L,
    (* edge_contact = "AJ2" *) inout wire BUS_D09_L,
    (* edge_contact = "AH1" *) inout wire BUS_D08_L,
    (* edge_contact = "CL2" *) inout wire SEX_H,
    (* edge_contact = "CN1" *) inout wire SP_15_1_H,
    (* edge_contact = "FL2" *) inout wire ALU_COUT_H,
    (* edge_contact = "AM2" *) inout wire BUS_D15_L,
    (* edge_contact = "AL1" *) inout wire BUS_D14_L,
    (* edge_contact = "AL2" *) inout wire BUS_D13_L,
    (* edge_contact = "AK1" *) inout wire BUS_D12_L,
    (* edge_contact = "CB2" *) inout wire SSMUX_15_H,
    (* edge_contact = "CH1" *) inout wire SSMUX_14_H,
    (* edge_contact = "CD2" *) inout wire SSMUX_13_H,
    (* edge_contact = "CJ1" *) inout wire SSMUX_12_H,
    (* edge_contact = "EE2" *) inout wire PSW_15_1_L,
    (* edge_contact = "EK1" *) inout wire FORCE_KERNEL_1_H,
    (* connector_contact = "J1-B" *) inout wire BA_07_1_H,
    (* connector_contact = "J1-F" *) inout wire BA_10_1_H,
    (* connector_contact = "J1-L" *) inout wire BA_06_1_H,
    (* edge_contact = "EA1", connector_contact = "J1-M" *) inout wire START_TRAN_L,
    (* connector_contact = "J1-T" *) inout wire BA_09_1_H,
    (* connector_contact = "J1-V" *) inout wire VBA_01_1_H,
    (* connector_contact = "J1-Z" *) inout wire BA_08_1_H,
    (* connector_contact = "J1-DD" *) inout wire VBA_02_1_H,
    (* connector_contact = "J1-FF" *) inout wire VBA_04_1_H,
    (* connector_contact = "J1-MM" *) inout wire VBA_03_1_H,
    (* connector_contact = "J1-RR" *) inout wire K15_1134_CLK_L,
    (* connector_contact = "J1-UU" *) inout wire VBA_05_1_H,
    (* connector_contact = "J1-D,J1-J,J1-N,J1-R,J1-X,J1-BB,J1-HH,J1-PP,J1-SS,J1-VV" *) inout wire GND,
    (* edge_contact = "ES1" *) inout wire MAN_CLK_L,
    (* edge_contact = "FE1" *) inout wire MAN_CLK_ENAB_L,
    (* edge_contact = "FC1" *) inout wire TRAN_INH_L,
    (* edge_contact = "FB1" *) inout wire BG_INH_L,
    (* edge_contact = "FD1" *) inout wire LOAD_BA_1_H,
    (* edge_contact = "EV2" *) inout wire LONG_CYCLE_1_L,
    (* edge_contact = "FA1" *) inout wire ALLOW_MSYN_H,
    (* edge_contact = "EJ2" *) inout wire PROC_CLK_H,
    (* edge_contact = "EM1" *) inout wire PROC_CLK_L,
    (* edge_contact = "EM2" *) inout wire EXT_TAP_30_H,
    (* edge_contact = "BS1" *) inout wire BUS_A17_L,
    (* edge_contact = "BS2" *) inout wire BUS_A16_L,
    (* edge_contact = "BR1" *) inout wire BUS_A15_L,
    (* edge_contact = "BR2" *) inout wire BUS_A14_L,
    (* edge_contact = "BP1" *) inout wire BUS_A13_L,
    (* edge_contact = "BP2" *) inout wire BUS_A12_L,
    (* edge_contact = "BN1" *) inout wire BUS_A11_L,
    (* edge_contact = "BN2" *) inout wire BUS_A10_L,
    (* edge_contact = "BM1" *) inout wire BUS_A09_L,
    (* edge_contact = "BM2" *) inout wire BUS_A08_L,
    (* edge_contact = "BL1" *) inout wire BUS_A07_L,
    (* edge_contact = "BL2" *) inout wire BUS_A06_L,
    (* edge_contact = "BK1" *) inout wire BUS_A05_L,
    (* edge_contact = "BK2" *) inout wire BUS_A04_L,
    (* edge_contact = "BJ1" *) inout wire BUS_A03_L,
    (* edge_contact = "BJ2" *) inout wire BUS_A02_L,
    (* edge_contact = "BH1" *) inout wire BUS_A01_L,
    (* edge_contact = "BH2" *) inout wire BUS_A00_L,
    (* edge_contact = "BT2" *) inout wire BUS_C1_L,
    (* edge_contact = "BU2" *) inout wire BUS_C0_L,
    (* edge_contact = "DV2" *) inout wire PBA_01_H,
    (* edge_contact = "EC1" *) inout wire PBA_00_H,
    (* edge_contact = "EB1" *) inout wire PBA_03_H,
    (* edge_contact = "DU2" *) inout wire PBA_02_H,
    (* edge_contact = "DJ1" *) inout wire UBUS_C1_1_H,
    (* edge_contact = "DH2" *) inout wire UBUS_C0_1_H,
    (* edge_contact = "DE2" *) inout wire ENAB_ADDRS_L,
    (* edge_contact = "DH1" *) inout wire VBA_00_1_H,
    (* edge_contact = "FK2" *) inout wire MSYN_1_L,
    (* edge_contact = "FS1" *) inout wire DISABLE_WBIT_L,
    (* edge_contact = "FU2" *) inout wire ENAB_MAINT_1_H,
    (* edge_contact = "FE2" *) inout wire RCD_INIT_L,
    (* edge_contact = "FT2" *) inout wire KT_FAULT_L,
    (* edge_contact = "DJ2" *) inout wire PREVIOUS_MODE_1_L,
    (* edge_contact = "FV2" *) inout wire CLK_MSYN_H,
    // Labeled K1-5 LOAD IR (1) H on K1-9 and K2-5 LOAD IR H on K1-10.
    (* edge_contact = "FP2" *) inout wire LOAD_IR_H,
    (* edge_contact = "EJ1" *) inout wire ENAB_OVX_L,
    (* edge_contact = "FN2" *) inout wire K110_8_15_EQ_0_L,
    (* edge_contact = "FB2" *) inout wire CC_Z_H,
    (* edge_contact = "FJ2" *) inout wire CC_N_H,
    (* edge_contact = "FL1" *) inout wire BYTE_L,
    (* edge_contact = "FN1" *) inout wire SERIAL_SHIFT_H,
    (* edge_contact = "FK1" *) inout wire ROT_CBIT_1_H,
    (* edge_contact = "FM2" *) inout wire CC_CODE_02_H,
    (* edge_contact = "FH2" *) inout wire CC_CODE_01_H,
    (* edge_contact = "FJ1" *) inout wire CC_CODE_00_H,
    (* edge_contact = "FP1" *) inout wire SHIFT_MUX_01_L,
    (* edge_contact = "FM1" *) inout wire SHIFT_MUX_00_L,
    (* edge_contact = "DC1" *) inout wire INT_MSYN_H,
    (* edge_contact = "EL1" *) inout wire SACK_RET_1_H,
    (* edge_contact = "FH1" *) inout wire INTR_H,
    (* edge_contact = "FR2" *) inout wire AMUX_S1_1_L,
    (* edge_contact = "FV1" *) inout wire AMUX_S0_1_L,
    (* edge_contact = "FU1" *) inout wire INT_VECTOR_L,
    (* edge_contact = "FF1" *) inout wire ASSERT_SSYN_H
);

    wire ACF1_1_H;
    wire ACF2_1_H;
    wire ALU_07_H;
    wire ALU_15_H;
    wire ALU_COUT_11_L;
    wire ALU_COUT_3_L;
    wire ALU_COUT_7_L;
    wire AMUX_S0_H;
    wire AMUX_S1_H;
    wire ASSERT_SSYN_L;
    wire BLEG_07_H;
    wire BLEG_15_H;
    wire BREG_03_1_H;
    wire BREG_04_1_H;
    wire BREG_07_1_H;
    wire BREG_08_1_H;
    wire BREG_11_1_H;
    wire BREG_12_1_H;
    wire BREG_14_1_H;
    wire BREG_15_1_H;
    wire BXREG_03_1_H;
    wire BXREG_04_1_H;
    wire BXREG_07_1_H;
    wire BXREG_08_1_H;
    wire BXREG_11_1_H;
    wire BXREG_12_1_H;
    wire BXREG_15_1_H;
    wire C0_H;
    wire C1_H;
    wire CC_C_H;
    wire CC_V_H;
    wire CLK_START;
    wire E1_10;
    wire E1_13;
    wire E1_3;
    wire E1_6;
    wire E10_13;
    wire E10_14;
    wire E100_8;
    wire E101_1;
    wire E101_10;
    wire E102_1;
    wire E102_10;
    wire E102_13;
    wire E102_4;
    wire E103_11;
    wire E103_14;
    wire E103_6;
    wire E104_105;
    wire E104_120;
    wire E104_150;
    wire E104_75;
    wire E104_90;
    wire E105_10;
    wire E105_2;
    wire E105_5;
    wire E105_9;
    wire E106_6;
    wire E106_8;
    wire E108_12;
    wire E108_9;
    wire E109_9;
    wire E11_14;
    wire E110_11;
    wire E110_3;
    wire E111_6;
    wire E111_8;
    wire E112_11;
    wire E112_3;
    wire E112_6;
    wire E112_8;
    wire E113_11;
    wire E113_3;
    wire E113_8;
    wire E114_10;
    wire E114_13;
    wire E115_11;
    wire E115_3;
    wire E115_6;
    wire E116_3;
    wire E116_6;
    wire E117_4;
    wire E117_6;
    wire E118_11;
    wire E118_6;
    wire E12_10;
    wire E12_13;
    wire E12_3;
    wire E12_6;
    wire E120_2;
    wire E120_6;
    wire E120_8;
    wire E121_6;
    wire E122_3;
    wire E122_8;
    wire E123_12;
    wire E18_F0;
    wire E18_F1;
    wire E18_F2;
    wire E18_F3;
    wire E18_G;
    wire E18_P;
    wire E20_12;
    wire E20_4;
    wire E20_7;
    wire E20_9;
    wire E21_13;
    wire E21_14;
    wire E22_13;
    wire E22_14;
    wire E23_10;
    wire E23_13;
    wire E23_3;
    wire E23_6;
    wire E28_F0;
    wire E28_F1;
    wire E28_F2;
    wire E28_G;
    wire E28_P;
    wire E30_12;
    wire E30_4;
    wire E30_7;
    wire E31_13;
    wire E31_14;
    wire E32_13;
    wire E32_14;
    wire E33_10;
    wire E33_13;
    wire E33_3;
    wire E33_6;
    wire E38_COUT4;
    wire E38_F0;
    wire E38_F1;
    wire E38_F2;
    wire E40_12;
    wire E40_4;
    wire E40_7;
    wire E41_13;
    wire E44_12;
    wire E44_13;
    wire E44_14;
    wire E44_15;
    wire E45_S1;
    wire E45_S2;
    wire E45_S3;
    wire E45_S4;
    wire E51_2;
    wire E51_5;
    wire E52_13;
    wire E52_14;
    wire E55_13;
    wire E55_14;
    wire E55_15;
    wire E56_14;
    wire E56_S1;
    wire E56_S2;
    wire E56_S3;
    wire E56_S4;
    wire E6_7;
    wire E6_9;
    wire E61_5;
    wire E61_6;
    wire E61_7;
    wire E62_10;
    wire E62_12;
    wire E62_15;
    wire E62_2;
    wire E62_5;
    wire E62_7;
    wire E66_14;
    wire E66_S1;
    wire E66_S2;
    wire E66_S3;
    wire E66_S4;
    wire E7_F0;
    wire E7_F1;
    wire E7_F2;
    wire E7_F3;
    wire E7_G;
    wire E7_P;
    wire E72_5;
    wire E81_11;
    wire E83_10;
    wire E83_11;
    wire E83_12;
    wire E85_3;
    wire E85_8;
    wire E9_7;
    wire E9_9;
    wire E91_12;
    wire E91_4;
    wire E91_7;
    wire E91_9;
    wire E92_12;
    wire E92_9;
    wire E93_12;
    wire E93_4;
    wire E93_7;
    wire E93_9;
    wire E94_12;
    wire E94_6;
    wire E94_8;
    wire E95_3;
    wire E95_6;
    wire E96_12;
    wire E96_7;
    wire E98_12;
    wire E98_4;
    wire E98_7;
    wire E98_9;
    wire ED_1_H;
    wire ENAB_DATA_L;
    wire ENAB_GR_L;
    wire ENAB_KT_MUX_L;
    wire ENAB_W_L;
    wire ERROR_H;
    wire GEN_REG_L;
    wire INT_DATI_H;
    wire INT_DATI_L;
    wire INT_SSYN_L;
    wire K11_0_3_EQ_0_H;
    wire K11_TRI_STATE_AMUX_H;
    wire K110_772XXX_L;
    wire K12_4_7_EQ_0_H;
    wire K13_8_11_EQ_0_H;
    wire K13_TRI_STATE_AMUX_H;
    wire K14_12_15_EQ_0_H;
    wire KT_MUX_S0_L;
    wire LOAD_BAR_L;
    wire LOAD_HPSW_H;
    wire LOAD_PAR_HIGH_L;
    wire LOAD_PAR_LOW_L;
    wire LOAD_PDR_HIGH_L;
    wire LOAD_PDR_LOW_L;
    wire LOAD_PSW_12_15_L;
    wire LOAD_PSW_HIGH_L;
    wire LOAD_PSW_LOW_L;
    wire LOAD_SR0_HIGH_L;
    wire LOAD_SR0_LOW_L;
    wire LOAD_SR2_H;
    wire LOAD_VBA_H;
    wire LOAD_WBIT_L;
    wire MAINT_1_H;
    wire MODE_00_H;
    wire MODE_01_H;
    wire NR_1_H;
    wire OUTPUT_STORE_L;
    wire OVX_1_H;
    wire P3VA;
    wire P3VB;
    wire P3VC;
    wire P3VD;
    wire P5V;
    wire PAR_00_1_H;
    wire PAR_01_1_H;
    wire PAR_02_1_H;
    wire PAR_03_1_H;
    wire PAR_04_1_H;
    wire PAR_05_1_H;
    wire PAR_06_1_H;
    wire PAR_07_1_H;
    wire PAR_08_1_H;
    wire PAR_09_1_H;
    wire PAR_10_1_H;
    wire PAR_11_1_H;
    wire PAR_PLUS_PDR_L;
    wire PBA_04_H;
    wire PBA_05_H;
    wire PBA_06_H;
    wire PBA_07_H;
    wire PBA_08_H;
    wire PBA_09_H;
    wire PBA_10_H;
    wire PBA_11_H;
    wire PBA_12_H;
    wire PBA_13_H;
    wire PBA_14_H;
    wire PBA_15_H;
    wire PBA_16_H;
    wire PBA_17_H;
    wire PDR_08_1_H;
    wire PDR_09_1_H;
    wire PDR_10_1_H;
    wire PDR_11_1_H;
    wire PDR_12_1_H;
    wire PDR_13_1_H;
    wire PDR_14_1_H;
    wire PL_1_H;
    wire PSW_12_1_H;
    wire PSW_13_1_H;
    wire PSW_14_1_H;
    wire PSW_15_1_H;
    wire PSW_DATI_L;
    wire REG_CLK_H;
    wire REG_CLK_L;
    wire RELOCATE_ENB_1_H;
    wire RELOCATE_H;
    wire RO_1_H;
    wire SHIFT_IN_07_H;
    wire SHIFT_IN_B_H;
    wire SHIFT_IN_BX_H;
    wire SP_00_1_H;
    wire SP_01_1_H;
    wire SP_02_1_H;
    wire SP_03_1_H;
    wire SP_04_1_H;
    wire SP_05_1_H;
    wire SP_06_1_H;
    wire SP_07_1_H;
    wire SP_08_1_H;
    wire SP_09_1_H;
    wire SP_10_1_H;
    wire SP_11_1_H;
    wire SP_12_1_H;
    wire SP_13_1_H;
    wire SP_14_1_H;
    wire SR0_01_1_H;
    wire SR0_02_1_H;
    wire SR0_03_1_H;
    wire SR0_05_1_H;
    wire SR0_06_1_H;
    wire SR2_00_1_H;
    wire SR2_01_1_H;
    wire SR2_02_1_H;
    wire SR2_03_1_H;
    wire SR2_04_1_H;
    wire SR2_05_1_H;
    wire SR2_06_1_H;
    wire SR2_07_1_H;
    wire START_TRAN_H;
    wire TAP_120_H;
    wire TAP_30_H;
    wire TAP_30_L;
    wire TAP_30_RAW;
    wire TAP_90_H;
    wire VBA_06_1_H;
    wire VBA_07_1_H;
    wire VBA_08_1_H;
    wire VBA_09_1_H;
    wire VBA_10_1_H;
    wire VBA_11_1_H;
    wire VBA_12_1_H;
    wire VBA_13_1_H;
    wire VBA_14_1_H;
    wire VBA_15_1_H;
    wire WBIT_H;
    wire WRITE_HIGH_L;
    wire XX556X_L;

// K1-1 DATA PATH 0-3, PDF page 20

    // E8 85S68, zone D7
    // The drawing labels pins 3 to 6 A3 to A0, the 85S68 has A0 on pin 3.
    dm85s68 E8 (
        .p1(SSMUX_01_H),             // D2
        .p2(SSMUX_00_H),             // D1
        .p3(SPA_03_H),               // A0
        .p4(SPA_02_H),               // A2
        .p5(SPA_01_H),               // A3
        .p6(SPA_00_H),               // A1
        .p7(SP_00_1_H),              // O1
        .p8(SP_01_1_H),              // O2
        .p9(GND),                    // GND
        .p10(SP_02_1_H),             // O3
        .p11(SP_03_1_H),             // O4
        .p12(ENAB_GR_L),             // OD
        .p13(OUTPUT_STORE_L),        // OS_N
        .p14(REG_CLK_H),             // CLK
        .p15(SP_WRITE_L),            // WE_N
        .p16(SSMUX_03_H),            // D4
        .p17(SSMUX_02_H),            // D3
        .p18(P5V)                    // VCC
    );

    // E95 74S00, zone D7, K1-5 C3
    ttl_74s00 E95 (
        .p1(RELOCATE_H),             // A1, K1-5
        .p2(LOAD_BA_1_H),            // B1, K1-5
        .p3(E95_3),                  // Y1, K1-5
        .p4(E95_3),                  // A2, K1-5
        .p5(LONG_CYCLE_1_L),         // B2, K1-5
        .p6(E95_6),                  // Y2, K1-5
        .p7(GND),                    // GND
        .p8(OUTPUT_STORE_L),         // Y3
        .p9(TAP_30_H),               // A3
        .p10(GEN_REG_L),             // B3
        .p11(LOAD_BAR_L),            // Y4, K1-5
        .p12(ALLOW_MSYN_H),          // A4, K1-5
        .p13(LOAD_BA_1_H),           // B4, K1-5
        .p14(P5V)                    // VCC
    );

    // E10 74194, zone C7
    ttl_74194 E10 (
        .p1(P3VC),                   // CLR_N
        .p2(BREG_04_1_H),            // SR_SER
        .p3(SSMUX_03_H),             // A
        .p4(SSMUX_02_H),             // B
        .p5(SSMUX_01_H),             // C
        .p6(SSMUX_00_H),             // D
        .p7(SHIFT_IN_B_H),           // SL_SER
        .p8(GND),                    // GND
        .p9(BMODE_00_L),             // S0
        .p10(BMODE_01_L),            // S1
        .p11(PROC_CLK_L),            // CLK
        .p12(BREG_00_1_H),           // QD
        .p13(E10_13),                // QC
        .p14(E10_14),                // QB
        .p15(BREG_03_1_H),           // QA
        .p16(P5V)                    // VCC
    );

    // E11 74194, zone B7
    ttl_74194 E11 (
        .p1(P3VC),                   // CLR_N
        .p2(BXREG_04_1_H),           // SR_SER
        .p3(SSMUX_03_H),             // A
        .p4(SSMUX_02_H),             // B
        .p5(SSMUX_01_H),             // C
        .p6(SSMUX_00_H),             // D
        .p7(SHIFT_IN_BX_H),          // SL_SER
        .p8(GND),                    // GND
        .p9(BXMODE_00_L),            // S0
        .p10(BXMODE_01_L),           // S1
        .p11(PROC_CLK_L),            // CLK
        .p12(BXREG_00_1_H),          // QD
        .p13(BXREG_01_1_H),          // QC
        .p14(E11_14),                // QB
        .p15(BXREG_03_1_H),          // QA
        .p16(P5V)                    // VCC
    );

    // E9 74153, zone C6
    ttl_74153 E9 (
        .p1(GND),                    // 1G_N
        .p2(BLEG_01_H),              // B
        .p3(GND),                    // 1C3
        .p4(AUX_CONTROL_1_L),        // 1C2
        .p5(E11_14),                 // 1C1
        .p6(E10_14),                 // 1C0
        .p7(E9_7),                   // 1Y
        .p8(GND),                    // GND
        .p9(E9_9),                   // 2Y
        .p10(BREG_03_1_H),           // 2C0
        .p11(BXREG_03_1_H),          // 2C1
        .p12(AUX_CONTROL_1_L),       // 2C2
        .p13(GND),                   // 2C3
        .p14(BLEG_00_H),             // A
        .p15(GND),                   // 2G_N
        .p16(P5V)                    // VCC
    );

    // E6 74153, zone B6
    // The ground run on pins 13 and 4 is broken before pin 3, which goes to
    // DISABLE MSYN +1 L.
    ttl_74153 E6 (
        .p1(GND),                    // 1G_N
        .p2(BLEG_01_H),              // B
        .p3(DISABLE_MSYN_PLUS_1_L),  // 1C3
        .p4(GND),                    // 1C2
        .p5(BXREG_00_1_H),           // 1C1
        .p6(BREG_00_1_H),            // 1C0
        .p7(E6_7),                   // 1Y
        .p8(GND),                    // GND
        .p9(E6_9),                   // 2Y
        .p10(E10_13),                // 2C0
        .p11(BXREG_01_1_H),          // 2C1
        .p12(AUX_CONTROL_1_L),       // 2C2
        .p13(GND),                   // 2C3
        .p14(BLEG_00_H),             // A
        .p15(GND),                   // 2G_N
        .p16(P5V)                    // VCC
    );

    // E7 74S181, zone C4
    ttl_74s181 E7 (
        .p1(E6_7),                   // B0_N
        .p2(SP_00_1_H),              // A0_N
        .p3(ALU_S3_H),               // S3
        .p4(ALU_S2_H),               // S2
        .p5(ALU_S1_H),               // S1
        .p6(ALU_S0_H),               // S0
        .p7(ALU_CIN_L),              // CN
        .p8(ALU_MODE_H),             // M
        .p9(E7_F0),                  // F0_N
        .p10(E7_F1),                 // F1_N
        .p11(E7_F2),                 // F2_N
        .p12(GND),                   // GND
        .p13(E7_F3),                 // F3_N
        .p14(),                      // A_EQ_B
        .p15(E7_P),                  // P_N
        .p16(),                      // CN4
        .p17(E7_G),                  // G_N
        .p18(E9_9),                  // B3_N
        .p19(SP_03_1_H),             // A3_N
        .p20(E9_7),                  // B2_N
        .p21(SP_02_1_H),             // A2_N
        .p22(E6_9),                  // B1_N
        .p23(SP_01_1_H),             // A1_N
        .p24(P5V)                    // VCC
    );

    // E27 74S182, zone D4, K1-2 D4, K1-3 D4
    ttl_74s182 E27 (
        .p1(E28_G),                  // G1_N, K1-2
        .p2(E28_P),                  // P1_N, K1-2
        .p3(E7_G),                   // G0_N
        .p4(E7_P),                   // P0_N
        .p5(),                       // G3_N
        .p6(),                       // P3_N
        .p7(),                       // P_N
        .p8(GND),                    // GND
        .p9(ALU_COUT_11_L),          // CNZ, K1-3
        .p10(),                      // G_N
        .p11(ALU_COUT_7_L),          // CNY, K1-2
        .p12(ALU_COUT_3_L),          // CNX
        .p13(ALU_CIN_L),             // CN
        .p14(E18_G),                 // G2_N, K1-3
        .p15(E18_P),                 // P2_N, K1-3
        .p16(P5V)                    // VCC
    );

    // E17 8815, zone C3, K1-3 B4
    signetics_8815 E17 (
        .p1(E7_F3),                  // A1
        .p2(E7_F2),                  // B1
        .p3(),                       // NC
        .p4(E7_F1),                  // C1
        .p5(E7_F0),                  // D1
        .p6(K11_0_3_EQ_0_H),         // Y1
        .p7(GND),                    // GND
        .p8(K13_8_11_EQ_0_H),        // Y2, K1-3
        .p9(E18_F0),                 // A2, K1-3
        .p10(E18_F1),                // B2, K1-3
        .p11(),                      // NC
        .p12(E18_F2),                // C2, K1-3
        .p13(E18_F3),                // D2, K1-3
        .p14(P5V)                    // VCC
    );

    // E2 74S157, zone D3
    ttl_74s157 E2 (
        .p1(SWAP_H),                 // S
        .p2(AMUX_03_H),              // A1
        .p3(AMUX_11_H),              // B1
        .p4(SSMUX_03_H),             // Y1
        .p5(AMUX_01_H),              // A2
        .p6(AMUX_09_H),              // B2
        .p7(SSMUX_01_H),             // Y2
        .p8(GND),                    // GND
        .p9(SSMUX_00_H),             // Y3
        .p10(AMUX_08_H),             // B3
        .p11(AMUX_00_H),             // A3
        .p12(SSMUX_02_H),            // Y4
        .p13(AMUX_10_H),             // B4
        .p14(AMUX_02_H),             // A4
        .p15(GND),                   // G_N
        .p16(P5V)                    // VCC
    );

    // E1 8641, zone D2
    // Pin 7 is labeled TAP 30 L without sheet reference.
    ds8641 E1 (
        .p1(BUS_D03_L),              // BUS3
        .p2(SSMUX_03_H),             // IN3
        .p3(E1_3),                   // OUT3
        .p4(BUS_D01_L),              // BUS4
        .p5(SSMUX_01_H),             // IN4
        .p6(E1_6),                   // OUT4
        .p7(TAP_30_L),               // DISABLE_B
        .p8(GND),                    // GND
        .p9(ENAB_DATA_L),            // DISABLE_A
        .p10(E1_10),                 // OUT2
        .p11(SSMUX_00_H),            // IN2
        .p12(BUS_D00_L),             // BUS2
        .p13(E1_13),                 // OUT1
        .p14(SSMUX_02_H),            // IN1
        .p15(BUS_D02_L),             // BUS1
        .p16(P5V)                    // VCC
    );

    // E3 74S253, zone C2
    ttl_74s253 E3 (
        .p1(K11_TRI_STATE_AMUX_H),   // 1G_N
        .p2(AMUX_S1_H),              // B
        .p3(N_BIT_1_H),              // 1C3
        .p4(E7_F3),                  // 1C2
        .p5(C3_H),                   // 1C1
        .p6(E1_3),                   // 1C0
        .p7(AMUX_03_H),              // 1Y
        .p8(GND),                    // GND
        .p9(AMUX_02_H),              // 2Y
        .p10(E1_13),                 // 2C0
        .p11(C2_H),                  // 2C1
        .p12(E7_F2),                 // 2C2
        .p13(Z_BIT_1_H),             // 2C3
        .p14(AMUX_S0_H),             // A
        .p15(K11_TRI_STATE_AMUX_H),  // 2G_N
        .p16(P5V)                    // VCC
    );

    // E4 74S253, zone B2
    ttl_74s253 E4 (
        .p1(K11_TRI_STATE_AMUX_H),   // 1G_N
        .p2(AMUX_S1_H),              // B
        .p3(V_BIT_1_H),              // 1C3
        .p4(E7_F1),                  // 1C2
        .p5(GND),                    // 1C1
        .p6(E1_6),                   // 1C0
        .p7(AMUX_01_H),              // 1Y
        .p8(GND),                    // GND
        .p9(AMUX_00_H),              // 2Y
        .p10(E1_10),                 // 2C0
        .p11(GND),                   // 2C1
        .p12(E7_F0),                 // 2C2
        .p13(C_BIT_1_H),             // 2C3
        .p14(AMUX_S0_H),             // A
        .p15(K11_TRI_STATE_AMUX_H),  // 2G_N
        .p16(P5V)                    // VCC
    );

    // E98 74S157, zone B4
    // Reference partly illegible, read E98.
    ttl_74s157 E98 (
        .p1(E122_3),                 // S
        .p2(CC_N_H),                 // A1
        .p3(SSMUX_03_H),             // B1
        .p4(E98_4),                  // Y1
        .p5(CC_Z_H),                 // A2
        .p6(SSMUX_02_H),             // B2
        .p7(E98_7),                  // Y2
        .p8(GND),                    // GND
        .p9(E98_9),                  // Y3
        .p10(SSMUX_00_H),            // B3
        .p11(CC_C_H),                // A3
        .p12(E98_12),                // Y4
        .p13(SSMUX_01_H),            // B4
        .p14(CC_V_H),                // A4
        .p15(GND),                   // G_N
        .p16(P5V)                    // VCC
    );

    // E97 74S175, zone B3
    ttl_74s175 E97 (
        .p1(PROC_INIT_L),            // CLR_N
        .p2(N_BIT_1_H),              // Q1
        .p3(),                       // Q1_N
        .p4(E98_4),                  // D1
        .p5(E98_7),                  // D2
        .p6(),                       // Q2_N
        .p7(Z_BIT_1_H),              // Q2
        .p8(GND),                    // GND
        .p9(E102_1),                 // CLK
        .p10(V_BIT_1_H),             // Q3
        .p11(),                      // Q3_N
        .p12(E98_12),                // D3
        .p13(E98_9),                 // D4
        .p14(),                      // Q4_N
        .p15(C_BIT_1_H),             // Q4
        .p16(P5V)                    // VCC
    );

    // E122 7432, zone A5, K1-2 A5, K1-3 D7, K1-8 C2
    ttl_7432 E122 (
        .p1(AUX_CONTROL_1_L),        // 1A
        .p2(E115_11),                // 1B
        .p3(E122_3),                 // 1Y
        .p4(DISABLE_LOAD_PSW_H),     // 2A, K1-2
        .p5(LOAD_PSW_L),             // 2B, K1-2
        .p6(LOAD_PSW_12_15_L),       // 2Y, K1-2
        .p7(GND),                    // GND
        .p8(E122_8),                 // 3Y, K1-8
        .p9(ERROR_H),                // 3A, K1-8
        .p10(KT_FAULT_L),            // 3B, K1-8
        .p11(WRITE_HIGH_L),          // 4Y, K1-3
        .p12(SP_WRITE_L),            // 4A, K1-3
        .p13(DISABLE_UPPER_BYTE_WRITE_H), // 4B, K1-3
        .p14(P5V)                    // VCC
    );

    // E115 7486, zone A6, K1-8 C7, K1-10 D6
    ttl_7486 E115 (
        .p1(MODE_01_H),              // 1A, K1-8
        .p2(MODE_00_H),              // 1B, K1-8
        .p3(E115_3),                 // 1Y, K1-8
        .p4(BREG_15_1_H),            // 2A, K1-10
        .p5(BREG_14_1_H),            // 2B, K1-10
        .p6(E115_6),                 // 2Y, K1-10
        .p7(GND),                    // GND
        .p8(),                       // 3Y
        .p9(),                       // 3A
        .p10(),                      // 3B
        .p11(E115_11),               // 4Y
        .p12(LOAD_PSW_LOW_L),        // 4A
        .p13(P3VA),                  // 4B
        .p14(P5V)                    // VCC
    );

    // E112 7408, zone A5, K1-2 A4, K1-3 A5
    ttl_7408 E112 (
        .p1(LOAD_PSW_L),             // A1
        .p2(LOAD_PSW_LOW_L),         // B1
        .p3(E112_3),                 // Y1
        .p4(E112_3),                 // A2
        .p5(LOAD_CC_L),              // B2
        .p6(E112_6),                 // Y2
        .p7(GND),                    // GND
        .p8(E112_8),                 // Y3, K1-3
        .p9(LOAD_PSW_HIGH_L),        // A3, K1-3
        .p10(LOAD_PSW_12_15_L),      // B3, K1-3
        .p11(E112_11),               // Y4, K1-2
        .p12(LOAD_PSW_12_15_L),      // A4, K1-2
        .p13(LOAD_PSW_LOW_L),        // B4, K1-2
        .p14(P5V)                    // VCC
    );

    // E102 7402, zone A4, K1-2 A4, K1-8 D4
    // Encircled by hand on K1-8, no wiring change visible.
    ttl_7402 E102 (
        .p1(E102_1),                 // Y1
        .p2(E112_6),                 // A1
        .p3(REG_CLK_L),              // B1
        .p4(E102_4),                 // Y2, K1-8
        .p5(ERROR_H),                // A2, K1-8
        .p6(E113_8),                 // B2, K1-8
        .p7(GND),                    // GND
        .p8(E112_11),                // A3, K1-2
        .p9(REG_CLK_L),              // B3, K1-2
        .p10(E102_10),               // Y3, K1-2
        .p11(LOAD_SR0_LOW_L),        // A4, K1-8
        .p12(REG_CLK_L),             // B4, K1-8
        .p13(E102_13),               // Y4, K1-8
        .p14(P5V)                    // VCC
    );

    // E16 74S04, zone A3, K1-3 C4, K1-5 C3
    ttl_74s04 E16 (
        .p1(TRI_STATE_AMUX_L),       // A1, K1-3
        .p2(K13_TRI_STATE_AMUX_H),   // Y1, K1-3
        .p3(TRI_STATE_AMUX_L),       // A2
        .p4(K11_TRI_STATE_AMUX_H),   // Y2
        .p5(TAP_30_H),               // A3, K1-5
        .p6(TAP_30_L),               // Y3, K1-5
        .p7(GND),                    // GND
        .p8(),                       // Y4
        .p9(),                       // A4
        .p10(),                      // Y5
        .p11(),                      // A5
        .p12(),                      // Y6
        .p13(),                      // A6
        .p14(P5V)                    // VCC
    );

    // R31 390 ohm, zone A3
    resistor R31 (
        .p1(P5V),                    // END1
        .p2(TRI_STATE_AMUX_L)        // END2
    );

// K1-2 DATA PATH 4-7, PDF page 21

    // E29 85S68, zone D7
    dm85s68 E29 (
        .p1(SSMUX_05_H),             // D2
        .p2(SSMUX_04_H),             // D1
        .p3(SPA_03_H),               // A0
        .p4(SPA_02_H),               // A2
        .p5(SPA_01_H),               // A3
        .p6(SPA_00_H),               // A1
        .p7(SP_04_1_H),              // O1
        .p8(SP_05_1_H),              // O2
        .p9(GND),                    // GND
        .p10(SP_06_1_H),             // O3
        .p11(SP_07_1_H),             // O4
        .p12(ENAB_GR_L),             // OD
        .p13(OUTPUT_STORE_L),        // OS_N
        .p14(REG_CLK_H),             // CLK
        .p15(SP_WRITE_L),            // WE_N
        .p16(SSMUX_07_H),            // D4
        .p17(SSMUX_06_H),            // D3
        .p18(P5V)                    // VCC
    );

    // E31 74194, zone C7
    ttl_74194 E31 (
        .p1(P3VC),                   // CLR_N
        .p2(SHIFT_IN_07_H),          // SR_SER
        .p3(SSMUX_07_H),             // A
        .p4(SSMUX_06_H),             // B
        .p5(SSMUX_05_H),             // C
        .p6(SSMUX_04_H),             // D
        .p7(BREG_03_1_H),            // SL_SER
        .p8(GND),                    // GND
        .p9(BMODE_00_L),             // S0
        .p10(BMODE_01_L),            // S1
        .p11(PROC_CLK_L),            // CLK
        .p12(BREG_04_1_H),           // QD
        .p13(E31_13),                // QC
        .p14(E31_14),                // QB
        .p15(BREG_07_1_H),           // QA
        .p16(P5V)                    // VCC
    );

    // E32 74194, zone B7
    ttl_74194 E32 (
        .p1(P3VC),                   // CLR_N
        .p2(BXREG_08_1_H),           // SR_SER
        .p3(SSMUX_07_H),             // A
        .p4(SSMUX_06_H),             // B
        .p5(SSMUX_05_H),             // C
        .p6(SSMUX_04_H),             // D
        .p7(BXREG_03_1_H),           // SL_SER
        .p8(GND),                    // GND
        .p9(BXMODE_00_L),            // S0
        .p10(BXMODE_01_L),           // S1
        .p11(PROC_CLK_L),            // CLK
        .p12(BXREG_04_1_H),          // QD
        .p13(E32_13),                // QC
        .p14(E32_14),                // QB
        .p15(BXREG_07_1_H),          // QA
        .p16(P5V)                    // VCC
    );

    // E30 74157, zone B6
    ttl_74157 E30 (
        .p1(BLEG_00_H),              // SELECT
        .p2(BREG_04_1_H),            // 1A
        .p3(BXREG_04_1_H),           // 1B
        .p4(E30_4),                  // 1Y
        .p5(E31_14),                 // 2A
        .p6(E32_14),                 // 2B
        .p7(E30_7),                  // 2Y
        .p8(GND),                    // GND
        .p9(BLEG_07_H),              // 3Y
        .p10(BXREG_07_1_H),          // 3B
        .p11(BREG_07_1_H),           // 3A
        .p12(E30_12),                // 4Y
        .p13(E32_13),                // 4B
        .p14(E31_13),                // 4A
        .p15(BLEG_01_H),             // G_N
        .p16(P5V)                    // VCC
    );

    // E28 74S181, zone C4
    ttl_74s181 E28 (
        .p1(E30_4),                  // B0_N
        .p2(SP_04_1_H),              // A0_N
        .p3(ALU_S3_H),               // S3
        .p4(ALU_S2_H),               // S2
        .p5(ALU_S1_H),               // S1
        .p6(ALU_S0_H),               // S0
        .p7(ALU_COUT_3_L),           // CN
        .p8(ALU_MODE_H),             // M
        .p9(E28_F0),                 // F0_N
        .p10(E28_F1),                // F1_N
        .p11(E28_F2),                // F2_N
        .p12(GND),                   // GND
        .p13(ALU_07_H),              // F3_N
        .p14(),                      // A_EQ_B
        .p15(E28_P),                 // P_N
        .p16(),                      // CN4
        .p17(E28_G),                 // G_N
        .p18(BLEG_07_H),             // B3_N
        .p19(SP_07_1_H),             // A3_N
        .p20(E30_7),                 // B2_N
        .p21(SP_06_1_H),             // A2_N
        .p22(E30_12),                // B1_N
        .p23(SP_05_1_H),             // A1_N
        .p24(P5V)                    // VCC
    );

    // E37 8815, zone C3, K1-4 C3
    signetics_8815 E37 (
        .p1(E28_F0),                 // A1
        .p2(E28_F1),                 // B1
        .p3(),                       // NC
        .p4(E28_F2),                 // C1
        .p5(ALU_07_H),               // D1
        .p6(K12_4_7_EQ_0_H),         // Y1
        .p7(GND),                    // GND
        .p8(K14_12_15_EQ_0_H),       // Y2, K1-4
        .p9(E38_F1),                 // A2, K1-4
        .p10(E38_F0),                // B2, K1-4
        .p11(),                      // NC
        .p12(ALU_15_H),              // C2, K1-4
        .p13(E38_F2),                // D2, K1-4
        .p14(P5V)                    // VCC
    );

    // E24 74S157, zone D3
    ttl_74s157 E24 (
        .p1(SWAP_H),                 // S
        .p2(AMUX_07_H),              // A1
        .p3(AMUX_15_H),              // B1
        .p4(SSMUX_07_H),             // Y1
        .p5(AMUX_05_H),              // A2
        .p6(AMUX_13_H),              // B2
        .p7(SSMUX_05_H),             // Y2
        .p8(GND),                    // GND
        .p9(SSMUX_04_H),             // Y3
        .p10(AMUX_12_H),             // B3
        .p11(AMUX_04_H),             // A3
        .p12(SSMUX_06_H),            // Y4
        .p13(AMUX_14_H),             // B4
        .p14(AMUX_06_H),             // A4
        .p15(GND),                   // G_N
        .p16(P5V)                    // VCC
    );

    // E23 8641, zone D2
    ds8641 E23 (
        .p1(BUS_D07_L),              // BUS3
        .p2(SSMUX_07_H),             // IN3
        .p3(E23_3),                  // OUT3
        .p4(BUS_D05_L),              // BUS4
        .p5(SSMUX_05_H),             // IN4
        .p6(E23_6),                  // OUT4
        .p7(TAP_30_L),               // DISABLE_B
        .p8(GND),                    // GND
        .p9(ENAB_DATA_L),            // DISABLE_A
        .p10(E23_10),                // OUT2
        .p11(SSMUX_04_H),            // IN2
        .p12(BUS_D04_L),             // BUS2
        .p13(E23_13),                // OUT1
        .p14(SSMUX_06_H),            // IN1
        .p15(BUS_D06_L),             // BUS1
        .p16(P5V)                    // VCC
    );

    // E25 74S253, zone C2
    ttl_74s253 E25 (
        .p1(K11_TRI_STATE_AMUX_H),   // 1G_N
        .p2(AMUX_S1_H),              // B
        .p3(PSW_07_1_H),             // 1C3
        .p4(ALU_07_H),               // 1C2
        .p5(KTE_1_H),                // 1C1
        .p6(E23_3),                  // 1C0
        .p7(AMUX_07_H),              // 1Y
        .p8(GND),                    // GND
        .p9(AMUX_06_H),              // 2Y
        .p10(E23_13),                // 2C0
        .p11(PE_1_H),                // 2C1
        .p12(E28_F2),                // 2C2
        .p13(PSW_06_1_H),            // 2C3
        .p14(AMUX_S0_H),             // A
        .p15(K11_TRI_STATE_AMUX_H),  // 2G_N
        .p16(P5V)                    // VCC
    );

    // E26 74S253, zone B2
    ttl_74s253 E26 (
        .p1(K11_TRI_STATE_AMUX_H),   // 1G_N
        .p2(AMUX_S1_H),              // B
        .p3(PSW_05_1_H),             // 1C3
        .p4(E28_F1),                 // 1C2
        .p5(KTE_1_H),                // 1C1
        .p6(E23_6),                  // 1C0
        .p7(AMUX_05_H),              // 1Y
        .p8(GND),                    // GND
        .p9(AMUX_04_H),              // 2Y
        .p10(E23_10),                // 2C0
        .p11(C4_H),                  // 2C1
        .p12(E28_F0),                // 2C2
        .p13(TBIT_1_H),              // 2C3
        .p14(AMUX_S0_H),             // A
        .p15(K11_TRI_STATE_AMUX_H),  // 2G_N
        .p16(P5V)                    // VCC
    );

    // E99 74175, zone B3
    ttl_74175 E99 (
        .p1(PROC_INIT_L),            // CLR_N
        .p2(PSW_07_1_H),             // Q1
        .p3(),                       // Q1_N
        .p4(SSMUX_07_H),             // D1
        .p5(SSMUX_06_H),             // D2
        .p6(),                       // Q2_N
        .p7(PSW_06_1_H),             // Q2
        .p8(GND),                    // GND
        .p9(E102_10),                // CLK
        .p10(PSW_05_1_H),            // Q3
        .p11(),                      // Q3_N
        .p12(SSMUX_05_H),            // D3
        .p13(),                      // D4
        .p14(),                      // Q4_N
        .p15(),                      // Q4
        .p16(P5V)                    // VCC
    );

    // E109 7474, zone A2, K1-10 B6
    // Drawn 74S74 on K1-10, 7474 on K1-2.
    ttl_7474 E109 (
        .p1(PROC_INIT_L),            // CLR1_N
        .p2(SSMUX_04_H),             // D1
        .p3(E101_10),                // CLK1
        .p4(P3VD),                   // PRE1_N
        .p5(TBIT_1_H),               // Q1
        .p6(),                       // Q1_N
        .p7(GND),                    // GND
        .p8(),                       // Q2_N, K1-10
        .p9(E109_9),                 // Q2, K1-10
        .p10(INTR_H),                // PRE2_N, K1-10
        .p11(PROC_CLK_L),            // CLK2, K1-10
        .p12(GND),                   // D2, K1-10
        .p13(P3VD),                  // CLR2_N, K1-10
        .p14(P5V)                    // VCC
    );

    // E101 7402, zone A3, K1-3 A4, K1-9 A7, K1-10 D6
    ttl_7402 E101 (
        .p1(E101_1),                 // Y1, K1-10
        .p2(PROC_CLK_L),             // A1, K1-10
        .p3(ENAB_OVX_L),             // B1, K1-10
        .p4(LOAD_SR2_H),             // Y2, K1-9
        .p5(E117_4),                 // A2, K1-9
        .p6(ERROR_H),                // B2, K1-9
        .p7(GND),                    // GND
        .p8(LOAD_PSW_L),             // A3
        .p9(REG_CLK_L),              // B3
        .p10(E101_10),               // Y3
        .p11(E112_8),                // A4, K1-3
        .p12(REG_CLK_L),             // B4, K1-3
        .p13(LOAD_HPSW_H),           // Y4, K1-3
        .p14(P5V)                    // VCC
    );

// K1-3 DATA PATH 8-11, PDF page 22

    // E19 85S68, zone D7
    dm85s68 E19 (
        .p1(SSMUX_09_H),             // D2
        .p2(SSMUX_08_H),             // D1
        .p3(SPA_03_H),               // A0
        .p4(SPA_02_H),               // A2
        .p5(SPA_01_H),               // A3
        .p6(SPA_00_H),               // A1
        .p7(SP_08_1_H),              // O1
        .p8(SP_09_1_H),              // O2
        .p9(GND),                    // GND
        .p10(SP_10_1_H),             // O3
        .p11(SP_11_1_H),             // O4
        .p12(ENAB_GR_L),             // OD
        .p13(OUTPUT_STORE_L),        // OS_N
        .p14(REG_CLK_H),             // CLK
        .p15(WRITE_HIGH_L),          // WE_N
        .p16(SSMUX_11_H),            // D4
        .p17(SSMUX_10_H),            // D3
        .p18(P5V)                    // VCC
    );

    // E21 74194, zone C7
    ttl_74194 E21 (
        .p1(P3VC),                   // CLR_N
        .p2(BREG_12_1_H),            // SR_SER
        .p3(SSMUX_11_H),             // A
        .p4(SSMUX_10_H),             // B
        .p5(SSMUX_09_H),             // C
        .p6(SSMUX_08_H),             // D
        .p7(BREG_07_1_H),            // SL_SER
        .p8(GND),                    // GND
        .p9(BMODE_00_L),             // S0
        .p10(BMODE_01_L),            // S1
        .p11(PROC_CLK_L),            // CLK
        .p12(BREG_08_1_H),           // QD
        .p13(E21_13),                // QC
        .p14(E21_14),                // QB
        .p15(BREG_11_1_H),           // QA
        .p16(P5V)                    // VCC
    );

    // E22 74194, zone B7
    ttl_74194 E22 (
        .p1(P3VC),                   // CLR_N
        .p2(BXREG_12_1_H),           // SR_SER
        .p3(SSMUX_11_H),             // A
        .p4(SSMUX_10_H),             // B
        .p5(SSMUX_09_H),             // C
        .p6(SSMUX_08_H),             // D
        .p7(BXREG_07_1_H),           // SL_SER
        .p8(GND),                    // GND
        .p9(BXMODE_00_L),            // S0
        .p10(BXMODE_01_L),           // S1
        .p11(PROC_CLK_L),            // CLK
        .p12(BXREG_08_1_H),          // QD
        .p13(E22_13),                // QC
        .p14(E22_14),                // QB
        .p15(BXREG_11_1_H),          // QA
        .p16(P5V)                    // VCC
    );

    // E20 74157, zone B6
    ttl_74157 E20 (
        .p1(BLEG_00_H),              // SELECT
        .p2(BREG_08_1_H),            // 1A
        .p3(BXREG_08_1_H),           // 1B
        .p4(E20_4),                  // 1Y
        .p5(E21_14),                 // 2A
        .p6(E22_14),                 // 2B
        .p7(E20_7),                  // 2Y
        .p8(GND),                    // GND
        .p9(E20_9),                  // 3Y
        .p10(BXREG_11_1_H),          // 3B
        .p11(BREG_11_1_H),           // 3A
        .p12(E20_12),                // 4Y
        .p13(E22_13),                // 4B
        .p14(E21_13),                // 4A
        .p15(BLEG_01_H),             // G_N
        .p16(P5V)                    // VCC
    );

    // E18 74S181, zone C4
    ttl_74s181 E18 (
        .p1(E20_4),                  // B0_N
        .p2(SP_08_1_H),              // A0_N
        .p3(ALU_S3_H),               // S3
        .p4(ALU_S2_H),               // S2
        .p5(ALU_S1_H),               // S1
        .p6(ALU_S0_H),               // S0
        .p7(ALU_COUT_7_L),           // CN
        .p8(ALU_MODE_H),             // M
        .p9(E18_F0),                 // F0_N
        .p10(E18_F1),                // F1_N
        .p11(E18_F2),                // F2_N
        .p12(GND),                   // GND
        .p13(E18_F3),                // F3_N
        .p14(),                      // A_EQ_B
        .p15(E18_P),                 // P_N
        .p16(),                      // CN4
        .p17(E18_G),                 // G_N
        .p18(E20_9),                 // B3_N
        .p19(SP_11_1_H),             // A3_N
        .p20(E20_7),                 // B2_N
        .p21(SP_10_1_H),             // A2_N
        .p22(E20_12),                // B1_N
        .p23(SP_09_1_H),             // A1_N
        .p24(P5V)                    // VCC
    );

    // E12 8641, zone D2
    ds8641 E12 (
        .p1(BUS_D11_L),              // BUS3
        .p2(SSMUX_11_H),             // IN3
        .p3(E12_3),                  // OUT3
        .p4(BUS_D09_L),              // BUS4
        .p5(SSMUX_09_H),             // IN4
        .p6(E12_6),                  // OUT4
        .p7(TAP_30_L),               // DISABLE_B
        .p8(GND),                    // GND
        .p9(ENAB_DATA_L),            // DISABLE_A
        .p10(E12_10),                // OUT2
        .p11(SSMUX_08_H),            // IN2
        .p12(BUS_D08_L),             // BUS2
        .p13(E12_13),                // OUT1
        .p14(SSMUX_10_H),            // IN1
        .p15(BUS_D10_L),             // BUS1
        .p16(P5V)                    // VCC
    );

    // E15 74S253, zone C3
    ttl_74s253 E15 (
        .p1(K13_TRI_STATE_AMUX_H),   // 1G_N
        .p2(AMUX_S1_H),              // B
        .p3(GND),                    // 1C3
        .p4(E18_F3),                 // 1C2
        .p5(GND),                    // 1C1
        .p6(E12_3),                  // 1C0
        .p7(AMUX_11_H),              // 1Y
        .p8(GND),                    // GND
        .p9(AMUX_10_H),              // 2Y
        .p10(E12_13),                // 2C0
        .p11(GND),                   // 2C1
        .p12(E18_F2),                // 2C2
        .p13(GND),                   // 2C3
        .p14(AMUX_S0_H),             // A
        .p15(K13_TRI_STATE_AMUX_H),  // 2G_N
        .p16(P5V)                    // VCC
    );

    // E5 74S253, zone B4
    ttl_74s253 E5 (
        .p1(K13_TRI_STATE_AMUX_H),   // 1G_N
        .p2(AMUX_S1_H),              // B
        .p3(GND),                    // 1C3
        .p4(E18_F1),                 // 1C2
        .p5(GND),                    // 1C1
        .p6(E12_6),                  // 1C0
        .p7(AMUX_09_H),              // 1Y
        .p8(GND),                    // GND
        .p9(AMUX_08_H),              // 2Y
        .p10(E12_10),                // 2C0
        .p11(GND),                   // 2C1
        .p12(E18_F0),                // 2C2
        .p13(GND),                   // 2C3
        .p14(AMUX_S0_H),             // A
        .p15(K13_TRI_STATE_AMUX_H),  // 2G_N
        .p16(P5V)                    // VCC
    );

    // E13 74S153, zone C2
    ttl_74s153 E13 (
        .p1(GND),                    // G1_N
        .p2(SEX_H),                  // B
        .p3(AMUX_15_H),              // C3_1
        .p4(AMUX_07_H),              // C2_1
        .p5(AMUX_03_H),              // C1_1
        .p6(AMUX_11_H),              // C0_1
        .p7(SSMUX_11_H),             // Y1
        .p8(GND),                    // GND
        .p9(SSMUX_10_H),             // Y2
        .p10(AMUX_10_H),             // C0_2
        .p11(AMUX_02_H),             // C1_2
        .p12(AMUX_07_H),             // C2_2
        .p13(AMUX_15_H),             // C3_2
        .p14(SWAP_H),                // A
        .p15(GND),                   // G2_N
        .p16(P5V)                    // VCC
    );

    // E14 74S153, zone B2
    ttl_74s153 E14 (
        .p1(GND),                    // G1_N
        .p2(SEX_H),                  // B
        .p3(AMUX_15_H),              // C3_1
        .p4(AMUX_07_H),              // C2_1
        .p5(AMUX_01_H),              // C1_1
        .p6(AMUX_09_H),              // C0_1
        .p7(SSMUX_09_H),             // Y1
        .p8(GND),                    // GND
        .p9(SSMUX_08_H),             // Y2
        .p10(AMUX_08_H),             // C0_2
        .p11(AMUX_00_H),             // C1_2
        .p12(AMUX_07_H),             // C2_2
        .p13(AMUX_15_H),             // C3_2
        .p14(SWAP_H),                // A
        .p15(GND),                   // G2_N
        .p16(P5V)                    // VCC
    );

// K1-4 DATA PATH 12-15, PDF page 23

    // E39 85S68, zone D7
    dm85s68 E39 (
        .p1(SSMUX_13_H),             // D2
        .p2(SSMUX_12_H),             // D1
        .p3(SPA_03_H),               // A0
        .p4(SPA_02_H),               // A2
        .p5(SPA_01_H),               // A3
        .p6(SPA_00_H),               // A1
        .p7(SP_12_1_H),              // O1
        .p8(SP_13_1_H),              // O2
        .p9(GND),                    // GND
        .p10(SP_14_1_H),             // O3
        .p11(SP_15_1_H),             // O4
        .p12(ENAB_GR_L),             // OD
        .p13(OUTPUT_STORE_L),        // OS_N
        .p14(REG_CLK_H),             // CLK
        .p15(WRITE_HIGH_L),          // WE_N
        .p16(SSMUX_15_H),            // D4
        .p17(SSMUX_14_H),            // D3
        .p18(P5V)                    // VCC
    );

    // E41 74194, zone C7
    // DSR is SHIFT IN B here, while E10 on K1-1 uses DSL.
    ttl_74194 E41 (
        .p1(P3VC),                   // CLR_N
        .p2(SHIFT_IN_B_H),           // SR_SER
        .p3(SSMUX_15_H),             // A
        .p4(SSMUX_14_H),             // B
        .p5(SSMUX_13_H),             // C
        .p6(SSMUX_12_H),             // D
        .p7(BREG_11_1_H),            // SL_SER
        .p8(GND),                    // GND
        .p9(BMODE_00_L),             // S0
        .p10(BMODE_01_L),            // S1
        .p11(PROC_CLK_L),            // CLK
        .p12(BREG_12_1_H),           // QD
        .p13(E41_13),                // QC
        .p14(BREG_14_1_H),           // QB
        .p15(BREG_15_1_H),           // QA
        .p16(P5V)                    // VCC
    );

    // E52 74194, zone B7
    ttl_74194 E52 (
        .p1(P3VC),                   // CLR_N
        .p2(BREG_00_1_H),            // SR_SER
        .p3(SSMUX_15_H),             // A
        .p4(SSMUX_14_H),             // B
        .p5(SSMUX_13_H),             // C
        .p6(SSMUX_12_H),             // D
        .p7(BXREG_11_1_H),           // SL_SER
        .p8(GND),                    // GND
        .p9(BXMODE_00_L),            // S0
        .p10(BXMODE_01_L),           // S1
        .p11(PROC_CLK_L),            // CLK
        .p12(BXREG_12_1_H),          // QD
        .p13(E52_13),                // QC
        .p14(E52_14),                // QB
        .p15(BXREG_15_1_H),          // QA
        .p16(P5V)                    // VCC
    );

    // E40 74157, zone B6
    ttl_74157 E40 (
        .p1(BLEG_00_H),              // SELECT
        .p2(BREG_12_1_H),            // 1A
        .p3(BXREG_12_1_H),           // 1B
        .p4(E40_4),                  // 1Y
        .p5(BREG_14_1_H),            // 2A
        .p6(E52_14),                 // 2B
        .p7(E40_7),                  // 2Y
        .p8(GND),                    // GND
        .p9(BLEG_15_H),              // 3Y
        .p10(BXREG_15_1_H),          // 3B
        .p11(BREG_15_1_H),           // 3A
        .p12(E40_12),                // 4Y
        .p13(E52_13),                // 4B
        .p14(E41_13),                // 4A
        .p15(BLEG_01_H),             // G_N
        .p16(P5V)                    // VCC
    );

    // E38 74S181, zone C4
    ttl_74s181 E38 (
        .p1(E40_4),                  // B0_N
        .p2(SP_12_1_H),              // A0_N
        .p3(ALU_S3_H),               // S3
        .p4(ALU_S2_H),               // S2
        .p5(ALU_S1_H),               // S1
        .p6(ALU_S0_H),               // S0
        .p7(ALU_COUT_11_L),          // CN
        .p8(ALU_MODE_H),             // M
        .p9(E38_F0),                 // F0_N
        .p10(E38_F1),                // F1_N
        .p11(E38_F2),                // F2_N
        .p12(GND),                   // GND
        .p13(ALU_15_H),              // F3_N
        .p14(),                      // A_EQ_B
        .p15(),                      // P_N
        .p16(E38_COUT4),             // CN4
        .p17(),                      // G_N
        .p18(BLEG_15_H),             // B3_N
        .p19(SP_15_1_H),             // A3_N
        .p20(E40_7),                 // B2_N
        .p21(SP_14_1_H),             // A2_N
        .p22(E40_12),                // B1_N
        .p23(SP_13_1_H),             // A1_N
        .p24(P5V)                    // VCC
    );

    // E118 74S00, zone D4, K1-10 D7, K1-10 B4
    // Gate 4 5 6 is drawn E11? on K1-10. E116 is a 7437.
    ttl_74s00 E118 (
        .p1(P3VA),                   // A1
        .p2(E38_COUT4),              // B1
        .p3(ALU_COUT_H),             // Y1
        .p4(K110_8_15_EQ_0_L),       // A2, K1-10
        .p5(BYTE_L),                 // B2, K1-10
        .p6(E118_6),                 // Y2, K1-10
        .p7(GND),                    // GND
        .p8(K110_8_15_EQ_0_L),       // Y3, K1-10
        .p9(K14_12_15_EQ_0_H),       // A3, K1-10
        .p10(K13_8_11_EQ_0_H),       // B3, K1-10
        .p11(E118_11),               // Y4, K1-10
        .p12(E105_10),               // A4, K1-10
        .p13(PBA_07_H),              // B4, K1-10
        .p14(P5V)                    // VCC
    );

    // E33 8641, zone D2
    ds8641 E33 (
        .p1(BUS_D15_L),              // BUS3
        .p2(SSMUX_15_H),             // IN3
        .p3(E33_3),                  // OUT3
        .p4(BUS_D13_L),              // BUS4
        .p5(SSMUX_13_H),             // IN4
        .p6(E33_6),                  // OUT4
        .p7(TAP_30_L),               // DISABLE_B
        .p8(GND),                    // GND
        .p9(ENAB_DATA_L),            // DISABLE_A
        .p10(E33_10),                // OUT2
        .p11(SSMUX_12_H),            // IN2
        .p12(BUS_D12_L),             // BUS2
        .p13(E33_13),                // OUT1
        .p14(SSMUX_14_H),            // IN1
        .p15(BUS_D14_L),             // BUS1
        .p16(P5V)                    // VCC
    );

    // E36 74S253, zone C3
    // A hand drawn change cuts the ground run shared with E46 and grounds
    // pins 5 and 11 locally.
    ttl_74s253 E36 (
        .p1(K13_TRI_STATE_AMUX_H),   // 1G_N
        .p2(AMUX_S1_H),              // B
        .p3(PSW_15_1_H),             // 1C3
        .p4(ALU_15_H),               // 1C2
        .p5(GND),                    // 1C1
        .p6(E33_3),                  // 1C0
        .p7(AMUX_15_H),              // 1Y
        .p8(GND),                    // GND
        .p9(AMUX_14_H),              // 2Y
        .p10(E33_13),                // 2C0
        .p11(GND),                   // 2C1
        .p12(E38_F2),                // 2C2
        .p13(PSW_14_1_H),            // 2C3
        .p14(AMUX_S0_H),             // A
        .p15(K13_TRI_STATE_AMUX_H),  // 2G_N
        .p16(P5V)                    // VCC
    );

    // E46 74S253, zone B3
    ttl_74s253 E46 (
        .p1(K13_TRI_STATE_AMUX_H),   // 1G_N
        .p2(AMUX_S1_H),              // B
        .p3(PSW_13_1_H),             // 1C3
        .p4(E38_F1),                 // 1C2
        .p5(GND),                    // 1C1
        .p6(E33_6),                  // 1C0
        .p7(AMUX_13_H),              // 1Y
        .p8(GND),                    // GND
        .p9(AMUX_12_H),              // 2Y
        .p10(E33_10),                // 2C0
        .p11(GND),                   // 2C1
        .p12(E38_F0),                // 2C2
        .p13(PSW_12_1_H),            // 2C3
        .p14(AMUX_S0_H),             // A
        .p15(K13_TRI_STATE_AMUX_H),  // 2G_N
        .p16(P5V)                    // VCC
    );

    // E34 74S153, zone C2
    ttl_74s153 E34 (
        .p1(GND),                    // G1_N
        .p2(SEX_H),                  // B
        .p3(AMUX_15_H),              // C3_1
        .p4(AMUX_07_H),              // C2_1
        .p5(AMUX_07_H),              // C1_1
        .p6(AMUX_15_H),              // C0_1
        .p7(SSMUX_15_H),             // Y1
        .p8(GND),                    // GND
        .p9(SSMUX_14_H),             // Y2
        .p10(AMUX_14_H),             // C0_2
        .p11(AMUX_06_H),             // C1_2
        .p12(AMUX_07_H),             // C2_2
        .p13(AMUX_15_H),             // C3_2
        .p14(SWAP_H),                // A
        .p15(GND),                   // G2_N
        .p16(P5V)                    // VCC
    );

    // E35 74S153, zone B2
    ttl_74s153 E35 (
        .p1(GND),                    // G1_N
        .p2(SEX_H),                  // B
        .p3(AMUX_15_H),              // C3_1
        .p4(AMUX_07_H),              // C2_1
        .p5(AMUX_05_H),              // C1_1
        .p6(AMUX_13_H),              // C0_1
        .p7(SSMUX_13_H),             // Y1
        .p8(GND),                    // GND
        .p9(SSMUX_12_H),             // Y2
        .p10(AMUX_12_H),             // C0_2
        .p11(AMUX_04_H),             // C1_2
        .p12(AMUX_07_H),             // C2_2
        .p13(AMUX_15_H),             // C3_2
        .p14(SWAP_H),                // A
        .p15(GND),                   // G2_N
        .p16(P5V)                    // VCC
    );

    // E82 74175, zone B4
    ttl_74175 E82 (
        .p1(PROC_INIT_L),            // CLR_N
        .p2(PSW_15_1_H),             // Q1
        .p3(PSW_15_1_L),             // Q1_N
        .p4(SSMUX_15_H),             // D1
        .p5(SSMUX_14_H),             // D2
        .p6(),                       // Q2_N
        .p7(PSW_14_1_H),             // Q2
        .p8(GND),                    // GND
        .p9(LOAD_HPSW_H),            // CLK
        .p10(PSW_13_1_H),            // Q3
        .p11(),                      // Q3_N
        .p12(E92_12),                // D3
        .p13(E92_9),                 // D4
        .p14(),                      // Q4_N
        .p15(PSW_12_1_H),            // Q4
        .p16(P5V)                    // VCC
    );

    // E92 74157, zone B5
    ttl_74157 E92 (
        .p1(FORCE_KERNEL_1_H),       // SELECT
        .p2(),                       // 1A
        .p3(),                       // 1B
        .p4(),                       // 1Y
        .p5(),                       // 2A
        .p6(),                       // 2B
        .p7(),                       // 2Y
        .p8(GND),                    // GND
        .p9(E92_9),                  // 3Y
        .p10(PSW_14_1_H),            // 3B
        .p11(SSMUX_12_H),            // 3A
        .p12(E92_12),                // 4Y
        .p13(PSW_15_1_H),            // 4B
        .p14(SSMUX_13_H),            // 4A
        .p15(GND),                   // G_N
        .p16(P5V)                    // VCC
    );

// K1-5 CPU CLK, PDF page 24

    // R20 1 kohm, zone C7
    resistor R20 (
        .p1(P5V),                    // END1
        .p2(MAN_CLK_L)               // END2
    );

    // R21 1 kohm, zone C7
    resistor R21 (
        .p1(P5V),                    // END1
        .p2(MAN_CLK_ENAB_L)          // END2
    );

    // E94 74S10, zone C7
    ttl_74s10 E94 (
        .p1(E96_12),                 // A1
        .p2(MAN_CLK_ENAB_L),         // B1
        .p3(PROC_INIT_L),            // A2
        .p4(TRAN_INH_L),             // B2
        .p5(BG_INH_L),               // C2
        .p6(E94_6),                  // Y2
        .p7(GND),                    // GND
        .p8(E94_8),                  // Y3
        .p9(ASSERT_SSYN_L),          // A3
        .p10(MAN_CLK_L),             // B3
        .p11(E94_12),                // C3
        .p12(E94_12),                // Y1
        .p13(E85_8),                 // C1
        .p14(P5V)                    // VCC
    );

    // E85 74S37, zone C6
    ttl_74s37 E85 (
        .p1(E94_8),                  // A1
        .p2(TAP_30_H),               // B1
        .p3(E85_3),                  // Y1
        .p4(CLK_START),              // A2
        .p5(TAP_30_H),               // B2
        .p6(K15_1134_CLK_L),         // Y2
        .p7(GND),                    // GND
        .p8(E85_8),                  // Y3
        .p9(E94_12),                 // A3
        .p10(E94_6),                 // B3
        .p11(CLK_START),             // Y4
        .p12(E94_12),                // A4
        .p13(MAN_CLK_L),             // B4
        .p14(P5V)                    // VCC
    );

    // R30 220 ohm, zone C6
    resistor R30 (
        .p1(P5V),                    // END1
        .p2(CLK_START)               // END2
    );

    // E104 DEC 16-12463 delay line, zone C5
    dec_1612463 E104 (
        .p1(CLK_START),              // DELAY_INPUT
        .p2(),                       // TAP_15_NS
        .p3(),                       // TAP_45_NS
        .p4(E104_75),                // TAP_75_NS
        .p5(E104_105),               // TAP_105_NS
        .p6(),                       // TAP_135_NS
        .p7(E104_150),               // TAP_150_NS
        .p8(),                       // NOT_SHOWN
        .p9(E104_120),               // TAP_120_NS
        .p10(E104_90),               // TAP_90_NS
        .p11(),                      // NOT_SHOWN
        .p12(),                      // TAP_60_NS
        .p13(TAP_30_RAW),            // TAP_30_NS
        .p14(GND)                    // GND
    );

    // R25 220 ohm, zone C4
    resistor R25 (
        .p1(E104_150),               // END1
        .p2(GND)                     // END2
    );

    // W2 jumper, zone C5
    // Installed.
    jumper W2 (
        .p1(E104_105),               // END1
        .p2(E105_9)                  // END2
    );

    // W4 jumper, zone C5
    // Not installed, dashed on the drawing.
    jumper W4 (
        .p1(E104_120),               // END1
        .p2(E105_9)                  // END2
    );

    // W1 jumper, zone C5
    // Installed.
    jumper W1 (
        .p1(E104_75),                // END1
        .p2(E105_5)                  // END2
    );

    // W3 jumper, zone C5
    // Not installed, dashed on the drawing.
    jumper W3 (
        .p1(E104_90),                // END1
        .p2(E105_5)                  // END2
    );

    // E105 74S04, zone D5, K1-10 B4
    // TAP 120 H comes from the 105 ns tap through W2, TAP 90 H from the 75 ns
    // tap through W1.
    ttl_74s04 E105 (
        .p1(CLK_START),              // A1
        .p2(E105_2),                 // Y1
        .p3(TAP_30_RAW),             // A2
        .p4(EXT_TAP_30_H),           // Y2
        .p5(E105_5),                 // A3
        .p6(TAP_90_H),               // Y3
        .p7(GND),                    // GND
        .p8(TAP_120_H),              // Y4
        .p9(E105_9),                 // A4
        .p10(E105_10),               // Y5, K1-10
        .p11(PBA_04_H),              // A5, K1-10
        .p12(TAP_30_H),              // Y6
        .p13(TAP_30_RAW),            // A6
        .p14(P5V)                    // VCC
    );

    // E84 74S37, zone B4
    ttl_74s37 E84 (
        .p1(E94_8),                  // A1
        .p2(EXT_TAP_30_H),           // B1
        .p3(REG_CLK_L),              // Y1
        .p4(CLK_START),              // A2
        .p5(EXT_TAP_30_H),           // B2
        .p6(PROC_CLK_L),             // Y2
        .p7(GND),                    // GND
        .p8(PROC_CLK_H),             // Y3
        .p9(K15_1134_CLK_L),         // A3
        .p10(K15_1134_CLK_L),        // B3
        .p11(REG_CLK_H),             // Y4
        .p12(E85_3),                 // A4
        .p13(E85_3),                 // B4
        .p14(P5V)                    // VCC
    );

    // E106 74S08, zone D4, K1-8 B4, K1-10 B5
    // Drawn 7408 on K1-10.
    ttl_74s08 E106 (
        .p1(E96_7),                  // 1A
        .p2(LOAD_BA_1_H),            // 1B
        .p3(LOAD_VBA_H),             // 1Y
        .p4(TAP_30_H),               // 2A
        .p5(TAP_120_H),              // 2B
        .p6(E106_6),                 // 2Y
        .p7(GND),                    // GND
        .p8(E106_8),                 // 3Y, K1-8
        .p9(E120_8),                 // 3A, K1-8
        .p10(REG_CLK_H),             // 3B, K1-8
        .p11(ENAB_DATA_L),           // 4Y, K1-10
        .p12(E114_10),               // 4A, K1-10
        .p13(E120_6),                // 4B, K1-10
        .p14(P5V)                    // VCC
    );

    // E96 74S157, zone D3
    ttl_74s157 E96 (
        .p1(E95_6),                  // S
        .p2(),                       // A1
        .p3(),                       // B1
        .p4(),                       // Y1
        .p5(TAP_30_H),               // A2
        .p6(E105_2),                 // B2
        .p7(E96_7),                  // Y2
        .p8(GND),                    // GND
        .p9(ALLOW_MSYN_H),           // Y3
        .p10(E106_6),                // B3
        .p11(TAP_30_H),              // A3
        .p12(E96_12),                // Y4
        .p13(TAP_120_H),             // B4
        .p14(TAP_90_H),              // A4
        .p15(GND),                   // G_N
        .p16(P5V)                    // VCC
    );

// K1-6 UNIBUS ADDRESS, PDF page 25

    // E121 74S11, zone D7, K1-10 D7
    ttl_74s11 E121 (
        .p1(),                       // 1A
        .p2(),                       // 1B
        .p3(SP_15_1_H),              // 2A
        .p4(SP_14_1_H),              // 2B
        .p5(SP_13_1_H),              // 2C
        .p6(E121_6),                 // 2Y
        .p7(GND),                    // GND
        .p8(CC_Z_H),                 // 3Y, K1-10
        .p9(K11_0_3_EQ_0_H),         // 3A, K1-10
        .p10(K12_4_7_EQ_0_H),        // 3B, K1-10
        .p11(E118_6),                // 3C, K1-10
        .p12(),                      // 1Y
        .p13(),                      // 1C
        .p14(P5V)                    // VCC
    );

    // E49 74S174, zone C7
    ttl_74s174 E49 (
        .p1(PROC_INIT_L),            // CLR_N
        .p2(VBA_12_1_H),             // Q1
        .p3(SP_12_1_H),              // D1
        .p4(SP_10_1_H),              // D2
        .p5(VBA_10_1_H),             // Q2
        .p6(SP_11_1_H),              // D3
        .p7(VBA_11_1_H),             // Q3
        .p8(GND),                    // GND
        .p9(LOAD_VBA_H),             // CLK
        .p10(VBA_13_1_H),            // Q4
        .p11(SP_13_1_H),             // D4
        .p12(VBA_14_1_H),            // Q5
        .p13(SP_14_1_H),             // D5
        .p14(SP_15_1_H),             // D6
        .p15(VBA_15_1_H),            // Q6
        .p16(P5V)                    // VCC
    );

    // E48 74S174, zone B7
    ttl_74s174 E48 (
        .p1(PROC_INIT_L),            // CLR_N
        .p2(VBA_08_1_H),             // Q1
        .p3(SP_08_1_H),              // D1
        .p4(SP_06_1_H),              // D2
        .p5(VBA_06_1_H),             // Q2
        .p6(SP_07_1_H),              // D3
        .p7(VBA_07_1_H),             // Q3
        .p8(GND),                    // GND
        .p9(LOAD_VBA_H),             // CLK
        .p10(VBA_09_1_H),            // Q4
        .p11(SP_09_1_H),             // D4
        .p12(VBA_04_1_H),            // Q5
        .p13(SP_04_1_H),             // D5
        .p14(SP_05_1_H),             // D6
        .p15(VBA_05_1_H),            // Q6
        .p16(P5V)                    // VCC
    );

    // E47 74S174, zone A7
    ttl_74s174 E47 (
        .p1(PROC_INIT_L),            // CLR_N
        .p2(VBA_02_1_H),             // Q1
        .p3(SP_02_1_H),              // D1
        .p4(SP_00_1_H),              // D2
        .p5(VBA_00_1_H),             // Q2
        .p6(SP_01_1_H),              // D3
        .p7(VBA_01_1_H),             // Q3
        .p8(GND),                    // GND
        .p9(LOAD_VBA_H),             // CLK
        .p10(VBA_03_1_H),            // Q4
        .p11(SP_03_1_H),             // D4
        .p12(),                      // Q5
        .p13(),                      // D5
        .p14(),                      // D6
        .p15(),                      // Q6
        .p16(P5V)                    // VCC
    );

    // E45 7483, zone D5
    ttl_7483 E45 (
        .p1(GND),                    // A4
        .p2(E45_S3),                 // SUM3
        .p3(GND),                    // A3
        .p4(PAR_10_1_H),             // B3
        .p5(P5V),                    // VCC
        .p6(E45_S2),                 // SUM2
        .p7(PAR_09_1_H),             // B2
        .p8(GND),                    // A2
        .p9(E45_S1),                 // SUM1
        .p10(GND),                   // A1
        .p11(PAR_08_1_H),            // B1
        .p12(GND),                   // GND
        .p13(E56_14),                // C0
        .p14(),                      // C4
        .p15(E45_S4),                // SUM4
        .p16(PAR_11_1_H)             // B4
    );

    // E56 7483, zone C5
    ttl_7483 E56 (
        .p1(GND),                    // A4
        .p2(E56_S3),                 // SUM3
        .p3(VBA_12_1_H),             // A3
        .p4(PAR_06_1_H),             // B3
        .p5(P5V),                    // VCC
        .p6(E56_S2),                 // SUM2
        .p7(PAR_05_1_H),             // B2
        .p8(VBA_11_1_H),             // A2
        .p9(E56_S1),                 // SUM1
        .p10(VBA_10_1_H),            // A1
        .p11(PAR_04_1_H),            // B1
        .p12(GND),                   // GND
        .p13(E66_14),                // C0
        .p14(E56_14),                // C4
        .p15(E56_S4),                // SUM4
        .p16(PAR_07_1_H)             // B4
    );

    // E66 7483, zone B5
    ttl_7483 E66 (
        .p1(VBA_09_1_H),             // A4
        .p2(E66_S3),                 // SUM3
        .p3(VBA_08_1_H),             // A3
        .p4(PAR_02_1_H),             // B3
        .p5(P5V),                    // VCC
        .p6(E66_S2),                 // SUM2
        .p7(PAR_01_1_H),             // B2
        .p8(VBA_07_1_H),             // A2
        .p9(E66_S1),                 // SUM1
        .p10(VBA_06_1_H),            // A1
        .p11(PAR_00_1_H),            // B1
        .p12(GND),                   // GND
        .p13(GND),                   // C0
        .p14(E66_14),                // C4
        .p15(E66_S4),                // SUM4
        .p16(PAR_03_1_H)             // B4
    );

    // E44 74298, zone D4
    ttl_74298 E44 (
        .p1(E45_S4),                 // B2
        .p2(E45_S3),                 // A2
        .p3(E121_6),                 // A1
        .p4(E121_6),                 // B1
        .p5(E45_S2),                 // C2
        .p6(E45_S1),                 // D2
        .p7(SP_14_1_H),              // D1
        .p8(GND),                    // GND
        .p9(SP_15_1_H),              // C1
        .p10(RELOCATE_H),            // WS
        .p11(LOAD_BAR_L),            // CLK
        .p12(E44_12),                // QD
        .p13(E44_13),                // QC
        .p14(E44_14),                // QB
        .p15(E44_15),                // QA
        .p16(P5V)                    // VCC
    );

    // E55 74298, zone C4
    ttl_74298 E55 (
        .p1(E56_S4),                 // B2
        .p2(E56_S3),                 // A2
        .p3(SP_12_1_H),              // A1
        .p4(SP_13_1_H),              // B1
        .p5(E56_S2),                 // C2
        .p6(E56_S1),                 // D2
        .p7(SP_10_1_H),              // D1
        .p8(GND),                    // GND
        .p9(SP_11_1_H),              // C1
        .p10(RELOCATE_H),            // WS
        .p11(LOAD_BAR_L),            // CLK
        .p12(BA_10_1_H),             // QD
        .p13(E55_13),                // QC
        .p14(E55_14),                // QB
        .p15(E55_15),                // QA
        .p16(P5V)                    // VCC
    );

    // E65 74298, zone B4
    ttl_74298 E65 (
        .p1(E66_S4),                 // B2
        .p2(E66_S3),                 // A2
        .p3(SP_08_1_H),              // A1
        .p4(SP_09_1_H),              // B1
        .p5(E66_S2),                 // C2
        .p6(E66_S1),                 // D2
        .p7(SP_06_1_H),              // D1
        .p8(GND),                    // GND
        .p9(SP_07_1_H),              // C1
        .p10(RELOCATE_H),            // WS
        .p11(LOAD_BAR_L),            // CLK
        .p12(BA_06_1_H),             // QD
        .p13(BA_07_1_H),             // QC
        .p14(BA_09_1_H),             // QB
        .p15(BA_08_1_H),             // QA
        .p16(P5V)                    // VCC
    );

    // E43 8641, zone D2
    ds8641 E43 (
        .p1(BUS_A16_L),              // BUS3
        .p2(E44_15),                 // IN3
        .p3(PBA_16_H),               // OUT3
        .p4(BUS_A15_L),              // BUS4
        .p5(E44_13),                 // IN4
        .p6(PBA_15_H),               // OUT4
        .p7(GND),                    // DISABLE_B
        .p8(GND),                    // GND
        .p9(ENAB_ADDRS_L),           // DISABLE_A
        .p10(PBA_14_H),              // OUT2
        .p11(E44_12),                // IN2
        .p12(BUS_A14_L),             // BUS2
        .p13(PBA_17_H),              // OUT1
        .p14(E44_14),                // IN1
        .p15(BUS_A17_L),             // BUS1
        .p16(P5V)                    // VCC
    );

    // E53 8641, zone C2
    ds8641 E53 (
        .p1(BUS_A12_L),              // BUS3
        .p2(E55_15),                 // IN3
        .p3(PBA_12_H),               // OUT3
        .p4(BUS_A11_L),              // BUS4
        .p5(E55_13),                 // IN4
        .p6(PBA_11_H),               // OUT4
        .p7(GND),                    // DISABLE_B
        .p8(GND),                    // GND
        .p9(ENAB_ADDRS_L),           // DISABLE_A
        .p10(PBA_10_H),              // OUT2
        .p11(BA_10_1_H),             // IN2
        .p12(BUS_A10_L),             // BUS2
        .p13(PBA_13_H),              // OUT1
        .p14(E55_14),                // IN1
        .p15(BUS_A13_L),             // BUS1
        .p16(P5V)                    // VCC
    );

    // E64 8641, zone B2
    ds8641 E64 (
        .p1(BUS_A08_L),              // BUS3
        .p2(BA_08_1_H),              // IN3
        .p3(PBA_08_H),               // OUT3
        .p4(BUS_A07_L),              // BUS4
        .p5(BA_07_1_H),              // IN4
        .p6(PBA_07_H),               // OUT4
        .p7(GND),                    // DISABLE_B
        .p8(GND),                    // GND
        .p9(ENAB_ADDRS_L),           // DISABLE_A
        .p10(PBA_06_H),              // OUT2
        .p11(BA_06_1_H),             // IN2
        .p12(BUS_A06_L),             // BUS2
        .p13(PBA_09_H),              // OUT1
        .p14(BA_09_1_H),             // IN1
        .p15(BUS_A09_L),             // BUS1
        .p16(P5V)                    // VCC
    );

    // E42 8641, zone B2
    ds8641 E42 (
        .p1(BUS_A03_L),              // BUS3
        .p2(VBA_03_1_H),             // IN3
        .p3(PBA_03_H),               // OUT3
        .p4(BUS_A04_L),              // BUS4
        .p5(VBA_04_1_H),             // IN4
        .p6(PBA_04_H),               // OUT4
        .p7(GND),                    // DISABLE_B
        .p8(GND),                    // GND
        .p9(ENAB_ADDRS_L),           // DISABLE_A
        .p10(PBA_05_H),              // OUT2
        .p11(VBA_05_1_H),            // IN2
        .p12(BUS_A05_L),             // BUS2
        .p13(PBA_02_H),              // OUT1
        .p14(VBA_02_1_H),            // IN1
        .p15(BUS_A02_L),             // BUS1
        .p16(P5V)                    // VCC
    );

    // E63 8641, zone A6
    ds8641 E63 (
        .p1(BUS_C1_L),               // BUS3
        .p2(UBUS_C1_1_H),            // IN3
        .p3(C1_H),                   // OUT3
        .p4(BUS_A00_L),              // BUS4
        .p5(VBA_00_1_H),             // IN4
        .p6(PBA_00_H),               // OUT4
        .p7(GND),                    // DISABLE_B
        .p8(GND),                    // GND
        .p9(ENAB_ADDRS_L),           // DISABLE_A
        .p10(PBA_01_H),              // OUT2
        .p11(VBA_01_1_H),            // IN2
        .p12(BUS_A01_L),             // BUS2
        .p13(C0_H),                  // OUT1
        .p14(UBUS_C0_1_H),           // IN1
        .p15(BUS_C0_L),              // BUS1
        .p16(P5V)                    // VCC
    );

// K1-7 PAR + PDR, PDF page 26

    // E80 85S68, zone D6
    dm85s68 E80 (
        .p1(SSMUX_09_H),             // D2
        .p2(SSMUX_08_H),             // D1
        .p3(E91_4),                  // A0
        .p4(E91_12),                 // A2
        .p5(E91_7),                  // A3
        .p6(E91_9),                  // A1
        .p7(PAR_08_1_H),             // O1
        .p8(PAR_09_1_H),             // O2
        .p9(GND),                    // GND
        .p10(PAR_10_1_H),            // O3
        .p11(PAR_11_1_H),            // O4
        .p12(GND),                   // OD
        .p13(P3VB),                  // OS_N
        .p14(REG_CLK_H),             // CLK
        .p15(LOAD_PAR_HIGH_L),       // WE_N
        .p16(SSMUX_11_H),            // D4
        .p17(SSMUX_10_H),            // D3
        .p18(P5V)                    // VCC
    );

    // E79 85S68, zone C6
    dm85s68 E79 (
        .p1(SSMUX_05_H),             // D2
        .p2(SSMUX_04_H),             // D1
        .p3(E91_4),                  // A0
        .p4(E91_12),                 // A2
        .p5(E91_7),                  // A3
        .p6(E91_9),                  // A1
        .p7(PAR_04_1_H),             // O1
        .p8(PAR_05_1_H),             // O2
        .p9(GND),                    // GND
        .p10(PAR_06_1_H),            // O3
        .p11(PAR_07_1_H),            // O4
        .p12(GND),                   // OD
        .p13(P3VB),                  // OS_N
        .p14(REG_CLK_H),             // CLK
        .p15(LOAD_PAR_LOW_L),        // WE_N
        .p16(SSMUX_07_H),            // D4
        .p17(SSMUX_06_H),            // D3
        .p18(P5V)                    // VCC
    );

    // E78 85S68, zone B6
    dm85s68 E78 (
        .p1(SSMUX_01_H),             // D2
        .p2(SSMUX_00_H),             // D1
        .p3(E91_4),                  // A0
        .p4(E91_12),                 // A2
        .p5(E91_7),                  // A3
        .p6(E91_9),                  // A1
        .p7(PAR_00_1_H),             // O1
        .p8(PAR_01_1_H),             // O2
        .p9(GND),                    // GND
        .p10(PAR_02_1_H),            // O3
        .p11(PAR_03_1_H),            // O4
        .p12(GND),                   // OD
        .p13(P3VB),                  // OS_N
        .p14(REG_CLK_H),             // CLK
        .p15(LOAD_PAR_LOW_L),        // WE_N
        .p16(SSMUX_03_H),            // D4
        .p17(SSMUX_02_H),            // D3
        .p18(P5V)                    // VCC
    );

    // E89 85S68, zone D3
    dm85s68 E89 (
        .p1(SSMUX_13_H),             // D2
        .p2(SSMUX_12_H),             // D1
        .p3(E91_4),                  // A0
        .p4(E91_12),                 // A2
        .p5(E91_7),                  // A3
        .p6(E91_9),                  // A1
        .p7(PDR_12_1_H),             // O1
        .p8(PDR_13_1_H),             // O2
        .p9(GND),                    // GND
        .p10(PDR_14_1_H),            // O3
        .p11(),                      // O4
        .p12(GND),                   // OD
        .p13(P3VB),                  // OS_N
        .p14(REG_CLK_H),             // CLK
        .p15(LOAD_PDR_HIGH_L),       // WE_N
        .p16(GND),                   // D4
        .p17(SSMUX_14_H),            // D3
        .p18(P5V)                    // VCC
    );

    // E90 85S68, zone C3
    dm85s68 E90 (
        .p1(SSMUX_09_H),             // D2
        .p2(SSMUX_08_H),             // D1
        .p3(E91_4),                  // A0
        .p4(E91_12),                 // A2
        .p5(E91_7),                  // A3
        .p6(E91_9),                  // A1
        .p7(PDR_08_1_H),             // O1
        .p8(PDR_09_1_H),             // O2
        .p9(GND),                    // GND
        .p10(PDR_10_1_H),            // O3
        .p11(PDR_11_1_H),            // O4
        .p12(GND),                   // OD
        .p13(P3VB),                  // OS_N
        .p14(REG_CLK_H),             // CLK
        .p15(LOAD_PDR_HIGH_L),       // WE_N
        .p16(SSMUX_11_H),            // D4
        .p17(SSMUX_10_H),            // D3
        .p18(P5V)                    // VCC
    );

    // E88 85S68, zone B3
    dm85s68 E88 (
        .p1(SSMUX_01_H),             // D2
        .p2(GND),                    // D1
        .p3(E91_4),                  // A0
        .p4(E91_12),                 // A2
        .p5(E91_7),                  // A3
        .p6(E91_9),                  // A1
        .p7(),                       // O1
        .p8(ACF1_1_H),               // O2
        .p9(GND),                    // GND
        .p10(ACF2_1_H),              // O3
        .p11(ED_1_H),                // O4
        .p12(GND),                   // OD
        .p13(P3VB),                  // OS_N
        .p14(REG_CLK_H),             // CLK
        .p15(LOAD_PDR_LOW_L),        // WE_N
        .p16(SSMUX_03_H),            // D4
        .p17(SSMUX_02_H),            // D3
        .p18(P5V)                    // VCC
    );

    // E81 85S68, zone B4
    dm85s68 E81 (
        .p1(GND),                    // D2
        .p2(GND),                    // D1
        .p3(E91_4),                  // A0
        .p4(E91_12),                 // A2
        .p5(E91_7),                  // A3
        .p6(E91_9),                  // A1
        .p7(),                       // O1
        .p8(),                       // O2
        .p9(GND),                    // GND
        .p10(),                      // O3
        .p11(E81_11),                // O4
        .p12(GND),                   // OD
        .p13(LOAD_WBIT_L),           // OS_N
        .p14(REG_CLK_H),             // CLK
        .p15(LOAD_WBIT_L),           // WE_N
        .p16(ENAB_W_L),              // D4
        .p17(GND),                   // D3
        .p18(P5V)                    // VCC
    );

    // E91 74S157, zone A6
    ttl_74s157 E91 (
        .p1(PAR_PLUS_PDR_L),         // S
        .p2(PBA_08_H),               // A1
        .p3(MODE_01_H),              // B1
        .p4(E91_4),                  // Y1
        .p5(PBA_02_H),               // A2
        .p6(VBA_14_1_H),             // B2
        .p7(E91_7),                  // Y2
        .p8(GND),                    // GND
        .p9(E91_9),                  // Y3
        .p10(VBA_13_1_H),            // B3
        .p11(PBA_01_H),              // A3
        .p12(E91_12),                // Y4
        .p13(VBA_15_1_H),            // B4
        .p14(PBA_03_H),              // A4
        .p15(GND),                   // G_N
        .p16(P5V)                    // VCC
    );

    // E110 7400, zone B5
    ttl_7400 E110 (
        .p1(MSYN_1_L),               // A1
        .p2(PAR_PLUS_PDR_L),         // B1
        .p3(E110_3),                 // Y1
        .p4(E110_3),                 // A2
        .p5(C1_H),                   // B2
        .p6(LOAD_WBIT_L),            // Y2
        .p7(GND),                    // GND
        .p8(ENAB_W_L),               // Y3
        .p9(PAR_PLUS_PDR_L),         // A3
        .p10(E110_11),               // B3
        .p11(E110_11),               // Y4
        .p12(E123_12),               // A4
        .p13(E81_11),                // B4
        .p14(P5V)                    // VCC
    );

    // E117 7404, zone B3, K1-9 A8, K1-10 B5
    ttl_7404 E117 (
        .p1(START_TRAN_L),           // A1, K1-10
        .p2(START_TRAN_H),           // Y1, K1-10
        .p3(LOAD_IR_H),              // A2, K1-9
        .p4(E117_4),                 // Y2, K1-9
        .p5(INT_DATI_L),             // A3, K1-10
        .p6(E117_6),                 // Y3, K1-10
        .p7(GND),                    // GND
        .p8(ENAB_GR_L),              // Y4, K1-10
        .p9(ENAB_KT_MUX_L),          // A4, K1-10
        .p10(WBIT_H),                // Y5
        .p11(E81_11),                // A5
        .p12(),                      // Y6
        .p13(),                      // A6
        .p14(P5V)                    // VCC
    );

    // E123 7410, zone B3, K1-8 C3, K1-10 A7
    ttl_7410 E123 (
        .p1(RELOCATE_H),             // 1A
        .p2(DISABLE_WBIT_L),         // 1B
        .p3(E103_6),                 // 2A, K1-8
        .p4(E103_11),                // 2B, K1-8
        .p5(E103_14),                // 2C, K1-8
        .p6(ERROR_H),                // 2Y, K1-8
        .p7(GND),                    // GND
        .p8(AMUX_S0_H),              // 3Y, K1-10
        .p9(AMUX_S0_1_L),            // 3A, K1-10
        .p10(PSW_DATI_L),            // 3B, K1-10
        .p11(INT_VECTOR_L),          // 3C, K1-10
        .p12(E123_12),               // 1Y
        .p13(KT_MUX_S0_L),           // 1C
        .p14(P5V)                    // VCC
    );

// K1-8 SR0 AND KT ERROR, PDF page 27

    // E72 7485, zone C7
    ttl_7485 E72 (
        .p1(PDR_14_1_H),             // B3
        .p2(E61_7),                  // LT_IN
        .p3(E61_6),                  // EQ_IN
        .p4(E61_5),                  // GT_IN
        .p5(E72_5),                  // GT_OUT
        .p6(),                       // EQ_OUT
        .p7(),                       // LT_OUT
        .p8(GND),                    // GND
        .p9(PDR_11_1_H),             // B0
        .p10(VBA_09_1_H),            // A0
        .p11(PDR_12_1_H),            // B1
        .p12(VBA_10_1_H),            // A1
        .p13(VBA_11_1_H),            // A2
        .p14(PDR_13_1_H),            // B2
        .p15(VBA_12_1_H),            // A3
        .p16(P5V)                    // VCC
    );

    // E61 7485, zone B7
    ttl_7485 E61 (
        .p1(PDR_10_1_H),             // B3
        .p2(GND),                    // LT_IN
        .p3(GND),                    // EQ_IN
        .p4(ED_1_H),                 // GT_IN
        .p5(E61_5),                  // GT_OUT
        .p6(E61_6),                  // EQ_OUT
        .p7(E61_7),                  // LT_OUT
        .p8(GND),                    // GND
        .p9(GND),                    // B0
        .p10(GND),                   // A0
        .p11(PDR_08_1_H),            // B1
        .p12(VBA_06_1_H),            // A1
        .p13(VBA_07_1_H),            // A2
        .p14(PDR_09_1_H),            // B2
        .p15(VBA_08_1_H),            // A3
        .p16(P5V)                    // VCC
    );

    // E83 82S126, DEC 23-165A2, zone C6
    prom_82s126 E83 (
        .p1(UBUS_C0_1_H),            // A6
        .p2(DISABLE_MSYN_PLUS_1_L),  // A5
        .p3(E115_3),                 // A4
        .p4(ACF2_1_H),               // A3
        .p5(ED_1_H),                 // A0
        .p6(E72_5),                  // A1
        .p7(ACF1_1_H),               // A2
        .p8(GND),                    // GND
        .p9(KT_FAULT_L),             // O4
        .p10(E83_10),                // O3
        .p11(E83_11),                // O2
        .p12(E83_12),                // O1
        .p13(START_TRAN_L),          // CE1_N
        .p14(E113_11),               // CE2_N
        .p15(UBUS_C1_1_H),           // A7
        .p16(P5V)                    // VCC
    );

    // R1 470 ohm, zone C5
    resistor R1 (
        .p1(P5V),                    // END1
        .p2(E83_12)                  // END2
    );

    // R2 470 ohm, zone C5
    resistor R2 (
        .p1(P5V),                    // END1
        .p2(E83_11)                  // END2
    );

    // R3 470 ohm, zone C5
    resistor R3 (
        .p1(P5V),                    // END1
        .p2(E83_10)                  // END2
    );

    // R4 470 ohm, zone C5
    resistor R4 (
        .p1(P5V),                    // END1
        .p2(KT_FAULT_L)              // END2
    );

    // E113 74S00, zone C6
    ttl_74s00 E113 (
        .p1(ENAB_MAINT_1_H),         // A1
        .p2(MAINT_1_H),              // B1
        .p3(E113_3),                 // Y1
        .p4(E113_3),                 // A2
        .p5(E100_8),                 // B2
        .p6(RELOCATE_H),             // Y2
        .p7(GND),                    // GND
        .p8(E113_8),                 // Y3
        .p9(RELOCATE_H),             // A3
        .p10(CLK_MSYN_H),            // B3
        .p11(E113_11),               // Y4
        .p12(RELOCATE_H),            // A4
        .p13(MSYN_1_L),              // B4
        .p14(P5V)                    // VCC
    );

    // E100 74S74, zone D3, K1-10 D5
    // Encircled by hand on K1-8, no wiring change visible.
    ttl_74s74 E100 (
        .p1(PROC_INIT_L),            // CLR1_N, K1-10
        .p2(GND),                    // D1, K1-10
        .p3(LOAD_IR_H),              // CLK1, K1-10
        .p4(E111_6),                 // PRE1_N, K1-10
        .p5(OVX_1_H),                // Q1, K1-10
        .p6(),                       // Q1_N, K1-10
        .p7(GND),                    // GND
        .p8(E100_8),                 // Q2_N
        .p9(RELOCATE_ENB_1_H),       // Q2
        .p10(P3VD),                  // PRE2_N
        .p11(E102_13),               // CLK2
        .p12(SSMUX_00_H),            // D2
        .p13(RCD_INIT_L),            // CLR2_N
        .p14(P5V)                    // VCC
    );

    // E93 74157, zone C4
    ttl_74157 E93 (
        .p1(KT_FAULT_L),             // SELECT
        .p2(MAINT_1_H),              // 1A
        .p3(SSMUX_08_H),             // 1B
        .p4(E93_4),                  // 1Y
        .p5(E83_12),                 // 2A
        .p6(SSMUX_15_H),             // 2B
        .p7(E93_7),                  // 2Y
        .p8(GND),                    // GND
        .p9(E93_9),                  // 3Y
        .p10(SSMUX_13_H),            // 3B
        .p11(E83_10),                // 3A
        .p12(E93_12),                // 4Y
        .p13(SSMUX_14_H),            // 4B
        .p14(E83_11),                // 4A
        .p15(GND),                   // G_N
        .p16(P5V)                    // VCC
    );

    // E103 74175, zone C3
    ttl_74175 E103 (
        .p1(RCD_INIT_L),             // CLR_N
        .p2(MAINT_1_H),              // Q1
        .p3(),                       // Q1_N
        .p4(E93_4),                  // D1
        .p5(E93_7),                  // D2
        .p6(E103_6),                 // Q2_N
        .p7(NR_1_H),                 // Q2
        .p8(GND),                    // GND
        .p9(E106_8),                 // CLK
        .p10(PL_1_H),                // Q3
        .p11(E103_11),               // Q3_N
        .p12(E93_12),                // D3
        .p13(E93_9),                 // D4
        .p14(E103_14),               // Q4_N
        .p15(RO_1_H),                // Q4
        .p16(P5V)                    // VCC
    );

    // E120 74132, zone B4, K1-10 B5, K1-10 B6, K1-10 A7
    ttl_74132 E120 (
        .p1(INT_MSYN_H),             // A1, K1-10
        .p2(E120_2),                 // B1, K1-10
        .p3(ASSERT_SSYN_L),          // Y1, K1-10
        .p4(START_TRAN_H),           // A2, K1-10
        .p5(UBUS_C1_1_H),            // B2, K1-10
        .p6(E120_6),                 // Y2, K1-10
        .p7(GND),                    // GND
        .p8(E120_8),                 // Y3
        .p9(E122_8),                 // A3
        .p10(LOAD_SR0_HIGH_L),       // B3
        .p11(AMUX_S1_H),             // Y4, K1-10
        .p12(AMUX_S1_1_L),           // A4, K1-10
        .p13(INT_DATI_L),            // B4, K1-10
        .p14(P5V)                    // VCC
    );

    // E71 74S157, zone B4
    ttl_74s157 E71 (
        .p1(PREVIOUS_MODE_1_L),      // S
        .p2(PSW_13_1_H),             // A1
        .p3(PSW_15_1_H),             // B1
        .p4(MODE_01_H),              // Y1
        .p5(),                       // A2
        .p6(),                       // B2
        .p7(),                       // Y2
        .p8(GND),                    // GND
        .p9(),                       // Y3
        .p10(),                      // B3
        .p11(),                      // A3
        .p12(MODE_00_H),             // Y4
        .p13(PSW_14_1_H),            // B4
        .p14(PSW_12_1_H),            // A4
        .p15(FORCE_KERNEL_1_H),      // G_N
        .p16(P5V)                    // VCC
    );

    // E73 74174, zone B3
    ttl_74174 E73 (
        .p1(RCD_INIT_L),             // CLR_N
        .p2(SR0_03_1_H),             // 1Q
        .p3(VBA_15_1_H),             // 1D
        .p4(MODE_01_H),              // 2D
        .p5(SR0_06_1_H),             // 2Q
        .p6(VBA_13_1_H),             // 3D
        .p7(SR0_01_1_H),             // 3Q
        .p8(GND),                    // GND
        .p9(E102_4),                 // CLK
        .p10(),                      // 4Q
        .p11(),                      // 4D
        .p12(SR0_05_1_H),            // 5Q
        .p13(MODE_00_H),             // 5D
        .p14(VBA_14_1_H),            // 6D
        .p15(SR0_02_1_H),            // 6Q
        .p16(P5V)                    // VCC
    );

// K1-9 SR2 AND KT MUX, PDF page 28

    // E62 74174, zone C7
    // Outputs are SR2 bits 15 to 10, unlabeled on the drawing.
    ttl_74174 E62 (
        .p1(RCD_INIT_L),             // CLR_N
        .p2(E62_2),                  // 1Q
        .p3(VBA_13_1_H),             // 1D
        .p4(VBA_14_1_H),             // 2D
        .p5(E62_5),                  // 2Q
        .p6(VBA_15_1_H),             // 3D
        .p7(E62_7),                  // 3Q
        .p8(GND),                    // GND
        .p9(LOAD_SR2_H),             // CLK
        .p10(E62_10),                // 4Q
        .p11(VBA_12_1_H),            // 4D
        .p12(E62_12),                // 5Q
        .p13(VBA_10_1_H),            // 5D
        .p14(VBA_11_1_H),            // 6D
        .p15(E62_15),                // 6Q
        .p16(P5V)                    // VCC
    );

    // E51 74174, zone B7
    ttl_74174 E51 (
        .p1(RCD_INIT_L),             // CLR_N
        .p2(E51_2),                  // 1Q
        .p3(VBA_08_1_H),             // 1D
        .p4(VBA_09_1_H),             // 2D
        .p5(E51_5),                  // 2Q
        .p6(VBA_05_1_H),             // 3D
        .p7(SR2_05_1_H),             // 3Q
        .p8(GND),                    // GND
        .p9(LOAD_SR2_H),             // CLK
        .p10(SR2_07_1_H),            // 4Q
        .p11(VBA_07_1_H),            // 4D
        .p12(SR2_04_1_H),            // 5Q
        .p13(VBA_04_1_H),            // 5D
        .p14(VBA_06_1_H),            // 6D
        .p15(SR2_06_1_H),            // 6Q
        .p16(P5V)                    // VCC
    );

    // E50 74174, zone A7
    ttl_74174 E50 (
        .p1(RCD_INIT_L),             // CLR_N
        .p2(SR2_01_1_H),             // 1Q
        .p3(VBA_01_1_H),             // 1D
        .p4(VBA_00_1_H),             // 2D
        .p5(SR2_00_1_H),             // 2Q
        .p6(VBA_02_1_H),             // 3D
        .p7(SR2_02_1_H),             // 3Q
        .p8(GND),                    // GND
        .p9(LOAD_SR2_H),             // CLK
        .p10(SR2_03_1_H),            // 4Q
        .p11(VBA_03_1_H),            // 4D
        .p12(),                      // 5Q
        .p13(GND),                   // 5D
        .p14(GND),                   // 6D
        .p15(),                      // 6Q
        .p16(P5V)                    // VCC
    );

    // E70 74LS253, zone D4
    ttl_74ls253 E70 (
        .p1(ENAB_KT_MUX_L),          // 1G_N
        .p2(PAR_PLUS_PDR_L),         // B
        .p3(NR_1_H),                 // 1C3
        .p4(E62_7),                  // 1C2
        .p5(GND),                    // 1C1
        .p6(GND),                    // 1C0
        .p7(SP_15_1_H),              // 1Y
        .p8(GND),                    // GND
        .p9(SP_14_1_H),              // 2Y
        .p10(PDR_14_1_H),            // 2C0
        .p11(GND),                   // 2C1
        .p12(E62_5),                 // 2C2
        .p13(PL_1_H),                // 2C3
        .p14(KT_MUX_S0_L),           // A
        .p15(ENAB_KT_MUX_L),         // 2G_N
        .p16(P5V)                    // VCC
    );

    // E60 74LS253, zone C4
    ttl_74ls253 E60 (
        .p1(ENAB_KT_MUX_L),          // 1G_N
        .p2(PAR_PLUS_PDR_L),         // B
        .p3(RO_1_H),                 // 1C3
        .p4(E62_2),                  // 1C2
        .p5(GND),                    // 1C1
        .p6(PDR_13_1_H),             // 1C0
        .p7(SP_13_1_H),              // 1Y
        .p8(GND),                    // GND
        .p9(SP_12_1_H),              // 2Y
        .p10(PDR_12_1_H),            // 2C0
        .p11(GND),                   // 2C1
        .p12(E62_10),                // 2C2
        .p13(GND),                   // 2C3
        .p14(KT_MUX_S0_L),           // A
        .p15(ENAB_KT_MUX_L),         // 2G_N
        .p16(P5V)                    // VCC
    );

    // E69 74LS253, zone B4
    ttl_74ls253 E69 (
        .p1(ENAB_KT_MUX_L),          // 1G_N
        .p2(PAR_PLUS_PDR_L),         // B
        .p3(GND),                    // 1C3
        .p4(E62_15),                 // 1C2
        .p5(PAR_11_1_H),             // 1C1
        .p6(PDR_11_1_H),             // 1C0
        .p7(SP_11_1_H),              // 1Y
        .p8(GND),                    // GND
        .p9(SP_10_1_H),              // 2Y
        .p10(PDR_10_1_H),            // 2C0
        .p11(PAR_10_1_H),            // 2C1
        .p12(E62_12),                // 2C2
        .p13(GND),                   // 2C3
        .p14(KT_MUX_S0_L),           // A
        .p15(ENAB_KT_MUX_L),         // 2G_N
        .p16(P5V)                    // VCC
    );

    // E59 74LS253, zone A4
    ttl_74ls253 E59 (
        .p1(ENAB_KT_MUX_L),          // 1G_N
        .p2(PAR_PLUS_PDR_L),         // B
        .p3(GND),                    // 1C3
        .p4(E51_5),                  // 1C2
        .p5(PAR_09_1_H),             // 1C1
        .p6(PDR_09_1_H),             // 1C0
        .p7(SP_09_1_H),              // 1Y
        .p8(GND),                    // GND
        .p9(SP_08_1_H),              // 2Y
        .p10(PDR_08_1_H),            // 2C0
        .p11(PAR_08_1_H),            // 2C1
        .p12(E51_2),                 // 2C2
        .p13(MAINT_1_H),             // 2C3
        .p14(KT_MUX_S0_L),           // A
        .p15(ENAB_KT_MUX_L),         // 2G_N
        .p16(P5V)                    // VCC
    );

    // E68 74LS253, zone D2
    ttl_74ls253 E68 (
        .p1(ENAB_KT_MUX_L),          // 1G_N
        .p2(PAR_PLUS_PDR_L),         // B
        .p3(GND),                    // 1C3
        .p4(SR2_07_1_H),             // 1C2
        .p5(PAR_07_1_H),             // 1C1
        .p6(GND),                    // 1C0
        .p7(SP_07_1_H),              // 1Y
        .p8(GND),                    // GND
        .p9(SP_06_1_H),              // 2Y
        .p10(WBIT_H),                // 2C0
        .p11(PAR_06_1_H),            // 2C1
        .p12(SR2_06_1_H),            // 2C2
        .p13(SR0_06_1_H),            // 2C3
        .p14(KT_MUX_S0_L),           // A
        .p15(ENAB_KT_MUX_L),         // 2G_N
        .p16(P5V)                    // VCC
    );

    // E58 74LS253, zone C2
    // Pin 11 is labeled K1-9 PAR 04 (1) H, taken as K1-7 PAR 04 (1) H.
    ttl_74ls253 E58 (
        .p1(ENAB_KT_MUX_L),          // 1G_N
        .p2(PAR_PLUS_PDR_L),         // B
        .p3(SR0_05_1_H),             // 1C3
        .p4(SR2_05_1_H),             // 1C2
        .p5(PAR_05_1_H),             // 1C1
        .p6(GND),                    // 1C0
        .p7(SP_05_1_H),              // 1Y
        .p8(GND),                    // GND
        .p9(SP_04_1_H),              // 2Y
        .p10(GND),                   // 2C0
        .p11(PAR_04_1_H),            // 2C1
        .p12(SR2_04_1_H),            // 2C2
        .p13(GND),                   // 2C3
        .p14(KT_MUX_S0_L),           // A
        .p15(ENAB_KT_MUX_L),         // 2G_N
        .p16(P5V)                    // VCC
    );

    // E67 74LS253, zone B2
    ttl_74ls253 E67 (
        .p1(ENAB_KT_MUX_L),          // 1G_N
        .p2(PAR_PLUS_PDR_L),         // B
        .p3(SR0_03_1_H),             // 1C3
        .p4(SR2_03_1_H),             // 1C2
        .p5(PAR_03_1_H),             // 1C1
        .p6(ED_1_H),                 // 1C0
        .p7(SP_03_1_H),              // 1Y
        .p8(GND),                    // GND
        .p9(SP_02_1_H),              // 2Y
        .p10(ACF2_1_H),              // 2C0
        .p11(PAR_02_1_H),            // 2C1
        .p12(SR2_02_1_H),            // 2C2
        .p13(SR0_02_1_H),            // 2C3
        .p14(KT_MUX_S0_L),           // A
        .p15(ENAB_KT_MUX_L),         // 2G_N
        .p16(P5V)                    // VCC
    );

    // E57 74LS253, zone A2
    // Pin 13 is labeled K1-8 RELOCATE ENB (1) H, sheet K1-8 labels the same
    // signal K1-3.
    ttl_74ls253 E57 (
        .p1(ENAB_KT_MUX_L),          // 1G_N
        .p2(PAR_PLUS_PDR_L),         // B
        .p3(SR0_01_1_H),             // 1C3
        .p4(SR2_01_1_H),             // 1C2
        .p5(PAR_01_1_H),             // 1C1
        .p6(ACF1_1_H),               // 1C0
        .p7(SP_01_1_H),              // 1Y
        .p8(GND),                    // GND
        .p9(SP_00_1_H),              // 2Y
        .p10(GND),                   // 2C0
        .p11(PAR_00_1_H),            // 2C1
        .p12(SR2_00_1_H),            // 2C2
        .p13(RELOCATE_ENB_1_H),      // 2C3
        .p14(KT_MUX_S0_L),           // A
        .p15(ENAB_KT_MUX_L),         // 2G_N
        .p16(P5V)                    // VCC
    );

// K1-10 CC DECODE, ADDR DET., PDF page 29

    // E111 74H01, zone D5, K1-10 B7, K1-10 B5
    ttl_74h01 E111 (
        .p1(),                       // A1
        .p2(),                       // B1
        .p3(),                       // Y1
        .p4(E101_1),                 // A2
        .p5(E115_6),                 // B2
        .p6(E111_6),                 // Y2
        .p7(GND),                    // GND
        .p8(E111_8),                 // Y3
        .p9(PAR_PLUS_PDR_L),         // A3
        .p10(INT_SSYN_L),            // B3
        .p11(ASSERT_SSYN_H),         // Y4
        .p12(ASSERT_SSYN_L),         // A4
        .p13(E109_9),                // B4
        .p14(P5V)                    // VCC
    );

    // R19 470 ohm, zone D5
    resistor R19 (
        .p1(P5V),                    // END1
        .p2(E111_6)                  // END2
    );

    // E108 74S157, zone C7
    ttl_74s157 E108 (
        .p1(BYTE_L),                 // S
        .p2(SERIAL_SHIFT_H),         // A1
        .p3(BREG_08_1_H),            // B1
        .p4(SHIFT_IN_07_H),          // Y1
        .p5(ALU_07_H),               // A2
        .p6(ALU_15_H),               // B2
        .p7(CC_N_H),                 // Y2
        .p8(GND),                    // GND
        .p9(E108_9),                 // Y3
        .p10(SP_15_1_H),             // B3
        .p11(SP_07_1_H),             // A3
        .p12(E108_12),               // Y4
        .p13(BLEG_15_H),             // B4
        .p14(BLEG_07_H),             // A4
        .p15(GND),                   // G_N
        .p16(P5V)                    // VCC
    );

    // E107 82S129, DEC 23-164A2, zone B6
    prom_82s129 E107 (
        .p1(E108_9),                 // A6
        .p2(E108_12),                // A5
        .p3(ROT_CBIT_1_H),           // A4
        .p4(C_BIT_1_H),              // A3
        .p5(CC_CODE_00_H),           // A0
        .p6(CC_CODE_01_H),           // A1
        .p7(CC_CODE_02_H),           // A2
        .p8(GND),                    // GND
        .p9(CC_V_H),                 // O4
        .p10(CC_C_H),                // O3
        .p11(),                      // O2
        .p12(),                      // O1
        .p13(GND),                   // CE1_N
        .p14(GND),                   // CE2_N
        .p15(CC_N_H),                // A7
        .p16(P5V)                    // VCC
    );

    // E119 74153, zone C5
    ttl_74153 E119 (
        .p1(GND),                    // 1G_N
        .p2(SHIFT_MUX_01_L),         // B
        .p3(BXREG_15_1_H),           // 1C3
        .p4(GND),                    // 1C2
        .p5(SERIAL_SHIFT_H),         // 1C1
        .p6(BREG_15_1_H),            // 1C0
        .p7(SHIFT_IN_B_H),           // 1Y
        .p8(GND),                    // GND
        .p9(SHIFT_IN_BX_H),          // 2Y
        .p10(ALU_COUT_H),            // 2C0
        .p11(OVX_1_H),               // 2C1
        .p12(P3VD),                  // 2C2
        .p13(GND),                   // 2C3
        .p14(SHIFT_MUX_00_L),        // A
        .p15(GND),                   // 2G_N
        .p16(P5V)                    // VCC
    );

    // E114 7402, zone B5
    // Pin 8 drawn as 6.
    ttl_7402 E114 (
        .p1(),                       // Y1
        .p2(),                       // A1
        .p3(),                       // B1
        .p4(),                       // Y2
        .p5(),                       // A2
        .p6(),                       // B2
        .p7(GND),                    // GND
        .p8(E117_6),                 // A3
        .p9(E114_13),                // B3
        .p10(E114_10),               // Y3
        .p11(INT_MSYN_H),            // A4
        .p12(MAN_CLK_ENAB_L),        // B4
        .p13(E114_13),               // Y4
        .p14(P5V)                    // VCC
    );

    // E116 7437, zone B8
    ttl_7437 E116 (
        .p1(INT_DATI_L),             // A1
        .p2(INT_DATI_L),             // B1
        .p3(E116_3),                 // Y1
        .p4(E116_3),                 // A2
        .p5(GEN_REG_L),              // B2
        .p6(E116_6),                 // Y2
        .p7(GND),                    // GND
        .p8(INT_DATI_H),             // Y3
        .p9(E116_6),                 // A3
        .p10(E116_6),                // B3
        .p11(ENAB_KT_MUX_L),         // Y4
        .p12(INT_DATI_H),            // A4
        .p13(INT_MSYN_H),            // B4
        .p14(P5V)                    // VCC
    );

    // R23 1 kohm, zone B6
    resistor R23 (
        .p1(P5V),                    // END1
        .p2(E111_8)                  // END2
    );

    // R24 47 ohm, zone B6
    resistor R24 (
        .p1(E111_8),                 // END1
        .p2(E120_2)                  // END2
    );

    // C49 220 pF, zone B6
    capacitor C49 (
        .p1(E120_2),                 // END1
        .p2(GND)                     // END2
    );

    // R22 1 kohm, zone B5
    resistor R22 (
        .p1(P5V),                    // END1
        .p2(ASSERT_SSYN_H)           // END2
    );

    // E54 7430, zone A4
    ttl_7430 E54 (
        .p1(INT_MSYN_H),             // A
        .p2(PBA_10_H),               // B
        .p3(PBA_14_H),               // C
        .p4(PBA_17_H),               // D
        .p5(PBA_15_H),               // E
        .p6(PBA_16_H),               // F
        .p7(GND),                    // GND
        .p8(K110_772XXX_L),          // Y
        .p9(),                       // NC
        .p10(),                      // NC
        .p11(PBA_12_H),              // G
        .p12(PBA_13_H),              // H
        .p13(),                      // NC
        .p14(P5V)                    // VCC
    );

    // E87 7430, zone A4
    // Drawn E8?, the last digit partly erased.
    ttl_7430 E87 (
        .p1(P3VB),                   // A
        .p2(P3VB),                   // B
        .p3(PBA_09_H),               // C
        .p4(PBA_06_H),               // D
        .p5(PBA_11_H),               // E
        .p6(PBA_04_H),               // F
        .p7(GND),                    // GND
        .p8(XX556X_L),               // Y
        .p9(),                       // NC
        .p10(),                      // NC
        .p11(PBA_05_H),              // G
        .p12(PBA_08_H),              // H
        .p13(),                      // NC
        .p14(P5V)                    // VCC
    );

    // E86 82S126, DEC 23-166A2, zone B4
    prom_82s126 E86 (
        .p1(PBA_08_H),               // A6
        .p2(PBA_07_H),               // A5
        .p3(PBA_06_H),               // A4
        .p4(PBA_05_H),               // A3
        .p5(C1_H),                   // A0
        .p6(SACK_RET_1_H),           // A1
        .p7(PBA_11_H),               // A2
        .p8(GND),                    // GND
        .p9(GEN_REG_L),              // O4
        .p10(SP_WRITE_L),            // O3
        .p11(INT_DATI_L),            // O2
        .p12(INT_SSYN_L),            // O1
        .p13(PBA_04_H),              // CE1_N
        .p14(K110_772XXX_L),         // CE2_N
        .p15(PBA_09_H),              // A7
        .p16(P5V)                    // VCC
    );

    // R18 470 ohm, zone B3
    resistor R18 (
        .p1(P5V),                    // END1
        .p2(GEN_REG_L)               // END2
    );

    // E77 82S126, DEC 23-167A2, zone D3
    prom_82s126 E77 (
        .p1(C0_H),                   // A6
        .p2(PBA_00_H),               // A5
        .p3(PBA_11_H),               // A4
        .p4(PBA_09_H),               // A3
        .p5(PBA_05_H),               // A0
        .p6(PBA_06_H),               // A1
        .p7(PBA_08_H),               // A2
        .p8(GND),                    // GND
        .p9(LOAD_PAR_HIGH_L),        // O4
        .p10(LOAD_PAR_LOW_L),        // O3
        .p11(LOAD_PDR_HIGH_L),       // O2
        .p12(LOAD_PDR_LOW_L),        // O1
        .p13(E118_11),               // CE1_N
        .p14(K110_772XXX_L),         // CE2_N
        .p15(C1_H),                  // A7
        .p16(P5V)                    // VCC
    );

    // R5 470 ohm, zone D2
    resistor R5 (
        .p1(P5V),                    // END1
        .p2(LOAD_PAR_HIGH_L)         // END2
    );

    // R6 470 ohm, zone D2
    resistor R6 (
        .p1(P5V),                    // END1
        .p2(LOAD_PAR_LOW_L)          // END2
    );

    // R7 470 ohm, zone D2
    resistor R7 (
        .p1(P5V),                    // END1
        .p2(LOAD_PDR_HIGH_L)         // END2
    );

    // R8 470 ohm, zone D2
    resistor R8 (
        .p1(P5V),                    // END1
        .p2(LOAD_PDR_LOW_L)          // END2
    );

    // E76 82S126, DEC 23-168A2, zone C3
    // Pin 13 drawn as 3.
    prom_82s126 E76 (
        .p1(C0_H),                   // A6
        .p2(PBA_00_H),               // A5
        .p3(PBA_11_H),               // A4
        .p4(PBA_09_H),               // A3
        .p5(PBA_05_H),               // A0
        .p6(PBA_06_H),               // A1
        .p7(PBA_08_H),               // A2
        .p8(GND),                    // GND
        .p9(INT_DATI_L),             // O4
        .p10(PAR_PLUS_PDR_L),        // O3
        .p11(),                      // O2
        .p12(KT_MUX_S0_L),           // O1
        .p13(E118_11),               // CE1_N
        .p14(K110_772XXX_L),         // CE2_N
        .p15(C1_H),                  // A7
        .p16(P5V)                    // VCC
    );

    // R9 470 ohm, zone C2
    resistor R9 (
        .p1(P5V),                    // END1
        .p2(PAR_PLUS_PDR_L)          // END2
    );

    // R10 470 ohm, zone C2
    resistor R10 (
        .p1(P5V),                    // END1
        .p2(INT_DATI_L)              // END2
    );

    // R11 470 ohm, zone C2
    resistor R11 (
        .p1(P5V),                    // END1
        .p2(KT_MUX_S0_L)             // END2
    );

    // E75 82S126, DEC 23-169A2, zone B3
    // No O.C. mark on this symbol, same PROM number as E74.
    prom_82s126 E75 (
        .p1(C0_H),                   // A6
        .p2(PBA_00_H),               // A5
        .p3(PBA_07_H),               // A4
        .p4(PBA_03_H),               // A3
        .p5(GND),                    // A0
        .p6(PBA_01_H),               // A1
        .p7(PBA_02_H),               // A2
        .p8(GND),                    // GND
        .p9(LOAD_PSW_HIGH_L),        // O4
        .p10(LOAD_PSW_LOW_L),        // O3
        .p11(LOAD_SR0_HIGH_L),       // O2
        .p12(LOAD_SR0_LOW_L),        // O1
        .p13(XX556X_L),              // CE1_N
        .p14(K110_772XXX_L),         // CE2_N
        .p15(C1_H),                  // A7
        .p16(P5V)                    // VCC
    );

    // R12 470 ohm, zone B2
    resistor R12 (
        .p1(P5V),                    // END1
        .p2(LOAD_PSW_HIGH_L)         // END2
    );

    // R13 470 ohm, zone B2
    resistor R13 (
        .p1(P5V),                    // END1
        .p2(LOAD_PSW_LOW_L)          // END2
    );

    // R14 470 ohm, zone B2
    resistor R14 (
        .p1(P5V),                    // END1
        .p2(LOAD_SR0_HIGH_L)         // END2
    );

    // R15 470 ohm, zone B2
    // Value blurred, the parts list gives 470 for R1 to R19.
    resistor R15 (
        .p1(P5V),                    // END1
        .p2(LOAD_SR0_LOW_L)          // END2
    );

    // E74 82S126, DEC 23-169A2, zone A3
    prom_82s126 E74 (
        .p1(C0_H),                   // A6
        .p2(PBA_00_H),               // A5
        .p3(PBA_07_H),               // A4
        .p4(PBA_03_H),               // A3
        .p5(P3VB),                   // A0
        .p6(PBA_01_H),               // A1
        .p7(PBA_02_H),               // A2
        .p8(GND),                    // GND
        .p9(INT_DATI_L),             // O4
        .p10(INT_SSYN_L),            // O3
        .p11(PSW_DATI_L),            // O2
        .p12(KT_MUX_S0_L),           // O1
        .p13(XX556X_L),              // CE1_N
        .p14(K110_772XXX_L),         // CE2_N
        .p15(C1_H),                  // A7
        .p16(P5V)                    // VCC
    );

    // R16 470 ohm, zone A2
    resistor R16 (
        .p1(P5V),                    // END1
        .p2(INT_SSYN_L)              // END2
    );

    // R17 470 ohm, zone A2
    resistor R17 (
        .p1(P5V),                    // END1
        .p2(PSW_DATI_L)              // END2
    );

endmodule
`default_nettype wire
