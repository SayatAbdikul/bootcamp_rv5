#include <iostream>
#include <fstream>
#include <iomanip>
#include <cstdint>

using namespace std;

int main() {
    ifstream file("imem.hex");
    string line;
    int addr = 0;
    
    cout << "Instructions around PC=0x6500:" << endl;
    while (getline(file, line) && addr < 256) {
        if (line.empty() || line[0] == '#') continue;
        
        uint32_t instr = stoul(line, nullptr, 16);
        uint32_t pc = addr * 4;
        
        // Show instructions around the problem area
        if (pc >= 0x6500 - 32 && pc <= 0x6510 + 32) {
            uint32_t opcode = instr & 0x7F;
            uint32_t rd = (instr >> 7) & 0x1F;
            uint32_t funct3 = (instr >> 12) & 0x07;
            uint32_t rs1 = (instr >> 15) & 0x1F;
            uint32_t rs2 = (instr >> 20) & 0x1F;
            int32_t imm_i = static_cast<int32_t>(instr) >> 20;
            uint32_t imm_u = instr >> 12;
            
            cout << "0x" << hex << setw(8) << setfill('0') << pc << ": ";
            cout << "0x" << setw(8) << instr << " ";
            
            if (opcode == 0b0110111) {
                cout << "lui x" << dec << rd << ", 0x" << hex << imm_u;
            } else if (opcode == 0b0010011 && funct3 == 0) {
                cout << "addi x" << dec << rd << ", x" << rs1 << ", " << imm_i;
            } else if (opcode == 0b1100111) {
                cout << "jalr x" << dec << rd << ", " << imm_i << "(x" << rs1 << ")";
            } else if (opcode == 0b0100011 && funct3 == 0) {
                cout << "sb x" << dec << rs2 << ", " << imm_i << "(x" << rs1 << ")";
            } else if (opcode == 0b0000011 && funct3 == 4) {
                cout << "lbu x" << dec << rd << ", " << imm_i << "(x" << rs1 << ")";
            } else {
                cout << "??? (opcode=" << hex << opcode << ")";
            }
            
            if (rd == 4) cout << " <-- writes to x4!";
            
            cout << endl;
        }
        addr++;
    }
    
    return 0;
}
