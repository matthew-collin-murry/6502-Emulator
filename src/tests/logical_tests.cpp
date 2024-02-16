#include "./tests/logical_tests.h"

void LogicalTests::check_logical_unmodified_flags()
{
    EXPECT_EQ(cpu.flag.C, default_cpu_state.flag.C);
    EXPECT_EQ(cpu.flag.I, default_cpu_state.flag.I);
    EXPECT_EQ(cpu.flag.D, default_cpu_state.flag.D);
    EXPECT_EQ(cpu.flag.B, default_cpu_state.flag.B);
    EXPECT_EQ(cpu.flag.V, default_cpu_state.flag.V);
}

constexpr auto and_lambda = [](byte a, byte memory){ return a & memory; };
constexpr auto eor_lambda = [](byte a, byte memory){ return a ^ memory; };
constexpr auto or_lambda = [](byte a, byte memory){ return a | memory; };

TEST_F(LogicalTests, AND_IM)
{
    test_logical_im(CPU::INS_AND_IM, and_lambda);
}

TEST_F(LogicalTests, AND_ZP)
{
    test_logical_zp(CPU::INS_AND_ZP, and_lambda);
}

TEST_F(LogicalTests, AND_ZPX)
{
    test_logical_zpx(CPU::INS_AND_ZPX, and_lambda);
}

TEST_F(LogicalTests, AND_ABS)
{
    test_logical_abs(CPU::INS_AND_ABS, and_lambda);
}

TEST_F(LogicalTests, AND_AX)
{
    test_logical_ax(CPU::INS_AND_AX, and_lambda);
}

TEST_F(LogicalTests, AND_AY)
{
    test_logical_ay(CPU::INS_AND_AY, and_lambda);
}

TEST_F(LogicalTests, AND_IX)
{
    test_logical_ix(CPU::INS_AND_IX, and_lambda);
}

TEST_F(LogicalTests, AND_IY)
{
    test_logical_iy(CPU::INS_AND_IY, and_lambda);
}

// EOR Tests ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

TEST_F(LogicalTests, EOR_IM)
{
    test_logical_im(CPU::INS_EOR_IM, eor_lambda);
}

TEST_F(LogicalTests, EOR_ZP)
{
    test_logical_zp(CPU::INS_EOR_ZP, eor_lambda);
}

TEST_F(LogicalTests, EOR_ZPX)
{
    test_logical_zpx(CPU::INS_EOR_ZPX, eor_lambda);
}

TEST_F(LogicalTests, EOR_ABS)
{
    test_logical_abs(CPU::INS_EOR_ABS, eor_lambda);
}

TEST_F(LogicalTests, EOR_AX)
{
    test_logical_ax(CPU::INS_EOR_AX, eor_lambda);
}

TEST_F(LogicalTests, EOR_AY)
{
    test_logical_ay(CPU::INS_EOR_AY, eor_lambda);
}

TEST_F(LogicalTests, EOR_IX)
{
    test_logical_ix(CPU::INS_EOR_IX, eor_lambda);
}

TEST_F(LogicalTests, EOR_IY)
{
    test_logical_iy(CPU::INS_EOR_IY, eor_lambda);
}

// ORA Tests ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

TEST_F(LogicalTests, ORA_IM)
{
    test_logical_im(CPU::INS_ORA_IM, or_lambda);
}

TEST_F(LogicalTests, ORA_ZP)
{
    test_logical_zp(CPU::INS_ORA_ZP, or_lambda);
}

TEST_F(LogicalTests, ORA_ZPX)
{
    test_logical_zpx(CPU::INS_ORA_ZPX, or_lambda);
}

TEST_F(LogicalTests, ORA_ABS)
{
    test_logical_abs(CPU::INS_ORA_ABS, or_lambda);
}

TEST_F(LogicalTests, ORA_AX)
{
    test_logical_ax(CPU::INS_ORA_AX, or_lambda);
}

TEST_F(LogicalTests, ORA_AY)
{
    test_logical_ay(CPU::INS_ORA_AY, or_lambda);
}

TEST_F(LogicalTests, ORA_IX)
{
    test_logical_ix(CPU::INS_ORA_IX, or_lambda);
}

TEST_F(LogicalTests, ORA_IY)
{
    test_logical_iy(CPU::INS_ORA_IY, or_lambda);
}

// TODO Logical Operations flag tests