`default_nettype none
// 74S08 quad 2-input AND. TI SDLS033, page 1
(* blackbox *)
module ttl_74s08 (
    input  wire p1,                          // 1A
    input  wire p2,                          // 1B
    output wire p3,                          // 1Y
    input  wire p4,                          // 2A
    input  wire p5,                          // 2B
    output wire p6,                          // 2Y
    input  wire p7,                          // GND
    output wire p8,                          // 3Y
    input  wire p9,                          // 3A
    input  wire p10,                         // 3B
    output wire p11,                         // 4Y
    input  wire p12,                         // 4A
    input  wire p13,                         // 4B
    input  wire p14                          // VCC
);
endmodule
`default_nettype wire
