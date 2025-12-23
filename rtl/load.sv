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
        if (lw_enable || lbu_enable) begin
            address = {{12{immediate[19]}}, immediate}; // Sign-extend immediate for load instructions
        end else if (sb_enable || sw_enable) begin
            address = {{20{offset[11]}}, offset}; // Sign-extend offset for store instructions
        end
    end
    
endmodule