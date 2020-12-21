#include "vm.h"

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>

// Define this to print instructions as they are executed
//#define PRINT_DEBUG_INFO 


// Just a cheap macro so that we can decide if we want to print instructions or not
#ifdef PRINT_DEBUG_INFO 
#define PRINT_INSTRUCTION(x) x
#else
#define PRINT_INSTRUCTION(x) 
#endif 

char* program_name = "compiled_program.pvm";

/// Util functions
RetCode read_program_from_file(char* filename, _OUT_PARAM char** pmem, _OUT_PARAM size_t* program_size,_OUT_PARAM size_t* num_instructions );
char* register_namestr(RegisterName rn); 




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
    Instruction* instructions = NULL; 
    size_t num_instructions = 0; // Number of instructions of the main function
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

    char* storage = pmemory + (num_instructions+1)*6; // dispatch table + string storage
    size_t storage_size =  program_size - (num_instructions+1)*6;

    Stack stack = { .base = pmemory + memory_size }; 
    Heap heap = { .base = storage, .free = storage_size  };  // Grow heap from low to high

    printf("(half life scientist) everything.. seems to be in order\n");
    
    int running = BOOL_TRUE;
    while ( running ) {
        int32_t current_instruction = get_register(registers, RIP);
        assert((current_instruction) >= 0 && (current_instruction < num_instructions));
    
        Instruction in = instructions[current_instruction];
        add_to_register(registers, RIP, 1); //Increment Instruction Pointer
        
        PRINT_INSTRUCTION(printf("[%d] ", current_instruction);)

        Byte opcode = in.bytes[5];
         switch (opcode)
        {
        case I_NOOP:  {
            PRINT_INSTRUCTION(printf("NOOP instruction\n");)
        } break;
        case I_STORE: {
            I_Store* store = (I_Store*)&in; 
            set_register(registers, store->dst_reg, get_register(registers, store->src_reg));
            PRINT_INSTRUCTION(printf("STORE: %s <- %s  (%d)\n", 
                register_namestr(store->dst_reg), 
                register_namestr(store->src_reg), 
                get_register(registers, store->src_reg) 
            ) ;)
        } break;
        case I_STORE_HEAP_OFFSET: {
            I_StoreHeap* store = (I_StoreHeap*)&in; 
            int val = get_register(registers, store->src_reg);
            store_heap(&heap, get_register(registers, store->addr_reg), val);
            PRINT_INSTRUCTION(printf("STORE_HEAP_OFFSET: offset: %d <- (%d)\n", get_register(registers, store->addr_reg), val );)
        } break;
        case I_STORE_STACKFRAME_OFFSET: {
            I_StoreStack* store = (I_StoreStack*)&in;
            MemOffset offset = store->offset;
            int32_t val = get_register(registers, store->reg);
            Byte* pval = stack.base - offset;
            int32_t* pint = (int32_t*)pval;
            *pint = val;
            PRINT_INSTRUCTION(printf("STORE_STACKFRAME_OFFSET: %s -> (%d)\n", register_namestr(store->reg), val );)
        } break;
        case I_STORE_TO_STACK_OFFSET_FROM_REG: {
            I_StoreToStackOffsetFromReg* store = (I_StoreToStackOffsetFromReg*)&in;
            MemOffset offset = get_register(registers, store->addr_reg);
            int32_t value = get_register(registers, store->value_reg);
            Byte* pval = stack.base - offset;
            int32_t* pint = (int32_t*)pval;
            *pint = value;
          PRINT_INSTRUCTION(printf("STORE_STACK_OFFSET_FROM_REG: %s -> (%d), offset: %d\n", register_namestr(store->value_reg), value, offset );)
        } break;
        case I_LOAD_STACKFRAME_OFFSET: {
            I_LoadStackOffset* load = (I_LoadStackOffset*) &in; 
            int stack_offset = load->address;
            Byte* pval = stack.base -  stack_offset;
            int32_t value = (*(int32_t*)pval); 
            set_register(registers, load->reg, value);
            PRINT_INSTRUCTION(printf("LOAD_STACKFRAME_OFFSET: %s <- val:(%d), offset: %d\n", register_namestr(load->reg), value, stack_offset );)
        } break;
        case I_LOAD_CONST: {
            I_LoadConst* load = (I_LoadConst*) &in; 
            set_register(registers, load->reg, load->intlit);
            PRINT_INSTRUCTION(printf("LOAD_CONST: %s <- %d \n", register_namestr(load->reg), load->intlit);)
        } break;
         case I_LOAD: {
            I_Load* load = (I_Load*) &in; 
            int32_t value = get_heap_value(&heap, get_register(registers, load->addr_reg));
            set_register(registers, load->src_reg, value);
            PRINT_INSTRUCTION(printf("LOAD: %s <- %d, from heap-offset: \n", register_namestr(load->src_reg), value, get_register(registers,load->addr_reg) );)
        } break;
         case I_LOAD_STACKFRAME_OFFSET_FROM_REG: {
            I_LoadStackFromReg* load = (I_LoadStackFromReg*) &in; 
            MemOffset offset = get_register(registers, load->addr_reg);
            Byte* pmem = stack.base - offset;
            int32_t value = *((int32_t*)(pmem));

            set_register(registers, load->src_reg, value);
            PRINT_INSTRUCTION(printf("LOAD_STACKFRAME_OFFSET_FROM_REG: %s <- %d \n", register_namestr(load->src_reg), value );)
         } break;
        case I_ADD: {
            I_Add* add = (I_Add*)&in;
            int x = get_register(registers, add->reg_op_x);
            int y = get_register(registers, add->reg_op_y);
            set_register(registers, add->dest_reg, x + y );
            PRINT_INSTRUCTION(printf("ADD: %s <- %d + %d = %d \n", register_namestr(add->dest_reg), x, y, x+y);)
        } break;
        case I_ADD_REG: {
            I_AddReg* add = (I_AddReg*)&in;
            int x =add->intlit;
            add_to_register(registers, add->reg, x);
            PRINT_INSTRUCTION(printf("ADD_REG: %s <- + %d  \n", register_namestr(add->reg), x);)
        } break;
        case I_SUB: {
            I_Sub* sub = (I_Sub*)&in;
            int x = get_register(registers, sub->reg_op_x);
            int y = get_register(registers, sub->reg_op_y);
            set_register(registers, sub->dest_reg, x - y );
            PRINT_INSTRUCTION(printf("SUB: %s <- %d - %d = %d \n", register_namestr(sub->dest_reg), x, y, x-y);)
        } break;
        case I_SUB_REG: {
            I_SubReg* sub = (I_SubReg*)&in;
            int x =  sub->intlit;
            add_to_register(registers, sub->reg, -x);
            PRINT_INSTRUCTION(printf("SUB_REG: %s <- -%d  \n", register_namestr(sub->reg), x);)
        } break;
        case I_DIV: {
            I_Div* div = (I_Div*)&in;
            int x = get_register(registers, div->reg_op_x);
            int y = get_register(registers, div->reg_op_y);
            set_register(registers, div->dest_reg, x / y );
            PRINT_INSTRUCTION( printf("DIV: %s <- %d / %d = %d \n", register_namestr(div->dest_reg), x, y, x/y);)
        } break;
        case I_MUL: {
            I_Mul* mul = (I_Mul*)&in;
            int x = get_register(registers, mul->reg_op_x);
            int y = get_register(registers, mul->reg_op_y);
            set_register(registers, mul->dest_reg, x * y );
            PRINT_INSTRUCTION(printf("MUL: %s <- %d * %d = %d \n", register_namestr(mul->dest_reg), x, y, x*y);)
        } break;
        case I_AND: {
            I_And* and = (I_And*)&in;
            int x = get_register(registers, and->reg_op_x);
            int y = get_register(registers, and->reg_op_y);
            set_register(registers, and->dest_reg, x && y );
            PRINT_INSTRUCTION( printf("AND: %s <- %d && %d = %d\n", register_namestr(and->dest_reg),x,y,x&&y);)
        } break;
        case I_OR: {
            I_Or* or = (I_Or*)&in;
            int x = get_register(registers, or->reg_op_x);
            int y = get_register(registers, or->reg_op_y);
            set_register(registers, or->dest_reg, x || y );
            PRINT_INSTRUCTION(printf("OR: %s <- %d || %d = %d \n",register_namestr(or->dest_reg),x,y,x||y);)
        } break;
        case I_INCR: {
            I_Increment* incr  = (I_Increment*)& in;
            add_to_register(registers, incr->reg, 1);
            PRINT_INSTRUCTION(printf("INCR: %s\n", register_namestr(incr->reg));)
        } break;
        case I_DECR: {
            I_Decrement* incr  = (I_Decrement*)& in;
            add_to_register(registers, incr->reg, -1);
            PRINT_INSTRUCTION(printf("DECR %s\n", register_namestr(incr->reg));)
        } break;
        case I_CMP_EQ: {
            I_CompareEquals* eq = (I_CompareEquals*)&in;
            int x = get_register(registers, eq->reg_op_x);
            int y = get_register(registers, eq->reg_op_y);
            set_register(registers, eq->dest_reg, x == y);
            set_flag(registers, RFLAG_COMPARE, (x == y));
           PRINT_INSTRUCTION(printf("CMP_EQ, %d == %d (%d) \n", x,y,x==y);)
        } break;
        case I_CMP_LESS: {
            I_CompareLess* eq = (I_CompareLess*)&in;
            int x = get_register(registers, eq->reg_op_x);
            int y = get_register(registers, eq->reg_op_y);
            set_flag(registers, RFLAG_COMPARE, (x < y));
            set_register(registers, eq->dest_reg, x < y);
           PRINT_INSTRUCTION(printf("CMP_LESS, %d < %d (%d) \n", x,y,x<y);)
        } break;
        case I_JMP: {
            I_Jump* jmp = (I_Jump*)&in;
            set_register(registers, RIP, jmp->instruction_nr);
            PRINT_INSTRUCTION(printf("JMP: jumpt to [%d]\n", jmp->instruction_nr);)
        } break;
        case I_JMPEQ: {
            I_JumpEquals* jmp = (I_JumpEquals*)&in;
            if (get_flag(registers, RFLAG_COMPARE) == 0 ) {
                set_register(registers, RIP, jmp->instruction_nr);
                PRINT_INSTRUCTION(printf("JMPEQ: jumpt to [%d]\n", jmp->instruction_nr);)
            }
            else {
            PRINT_INSTRUCTION(printf("JMPEQ: no jump\n");)
            }
        } break; 
        case I_JMPNEQ: {
            I_JumpNeq* jmp = (I_JumpNeq*)&in;
            if (get_flag(registers, RFLAG_COMPARE) == 1) {
                set_register(registers, RIP, jmp->instruction_nr);
                PRINT_INSTRUCTION(printf("JMPNEQ: jumpt to [%d]\n", jmp->instruction_nr);)
            }
            else {
            PRINT_INSTRUCTION(printf("JMPNEQ: no jump\n");)
            }
        } break;
        case I_PUSH: {
            I_Push* psh = (I_Push*)&in;
            int value = get_register(registers, psh->srcReg);
            push(&stack, registers, value);
            PRINT_INSTRUCTION(printf("PUSH: %s -> (%d)\n", register_namestr(psh->srcReg), value);)
        } break;
        case I_PUSH_INT: {
            I_PushInt* psh = (I_PushInt*)&in;
            push(&stack,registers, psh->value);
            PRINT_INSTRUCTION(printf("PUSH_INT: (%d)\n", psh->value);)
        } break;
        case I_POP: {
            I_Pop* ipop = (I_Pop*)&in;
            int32_t val = pop(&stack, registers);
            set_register(registers, ipop->reg, val);
            PRINT_INSTRUCTION(printf("POP: %s <- (%d)\n", register_namestr(ipop->reg), val);)
        } break;
        case I_CALL: {
            I_Call* icall = (I_Call*)&in;
            int32_t entrypoint = get_register(registers, icall->reg);
            push(&stack, registers, current_instruction+1 ); // Resume from the next instruction,
            set_register(registers, RIP, entrypoint+1); // don't ask... 
            PRINT_INSTRUCTION(printf("CALL: jump to: [%d], return to: [%d]\n", entrypoint, current_instruction+1 );)
        } break;
        case I_RETURN: {
            //Read IP from stack
            int32_t next_instruction = pop(&stack, registers);
            set_register(registers,RIP, next_instruction  ); 
            PRINT_INSTRUCTION(printf("RETURN: jump to: [%d]\n", next_instruction  );)
        } break;
        case I_PRINTLN: {
            I_PrintLn* iprint = (I_PrintLn*)&in;
            MemOffset offset = get_register(registers, iprint->reg);
            char* s = heap.base + offset; 
            PRINT_INSTRUCTION(printf("PRINTLN \n");)
            printf("%s\n",s);
        } break;
        case I_PRINTLN_INT: {
            I_PrintLnInt* iprint = (I_PrintLnInt*)&in;
            int32_t val = get_register(registers, iprint->reg);
            PRINT_INSTRUCTION(printf("PRINTLN_INT \n");)
            printf("%d\n",val);
        } break;
        case I_STOP: {
            PRINT_INSTRUCTION(printf("STOP");)
            running = BOOL_FALSE;
        }; break;

        case I_ALLOC: {
            I_Alloc* ialloc = (I_Alloc*)&in;
            set_register(registers, ialloc->dstReg, heap_alloc(&heap, ialloc->size));
            PRINT_INSTRUCTION(printf("ALLOC, size: %d bytes\n",ialloc->size );)
        } break;

        case I_CONCAT_STR_STR: {
            I_ConcatStrStr* istr = (I_ConcatStrStr*)&in;

            char* str_a = heap.base + get_register(registers, istr->arg0_reg);
            char* str_b = heap.base + get_register(registers, istr->arg1_reg);
            size_t concat_size = strlen(str_a) + strlen(str_b);
            MemOffset concat_str_ref = heap_alloc(&heap, concat_size + 1); // Extra byte for null-terminator

            char* new_ref = strcpy(heap.base + concat_str_ref, str_a);
            strcpy(new_ref + strlen(str_a), str_b);
            set_register(registers, istr->dest_reg, concat_str_ref);

            PRINT_INSTRUCTION(printf("CONCAT_STR_STR: %s + %s, new string stored at: %d",str_a, str_b, concat_str_ref  );)
        }break;

        case I_CONCAT_STR_INT: {
            I_ConcatStrInt* istr = (I_ConcatStrInt*)&in;

            char* str_a = heap.base + get_register(registers, istr->arg0_reg);
            int32_t n  = get_register(registers, istr->arg1_reg);
            
            // Convert int into a string:
            char temp_buf[12];
            char* str_b = _itoa(n, temp_buf, 10);

            // Now place on heap and concat strings
            size_t concat_size = strlen(str_a) + strlen(str_b);
            MemOffset concat_str_ref = heap_alloc(&heap, concat_size + 1); // Extra byte for null-terminator

            char* new_ref = strcpy(heap.base + concat_str_ref, str_a);
            strcpy(new_ref + strlen(str_a), str_b);
            set_register(registers, istr->dest_reg, concat_str_ref);
            PRINT_INSTRUCTION(printf("CONCAT_STR_INT: %s + %d, new string stored at: %d", str_a, n, concat_str_ref );)
        }break;

         case I_CONCAT_INT_STR: {
            I_ConcatIntStr* istr = (I_ConcatIntStr*)&in;

            char* str_b = heap.base + get_register(registers, istr->arg1_reg);
            int32_t n  = get_register(registers, istr->arg0_reg);
            
            // Convert int into a string:
            char temp_buf[12];
            char* str_a = _itoa(n, temp_buf, 10);

            // Now place on heap and concat strings
            size_t concat_size = strlen(str_a) + strlen(str_b);
            MemOffset concat_str_ref = heap_alloc(&heap, concat_size + 1); // Extra byte for null-terminator

            char* new_ref = strcpy(heap.base + concat_str_ref, str_a);
            strcpy(new_ref + strlen(str_a), str_b);
            set_register(registers, istr->dest_reg, concat_str_ref);
            PRINT_INSTRUCTION(printf("CONCAT_INT_STR: %d + %s, new string stored at: %d", n, str_b,  concat_str_ref );)
        }break;
 
         case I_NULL_CHECK: {
             I_NullCheck* icheck = (I_NullCheck*)&in;

             MemOffset val =(MemOffset) get_register(registers, icheck->reg); 
             if (val == NULL_CONSTANT) {
                 printf("[FATAL-ERROR] Attempting to dereference null-pointer\n");
                 assert(0);
             }
             PRINT_INSTRUCTION(printf("NULL_CHECK\n");)
         } break;


        default: {
            PRINT_INSTRUCTION(printf("UNKNOWN instruction\n");)
        }   break;
        }
    }

    free(pmemory);
    printf("\nP0VM Terminating, good bye!\n");
    return 0; 
}

