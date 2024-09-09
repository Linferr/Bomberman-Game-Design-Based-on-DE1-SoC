`timescale 1 ps / 1 ps

module wordcopy(input logic clk, input logic rst_n,
                input logic en, output logic rdy,
                input logic [4:0] xin, input [4:0] yin, //15x15 vga(x,y)
                input logic [6:0] copytype, // which pic to write
                input logic [7:0] readdata, output logic [14:0] read_addr, //read from 29x29 or 89x89 mem
                output logic [7:0] writedata, output logic [18:0] write_addr, output logic wren //write to vag 640x48
                );

    enum {WAIT, Read, WaitR, Write, Finish} current_state;

    
    logic [6:0] length; //pic width e.g. 29
    logic [12:0] size; //mem size e.g. 29x29
    logic [18:0] padding; //padding when write to vga e.g.640-29
    logic [14:0] read_addr_init;

    always_comb begin
        if (copytype <= 8) begin
            length = 7'd29;
            size = 13'd840; //  29*29-1
            padding = 19'd612; // 640-29+1
            read_addr_init = length*length*copytype; //e.g. ct = 1, addr_init = 840
        end else if (copytype <= 10) begin
            length = 7'd20;
            size = 13'd399; // 20*20-1
            padding = 19'd621; // 640-20+1
            read_addr_init = 29*29*9 + length*length*(copytype-9);
        end else begin // defualt 29*29
            length = 7'd29;
            size = 13'd840; //  29*29-1
            padding = 19'd612; // 640-29+1
            read_addr_init = length*length*0; //black
        end
    end

    logic [9:0] x, y;
    logic [18:0] write_addr_init;
    vga_maploc_translator coor2vga (xin, yin, x, y);
    vga_address_translator vga2add (x, y, write_addr_init);

    logic [12:0] count;

    always @(posedge clk) begin
        if( rst_n==0 ) begin
            current_state <= WAIT;
            rdy <= 1'b1; 
            read_addr <= read_addr_init;
            writedata <= 8'b0;
            write_addr <= write_addr_init;
            wren <= 1'b0;
            count <= 13'b0;
        end
        else begin
            case( current_state )
                WAIT: begin
                    rdy <= 1'b1;
                    if (en == 1'b1) begin
                        current_state <= Read;
                        rdy <= 1'b0;
                        read_addr <= read_addr_init;
                        writedata <= 8'b0;
                        write_addr <= write_addr_init;
                        wren <= 1'b0;
                        count <= 13'b0;
                    end
                end

                Read: begin
                    current_state <= WaitR;
                    rdy <= 1'b0;
                    read_addr <= read_addr_init + count;
                    writedata <= 8'b0;
                    write_addr <= write_addr;
                    wren <= 1'b0;
                end

                WaitR: begin
                    current_state <= Write;
                end

                Write: begin
                    if(count < size) current_state <= Read;
                    else current_state <= Finish;
                    
                    rdy <= 1'b0;
                    read_addr <= read_addr;
                    writedata <= readdata;
                    
                    if(count != 0 && count % length == 0) 
                        write_addr <= write_addr + padding;
                    else begin
                        if (count == 0) write_addr <= write_addr_init;
                        else write_addr <= write_addr + 18'd1;
                    end

                    wren <= 1'b1;
                    count <= count + 13'b1;
                end

                default: begin             // Finish state
                    current_state <= WAIT;
                    rdy <= 1'b1;
                    read_addr <= 15'd0;
                    writedata <= 8'd0;
                    write_addr <= write_addr_init;
                    wren <= 1'b0;
                    count <= 13'd0;
                end
            endcase
        end
    end


endmodule: wordcopy