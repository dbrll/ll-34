`default_nettype none
// 7442 BCD to decimal decoder. TI SDLS109, page 1
(* blackbox *)
module ttl_7442 (
    output wire p1,                          // Y0_N
    output wire p2,                          // Y1_N
    output wire p3,                          // Y2_N
    output wire p4,                          // Y3_N
    output wire p5,                          // Y4_N
    output wire p6,                          // Y5_N
    output wire p7,                          // Y6_N
    input  wire p8,                          // GND
    output wire p9,                          // Y7_N
    output wire p10,                         // Y8_N
    output wire p11,                         // Y9_N
    input  wire p12,                         // D
    input  wire p13,                         // C
    input  wire p14,                         // B
    input  wire p15,                         // A
    input  wire p16                          // VCC
);
endmodule
`default_nettype wire
