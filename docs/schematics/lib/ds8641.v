`default_nettype none
// DS8641 quad unified bus transceiver. National Interface Databook 1983, 2-70
(* blackbox *)
module ds8641 (
    (* open_collector *) inout  wire p1,     // BUS3
    input  wire p2,                          // IN3
    output wire p3,                          // OUT3
    (* open_collector *) inout  wire p4,     // BUS4
    input  wire p5,                          // IN4
    output wire p6,                          // OUT4
    input  wire p7,                          // DISABLE_B
    input  wire p8,                          // GND
    input  wire p9,                          // DISABLE_A
    output wire p10,                         // OUT2
    input  wire p11,                         // IN2
    (* open_collector *) inout  wire p12,    // BUS2
    output wire p13,                         // OUT1
    input  wire p14,                         // IN1
    (* open_collector *) inout  wire p15,    // BUS1
    input  wire p16                          // VCC
);
endmodule
`default_nettype wire
