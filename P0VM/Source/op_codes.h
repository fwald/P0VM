#pragma once 
#ifndef P0VM_OPCODES_H
#define P0VM_OPCODES_H
/*
Instruction OpCodes for P0VM, this file has been automatically generated
*/
enum instruction_opcodes {
    I_NOOP,
    I_STORE,
    I_STORE_STACKFRAME_OFFSET,
    I_STORE_HEAP_OFFSET,
    I_LOAD,
    I_LOAD_STACKFRAME_OFFSET,
    I_LOAD_CONST,
    I_PUSH,
    I_PUSH_INT,
    I_POP,
    I_ADD,
    I_SUB,
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
    I_PRINTLN,
    I_PRINTLN_INT,
};


#endif // P0VM_OPCODES_H
