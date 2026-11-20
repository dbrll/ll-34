`default_nettype none
// 74S174 hex D flip-flop with clear. TI SDLS068A, page 1
(* blackbox *)
module ttl_74s174 (
    input  wire p1,                          // CLR_N
    output wire p2,                          // Q1
    input  wire p3,                          // D1
    input  wire p4,                          // D2
    output wire p5,                          // Q2
    input  wire p6,                          // D3
    output wire p7,                          // Q3
    input  wire p8,                          // GND
    input  wire p9,                          // CLK
    output wire p10,                         // Q4
    input  wire p11,                         // D4
    output wire p12,                         // Q5
    input  wire p13,                         // D5
    input  wire p14,                         // D6
    output wire p15,                         // Q6
    input  wire p16                          // VCC
);
endmodule
`default_nettype wire
