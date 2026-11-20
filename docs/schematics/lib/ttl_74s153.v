`default_nettype none
// 74S153 dual 4-line to 1-line multiplexer. National TTL Databook 1976, 2-57
(* blackbox *)
module ttl_74s153 (
    input  wire p1,                          // G1_N
    input  wire p2,                          // B
    input  wire p3,                          // C3_1
    input  wire p4,                          // C2_1
    input  wire p5,                          // C1_1
    input  wire p6,                          // C0_1
    output wire p7,                          // Y1
    input  wire p8,                          // GND
    output wire p9,                          // Y2
    input  wire p10,                         // C0_2
    input  wire p11,                         // C1_2
    input  wire p12,                         // C2_2
    input  wire p13,                         // C3_2
    input  wire p14,                         // A
    input  wire p15,                         // G2_N
    input  wire p16                          // VCC
);
endmodule
`default_nettype wire
