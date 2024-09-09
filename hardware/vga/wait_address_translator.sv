`timescale 1 ps / 1 ps

/* This module converts a user specified coordinates into a memory address.
 * The output of the module depends on the resolution set by the user.
 */
module wait_address_translator(x, y, mem_address);

	input logic [9:0] x;
	input logic [9:0] y;
	output logic [16:0] mem_address;
	
	logic [9:0] x_within;
	logic [9:0] y_within;

	always@(*) begin
    		if((x>=9'd160) && (x<9'd480) && (y>=9'd130) && (y<9'd350)) begin
				x_within = x - 9'd160;
				y_within = y - 9'd130;
    		end else begin
				x_within = 0;
				y_within = 0;
    		end
	end

	assign mem_address = ({1'b0, y_within, 8'd0} + {1'b0, y_within, 6'd0} + {1'b0, x_within});

endmodule