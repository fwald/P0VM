#pragma once 
#ifndef P0VM_OPCODES_H
#define P0VM_OPCODES_H
/*
Instruction OpCodes for P0VM, this file has been automatically generated
*/
enum instruction_opcodes {
    I_NOOP,
    I_STORE,
    I_STORE_TO_STACK_OFFSET,
    I_STORE_HEAP_OFFSET,
    I_STORE_TO_STACK_OFFSET_FROM_REG,
    I_LOAD,
    I_LOAD_FROM_STACK_OFFSET_FROM_REG,
    I_LOAD_FROM_STACK_OFFSET,
    I_LOAD_CONST,
    I_PUSH,
    I_PUSH_INT,
    I_POP,
    I_ADD,
    I_SUB,
    I_ADD_REG,
    I_SUB_REG,
    I_DIV,
    I_MUL,
    I_INCR,
    I_DECR,
    I_AND,
    I_OR,
    I_CMP_EQ,
    I_CMP_LESS,
    I_IF,
    I_IFELSE,
    I_JMP,
    I_JMPEQ,
    I_JMPNEQ,
    I_CALL,
    I_NULL_CHECK,
    I_ALLOC,
    I_RETURN,
    I_PRINTLN,
    I_PRINTLN_INT,
    I_CONCAT_STR_STR,
    I_CONCAT_STR_INT,
    I_CONCAT_INT_STR,
    I_STOP,
};


#endif // P0VM_OPCODES_H
