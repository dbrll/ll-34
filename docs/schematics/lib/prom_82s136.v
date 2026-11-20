`default_nettype none
// 82S136 1024 x 4 PROM, open collector. Signetics Bipolar and MOS Memory 1977, 122
(* blackbox *)
module prom_82s136 (
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
    (* open_collector *) output wire p11,    // O4
    (* open_collector *) output wire p12,    // O3
    (* open_collector *) output wire p13,    // O2
    (* open_collector *) output wire p14,    // O1
    input  wire p15,                         // A9
    input  wire p16,                         // A8
    input  wire p17,                         // A7
    input  wire p18                          // VCC
);
endmodule
`default_nettype wire
