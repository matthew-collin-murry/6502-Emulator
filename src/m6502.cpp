#include "m6502.h"

using namespace emulator6502;

//~~~~~~~~~~~~~~~~~Memory Functions~~~~~~~~~~~~~~~~~

void Memory::init()
{
    for (u32 i = 0; i < MAX_MEMORY; i++)
    {
        data[i] = 0;
    }
};

// read byte at address
byte Memory::operator[](u32 address) const
{
    assert(address < MAX_MEMORY);
    return data[address];
}

// write byte at address
byte& Memory::operator[](u32 address)
{
    assert(address < MAX_MEMORY);
    return data[address];
}

// write two bytes (i.e. word)
void Memory::write_word(word w, u32 address)
{
    data[address]       = w & 0xFF;
    data[address + 1]   = (w >> 8);
}

//~~~~~~~~~~~~~~~~~CPU Functions~~~~~~~~~~~~~~~~~

CPU::CPU(Memory& mem)
    : mem_ref(mem)
{}

void CPU::set_memory(Memory& mem)
{
    mem_ref = mem;
}

// http:://www.c64-wiki.com/wiki/Reset_(Process)
void CPU::reset()
{
    // reset addresses
    PC = 0xFFFC;
    SP = 0x00FF;

    // clear flags
    C = Z = I = D = B = V = N = 0;

    // clear registers
    A = X = Y = 0;

    // set up memory
    mem_ref.init();
};

byte CPU::fetch_byte(s32& cycles)
{
    byte value = mem_ref[PC];
    PC++;
    cycles--;
    return value;
}

byte CPU::read_byte(s32& cycles, word address)
{
    byte value = mem_ref[address];
    cycles--;
    return value;
}

// 6502 is little endian
word CPU::fetch_word(s32& cycles)
{
    // get lower 
    word value = mem_ref[PC];
    PC++;

    // get upper
    value |= (mem_ref[PC] << 8);
    PC++;

    cycles -= 2;

    //handle endianness
    if (std::endian::native == std::endian::big)
        { byteswap(value); }

    return value;
}

word CPU::read_word(s32& cycles, word address)
{
    byte low = read_byte(cycles, address);
    byte high = read_byte(cycles, address + 0x1);
    return low | (high << 8);
}

/** @return number of cycles used */
s32 CPU::execute(s32 cycles)
{
    auto load_register =
        [&cycles, this]
        (word address, byte& reg)
    {
        reg = read_byte(cycles, address);
        _LD__set_flags(reg);
    };

    const s32 start_cycles = cycles;
    while (cycles > 0)
    {
        byte instruction = fetch_byte(cycles);
        switch (instruction)
        {
        // LDA ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
        case INS_LDA_IM:
        {
            A = fetch_byte(cycles);
            _LD__set_flags(A);
        } break;
        case INS_LDA_ZP:
        {
            word address = addr_zero_page(cycles);
            load_register(address, A);
        } break;
        case INS_LDA_ZPX:
        {
            word address = addr_zero_page_x(cycles);
            load_register(address, A);
        } break;
        case INS_LDA_ABS:
        {
            word address = addr_abs(cycles);
            load_register(address, A);
        } break;
        case INS_LDA_AX:
        {
            word address = addr_abs_x_page(cycles);
            load_register(address, A);
        } break;
        case INS_LDA_AY:
        {
            word address = addr_abs_y_page(cycles);
            load_register(address, A);
        } break;
        case INS_LDA_IX:
        {
            word address = addr_in_x(cycles);
            load_register(address, A);
            cycles--;
        } break;
        case INS_LDA_IY:
        {
            word address = addr_in_y_page(cycles);
            load_register(address, A);
        } break;
        // LDX ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
        case INS_LDX_IM:
        {
            X = fetch_byte(cycles);
            _LD__set_flags(X);
        } break;
        case INS_LDX_ZP:
        {
            word address = addr_zero_page(cycles);
            load_register(address, X);
        } break;
        case INS_LDX_ZPY:
        {
            word address = addr_zero_page_y(cycles);
            load_register(address, X);
        } break;
        case INS_LDX_ABS:
        {
            word address = addr_abs(cycles);
            load_register(address, X);
        } break;
        case INS_LDX_AY:
        {
            word address = addr_abs_y_page(cycles);
            load_register(address, X);
        } break;
        // LDY ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
        case INS_LDY_IM:
        {
            Y = fetch_byte(cycles);
            _LD__set_flags(Y);
        } break;
        case INS_LDY_ZP:
        {
            word address = addr_zero_page(cycles);
            load_register(address, Y);
        } break;
        case INS_LDY_ZPX:
        {
            word address = addr_zero_page_x(cycles);
            load_register(address, Y);
        } break;
        case INS_LDY_ABS:
        {
            word address = addr_abs(cycles);
            load_register(address, Y);
        } break;
        case INS_LDY_AX:
        {
            word address = addr_abs_x_page(cycles);
            load_register(address, Y);
        } break;
        // STA ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
        case INS_STA_ZP:
        {
            word address = addr_zero_page(cycles);
            mem_ref[address] = A;
            cycles--;
        } break;
        case INS_STA_ZPX:
        {
            word address = addr_zero_page_x(cycles);
            mem_ref[address] = A;
            cycles--;
        } break;
        case INS_STA_ABS:
        {
            word address = addr_abs(cycles);
            mem_ref[address] = A;
            cycles--;
        } break;
        case INS_STA_AX:
        {
            word address = addr_abs_x(cycles);
            mem_ref[address] = A;
            cycles--;
        } break;
        case INS_STA_AY:
        {
            word address = addr_abs_y(cycles);
            mem_ref[address] = A;
            cycles--;
        } break;
        case INS_STA_IX:
        {
            word address = addr_in_x(cycles);
            mem_ref[address] = A;
            cycles -= 2;
        } break;
        case INS_STA_IY:
        {
            word address = addr_in_y(cycles);
            mem_ref[address] = A;
            cycles -= 2;
        } break;
        // STX ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
        case INS_STX_ZP:
        {
            word address = addr_zero_page(cycles);
            mem_ref[address] = X;
            cycles--;
        } break;
        case INS_STX_ZPY:
        {
            word address = addr_zero_page_y(cycles);
            mem_ref[address] = X;
            cycles--;
        } break;
        case INS_STX_ABS:
        {
            word address = addr_abs(cycles);
            mem_ref[address] = X;
            cycles--;
        } break;
        // JSY ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
        case INS_STY_ZP:
        {
            word address = addr_zero_page(cycles);
            mem_ref[address] = Y;
            cycles--;
        } break;
        case INS_STY_ZPX:
        {
            word address = addr_zero_page_x(cycles);
            mem_ref[address] = Y;
            cycles--;
        } break;
        case INS_STY_ABS:
        {
            word address = addr_abs(cycles);
            mem_ref[address] = Y;
            cycles--;
        } break;
        // JSR ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
        case INS_JSR:
        {
            word sub_routine_addr = fetch_word(cycles);
            write_word_cpu(cycles, PC - 1, SP);
            SP += 2;
            PC = sub_routine_addr;
            cycles--;
        } break;
        default: 
            throw UnknownInstructionException(
                std::format("Unknown instruction: {}", instruction).c_str()
            );
            break;
        }
    }

    return start_cycles - cycles;
}

