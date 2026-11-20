`default_nettype none
// 74157 quad 2-line to 1-line multiplexer. TI SDLS058, page 1
(* blackbox *)
module ttl_74157 (
    input  wire p1,                          // SELECT
    input  wire p2,                          // 1A
    input  wire p3,                          // 1B
    output wire p4,                          // 1Y
    input  wire p5,                          // 2A
    input  wire p6,                          // 2B
    output wire p7,                          // 2Y
    input  wire p8,                          // GND
    output wire p9,                          // 3Y
    input  wire p10,                         // 3B
    input  wire p11,                         // 3A
    output wire p12,                         // 4Y
    input  wire p13,                         // 4B
    input  wire p14,                         // 4A
    input  wire p15,                         // G_N
    input  wire p16                          // VCC
);
endmodule
`default_nettype wire
