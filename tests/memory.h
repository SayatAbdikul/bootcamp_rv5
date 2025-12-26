#pragma once
#include <stdint.h>

#define MEM_SIZE (128 * 1024 * 1024)

#ifdef __cplusplus
extern "C" {
#endif

// Initialize memory from a hex file (one 32-bit word per line, little-endian layout).
// If path is null, defaults to "imem.hex".
void mem_init(const char *path);

// Read a 32-bit word from an address (word-aligned internally).
int mem_read(int raddr);

// Write bytes according to wmask to a 32-bit word at address.
// wmask bit 0 -> byte 0 (LSB), bit 3 -> byte 3 (MSB).
void mem_write(int waddr, int wdata, unsigned char wmask);

#ifdef __cplusplus
}
#endif
