`default_nettype none
// 74174 hex D flip-flop with clear. TI SDLS068A, page 1
(* blackbox *)
module ttl_74174 (
    input  wire p1,                          // CLR_N
    output wire p2,                          // 1Q
    input  wire p3,                          // 1D
    input  wire p4,                          // 2D
    output wire p5,                          // 2Q
    input  wire p6,                          // 3D
    output wire p7,                          // 3Q
    input  wire p8,                          // GND
    input  wire p9,                          // CLK
    output wire p10,                         // 4Q
    input  wire p11,                         // 4D
    output wire p12,                         // 5Q
    input  wire p13,                         // 5D
    input  wire p14,                         // 6D
    output wire p15,                         // 6Q
    input  wire p16                          // VCC
);
endmodule
`default_nettype wire
