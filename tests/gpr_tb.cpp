#include <verilated.h>
#include <iostream>
#include <cassert>
#include "Vgpr.h"

using namespace std;

// Simple clock tick helper
void tick(Vgpr* dut) {
    dut->clk = 0;
    dut->eval();
    dut->clk = 1;
    dut->eval();
}

int main(int argc, char** argv) {
    Verilated::commandArgs(argc, argv);

    Vgpr* dut = new Vgpr;

    cout << "==== GPR TESTBENCH ====\n";

    // -------------------------
    // Reset
    // -------------------------
    dut->rst = 1;
    dut->write_enable = 0;
    dut->rs1 = 0;
    dut->rs2 = 0;
    dut->rd  = 0;
    dut->write_data = 0;

    tick(dut);  // apply reset
    dut->rst = 0;

    // Check reset cleared registers
    for (int i = 0; i < 16; i++) {
        dut->rs1 = i;
        dut->eval();
        assert(dut->read_data1 == 0);
    }

    cout << "Reset test passed\n";

    // -------------------------
    // Write register 3
    // -------------------------
    dut->rd = 3;
    dut->write_data = 0xDEADBEEF;
    dut->write_enable = 1;
    tick(dut);
    dut->write_enable = 0;

    // Read back register 3
    dut->rs1 = 3;
    dut->eval();
    cout << "Reg[3] = 0x" << hex << dut->read_data1 << dec << endl;
    assert(dut->read_data1 == 0xDEADBEEF);

    // -------------------------
    // Write register 7
    // -------------------------
    dut->rd = 7;
    dut->write_data = 12345;
    dut->write_enable = 1;
    tick(dut);
    dut->write_enable = 0;

    // Read both ports simultaneously
    dut->rs1 = 3;
    dut->rs2 = 7;
    dut->eval();

    cout << "Reg[3] = " << dut->read_data1 << endl;
    cout << "Reg[7] = " << dut->read_data2 << endl;

    assert(dut->read_data1 == 0xDEADBEEF);
    assert(dut->read_data2 == 12345);

    // -------------------------
    // Overwrite test
    // -------------------------
    dut->rd = 3;
    dut->write_data = 42;
    dut->write_enable = 1;
    tick(dut);
    dut->write_enable = 0;

    dut->rs1 = 3;
    dut->eval();
    assert(dut->read_data1 == 42);

    cout << "Overwrite test passed\n";

    cout << "==== ALL GPR TESTS PASSED ====\n";

    delete dut;
    return 0;
}
