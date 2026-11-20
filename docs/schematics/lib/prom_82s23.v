`default_nettype none
// 82S23 32 x 8 PROM, open collector. Signetics Bipolar Memories 1975, 22
(* blackbox *)
module prom_82s23 (
    (* open_collector *) output wire p1,     // O1
    (* open_collector *) output wire p2,     // O2
    (* open_collector *) output wire p3,     // O3
    (* open_collector *) output wire p4,     // O4
    (* open_collector *) output wire p5,     // O5
    (* open_collector *) output wire p6,     // O6
    (* open_collector *) output wire p7,     // O7
    input  wire p8,                          // GND
    (* open_collector *) output wire p9,     // O8
    input  wire p10,                         // A0
    input  wire p11,                         // A1
    input  wire p12,                         // A2
    input  wire p13,                         // A3
    input  wire p14,                         // A4
    input  wire p15,                         // CE_N
    input  wire p16                          // VCC
);
endmodule
`default_nettype wire
