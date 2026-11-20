`default_nettype none
// 74S05 hex inverter, open collector. TI SDLS030A, 1
(* blackbox *)
module ttl_74s05 (
    input  wire p1,                          // A1
    (* open_collector *) output wire p2,     // Y1
    input  wire p3,                          // A2
    (* open_collector *) output wire p4,     // Y2
    input  wire p5,                          // A3
    (* open_collector *) output wire p6,     // Y3
    input  wire p7,                          // GND
    (* open_collector *) output wire p8,     // Y4
    input  wire p9,                          // A4
    (* open_collector *) output wire p10,    // Y5
    input  wire p11,                         // A5
    (* open_collector *) output wire p12,    // Y6
    input  wire p13,                         // A6
    input  wire p14                          // VCC
);
endmodule
`default_nettype wire
