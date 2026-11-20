`default_nettype none
// 7493 4-bit binary counter. TI SDLS940A, page 1
(* blackbox *)
module ttl_7493 (
    input  wire p1,                          // CKB
    input  wire p2,                          // R0_1
    input  wire p3,                          // R0_2
    inout  wire p4,                          // NC4
    input  wire p5,                          // VCC
    inout  wire p6,                          // NC6
    inout  wire p7,                          // NC7
    output wire p8,                          // QC
    output wire p9,                          // QB
    input  wire p10,                         // GND
    output wire p11,                         // QD
    output wire p12,                         // QA
    inout  wire p13,                         // NC13
    input  wire p14                          // CKA
);
endmodule
`default_nettype wire
