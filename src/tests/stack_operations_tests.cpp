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
    EXPECT_TRUE(cpu.flag.Z);
    EXPECT_FALSE(cpu.flag.N);
}

TEST_F(StackOperationsTests, TSX_NEG_FLAG)
{
    cpu.SP = 0xFF;
    mem[0xFFFC] = CPU::INS_TSX;
    auto cycles_used = cpu.execute(2);
    EXPECT_EQ(cycles_used, 2);
    EXPECT_EQ(cpu.X, cpu.SP);
    EXPECT_TRUE(cpu.flag.N);
    EXPECT_FALSE(cpu.flag.Z);
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
    cpu.PS = 0x4C;
    auto cycles_used = cpu.execute(3);
    EXPECT_EQ(cycles_used, 3);
    EXPECT_EQ(mem[cpu.sp_to_address() + 1], cpu.PS);
}

TEST_F(StackOperationsTests, PLA)
{
    mem[0xFFFC] = CPU::INS_PLA;
    mem[0x01FF] = 0x42;
    auto cycles_used = cpu.execute(4);
    EXPECT_EQ(cycles_used, 4);
    EXPECT_EQ(cpu.A, 0x42);
}

TEST_F(StackOperationsTests, PLA_ZERO_FLAG)
{
    mem[0xFFFC] = CPU::INS_PLA;
    mem[0x01FF] = 0x0;
    auto cycles_used = cpu.execute(4);
    EXPECT_EQ(cycles_used, 4);
    EXPECT_EQ(cpu.A, 0x0);
    EXPECT_TRUE(cpu.flag.Z);
    EXPECT_FALSE(cpu.flag.N);
}

TEST_F(StackOperationsTests, PLA_NEG_FLAG)
{
    mem[0xFFFC] = CPU::INS_PLA;
    mem[0x01FF] = 0xFF;
    auto cycles_used = cpu.execute(4);
    EXPECT_EQ(cycles_used, 4);
    EXPECT_EQ(cpu.A, 0xFF);
    EXPECT_TRUE(cpu.flag.N);
    EXPECT_FALSE(cpu.flag.Z);
}

TEST_F(StackOperationsTests, PLP)
{
    mem[0xFFFC] = CPU::INS_PLP;
    mem[0x01FF] = 0xF4;
    auto cycles_used = cpu.execute(4);
    EXPECT_EQ(cycles_used, 4);
    EXPECT_EQ(cpu.PS, 0xF4);
}