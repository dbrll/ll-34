`default_nettype none
// DEC 12-11858 keypad. Contacts from MP00015 sheet 2
(* blackbox *)
module keypad_1211858 (
    inout  wire p1,                          // START
    inout  wire p2,                          // HLT_SS
    inout  wire p3,                          // CONT
    inout  wire p4,                          // BOOT
    inout  wire p5,                          // KEY6
    inout  wire p6,                          // DEP
    inout  wire p7,                          // KEY3
    inout  wire p8,                          // CNTRL
    inout  wire p9,                          // KEY5
    inout  wire p10,                         // EXAM
    inout  wire p11,                         // KEY2
    inout  wire p12,                         // INIT
    inout  wire p13,                         // KEY4
    inout  wire p14,                         // KEY7
    inout  wire p15,                         // KEY1
    inout  wire p16,                         // KEY0
    inout  wire p17,                         // LAD
    inout  wire p18,                         // DIS_AD
    inout  wire p19,                         // LSR
    inout  wire p20,                         // CLR
    inout  wire p21                          // COMMON
);
endmodule
`default_nettype wire
