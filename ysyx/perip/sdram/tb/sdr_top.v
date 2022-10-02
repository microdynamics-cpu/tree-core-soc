module sdr_top(
    input clk,
    input cke,
    input cs,
    input ras,
    input cas,
    input we,
    input [1:0] dqm,
    input [12:0] addr,
    input [1:0] ba,
    output [15:0] data_input,
    input [15:0] data_output,
    input data_out_en
);

endmodule