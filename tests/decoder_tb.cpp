#include <verilated.h>                  // Verilator main header (simulation engine API)
#include <iostream>                     // For std::cout / std::cerr
#include <iomanip>                      // For std::setw / std::setfill formatting
#include "Vdecoder.h"                        // Generated model header for pc
using namespace std;
int main(int argc, char** argv){
    Verilated::commandArgs(argc, argv); // Pass command-line args to Verilator (e.g., +trace)
    auto* top = new Vdecoder;          // Create instance of the top module
    int32_t instructions[] = {
        0x007302B3, // add x5, x6, x7
        0x00A30293, // addi x5, x6, 10
        0x123452B7, // lui x5, 0x12345
        0x00832303, // lw x6, 8(x6)
        0x00334283, // lbu x5, 3(x6)
        0x00532623, // sw x5, 12(x6)
        0x005300A3, // sb x5, 1(x6)
        0x000280E7 // jalr x1, 0(x5)
    };
    for(int i = 0; i < 8; i++) {
        top->instruction = instructions[i];
        top->eval();                   // Initial evaluation with reset
        cout << "Instruction: 0x"
     << hex << setw(8) << setfill('0')
     << instructions[i] << dec << "\n";

cout << "  opcode : 0x" << hex << (int)top->opcode << dec << "\n";
cout << "  funct3 : 0x" << hex << (int)top->funct3 << dec << "\n";
cout << "  funct7 : 0x" << hex << (int)top->funct7 << dec << "\n";

cout << "  rd     : " << (int)top->rd << "\n";
cout << "  rs1    : " << (int)top->rs1 << "\n";
cout << "  rs2    : " << (int)top->rs2 << "\n";

cout << "  imm_u  : " << (int)top->imm_u << "\n";
cout << "  imm_i  : " << (int)top->imm_i << "\n";

cout << "------------------------\n";

    }
    delete top;                    // Clean up
    return 0;
}
