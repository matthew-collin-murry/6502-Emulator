#include "m6502.h"

// CPU Emulator (6502)
// http://www.6502.org/users/obelisk/6502/index.html
// based off of: https://www.youtube.com/watch?v=qJgsuQoy9bc

using namespace emulator6502;

int main()
{
    Memory mem;
    CPU cpu(mem);
    cpu.reset();

    mem[0xFFFC] = CPU::INS_JSR;
    mem[0xFFFD] = 0x42;
    mem[0xFFFE] = 0x42;

    cpu.execute(6);

    return 0;
}