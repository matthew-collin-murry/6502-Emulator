#include "gtest/gtest.h"
#include "m6502.h"

using namespace emulator6502;

class LoadRegisterTests : public testing::Test
{
public:
    Memory mem;
    CPU cpu;
    CPU default_cpu_state;

    LoadRegisterTests()
        : cpu(CPU(mem)), default_cpu_state(CPU(mem))
    {
        default_cpu_state.reset();
    }

    virtual void SetUp()
    {
        cpu.reset();
    }

    void check_LD__unmodified_flags();
    void test_load_register_im(byte, byte CPU::*);
    void test_load_register_zp(byte, byte CPU::*);
    void test_load_register_zpx(byte, byte CPU::*);
    void test_load_register_zpy(byte, byte CPU::*);
    void test_load_register_abs(byte, byte CPU::*);
    void test_load_register_ax(byte, byte CPU::*);
    void test_load_register_ay(byte, byte CPU::*);
    void test_load_register_ix(byte, byte CPU::*);
    void test_load_register_iy(byte, byte CPU::*);
    void test_load_register_flags(byte);
};

void LoadRegisterTests::check_LD__unmodified_flags()
{
    EXPECT_EQ(cpu.C, default_cpu_state.C);
    EXPECT_EQ(cpu.I, default_cpu_state.I);
    EXPECT_EQ(cpu.D, default_cpu_state.D);
    EXPECT_EQ(cpu.B, default_cpu_state.B);
    EXPECT_EQ(cpu.V, default_cpu_state.V);
}

void LoadRegisterTests::test_load_register_im(byte opcode, byte CPU::*reg)
{
    mem[0xFFFC] = opcode;
    mem[0xFFFD] = 0x84;
    s32 cycles_used = cpu.execute(2);
    EXPECT_EQ(cpu.*reg, 0x84);
    EXPECT_EQ(cycles_used, 2);
    check_LD__unmodified_flags();
}

void LoadRegisterTests::test_load_register_zp(byte opcode, byte CPU::*reg)
{
    mem[0xFFFC] = opcode;
    mem[0xFFFD] = 0x0F;
    mem[0x000F] = 0x0004;
    auto cycles_used = cpu.execute(3);
    EXPECT_EQ(cpu.*reg, 0x0004);
    EXPECT_EQ(cycles_used, 3);
    check_LD__unmodified_flags();
}

void LoadRegisterTests::test_load_register_zpx(byte opcode, byte CPU::*reg)
{
    mem[0xFFFC] = opcode;
    mem[0xFFFD] = 0x000F;
    mem[0x0010] = 0x0004;
    cpu.X = 0x1;
    auto cycles_used = cpu.execute(4);
    EXPECT_EQ(cpu.*reg, 0x0004);
    EXPECT_EQ(cycles_used, 4);
    check_LD__unmodified_flags();

    cpu.reset();

    // wrap around check
    mem[0xFFFC] = opcode;
    mem[0xFFFD] = 0x80;
    mem[0x007F] = 0x4;
    cpu.X = 0xFF;
    cpu.execute(4);
    EXPECT_EQ(cpu.*reg, 0x4);
}

void LoadRegisterTests::test_load_register_zpy(byte opcode, byte CPU::*reg)
{
    mem[0xFFFC] = opcode;
    mem[0xFFFD] = 0x000F;
    mem[0x0010] = 0x0004;
    cpu.Y = 0x1;
    auto cycles_used = cpu.execute(4);
    EXPECT_EQ(cpu.*reg, 0x0004);
    EXPECT_EQ(cycles_used, 4);
    check_LD__unmodified_flags();

    cpu.reset();

    // wrap around check
    mem[0xFFFC] = opcode;
    mem[0xFFFD] = 0x80;
    mem[0x007F] = 0x4;
    cpu.Y = 0xFF;
    cpu.execute(4);
    EXPECT_EQ(cpu.*reg, 0x4);
}

void LoadRegisterTests::test_load_register_abs(byte opcode, byte CPU::*reg)
{
    mem[0xFFFC] = opcode;
    mem.write_word(0xFFFF, 0xFFFD);
    mem[0xFFFF] = 0x42;
    auto cycles_used = cpu.execute(4);
    EXPECT_EQ(cpu.*reg, 0x42);
    EXPECT_EQ(cycles_used, 4);
}

void LoadRegisterTests::test_load_register_ax(byte opcode, byte CPU::*reg)
{
    mem[0xFFFC] = opcode;
    mem.write_word(0x4480, 0xFFFD);
    mem[0x4481] = 0x42;
    cpu.X = 1;
    auto cycles_used = cpu.execute(4);
    EXPECT_EQ(cpu.*reg, 0x42);
    EXPECT_EQ(cycles_used, 4);

    cpu.reset();

    // page cross check
    mem[0xFFFC] = opcode;
    mem.write_word(0x4402, 0xFFFD);
    mem[0x4501] = 0x42; // 0x4402 + 0xFF
    // force page cross
    cpu.X = 0xFF;
    cycles_used = cpu.execute(5);
    EXPECT_EQ(cpu.*reg, 0x42);
    EXPECT_EQ(cycles_used, 5);
}

