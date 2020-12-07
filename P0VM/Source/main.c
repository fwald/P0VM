#include "vm.h"

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>


Instruction g_test_program[100];
int g_test_program_len = 0;

void generate_test_program(); 


int main(int argv, char argc[]) {
   
    printf("Initializing Punkt0 VM ...\n");

   
    Register registers[NUM_REGISTERS]; // General purpose registers
    clear_registers(registers);

    printf("Allocating memory... ");
    //Get memory 
    size_t memory_size = 0xFFFFFFFFULL; // 4GiB
    Byte* pmemory = (Byte*)calloc(memory_size, sizeof(Byte));
    if (!pmemory) {
        printf("[Fatal error]: Failed to allocate VM memory\n");
        return -1; 
    }
    printf("    ...Done!\n");

    Stack stack = { 0 };
    stack.base = pmemory + memory_size ;  // Grow stack from high to low 
    Heap heap = { 0 };  // Grow heap from low to high
    heap.base = pmemory; 
    
    printf("Reading instructions...");
    Instruction* instruction_stream = NULL ; 
    size_t num_instructions = 0;
    int IP = 0 ; // Instruction offset 
    //read instructions from file
    //char* filename = ""; 
    //FILE* instruction_file = fopen(filename, "r");
    //fclose(instruction_file);
    printf("    ...Done!\n");

    printf("(Half-Life scientist): Everything... seems to be in order!\n\n");

    generate_test_program();
    for (int i = 0; i < g_test_program_len; i++) {
        Instruction in = g_test_program[i];
        
        Byte opcode = in.bytes[5];
        switch (opcode)
        {
        case I_NOOP:  {
            printf("NOOP instruction\n");
        } break;
        case I_STORE: {
            printf("STORE instruction\n");
        } break;
        case I_STORE_AT: {
            printf("STORE_AT instruction\n");
            I_StoreAt* store = (I_StoreAt*)&in; 
            int val = get_register(registers, store->reg);
            set_memory(pmemory, memory_size, store->dest_address, val);
        } break;
        case I_STORE_CONST: {
            printf("STORE_CONST instruction\n");
        } break;
        case I_LOAD: {
            printf("LOAD instruction\n");
        } break;
        case I_LOAD_CONST: {
            I_LoadConst* load = (I_LoadConst*) &in; 
            set_register(registers, load->reg, load->intlit);
            printf("LOAD_CONST instruction\n");
        } break;
        case I_ADD: {
            I_Add* add = (I_Add*)&in;
            int x = get_register(registers, add->reg_op_x);
            int y = get_register(registers, add->reg_op_y);
            set_register(registers, add->dest_reg, x + y );
            printf("ADD instruction\n");
        } break;
        case I_SUB: {
            I_Sub* sub = (I_Sub*)&in;
            int x = get_register(registers, sub->reg_op_x);
            int y = get_register(registers, sub->reg_op_y);
            set_register(registers, sub->dest_reg, x - y );
            printf("SUB instruction\n");
        } break;
        case I_DIV: {
            I_Div* div = (I_Div*)&in;
            int x = get_register(registers, div->reg_op_x);
            int y = get_register(registers, div->reg_op_y);
            set_register(registers, div->dest_reg, x / y );
            printf("DIV instruction\n");
        } break;
        case I_MUL: {
            I_Mul* mul = (I_Mul*)&in;
            int x = get_register(registers, mul->reg_op_x);
            int y = get_register(registers, mul->reg_op_y);
            set_register(registers, mul->dest_reg, x * y );
            printf("MUL instruction\n");
        } break;
        case I_CALL: {
            printf("CALL instruction\n");
        } break;
        case I_PRINTLN: {
            printf("PRINTLN instruction\n");
        } break;
        default: {
            printf("UNKNOWN instruction\n");
        }   break;
        }
         
    }
    print_registers(registers) ;
    print_int_at_memory_offset(pmemory, memory_size, 0);


    free(pmemory);
    printf("\nTerminating, good bye!\n");
    return 0; 
}



void generate_test_program() {
    I_LoadConst load1 = { .opcode = I_LOAD_CONST,  .reg = RA, .intlit = 666 };
    I_LoadConst load2 = { .opcode = I_LOAD_CONST,  .reg = RB, .intlit = 111 };
    I_Add add1 = { .opcode = I_ADD, .reg_op_x = RA, .reg_op_y = RB, .dest_reg = RC };
    I_StoreAt store1 = { .opcode = I_STORE_AT, .reg = RC, .dest_address = 0 };
  

    I_LoadConst load3 = { .opcode = I_LOAD_CONST,  .reg = RA, .intlit = 2 };
    I_LoadConst load4 = { .opcode = I_LOAD_CONST,  .reg = RB, .intlit = 5 };
    I_Sub sub1 = { .opcode = I_SUB, .reg_op_x = RA, .reg_op_y = RB, .dest_reg = RE };
    I_Mul mul1 = { .opcode = I_MUL, .reg_op_x = RA, .reg_op_y = RB, .dest_reg = RF };
    I_Div div1 = { .opcode = I_DIV, .reg_op_x = RF, .reg_op_y = RA, .dest_reg = RG };



    g_test_program[g_test_program_len++] = load1._instruction;
    g_test_program[g_test_program_len++] = load2._instruction;
    g_test_program[g_test_program_len++] = add1._instruction;
    g_test_program[g_test_program_len++] = store1._instruction;
    
    g_test_program[g_test_program_len++] = load3._instruction;
    g_test_program[g_test_program_len++] = load4._instruction;
    g_test_program[g_test_program_len++] = sub1._instruction;
    g_test_program[g_test_program_len++] = mul1._instruction;
    g_test_program[g_test_program_len++] = div1._instruction;
    

}


void print_registers(Register* r) {
    for (int i = 0; i < NUM_REGISTERS; i++) {
        printf("R%c: [%d]\n", 'A'+i, r[i].store);
    }
}

void print_register(Register* registers, RegisterName rn) {
   assert(rn < NUM_REGISTERS); 
   printf("R%c: [%d]\n", 'A'+rn, registers[rn].store);
}


void clear_registers(Register* r )  {
    for (int i = 0; i < NUM_REGISTERS; i++) {
        r[i].store = 0; 
    }
}


void set_register(Register* registers, RegisterName r, int val) {
    assert(r >= 0 && r < NUM_REGISTERS);
    registers[r].store = val; 
}

int get_register(Register* registers, RegisterName r) {
    assert(r >= 0 && r < NUM_REGISTERS);
    return registers[r].store; 
}

void set_memory(Byte* membase, size_t memsize, MemOffset offset, int val) {
    assert(offset + 4 < memsize); // We will store a 32-bit int 
    int* mem = (int*)(membase + offset);
    *mem = val; 
}

void print_int_at_memory_offset(Byte* membase, size_t memsize, MemOffset offset) {
    assert(offset+4 < memsize );
    int* mem = (int*)(membase + offset);
    printf("Value at memory offset %u is %d\n", offset, (*mem)); 
}


