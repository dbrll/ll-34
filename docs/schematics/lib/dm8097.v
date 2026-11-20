`default_nettype none
// DM8097 hex buffer, three-state. National TTL Databook 1976, 3-7
(* blackbox *)
module dm8097 (
    input  wire p1,                          // G1_N (A1 through A4)
    input  wire p2,                          // A1
    (* three_state *) output wire p3,        // Y1
    input  wire p4,                          // A2
    (* three_state *) output wire p5,        // Y2
    input  wire p6,                          // A3
    (* three_state *) output wire p7,        // Y3
    input  wire p8,                          // GND
    (* three_state *) output wire p9,        // Y4
    input  wire p10,                         // A4
    (* three_state *) output wire p11,       // Y5
    input  wire p12,                         // A5
    (* three_state *) output wire p13,       // Y6
    input  wire p14,                         // A6
    input  wire p15,                         // G2_N (A5 and A6)
    input  wire p16                          // VCC
);
endmodule
`default_nettype wire
