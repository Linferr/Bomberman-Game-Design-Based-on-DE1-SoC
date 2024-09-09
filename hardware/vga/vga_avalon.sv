module vga_avalon(input logic clock, 
				 input logic resetn,
				 // slave (CPU-facing)
				output logic slave_waitrequest,
				input logic [3:0] slave_address,
				input logic slave_read, 
				output logic [31:0] slave_readdata,
				input logic slave_write, 
				input logic [31:0] slave_writedata,
				//---------
				output logic [7:0] vga_red, 
				output logic [7:0] vga_grn, 
				output logic [7:0] vga_blu,
				output logic vga_hsync, 
				output logic vga_vsync, 
				output logic vga_clk,
				output logic vga_sync,
				output logic vga_blank);

	logic [31:0] data_avalon;
	logic [2:0] start;
	logic [31:0] startword;
	enum{CPU_WORK, COPY, WAIT_COPY, WAIT_READY, DONE}state;

	// vga_clock Initialization logic
	logic clock_25;

	// On-chip Memory Initialization logic
	// vga_mem logic
	logic [7:0] vga_writedata;
	logic [18:0] vga_readaddress;
	logic [18:0] vga_writeaddress;
	logic wren;
	logic [7:0] vga_dout;

	// wait_mem logic
	logic [16:0] wait_readaddress;
	logic [7:0] wait_dout;

	// win_mem logic
	logic [16:0] win_readaddress;
	logic [7:0] win_dout;

	// ingame_mem logic
	logic [13:0] img_readaddress;
	logic [7:0] img_dout;

	// wordcopy logic
	logic [4:0] block_x;
	logic [4:0] block_y;
	logic [6:0] copytype;
	logic [7:0] wordcopy_readdata;
	logic [12:0] wordcopy_readaddress;
	logic en;
	logic rdy;

	// vga_controller logic
	logic [7:0] pixel_color;
	logic [9:0] x;
	logic [9:0] y;


	// vga_clock Initialization
	vga_pll vga_pll(.clock_in(clock), .clock_out(clock_25));


	// On-chip Memory Initialization
	vga_mem vga_memory (.data(vga_writedata),
			.rdaddress(vga_readaddress),
			.rdclock(clock_25),
			.wraddress(vga_writeaddress),
			.wrclock(clock),
			.wren(wren),
			.q(vga_dout));

	wait_mem wait_mem (.address(wait_readaddress),
			.clock(clock_25),
			.q(wait_dout));

	ingame_mem ingame_mem (.address(img_readaddress),
			.clock(clock),
			.q(img_dout));
	
	win_mem win_mem (.address(win_readaddress),
			.clock(clock_25),
			.q(win_dout));


	// Memory Translation
	vga_address_translator vga_address(.x(x), .y(y), .mem_address(vga_readaddress));
	wait_address_translator wait_address(.x(x), .y(y), .mem_address(wait_readaddress));
	win_address_translator win_address(.start(start), .x(x), .y(y), .mem_address(win_readaddress));

	// Memory Copy
	wordcopy copy(.clk(clock),
			.rst_n(resetn),
			.en(en), .rdy(rdy),
			.xin(block_x),
			.yin(block_y),
			.copytype(copytype),
			.readdata(wordcopy_readdata),
			.read_addr(wordcopy_readaddress), //read from 29x29 mem
			.writedata(vga_writedata),
			.write_addr(vga_writeaddress), 
			.wren(wren) //write to vga 640x48
			);


	// Instantiate VGA driver				   
	vga_controller vga_control (.clock(clock_25),		// 25 MHz PLL
			.reset(resetn),	  // Active low reset, manipulated by instantiating module
			.color_in(pixel_color), // Pixel color (RRRGGGBB) for pixel being drawn
			.next_x(x),		// X-coordinate (range [0, 639]) of next pixel to be drawn
			.next_y(y),		// Y-coordinate (range [0, 479]) of next pixel to be drawn
			.hsync(vga_hsync),		 // All of the connections to the VGA screen below
			.vsync(vga_vsync),
			.red(vga_red),
			.green(vga_grn),
			.blue(vga_blu),
			.sync(vga_sync),
			.clk(vga_clk),
			.blank(vga_blank));	

	assign block_x = data_avalon[7:3]; // 5 bit
	assign block_y = data_avalon[12:8]; // 5 bit
	assign copytype = data_avalon[18:13]; //6 bit
	assign start = data_avalon[2:0];

	assign wordcopy_readdata = img_dout;
	assign img_readaddress = wordcopy_readaddress;


	always @(posedge clock) begin
		if( resetn == 0 ) begin
			state <= CPU_WORK;
			slave_waitrequest <= 1'b0;
			en <= 0;
		end else begin
			case(state)
					CPU_WORK: begin
							if(slave_write == 1)begin
					case(slave_address)
						4'b0000: begin startword <= slave_writedata; state <= COPY; slave_waitrequest = 1'b1; end
						4'b0001: data_avalon <= slave_writedata;
					endcase
				end
						end
			COPY: begin
				if(rdy == 1)begin
					en <= 1;
					state <= WAIT_COPY;
					end
				else begin
					state <= COPY;
					en <= 0;
				end
			end
			WAIT_COPY: begin
				en <= 0;
				if(rdy == 0)begin
					state <= WAIT_READY;
				end
				else begin
					state <= WAIT_COPY;
				end
						end
			WAIT_READY: begin
				en <= 0;
				if(rdy == 1)begin
					state <= DONE;
				end
				else begin
					state <= WAIT_READY;
				end
			end
			DONE: begin
						slave_waitrequest <= 1'b0;
						state <= CPU_WORK;
					end
					default: begin
						state <= CPU_WORK;
					end
		endcase
		end
	end

	always@(*) begin
			if(start == 3'd1)begin // in game
				 pixel_color = vga_dout;
			end else if (start == 3'd0) begin //wait
				if ((x>=10'd160) && (x<10'd480) && (y>=10'd130) && (y<10'd350)) begin
				 pixel_color = wait_dout;
				end else begin // win page
					pixel_color = 8'b00000000;
				end
			end else if (start == 3'd2 && (x>=31) && (x<31+36) && (y>=103) && (y<103+28)) begin
					pixel_color = win_dout;
			end else if (start == 3'd3 && (x>=576) && (x<576+36) && (y>=240) && (y<240+28)) begin
					pixel_color = win_dout;
			end else if (start == 3'd4 && (x>=30) && (x<30+36) && (y>=240) && (y<240+28)) begin
					pixel_color = win_dout;
			end else if (start == 3'd5 && (x>=576) && (x<576+36) && (y>=103) && (y<103+28)) begin
					pixel_color = win_dout;
			end else pixel_color = vga_dout;
	end


endmodule: vga_avalon