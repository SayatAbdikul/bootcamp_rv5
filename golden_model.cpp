/**
 * Golden Model for RV32 Single-Cycle Processor
 * Supports: ADD, ADDI, LUI, LW, LBU, SW, SB, JALR
 * 16 GPRs (x0-x15)
 */

#include <cstdint>
#include <cstring>
#include <iostream>
#include <fstream>
#include <vector>
#include <iomanip>

class RV32GoldenModel {
private:
    // 16 General Purpose Registers
    uint32_t gpr[16];
    
    // Program Counter
    uint32_t pc;
    
    // Instruction Memory (256 words = 1KB)
    uint32_t imem[256];
    
    // Data Memory (256 bytes)
    uint8_t dmem[256];
    
    // Instruction fields
    uint32_t opcode, rd, rs1, rs2, funct3, funct7;
    int32_t imm_i;
    uint32_t imm_u;
    
    // Decode instruction
    void decode(uint32_t instr) {
        opcode = instr & 0x7F;
        rd = (instr >> 7) & 0x1F;
        funct3 = (instr >> 12) & 0x07;
        rs1 = (instr >> 15) & 0x1F;
        rs2 = (instr >> 20) & 0x1F;
        funct7 = (instr >> 25) & 0x7F;
        
        // I-type immediate (sign-extended)
        imm_i = static_cast<int32_t>(instr) >> 20;
        
        // U-type immediate
        imm_u = instr >> 12;
    }
    
    // Write to register (x0 is hardwired to 0)
    void write_gpr(uint32_t index, uint32_t value) {
        if ((index & 0xF) != 0) {  // Only use lower 4 bits, skip x0
            gpr[index & 0xF] = value;
        }
    }
    
    // Read from register
    uint32_t read_gpr(uint32_t index) {
        return gpr[index & 0xF];
    }
    
    // Memory operations
    uint32_t load_word(uint32_t addr) {
        addr &= 0xFF;  // 256 bytes memory
        return (dmem[addr + 3] << 24) | (dmem[addr + 2] << 16) | 
               (dmem[addr + 1] << 8) | dmem[addr];
    }
    
    uint32_t load_byte_unsigned(uint32_t addr) {
        addr &= 0xFF;
        return dmem[addr];
    }
    
    void store_word(uint32_t addr, uint32_t value) {
        addr &= 0xFF;
        dmem[addr] = value & 0xFF;
        dmem[addr + 1] = (value >> 8) & 0xFF;
        dmem[addr + 2] = (value >> 16) & 0xFF;
        dmem[addr + 3] = (value >> 24) & 0xFF;
    }
    
    void store_byte(uint32_t addr, uint32_t value) {
        addr &= 0xFF;
        dmem[addr] = value & 0xFF;
    }

public:
    RV32GoldenModel() {
        reset();
    }
    
    void reset() {
        // Clear all registers
        memset(gpr, 0, sizeof(gpr));
        pc = 0;
        
        // Clear memories
        memset(imem, 0, sizeof(imem));
        memset(dmem, 0, sizeof(dmem));
    }
    
    // Load instruction memory from hex file
    bool load_imem(const std::string& filename) {
        std::ifstream file(filename);
        if (!file.is_open()) {
            std::cerr << "Error: Cannot open " << filename << std::endl;
            return false;
        }
        
        std::string line;
        int addr = 0;
        while (std::getline(file, line) && addr < 256) {
            if (line.empty() || line[0] == '#') continue;
            
            // Parse hex instruction
            uint32_t instr = std::stoul(line, nullptr, 16);
            imem[addr++] = instr;
        }
        
        std::cout << "Loaded " << addr << " instructions from " << filename << std::endl;
        file.close();
        return true;
    }
    
    // Load data memory from hex file
    bool load_dmem(const std::string& filename) {
        std::ifstream file(filename);
        if (!file.is_open()) {
            std::cerr << "Warning: Cannot open " << filename << std::endl;
            return false;
        }
        
        std::string line;
        int addr = 0;
        while (std::getline(file, line) && addr < 256) {
            if (line.empty() || line[0] == '#') continue;
            
            // Parse hex byte
            uint8_t byte = std::stoul(line, nullptr, 16);
            dmem[addr++] = byte;
        }
        
        std::cout << "Loaded " << addr << " bytes from " << filename << std::endl;
        file.close();
        return true;
    }
    
