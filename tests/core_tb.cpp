#include <verilated.h>
#include <verilated_vcd_c.h>
#include <iostream>
#include <fstream>
#include <cstring>
#include <iomanip>
#include "Vcore.h"
#include "memory.h"

using namespace std;

// Golden Model Class
class RV32GoldenModel {
private:
    uint32_t gpr[16];
    uint32_t pc;
    uint32_t opcode, rd, rs1, rs2, funct3, funct7;
    int32_t imm_i;
    uint32_t imm_u;
    
    void decode(uint32_t instr) {
        opcode = instr & 0x7F;
        rd = (instr >> 7) & 0x1F;
        funct3 = (instr >> 12) & 0x07;
        rs1 = (instr >> 15) & 0x1F;
        rs2 = (instr >> 20) & 0x1F;
        funct7 = (instr >> 25) & 0x7F;
        imm_i = static_cast<int32_t>(instr) >> 20;
        imm_u = instr >> 12;
    }
    
    void write_gpr(uint32_t index, uint32_t value) {
        if ((index & 0xF) != 0) {
            gpr[index & 0xF] = value;
        }
    }
    
    uint32_t read_gpr(uint32_t index) {
        return gpr[index & 0xF];
    }
    
    uint32_t load_word(uint32_t byte_addr) {
        return static_cast<uint32_t>(mem_read(static_cast<int>(byte_addr)));
    }
    
    uint32_t load_byte_unsigned(uint32_t byte_addr) {
        uint32_t word = static_cast<uint32_t>(mem_read(static_cast<int>(byte_addr)));
        uint32_t byte_offset = byte_addr & 0x3;
        return (word >> (byte_offset * 8)) & 0xFF;
    }
    
    void store_word(uint32_t byte_addr, uint32_t value) {
        mem_write(static_cast<int>(byte_addr), static_cast<int>(value), 0xF);
    }
    
    void store_byte(uint32_t byte_addr, uint32_t value) {
        uint32_t byte_offset = byte_addr & 0x3;
        unsigned char mask = 1u << byte_offset;
        mem_write(static_cast<int>(byte_addr), static_cast<int>(value), mask);
    }

public:
    RV32GoldenModel() {
        reset();
    }
    
    void reset() {
        memset(gpr, 0, sizeof(gpr));
        pc = 0;
    }
    
    bool load_memory(const string& filename) {
        mem_init(filename.c_str());
        return true;
    }
    
    void step() {
        uint32_t instr = static_cast<uint32_t>(mem_read(static_cast<int>(pc)));
        uint32_t current_pc = pc;
        decode(instr);
        
        uint32_t next_pc = pc + 4;
        uint32_t alu_result = 0;
        bool branch_taken = false;
        
        switch (opcode) {
            case 0b0110011: // ADD
                if (funct7 == 0x00 && funct3 == 0x0) {
                    alu_result = read_gpr(rs1) + read_gpr(rs2);
                    write_gpr(rd, alu_result);
                }
                break;
            case 0b0010011: // ADDI
                if (funct3 == 0x0) {
                    alu_result = read_gpr(rs1) + imm_i;
                    write_gpr(rd, alu_result);
                }
                break;
            case 0b0110111: // LUI
                alu_result = imm_u << 12;
                write_gpr(rd, alu_result);
                break;
            case 0b0000011: // Load
                alu_result = read_gpr(rs1) + imm_i;
                if (funct3 == 0b010) {
                    write_gpr(rd, load_word(alu_result));
                } else if (funct3 == 0b100) {
                    write_gpr(rd, load_byte_unsigned(alu_result));
                }
                break;
            case 0b0100011: // Store
                alu_result = read_gpr(rs1) + imm_i;
                if (funct3 == 0b010) {
                    store_word(alu_result, read_gpr(rs2));
                } else if (funct3 == 0b000) {
                    store_byte(alu_result, read_gpr(rs2));
                }
                break;
            case 0b1100111: // JALR
                if (funct3 == 0x0) {
                    alu_result = (read_gpr(rs1) + imm_i) & ~1;
                    write_gpr(rd, current_pc + 4);
                    next_pc = alu_result;
                    branch_taken = true;
                }
                break;
            default:
                break;
        }
        pc = next_pc;
    }
    
    uint32_t get_gpr(int index) const { return gpr[index & 0xF]; }
    uint32_t get_pc() const { return pc; }
    
    // Get instruction at PC
    uint32_t get_instruction_at_pc() const {
        return static_cast<uint32_t>(mem_read(static_cast<int>(pc)));
    }
    
