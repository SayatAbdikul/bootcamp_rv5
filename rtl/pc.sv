module pc (
    input logic clk,
    input logic rst,
    input logic branch_enable,
    input logic [31:0] branch_target,
    output logic [31:0] pc_out
);
    logic [31:0] pc_reg;

    always_ff @(posedge clk) begin
        if (rst) begin
            pc_reg <= 32'b0;
        end else if (branch_enable) begin
            pc_reg <= branch_target; // Jump to target address
        end else begin
            pc_reg <= pc_reg + 4; // Increment PC by 4 on each clock cycle
        end
    end

    assign pc_out = pc_reg;
endmodule
