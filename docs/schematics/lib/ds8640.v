`default_nettype none
// DS8640 quad NOR unified bus receiver. National Interface Databook 1983, 2-68
(* blackbox *)
module ds8640 (
    input  wire p1,                          // GND
    output wire p2,                          // OUT2
    output wire p3,                          // OUT1
    input  wire p4,                          // IN1A
    input  wire p5,                          // IN1B
    input  wire p6,                          // IN2A
    input  wire p7,                          // IN2B
    input  wire p8,                          // VCC
    input  wire p9,                          // IN3B
    input  wire p10,                         // IN3A
    input  wire p11,                         // IN4B
    input  wire p12,                         // IN4A
    output wire p13,                         // OUT4
    output wire p14                          // OUT3
);
endmodule
`default_nettype wire
