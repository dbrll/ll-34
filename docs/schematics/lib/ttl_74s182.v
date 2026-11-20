`default_nettype none
// 74S182 look-ahead carry generator. TI SDLS206, page 1
(* blackbox *)
module ttl_74s182 (
    input  wire p1,                          // G1_N
    input  wire p2,                          // P1_N
    input  wire p3,                          // G0_N
    input  wire p4,                          // P0_N
    input  wire p5,                          // G3_N
    input  wire p6,                          // P3_N
    output wire p7,                          // P_N
    input  wire p8,                          // GND
    output wire p9,                          // CNZ
    output wire p10,                         // G_N
    output wire p11,                         // CNY
    output wire p12,                         // CNX
    input  wire p13,                         // CN
    input  wire p14,                         // G2_N
    input  wire p15,                         // P2_N
    input  wire p16                          // VCC
);
endmodule
`default_nettype wire
