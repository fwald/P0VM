#pragma once
#ifndef P0VM_INSTRUCTIONS_H
#define P0VM_INSTRUCTIONS_H


#include "vm_types.h"

enum instruction_opcodes {
    I_NOOP = 0,
    I_STORE, // Mem[] <- Register
    I_STORE_AT, // Store from register to the specified address 
    I_STORE_CONST,
    I_LOAD, // Register <- Mem[]
    I_LOAD_CONST,
    I_PUSH,
    I_POP,
    I_ADD,
    I_SUB,
    I_DIV,
    I_MUL,
    I_CALL,
    I_PRINTLN,
};

/*
*   Most significant   --------->     Least significant
    Byte 5 | Byte 4 | Byte 3 | Byte 2 | Byte 1 | Byte 0 
    op-code|optional|optional|optional|optional|optional|
*/

typedef struct instruction_t {
    Byte bytes[6];
} Instruction ;

typedef union load_instruction_t {
    Instruction _instruction;
    struct {
        MemOffset address; //Memory address to load from
        Byte reg; //Target register
        Byte opcode;
    };
} I_Load;

typedef union load_const_instruction_t {
    Instruction _instruction;
    struct {
        int32_t intlit; //constant value   
        Byte reg; //Target register
        Byte opcode;
    };
}I_LoadConst;


typedef union store_instruction_t {
    Instruction _instruction;
    struct {
        MemOffset dest_address; //Memory address to store to
        Byte reg; // Source register
        Byte opcode;
    };
} I_StoreAt;


typedef union binop_instruction_t {
    Instruction _instruction;
    struct {
        Byte _pad0;
        Byte _pad1;
        Byte dest_reg;
        Byte reg_op_y;
        Byte reg_op_x;
        Byte opcode;
    };
}I_Add, I_Sub, I_Mul, I_Div ;


#endif // !P0VM_INSTRUCTIONS_H

