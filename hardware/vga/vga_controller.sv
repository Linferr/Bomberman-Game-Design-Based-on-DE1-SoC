// Code reference: https://vanhunteradams.com/DE1/VGA_Driver/Driver.html

`timescale 1 ps / 1 ps

module vga_controller(
    input wire clock,     // 25 MHz
    input wire reset,     // Active low
    input [7:0] color_in, // Pixel color data (RRRGGGBB)
    output [9:0] next_x,  // x-coordinate of NEXT pixel that will be drawn
    output [9:0] next_y,  // y-coordinate of NEXT pixel that will be drawn
    output wire hsync,    // HSYNC (to VGA connector)
    output wire vsync,    // VSYNC (to VGA connctor)
    output [7:0] red,     // RED (to resistor DAC VGA connector)
    output [7:0] green,   // GREEN (to resistor DAC to VGA connector)
    output [7:0] blue,    // BLUE (to resistor DAC to VGA connector)
    output sync,          // SYNC to VGA connector
    output clk,           // CLK to VGA connector
    output blank          // BLANK to VGA connector
);

    // Horizontal parameters (measured in clock cycles)
    parameter [9:0] H_ACTIVE  =  10'd_639 ;
    parameter [9:0] H_FRONT   =  10'd_15 ;
    parameter [9:0] H_PULSE   =  10'd_95 ;
    parameter [9:0] H_BACK    =  10'd_47 ;

    // Vertical parameters (measured in lines)
    parameter [9:0] V_ACTIVE   =  10'd_479 ;
    parameter [9:0] V_FRONT    =  10'd_9 ;
    parameter [9:0] V_PULSE =  10'd_1 ;
    parameter [9:0] V_BACK  =  10'd_32 ;

    // Parameters for readability
    parameter   LOW     = 1'b_0 ;
    parameter   HIGH    = 1'b_1 ;

    // States (more readable)
    parameter   [7:0]    H_ACTIVE_STATE    = 8'd_0 ;
    parameter   [7:0]   H_FRONT_STATE     = 8'd_1 ;
    parameter   [7:0]   H_PULSE_STATE   = 8'd_2 ;
    parameter   [7:0]   H_BACK_STATE     = 8'd_3 ;

    parameter   [7:0]    V_ACTIVE_STATE    = 8'd_0 ;
    parameter   [7:0]   V_FRONT_STATE    = 8'd_1 ;
    parameter   [7:0]   V_PULSE_STATE   = 8'd_2 ;
    parameter   [7:0]   V_BACK_STATE     = 8'd_3 ;

    // Clocked registers
    reg              hysnc_reg ;
    reg              vsync_reg ;
    reg     [7:0]   red_reg ;
    reg     [7:0]   green_reg ;
    reg     [7:0]   blue_reg ;
    reg              line_done ;

    // addons
    logic     [7:0]   red_trans ;
    logic     [7:0]   green_trans ;
    logic     [7:0]   blue_trans ;

    // Control registers
    reg     [9:0]   h_counter ;
    reg     [9:0]   v_counter ;

    reg     [7:0]    h_state ;
    reg     [7:0]    v_state ;

    // State machine
    always@(posedge clock) begin
        // At reset . . .
        if (!reset) begin
            // Zero the counters
            h_counter   <= 10'd_0 ;
            v_counter   <= 10'd_0 ;
            // States to ACTIVE
            h_state     <= H_ACTIVE_STATE  ;
            v_state     <= V_ACTIVE_STATE  ;
            // Deassert line done
            line_done   <= LOW ;
        end
        else begin
            //////////////////////////////////////////////////////////////////////////
            ///////////////////////// HORIZONTAL /////////////////////////////////////
            //////////////////////////////////////////////////////////////////////////
            if (h_state == H_ACTIVE_STATE) begin
                // Iterate horizontal counter, zero at end of ACTIVE mode
                h_counter <= (h_counter==H_ACTIVE)?10'd_0:(h_counter + 10'd_1) ;
                // Set hsync
                hysnc_reg <= HIGH ;
                // Deassert line done
                line_done <= LOW ;
                // State transition
                h_state <= (h_counter == H_ACTIVE)?H_FRONT_STATE:H_ACTIVE_STATE ;
            end
            if (h_state == H_FRONT_STATE) begin
                // Iterate horizontal counter, zero at end of H_FRONT mode
                h_counter <= (h_counter==H_FRONT)?10'd_0:(h_counter + 10'd_1) ;
                // Set hsync
                hysnc_reg <= HIGH ;
                // State transition
                h_state <= (h_counter == H_FRONT)?H_PULSE_STATE:H_FRONT_STATE ;
            end
            if (h_state == H_PULSE_STATE) begin
                // Iterate horizontal counter, zero at end of H_PULSE mode
                h_counter <= (h_counter==H_PULSE)?10'd_0:(h_counter + 10'd_1) ;
                // Clear hsync
                hysnc_reg <= LOW ;
                // State transition
                h_state <= (h_counter == H_PULSE)?H_BACK_STATE:H_PULSE_STATE ;
            end
            if (h_state == H_BACK_STATE) begin
                // Iterate horizontal counter, zero at end of H_BACK mode
                h_counter <= (h_counter==H_BACK)?10'd_0:(h_counter + 10'd_1) ;
                // Set hsync
                hysnc_reg <= HIGH ;
                // State transition
                h_state <= (h_counter == H_BACK)?H_ACTIVE_STATE:H_BACK_STATE ;
                // Signal line complete at state transition (offset by 1 for synchronous state transition)
                line_done <= (h_counter == (H_BACK-1))?HIGH:LOW ;
            end
            //////////////////////////////////////////////////////////////////////////
            ///////////////////////// VERTICAL ///////////////////////////////////////
            //////////////////////////////////////////////////////////////////////////
            if (v_state == V_ACTIVE_STATE) begin
                // increment vertical counter at end of line, zero on state transition
                v_counter<=(line_done==HIGH)?((v_counter==V_ACTIVE)?10'd_0:(v_counter+10'd_1)):v_counter ;
                // set vsync in active mode
                vsync_reg <= HIGH ;
                // state transition - only on end of lines
                v_state<=(line_done==HIGH)?((v_counter==V_ACTIVE)?V_FRONT_STATE:V_ACTIVE_STATE):V_ACTIVE_STATE ;
            end
            if (v_state == V_FRONT_STATE) begin
                // increment vertical counter at end of line, zero on state transition
                v_counter<=(line_done==HIGH)?((v_counter==V_FRONT)?10'd_0:(v_counter + 10'd_1)):v_counter ;
                // set vsync in front porch
                vsync_reg <= HIGH ;
                // state transition
                v_state<=(line_done==HIGH)?((v_counter==V_FRONT)?V_PULSE_STATE:V_FRONT_STATE):V_FRONT_STATE;
            end
            if (v_state == V_PULSE_STATE) begin
                // increment vertical counter at end of line, zero on state transition
                v_counter<=(line_done==HIGH)?((v_counter==V_PULSE)?10'd_0:(v_counter + 10'd_1)):v_counter ;
                // clear vsync in pulse
                vsync_reg <= LOW ;
                // state transition
                v_state<=(line_done==HIGH)?((v_counter==V_PULSE)?V_BACK_STATE:V_PULSE_STATE):V_PULSE_STATE;
            end
            if (v_state == V_BACK_STATE) begin
                // increment vertical counter at end of line, zero on state transition
                v_counter<=(line_done==HIGH)?((v_counter==V_BACK)?10'd_0:(v_counter + 10'd_1)):v_counter ;
                // set vsync in back porch
                vsync_reg <= HIGH ;
                // state transition
                v_state<=(line_done==HIGH)?((v_counter==V_BACK)?V_ACTIVE_STATE:V_BACK_STATE):V_BACK_STATE ;
            end

            //////////////////////////////////////////////////////////////////////////
            //////////////////////////////// COLOR OUT ///////////////////////////////
            //////////////////////////////////////////////////////////////////////////
            // Assign colors if in active mode
            case(color_in[7:5]) //r
                3'd0: red_trans = 8'b0;
                3'd1: red_trans = 8'b100100;
                3'd2: red_trans = 8'b1001000;
                3'd3: red_trans = 8'b1101100;
                3'd4: red_trans = 8'b10010000;
                3'd5: red_trans = 8'b10110100;
                3'd6: red_trans = 8'b11011000;
                3'd7: red_trans = 8'b11111100;
                default: red_trans = 8'b0;
            endcase

            case(color_in[4:2]) //g
                3'd0: green_trans = 8'b0;
                3'd1: green_trans = 8'b100100;
                3'd2: green_trans = 8'b1001000;
                3'd3: green_trans = 8'b1101100;
                3'd4: green_trans = 8'b10010000;
                3'd5: green_trans = 8'b10110100;
                3'd6: green_trans = 8'b11011000;
                3'd7: green_trans = 8'b11111100;
                default: green_trans = 8'b0;
            endcase

            case(color_in[1:0]) //b
                2'd0: blue_trans = 8'b0;
                2'd1: blue_trans = 8'b1010101;
                2'd2: blue_trans = 8'b10101010;
                2'd3: blue_trans = 8'b11111111;
                default: blue_trans = 8'b0;
            endcase

            red_reg<=(h_state==H_ACTIVE_STATE)?((v_state==V_ACTIVE_STATE)?{red_trans}:8'd_0):8'd_0 ;
            green_reg<=(h_state==H_ACTIVE_STATE)?((v_state==V_ACTIVE_STATE)?{green_trans}:8'd_0):8'd_0 ;
            blue_reg<=(h_state==H_ACTIVE_STATE)?((v_state==V_ACTIVE_STATE)?{blue_trans}:8'd_0):8'd_0 ;

        end
    end
    // Assign output values - to VGA connector
    assign hsync = hysnc_reg ;
    assign vsync = vsync_reg ;
    assign red = red_reg ;
    assign green = green_reg ;
    assign blue = blue_reg ;
    assign clk = clock ;
    assign sync = 1'b_0 ;
    assign blank = hysnc_reg & vsync_reg ;
    // The x/y coordinates that should be available on the NEXT cycle
    assign next_x = (h_state==H_ACTIVE_STATE)?h_counter:10'd_0 ;
    assign next_y = (v_state==V_ACTIVE_STATE)?v_counter:10'd_0 ;

endmodule