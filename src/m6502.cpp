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
    flag.C = flag.Z = flag.I = flag.D = flag.B = flag.V = flag.N = 0;
    PS = 0x00;

    // clear registers
    A = X = Y = 0;

    // set up memory
    mem_ref.init();
};

byte CPU::fetch_byte()
{
    byte value = mem_ref[PC];
    PC++;
    cycles--;
    return value;
}

byte CPU::read_byte(word address)
{
    byte value = mem_ref[address];
    cycles--;
    return value;
}

word CPU::fetch_word()
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

word CPU::read_word(word address)
{
    byte low = read_byte(address);
    byte high = read_byte(address + 0x1);
    return low | (high << 8);
}

void CPU::write_byte(byte data, word address)
{
    mem_ref[address] = data;
    cycles--;
}

void CPU::write_word(word data, word address)
{
    cycles -= 2;
    mem_ref.write_word(data, address);
}

/** @return stack pointer as 16 bit address */
word CPU::sp_to_address() const
{
    return 0x100 | SP;
}

void CPU::push_pc_sp()
{
    write_word(PC-1, sp_to_address() - 1);
    SP -= 2;
    cycles--;
}

word CPU::pop_word_from_stack()
{
    word value = read_word(sp_to_address() + 1);
    SP += 2;
    cycles--;
    return value;
}

byte CPU::pop_byte_from_stack()
{
    byte value = read_byte(sp_to_address());
    SP++;
    cycles--;
    return value;
}

void CPU::push_byte_to_stack(byte value)
{
    mem_ref[sp_to_address()] = value;
    cycles--;
    SP--;
}

