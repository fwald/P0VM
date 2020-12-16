#pragma once
#ifndef P0VM_TYPES
#define P0VM_TYPES

#include <stdint.h>


typedef int8_t Byte;
typedef uint32_t MemOffset; 

/*
    Constants
*/

// General purpose registers
typedef enum vm_registers {
    RA =0,
    RB,
    RC,
    RD,
    RE,
    RF,
    RBP, //Stack Base-Pointer
    RSP, //Stack Top-Pointer
    NUM_GENERAL_REGISTERS, // Number of general purpose registers
    RFLAGS = NUM_GENERAL_REGISTERS, // A special register where status flags are set
    RIP, // Instruction Pointer
    NUM_REGISTERS,
} RegisterName ;

typedef enum rflags_shifts {
    RFLAG_COMPARE , 
    RFLAG_OVERFLOW,
    RFLAG_SIGN,
    RFLAG_NUM_FLAGS,
} RegisterFlagShifts ;


typedef struct register_t  {
    int32_t store; 
} Register ;

typedef struct memory_stack_t {
    Byte* base;
  //  MemOffset top; //offset, in bytes, from base to top of stack, i.e. the stack-pointer (SP)
} Stack ;

typedef struct stack_frame_t {
    MemOffset top;
    MemOffset base;
} StackFrame;


typedef struct memory_heap_t {
    Byte* base;
    MemOffset free; 
} Heap ;

#endif 
