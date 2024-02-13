#include "gtest/gtest.h"
#include "m6502.h"

using namespace emulator6502;

class JumpReturnTests : public testing::Test
{
public:
    Memory mem;
    CPU cpu;
    CPU default_cpu_state;

    JumpReturnTests()
        : cpu(CPU(mem)), default_cpu_state(CPU(mem))
    {}

    virtual void SetUp()
    {
        cpu.reset();
    }
};

TEST_F(JumpReturnTests, JSR_Basic)
{
    mem[0xFFFC] = CPU::INS_JSR;
    mem.write_word(0x4242, 0xFFFD);
    mem[0x4242] = CPU::INS_LDA_IM;
    mem[0x4243] = 0x84;
    auto cycles_used = cpu.execute(8);
    EXPECT_EQ(cpu.A, 0x84);
    EXPECT_EQ(cycles_used, 8);
}

TEST_F(JumpReturnTests, JSR_RTS)
{
    cpu.reset(0xFF00);
    mem[0xFF00] = CPU::INS_JSR;
    mem.write_word(0x4242, 0xFF01);
    mem[0x4242] = CPU::INS_RTS;
    mem[0xFF03] = CPU::INS_LDA_IM;
    mem[0xFF04] = 0x42;
    auto cycles_used = cpu.execute(14);
    EXPECT_EQ(cpu.A, 0x42);
    EXPECT_EQ(cycles_used, 14);
}