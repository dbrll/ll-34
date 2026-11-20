`default_nettype none
// 7485 4-bit magnitude comparator. TI SDLS123, page 1
(* blackbox *)
module ttl_7485 (
    input  wire p1,                          // B3
    input  wire p2,                          // LT_IN
    input  wire p3,                          // EQ_IN
    input  wire p4,                          // GT_IN
    output wire p5,                          // GT_OUT
    output wire p6,                          // EQ_OUT
    output wire p7,                          // LT_OUT
    input  wire p8,                          // GND
    input  wire p9,                          // B0
    input  wire p10,                         // A0
    input  wire p11,                         // B1
    input  wire p12,                         // A1
    input  wire p13,                         // A2
    input  wire p14,                         // B2
    input  wire p15,                         // A3
    input  wire p16                          // VCC
);
endmodule
`default_nettype wire
