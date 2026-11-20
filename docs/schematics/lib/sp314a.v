`default_nettype none
// SP314A seven-input NOR. SP314 datasheet, page 1
(* blackbox *)
module sp314a (
    input  wire p1,                          // GND
    inout  wire p2,                          // NC2
    output wire p3,                          // Y
    input  wire p4,                          // A
    input  wire p5,                          // B
    input  wire p6,                          // C
    inout  wire p7,                          // NC7
    input  wire p8,                          // VCC
    input  wire p9,                          // D
    input  wire p10,                         // E
    input  wire p11,                         // F
    input  wire p12,                         // G
    inout  wire p13,                         // NC13
    inout  wire p14                          // NC14
);
endmodule
`default_nettype wire
