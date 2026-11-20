`default_nettype none
// 82S131 512 x 4 PROM, three-state. Signetics 82S130/82S131 datasheet, 56
(* blackbox *)
module prom_82s131 (
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
    input  wire p13,                         // CE_N
    input  wire p14,                         // A8
    input  wire p15,                         // A7
    input  wire p16                          // VCC
);
endmodule
`default_nettype wire
