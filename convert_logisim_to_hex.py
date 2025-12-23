#!/usr/bin/env python3
"""Convert Logisim hex format to standard hex format for $readmemh"""

import sys
import re

def convert_logisim_to_hex(input_file, output_file):
    with open(input_file, 'r') as f:
        lines = f.readlines()
    
    # Skip the first line if it's the Logisim header
    start_idx = 0
    if lines[0].strip().startswith('v3.0'):
        start_idx = 1
    
    # Dictionary to store address -> instruction mapping
    memory = {}
    max_addr = 0
    
    for line in lines[start_idx:]:
        line = line.strip()
        if not line:
            continue
        
        # Parse address prefix (e.g., "00000:")
        if ':' in line:
            parts = line.split(':', 1)
            addr_str = parts[0].strip()
            data_str = parts[1].strip()
            
            # Address in Logisim is WORD-addressed
            # Convert to word index (not byte address yet)
            base_addr = int(addr_str, 16)
            
            # Split by whitespace and add each instruction
            words = data_str.split()
            for i, word in enumerate(words):
                if word:  # Skip empty strings
                    word_addr = base_addr + i
                    memory[word_addr] = word
                    max_addr = max(max_addr, word_addr)
    
    # Write to output file with proper address spacing
    # Fill gaps with zeros
    with open(output_file, 'w') as f:
        for addr in range(max_addr + 1):
            if addr in memory:
                f.write(f"{memory[addr]}\n")
            else:
                f.write("00000000\n")
    
    print(f"Converted {len(memory)} instructions from {input_file} to {output_file}")
    print(f"Memory spans from word 0 to word {max_addr} (byte address 0x{max_addr*4:x})")

if __name__ == "__main__":
    if len(sys.argv) != 3:
        print("Usage: python3 convert_logisim_to_hex.py <input.hex> <output.hex>")
        sys.exit(1)
    
    convert_logisim_to_hex(sys.argv[1], sys.argv[2])
