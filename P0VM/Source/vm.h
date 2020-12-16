#pragma once
#ifndef P0VM_H
#define P0VM_H

#include "vm_types.h"
#include "instructions.h"

#define _OUT_PARAM 


typedef enum return_codes_t {
    RETCODE_ERROR,
    RETCODE_OK,
} RetCode;


void clear_registers(Register* registers);
void print_registers(Register* registers);
void print_register(Register* registers, RegisterName rn);
void print_register_flags(Register* registers);


/*
* Sets register r to val 
*/
void set_register(Register* registers, RegisterName r, int val); 

/*
* Returns the value stored in register r
*/
int get_register(Register* registers, RegisterName r); 
void add_to_register(Register* registers, RegisterName r, int val);

void set_flag(Register* registers, RegisterFlagShifts shifts, int flag_value);
int get_flag(Register* registers, RegisterFlagShifts shifts);

/*
* Set the memory to val at offset bytes from membase 
*/
void store_heap( Heap* heap, MemOffset offset, int val);
MemOffset heap_alloc(Heap* heap, uint32_t size);


void print_int_at_memory_offset(Byte* membase, size_t memsize, MemOffset offset);

// Stack stuff
void push(Stack* stack, Register* registers, int32_t value);
int32_t pop(Stack* stack, Register* registers);
void increment_stack_pointer(Stack* stack,Register* registers, MemOffset increment);
void decrement_stack_pointer(Stack* stack,Register* registers, MemOffset decrement);
/*
    Print stack, beginning with top element
*/
void print_stack(Stack* stack, Register* registers);




#endif 
