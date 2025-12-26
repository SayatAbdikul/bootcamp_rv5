#include "memory.h"

#include <cstdio>
#include <cstring>
#include <string>

static unsigned char memory[MEM_SIZE];
static bool initialized = false;

static inline uint32_t clamp_addr(uint32_t addr) {
    // Word-align and clamp to MEM_SIZE-4 (avoid overflow on last word)
    uint32_t aligned = addr & ~0x3u;
    if (aligned >= MEM_SIZE - 4) return (MEM_SIZE - 4);
    return aligned;
}

extern "C" void mem_init(const char *path) {
    if (initialized) return;
    initialized = true;
    std::memset(memory, 0, sizeof(memory));

    const char *file = path && path[0] ? path : "imem.hex";
    FILE *fp = std::fopen(file, "r");
    if (!fp) {
        std::perror("mem_init fopen");
        return;
    }
    char line[128];
    uint32_t addr = 0;
    while (std::fgets(line, sizeof(line), fp)) {
        // Skip empty/comment lines
        if (line[0] == '#' || line[0] == '\n' || line[0] == '\0') continue;
        uint32_t word = 0;
        if (std::sscanf(line, "%x", &word) != 1) continue;
        uint32_t a = clamp_addr(addr);
        memory[a + 0] = (word >> 0) & 0xFF;
        memory[a + 1] = (word >> 8) & 0xFF;
        memory[a + 2] = (word >> 16) & 0xFF;
        memory[a + 3] = (word >> 24) & 0xFF;
        addr += 4;
        if (addr >= MEM_SIZE) break;
    }
    std::fclose(fp);
}

extern "C" int mem_read(int raddr) {
    if (!initialized) mem_init(nullptr);
    uint32_t a = clamp_addr(static_cast<uint32_t>(raddr));
    uint32_t word = 0;
    word |= static_cast<uint32_t>(memory[a + 0]) << 0;
    word |= static_cast<uint32_t>(memory[a + 1]) << 8;
    word |= static_cast<uint32_t>(memory[a + 2]) << 16;
    word |= static_cast<uint32_t>(memory[a + 3]) << 24;
    return static_cast<int>(word);
}

extern "C" void mem_write(int waddr, int wdata, unsigned char wmask) {
    if (!initialized) mem_init(nullptr);
    uint32_t a = clamp_addr(static_cast<uint32_t>(waddr));
    uint32_t data = static_cast<uint32_t>(wdata);
    if (wmask & 0x1) memory[a + 0] = (data >> 0) & 0xFF;
    if (wmask & 0x2) memory[a + 1] = (data >> 8) & 0xFF;
    if (wmask & 0x4) memory[a + 2] = (data >> 16) & 0xFF;
    if (wmask & 0x8) memory[a + 3] = (data >> 24) & 0xFF;
}
