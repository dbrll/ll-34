`default_nettype none
// 74194 4-bit bidirectional shift register. TI SDLS075, page 1
(* blackbox *)
module ttl_74194 (
    input  wire p1,                          // CLR_N
    input  wire p2,                          // SR_SER
    input  wire p3,                          // A
    input  wire p4,                          // B
    input  wire p5,                          // C
    input  wire p6,                          // D
    input  wire p7,                          // SL_SER
    input  wire p8,                          // GND
    input  wire p9,                          // S0
    input  wire p10,                         // S1
    input  wire p11,                         // CLK
    output wire p12,                         // QD
    output wire p13,                         // QC
    output wire p14,                         // QB
    output wire p15,                         // QA
    input  wire p16                          // VCC
);
endmodule
`default_nettype wire
