`default_nettype none
// DM74S471 256 x 8 PROM, three-state. National Memory Databook 1976, 5-3
(* blackbox *)
module prom_74s471 (
    input  wire p1,                          // A0
    input  wire p2,                          // A1
    input  wire p3,                          // A2
    input  wire p4,                          // A3
    input  wire p5,                          // A4
    (* three_state *) output wire p6,        // OUT1
    (* three_state *) output wire p7,        // OUT2
    (* three_state *) output wire p8,        // OUT3
    (* three_state *) output wire p9,        // OUT4
    input  wire p10,                         // GND
    (* three_state *) output wire p11,       // OUT5
    (* three_state *) output wire p12,       // OUT6
    (* three_state *) output wire p13,       // OUT7
    (* three_state *) output wire p14,       // OUT8
    input  wire p15,                         // EN1_N
    input  wire p16,                         // EN2_N
    input  wire p17,                         // A5
    input  wire p18,                         // A6
    input  wire p19,                         // A7
    input  wire p20                          // VCC
);
endmodule
`default_nettype wire
