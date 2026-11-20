`default_nettype none
// 82S129 256 x 4 PROM, three-state. Signetics Bipolar Memories 1975, 51
(* blackbox *)
module prom_82s129 (
    input  wire p1,                          // A6
    input  wire p2,                          // A5
    input  wire p3,                          // A4
    input  wire p4,                          // A3
    input  wire p5,                          // A0
    input  wire p6,                          // A1
    input  wire p7,                          // A2
    input  wire p8,                          // GND
    (* three_state *) output wire p9,        // O4
    (* three_state *) output wire p10,       // O3
    (* three_state *) output wire p11,       // O2
    (* three_state *) output wire p12,       // O1
    input  wire p13,                         // CE1_N
    input  wire p14,                         // CE2_N
    input  wire p15,                         // A7
    input  wire p16                          // VCC
);
endmodule
`default_nettype wire
