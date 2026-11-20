`default_nettype none
// 74S03 quad 2-input NAND, open collector. National TTL Databook 1976, 1-2
(* blackbox *)
module ttl_74s03 (
    input  wire p1,                          // A1
    input  wire p2,                          // B1
    (* open_collector *) output wire p3,     // Y1
    input  wire p4,                          // A2
    input  wire p5,                          // B2
    (* open_collector *) output wire p6,     // Y2
    input  wire p7,                          // GND
    (* open_collector *) output wire p8,     // Y3
    input  wire p9,                          // A3
    input  wire p10,                         // B3
    (* open_collector *) output wire p11,    // Y4
    input  wire p12,                         // A4
    input  wire p13,                         // B4
    input  wire p14                          // VCC
);
endmodule
`default_nettype wire
