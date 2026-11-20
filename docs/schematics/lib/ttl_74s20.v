`default_nettype none
// 74S20 dual 4-input NAND. National TTL Databook 1976, 1-7
(* blackbox *)
module ttl_74s20 (
    input  wire p1,                          // A1
    input  wire p2,                          // B1
    inout  wire p3,                          // NC
    input  wire p4,                          // C1
    input  wire p5,                          // D1
    output wire p6,                          // Y1
    input  wire p7,                          // GND
    output wire p8,                          // Y2
    input  wire p9,                          // A2
    input  wire p10,                         // B2
    inout  wire p11,                         // NC
    input  wire p12,                         // C2
    input  wire p13,                         // D2
    input  wire p14                          // VCC
);
endmodule
`default_nettype wire
