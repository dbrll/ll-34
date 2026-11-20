`default_nettype none
// Intel 8008 8-bit microprocessor. Intel 8008 datasheet, page 3
(* blackbox *)
module intel_8008 (
    input  wire p1,                          // VDD
    (* three_state *) inout  wire p2,        // D7
    (* three_state *) inout  wire p3,        // D6
    (* three_state *) inout  wire p4,        // D5
    (* three_state *) inout  wire p5,        // D4
    (* three_state *) inout  wire p6,        // D3
    (* three_state *) inout  wire p7,        // D2
    (* three_state *) inout  wire p8,        // D1
    (* three_state *) inout  wire p9,        // D0
    input  wire p10,                         // VCC
    output wire p11,                         // S2
    output wire p12,                         // S1
    output wire p13,                         // S0
    output wire p14,                         // SYNC
    input  wire p15,                         // PHI2
    input  wire p16,                         // PHI1
    input  wire p17,                         // READY
    input  wire p18                          // INTERRUPT
);
endmodule
`default_nettype wire
