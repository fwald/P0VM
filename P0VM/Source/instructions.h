#pragma once
#ifndef P0VM_INSTRUCTIONS_H
#define P0VM_INSTRUCTIONS_H


#include "vm_types.h"
#include "op_codes.h"

enum bool_values {
    BOOL_FALSE = 0,
    BOOL_TRUE = 1, 
};

enum instruction_constants {
    INSTRUCTION_SIZE_BYTES = 6,
};
   

/*
    I_STORE, // Mem[] <- Register
    I_STORE_AT, // Store from register to the specified address 
    I_LOAD, // Register <- Mem[]
    I_JMPEQ, // Jumpt if value in register is == 0 
    I_JMPNEQ, // Jumpt if value in register is != 0 
*/

/*
*   Most significant   --------->     Least significant
    Byte 5 | Byte 4 | Byte 3 | Byte 2 | Byte 1 | Byte 0 
    op-code|optional|optional|optional|optional|optional|
*/

typedef struct instruction_t {
    Byte bytes[INSTRUCTION_SIZE_BYTES];
} Instruction, I_Return, I_Stop ;

typedef union load_instruction_t {
    Instruction _instruction;
    struct {
        MemOffset address; //Memory address to load from, for I_LoadStackOffset -the offset, in bytes, from current *stack-frame base-pointer*
        Byte reg; //Target register
        Byte opcode;
    };
} I_Load, I_LoadStackOffset; 

typedef union load_const_instruction_t {
    Instruction _instruction;
    struct {
        int32_t intlit; //constant value   
        Byte reg; //Target register
        Byte opcode;
    };
}I_LoadConst, I_AddReg, I_SubReg ;

typedef union store_instructions_t {
    Instruction _instruction;
    struct {
        MemOffset offset; //Memory address to store to
        Byte reg; // Source register
        Byte opcode;
    };
} I_StoreStack;

typedef union alloc_instruction_t {
    Instruction _instruction;
    struct {
        MemOffset size; //Size, in bytes, of allocation
        Byte dstReg; // Register where memory offset to beginning of allocation will be stored 
        Byte opcode;
    };
} I_Alloc;



typedef union src_dst_register_instruction_t {
    Instruction _instruction;
    struct {
        Byte _pad0;
        Byte _pad1;
        Byte _pad2;
        Byte src_reg;
        Byte dst_reg;
        Byte opcode;
    };
} I_Store;

typedef union store_heap_instruction_t {
    Instruction _instruction;
    struct {
        Byte _pad0;
        Byte _pad1;
        Byte _pad2;
        Byte addr_reg;
        Byte src_reg;
        Byte opcode;
    };
} I_StoreHeap;



typedef union binop_instruction_t {
    Instruction _instruction;
    struct {
        Byte _pad0;
        Byte _pad1;
        Byte dest_reg; // Ignored for Compare instructions
        Byte reg_op_y;
        Byte reg_op_x;
        Byte opcode;
    };
}I_Binop, I_Add, I_Sub, I_Mul, I_Div, I_CompareEquals, I_CompareLess, I_And, I_Or;

typedef union single_reg_instruction_t {
    Instruction _instruction;
    struct {
        Byte _pad0;
        Byte _pad1;
        Byte _pad2;
        Byte _pad3;
        Byte reg; // The register whose value will be incremented or decremented. Also destination register for Pop, src register for PrintLn
        Byte opcode;
    };
}I_Increment, I_Decrement, I_Pop, I_PrintLn, I_PrintLnInt, I_Call; 


typedef union jump_instruction_t {
    Instruction _instruction;
    struct {
        MemOffset instruction_nr; // Jumpt to this instruction number. Note that since this is a uint32 we have a max size of the jump that can be made
        Byte reg; 
        Byte opcode;
    };
} I_JumpEquals, I_JumpNeq, I_Jump;

typedef union push_instruction_t {
    Instruction _instruction;
    struct {
        int32_t value; //Only used for PushInt, Keep 32-bit value in the lower 4 bytes. Makes sure that it is aligned
        Byte srcReg; // Only used For Push
        Byte opcode; 
    };
} I_Push, I_PushInt;



#endif // !P0VM_INSTRUCTIONS_H

