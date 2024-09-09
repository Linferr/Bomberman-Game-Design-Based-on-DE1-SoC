module NIOS_VGA(input logic CLOCK_50, 
             input logic [9:0] SW,
             input logic [3:0] KEY,
             output logic [9:0] LEDR,
             output logic [6:0] HEX0, 
             output logic [6:0] HEX1, 
             output logic [6:0] HEX2,
             output logic [6:0] HEX3, 
             output logic [6:0] HEX4, 
             output logic [6:0] HEX5,
             output logic [7:0] VGA_R, 
             output logic [7:0] VGA_G, 
             output logic [7:0] VGA_B,
             output logic VGA_HS, 
             output logic VGA_VS, 
             output logic VGA_CLK,
                 output logic VGA_SYNC_N,
                 output logic VGA_BLANK_N,
             inout [35:0] GPIO_0
             );

    logic [20:0] SEG0;
   logic [20:0] SEG1;


    nios_vga nios_vga (.clk_clk(CLOCK_50),
            .reset_reset_n(KEY[3]),
            .hex0_export(SEG0),   //    hex0.export
           .hex1_export(SEG1),   //    hex1.export
           .rs232_0_RXD(GPIO_0[35]),   // rs232_0.RXD
           .rs232_0_TXD(GPIO_0[33]),   //        .TXD
            .vga_vga_blank(VGA_BLANK_N),
            .vga_vga_blu(VGA_B),
            .vga_vga_clk(VGA_CLK),
            .vga_vga_grn(VGA_G),
            .vga_vga_hsync(VGA_HS),
            .vga_vga_red(VGA_R),
            .vga_vga_sync(VGA_SYNC_N),
            .vga_vga_vsync(VGA_VS));


    assign HEX0 = SEG0[6:0];
    assign HEX1 = SEG0[13:7];
    assign HEX2 = SEG0[20:14];
    assign HEX3 = SEG1[6:0];
    assign HEX4 = SEG1[13:7];
    assign HEX5 = SEG1[20:14];


endmodule: NIOS_VGA