#pragma once
#ifndef P0VM_H
#define P0VM_H

#include "vm_types.h"
#include "instructions.h"


void clear_registers(Register* registers);
void print_registers(Register* registers);
void print_register(Register* registers, RegisterName rn);


/*
* Sets register r to val 
*/
void set_register(Register* registers, RegisterName r, int val); 

/*
* Returns the value stored in register r
*/
int get_register(Register* registers, RegisterName r); 

void add_to_register(Register* registers, RegisterName r, int val);

/*
* Set the memory to val at offset bytes from membase 
*/
void set_memory(Byte* membase, size_t memsize, MemOffset offset, int val);

void print_int_at_memory_offset(Byte* membase, size_t memsize, MemOffset offset);




#endif 
