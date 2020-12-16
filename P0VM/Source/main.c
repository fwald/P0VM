#include "vm.h"

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>



char* program_name = "compiled_program.pvm";



//Util functions
void generate_test_program_1(); 
void generate_test_program_2(); 
void write_program_to_file();
RetCode read_program_from_file(char* filename, _OUT_PARAM char** pmem, _OUT_PARAM size_t* program_size,_OUT_PARAM size_t* num_instructions );

char register_letter(RegisterName rn) {
    return 'A' + rn;
}


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

    
    printf("Loading program...");
    Instruction* instructions = NULL ; 
    size_t num_instructions = 0;
    size_t program_size = 0;

    if (read_program_from_file(program_name, &pmemory, &program_size, &num_instructions )) {
        printf("    ...Done!\n");
    }
    else {
        printf("   ...Failed!\n");
        return -1; 
    }
    instructions = (Instruction*) pmemory; 
    instructions++; // First 6 bytes is header data 


    Stack stack = { .base = pmemory + memory_size, .top =0 }; // Grow stack from high to low 
    
    //Make sure that we align heap memory;
    size_t heap_start = program_size;
    if (heap_start % 2) {
        heap_start++; 
    }
    Heap heap = { .base = pmemory + heap_start };  // Grow heap from low to high

    printf("(Half-Life scientist): Everything... seems to be in order!\n\n");
    
