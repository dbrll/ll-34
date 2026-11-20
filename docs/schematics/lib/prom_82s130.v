`default_nettype none
// 82S130 512 x 4 PROM, open collector. Signetics 82S130/82S131 datasheet, 56
(* blackbox *)
module prom_82s130 (
    input  wire p1,                          // A6
    input  wire p2,                          // A5
    input  wire p3,                          // A4
    input  wire p4,                          // A3
    input  wire p5,                          // A0
    input  wire p6,                          // A1
    input  wire p7,                          // A2
    input  wire p8,                          // GND
    (* open_collector *) output wire p9,     // O4
    (* open_collector *) output wire p10,    // O3
    (* open_collector *) output wire p11,    // O2
    (* open_collector *) output wire p12,    // O1
    input  wire p13,                         // CE_N
    input  wire p14,                         // A8
    input  wire p15,                         // A7
    input  wire p16                          // VCC
);
endmodule
`default_nettype wire
