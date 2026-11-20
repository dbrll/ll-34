`default_nettype none
// 74S175 quad D flip-flop with clear. TI SDLS068A, page 1
(* blackbox *)
module ttl_74s175 (
    input  wire p1,                          // CLR_N
    output wire p2,                          // Q1
    output wire p3,                          // Q1_N
    input  wire p4,                          // D1
    input  wire p5,                          // D2
    output wire p6,                          // Q2_N
    output wire p7,                          // Q2
    input  wire p8,                          // GND
    input  wire p9,                          // CLK
    output wire p10,                         // Q3
    output wire p11,                         // Q3_N
    input  wire p12,                         // D3
    input  wire p13,                         // D4
    output wire p14,                         // Q4_N
    output wire p15,                         // Q4
    input  wire p16                          // VCC
);
endmodule
`default_nettype wire
