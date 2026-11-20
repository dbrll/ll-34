`default_nettype none
// DEC 16-12463 delay line, 150 ns in 15 ns taps. Pinout from MP00190 sheet K1-5
(* blackbox *)
module dec_1612463 (
    inout  wire p1,                          // DELAY_INPUT
    inout  wire p2,                          // TAP_15_NS
    inout  wire p3,                          // TAP_45_NS
    inout  wire p4,                          // TAP_75_NS
    inout  wire p5,                          // TAP_105_NS
    inout  wire p6,                          // TAP_135_NS
    inout  wire p7,                          // TAP_150_NS
    inout  wire p8,                          // NOT_SHOWN
    inout  wire p9,                          // TAP_120_NS
    inout  wire p10,                         // TAP_90_NS
    inout  wire p11,                         // NOT_SHOWN
    inout  wire p12,                         // TAP_60_NS
    inout  wire p13,                         // TAP_30_NS
    input  wire p14                          // GND
);
endmodule
`default_nettype wire
