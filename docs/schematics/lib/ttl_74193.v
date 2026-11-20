`default_nettype none
// 74193 synchronous up/down binary counter. National TTL Databook 1976, 2-133
(* blackbox *)
module ttl_74193 (
    input  wire p1,                          // B
    output wire p2,                          // QB
    output wire p3,                          // QA
    input  wire p4,                          // DOWN
    input  wire p5,                          // UP
    output wire p6,                          // QC
    output wire p7,                          // QD
    input  wire p8,                          // GND
    input  wire p9,                          // D
    input  wire p10,                         // C
    input  wire p11,                         // LOAD_N
    output wire p12,                         // CARRY_N
    output wire p13,                         // BORROW_N
    input  wire p14,                         // CLEAR
    input  wire p15,                         // A
    input  wire p16                          // VCC
);
endmodule
`default_nettype wire
