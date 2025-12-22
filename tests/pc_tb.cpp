#include <verilated.h>                  // Verilator main header (simulation engine API)
#include <iostream>                     // For std::cout / std::cerr
#include <iomanip>                      // For std::setw / std::setfill formatting
#include "Vpc.h"                        // Generated model header for pc

int main(int argc, char** argv){
    Verilated::commandArgs(argc, argv); // Pass command-line args to Verilator (e.g., +trace)
    auto* top = new Vpc;          // Create instance of the top module
    top->clk = 0;                  // Initialize clock
    top->rst = 1;                // Start with reset active
    top->eval();                   // Initial evaluation with reset
    top->rst = 0;                // Deactivate reset
    const int sim_cycles = 20;     // Number of clock cycles to simulate
    for (int cycle = 0; cycle < sim_cycles; ++cycle) {
        // Toggle clock
        top->clk = 1;
        top->eval();               // Evaluate with clock high
        // Print current PC value
        std::cout << "Cycle " << std::setw(2) << std::setfill('0') << cycle 
                  << ": PC = 0x" << std::hex << top->pc_out << std::dec << std::endl;
        top->clk = 0;
        top->eval();               // Evaluate with clock low
    }
    delete top;                    // Clean up
    return 0;
}
