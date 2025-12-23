#!/usr/bin/env python3
"""
Compare RTL testbench output with Golden Model output
Extracts register states and compares cycle-by-cycle
"""

import re
import sys

def parse_output(filename):
    """Parse testbench output to extract cycle-by-cycle register states"""
    cycles = {}
    
    with open(filename, 'r') as f:
        content = f.read()
    
    # Two patterns: one for combined line, one for separate lines
    # Pattern 1: === Cycle X === PC=0xYYYY
    pattern1 = r'=== Cycle (\w+) === PC=(0x[0-9a-fA-F]+)(?:[^\n]*\n)+((?:x\d+=0x[0-9a-fA-F]+\s*)+)'
    # Pattern 2: === Cycle X ===\nPC=0xYYYY
    pattern2 = r'=== Cycle (\w+) ===\s+PC=(0x[0-9a-fA-F]+)\s+((?:x\d+=0x[0-9a-fA-F]+\s*)+)'
    
    for pattern in [pattern1, pattern2]:
        for match in re.finditer(pattern, content):
            cycle_num = match.group(1)
            pc = match.group(2)
            regs_str = match.group(3)
            
            # Convert hex cycle number to decimal
            if cycle_num.isdigit():
                cycle = int(cycle_num)
            else:
                cycle = int(cycle_num, 16)
            
            # Parse registers
            regs = {}
            for reg_match in re.finditer(r'x(\d+)=(0x[0-9a-fA-F]+)', regs_str):
                reg_num = int(reg_match.group(1))
                reg_val = reg_match.group(2)
                regs[reg_num] = reg_val
            
            cycles[cycle] = {'pc': pc, 'regs': regs}
    
    return cycles

def compare_cycles(rtl_cycles, golden_cycles):
    """Compare RTL and Golden Model outputs"""
    
    print("=" * 80)
    print("RTL vs Golden Model Comparison")
    print("=" * 80)
    
    all_cycles = sorted(set(rtl_cycles.keys()) | set(golden_cycles.keys()))
    
    mismatches = 0
    matches = 0
    
    for cycle in all_cycles:
        if cycle not in rtl_cycles:
            print(f"\n❌ Cycle {cycle}: Missing in RTL output")
            mismatches += 1
            continue
        
        if cycle not in golden_cycles:
            print(f"\n❌ Cycle {cycle}: Missing in Golden Model output")
            mismatches += 1
            continue
        
        rtl = rtl_cycles[cycle]
        golden = golden_cycles[cycle]
        
        # Compare PC
        pc_match = rtl['pc'].lower() == golden['pc'].lower()
        
        # Compare registers
        reg_mismatches = []
        for reg_num in range(16):
            rtl_val = rtl['regs'].get(reg_num, '0x0')
            golden_val = golden['regs'].get(reg_num, '0x0')
            
            if rtl_val.lower() != golden_val.lower():
                reg_mismatches.append((reg_num, rtl_val, golden_val))
        
        if pc_match and len(reg_mismatches) == 0:
            matches += 1
            print(f"✅ Cycle {cycle}: MATCH (PC={rtl['pc']})")
        else:
            mismatches += 1
            print(f"\n❌ Cycle {cycle}: MISMATCH")
            
            if not pc_match:
                print(f"   PC: RTL={rtl['pc']} vs Golden={golden['pc']}")
            
            for reg_num, rtl_val, golden_val in reg_mismatches:
                print(f"   x{reg_num}: RTL={rtl_val} vs Golden={golden_val}")
    
    print("\n" + "=" * 80)
    print(f"Summary: {matches} matches, {mismatches} mismatches")
    print("=" * 80)
    
    return mismatches == 0

if __name__ == "__main__":
    if len(sys.argv) != 3:
        print("Usage: python3 compare_outputs.py <rtl_output.txt> <golden_output.txt>")
        sys.exit(1)
    
    rtl_file = sys.argv[1]
    golden_file = sys.argv[2]
    
    print(f"Parsing RTL output from {rtl_file}...")
    rtl_cycles = parse_output(rtl_file)
    print(f"Found {len(rtl_cycles)} cycles in RTL output")
    
    print(f"Parsing Golden Model output from {golden_file}...")
    golden_cycles = parse_output(golden_file)
    print(f"Found {len(golden_cycles)} cycles in Golden Model output\n")
    
    success = compare_cycles(rtl_cycles, golden_cycles)
    
    sys.exit(0 if success else 1)
