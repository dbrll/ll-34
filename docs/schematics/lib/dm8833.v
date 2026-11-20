`default_nettype none
// DM8833 quad three-state bus transceiver. National DS8833 datasheet, TL/F/5808
(* blackbox *)
module dm8833 (
    (* three_state *) inout  wire p1,        // BUS_A
    input  wire p2,                          // IN_A
    (* three_state *) output wire p3,        // OUT_A
    (* three_state *) inout  wire p4,        // BUS_B
    input  wire p5,                          // IN_B
    (* three_state *) output wire p6,        // OUT_B
    input  wire p7,                          // RECEIVER_DISABLE
    input  wire p8,                          // GND
    input  wire p9,                          // DRIVER_DISABLE
    (* three_state *) output wire p10,       // OUT_C
    input  wire p11,                         // IN_C
    (* three_state *) inout  wire p12,       // BUS_C
    (* three_state *) output wire p13,       // OUT_D
    input  wire p14,                         // IN_D
    (* three_state *) inout  wire p15,       // BUS_D
    input  wire p16                          // VCC
);
endmodule
`default_nettype wire
