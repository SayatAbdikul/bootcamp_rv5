module gpr (
    input logic clk,
    input logic rst,
    /* verilator lint_off UNUSEDSIGNAL */
    input logic [4:0] rs1,
    input logic [4:0] rs2,
    input logic [4:0] rd,
    /* verilator lint_on UNUSEDSIGNAL */
    input logic [31:0] write_data,
    input logic write_enable,
    output logic [31:0] read_data1,
    output logic [31:0] read_data2,
    output logic [31:0] registers_out [0:15]
);
    logic [31:0] registers [0:15]; // 16 general-purpose registers
    // Read operations
    assign read_data1 = registers[rs1[3:0]];
    assign read_data2 = registers[rs2[3:0]];
    // Expose all registers
    assign registers_out = registers;
    // Write operation
    always_ff @(posedge clk) begin 
        if (rst) begin
            // Initialize registers to zero on reset
            integer i;
            for (i = 0; i < 16; i = i + 1) begin
                registers[i] <= 32'b0;
            end
        end else if (write_enable && rd[3:0] != 4'b0) begin
            // Prevent writes to x0 (hardwired to 0 in RISC-V)
            registers[rd[3:0]] <= write_data;
        end
    end
endmodule
