`default_nettype none
// 74S02 quad 2-input NOR. TI SDLS027, page 1
(* blackbox *)
module ttl_74s02 (
    output wire p1,                          // Y1
    input  wire p2,                          // A1
    input  wire p3,                          // B1
    output wire p4,                          // Y2
    input  wire p5,                          // A2
    input  wire p6,                          // B2
    input  wire p7,                          // GND
    input  wire p8,                          // A3
    input  wire p9,                          // B3
    output wire p10,                         // Y3
    input  wire p11,                         // A4
    input  wire p12,                         // B4
    output wire p13,                         // Y4
    input  wire p14                          // VCC
);
endmodule
`default_nettype wire
