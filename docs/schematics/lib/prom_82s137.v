`default_nettype none
// 82S137 1024 x 4 PROM, three-state. Signetics 82S136/82S137 datasheet, 120
(* blackbox *)
module prom_82s137 (
    input  wire p1,                          // A6
    input  wire p2,                          // A5
    input  wire p3,                          // A4
    input  wire p4,                          // A3
    input  wire p5,                          // A0
    input  wire p6,                          // A1
    input  wire p7,                          // A2
    input  wire p8,                          // CE1_N
    input  wire p9,                          // GND
    input  wire p10,                         // CE2_N
    (* three_state *) output wire p11,       // O4
    (* three_state *) output wire p12,       // O3
    (* three_state *) output wire p13,       // O2
    (* three_state *) output wire p14,       // O1
    input  wire p15,                         // A9
    input  wire p16,                         // A8
    input  wire p17,                         // A7
    input  wire p18                          // VCC
);
endmodule
`default_nettype wire
