`default_nettype none
// 7474 dual D flip-flop with preset and clear. TI SDLS119, page 1
(* blackbox *)
module ttl_7474 (
    input  wire p1,                          // CLR1_N
    input  wire p2,                          // D1
    input  wire p3,                          // CLK1
    input  wire p4,                          // PRE1_N
    output wire p5,                          // Q1
    output wire p6,                          // Q1_N
    input  wire p7,                          // GND
    output wire p8,                          // Q2_N
    output wire p9,                          // Q2
    input  wire p10,                         // PRE2_N
    input  wire p11,                         // CLK2
    input  wire p12,                         // D2
    input  wire p13,                         // CLR2_N
    input  wire p14                          // VCC
);
endmodule
`default_nettype wire