    // Execute one instruction
    void step() {
        // Fetch
        uint32_t instr = imem[(pc >> 2) & 0xFF];
        uint32_t current_pc = pc;
        
        // Decode
        decode(instr);
        
        // Execute
        uint32_t next_pc = pc + 4;
        uint32_t alu_result = 0;
        bool branch_taken = false;
        
        switch (opcode) {
            case 0b0110011: // R-type: ADD
                if (funct7 == 0x00 && funct3 == 0x0) {
                    alu_result = read_gpr(rs1) + read_gpr(rs2);
                    write_gpr(rd, alu_result);
                }
                break;
                
            case 0b0010011: // I-type: ADDI
                if (funct3 == 0x0) {
                    alu_result = read_gpr(rs1) + imm_i;
                    write_gpr(rd, alu_result);
                }
                break;
                
            case 0b0110111: // U-type: LUI
                alu_result = imm_u << 12;
                write_gpr(rd, alu_result);
                break;
                
            case 0b0000011: // I-type: Load
                alu_result = read_gpr(rs1) + imm_i;
                if (funct3 == 0b010) { // LW
                    write_gpr(rd, load_word(alu_result));
                } else if (funct3 == 0b100) { // LBU
                    write_gpr(rd, load_byte_unsigned(alu_result));
                }
                break;
                
            case 0b0100011: // S-type: Store
                alu_result = read_gpr(rs1) + imm_i;
                if (funct3 == 0b010) { // SW
                    store_word(alu_result, read_gpr(rs2));
                } else if (funct3 == 0b000) { // SB
                    store_byte(alu_result, read_gpr(rs2));
                }
                break;
                
            case 0b1100111: // I-type: JALR
                if (funct3 == 0x0) {
                    alu_result = (read_gpr(rs1) + imm_i) & ~1; // Clear LSB
                    write_gpr(rd, current_pc + 4);
                    next_pc = alu_result;
                    branch_taken = true;
                }
                break;
                
            default:
                // Unknown instruction - treat as NOP
                break;
        }
        
        // Update PC
        pc = next_pc;
    }
    
    // Print register state
    void print_state() {
        std::cout << "PC=0x" << std::hex << std::setw(8) << std::setfill('0') << pc << std::endl;
        for (int i = 0; i < 16; i++) {
            std::cout << "x" << std::dec << i << "=0x" << std::hex 
                      << std::setw(8) << std::setfill('0') << gpr[i];
            if (i % 4 == 3) std::cout << std::endl;
            else std::cout << " ";
        }
        std::cout << std::dec;
    }
    
    // Get register value
    uint32_t get_gpr(int index) const {
        return gpr[index & 0xF];
    }
    
    // Get PC
    uint32_t get_pc() const {
        return pc;
    }
    
    // Get memory byte
    uint8_t get_dmem(int addr) const {
        return dmem[addr & 0xFF];
    }
};

// Main test program
int main(int argc, char** argv) {
    RV32GoldenModel model;
    
    std::string imem_file = "imem.hex";
    std::string dmem_file = "dmem.hex";
    int num_cycles = 20;
    
    // Parse command line arguments
    if (argc > 1) imem_file = argv[1];
    if (argc > 2) dmem_file = argv[2];
    if (argc > 3) num_cycles = std::stoi(argv[3]);
    
    std::cout << "==== RV32 GOLDEN MODEL ====" << std::endl;
    
    // Load memories
    if (!model.load_imem(imem_file)) {
        return 1;
    }
    model.load_dmem(dmem_file);
    
    std::cout << "\nRunning for " << num_cycles << " cycles...\n" << std::endl;
    
    // Execute instructions
    for (int cycle = 0; cycle < num_cycles; cycle++) {
        model.step();
        std::cout << "\n=== Cycle " << cycle << " ===" << std::endl;
        model.print_state();
    }
    
    std::cout << "\n==== GOLDEN MODEL COMPLETED ====" << std::endl;
    
    return 0;
}
