`timescale 1 ps / 1 ps

/* This module converts a user specified coordinates into a memory address.
 * The output of the module depends on the resolution set by the user.
 */
module win_address_translator(start, x, y, mem_address);

    input logic [2:0] start;
	input logic [9:0] x;
	input logic [9:0] y;
	output logic [16:0] mem_address;
	
	logic [9:0] x_within;
	logic [9:0] y_within;

    always@(*) begin
        if (start == 3'd2 && (x>=31) && (x<31+36) && (y>=103) && (y<103+28)) begin
            x_within = x - 31;
            y_within = y - 103;
        end else if (start == 3'd3 && (x>=576) && (x<576+36) && (y>=240) && (y<240+28)) begin
            x_within = x - 576;
            y_within = y - 240;
        end else if (start == 3'd4 && (x>=30) && (x<30+36) && (y>=240) && (y<240+28)) begin
            x_within = x - 30;
            y_within = y - 240;
        end else if (start == 3'd5 && (x>=576) && (x<576+36) && (y>=103) && (y<103+28)) begin
            x_within = x - 576;
            y_within = y - 103;
        end else begin
            x_within = 0;
            y_within = 0;
        end
	end

	assign mem_address = ({1'b0, y_within, 5'd0} + {1'b0, y_within, 2'd0} + {1'b0, x_within});


endmodule