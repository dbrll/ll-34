`default_nettype none
// 74S11 triple 3-input AND. TI SDLS131, page 1
(* blackbox *)
module ttl_74s11 (
    input  wire p1,                          // 1A
    input  wire p2,                          // 1B
    input  wire p3,                          // 2A
    input  wire p4,                          // 2B
    input  wire p5,                          // 2C
    output wire p6,                          // 2Y
    input  wire p7,                          // GND
    output wire p8,                          // 3Y
    input  wire p9,                          // 3A
    input  wire p10,                         // 3B
    input  wire p11,                         // 3C
    output wire p12,                         // 1Y
    input  wire p13,                         // 1C
    input  wire p14                          // VCC
);
endmodule
`default_nettype wire
