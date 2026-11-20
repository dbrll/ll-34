`default_nettype none
// Diode, pin 1 anode, pin 2 cathode
(* blackbox *)
module diode (
    inout  wire p1,                          // ANODE
    inout  wire p2                           // CATHODE
);
endmodule
`default_nettype wire
