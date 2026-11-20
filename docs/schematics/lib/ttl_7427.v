`default_nettype none
// 7427 triple 3-input NOR. TI SDLS089, page 1
(* blackbox *)
module ttl_7427 (
    input  wire p1,                          // A1
    input  wire p2,                          // B1
    input  wire p3,                          // A2
    input  wire p4,                          // B2
    input  wire p5,                          // C2
    output wire p6,                          // Y2
    input  wire p7,                          // GND
    output wire p8,                          // Y3
    input  wire p9,                          // A3
    input  wire p10,                         // B3
    input  wire p11,                         // C3
    output wire p12,                         // Y1
    input  wire p13,                         // C1
    input  wire p14                          // VCC
);
endmodule
`default_nettype wire