//    generate_test_program_1();
 //   write_program_to_file();
    uint32_t i = 0;

    int IP = 0 ; // Instruction offset 
    while ( i < num_instructions) {
        Instruction in = instructions[i];
        Byte opcode = in.bytes[5];
        i++; 

        switch (opcode)
        {
        case I_NOOP:  {
            printf("NOOP instruction\n");
        } break;
        case I_STORE: {
            printf("STORE instruction\n");
        } break;
        case I_STORE_HEAP_OFFSET: {
            printf("STORE_HEAP_OFFSET instruction\n");
            I_StoreHeap* store = (I_StoreHeap*)&in; 
            int val = get_register(registers, store->reg);
            set_memory(pmemory, memory_size, store->offset, val);
        } break;
        case I_STORE_STACKFRAME_OFFSET: {
            I_StoreStack* store = (I_StoreStack*)&in;

            MemOffset offset = store->offset;
            int32_t val = get_register(registers, store->reg);

            //TODO Create a function. Also implement for StackFrames
            Byte* pval = stack.base - offset;
            int32_t* pint = (int32_t*)pval;
            *pint = val;
            printf("STORE_STACKFRAME_OFFSET: %c -> (%d)\n", register_letter(store->reg), val );
        } break;
        case I_LOAD_STACKFRAME_OFFSET: {
            I_LoadStackOffset* load = (I_LoadStackOffset*) &in; 
            int stack_offset = load->address;
            //TODO implement StackFrames

            Byte* pval = stack.base -  stack_offset;
            int32_t value = (*(int32_t*)pval); 
            set_register(registers, load->reg, value);

            printf("LOAD_STACK_OFFSET: R%c <- val:(%d), offset: %d\n", register_letter(load->reg), value, stack_offset );
        } break;
        case I_LOAD_CONST: {
            I_LoadConst* load = (I_LoadConst*) &in; 
            set_register(registers, load->reg, load->intlit);
            printf("LOAD_CONST: R%c <- %d \n", register_letter(load->reg), load->intlit);
        } break;
        case I_ADD: {
            I_Add* add = (I_Add*)&in;
            int x = get_register(registers, add->reg_op_x);
            int y = get_register(registers, add->reg_op_y);
            set_register(registers, add->dest_reg, x + y );
            printf("ADD: R%c <- %d + %d = %d \n", register_letter(add->dest_reg), x, y, x+y);
        } break;
        case I_SUB: {
            I_Sub* sub = (I_Sub*)&in;
            int x = get_register(registers, sub->reg_op_x);
            int y = get_register(registers, sub->reg_op_y);
            set_register(registers, sub->dest_reg, x - y );
            printf("SUB: R%c <- %d - %d = %d \n", register_letter(sub->dest_reg), x, y, x-y);
        } break;
        case I_DIV: {
            I_Div* div = (I_Div*)&in;
            int x = get_register(registers, div->reg_op_x);
            int y = get_register(registers, div->reg_op_y);
            set_register(registers, div->dest_reg, x / y );
            printf("DIV: R%c <- %d / %d = %d \n", register_letter(div->dest_reg), x, y, x/y);

        } break;
        case I_MUL: {
            I_Mul* mul = (I_Mul*)&in;
            int x = get_register(registers, mul->reg_op_x);
            int y = get_register(registers, mul->reg_op_y);
            set_register(registers, mul->dest_reg, x * y );
            printf("MUL: R%c <- %d * %d = %d \n", register_letter(mul->dest_reg), x, y, x*y);
        } break;
        case I_AND: {
            I_And* and = (I_And*)&in;
            int x = get_register(registers, and->reg_op_x);
            int y = get_register(registers, and->reg_op_y);
            set_register(registers, and->dest_reg, x && y );
            printf("AND: R%c <- %d && %d = %d\n", register_letter(and->dest_reg),x,y,x&&y);
        } break;
        case I_OR: {
            I_Or* or = (I_Or*)&in;
            int x = get_register(registers, or->reg_op_x);
            int y = get_register(registers, or->reg_op_y);
            set_register(registers, or->dest_reg, x || y );
            printf("OR: R%c <- %d || %d = %d \n",register_letter(or->dest_reg),x,y,x||y);
        } break;
        case I_INCR: {
            I_Increment* incr  = (I_Increment*)& in;
            add_to_register(registers, incr->reg, 1);
            printf("INCR: R%c\n", register_letter(incr->reg));
        } break;
        case I_DECR: {
            I_Decrement* incr  = (I_Decrement*)& in;
            add_to_register(registers, incr->reg, -1);
            printf("DECR R%c\n", register_letter(incr->reg));
        } break;
        case I_CMP_EQ: {
            I_CompareEquals* eq = (I_CompareEquals*)&in;
            int x = get_register(registers, eq->reg_op_x);
            int y = get_register(registers, eq->reg_op_y);
            set_flag(registers, RFLAG_COMPARE, (x == y));
            printf("CMP_EQ \n");
            print_register_flags(registers);
        } break;
        case I_CMP_LESS: {
            I_CompareLess* eq = (I_CompareLess*)&in;
            int x = get_register(registers, eq->reg_op_x);
            int y = get_register(registers, eq->reg_op_y);
            set_flag(registers, RFLAG_COMPARE, (x < y));
            
            printf("CMP_LESS\n");
            print_register_flags(registers);
        } break;
        case I_JMP: {
            I_Jump* jmp = (I_Jump*)&in;
            i = jmp->instruction_nr; 
            printf("JMP\n");
        } break;

        case I_JMPEQ: {
            I_JumpEquals* jmp = (I_JumpEquals*)&in;
            if (get_flag(registers, RFLAG_COMPARE) == 0 ) {
                i = jmp->instruction_nr; 
            }
            printf("JMPEQ\n");
        } break; 
        case I_JMPNEQ: {
            I_JumpNeq* jmp = (I_JumpNeq*)&in;
            if (get_flag(registers, RFLAG_COMPARE) == 1) {
                i = jmp->instruction_nr; 
            }
            printf("JMPNEQ\n");
        } break;
        case I_PUSH: {
            I_Push* psh = (I_Push*)&in;
            int value = get_register(registers, psh->srcReg);
            push(&stack, value);
            printf("PUSH: R%c -> (%d)\n", register_letter(psh->srcReg), value);
        } break;
        case I_PUSH_INT: {
            I_PushInt* psh = (I_PushInt*)&in;
            push(&stack, psh->value);
            printf("PUSH_INT: (%d)\n", psh->value);
        } break;
        case I_POP: {
            I_Pop* ipop = (I_Pop*)&in;
            int32_t val = pop(&stack);
            set_register(registers, ipop->reg, val);
            printf("POP: R%c <- (%d)\n", register_letter(ipop->reg), val);
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

    printf("\n");
    print_registers(registers) ;
    print_stack(&stack);
  //  print_int_at_memory_offset(pmemory, memory_size, 0);


    free(pmemory);
    printf("\nTerminating, good bye!\n");
    return 0; 
}

void set_flag(Register* registers, RegisterFlagShifts shifts, int flag_value ) {
    registers[RFLAGS].store &= ~(1 << shifts); //Clear flag 
    registers[RFLAGS].store |= (flag_value << shifts); //Set flag 
}

int get_flag(Register* registers, RegisterFlagShifts shifts) {
    return ((registers[RFLAGS].store >> shifts) & 1); 
}

void push(Stack* stack, int32_t value) {
    increment_stack_pointer(stack, (MemOffset)sizeof(value)); //make space
    Byte* p = stack->base - stack->top; //Get pointer to top of stack, stack grows downwards, hence the '-' 
    int32_t* pint = (int32_t*)p; 
    (*pint) = value;
}

int32_t pop(Stack* stack) {
    Byte* p = (stack->base - stack->top); 
    int32_t val = (*(int32_t*)(p));
    decrement_stack_pointer(stack, (MemOffset)sizeof(val) );
    return val; 
}


void increment_stack_pointer(Stack* stack, MemOffset increment ) {
    int64_t signed_offset = (int64_t)stack->top;
    if ( (signed_offset + increment) > 0xFFFFFFFFLL) {
        printf("Error, stack overflow! You have consumed the entire memory of the VM :( \n");
        assert(0); //For now, just shut down
    }
    stack->top += increment;  
}

void decrement_stack_pointer(Stack* stack, MemOffset decrement) {
    int64_t signed_offset = (int64_t)stack->top;
    if ((signed_offset - decrement) <0)  {
        printf("[FATAL-ERROR]: Stack underflow! Trying to pop more elements of the stack than it contains! Check your code-gen! \n");
        assert(0); // Crasch 
    }
    stack->top -= decrement; 
}


void print_stack(Stack* stack) {
    Byte* p = (stack->base - stack->top); 
    printf("Printing stack\n");
    for (p; p < stack->base; p += sizeof(int32_t)) {
        printf("%d\n", (*(int32_t*)(p)));
    }
}


void generate_test_program_1() {
    //I_LoadConst load1 = { .opcode = I_LOAD_CONST,  .reg = RA, .intlit = 666 };
    //I_LoadConst load2 = { .opcode = I_LOAD_CONST,  .reg = RB, .intlit = 111 };
    //I_Add add1 = { .opcode = I_ADD, .reg_op_x = RA, .reg_op_y = RB, .dest_reg = RC };
    //I_StoreHeap store1 = { .opcode = I_STORE_HEAP_OFFSET, .reg = RC, .offset = 0 };

    //I_LoadConst load3 = { .opcode = I_LOAD_CONST,  .reg = RA, .intlit = 2 };
    //I_LoadConst load4 = { .opcode = I_LOAD_CONST,  .reg = RB, .intlit = 5 };
    //I_Sub sub1 = { .opcode = I_SUB, .reg_op_x = RA, .reg_op_y = RB, .dest_reg = RE };
    //I_Mul mul1 = { .opcode = I_MUL, .reg_op_x = RA, .reg_op_y = RB, .dest_reg = RF };
    //I_Div div1 = { .opcode = I_DIV, .reg_op_x = RF, .reg_op_y = RA, .dest_reg = RG };

    //I_CompareEquals cmpeq1 = { .opcode = I_CMP_EQ, .reg_op_x = RA, .reg_op_y = RB, .dest_reg = RD };
    //I_CompareEquals cmpeq2 = { .opcode = I_CMP_EQ, .reg_op_x = RA, .reg_op_y = RA, .dest_reg = RH };

    //I_Increment incr1 = { .opcode = I_INCR, .reg = RH };
    //I_Decrement decr1 = { .opcode = I_DECR, .reg = RB };

    //g_test_program[g_test_program_len++] = load1._instruction;
    //g_test_program[g_test_program_len++] = load2._instruction;
    //g_test_program[g_test_program_len++] = add1._instruction;
    //g_test_program[g_test_program_len++] = store1._instruction;

    //g_test_program[g_test_program_len++] = load3._instruction;
    //g_test_program[g_test_program_len++] = load4._instruction;
    //g_test_program[g_test_program_len++] = sub1._instruction;
    //g_test_program[g_test_program_len++] = mul1._instruction;
    //g_test_program[g_test_program_len++] = div1._instruction;

    //g_test_program[g_test_program_len++] = cmpeq1._instruction;
    //g_test_program[g_test_program_len++] = cmpeq2._instruction;

    //g_test_program[g_test_program_len++] = incr1._instruction;
    //g_test_program[g_test_program_len++] = decr1._instruction;
}

void generate_test_program_2() {
    //I_LoadConst instr_0 = { .opcode = I_LOAD_CONST,  .reg = RA, .intlit = 10 };
    //I_Increment instr_1 = { .opcode = I_INCR, .reg = RB };
    //I_Decrement instr_2 = { .opcode = I_DECR, .reg = RA };

    //I_JumpEquals instr_3 = { .opcode = I_JMPNEQ, .reg = RA, .instruction_nr = 1 };

    //
    //g_test_program[g_test_program_len++] = instr_0._instruction;
    //g_test_program[g_test_program_len++] = instr_1._instruction;
    //g_test_program[g_test_program_len++] = instr_2._instruction;
    //g_test_program[g_test_program_len++] = instr_3._instruction;
}


void print_registers(Register* r) {
    for (int i = 0; i < NUM_GENERAL_REGISTERS; i++) {
        printf("R%c: [%d]\n", 'A'+i, r[i].store);
    }

    print_register_flags(r);

}

void print_register_flags(Register* registers) {
    printf("RFLAGS: ");
    for (int i = RFLAG_NUM_FLAGS-1; i >= 0; i--) {
        printf("%d ", (registers[RFLAGS].store >> i) & 1);
       
    }
    printf("\n");
}


void print_register(Register* registers, RegisterName rn) {
   assert(rn < NUM_REGISTERS); 
   printf("R%c: [%d]\n", 'A'+rn, registers[rn].store);
}

void add_to_register(Register* registers, RegisterName rn, int val) {
    assert(rn >= 0 && rn < NUM_GENERAL_REGISTERS); 
    registers[rn].store += val; 
}

void clear_registers(Register* r )  {
    for (int i = 0; i < NUM_REGISTERS; i++) {
        r[i].store = 0; 
    }
}


void set_register(Register* registers, RegisterName r, int val) {
    assert(r >= 0 && r < NUM_GENERAL_REGISTERS);
    registers[r].store = val; 
}

int get_register(Register* registers, RegisterName r) {
    assert(r >= 0 && r < NUM_GENERAL_REGISTERS);
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


void write_program_to_file() {
    //FILE* file =  fopen("program.pvm", "wb");
    //if (!file) {
    //    printf("Could not open file for writing!\n");
    //    return; 
    //}

    //fwrite(g_test_program, sizeof(Instruction), g_test_program_len, file);
    //fclose(file);
}

RetCode read_program_from_file(char* filename, _OUT_PARAM char** pmem, _OUT_PARAM size_t* program_size,_OUT_PARAM size_t* num_instructions  ) {
    FILE* file = fopen(filename , "rb");
    if (file) {
        size_t filesize = 0;
        fseek(file, 0L, SEEK_END);
        filesize = ftell(file);
        fseek(file, 0L, SEEK_SET);
      
        //g_test_program_size = filesize; 
        //g_test_program_len = filesize / INSTRUCTION_SIZE_BYTES;
//        g_test_program = (Instruction*)malloc(filesize);

        RetCode ret = RETCODE_ERROR;
        fread(*pmem, sizeof(char), filesize, file);

        int32_t* n_instructions = (int32_t*)*pmem;
       // g_test_program_len = *n_instructions; 
        (*program_size) = filesize;
        (*num_instructions) = *n_instructions;

        ret = RETCODE_OK;

        fclose(file);
        return ret; 
    }
    else {
        printf("Could not open file for reading!\n");
        return RETCODE_ERROR;
    }
}

