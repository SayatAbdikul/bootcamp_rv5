module fetch (
    /* verilator lint_off UNUSEDSIGNAL */
    input logic [31:0] pc_in,
    /* verilator lint_on UNUSEDSIGNAL */
    output logic [31:0] instruction_out
);
    
    logic [31:0] memory [0:16777215]; // Simple instructions array
    initial begin
        $readmemh("/Users/sayat/Documents/GitHub/bootcamp_rv5/imem.hex", memory); // Initialize memory from file
    end
    always_comb begin
        instruction_out = memory[pc_in[25:2]];
        // if((pc_in - 4) % 40000 == 0)
        //     $display("FETCH: PC=0x%08h INSTR=0x%08h", pc_in, instruction_out);
    end

endmodule
