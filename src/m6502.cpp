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
byte Memory::operator[](word address) const
{
    assert(address < MAX_MEMORY);
    return data[address];
}

// write byte at address
byte& Memory::operator[](word address)
{
    assert(address < MAX_MEMORY);
    return data[address];
}

// write two bytes (i.e. word)
void Memory::write_word(word w, word address)
{
    data[address]       = w & 0xFF;
    data[address + 1]   = (w >> 8);
}

//~~~~~~~~~~~~~~~~~CPU Functions~~~~~~~~~~~~~~~~~

CPU::CPU(Memory& mem)
    : mem_ref(mem)
{
    reset();
}

void CPU::set_memory(Memory& mem)
{
    mem_ref = mem;
}

// http:://www.c64-wiki.com/wiki/Reset_(Process)
void CPU::reset(word reset_vector)
{
    // reset addresses
    PC = reset_vector;
    SP = 0xFF;

    // clear flags
    SF.C = SF.Z = SF.I = SF.D = SF.B = SF.V = SF.N = 0;

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

byte CPU::read_byte(s32& cycles, word address) const
{
    byte value = mem_ref[address];
    cycles--;
    return value;
}

word CPU::fetch_word(s32& cycles)
{
    // get lower 
    word value = mem_ref[PC];
    PC++;

    // get upper
    value |= (mem_ref[PC] << 8);
    PC++;

    cycles -= 2;

    return value;
}

word CPU::read_word(s32& cycles, word address) const
{
    byte low = read_byte(cycles, address);
    byte high = read_byte(cycles, address + 0x1);
    return low | (high << 8);
}

void CPU::write_byte(s32& cycles, byte data, word address)
{
    mem_ref[address] = data;
    cycles--;
}

void CPU::write_word(s32& cycles, word data, word address)
{
    cycles -= 2;
    mem_ref.write_word(data, address);
}

/** @return stack pointer as 16 bit address */
word CPU::sp_to_address() const
{
    return 0x100 | SP;
}

void CPU::push_pc_sp(s32& cycles)
{
    write_word(cycles, PC-1, sp_to_address() - 1);
    SP -= 2;
    cycles--;
}

word CPU::pop_word_from_stack(s32& cycles)
{
    word value = read_word(cycles, sp_to_address() + 1);
    SP += 2;
    cycles--;
    return value;
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
            A = address_mode_zero_page_and_immediate(cycles);
            _LD__set_flags(A);
        } break;
        case INS_LDA_ZP:
        {
            word address = address_mode_zero_page_and_immediate(cycles);
            load_register(address, A);
        } break;
        case INS_LDA_ZPX:
        {
            word address = address_mode_zero_page_x_offset(cycles);
            load_register(address, A);
        } break;
        case INS_LDA_ABS:
        {
            word address = address_mode_absolute(cycles);
            load_register(address, A);
        } break;
        case INS_LDA_AX:
        {
            word address = address_mode_abosolute_x_offset_with_page_cycle(cycles);
            load_register(address, A);
        } break;
        case INS_LDA_AY:
        {
            word address = address_mode_abosolute_y_offset_with_page_cycle(cycles);
            load_register(address, A);
        } break;
        case INS_LDA_IX:
        {
            word address = address_mode_indirest_x_offset(cycles);
            load_register(address, A);
            cycles--;
        } break;
        case INS_LDA_IY:
        {
            word address = address_mode_indirect_y_offset_with_page_cycle(cycles);
            load_register(address, A);
        } break;
        // LDX ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
        case INS_LDX_IM:
        {
            X = address_mode_zero_page_and_immediate(cycles);
            _LD__set_flags(X);
        } break;
        case INS_LDX_ZP:
        {
            word address = address_mode_zero_page_and_immediate(cycles);
            load_register(address, X);
        } break;
        case INS_LDX_ZPY:
        {
            word address = address_mode_zero_page_y_offset(cycles);
            load_register(address, X);
        } break;
        case INS_LDX_ABS:
        {
            word address = address_mode_absolute(cycles);
            load_register(address, X);
        } break;
        case INS_LDX_AY:
        {
            word address = address_mode_abosolute_y_offset_with_page_cycle(cycles);
            load_register(address, X);
        } break;
        // LDY ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
        case INS_LDY_IM:
        {
            Y = address_mode_zero_page_and_immediate(cycles);
            _LD__set_flags(Y);
        } break;
        case INS_LDY_ZP:
        {
            word address = address_mode_zero_page_and_immediate(cycles);
            load_register(address, Y);
        } break;
        case INS_LDY_ZPX:
        {
            word address = address_mode_zero_page_x_offset(cycles);
            load_register(address, Y);
        } break;
        case INS_LDY_ABS:
        {
            word address = address_mode_absolute(cycles);
            load_register(address, Y);
        } break;
        case INS_LDY_AX:
        {
            word address = address_mode_abosolute_x_offset_with_page_cycle(cycles);
            load_register(address, Y);
        } break;
        // STA ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
        case INS_STA_ZP:
        {
            word address = address_mode_zero_page_and_immediate(cycles);
            write_byte(cycles, A, address);
        } break;
        case INS_STA_ZPX:
        {
            word address = address_mode_zero_page_x_offset(cycles);
            write_byte(cycles, A, address);
        } break;
        case INS_STA_ABS:
        {
            word address = address_mode_absolute(cycles);
            write_byte(cycles, A, address);
        } break;
        case INS_STA_AX:
        {
            word address = address_mode_absolute_x_offset(cycles);
            write_byte(cycles, A, address);
            cycles--;
        } break;
        case INS_STA_AY:
        {
            word address = address_mode_absolute_y_offset(cycles);
            write_byte(cycles, A, address);
            cycles--;
        } break;
        case INS_STA_IX:
        {
            word address = address_mode_indirest_x_offset(cycles);
            write_byte(cycles, A, address);
            cycles--;
        } break;
        case INS_STA_IY:
        {
            word address = address_mode_indirest_y_offset(cycles);
            write_byte(cycles, A, address);
            cycles--;
        } break;
        // STX ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
        case INS_STX_ZP:
        {
            word address = address_mode_zero_page_and_immediate(cycles);
            write_byte(cycles, X, address);
        } break;
        case INS_STX_ZPY:
        {
            word address = address_mode_zero_page_y_offset(cycles);
            write_byte(cycles, X, address);
        } break;
        case INS_STX_ABS:
        {
            word address = address_mode_absolute(cycles);
            write_byte(cycles, X, address);
        } break;
        // JSY ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
        case INS_STY_ZP:
        {
            word address = address_mode_zero_page_and_immediate(cycles);
            write_byte(cycles, Y, address);
        } break;
        case INS_STY_ZPX:
        {
            word address = address_mode_zero_page_x_offset(cycles);
            write_byte(cycles, Y, address);
        } break;
        case INS_STY_ABS:
        {
            word address = address_mode_absolute(cycles);
            write_byte(cycles, Y, address);
        } break;
        // Jumps and Returns ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
        case INS_JSR:
        {
            word sub_routine_addr = fetch_word(cycles);
            push_pc_sp(cycles);
            PC = sub_routine_addr;
        } break;
        case INS_RTS:
        {
            word return_addr = pop_word_from_stack(cycles);
            PC = return_addr + 1;
            cycles -= 2;
        } break;
        case INS_JMP_ABS:
        {
            PC = address_mode_absolute(cycles);
        } break;
        case INS_JMP_I:
        {
            word address = fetch_word(cycles);
            PC = read_word(cycles, address);
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

word CPU::address_mode_zero_page_and_immediate(s32& cycles)
{
    return fetch_byte(cycles);
}

word CPU::address_mode_zero_page_x_offset(s32& cycles)
{
    byte zero_page_addr = fetch_byte(cycles);
    zero_page_addr += X;
    cycles--;
    return zero_page_addr;
}

word CPU::address_mode_zero_page_y_offset(s32& cycles)
{
    byte zero_page_addr = fetch_byte(cycles);
    zero_page_addr += Y;
    cycles--;
    return zero_page_addr;
}

word CPU::address_mode_absolute(s32& cycles)
{
    return fetch_word(cycles);
}

word CPU::address_mode_absolute_x_offset(s32& cycles)
{
    word mem_addr = fetch_word(cycles);
    mem_addr += X;

    return mem_addr;
}

word CPU::address_mode_absolute_y_offset(s32& cycles)
{
    word mem_addr = fetch_word(cycles);
    mem_addr += Y;
    
    return mem_addr;
}

word CPU::address_mode_indirest_x_offset(s32& cycles)
{
    byte zp_addr = fetch_byte(cycles);
    zp_addr += X;
    word mem_addr = read_word(cycles, zp_addr);

    return mem_addr;
}

word CPU::address_mode_indirest_y_offset(s32& cycles)
{
    byte zp_addr = fetch_byte(cycles);
    zp_addr += Y;
    word mem_addr = read_word(cycles, zp_addr);

    return mem_addr;
}

word CPU::address_mode_abosolute_x_offset_with_page_cycle(s32& cycles)
{
    word mem_addr = fetch_word(cycles);
    word mem_addr_x = mem_addr + X;
    const bool cross_page_boundary = (mem_addr ^ mem_addr_x) >> 8;
    if (cross_page_boundary) cycles--;
    return mem_addr_x;
}

word CPU::address_mode_abosolute_y_offset_with_page_cycle(s32& cycles)
{
    word mem_addr = fetch_word(cycles);
    word mem_addr_y = mem_addr + Y;
    const bool cross_page_boundary = (mem_addr ^ mem_addr_y) >> 8;
    if (cross_page_boundary) cycles--;
    return mem_addr_y;
}

word CPU::address_mode_indirect_x_offset_with_page_cycle(s32& cycles)
{
    byte zp_addr = fetch_byte(cycles);
    // extra cycle for page boundary cross
    if ((word)X + (word)zp_addr >= 0xFF) cycles--;

    zp_addr += X;
    word mem_addr = read_word(cycles, zp_addr);

    return mem_addr;
}

word CPU::address_mode_indirect_y_offset_with_page_cycle(s32& cycles)
{
    byte zp_addr = fetch_byte(cycles);
    // extra cycle for page boundary cross
    if ((word)Y + (word)zp_addr >= 0xFF) cycles--;

    zp_addr += Y;
    word mem_addr = read_word(cycles, zp_addr);

    return mem_addr;
}
