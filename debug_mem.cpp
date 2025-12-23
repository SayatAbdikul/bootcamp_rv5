#include <iostream>
#include <fstream>
#include <iomanip>
using namespace std;

int main() {
    // Check what's in memory at different addresses
    uint8_t dmem[256] = {0};
    
    ifstream file("dmem.hex");
    if (file.is_open()) {
        string line;
        int addr = 0;
        while (getline(file, line) && addr < 256) {
            if (line.empty()) continue;
            dmem[addr++] = stoul(line, nullptr, 16);
        }
        cout << "Loaded " << addr << " bytes from dmem.hex" << endl;
    } else {
        cout << "dmem.hex is empty or doesn't exist" << endl;
    }
    
    // If x4 was some value before LW, what would we read?
    // Let's check a few possible values
    uint32_t test_values[] = {0x0, 0x14, 0x100, 0x200, 0x6514};
    
    for (uint32_t x4_old : test_values) {
        uint32_t addr = (x4_old - 20) & 0xFF;
        uint32_t word = (dmem[addr+3] << 24) | (dmem[addr+2] << 16) | 
                       (dmem[addr+1] << 8) | dmem[addr];
        cout << "If x4=" << hex << "0x" << x4_old << ", LW from addr=0x" << addr 
             << " would give 0x" << word << dec << endl;
    }
    
    return 0;
}
