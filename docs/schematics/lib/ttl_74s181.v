`default_nettype none
// 74S181 4-bit ALU. TI SDLS136, page 1
(* blackbox *)
module ttl_74s181 (
    input  wire p1,                          // B0_N
    input  wire p2,                          // A0_N
    input  wire p3,                          // S3
    input  wire p4,                          // S2
    input  wire p5,                          // S1
    input  wire p6,                          // S0
    input  wire p7,                          // CN
    input  wire p8,                          // M
    output wire p9,                          // F0_N
    output wire p10,                         // F1_N
    output wire p11,                         // F2_N
    input  wire p12,                         // GND
    output wire p13,                         // F3_N
    (* open_collector *) output wire p14,    // A_EQ_B
    output wire p15,                         // P_N
    output wire p16,                         // CN4
    output wire p17,                         // G_N
    input  wire p18,                         // B3_N
    input  wire p19,                         // A3_N
    input  wire p20,                         // B2_N
    input  wire p21,                         // A2_N
    input  wire p22,                         // B1_N
    input  wire p23,                         // A1_N
    input  wire p24                          // VCC
);
endmodule
`default_nettype wire
