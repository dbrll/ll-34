`default_nettype none
// 74S133 13-input NAND. National TTL Databook 1976, 1-28
(* blackbox *)
module ttl_74s133 (
    input  wire p1,                          // A
    input  wire p2,                          // B
    input  wire p3,                          // C
    input  wire p4,                          // D
    input  wire p5,                          // E
    input  wire p6,                          // F
    input  wire p7,                          // G
    input  wire p8,                          // GND
    output wire p9,                          // Y
    input  wire p10,                         // H
    input  wire p11,                         // I
    input  wire p12,                         // J
    input  wire p13,                         // K
    input  wire p14,                         // L
    input  wire p15,                         // M
    input  wire p16                          // VCC
);
endmodule
`default_nettype wire
