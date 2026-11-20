`default_nettype none
// DM8093 quad three-state buffer. National TTL Databook 1976, 3-5
(* blackbox *)
module dm8093 (
    input  wire p1,                          // C1
    input  wire p2,                          // A1
    (* three_state *) output wire p3,        // Y1
    input  wire p4,                          // C2
    input  wire p5,                          // A2
    (* three_state *) output wire p6,        // Y2
    input  wire p7,                          // GND
    (* three_state *) output wire p8,        // Y3
    input  wire p9,                          // A3
    input  wire p10,                         // C3
    (* three_state *) output wire p11,       // Y4
    input  wire p12,                         // A4
    input  wire p13,                         // C4
    input  wire p14                          // VCC
);
endmodule
`default_nettype wire
