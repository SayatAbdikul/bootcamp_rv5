#include <verilated.h>
#include <verilated_vcd_c.h>
#include <iostream>
#include "Vcore.h"

using namespace std;

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

    cout << "==== CORE TESTBENCH ====\n";

    // -------------------------
    // Reset
    // -------------------------
    cout << "Applying reset...\n";
    dut->rst = 1;
    tick(dut, tfp, time);
    tick(dut, tfp, time);
    dut->rst = 0;
    
    cout << "Running core for 20 clock cycles...\n";
    
    // -------------------------
    // Run for N cycles
    // -------------------------
    for (int cycle = 0; cycle < 20; cycle++) {
        tick(dut, tfp, time);
        
        cout << "\n=== Cycle " << cycle << " ===";
        cout << " PC=0x" << hex << dut->pc_out << dec << "\n";
        
        // Print all 16 registers in a formatted way
        for (int i = 0; i < 16; i++) {
            cout << "x" << dec << i << "=0x" << hex << dut->registers_out[i];
            if (i % 4 == 3) cout << "\n";
            else cout << " ";
        }
    }

    cout << "==== CORE TEST COMPLETED ====\n";
    cout << "Waveform saved to core_tb.vcd\n";

    tfp->close();
    delete tfp;
    delete dut;
    return 0;
}
