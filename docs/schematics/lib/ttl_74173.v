`default_nettype none
// 74173 4-bit three-state register. TI SDLS067A, page 1
(* blackbox *)
module ttl_74173 (
    input  wire p1,                          // M
    input  wire p2,                          // N
    (* three_state *) output wire p3,        // Q1
    (* three_state *) output wire p4,        // Q2
    (* three_state *) output wire p5,        // Q3
    (* three_state *) output wire p6,        // Q4
    input  wire p7,                          // CLK
    input  wire p8,                          // GND
    input  wire p9,                          // G1_N
    input  wire p10,                         // G2_N
    input  wire p11,                         // D4
    input  wire p12,                         // D3
    input  wire p13,                         // D2
    input  wire p14,                         // D1
    input  wire p15,                         // CLR
    input  wire p16                          // VCC
);
endmodule
`default_nettype wire
