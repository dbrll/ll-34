`default_nettype none
// DM8837 hex unified bus receiver. National Interface 1974, 4-21
(* blackbox *)
module dm8837 (
    input  wire p1,                          // IN4
    output wire p2,                          // OUT4
    input  wire p3,                          // IN5
    output wire p4,                          // OUT5
    input  wire p5,                          // IN6
    output wire p6,                          // OUT6
    input  wire p7,                          // DISABLE_B
    input  wire p8,                          // GND
    input  wire p9,                          // DISABLE_A
    output wire p10,                         // OUT3
    input  wire p11,                         // IN3
    output wire p12,                         // OUT2
    input  wire p13,                         // IN2
    output wire p14,                         // OUT1
    input  wire p15,                         // IN1
    input  wire p16                          // VCC
);
endmodule
`default_nettype wire
