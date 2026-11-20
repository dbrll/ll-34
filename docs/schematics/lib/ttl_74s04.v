`default_nettype none
// 74S04 hex inverter. TI SDLS029C, page 1
(* blackbox *)
module ttl_74s04 (
    input  wire p1,                          // A1
    output wire p2,                          // Y1
    input  wire p3,                          // A2
    output wire p4,                          // Y2
    input  wire p5,                          // A3
    output wire p6,                          // Y3
    input  wire p7,                          // GND
    output wire p8,                          // Y4
    input  wire p9,                          // A4
    output wire p10,                         // Y5
    input  wire p11,                         // A5
    output wire p12,                         // Y6
    input  wire p13,                         // A6
    input  wire p14                          // VCC
);
endmodule
`default_nettype wire
