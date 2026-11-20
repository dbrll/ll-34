`default_nettype none
// 74298 quad 2-input multiplexer with storage. TI SDLS098, page 1
(* blackbox *)
module ttl_74298 (
    input  wire p1,                          // B2
    input  wire p2,                          // A2
    input  wire p3,                          // A1
    input  wire p4,                          // B1
    input  wire p5,                          // C2
    input  wire p6,                          // D2
    input  wire p7,                          // D1
    input  wire p8,                          // GND
    input  wire p9,                          // C1
    input  wire p10,                         // WS
    input  wire p11,                         // CLK
    output wire p12,                         // QD
    output wire p13,                         // QC
    output wire p14,                         // QB
    output wire p15,                         // QA
    input  wire p16                          // VCC
);
endmodule
`default_nettype wire
