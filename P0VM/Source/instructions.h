#pragma once
#ifndef P0VM_INSTRUCTIONS_H
#define P0VM_INSTRUCTIONS_H

#include "vm_types.h"
#include "op_codes.h"


enum instruction_constants {
    INSTRUCTION_SIZE_BYTES = 6,
};
  
/*
*   Most significant   --------->     Least significant
    Byte 5 | Byte 4 | Byte 3 | Byte 2 | Byte 1 | Byte 0 
    op-code|optional|optional|optional|optional|optional|
*/

typedef struct instruction_t {
    Byte bytes[INSTRUCTION_SIZE_BYTES];
} Instruction, I_Return, I_Stop ;

typedef union load_stack_offset_instruction_t {
    Instruction _instruction;
    struct {
        MemOffset address; //The offset, in bytes, from stack base 
        Byte reg; //Target register
        Byte opcode;
    };
} I_LoadStackOffset; 

typedef union reg_intconst_instruction_t {
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

//Allocates an object to the heap, stores the address to the object in a register
typedef union alloc_instruction_t {
    Instruction _instruction;
    struct {
        MemOffset size; //Size, in bytes, of allocation
        Byte dstReg; // Register where memory offset to beginning of allocation will be stored 
        Byte opcode;
    };
} I_Alloc;

// Store the value from one register to the other
typedef union store_instruction_t {
    Instruction _instruction;
    struct {
        Byte _pad0;
        Byte _pad1;
        Byte _pad2;
        Byte src_reg; // Contains the value 
        Byte dst_reg; // Receives value 
        Byte opcode;
    };
} I_Store;


// Store value, from src_reg to the heap at memory address contained in addr_reg
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

// Load value from heap, at address stored in addr_reg into src_reg
typedef union load_instruction_t {
    Instruction _instruction;
    struct {
        Byte _pad0;
        Byte _pad1;
        Byte _pad2;
        Byte addr_reg;
        Byte src_reg;
        Byte opcode;
    };
} I_Load;

// Load value from stack, at offset from stack base stored in addr_reg into src_reg
typedef union load_stack_from_reg_instruction_t {
    Instruction _instruction;
    struct {
        Byte _pad0;
        Byte _pad1;
        Byte _pad2;
        Byte addr_reg;
        Byte src_reg;
        Byte opcode;
    };
} I_LoadStackFromReg;

/* store a value in a register to the stack, using an offset that is stored in a register
* addr_reg contains stack offset(from stack base) that the value will be stored in,
*  val_reg  contains the value which will be stored to the stack
*/
typedef union store_to_stack_offset_from_reg_instruction_t {
    Instruction _instruction;
    struct {
        Byte _pad0;
        Byte _pad1;
        Byte _pad2;
        Byte value_reg;
        Byte addr_reg;
        Byte opcode;
    };
} I_StoreToStackOffsetFromReg;

// Binop instructions, reg_op_x contains the first argument, reg_op_y the second, the result is stored in dest_reg
// CompareEquals and CompareLess also sets the COMPARE flag of the RFLAGS register
// if the comparison results in true, COMPARE is set to 1 otherwise to 0
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
}I_Binop, I_Add, I_Sub, I_Mul, I_Div, I_CompareEquals, I_CompareLess, I_And, I_Or;


typedef union single_reg_instruction_t {
    Instruction _instruction;
    struct {
        Byte _pad0;
        Byte _pad1;
        Byte _pad2;
        Byte _pad3;
        Byte reg; // The register whose value will be incremented or decremented. Also destination register for Pop
        Byte opcode;
    };
}I_Increment, I_Decrement, I_Pop;


typedef union printl_instruction_t {
    Instruction _instruction;
    struct {
        Byte _pad0;
        Byte _pad1;
        Byte _pad2;
        Byte _pad3;
        Byte reg; // For PRINTLN it contains the address to the string literal, for PRINTLN_INT it contains the int to be printed
        Byte opcode;
    };
} I_PrintLn, I_PrintLnInt;



// Pushes current instruction pointer to the stack, jumps to instruction address stored in reg
typedef union call_instruction_t {
    Instruction _instruction;
    struct {
        Byte _pad0;
        Byte _pad1;
        Byte _pad2;
        Byte _pad3;
        Byte reg; // Contains the instruction address which will be jumped to 
        Byte opcode;
    };
} I_Call;


// Checks if the value stored in reg is equal to NULL_CONSTANT
// If it is, the VM will crash
typedef union null_check_instruction_t {
    Instruction _instruction;
    struct {
        Byte _pad0;
        Byte _pad1;
        Byte _pad2;
        Byte _pad3;
        Byte reg; 
        Byte opcode;
    };
} I_NullCheck;


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

// Concatenation instruction. Concatenation order: arg0 arg1 
// The resulting string is stored on the heap and the address to the string is returned in dest_reg
typedef union concat_str_t {
    Instruction _instruction;
    struct {
        Byte _pad0;
        Byte _pad1;
        Byte dest_reg;
        Byte arg1_reg;
        Byte arg0_reg;
        Byte opcode;
    };
} I_ConcatStrStr, I_ConcatStrInt, I_ConcatIntStr;



#endif // !P0VM_INSTRUCTIONS_H

