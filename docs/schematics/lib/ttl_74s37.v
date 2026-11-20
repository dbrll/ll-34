`default_nettype none
// 74S37 quad 2-input NAND buffer. TI SDLS103, page 1
(* blackbox *)
module ttl_74s37 (
    input  wire p1,                          // A1
    input  wire p2,                          // B1
    output wire p3,                          // Y1
    input  wire p4,                          // A2
    input  wire p5,                          // B2
    output wire p6,                          // Y2
    input  wire p7,                          // GND
    output wire p8,                          // Y3
    input  wire p9,                          // A3
    input  wire p10,                         // B3
    output wire p11,                         // Y4
    input  wire p12,                         // A4
    input  wire p13,                         // B4
    input  wire p14                          // VCC
);
endmodule
`default_nettype wire
