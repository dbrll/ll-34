`default_nettype none
// 7430 8-input NAND. TI SDLS099, page 1
(* blackbox *)
module ttl_7430 (
    input  wire p1,                          // A
    input  wire p2,                          // B
    input  wire p3,                          // C
    input  wire p4,                          // D
    input  wire p5,                          // E
    input  wire p6,                          // F
    input  wire p7,                          // GND
    output wire p8,                          // Y
    inout  wire p9,                          // NC
    inout  wire p10,                         // NC
    input  wire p11,                         // G
    input  wire p12,                         // H
    inout  wire p13,                         // NC
    input  wire p14                          // VCC
);
endmodule
`default_nettype wire
