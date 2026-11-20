`default_nettype none
// LED, pin 1 anode, pin 2 cathode
(* blackbox *)
module led (
    inout  wire p1,                          // ANODE
    inout  wire p2                           // CATHODE
);
endmodule
`default_nettype wire
