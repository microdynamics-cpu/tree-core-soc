module kdb(
    input clock,
    input resetn,
    output ps2_clk,
    output ps2_dat
);

// 25M -> 10k
parameter NUM_DIV = 2500;
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


typedef enum [2:0] { idle_t, start_t, data_t, chk_t, stop_t } state_t;
reg [2:0] state;
reg[10:0] send_buf;
reg[3:0] send_cnt;
reg [2:0] ps2_clk_sync;
always @(posedge clock) begin
    ps2_clk_sync <= {ps2_clk_sync[1:0],ps2_clk};
end

wire sampling = ps2_clk_sync[2] & ~ps2_clk_sync[1];

always@(posedge clock or negedge resetn) begin
    if(!resetn) begin
        state <= idle_t;
    end
    else if(sampling) begin
        case (state)
            idle_t: state <= (1) ? start_t : state;
            start_t: state <= data_t;
            data_t: state <= (send_cnt == 4'd8)? chk_t : state; 
            chk_t: state <= stop_t;
            stop_t: state <= idle_t;
        endcase
    end
end

always@(posedge clock or negedge resetn) begin
    if(!resetn) begin
        send_buf <= 11'd0;
        send_cnt <= 4'd0;
    end
    else if(sampling) begin
        case (state)
            idle_t: begin
                send_cnt <=  4'd0;
            end
            start_t: begin
                send_cnt <= send_cnt + 1'b1;
                send_buf[send_cnt] <= 1'b0;
            end
            data_t: begin
                send_cnt <= send_cnt + 1'b1;
                send_buf[send_cnt] <= 1'b1;
            end
            chk_t: begin
                send_cnt <= send_cnt + 1'b1;
                send_buf[send_cnt] <= ~(^send_buf[8:1]);
            end
            stop_t: begin
                send_cnt <= send_cnt + 1'b1;
                send_buf[send_cnt] <= 1'b1;
            end
            default: begin
                state <= state;
                $display("error state \r");
                $fatal;
            end
        endcase
    end
end

assign ps2_dat = (state != idle_t) ? send_buf[send_cnt] : 1'b1;
endmodule

import "DPI-C" function void ps2_read(output byte dat);

module PS2Read (
  input       clock,
  input       ren,
  input [7:0] dat,
);
//   always@(posedge clock) begin
//     if (ren) flash_read(addr, data);
//   end
endmodule