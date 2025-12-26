module ram (
    input logic clk,
    input logic [31:0] address,
    input logic [31:0] write_data,
    input logic w_write_enable,
    input logic b_write_enable,
    input logic read_enable,
    input logic [2:0] funct3,
    output logic [31:0] read_data
);
    import "DPI-C" function void mem_init(string path);
    import "DPI-C" function int  mem_read(int addr);
    import "DPI-C" function void mem_write(int addr, int data, byte wmask);

    wire [1:0] byte_offset = address[1:0];

    initial begin
        mem_init("/Users/sayat/Documents/GitHub/bootcamp_rv5/imem.hex");
    end

    always_comb begin
        read_data = 32'b0; // Default read data
        if (read_enable) begin
            logic [31:0] word;
            word = mem_read(address);
            case (funct3)
                3'b010: begin // LW - load word
                    read_data = word;
                end
                3'b100: begin // LBU - load byte unsigned
                    case (byte_offset)
                        2'b00: read_data = {24'b0, word[7:0]};
                        2'b01: read_data = {24'b0, word[15:8]};
                        2'b10: read_data = {24'b0, word[23:16]};
                        2'b11: read_data = {24'b0, word[31:24]};
                    endcase
                end
                default: begin
                    read_data = 32'b0;
                end
            endcase
        end
    end

    always_ff @( posedge clk ) begin
        if (w_write_enable) begin
            mem_write(address, write_data, 8'hF);
        end else if (b_write_enable) begin
            case (byte_offset)
                2'b00: mem_write(address, write_data, 8'h1);
                2'b01: mem_write(address, write_data, 8'h2);
                2'b10: mem_write(address, write_data, 8'h4);
                2'b11: mem_write(address, write_data, 8'h8);
            endcase
        end
    end
endmodule
