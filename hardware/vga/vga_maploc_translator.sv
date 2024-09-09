`timescale 1 ps / 1 ps

/* This module converts coordinates on 15x15 game map into vga coordinates 640x480. */
module vga_maploc_translator(xin, yin, x, y);

	input logic [4:0] xin;
	input logic [4:0] yin;
	output logic [9:0] x;
	output logic [9:0] y;

	logic [15:0] wide_x, wide_y;
	
	assign wide_x = {9'b0, xin};
	assign wide_y = {9'b0, yin};

    always_comb begin
        if (xin == 15) begin    //player 1 health initial
            if (yin == 0) begin
                x = 4;
                y = 121;
            end else if (yin == 1) begin
                x = 14;
                y = 216;
            end else if (yin == 2) begin
                x = 38;
                y = 216;
            end else begin // if (yin == 3)
                x = 62;
                y = 216;
            end
        end
        else if (xin == 16) begin
            if (yin == 0) begin
                x = 548;
                y = 254;
            end else if (yin == 1) begin
                x = 558;
                y = 349;
            end else if (yin == 2) begin
                x = 582; //558+24
                y = 349;
            end else begin // if (yin == 3)
                x = 606;
                y = 349;
            end
        end
        else if (xin == 17) begin
            if (yin == 0) begin
                x = 4;
                y = 254;
            end else if (yin == 1) begin
                x = 14;
                y = 349;
            end else if (yin == 2) begin
                x = 38;
                y = 349;
            end else begin // if (yin == 3)
                x = 62;
                y = 349;
            end
        end
        else if (xin == 18) begin
            if (yin == 0) begin
                x = 548;
                y = 121;
            end else if (yin == 1) begin
                x = 558;
                y = 216;
            end else if (yin == 2) begin
                x = 582;
                y = 216;
            end else begin // if (yin == 3)
                x = 606;
                y = 216;
            end
        end
        else begin // inside 15x15 map
            x = (9'd95 + 30*(wide_x));
            y = (9'd15 + 30*(wide_y));
            // x = (9'd95 + {1'b0, wide_x, 4'd0} + {1'b0, wide_x, 3'd0} + {1'b0, wide_x, 2'd0} + {1'b0, wide_x, 1'd0});
            // y = (9'd15 + {1'b0, wide_y, 4'd0} + {1'b0, wide_y, 3'd0} + {1'b0, wide_y, 2'd0} + {1'b0, wide_y, 1'd0});
        end
    end

endmodule