void LoadRegisterTests::test_load_register_ay(byte opcode, byte CPU::*reg)
{
    mem[0xFFFC] = opcode;
    mem.write_word(0x4480, 0xFFFD);
    mem[0x4481] = 0x42;
    cpu.Y = 1;
    auto cycles_used = cpu.execute(4);
    EXPECT_EQ(cpu.*reg, 0x42);
    EXPECT_EQ(cycles_used, 4);

    cpu.reset();

    // page cross check
    mem[0xFFFC] = opcode;
    mem.write_word(0x4402, 0xFFFD);
    mem[0x4501] = 0x42; // 0x4402 + 0xFF
    // force page cross
    cpu.Y = 0xFF;
    cycles_used = cpu.execute(5);
    EXPECT_EQ(cpu.*reg, 0x42);
    EXPECT_EQ(cycles_used, 5);
}

void LoadRegisterTests::test_load_register_ix(byte opcode, byte CPU::*reg)
{
    mem[0xFFFC] = opcode;
    mem[0xFFFD] = 0x02;
    mem.write_word(0x8000, 0x0006);
    mem[0x8000] = 0x42;
    cpu.X = 0x4;
    auto cycles_used = cpu.execute(6);
    EXPECT_EQ(cpu.*reg, 0x42);
    EXPECT_EQ(cycles_used, 6);
}

void LoadRegisterTests::test_load_register_iy(byte opcode, byte CPU::*reg)
{
    mem[0xFFFC] = opcode;
    mem[0xFFFD] = 0x02;
    mem.write_word(0x8000, 0x0006);
    mem[0x8000] = 0x42;
    cpu.Y = 0x4;
    auto cycles_used = cpu.execute(5);
    EXPECT_EQ(cpu.*reg, 0x42);
    EXPECT_EQ(cycles_used, 5);

    cpu.reset();

    // page cross check
    mem[0xFFFC] = opcode;
    mem[0xFFFD] = 0x02;
    mem[0x0001] = 0x00;
    mem[0x0002] = 0x80;
    mem[0x8000] = 0x42;
    cpu.Y = 0xFF;
    cycles_used = cpu.execute(6);
    EXPECT_EQ(cpu.*reg, 0x42);
    EXPECT_EQ(cycles_used, 6);
}

void LoadRegisterTests::test_load_register_flags(byte opcode)
{
    // test for Zero flag not being triggered
    mem[0xFFFC] = opcode;
    mem[0xFFFD] = 0x0004;
    cpu.execute(2);
    EXPECT_FALSE(cpu.Z);

    cpu.reset();

    // test for Zero flag being triggered
    mem[0xFFFC] = opcode;
    mem[0xFFFD] = 0x0;
    cpu.execute(2);
    EXPECT_TRUE(cpu.Z);

    cpu.reset();

    // test for Negative flag being triggered
    mem[0xFFFC] = opcode;
    mem[0xFFFD] = 0x80;
    cpu.execute(2);
    EXPECT_TRUE(cpu.N);

    cpu.reset();

    // test for Negative flag not being triggered
    mem[0xFFFC] = opcode;
    mem[0xFFFD] = 0x0;
    cpu.execute(2);
    EXPECT_FALSE(cpu.N);
}

// ~~~~~~~~~~~~~~~~~~~~~~~~~~ Tests ~~~~~~~~~~~~~~~~~~~~~~~~~~

TEST_F(LoadRegisterTests, CPU_IDLE)
{
    constexpr s32 NUM_CYCLES = 0;
    auto cycles_used = cpu.execute(NUM_CYCLES);
    EXPECT_EQ(cycles_used, NUM_CYCLES);
}

TEST_F(LoadRegisterTests, UNKNOWN_INSTRUCTION)
{
    mem[0xFFFC] = 0x0; // invalid opcode
    
    EXPECT_THROW(cpu.execute(1), UnknownInstructionException);
}

// makes sure the cpu can execute more cycles than given to complete the instruction
TEST_F(LoadRegisterTests, CPU_NOT_ENOUGH_CYCLES_CONT)
{
    mem[0xFFFC] = CPU::INS_LDA_IM;
    mem[0xFFFD] = 0x0004;
    auto cycles_used = cpu.execute(1);
    EXPECT_EQ(cycles_used, 2);
}

// ~~~~~~~~~~~~~~~~~~~~~~~~~~ LDA Tests ~~~~~~~~~~~~~~~~~~~~~~~~~~

TEST_F(LoadRegisterTests, LDA_IM)
{
    test_load_register_im(CPU::INS_LDA_IM, &CPU::A);
}

