module kdb(
    input clock,
    input resetn,
    output ps2_clk,
    output ps2_dat
);

// 25M -> 6.25M
parameter NUM_DIV = 4;
// 25M -> 10k
// parameter NUM_DIV = 2500;
reg [12:0]cnt;
reg clk_div;
always @(posedge clock or negedge resetn)
    if(!resetn) begin
        cnt     <= 13'd0;
        clk_div <= 1'b0;
    end
    else if(cnt < NUM_DIV / 2 - 1) begin
        cnt     <= cnt + 1'b1;
        clk_div <= clk_div;
    end
    else begin
        cnt <= 13'd0;
        clk_div <= ~clk_div;
    end

assign ps2_clk = clk_div;
assign ps2_dat = 1'b1;
endmodule

import "DPI-C" function void ps2_read(output byte dat);

module PS2Read (
  input       clock,
  output reg[7:0] dat,
);
  always@(posedge clock) begin
    ps2_read(dat);
    $display("PS2Read dat: %0h", dat);
  end
endmodule