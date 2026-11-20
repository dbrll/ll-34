`default_nettype none
// 74154 4-line to 16-line decoder. National TTL Databook 1976, 2-60
(* blackbox *)
module ttl_74154 (
    output wire p1,                          // Y0_N
    output wire p2,                          // Y1_N
    output wire p3,                          // Y2_N
    output wire p4,                          // Y3_N
    output wire p5,                          // Y4_N
    output wire p6,                          // Y5_N
    output wire p7,                          // Y6_N
    output wire p8,                          // Y7_N
    output wire p9,                          // Y8_N
    output wire p10,                         // Y9_N
    output wire p11,                         // Y10_N
    input  wire p12,                         // GND
    output wire p13,                         // Y11_N
    output wire p14,                         // Y12_N
    output wire p15,                         // Y13_N
    output wire p16,                         // Y14_N
    output wire p17,                         // Y15_N
    input  wire p18,                         // G1_N
    input  wire p19,                         // G2_N
    input  wire p20,                         // D
    input  wire p21,                         // C
    input  wire p22,                         // B
    input  wire p23,                         // A
    input  wire p24                          // VCC
);
endmodule
`default_nettype wire
