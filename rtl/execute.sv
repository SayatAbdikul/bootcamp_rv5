module execute (
    input logic [31:0] reg_data1,
    input logic [31:0] reg_data2,
    input logic [11:0] imm_i,
    input logic [19:0] imm_u,
    input logic [6:0] opcode,
    input logic [31:0] pc_in,
    output logic [31:0] result,
    output logic [31:0] branch_target,
    output logic branch_enable
);
    always_comb begin
        // Default values
        branch_enable = 1'b0;
        branch_target = 32'b0;
        
        case (opcode)
            7'b0110011: begin // R-type ADD
                result = reg_data1 + reg_data2;
            end

            7'b0010011: begin // ADDI
                result = reg_data1 + {{20{imm_i[11]}}, imm_i};
            end

            7'b0110111: begin // LUI
                result = {imm_u, 12'b0};
            end

            7'b1100111: begin // JALR
                result = pc_in + 4; // Return address (PC+4)
                branch_target = (reg_data1 + {{20{imm_i[11]}}, imm_i}) & ~32'b1; // Target address, clear LSB
                branch_enable = 1'b1;
            end

            7'b0000011,
            7'b0100011: begin // Load / Store address calc
                result = reg_data1 + {{20{imm_i[11]}}, imm_i};
            end

            default: begin
                result = 32'b0;
            end
        endcase
    end

endmodule