void set_flag(Register* registers, RegisterFlagShifts shifts, int flag_value ) {
    registers[RFLAGS].store &= ~(1 << shifts); //Clear flag 
    registers[RFLAGS].store |= (flag_value << shifts); //Set flag 
}

int get_flag(Register* registers, RegisterFlagShifts shifts) {
    return ((registers[RFLAGS].store >> shifts) & 1); 
}

void push(Stack* stack, Register* registers, int32_t value) {
    increment_stack_pointer(stack, registers, (MemOffset)sizeof(value)); //make space
    Byte* p = stack->base - get_register(registers, RSP); //Get pointer to top of stack, stack grows downwards, hence the '-' 
    int32_t* pint = (int32_t*)p; 
    (*pint) = value;
}

int32_t pop(Stack* stack, Register* registers) {
    Byte* p = (stack->base - get_register(registers, RSP)); 
    int32_t val = (*(int32_t*)(p));
    decrement_stack_pointer(stack,registers, (MemOffset)sizeof(val) );
    return val; 
}

void increment_stack_pointer(Stack* stack, Register* registers, MemOffset increment ) {
    int32_t sp = get_register(registers, RSP);
    
    //This is just being overly defensive:
    int64_t signed_offset = (int64_t)sp;
    if ( (signed_offset + increment) > 0xFFFFFFFFLL) {
        printf("[FATAL-ERROR]: Stack overflow! You have consumed the entire memory of the VM :( \n");
        assert(0); //For now, just shut down
    }

    set_register(registers, RSP, sp + increment);
}