    // Decode and print instruction
    static string decode_instruction(uint32_t instr) {
        uint32_t opcode = instr & 0x7F;
        uint32_t rd = (instr >> 7) & 0x1F;
        uint32_t funct3 = (instr >> 12) & 0x07;
        uint32_t rs1 = (instr >> 15) & 0x1F;
        uint32_t rs2 = (instr >> 20) & 0x1F;
        uint32_t funct7 = (instr >> 25) & 0x7F;
        int32_t imm_i = static_cast<int32_t>(instr) >> 20;
        uint32_t imm_u = instr >> 12;
        
        stringstream ss;
        ss << "0x" << hex << setw(8) << setfill('0') << instr << " ";
        
        switch (opcode) {
            case 0b0110011: // R-type
                if (funct7 == 0x00 && funct3 == 0x0) {
                    ss << "add x" << dec << rd << ", x" << rs1 << ", x" << rs2;
                } else {
                    ss << "UNKNOWN R-type";
                }
                break;
            case 0b0010011: // I-type ALU
                if (funct3 == 0x0) {
                    ss << "addi x" << dec << rd << ", x" << rs1 << ", " << imm_i;
                } else {
                    ss << "UNKNOWN I-type ALU (funct3=" << funct3 << ")";
                }
                break;
            case 0b0110111: // LUI
                ss << "lui x" << dec << rd << ", 0x" << hex << imm_u;
                break;
            case 0b0000011: // Load
                if (funct3 == 0b010) {
                    ss << "lw x" << dec << rd << ", " << imm_i << "(x" << rs1 << ")";
                } else if (funct3 == 0b100) {
                    ss << "lbu x" << dec << rd << ", " << imm_i << "(x" << rs1 << ")";
                } else {
                    ss << "UNKNOWN Load (funct3=" << funct3 << ")";
                }
                break;
            case 0b0100011: // Store
                if (funct3 == 0b010) {
                    ss << "sw x" << dec << rs2 << ", " << imm_i << "(x" << rs1 << ")";
                } else if (funct3 == 0b000) {
                    ss << "sb x" << dec << rs2 << ", " << imm_i << "(x" << rs1 << ")";
                } else {
                    ss << "UNKNOWN Store (funct3=" << funct3 << ")";
                }
                break;
            case 0b1100111: // JALR
                ss << "jalr x" << dec << rd << ", " << imm_i << "(x" << rs1 << ")";
                break;
            default:
                ss << "UNSUPPORTED (opcode=" << hex << opcode << ")";
                break;
        }
        
        return ss.str();
    }
};

// Clock tick helper
void tick(Vcore* dut, VerilatedVcdC* tfp, vluint64_t& time) {
    dut->clk = 0;
    dut->eval();
    if (tfp) tfp->dump(time++);
    
    dut->clk = 1;
    dut->eval();
    if (tfp) tfp->dump(time++);
}

