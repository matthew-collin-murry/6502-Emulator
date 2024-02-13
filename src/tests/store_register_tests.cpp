#include "gtest/gtest.h"
#include "m6502.h"

using namespace emulator6502;

class StoreRegisterTests : public testing::Test
{
public:
    Memory mem;
    CPU cpu;
    CPU default_cpu_state;

    StoreRegisterTests()
        : cpu(CPU(mem)), default_cpu_state(CPU(mem))
    {}

    virtual void SetUp()
    {
        cpu.reset();
    }

    void flags_are_default()
    {
        EXPECT_EQ(cpu.C, default_cpu_state.C);
        EXPECT_EQ(cpu.Z, default_cpu_state.Z);
        EXPECT_EQ(cpu.I, default_cpu_state.I);
        EXPECT_EQ(cpu.D, default_cpu_state.D);
        EXPECT_EQ(cpu.B, default_cpu_state.B);
        EXPECT_EQ(cpu.V, default_cpu_state.V);
        EXPECT_EQ(cpu.N, default_cpu_state.N);
    }

    void test_store_register_zp(byte opcode, byte CPU::*reg);
    // ZPX, ZPY
    void test_store_register_zp_(byte opcode, byte CPU::*store, byte CPU::*offset);
    void test_store_register_abs(byte opcode, byte CPU::*reg);
    // AX, AY
    void test_store_register_abs_(byte opcode, byte CPU::*store, byte CPU::*offset);
    // IX, IY
    void test_store_register_i_(byte opcode, byte CPU::*store, byte CPU::*offset);
};

void StoreRegisterTests::test_store_register_zp(byte opcode, byte CPU::*reg)
{
    mem[0xFFFC] = opcode;
    mem[0xFFFD] = 0x0F;
    cpu.*reg = 0x12;
    s32 cycles_used = cpu.execute(3);
    EXPECT_EQ(mem[0x0F], cpu.*reg);
    EXPECT_EQ(cycles_used, 3);
    flags_are_default();
}

void StoreRegisterTests::test_store_register_zp_(byte opcode, byte CPU::*store, byte CPU::*offset)
{
    mem[0xFFFC] = opcode;
    mem[0xFFFD] = 0x0F;
    cpu.*store = 0x12;
    cpu.*offset = 0x01;
    s32 cycles_used = cpu.execute(4);
    EXPECT_EQ(mem[0x10], cpu.*store);
    EXPECT_EQ(cycles_used, 4);
    flags_are_default();
}

void StoreRegisterTests::test_store_register_abs(byte opcode, byte CPU::*reg)
{
    mem[0xFFFC] = opcode;
    mem.write_word(0xFF10, 0xFFFD);
    cpu.*reg = 0x12;
    s32 cycles_used = cpu.execute(4);
    EXPECT_EQ(mem[0xFF10], cpu.*reg);
    EXPECT_EQ(cycles_used, 4);
    flags_are_default();
}

void StoreRegisterTests::test_store_register_abs_(byte opcode, byte CPU::*store, byte CPU::*offset)
{
    mem[0xFFFC] = opcode;
    mem.write_word(0xFF10, 0xFFFD);
    cpu.*store = 0x12;
    cpu.*offset = 0x1;
    s32 cycles_used = cpu.execute(4);
    EXPECT_EQ(mem[0xFF11], cpu.*store);
    EXPECT_EQ(cycles_used, 5);
    flags_are_default();
}

void StoreRegisterTests::test_store_register_i_(byte opcode, byte CPU::*store, byte CPU::*offset)
{
    mem[0xFFFC] = opcode;
    mem[0xFFFD] = 0x02;
    mem.write_word(0x8000, 0x0003);
    cpu.*store = 0x4;
    cpu.*offset = 0x1;
    auto cycles_used = cpu.execute(6);
    EXPECT_EQ(mem[0x8000], 0x4);
    EXPECT_EQ(cycles_used, 6);
    flags_are_default();
}

// ~~~~~~~~~~~~~~~~~~~~~~~~~~ Tests ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

// ~~~~~~~~~~~~~~~~~~~~~~~~~~ STA Tests ~~~~~~~~~~~~~~~~~~~~~~~~~~

TEST_F(StoreRegisterTests, STA_ZP)
{
    test_store_register_zp(CPU::INS_STA_ZP, &CPU::A);
}

TEST_F(StoreRegisterTests, STA_ZPX)
{
    test_store_register_zp_(CPU::INS_STA_ZPX, &CPU::A, &CPU::X);
}

TEST_F(StoreRegisterTests, STA_ABS)
{
    test_store_register_abs(CPU::INS_STA_ABS, &CPU::A);
}

TEST_F(StoreRegisterTests, STA_ABSX)
{
    test_store_register_abs_(CPU::INS_STA_AX, &CPU::A, &CPU::X);
}

TEST_F(StoreRegisterTests, STA_ABSY)
{
    test_store_register_abs_(CPU::INS_STA_AY, &CPU::A, &CPU::Y);
}

TEST_F(StoreRegisterTests, STA_IX)
{
    test_store_register_i_(CPU::INS_STA_IX, &CPU::A, &CPU::X);
}

TEST_F(StoreRegisterTests, STA_IY)
{
    test_store_register_i_(CPU::INS_STA_IY, &CPU::A, &CPU::Y);
}

// // ~~~~~~~~~~~~~~~~~~~~~~~~~~ STX Tests ~~~~~~~~~~~~~~~~~~~~~~~~~~

TEST_F(StoreRegisterTests, STX_ZP)
{
    test_store_register_zp(CPU::INS_STX_ZP, &CPU::X);
}

TEST_F(StoreRegisterTests, STX_ZPY)
{
    test_store_register_zp_(CPU::INS_STX_ZPY, &CPU::X, &CPU::Y);
}

TEST_F(StoreRegisterTests, STX_ABS)
{
    test_store_register_abs(CPU::INS_STX_ABS, &CPU::X);
}

// // ~~~~~~~~~~~~~~~~~~~~~~~~~~ STY Tests ~~~~~~~~~~~~~~~~~~~~~~~~~~

TEST_F(StoreRegisterTests, STY_ZP)
{
    test_store_register_zp(CPU::INS_STY_ZP, &CPU::Y);
}

TEST_F(StoreRegisterTests, STY_ZPX)
{
    test_store_register_zp_(CPU::INS_STY_ZPX, &CPU::Y, &CPU::X);
}

TEST_F(StoreRegisterTests, STY_ABS)
{
    test_store_register_abs(CPU::INS_STY_ABS, &CPU::Y);
}