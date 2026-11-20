`default_nettype none
// 74123 dual retriggerable monostable. National TTL Databook 1976, 1-26
(* blackbox *)
module ttl_74123 (
    input  wire p1,                          // A1_N
    input  wire p2,                          // B1
    input  wire p3,                          // CLR1_N
    output wire p4,                          // Q1_N
    output wire p5,                          // Q2
    inout  wire p6,                          // CEXT2
    inout  wire p7,                          // REXT_CEXT2
    input  wire p8,                          // GND
    input  wire p9,                          // A2_N
    input  wire p10,                         // B2
    input  wire p11,                         // CLR2_N
    output wire p12,                         // Q2_N
    output wire p13,                         // Q1
    inout  wire p14,                         // CEXT1
    inout  wire p15,                         // REXT_CEXT1
    input  wire p16                          // VCC
);
endmodule
`default_nettype wire
