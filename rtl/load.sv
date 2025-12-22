module dmem_instructions (
    input logic [19:0] immediate,
    input logic [4:0] rd,

    input logic [11:0] offset,
    input logic lw_enable,
    input logic lbu_enable,
    input logic sb_enable,
    input logic sw_enable,
    output logic read_enable,
    output logic write_enable,
    output logic [31:0] address
);
    always_comb begin
        address = 32'b0; // Default address
        
    end
    
endmodule