/** @return number of cycles used */
s32 CPU::execute(s32 cycle_count)
{
    this->cycles = cycle_count;

    auto load_register =
        [this]
        (word address, byte& reg)
    {
        reg = read_byte(address);
        zero_and_negative_flag_set(reg);
    };

    // weird name because and is a keyword
    auto _and_ =
        [this]
        (word address)
    {
        A &= read_byte(address);
        zero_and_negative_flag_set(A);
    };

    auto eor =
        [this]
        (word address)
    {
        A ^= read_byte(address);
        zero_and_negative_flag_set(A);
    };

    // weird name because or is a keyword
    auto _or_ =
        [this]
        (word address)
    {
        A |= read_byte(address);
        zero_and_negative_flag_set(A);
    };

    const s32 start_cycles = cycles;
    while (cycles > 0)
    {
        byte instruction = fetch_byte();
        switch (instruction)
        {
        // LDA ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
        case INS_LDA_IM:
        {
            A = address_mode_zero_page_and_immediate();
            zero_and_negative_flag_set(A);
        } break;
        case INS_LDA_ZP:
        {
            word address = address_mode_zero_page_and_immediate();
            load_register(address, A);
        } break;
        case INS_LDA_ZPX:
        {
            word address = address_mode_zero_page_x_offset();
            load_register(address, A);
        } break;
        case INS_LDA_ABS:
        {
            word address = address_mode_absolute();
            load_register(address, A);
        } break;
        case INS_LDA_AX:
        {
            word address = address_mode_abosolute_x_offset_with_page_cycle();
            load_register(address, A);
        } break;
        case INS_LDA_AY:
        {
            word address = address_mode_abosolute_y_offset_with_page_cycle();
            load_register(address, A);
        } break;
        case INS_LDA_IX:
        {
            word address = address_mode_indirect_x_offset();
            load_register(address, A);
            cycles--;
        } break;
        case INS_LDA_IY:
        {
            word address = address_mode_indirect_y_offset_with_page_cycle();
            load_register(address, A);
        } break;
        // LDX ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
        case INS_LDX_IM:
        {
            X = address_mode_zero_page_and_immediate();
            zero_and_negative_flag_set(X);
        } break;
        case INS_LDX_ZP:
        {
            word address = address_mode_zero_page_and_immediate();
            load_register(address, X);
        } break;
        case INS_LDX_ZPY:
        {
            word address = address_mode_zero_page_y_offset();
            load_register(address, X);
        } break;
        case INS_LDX_ABS:
        {
            word address = address_mode_absolute();
            load_register(address, X);
        } break;
        case INS_LDX_AY:
        {
            word address = address_mode_abosolute_y_offset_with_page_cycle();
            load_register(address, X);
        } break;
        // LDY ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
        case INS_LDY_IM:
        {
            Y = address_mode_zero_page_and_immediate();
            zero_and_negative_flag_set(Y);
        } break;
        case INS_LDY_ZP:
        {
            word address = address_mode_zero_page_and_immediate();
            load_register(address, Y);
        } break;
        case INS_LDY_ZPX:
        {
            word address = address_mode_zero_page_x_offset();
            load_register(address, Y);
        } break;
        case INS_LDY_ABS:
        {
            word address = address_mode_absolute();
            load_register(address, Y);
        } break;
        case INS_LDY_AX:
        {
            word address = address_mode_abosolute_x_offset_with_page_cycle();
            load_register(address, Y);
        } break;
        // STA ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
        case INS_STA_ZP:
        {
            word address = address_mode_zero_page_and_immediate();
            write_byte(A, address);
        } break;
        case INS_STA_ZPX:
        {
            word address = address_mode_zero_page_x_offset();
            write_byte(A, address);
        } break;
        case INS_STA_ABS:
        {
            word address = address_mode_absolute();
            write_byte(A, address);
        } break;
        case INS_STA_AX:
        {
            word address = address_mode_absolute_x_offset();
            write_byte(A, address);
            cycles--;
        } break;
        case INS_STA_AY:
        {
            word address = address_mode_absolute_y_offset();
            write_byte(A, address);
            cycles--;
        } break;
        case INS_STA_IX:
        {
            word address = address_mode_indirect_x_offset();
            write_byte(A, address);
            cycles--;
        } break;
        case INS_STA_IY:
        {
            word address = address_mode_indirect_y_offset();
            write_byte(A, address);
            cycles--;
        } break;
        // STX ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
        case INS_STX_ZP:
        {
            word address = address_mode_zero_page_and_immediate();
            write_byte(X, address);
        } break;
        case INS_STX_ZPY:
        {
            word address = address_mode_zero_page_y_offset();
            write_byte(X, address);
        } break;
        case INS_STX_ABS:
        {
            word address = address_mode_absolute();
            write_byte(X, address);
        } break;
        // JSY ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
        case INS_STY_ZP:
        {
            word address = address_mode_zero_page_and_immediate();
            write_byte(Y, address);
        } break;
        case INS_STY_ZPX:
        {
            word address = address_mode_zero_page_x_offset();
            write_byte(Y, address);
        } break;
        case INS_STY_ABS:
        {
            word address = address_mode_absolute();
            write_byte(Y, address);
        } break;
        // Jumps and Returns ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
        case INS_JSR:
        {
            word sub_routine_addr = fetch_word();
            push_pc_sp();
            PC = sub_routine_addr;
        } break;
        case INS_RTS:
        {
            word return_addr = pop_word_from_stack();
            PC = return_addr + 1;
            cycles -= 2;
        } break;
        case INS_JMP_ABS:
        {
            PC = address_mode_absolute();
        } break;
        case INS_JMP_I:
        {
            word address = fetch_word();
            PC = read_word(address);
        } break;
        // Stack Operations ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
        case INS_TSX:
        {
            X = SP;
            cycles--;
            zero_and_negative_flag_set(X);
        } break;
        case INS_TXS:
        {
            SP = X;
            cycles--;
        } break;
        case INS_PHA:
        {
            push_byte_to_stack(A);
            cycles--;
        } break;
        case INS_PHP:
        {
            push_byte_to_stack(PS);
            cycles--;
        } break;
        case INS_PLA:
        {
            A = pop_byte_from_stack();
            zero_and_negative_flag_set(A);
            cycles--;
        } break;
        case INS_PLP:
        {  
            PS = pop_byte_from_stack();
            cycles--;
        } break;
        // Logical Operations ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
        case INS_AND_IM:
        {
            A &= address_mode_zero_page_and_immediate();
            zero_and_negative_flag_set(A);
        } break;
        case INS_AND_ZP:
        {
            word address = address_mode_zero_page_and_immediate();
            _and_(address);
        } break;
        case INS_AND_ZPX:
        {   
            word address = address_mode_zero_page_x_offset();
            _and_(address);
        } break;
        case INS_AND_ABS:
        {
            word address = address_mode_absolute();
            _and_(address);
        } break;
        case INS_AND_AX:
        {
            word address = address_mode_abosolute_x_offset_with_page_cycle();
            _and_(address);
        } break;
        case INS_AND_AY:
        {
            word address = address_mode_abosolute_y_offset_with_page_cycle();
            _and_(address);
        } break;
        case INS_AND_IX:
        {
            word address = address_mode_indirect_x_offset();
            _and_(address);
            cycles--;
        } break;
        case INS_AND_IY:
        {
            word address = address_mode_indirect_y_offset_with_page_cycle();
            _and_(address);
        } break;
    // EOR ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
        case INS_EOR_IM:
        {
            A ^= address_mode_zero_page_and_immediate();
            zero_and_negative_flag_set(A);
        } break;
        case INS_EOR_ZP:
        {
            word address = address_mode_zero_page_and_immediate();
            eor(address);
        } break;
        case INS_EOR_ZPX:
        {   
            word address = address_mode_zero_page_x_offset();
            eor(address);
        } break;
        case INS_EOR_ABS:
        {
            word address = address_mode_absolute();
            eor(address);
        } break;
        case INS_EOR_AX:
        {
            word address = address_mode_abosolute_x_offset_with_page_cycle();
            eor(address);
        } break;
        case INS_EOR_AY:
        {
            word address = address_mode_abosolute_y_offset_with_page_cycle();
            eor(address);
        } break;
        case INS_EOR_IX:
        {
            word address = address_mode_indirect_x_offset();
            eor(address);
            cycles--;
        } break;
        case INS_EOR_IY:
        {
            word address = address_mode_indirect_y_offset_with_page_cycle();
            eor(address);
        } break;
    // OR ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
        case INS_ORA_IM:
        {
            A |= address_mode_zero_page_and_immediate();
            zero_and_negative_flag_set(A);
        } break;
        case INS_ORA_ZP:
        {
            word address = address_mode_zero_page_and_immediate();
            _or_(address);
        } break;
        case INS_ORA_ZPX:
        {   
            word address = address_mode_zero_page_x_offset();
            _or_(address);
        } break;
        case INS_ORA_ABS:
        {
            word address = address_mode_absolute();
            _or_(address);
        } break;
        case INS_ORA_AX:
        {
            word address = address_mode_abosolute_x_offset_with_page_cycle();
            _or_(address);
        } break;
        case INS_ORA_AY:
        {
            word address = address_mode_abosolute_y_offset_with_page_cycle();
            _or_(address);
        } break;
        case INS_ORA_IX:
        {
            word address = address_mode_indirect_x_offset();
            _or_(address);
            cycles--;
        } break;
        case INS_ORA_IY:
        {
            word address = address_mode_indirect_y_offset_with_page_cycle();
            _or_(address);
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

word CPU::address_mode_zero_page_and_immediate()
{
    return fetch_byte();
}

word CPU::address_mode_zero_page_x_offset()
{
    byte zero_page_addr = fetch_byte();
    zero_page_addr += X;
    cycles--;
    return zero_page_addr;
}

word CPU::address_mode_zero_page_y_offset()
{
    byte zero_page_addr = fetch_byte();
    zero_page_addr += Y;
    cycles--;
    return zero_page_addr;
}

word CPU::address_mode_absolute()
{
    return fetch_word();
}

word CPU::address_mode_absolute_x_offset()
{
    word mem_addr = fetch_word();
    mem_addr += X;

    return mem_addr;
}

word CPU::address_mode_absolute_y_offset()
{
    word mem_addr = fetch_word();
    mem_addr += Y;
    
    return mem_addr;
}

word CPU::address_mode_indirect_x_offset()
{
    byte zp_addr = fetch_byte();
    zp_addr += X;
    word mem_addr = read_word(zp_addr);

    return mem_addr;
}

word CPU::address_mode_indirect_y_offset()
{
    byte zp_addr = fetch_byte();
    zp_addr += Y;
    word mem_addr = read_word(zp_addr);

    return mem_addr;
}

word CPU::address_mode_abosolute_x_offset_with_page_cycle()
{
    word mem_addr = fetch_word();
    word mem_addr_x = mem_addr + X;
    const bool cross_page_boundary = (mem_addr ^ mem_addr_x) >> 8;
    if (cross_page_boundary) cycles--;
    return mem_addr_x;
}

word CPU::address_mode_abosolute_y_offset_with_page_cycle()
{
    word mem_addr = fetch_word();
    word mem_addr_y = mem_addr + Y;
    const bool cross_page_boundary = (mem_addr ^ mem_addr_y) >> 8;
    if (cross_page_boundary) cycles--;
    return mem_addr_y;
}

word CPU::address_mode_indirect_x_offset_with_page_cycle()
{
    byte zp_addr = fetch_byte();
    // extra cycle for page boundary cross
    if ((word)X + (word)zp_addr >= 0xFF) cycles--;

    zp_addr += X;
    word mem_addr = read_word(zp_addr);

    return mem_addr;
}

word CPU::address_mode_indirect_y_offset_with_page_cycle()
{
    byte zp_addr = fetch_byte();
    // extra cycle for page boundary cross
    if ((word)Y + (word)zp_addr >= 0xFF) cycles--;

    zp_addr += Y;
    word mem_addr = read_word(zp_addr);

    return mem_addr;
}