word CPU::addr_zero_page(s32& cycles)
{
    return fetch_byte(cycles);
}

word CPU::addr_zero_page_x(s32& cycles)
{
    byte zero_page_addr = fetch_byte(cycles);
    zero_page_addr += X;
    cycles--;
    return zero_page_addr;
}

word CPU::addr_zero_page_y(s32& cycles)
{
    byte zero_page_addr = fetch_byte(cycles);
    zero_page_addr += Y;
    cycles--;
    return zero_page_addr;
}

word CPU::addr_abs(s32& cycles)
{
    return fetch_word(cycles);
}

word CPU::addr_abs_x(s32& cycles)
{
    word mem_addr = fetch_word(cycles);
    mem_addr += X;

    return mem_addr;
}

word CPU::addr_abs_y(s32& cycles)
{
    word mem_addr = fetch_word(cycles);
    mem_addr += Y;
    
    return mem_addr;
}

word CPU::addr_in_x(s32& cycles)
{
    byte zp_addr = fetch_byte(cycles);
    zp_addr += X;
    word mem_addr = read_word(cycles, zp_addr);

    return mem_addr;
}

word CPU::addr_in_y(s32& cycles)
{
    byte zp_addr = fetch_byte(cycles);
    zp_addr += Y;
    word mem_addr = read_word(cycles, zp_addr);

    return mem_addr;
}

word CPU::addr_abs_x_page(s32& cycles)
{
    word mem_addr = fetch_word(cycles);
    word mem_addr_x = mem_addr + X;
    const bool cross_page_boundary = (mem_addr ^ mem_addr_x) >> 8;
    if (cross_page_boundary) cycles--;
    return mem_addr_x;
}

word CPU::addr_abs_y_page(s32& cycles)
{
    word mem_addr = fetch_word(cycles);
    word mem_addr_y = mem_addr + Y;
    const bool cross_page_boundary = (mem_addr ^ mem_addr_y) >> 8;
    if (cross_page_boundary) cycles--;
    return mem_addr_y;
}

word CPU::addr_in_x_page(s32& cycles)
{
    byte zp_addr = fetch_byte(cycles);
    // extra cycle for page boundary cross
    if ((word)X + (word)zp_addr >= 0xFF) cycles--;

    zp_addr += X;
    word mem_addr = read_word(cycles, zp_addr);

    return mem_addr;
}

word CPU::addr_in_y_page(s32& cycles)
{
    byte zp_addr = fetch_byte(cycles);
    // extra cycle for page boundary cross
    if ((word)Y + (word)zp_addr >= 0xFF) cycles--;

    zp_addr += Y;
    word mem_addr = read_word(cycles, zp_addr);

    return mem_addr;
}
