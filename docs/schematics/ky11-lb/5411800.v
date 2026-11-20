`default_nettype none
// Verilog transcription of the 5411800 KY11-LB console board
// From DEC KY11-LB print set MP00015, sheets 2 and 3
// Displays D7 to D12 are not transcribed.

module ky11_lb_5411800 (
    (* edge_contact = "J1-1" *) inout wire J1_1,
    (* edge_contact = "J1-2" *) inout wire BOOT_SW,
    (* edge_contact = "J1-3" *) inout wire J1_3,
    (* edge_contact = "J1-4" *) inout wire J1_4,
    (* edge_contact = "J1-5" *) inout wire J1_5,
    (* edge_contact = "J1-6" *) inout wire J1_6,
    (* edge_contact = "J1-7" *) inout wire J1_7,
    (* edge_contact = "J1-8" *) inout wire J1_8,
    (* edge_contact = "J1-9" *) inout wire J1_9,
    (* edge_contact = "J1-10" *) inout wire J1_10,
    (* edge_contact = "J1-11" *) inout wire J1_11,
    (* edge_contact = "J1-12" *) inout wire J1_12,
    (* edge_contact = "J1-13" *) inout wire J1_13,
    (* edge_contact = "J1-14" *) inout wire J1_14,
    (* edge_contact = "J1-15" *) inout wire J1_15,
    (* edge_contact = "J1-16" *) inout wire J1_16,
    (* edge_contact = "J1-17" *) inout wire J1_17,
    (* edge_contact = "J1-18" *) inout wire J1_18,
    (* edge_contact = "J2-1,TB2" *) inout wire BOOT_EN,
    (* edge_contact = "J2-2" *) inout wire BATT,
    (* edge_contact = "J2-3" *) inout wire DC_OFF,
    (* edge_contact = "J2-4" *) inout wire STNBY,
    (* edge_contact = "J2-5,TB6" *) inout wire ON,
    (* edge_contact = "J2-6" *) inout wire J2_6,
    (* edge_contact = "J1-19,J2-9,J2-10" *) inout wire PLUS_5V,
    (* edge_contact = "J1-20,J2-7,J2-8" *) inout wire GND
);

    wire D10_ANODE;
    wire D11_ANODE;
    wire D12_ANODE;
    wire D2_ANODE;
    wire D3_ANODE;
    wire D4_ANODE;
    wire D5_ANODE;
    wire D6_ANODE;
    wire D7_ANODE;
    wire D8_ANODE;
    wire D9_ANODE;
    wire DRIVE_1_L;
    wire DRIVE_2_L;
    wire DRIVE_3_L;
    wire DRIVE_4_L;
    wire DRIVE_5_L;
    wire DRIVE_6_L;
    wire E6_A_N;
    wire E6_B_N;
    wire E6_BLANK;
    wire E6_C_N;
    wire E6_D_N;
    wire E6_E_N;
    wire E6_F_N;
    wire E6_G_N;
    wire KP_1;
    wire KP_10;
    wire KP_11;
    wire KP_12;
    wire KP_13;
    wire KP_14;
    wire KP_15;
    wire KP_16;
    wire KP_17;
    wire KP_18;
    wire KP_19;
    wire KP_2;
    wire KP_20;
    wire KP_3;
    wire KP_4;
    wire KP_5;
    wire KP_6;
    wire KP_7;
    wire KP_8;
    wire KP_9;
    wire Q1_B;
    wire Q2_B;
    wire Q3_B;
    wire Q4_B;
    wire Q5_B;
    wire Q6_B;
    wire READ_1_H;
    wire READ_2_H;
    wire READ_3_H;
    wire READ_4_H;
    wire READ_5_H;
    wire SEG_A;
    wire SEG_B;
    wire SEG_C;
    wire SEG_D;
    wire SEG_E;
    wire SEG_F;
    wire SEG_G;

// Sheet 2, MP00015 PDF page 4

    // KP keypad_1211858, zone C7
    keypad_1211858 KP (
        .p1(KP_1),                   // START
        .p2(KP_2),                   // HLT_SS
        .p3(KP_3),                   // CONT
        .p4(KP_4),                   // BOOT
        .p5(KP_5),                   // KEY6
        .p6(KP_6),                   // DEP
        .p7(KP_7),                   // KEY3
        .p8(KP_8),                   // CNTRL
        .p9(KP_9),                   // KEY5
        .p10(KP_10),                 // EXAM
        .p11(KP_11),                 // KEY2
        .p12(KP_12),                 // INIT
        .p13(KP_13),                 // KEY4
        .p14(KP_14),                 // KEY7
        .p15(KP_15),                 // KEY1
        .p16(KP_16),                 // KEY0
        .p17(KP_17),                 // LAD
        .p18(KP_18),                 // DIS_AD
        .p19(KP_19),                 // LSR
        .p20(KP_20),                 // CLR
        .p21(GND)                    // COMMON
    );

    // R23 10K. The line from R23 to E5 pin 2 shows a short gap on the scan, zone D7
    resistor R23 (
        .p1(PLUS_5V),                // END1
        .p2(KP_2)                    // END2
    );

    // R22 10 kohm, zone D7
    resistor R22 (
        .p1(PLUS_5V),                // END1
        .p2(KP_3)                    // END2
    );

    // R21 10 kohm, zone D7
    resistor R21 (
        .p1(PLUS_5V),                // END1
        .p2(KP_4)                    // END2
    );

    // R24 10 kohm, zone D7
    resistor R24 (
        .p1(PLUS_5V),                // END1
        .p2(KP_1)                    // END2
    );

    // R19 10 kohm, zone C7
    resistor R19 (
        .p1(PLUS_5V),                // END1
        .p2(KP_6)                    // END2
    );

    // R20 10 kohm, zone C7
    resistor R20 (
        .p1(PLUS_5V),                // END1
        .p2(KP_5)                    // END2
    );

    // R14 10 kohm, zone C7
    resistor R14 (
        .p1(PLUS_5V),                // END1
        .p2(KP_7)                    // END2
    );

    // R13 10 kohm, zone C7
    resistor R13 (
        .p1(PLUS_5V),                // END1
        .p2(KP_8)                    // END2
    );

    // R25 10 kohm, zone C7
    resistor R25 (
        .p1(PLUS_5V),                // END1
        .p2(KP_10)                   // END2
    );

    // R26 10 kohm, zone C7
    resistor R26 (
        .p1(PLUS_5V),                // END1
        .p2(KP_9)                    // END2
    );

    // R18 10 kohm, zone B7
    resistor R18 (
        .p1(PLUS_5V),                // END1
        .p2(KP_11)                   // END2
    );

    // R17 10 kohm, zone B7
    resistor R17 (
        .p1(PLUS_5V),                // END1
        .p2(KP_12)                   // END2
    );

    // R15 10 kohm, zone B7
    resistor R15 (
        .p1(PLUS_5V),                // END1
        .p2(KP_14)                   // END2
    );

    // R16 10 kohm, zone B7
    resistor R16 (
        .p1(PLUS_5V),                // END1
        .p2(KP_13)                   // END2
    );

    // R10 10 kohm, zone B7
    resistor R10 (
        .p1(PLUS_5V),                // END1
        .p2(KP_15)                   // END2
    );

    // R9 10 kohm, zone B7
    resistor R9 (
        .p1(PLUS_5V),                // END1
        .p2(KP_16)                   // END2
    );

    // R11 10 kohm, zone A7
    resistor R11 (
        .p1(PLUS_5V),                // END1
        .p2(KP_18)                   // END2
    );

    // R12 10K. The line from R12 to E1 pin 6 shows a short gap on the scan, zone A7
    resistor R12 (
        .p1(PLUS_5V),                // END1
        .p2(KP_17)                   // END2
    );

    // R3 10 kohm, zone A7
    resistor R3 (
        .p1(PLUS_5V),                // END1
        .p2(KP_19)                   // END2
    );

    // R4 10 kohm, zone A7
    resistor R4 (
        .p1(PLUS_5V),                // END1
        .p2(KP_20)                   // END2
    );

    // E5 signetics_8881, zone D5
    signetics_8881 E5 (
        .p1(J1_9),                   // Y1
        .p2(KP_2),                   // A1
        .p3(READ_1_H),               // B1
        .p4(J1_8),                   // Y2
        .p5(READ_1_H),               // A2
        .p6(KP_3),                   // B2
        .p7(GND),                    // GND
        .p8(KP_4),                   // A3
        .p9(READ_1_H),               // B3
        .p10(J1_7),                  // Y3
        .p11(READ_1_H),              // B4
        .p12(KP_1),                  // A4
        .p13(J1_6),                  // Y4
        .p14(PLUS_5V)                // VCC
    );

    // E3 signetics_8881, zone C5
    signetics_8881 E3 (
        .p1(J1_9),                   // Y1
        .p2(KP_6),                   // A1
        .p3(READ_2_H),               // B1
        .p4(J1_8),                   // Y2
        .p5(READ_2_H),               // A2
        .p6(KP_5),                   // B2
        .p7(GND),                    // GND
        .p8(KP_7),                   // A3
        .p9(READ_2_H),               // B3
        .p10(J1_7),                  // Y3
        .p11(READ_2_H),              // B4
        .p12(KP_8),                  // A4
        .p13(J1_6),                  // Y4
        .p14(PLUS_5V)                // VCC
    );

    // E4 signetics_8881, zone C5
    signetics_8881 E4 (
        .p1(J1_9),                   // Y1
        .p2(KP_10),                  // A1
        .p3(READ_3_H),               // B1
        .p4(J1_8),                   // Y2
        .p5(READ_3_H),               // A2
        .p6(KP_9),                   // B2
        .p7(GND),                    // GND
        .p8(KP_11),                  // A3
        .p9(READ_3_H),               // B3
        .p10(J1_7),                  // Y3
        .p11(READ_3_H),              // B4
        .p12(KP_12),                 // A4
        .p13(J1_6),                  // Y4
        .p14(PLUS_5V)                // VCC
    );

    // E2 signetics_8881, zone B5
    signetics_8881 E2 (
        .p1(J1_9),                   // Y1
        .p2(KP_14),                  // A1
        .p3(READ_4_H),               // B1
        .p4(J1_8),                   // Y2
        .p5(READ_4_H),               // A2
        .p6(KP_13),                  // B2
        .p7(GND),                    // GND
        .p8(KP_15),                  // A3
        .p9(READ_4_H),               // B3
        .p10(J1_7),                  // Y3
        .p11(READ_4_H),              // B4
        .p12(KP_16),                 // A4
        .p13(J1_6),                  // Y4
        .p14(PLUS_5V)                // VCC
    );

    // E1 signetics_8881, zone A5
    signetics_8881 E1 (
        .p1(J1_9),                   // Y1
        .p2(KP_18),                  // A1
        .p3(READ_5_H),               // B1
        .p4(J1_8),                   // Y2
        .p5(READ_5_H),               // A2
        .p6(KP_17),                  // B2
        .p7(GND),                    // GND
        .p8(KP_19),                  // A3
        .p9(READ_5_H),               // B3
        .p10(J1_7),                  // Y3
        .p11(READ_5_H),              // B4
        .p12(KP_20),                 // A4
        .p13(J1_6),                  // Y4
        .p14(PLUS_5V)                // VCC
    );

    // R27 1 kohm, zone D5
    resistor R27 (
        .p1(PLUS_5V),                // END1
        .p2(J1_9)                    // END2
    );

    // R28 1 kohm, zone D4
    resistor R28 (
        .p1(PLUS_5V),                // END1
        .p2(J1_8)                    // END2
    );

    // R29 1 kohm, zone D4
    resistor R29 (
        .p1(PLUS_5V),                // END1
        .p2(J1_7)                    // END2
    );

    // R30 1 kohm, zone D4
    resistor R30 (
        .p1(PLUS_5V),                // END1
        .p2(J1_6)                    // END2
    );

    // R1 150 ohm, zone D3
    resistor R1 (
        .p1(PLUS_5V),                // END1
        .p2(D2_ANODE)                // END2
    );

    // D2 led, zone D3
    led D2 (
        .p1(D2_ANODE),               // ANODE
        .p2(GND)                     // CATHODE
    );

    // R2 150 ohm, zone D3
    resistor R2 (
        .p1(PLUS_5V),                // END1
        .p2(D6_ANODE)                // END2
    );

    // D6 led, zone D3
    led D6 (
        .p1(D6_ANODE),               // ANODE
        .p2(J1_1)                    // CATHODE
    );

    // R5 150 ohm, zone D3
    resistor R5 (
        .p1(PLUS_5V),                // END1
        .p2(D3_ANODE)                // END2
    );

    // D3 led, zone D3
    led D3 (
        .p1(D3_ANODE),               // ANODE
        .p2(J1_3)                    // CATHODE
    );

    // R7 150 ohm, zone D2
    resistor R7 (
        .p1(PLUS_5V),                // END1
        .p2(D5_ANODE)                // END2
    );

    // D5 led, zone D2
    led D5 (
        .p1(D5_ANODE),               // ANODE
        .p2(J1_4)                    // CATHODE
    );

    // R6 150 ohm, zone D2
    resistor R6 (
        .p1(PLUS_5V),                // END1
        .p2(D4_ANODE)                // END2
    );

    // D4 led, zone D2
    led D4 (
        .p1(D4_ANODE),               // ANODE
        .p2(J1_5)                    // CATHODE
    );

// Sheet 3, MP00015 PDF page 5

    // E6 ttl_7447a, zone C7
    ttl_7447a E6 (
        .p1(J1_11),                  // B
        .p2(J1_10),                  // C
        .p3(E6_BLANK),               // LT_N
        .p4(E6_BLANK),               // BI_N_RBO_N
        .p5(E6_BLANK),               // RBI_N
        .p6(GND),                    // D
        .p7(J1_12),                  // A
        .p8(GND),                    // GND
        .p9(E6_E_N),                 // E_N
        .p10(E6_D_N),                // D_N
        .p11(E6_C_N),                // C_N
        .p12(E6_B_N),                // B_N
        .p13(E6_A_N),                // A_N
        .p14(E6_G_N),                // G_N
        .p15(E6_F_N),                // F_N
        .p16(PLUS_5V)                // VCC
    );

    // R56 10 kohm, zone C7
    resistor R56 (
        .p1(PLUS_5V),                // END1
        .p2(J1_12)                   // END2
    );

    // R54 10 kohm, zone C7
    resistor R54 (
        .p1(PLUS_5V),                // END1
        .p2(J1_11)                   // END2
    );

    // R55 10 kohm, zone C7
    resistor R55 (
        .p1(PLUS_5V),                // END1
        .p2(J1_10)                   // END2
    );

    // R46 10 kohm, zone C7
    resistor R46 (
        .p1(PLUS_5V),                // END1
        .p2(E6_BLANK)                // END2
    );

    // R53 150 ohm, zone C6
    resistor R53 (
        .p1(E6_A_N),                 // END1
        .p2(SEG_A)                   // END2
    );

    // R50 150 ohm, zone C6
    resistor R50 (
        .p1(E6_B_N),                 // END1
        .p2(SEG_B)                   // END2
    );

    // R48 150 ohm, zone C6
    resistor R48 (
        .p1(E6_C_N),                 // END1
        .p2(SEG_C)                   // END2
    );

    // R47 150 ohm, zone C6
    resistor R47 (
        .p1(E6_D_N),                 // END1
        .p2(SEG_D)                   // END2
    );

    // R51 150 ohm, zone C6
    resistor R51 (
        .p1(E6_E_N),                 // END1
        .p2(SEG_E)                   // END2
    );

    // R52 150 ohm, zone C6
    resistor R52 (
        .p1(E6_F_N),                 // END1
        .p2(SEG_F)                   // END2
    );

    // R49 150 ohm, zone C6
    resistor R49 (
        .p1(E6_G_N),                 // END1
        .p2(SEG_G)                   // END2
    );

    // Q1 mpsa55, zone D6
    mpsa55 Q1 (
        .p1(PLUS_5V),                // EMITTER
        .p2(Q1_B),                   // BASE
        .p3(D12_ANODE)               // COLLECTOR
    );

    // Q2 mpsa55, zone D5
    mpsa55 Q2 (
        .p1(PLUS_5V),                // EMITTER
        .p2(Q2_B),                   // BASE
        .p3(D11_ANODE)               // COLLECTOR
    );

    // Q3 mpsa55, zone D4
    mpsa55 Q3 (
        .p1(PLUS_5V),                // EMITTER
        .p2(Q3_B),                   // BASE
        .p3(D10_ANODE)               // COLLECTOR
    );

    // Q4 mpsa55, zone D3
    mpsa55 Q4 (
        .p1(PLUS_5V),                // EMITTER
        .p2(Q4_B),                   // BASE
        .p3(D9_ANODE)                // COLLECTOR
    );

    // Q5 mpsa55, zone D2
    mpsa55 Q5 (
        .p1(PLUS_5V),                // EMITTER
        .p2(Q5_B),                   // BASE
        .p3(D8_ANODE)                // COLLECTOR
    );

    // Q6 mpsa55, zone D1
    mpsa55 Q6 (
        .p1(PLUS_5V),                // EMITTER
        .p2(Q6_B),                   // BASE
        .p3(D7_ANODE)                // COLLECTOR
    );

    // R32 1 kohm, zone D6
    resistor R32 (
        .p1(PLUS_5V),                // END1
        .p2(DRIVE_6_L)               // END2
    );

    // R31 620 ohm, zone D6
    resistor R31 (
        .p1(DRIVE_6_L),              // END1
        .p2(Q1_B)                    // END2
    );

    // R34 1K. Label blurred, zone D5
    resistor R34 (
        .p1(PLUS_5V),                // END1
        .p2(DRIVE_5_L)               // END2
    );

    // R33 620. Label blurred, zone D5
    resistor R33 (
        .p1(DRIVE_5_L),              // END1
        .p2(Q2_B)                    // END2
    );

    // R36 1 kohm, zone D4
    resistor R36 (
        .p1(PLUS_5V),                // END1
        .p2(DRIVE_4_L)               // END2
    );

    // R35 620 ohm, zone D4
    resistor R35 (
        .p1(DRIVE_4_L),              // END1
        .p2(Q3_B)                    // END2
    );

    // R38 1 kohm, zone D3
    resistor R38 (
        .p1(PLUS_5V),                // END1
        .p2(DRIVE_3_L)               // END2
    );

    // R37 620 ohm, zone D3
    resistor R37 (
        .p1(DRIVE_3_L),              // END1
        .p2(Q4_B)                    // END2
    );

    // R40 1 kohm, zone D2
    resistor R40 (
        .p1(PLUS_5V),                // END1
        .p2(DRIVE_2_L)               // END2
    );

    // R39 620 ohm, zone D2
    resistor R39 (
        .p1(DRIVE_2_L),              // END1
        .p2(Q5_B)                    // END2
    );

    // R42 1 kohm, zone D1
    resistor R42 (
        .p1(PLUS_5V),                // END1
        .p2(DRIVE_1_L)               // END2
    );

    // R41 620 ohm, zone D1
    resistor R41 (
        .p1(DRIVE_1_L),              // END1
        .p2(Q6_B)                    // END2
    );

    // R8 10 kohm, zone C7
    resistor R8 (
        .p1(PLUS_5V),                // END1
        .p2(J1_18)                   // END2
    );

    // R43 10 kohm, zone C7
    resistor R43 (
        .p1(PLUS_5V),                // END1
        .p2(J1_17)                   // END2
    );

    // R58 10 kohm, zone C7
    resistor R58 (
        .p1(PLUS_5V),                // END1
        .p2(J1_16)                   // END2
    );

    // R44 10 kohm, zone C7
    resistor R44 (
        .p1(PLUS_5V),                // END1
        .p2(J1_15)                   // END2
    );

    // R57 10 kohm, zone C7
    resistor R57 (
        .p1(PLUS_5V),                // END1
        .p2(J1_14)                   // END2
    );

    // R45 10 kohm, zone C7
    resistor R45 (
        .p1(PLUS_5V),                // END1
        .p2(J1_13)                   // END2
    );

    // E7 ttl_7417, zone C6
    ttl_7417 E7 (
        .p1(J1_14),                  // A1
        .p2(DRIVE_5_L),              // Y1
        .p3(J1_16),                  // A2
        .p4(DRIVE_3_L),              // Y2
        .p5(J1_18),                  // A3
        .p6(DRIVE_1_L),              // Y3
        .p7(GND),                    // GND
        .p8(DRIVE_2_L),              // Y4
        .p9(J1_17),                  // A4
        .p10(DRIVE_4_L),             // Y5
        .p11(J1_15),                 // A5
        .p12(DRIVE_6_L),             // Y6
        .p13(J1_13),                 // A6
        .p14(PLUS_5V)                // VCC
    );

    // E8 ttl_7404, zone C6
    ttl_7404 E8 (
        .p1(J1_14),                  // A1
        .p2(READ_5_H),               // Y1
        .p3(J1_16),                  // A2
        .p4(READ_3_H),               // Y2
        .p5(J1_18),                  // A3
        .p6(READ_1_H),               // Y3
        .p7(GND),                    // GND
        .p8(),                       // Y4
        .p9(),                       // A4
        .p10(READ_4_H),              // Y5
        .p11(J1_15),                 // A5
        .p12(READ_2_H),              // Y6
        .p13(J1_17),                 // A6
        .p14(PLUS_5V)                // VCC
    );

    // S1 rotary_1212560, zone B3
    rotary_1212560 S1 (
        .p1(GND),                    // COMMON
        .p2(ON),                     // ON
        .p3(STNBY),                  // STNBY
        .p4(DC_OFF)                  // DC_OFF
    );

    // D1 led, zone B2
    led D1 (
        .p1(BATT),                   // ANODE
        .p2(GND)                     // CATHODE
    );

endmodule
`default_nettype wire
