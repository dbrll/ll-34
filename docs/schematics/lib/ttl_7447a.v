`default_nettype none
// 7447A BCD to seven segment decoder, open collector. TI SDLS111, page 1
(* blackbox *)
module ttl_7447a (
    input  wire p1,                          // B
    input  wire p2,                          // C
    input  wire p3,                          // LT_N
    (* open_collector *) inout  wire p4,     // BI_N_RBO_N
    input  wire p5,                          // RBI_N
    input  wire p6,                          // D
    input  wire p7,                          // A
    input  wire p8,                          // GND
    (* open_collector *) output wire p9,     // E_N
    (* open_collector *) output wire p10,    // D_N
    (* open_collector *) output wire p11,    // C_N
    (* open_collector *) output wire p12,    // B_N
    (* open_collector *) output wire p13,    // A_N
    (* open_collector *) output wire p14,    // G_N
    (* open_collector *) output wire p15,    // F_N
    input  wire p16                          // VCC
);
endmodule
`default_nettype wire
