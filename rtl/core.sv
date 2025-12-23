module core (
    input logic clk,
    input logic rst,
    output logic [31:0] registers_out [0:15],
    output logic [31:0] instruction_out,
    output logic [31:0] pc_out
);
    logic [31:0] pc;
    logic branch_enable;
    logic [31:0] branch_target;
    pc pc_inst (
        .clk(clk),
        .rst(rst),
        .branch_enable(branch_enable),
        .branch_target(branch_target),
        .pc_out(pc)
    );
    
    assign pc_out = pc;

    logic [31:0] instruction;
    fetch fetch_inst (
        .pc_in(pc),
        .instruction_out(instruction)
    );
    assign instruction_out = instruction;
    logic [4:0] rs1, rs2, rd;
    /* verilator lint_off UNUSEDSIGNAL */
    logic [6:0] funct7, opcode;
    /* verilator lint_on UNUSEDSIGNAL */
    logic [2:0] funct3;
    logic [11:0] imm_i;
    logic [19:0] imm_u;
    decoder decoder_inst (
        .instruction(instruction),
        .rs1(rs1),
        .rs2(rs2),
        .rd(rd),
        .opcode(opcode),
        .funct3(funct3),
        .funct7(funct7),
        .imm_i(imm_i),
        .imm_u(imm_u)
    );

    logic [31:0] reg_data1, reg_data2, reg_write;
    logic reg_write_enable;
    logic [31:0] execute_result;

    always_comb begin
        // Example logic to determine reg_write and reg_write_enable
        reg_write = 32'b0; // Default write data
        reg_write_enable = 1'b0; // Default write enable
        
        if(opcode == 7'b0110011 || opcode == 7'b0010011 || opcode == 7'b0110111) begin // add, addi, lui
            reg_write = execute_result; // Write result from execute stage
            reg_write_enable = 1'b1; 
        end else if(opcode == 7'b0000011) begin // lw, lbu
            reg_write = mem_read_data; // Write data from memory
            reg_write_enable = 1'b1;
        end else if(opcode == 7'b1100111) begin // jalr
            reg_write = execute_result; // Write return address (PC+4)
            reg_write_enable = 1'b1;
        end

    end
    logic [31:0] mem_read_data;
    ram ram_inst (
        .clk(clk),
        .address(execute_result), // Address from execute stage
        .write_data(reg_data2), // sw data from register
        .w_write_enable(opcode == 7'b0100011 && funct3 == 3'b010), // sw
        .b_write_enable(opcode == 7'b0100011 && funct3 == 3'b000 ), // sb
        .read_enable(opcode == 7'b0000011), // lw, lbu
        .funct3(funct3), // Pass funct3 to distinguish LW vs LBU
        .read_data(mem_read_data) 
    );
    gpr gpr_inst (
        .clk(clk),
        .rst(rst),
        .rs1(rs1), // Example source register 1
        .rs2(rs2), // Example source register 2
        .rd(rd),  // Example destination register
        .write_data(reg_write), // Example write data
        .write_enable(reg_write_enable), // Example write enable
        .read_data1(reg_data1),
        .read_data2(reg_data2),
        .registers_out(registers_out)
    );
    execute execute_inst (
        .reg_data1(reg_data1),
        .reg_data2(reg_data2),
        .imm_i(imm_i),
        .imm_u(imm_u),
        .opcode(opcode),
        .pc_in(pc),
        .result(execute_result),
        .branch_target(branch_target),
        .branch_enable(branch_enable)
    );

endmodule
