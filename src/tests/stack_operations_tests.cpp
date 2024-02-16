#include "gtest/gtest.h"
#include "m6502.h"

using namespace emulator6502;

class StackOperationsTests : public testing::Test
{
public:
    Memory mem;
    CPU cpu;
    CPU default_cpu_state;

    StackOperationsTests()
        : cpu(CPU(mem)), default_cpu_state(CPU(mem))
    {}

    virtual void SetUp()
    {
        cpu.reset();
    }

    void flags_are_default()
    {
        EXPECT_EQ(cpu.PS, default_cpu_state.PS);
    }
};

TEST_F(StackOperationsTests, TSX)
{
    cpu.SP = 0xF1;
    mem[0xFFFC] = CPU::INS_TSX;
    auto cycles_used = cpu.execute(2);
    EXPECT_EQ(cycles_used, 2);
    EXPECT_EQ(cpu.X, cpu.SP);
}

TEST_F(StackOperationsTests, TSX_ZERO_FLAG)
{
    cpu.SP = 0x0;
    mem[0xFFFC] = CPU::INS_TSX;
    auto cycles_used = cpu.execute(2);
    EXPECT_EQ(cycles_used, 2);
    EXPECT_EQ(cpu.X, cpu.SP);
    EXPECT_TRUE(cpu.Flag.Z);
    EXPECT_FALSE(cpu.Flag.N);
}

TEST_F(StackOperationsTests, TSX_NEG_FLAG)
{
    cpu.SP = 0xFF;
    mem[0xFFFC] = CPU::INS_TSX;
    auto cycles_used = cpu.execute(2);
    EXPECT_EQ(cycles_used, 2);
    EXPECT_EQ(cpu.X, cpu.SP);
    EXPECT_TRUE(cpu.Flag.N);
    EXPECT_FALSE(cpu.Flag.Z);
}

TEST_F(StackOperationsTests, TXS)
{
    cpu.X = 0xF1;
    mem[0xFFFC] = CPU::INS_TXS;
    auto cycles_used = cpu.execute(2);
    EXPECT_EQ(cycles_used, 2);
    EXPECT_EQ(cpu.X, cpu.SP);
    flags_are_default();
}

TEST_F(StackOperationsTests, PHA)
{
    mem[0xFFFC] = CPU::INS_PHA;
    cpu.A = 0x42;
    auto cycles_used = cpu.execute(3);
    EXPECT_EQ(cycles_used, 3);
    EXPECT_EQ(cpu.A, mem[cpu.sp_to_address() + 1]);
    flags_are_default();
}

TEST_F(StackOperationsTests, PHP)
{
    mem[0xFFFC] = CPU::INS_PHP;
    auto cycles_used = cpu.execute(3);
    EXPECT_EQ(cycles_used, 3);
    EXPECT_EQ(cpu.X, cpu.SP);
    flags_are_default();
}