#ifndef _H_LOGICAL_TESTS
#define _H_LOGICAL_TESTS

#include "gtest/gtest.h"
#include "m6502.h"

using namespace emulator6502;

class LogicalTests : public testing::Test
{
public:
    Memory mem;
    CPU cpu;
    CPU default_cpu_state;

    LogicalTests()
        : cpu(CPU(mem)), default_cpu_state(CPU(mem))
    {}

    virtual void SetUp()
    {
        cpu.reset();
    }

    void check_logical_unmodified_flags();
    template<typename F>
    void test_logical_im(byte, const F&);
    template<typename F>
    void test_logical_zp(byte, const F&);
    template<typename F>
    void test_logical_zpx(byte, const F&);
    template<typename F>
    void test_logical_abs(byte, const F&);
    template<typename F>
    void test_logical_ax(byte, const F&);
    template<typename F>
    void test_logical_ay(byte, const F&);
    template<typename F>
    void test_logical_ix(byte, const F&);
    template<typename F>
    void test_logical_iy(byte, const F&);
    void test_logical_flags(byte);
};

template<typename F>
void LogicalTests::test_logical_im(byte opcode, const F& f)
{
    byte memory_value = 0b1011;
    byte a_value = 0b0101;
    cpu.A = a_value;
    mem[0xFFFC] = opcode;
    mem[0xFFFD] = memory_value;
    auto cycles_used = cpu.execute(2);
    EXPECT_EQ(cycles_used, 2);
    EXPECT_EQ(f(a_value, memory_value), cpu.A);
    check_logical_unmodified_flags();
}

template<typename F>
void LogicalTests::test_logical_zp(byte opcode, const F& f)
{
    byte memory_value = 0b1011;
    byte a_value = 0b0101;
    cpu.A = a_value;
    mem[0xFFFC] = opcode;
    mem[0xFFFD] = 0x0004;
    mem[0x0004] = memory_value;
    auto cycles_used = cpu.execute(3);
    EXPECT_EQ(cycles_used, 3);
    EXPECT_EQ(f(a_value, memory_value), cpu.A);
    check_logical_unmodified_flags();
}

template<typename F>
void LogicalTests::test_logical_zpx(byte opcode, const F& f)
{
    byte memory_value = 0b1011;
    byte a_value = 0b0101;
    cpu.A = a_value;
    mem[0xFFFC] = opcode;
    mem[0xFFFD] = 0x0004;
    cpu.X = 0x1;
    mem[0x0005] = memory_value;
    auto cycles_used = cpu.execute(4);
    EXPECT_EQ(cycles_used, 4);
    EXPECT_EQ(f(a_value, memory_value), cpu.A);
    check_logical_unmodified_flags();

    cpu.reset();

    // wrap around check
    cpu.A = a_value;
    mem[0xFFFC] = opcode;
    mem[0xFFFD] = 0x0004;
    cpu.X = 0xFF;
    mem[0x0003] = memory_value;
    cycles_used = cpu.execute(4);
    EXPECT_EQ(cycles_used, 4);
    EXPECT_EQ(f(a_value, memory_value), cpu.A);
    check_logical_unmodified_flags();
}

template<typename F>
void LogicalTests::test_logical_abs(byte opcode, const F& f)
{
    byte memory_value = 0b1011;
    byte a_value = 0b0101;
    cpu.A = a_value;
    mem[0xFFFC] = opcode;
    mem.write_word(0xFFFF, 0xFFFD);
    mem[0xFFFF] = memory_value;
    auto cycles_used = cpu.execute(4);
    EXPECT_EQ(cycles_used, 4);
    EXPECT_EQ(f(a_value, memory_value), cpu.A);
    check_logical_unmodified_flags();
}

template<typename F>
void LogicalTests::test_logical_ax(byte opcode, const F& f)
{
    byte memory_value = 0b1011;
    byte a_value = 0b0101;

    cpu.A = a_value;
    mem[0xFFFC] = opcode;
    mem.write_word(0x4480, 0xFFFD);
    mem[0x4481] = memory_value;
    cpu.X = 1;
    auto cycles_used = cpu.execute(4);
    EXPECT_EQ(f(a_value, memory_value), cpu.A);
    EXPECT_EQ(cycles_used, 4);

    cpu.reset();

    // page cross check
    cpu.A = a_value;
    mem[0xFFFC] = opcode;
    mem.write_word(0x4402, 0xFFFD);
    mem[0x4501] = memory_value; // 0x4402 + 0xFF
    // force page cross
    cpu.X = 0xFF;
    cycles_used = cpu.execute(5);
    EXPECT_EQ(f(a_value, memory_value), cpu.A);
    EXPECT_EQ(cycles_used, 5);
}

template<typename F>
void LogicalTests::test_logical_ay(byte opcode, const F& f)
{
    byte memory_value = 0b1011;
    byte a_value = 0b0101;

    cpu.A = a_value;
    mem[0xFFFC] = opcode;
    mem.write_word(0x4480, 0xFFFD);
    mem[0x4481] = memory_value;
    cpu.Y = 1;
    auto cycles_used = cpu.execute(4);
    EXPECT_EQ(f(a_value, memory_value), cpu.A);
    EXPECT_EQ(cycles_used, 4);

    cpu.reset();

    // page cross check
    cpu.A = a_value;
    mem[0xFFFC] = opcode;
    mem.write_word(0x4402, 0xFFFD);
    mem[0x4501] = memory_value; // 0x4402 + 0xFF
    // force page cross
    cpu.Y = 0xFF;
    cycles_used = cpu.execute(5);
    EXPECT_EQ(f(a_value, memory_value), cpu.A);
    EXPECT_EQ(cycles_used, 5);
}

template<typename F>
void LogicalTests::test_logical_ix(byte opcode, const F& f)
{
    byte memory_value = 0b1011;
    byte a_value = 0b0101;
    cpu.A = a_value;
    mem[0xFFFC] = opcode;
    mem[0xFFFD] = 0x02;
    mem.write_word(0x8000, 0x0006);
    mem[0x8000] = memory_value;
    cpu.X = 0x4;
    auto cycles_used = cpu.execute(6);
    EXPECT_EQ(f(a_value, memory_value), cpu.A);
    EXPECT_EQ(cycles_used, 6);
}

template<typename F>
void LogicalTests::test_logical_iy(byte opcode, const F& f)
{
    byte memory_value = 0b1011;
    byte a_value = 0b0101;
    cpu.A = a_value;
    mem[0xFFFC] = opcode;
    mem[0xFFFD] = 0x02;
    mem.write_word(0x8000, 0x0006);
    mem[0x8000] = memory_value;
    cpu.Y = 0x4;
    auto cycles_used = cpu.execute(5);
    EXPECT_EQ(f(a_value, memory_value), cpu.A);
    EXPECT_EQ(cycles_used, 5);

    cpu.reset();

    // page cross check
    cpu.A = a_value;
    mem[0xFFFC] = opcode;
    mem[0xFFFD] = 0x02;
    mem.write_word(0x8000, 0x0001);
    mem[0x8000] = memory_value;
    cpu.Y = 0xFF;
    cycles_used = cpu.execute(6);
    EXPECT_EQ(f(a_value, memory_value), cpu.A);
    EXPECT_EQ(cycles_used, 6);
}

#endif