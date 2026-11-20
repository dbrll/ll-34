`default_nettype none
// 8815 dual 4-input NOR driver. Signetics SSI 8400 8800 1972, 2-24
(* blackbox *)
module signetics_8815 (
    input  wire p1,                          // A1
    input  wire p2,                          // B1
    inout  wire p3,                          // NC
    input  wire p4,                          // C1
    input  wire p5,                          // D1
    output wire p6,                          // Y1
    input  wire p7,                          // GND
    output wire p8,                          // Y2
    input  wire p9,                          // A2
    input  wire p10,                         // B2
    inout  wire p11,                         // NC
    input  wire p12,                         // C2
    input  wire p13,                         // D2
    input  wire p14                          // VCC
);
endmodule
`default_nettype wire
