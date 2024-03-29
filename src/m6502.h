#ifndef _H_MAIN
#define _H_MAIN

#include "project_header.h"

namespace emulator6502 {
    #define CHECK_BIT(var, pos) ((var >> (pos)) & 1)

    // specific sized types
    using byte = unsigned char; // 8 bits
    using word = unsigned short; // 16 bits

    using u32 = unsigned int;
    using s32 = signed int;

    // special exception
    class UnknownInstructionException : public std::exception
    {
    public:
        explicit UnknownInstructionException(const char* msg) : _msg(msg)
        {}

        char* what()
        {
            return const_cast<char*>(_msg);
        }
    private:
        const char* _msg;
    };

    struct Memory
    {
        static constexpr u32 MAX_MEMORY = 1024 * 64;
        byte data[MAX_MEMORY];

        void init();
        byte operator[](word) const;
        byte& operator[](word);
        void write_word(word, word);
    };

    struct StatusFlags
    {
        byte C : 1;
        byte Z : 1;
        byte I : 1;
        byte D : 1;
        byte B : 1;
        byte _ : 1; // unused
        byte V : 1;
        byte N : 1;
    };

    struct CPU 
    {
        word PC; // program counter
        byte SP; // stack pointer

        byte A, X, Y; // registers

        union // processor status
        {
            byte PS;
            StatusFlags flag;
        };

        explicit CPU(Memory&);
        void set_memory(Memory&);
        void reset(word = 0xFFFC);
        word sp_to_address() const;
        s32 execute(s32);

        /**
         * IM  : Imediate
         * ZP  : Zero Page
         * ZPX : Zero Page x-indexed
         * ZPY : Zero Page y-indexed
         * ABS : Absolute
         * AX  : Absolute x-indexed
         * AY  : Absolute y-indexed
         * IX  : Indirect x-indexed
         * IY  : Indirect y-indexed
         * I   : Indirect
        */
        // opcodes
        static constexpr byte 
        // ~~~~~~~~~~~~~~~~ Load/Store ~~~~~~~~~~~~~~~~
            // LDA
            INS_LDA_IM       = 0xA9,
            INS_LDA_ZP       = 0xA5,
            INS_LDA_ZPX      = 0xB5,
            INS_LDA_ABS      = 0xAD,
            INS_LDA_AX       = 0xBD,
            INS_LDA_AY       = 0xB9,
            INS_LDA_IX       = 0xA1,
            INS_LDA_IY       = 0xB1,
            // LDX
            INS_LDX_IM       = 0xA2,
            INS_LDX_ZP       = 0xA6,
            INS_LDX_ZPY      = 0xB6,
            INS_LDX_ABS      = 0xAE,
            INS_LDX_AY       = 0xBE,
            // LDY
            INS_LDY_IM       = 0xA0,
            INS_LDY_ZP       = 0xA4,
            INS_LDY_ZPX      = 0xB4,
            INS_LDY_ABS      = 0xAC,
            INS_LDY_AX       = 0xBC,
            // STA
            INS_STA_ZP       = 0x85,
            INS_STA_ZPX      = 0x95,
            INS_STA_ABS      = 0x8D,
            INS_STA_AX       = 0x9D,
            INS_STA_AY       = 0x99,
            INS_STA_IX       = 0x81,
            INS_STA_IY       = 0x91,
            // STX
            INS_STX_ZP       = 0x86,
            INS_STX_ZPY      = 0x96,
            INS_STX_ABS      = 0x8E,
            // STY
            INS_STY_ZP       = 0x84,
            INS_STY_ZPX      = 0x94,
            INS_STY_ABS      = 0x8C,
        // ~~~~~~~~~~~~~~~~ Jummps & Calls ~~~~~~~~~~~~~~~~
            INS_JSR          = 0x20,
            INS_RTS          = 0x60,
            INS_JMP_ABS      = 0x4C,
            INS_JMP_I        = 0x6C,
        // ~~~~~~~~~~~~~~~~ Stack Operations ~~~~~~~~~~~~~~~~
            INS_TSX          = 0xBA,
            INS_TXS          = 0x9A,
            INS_PHA          = 0x48,
            INS_PHP          = 0x08,
            INS_PLA          = 0x68,
            INS_PLP          = 0x28,
        // ~~~~~~~~~~~~~~~~ Logical ~~~~~~~~~~~~~~~~
        // TODO Implement logical insturctions
            INS_AND_IM       = 0x29,
            INS_AND_ZP       = 0x25,
            INS_AND_ZPX      = 0x35,
            INS_AND_ABS      = 0x2D,
            INS_AND_AX       = 0x3D,
            INS_AND_AY       = 0x39,
            INS_AND_IX       = 0x21,
            INS_AND_IY       = 0x31,
        // EOR
            INS_EOR_IM       = 0x49,
            INS_EOR_ZP       = 0x45,
            INS_EOR_ZPX      = 0x55,
            INS_EOR_ABS      = 0x4D,
            INS_EOR_AX       = 0x5D,
            INS_EOR_AY       = 0x59,
            INS_EOR_IX       = 0x41,
            INS_EOR_IY       = 0x51,
        // ORA
            INS_ORA_IM       = 0x09,
            INS_ORA_ZP       = 0x05,
            INS_ORA_ZPX      = 0x15,
            INS_ORA_ABS      = 0x0D,
            INS_ORA_AX       = 0x1D,
            INS_ORA_AY       = 0x19,
            INS_ORA_IX       = 0x01,
            INS_ORA_IY       = 0x11,
        // BIT
            INS_BIT_ZP       = 0x24,
            INS_BIT_ABS      = 0x2C;

    private:
        Memory& mem_ref;
        s32 cycles;

        // addressing modes
        // http://www.emulator101.com/6502-addressing-modes.html

        // addressing mode functions
        word address_mode_zero_page_and_immediate();
        word address_mode_zero_page_x_offset();
        word address_mode_zero_page_y_offset();
        word address_mode_absolute();
        word address_mode_absolute_x_offset();
        word address_mode_absolute_y_offset();
        word address_mode_indirect_x_offset();
        word address_mode_indirect_y_offset();

        // extra cycle for corssing page boundary
        word address_mode_abosolute_x_offset_with_page_cycle();
        word address_mode_abosolute_y_offset_with_page_cycle();
        word address_mode_indirect_x_offset_with_page_cycle();
        word address_mode_indirect_y_offset_with_page_cycle();

        // sets zero flags if reg is zero, and negative flag if bit 7 of reg is set
        void zero_and_negative_flag_set(byte reg)
        {
            // set zero flag if reg is zero
            flag.Z = (reg == 0);
            // set negative flag if bit 7 of reg is set
            flag.N = CHECK_BIT(reg, 7);
        }

        byte fetch_byte();
        byte read_byte(word);
        word fetch_word();
        word read_word(word);
        void write_byte(byte, word);
        void write_word(word, word);
        void push_pc_sp();
        word pop_word_from_stack();
        byte pop_byte_from_stack();
        void push_byte_to_stack(byte);
    };
}

#endif