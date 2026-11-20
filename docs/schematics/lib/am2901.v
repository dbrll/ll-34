`default_nettype none
// Am2901 four-bit microprocessor slice. AMD Am2900 Family Data Book 1978, 2-9
(* blackbox *)
module am2901 (
    input  wire p1,                          // A3
    input  wire p2,                          // A2
    input  wire p3,                          // A1
    input  wire p4,                          // A0
    input  wire p5,                          // I6
    input  wire p6,                          // I8
    input  wire p7,                          // I7
    (* three_state *) inout wire p8,         // RAM3
    (* three_state *) inout wire p9,         // RAM0
    input  wire p10,                         // VCC
    (* open_collector *) output wire p11,    // F_ZERO
    input  wire p12,                         // I0
    input  wire p13,                         // I1
    input  wire p14,                         // I2
    input  wire p15,                         // CP
    (* three_state *) inout wire p16,        // Q3
    input  wire p17,                         // B0
    input  wire p18,                         // B1
    input  wire p19,                         // B2
    input  wire p20,                         // B3
    (* three_state *) inout wire p21,        // Q0
    input  wire p22,                         // D3
    input  wire p23,                         // D2
    input  wire p24,                         // D1
    input  wire p25,                         // D0
    input  wire p26,                         // I3
    input  wire p27,                         // I5
    input  wire p28,                         // I4
    input  wire p29,                         // CN
    input  wire p30,                         // GND
    output wire p31,                         // F3
    output wire p32,                         // G_N
    output wire p33,                         // CN4
    output wire p34,                         // OVR
    output wire p35,                         // P_N
    (* three_state *) output wire p36,       // Y0
    (* three_state *) output wire p37,       // Y1
    (* three_state *) output wire p38,       // Y2
    (* three_state *) output wire p39,       // Y3
    input  wire p40                          // OE_N
);
endmodule
`default_nettype wire
