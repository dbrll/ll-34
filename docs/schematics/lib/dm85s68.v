`default_nettype none
// DM85S68 16 x 4 edge triggered register file. National Semiconductor, 5-69
(* blackbox *)
module dm85s68 (
    input  wire p1,                          // D2
    input  wire p2,                          // D1
    input  wire p3,                          // A0
    input  wire p4,                          // A2
    input  wire p5,                          // A3
    input  wire p6,                          // A1
    (* three_state *) output wire p7,        // O1
    (* three_state *) output wire p8,        // O2
    input  wire p9,                          // GND
    (* three_state *) output wire p10,       // O3
    (* three_state *) output wire p11,       // O4
    input  wire p12,                         // OD
    input  wire p13,                         // OS_N
    input  wire p14,                         // CLK
    input  wire p15,                         // WE_N
    input  wire p16,                         // D4
    input  wire p17,                         // D3
    input  wire p18                          // VCC
);
endmodule
`default_nettype wire
