`default_nettype none
// 7473 dual JK flip-flop with clear. National TTL Databook 1976, 1-20
(* blackbox *)
module ttl_7473 (
    input  wire p1,                          // CLK1
    input  wire p2,                          // CLR1_N
    input  wire p3,                          // K1
    input  wire p4,                          // VCC
    input  wire p5,                          // CLK2
    input  wire p6,                          // CLR2_N
    input  wire p7,                          // J2
    output wire p8,                          // Q2_N
    output wire p9,                          // Q2
    input  wire p10,                         // K2
    input  wire p11,                         // GND
    output wire p12,                         // Q1
    output wire p13,                         // Q1_N
    input  wire p14                          // J1
);
endmodule
`default_nettype wire
