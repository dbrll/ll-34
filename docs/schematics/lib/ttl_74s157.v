`default_nettype none
// 74S157 quad 2-line to 1-line multiplexer. National TTL Databook 1976, 2-66
(* blackbox *)
module ttl_74s157 (
    input  wire p1,                          // S
    input  wire p2,                          // A1
    input  wire p3,                          // B1
    output wire p4,                          // Y1
    input  wire p5,                          // A2
    input  wire p6,                          // B2
    output wire p7,                          // Y2
    input  wire p8,                          // GND
    output wire p9,                          // Y3
    input  wire p10,                         // B3
    input  wire p11,                         // A3
    output wire p12,                         // Y4
    input  wire p13,                         // B4
    input  wire p14,                         // A4
    input  wire p15,                         // G_N
    input  wire p16                          // VCC
);
endmodule
`default_nettype wire
