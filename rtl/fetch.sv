module fetch (
    /* verilator lint_off UNUSEDSIGNAL */
    input logic [31:0] pc_in,
    /* verilator lint_on UNUSEDSIGNAL */
    output logic [31:0] instruction_out
);

    import "DPI-C" function void mem_init(string path);
    import "DPI-C" function int mem_read(int addr);

    initial begin
        mem_init("/Users/sayat/Documents/GitHub/bootcamp_rv5/imem.hex");
    end

    always_comb begin
        // Word-aligned read; mem_read handles alignment internally
        instruction_out = mem_read(pc_in);
        // if((pc_in - 4) % 40000 == 0)
        //     $display("FETCH: PC=0x%08h INSTR=0x%08h", pc_in, instruction_out);
    end

endmodule
