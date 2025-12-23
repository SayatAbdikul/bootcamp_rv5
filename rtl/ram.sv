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
    // Use byte-addressed memory: 512KB = 524288 bytes = 131072 words
    logic [31:0] memory [0:131071]; // Word array
    
    initial begin
        // Initialize all memory to zero first
        for (int i = 0; i < 131072; i = i + 1) begin
            memory[i] = 32'b0;
        end
        // Then load from file if it exists
        $readmemh("/Users/sayat/Documents/GitHub/bootcamp_rv5/dmem_small.hex", memory);
    end
    
    // Helper to get word address from byte address
    wire [31:0] word_addr = address >> 2;
    wire [1:0] byte_offset = address[1:0];
    
    always_comb begin
        read_data = 32'b0; // Default read data
        if (read_enable) begin
            case (funct3)
                3'b010: begin // LW - load word (must be aligned)
                    read_data = memory[word_addr & 32'h1FFFF];
                end
                3'b100: begin // LBU - load byte unsigned
                    case (byte_offset)
                        2'b00: read_data = {24'b0, memory[word_addr & 32'h1FFFF][7:0]};
                        2'b01: read_data = {24'b0, memory[word_addr & 32'h1FFFF][15:8]};
                        2'b10: read_data = {24'b0, memory[word_addr & 32'h1FFFF][23:16]};
                        2'b11: read_data = {24'b0, memory[word_addr & 32'h1FFFF][31:24]};
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
            // Store word (must be aligned)
            memory[word_addr & 32'h1FFFF] <= write_data;
        end else if (b_write_enable) begin
            // Store byte
            case (byte_offset)
                2'b00: memory[word_addr & 32'h1FFFF][7:0]   <= write_data[7:0];
                2'b01: memory[word_addr & 32'h1FFFF][15:8]  <= write_data[7:0];
                2'b10: memory[word_addr & 32'h1FFFF][23:16] <= write_data[7:0];
                2'b11: memory[word_addr & 32'h1FFFF][31:24] <= write_data[7:0];
            endcase
        end
    end
endmodule
