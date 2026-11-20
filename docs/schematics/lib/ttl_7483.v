`default_nettype none
// 7483 4-bit binary full adder. National TTL Databook 1976, 2-17
(* blackbox *)
module ttl_7483 (
    input  wire p1,                          // A4
    output wire p2,                          // SUM3
    input  wire p3,                          // A3
    input  wire p4,                          // B3
    input  wire p5,                          // VCC
    output wire p6,                          // SUM2
    input  wire p7,                          // B2
    input  wire p8,                          // A2
    output wire p9,                          // SUM1
    input  wire p10,                         // A1
    input  wire p11,                         // B1
    input  wire p12,                         // GND
    input  wire p13,                         // C0
    output wire p14,                         // C4
    output wire p15,                         // SUM4
    input  wire p16                          // B4
);
endmodule
`default_nettype wire
