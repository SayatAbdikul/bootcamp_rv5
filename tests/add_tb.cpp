#include <verilated.h>
#include <iostream>
#include <cassert>
#include "Vadd.h"

using namespace std;

int main(int argc, char** argv) {
    Verilated::commandArgs(argc, argv);

    Vadd* dut = new Vadd;

    cout << "==== ADD / ADDI TESTBENCH ====\n";

    // ----------------------------
    // Test 1: ADD (R-type)
    // a = 10, b = 20
    // ----------------------------
    dut->a = 10;
    dut->b = 20;
    dut->imm = 0;
    dut->add_enable = 1;
    dut->addi_enable = 0;
    dut->eval();

    cout << "ADD: 10 + 20 = " << dut->result << endl;
    assert(dut->result == 30);

    // ----------------------------
    // Test 2: ADDI (positive imm)
    // a = 100, imm = 5
    // ----------------------------
    dut->a = 100;
    dut->b = 0;
    dut->imm = 5;
    dut->add_enable = 0;
    dut->addi_enable = 1;
    dut->eval();

    cout << "ADDI: 100 + 5 = " << dut->result << endl;
    assert(dut->result == 105);

    // ----------------------------
    // Test 3: ADDI (negative imm)
    // imm = -3 (12-bit)
    // ----------------------------
    dut->a = 50;
    dut->imm = 0xFFD;   // -3 in 12-bit two's complement
    dut->add_enable = 0;
    dut->addi_enable = 1;
    dut->eval();

    cout << "ADDI: 50 + (-3) = " << dut->result << endl;
    assert(dut->result == 47);

    // ----------------------------
    // Test 4: Priority check
    // both enables = 1 → ADD wins
    // ----------------------------
    dut->a = 7;
    dut->b = 8;
    dut->imm = 100;
    dut->add_enable = 1;
    dut->addi_enable = 1;
    dut->eval();

    cout << "PRIORITY: result = " << dut->result << endl;
    assert(dut->result == 15);

    cout << "==== ALL TESTS PASSED ====\n";

    delete dut;
    return 0;
}
