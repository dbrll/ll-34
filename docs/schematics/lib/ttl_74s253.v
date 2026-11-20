`default_nettype none
// 74S253 dual 4-line to 1-line multiplexer, three-state. TI SDLS147, page 1
(* blackbox *)
module ttl_74s253 (
    input  wire p1,                          // 1G_N
    input  wire p2,                          // B
    input  wire p3,                          // 1C3
    input  wire p4,                          // 1C2
    input  wire p5,                          // 1C1
    input  wire p6,                          // 1C0
    (* three_state *) output wire p7,        // 1Y
    input  wire p8,                          // GND
    (* three_state *) output wire p9,        // 2Y
    input  wire p10,                         // 2C0
    input  wire p11,                         // 2C1
    input  wire p12,                         // 2C2
    input  wire p13,                         // 2C3
    input  wire p14,                         // A
    input  wire p15,                         // 2G_N
    input  wire p16                          // VCC
);
endmodule
`default_nettype wire
