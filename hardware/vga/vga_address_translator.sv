`timescale 1 ps / 1 ps

/* This module converts a user specified coordinates into a memory address.
 * The output of the module depends on the resolution set by the user.
 */
module vga_address_translator(x, y, mem_address);

	input logic [9:0] x;
	input logic [9:0] y;
	output logic [18:0] mem_address;

	/* The basic formula is address = y*WIDTH + x;
	 * For 320x240 resolution we can write 640 as (512 + 128). Memory address becomes
	 * (y*512) + (y*128) + x;
	 * This simplifies multiplication a simple shift and add operation.
	 * A leading 0 bit is added to each operand to ensure that they are treated as unsigned
	 * inputs. By default the use a '+' operator will generate a signed adder.
	 */

	// assign mem_address = ({1'b0, y, 9'd0} + {1'b0, y, 7'd0} + {1'b0, x});
	assign mem_address = (y << 9) + (y << 7) + x;

endmodule