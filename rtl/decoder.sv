module decoder (
    input logic [31:0] instruction,
    output logic [4:0] rs1,
    output logic [4:0] rs2,
    output logic [4:0] rd,
    output logic [6:0] opcode,
    output logic [2:0] funct3,
    output logic [6:0] funct7,
    output logic [11:0] imm_i,
    output logic [19:0] imm_u
);
    always_comb begin
        rs1    = instruction[19:15];
        rs2    = instruction[24:20];
        rd     = instruction[11:7];
        opcode  = instruction[6:0];
        funct3  = instruction[14:12];
        funct7  = instruction[31:25];
        imm_i    = instruction[31:20]; // Example for I-type immediate
        imm_u    = instruction[31:12]; // Example for U-type immediate
    end
endmodule
