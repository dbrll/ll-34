`default_nettype none
// 8881 quad 2-input NAND, open collector. Signetics SSI 8400 8800 1972, 2-44
(* blackbox *)
module signetics_8881 (
    (* open_collector *) output wire p1,     // Y1
    input  wire p2,                          // A1
    input  wire p3,                          // B1
    (* open_collector *) output wire p4,     // Y2
    input  wire p5,                          // A2
    input  wire p6,                          // B2
    input  wire p7,                          // GND
    input  wire p8,                          // A3
    input  wire p9,                          // B3
    (* open_collector *) output wire p10,    // Y3
    input  wire p11,                         // B4
    input  wire p12,                         // A4
    (* open_collector *) output wire p13,    // Y4
    input  wire p14                          // VCC
);
endmodule
`default_nettype wire