void decrement_stack_pointer(Stack* stack, Register* registers, MemOffset decrement) {
    int32_t sp = get_register(registers, RSP);
    
    int64_t signed_offset = (int64_t)sp;
    if ((signed_offset - decrement) <0)  {
        printf("[FATAL-ERROR]: Stack underflow! Trying to pop more elements of the stack than it contains! Check your code-gen! \n");
        assert(0); // Crash 
    }
    set_register(registers, RSP, sp - decrement);
}

void print_stack(Stack* stack, Register* registers) {
    Byte* p = (stack->base - get_register(registers, RSP)); 
    printf("Printing stack\n");
    for (p; p < stack->base; p += sizeof(int32_t)) {
        printf("%d\n", (*(int32_t*)(p)));
    }
}

char* register_namestr (RegisterName rn) {

    switch (rn)
    {
    case RA: return "RA";
    case RB: return "RB";
    case RC: return "RC";
    case RD: return "RD";
    case RE: return "RE";
    case RF: return "RF";
    case RBP: return "RBP";
    case RSP: return "RSP";
    case RIP: return "RIP";
    case RFLAGS: return "RFLAGS";
    default: return "UNKOWN";
    }
}


void print_registers(Register* r) {
    for (int i = 0; i < NUM_GENERAL_REGISTERS; i++) {
        printf("%s: [%d]\n", register_namestr(i), r[i].store);
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
    assert(rn >= 0 && rn < NUM_REGISTERS); 
    registers[rn].store += val; 
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

void store_heap(Heap* heap,  MemOffset offset, int val) {
    if ( offset  == NULL_CONSTANT) {
        printf("[FATAL-ERROR] Null-pointer exception. Tried to store at NULL-address\n");
        assert(0);
    }
    Byte* pmem = heap->base + offset;
    int32_t* pval = (int32_t*)pmem;
    *pval = val; 
}

int32_t get_heap_value(Heap* heap, MemOffset offset) {
    if (offset  == NULL_CONSTANT) {
        printf("[FATAL-ERROR] Null-pointer exception. Tried to load from NULL-address\n");
        assert(0);
    }
    Byte* pmem = heap->base + offset;
    int32_t* pval = (int32_t*)pmem;
    return (*pval); 
}


void print_int_at_memory_offset(Byte* membase, size_t memsize, MemOffset offset) {
    assert(offset+4 < memsize );
    int* mem = (int*)(membase + offset);
    printf("Value at memory offset %u is %d\n", offset, (*mem)); 
}

MemOffset heap_alloc(Heap* heap, uint32_t size) {
    assert( ((uint64_t)(heap->free + size)) < 0xFFFFFFFFULL);
    MemOffset offset = heap->free;
    heap->free += size;
    return offset; 
}

RetCode read_program_from_file(char* filename, _OUT_PARAM char** pmem, _OUT_PARAM size_t* program_size,_OUT_PARAM size_t* num_instructions  ) {
    FILE* file = fopen(filename , "rb");
    if (file) {
        size_t filesize = 0;
        fseek(file, 0L, SEEK_END);
        filesize = ftell(file);
        fseek(file, 0L, SEEK_SET);
      
        RetCode ret = RETCODE_ERROR;
        fread(*pmem, sizeof(char), filesize, file);

        int32_t* n_instructions = (int32_t*)*pmem;
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