TEST_F(LoadRegisterTests, LDA_ALL_MODES_FLAGS)
{
    test_load_register_flags(CPU::INS_LDA_IM);
    // TODO implemental all addressing modes
    // test_load_register_flags(CPU::INS_LDA_ZP);
    // test_load_register_flags(CPU::INS_LDA_ZPX);
    // test_load_register_flags(CPU::INS_LDA_ABS);
    // test_load_register_flags(CPU::INS_LDA_AX);
    // test_load_register_flags(CPU::INS_LDA_AY);
    // test_load_register_flags(CPU::INS_LDA_IX);
    // test_load_register_flags(CPU::INS_LDA_IY);
}

TEST_F(LoadRegisterTests, LDA_ZP)
{
    test_load_register_zp(CPU::INS_LDA_ZP, &CPU::A);
}

TEST_F(LoadRegisterTests, LDA_ZPX)
{
    test_load_register_zpx(CPU::INS_LDA_ZPX, &CPU::A);
}

TEST_F(LoadRegisterTests, LDA_ABS)
{
    test_load_register_abs(CPU::INS_LDA_ABS, &CPU::A);
}

TEST_F(LoadRegisterTests, LDA_ABSX)
{
    test_load_register_ax(CPU::INS_LDA_AX, &CPU::A);
}

TEST_F(LoadRegisterTests, LDA_ABSY)
{
    test_load_register_ay(CPU::INS_LDA_AY, &CPU::A);
}

TEST_F(LoadRegisterTests, LDA_IX)
{
    test_load_register_ix(CPU::INS_LDA_IX, &CPU::A);
}

TEST_F(LoadRegisterTests, LDA_IY)
{
    test_load_register_iy(CPU::INS_LDA_IY, &CPU::A);
}

// ~~~~~~~~~~~~~~~~~~~~~~~~~~ LDX Tests ~~~~~~~~~~~~~~~~~~~~~~~~~~

TEST_F(LoadRegisterTests, LDX_IM)
{
    test_load_register_im(CPU::INS_LDX_IM, &CPU::X);
}

TEST_F(LoadRegisterTests, LDX_ALL_MODES_FLAGS)
{
    test_load_register_flags(CPU::INS_LDX_IM);
    // TODO implemental all addressing modes
    // test_load_register_flags(CPU::INS_LDX_ZP);
    // test_load_register_flags(CPU::INS_LDX_ZPY);
    // test_load_register_flags(CPU::INS_LDX_ABS);
    // test_load_register_flags(CPU::INS_LDX_AY);
}

TEST_F(LoadRegisterTests, LDX_ZP)
{
    test_load_register_zp(CPU::INS_LDX_ZP, &CPU::X);
}

TEST_F(LoadRegisterTests, LDX_ZPY)
{
    test_load_register_zpy(CPU::INS_LDX_ZPY, &CPU::X);
}

TEST_F(LoadRegisterTests, LDX_ABS)
{
    test_load_register_abs(CPU::INS_LDX_ABS, &CPU::X);
}

TEST_F(LoadRegisterTests, LDX_ABSY)
{
    test_load_register_ay(CPU::INS_LDX_AY, &CPU::X);
}

// ~~~~~~~~~~~~~~~~~~~~~~~~~~ LDY Tests ~~~~~~~~~~~~~~~~~~~~~~~~~~

TEST_F(LoadRegisterTests, LDY_IM)
{
    test_load_register_im(CPU::INS_LDY_IM, &CPU::Y);
}

TEST_F(LoadRegisterTests, LDY_ALL_MODES_FLAGS)
{
    test_load_register_flags(CPU::INS_LDY_IM);
    // TODO implemental all addressing modes
    // test_load_register_flags(CPU::INS_LDX_ZP);
    // test_load_register_flags(CPU::INS_LDX_ZPY);
    // test_load_register_flags(CPU::INS_LDX_ABS);
    // test_load_register_flags(CPU::INS_LDX_AY);
}

TEST_F(LoadRegisterTests, LDY_ZP)
{
    test_load_register_zp(CPU::INS_LDY_ZP, &CPU::Y);
}

TEST_F(LoadRegisterTests, LDY_ZPX)
{
    test_load_register_zpx(CPU::INS_LDY_ZPX, &CPU::Y);
}

TEST_F(LoadRegisterTests, LDY_ABS)
{
    test_load_register_abs(CPU::INS_LDY_ABS, &CPU::Y);
}

TEST_F(LoadRegisterTests, LDY_ABSX)
{
    test_load_register_ax(CPU::INS_LDY_AX, &CPU::Y);
}

// ~~~~~~~~~~~~~~~~~~~~~~~~~~ JSR Tests ~~~~~~~~~~~~~~~~~~~~~~~~~~

TEST_F(LoadRegisterTests, JSR)
{
    mem[0xFFFC] = CPU::INS_JSR;
    mem.write_word(0x4242, 0xFFFD);
    mem[0x4242] = CPU::INS_LDA_IM;
    mem[0x4243] = 0x84;
    cpu.execute(8);
    EXPECT_EQ(cpu.A, 0x84);
}