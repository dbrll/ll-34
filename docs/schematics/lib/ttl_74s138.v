`default_nettype none
// 74S138 3-line to 8-line decoder. National TTL Databook 1976, 2-46
(* blackbox *)
module ttl_74s138 (
    input  wire p1,                          // A
    input  wire p2,                          // B
    input  wire p3,                          // C
    input  wire p4,                          // G2A_N
    input  wire p5,                          // G2B_N
    input  wire p6,                          // G1
    output wire p7,                          // Y7_N
    input  wire p8,                          // GND
    output wire p9,                          // Y6_N
    output wire p10,                         // Y5_N
    output wire p11,                         // Y4_N
    output wire p12,                         // Y3_N
    output wire p13,                         // Y2_N
    output wire p14,                         // Y1_N
    output wire p15,                         // Y0_N
    input  wire p16                          // VCC
);
endmodule
`default_nettype wire
