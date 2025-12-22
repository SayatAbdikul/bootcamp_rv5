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
    logic [7:0] memory [0:255]; // Simple memory array
    initial begin
        $readmemh("/Users/sayat/Documents/GitHub/bootcamp_rv5/dmem.hex", memory); // Initialize memory from file
    end
    always_comb begin
        read_data = 32'b0; // Default read data
        if (read_enable) begin
            case (funct3)
                3'b010: begin // LW - load word
                    read_data = {memory[address + 3], memory[address + 2], memory[address + 1], memory[address]};
                end
                3'b100: begin // LBU - load byte unsigned
                    read_data = {24'b0, memory[address]};
                end
                default: begin
                    read_data = 32'b0;
                end
            endcase
        end
    end
    always_ff @( posedge clk ) begin
        if (w_write_enable) begin
            memory[address] <= write_data[7:0];
            memory[address + 1] <= write_data[15:8];
            memory[address + 2] <= write_data[23:16];
            memory[address + 3] <= write_data[31:24];
        end else if (b_write_enable) begin
            memory[address] <= write_data[7:0];
        end
    end
endmodule