int main(int argc, char** argv) {
    Verilated::commandArgs(argc, argv);
    Verilated::traceEverOn(true);

    Vcore* dut = new Vcore;
    
    // VCD tracing
    VerilatedVcdC* tfp = new VerilatedVcdC;
    dut->trace(tfp, 99);
    tfp->open("core_tb.vcd");
    vluint64_t time = 0;

    // Initialize Golden Model and shared memory
    RV32GoldenModel golden;
    if (!golden.load_memory("imem.hex")) {
        cerr << "Error: Cannot load imem.hex" << endl;
        return 1;
    }

    cout << "==== CORE TESTBENCH WITH GOLDEN MODEL ====\n";

    // -------------------------
    // Reset
    // -------------------------
    cout << "Applying reset...\n";
    dut->rst = 1;
    tick(dut, tfp, time);
    tick(dut, tfp, time);
    dut->rst = 0;
    
    cout << "Running core and checking against golden model...\n";
    
    int mismatches = 0;
    int matches = 0;
    
    // Store last few cycles for context
    const int CONTEXT_SIZE = 5;
    struct CycleInfo {
        int cycle;
        uint32_t rtl_pc;
        uint32_t golden_pc;
        uint32_t rtl_regs[16];
        uint32_t golden_regs[16];
        uint32_t instruction;
    };
    CycleInfo history[CONTEXT_SIZE];
    int history_idx = 0;
    
    // -------------------------
    // Run for N cycles
    // -------------------------
    for (int cycle = 0; cycle < 100000; cycle++) {
        // Store state before tick for debugging
        uint32_t pre_instruction = 0;
        if (cycle > 0) {
            // Fetch instruction from memory manually for debugging
            uint32_t pc_word = (golden.get_pc() >> 2) & 0xFF;
            // We'll read it from the fetch output after tick
        }
        
        tick(dut, tfp, time);
        golden.step();
        
        // Store cycle info in circular buffer
        history[history_idx].cycle = cycle;
        history[history_idx].rtl_pc = dut->pc_out;
        history[history_idx].golden_pc = golden.get_pc();
        history[history_idx].instruction = 0; // Will be filled if needed
        for (int i = 0; i < 16; i++) {
            history[history_idx].rtl_regs[i] = dut->registers_out[i];
            history[history_idx].golden_regs[i] = golden.get_gpr(i);
        }
        history_idx = (history_idx + 1) % CONTEXT_SIZE;
        
        // Compare RTL with Golden Model
        bool cycle_match = true;
        
        // Check PC
        if (dut->pc_out != golden.get_pc()) {
            cycle_match = false;
        }
        
        // Check all registers
        for (int i = 0; i < 16; i++) {
            if (dut->registers_out[i] != golden.get_gpr(i)) {
                cycle_match = false;
                break;
            }
        }
        
        if (!cycle_match) {
            mismatches++;
            cout << "\n" << string(80, '=') << endl;
            cout << "❌ MISMATCH DETECTED at Cycle " << dec << cycle << endl;
            cout << string(80, '=') << endl;
            
            // Get the instruction that was executed
            uint32_t prev_pc = cycle > 0 ? history[(history_idx + CONTEXT_SIZE - 1) % CONTEXT_SIZE].golden_pc : 0;
            uint32_t instr = golden.get_instruction_at_pc();
            
            cout << "\nInstruction being executed:" << endl;
            cout << "  PC = 0x" << hex << setw(8) << setfill('0') << prev_pc << endl;
            cout << "  " << RV32GoldenModel::decode_instruction(instr) << endl;
            
            // Show last few cycles for context
            cout << "\nContext (last " << CONTEXT_SIZE << " cycles):" << endl;
            for (int i = 0; i < CONTEXT_SIZE; i++) {
                int idx = (history_idx + i) % CONTEXT_SIZE;
                if (history[idx].cycle < cycle - CONTEXT_SIZE + 1) continue;
                
                bool was_match = true;
                if (history[idx].rtl_pc != history[idx].golden_pc) was_match = false;
                for (int r = 0; r < 16 && was_match; r++) {
                    if (history[idx].rtl_regs[r] != history[idx].golden_regs[r]) was_match = false;
                }
                
                cout << "  Cycle " << dec << setw(5) << history[idx].cycle 
                     << ": PC=0x" << hex << setw(8) << setfill('0') << history[idx].rtl_pc
                     << (was_match ? " ✓" : " ✗") << endl;
            }
            
            cout << "\nCurrent State Comparison:" << endl;
            cout << "  PC:" << endl;
            cout << "    RTL    = 0x" << hex << setw(8) << setfill('0') << dut->pc_out << endl;
            cout << "    Golden = 0x" << setw(8) << golden.get_pc() << endl;
            if (dut->pc_out != golden.get_pc()) {
                cout << "    DIFF   = " << (dut->pc_out > golden.get_pc() ? "+" : "") 
                     << dec << (int32_t)(dut->pc_out - golden.get_pc()) << endl;
            }
            
            cout << "\n  Registers:" << endl;
            cout << "    Reg | RTL      | Golden   | Match" << endl;
            cout << "    " << string(40, '-') << endl;
            
            for (int i = 0; i < 16; i++) {
                bool reg_match = (dut->registers_out[i] == golden.get_gpr(i));
                cout << "    x" << dec << setw(2) << i << " | "
                     << "0x" << hex << setw(6) << setfill('0') << dut->registers_out[i] << " | "
                     << "0x" << setw(6) << golden.get_gpr(i) << " | "
                     << (reg_match ? "✓" : "✗");
                if (!reg_match) {
                    int32_t diff = (int32_t)dut->registers_out[i] - (int32_t)golden.get_gpr(i);
                    cout << " (diff=" << dec << diff << ")";
                }
                cout << endl;
            }
            
            cout << string(80, '=') << endl;
            
            // Stop after first few mismatches
            if (mismatches >= 3) {
                cout << "\n⚠️  Stopping after " << mismatches << " mismatches for analysis" << endl;
                break;
            }
        } else {
            matches++;
        }
        
        // Print progress every 10000 cycles
        if (cycle % 10000 == 0 && cycle > 0) {
            cout << "✓ Cycle " << dec << cycle << ": " << matches << " matches, " 
                 << mismatches << " mismatches" << endl;
        }
    }

    cout << "\n==== CORE TEST COMPLETED ====\n";
    cout << "Total: " << matches << " matches, " << mismatches << " mismatches" << endl;
    
    if (mismatches == 0) {
        cout << "✅ ALL TESTS PASSED!" << endl;
    } else {
        cout << "❌ TESTS FAILED with " << mismatches << " mismatches" << endl;
    }
    
    cout << "Waveform saved to core_tb.vcd\n";

    tfp->close();
    delete tfp;
    delete dut;
    return (mismatches == 0) ? 0 : 1;
}
