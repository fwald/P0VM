#pragma once
#ifndef P0VM_TYPES
#define P0VM_TYPES

#include <stdint.h>


typedef int8_t Byte;
typedef uint32_t MemOffset;


/*
    Constants
*/
typedef enum vm_registers {
    RA =0,
    RB,
    RC,
    RD,
    RE,
    RF,
    RG,
    RH,
    NUM_REGISTERS, // Number of general purpose registers
} RegisterName ;


typedef struct register_t  {
    int32_t store; 
} Register ;

typedef struct memory_stack_t {
    Byte* base;
    MemOffset top; //offset, in bytes, from base to top of stack 
} Stack ;

typedef struct stack_frame_t {
    MemOffset base;
} StackFrame;


typedef struct memory_heap_t {
    Byte* base; 
} Heap ;



#endif 
