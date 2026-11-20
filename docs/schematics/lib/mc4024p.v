`default_nettype none
// MC4024P dual voltage controlled multivibrator. Motorola, 6-14
(* blackbox *)
module mc4024p (
    input  wire p1,                          // VCM1
    input  wire p2,                          // VCX1
    inout  wire p3,                          // CX1_A
    inout  wire p4,                          // CX1_B
    input  wire p5,                          // GND5
    output wire p6,                          // OUT1
    input  wire p7,                          // GND7
    output wire p8,                          // OUT2
    input  wire p9,                          // GND9
    inout  wire p10,                         // CX2_A
    inout  wire p11,                         // CX2_B
    input  wire p12,                         // VCX2
    input  wire p13,                         // VCM2
    input  wire p14                          // BUFFER_VCC
);
endmodule
`default_nettype wire
