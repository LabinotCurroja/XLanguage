

#ifndef _VMH
#define _VMH

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <string.h>

#include "opcodes.h"
#include "types.h"


#define DEBUG           true
#define VMX_MAXINT      281474976710656
#define NONE            0


/* important that is becomes more dynamic when system is stable */
#define VMX_default_program_mem    4096 * 2
#define VMX_code_segment           0
#define VMX_data_segment           4096 / 2                /* important that this is READ only */
#define VMX_stack_segment          4096









#define VMX_log(fmt, ...) \
            do { if (DEBUG) fprintf(stderr, fmt, __VA_ARGS__); } while (0)

#define VMX_log_line(fmt) \
            do { if (DEBUG) fprintf(stderr, fmt); } while (0)


/**
 * the masks for the system.
 * Each instruction can either be a primitive instruction, a butterfly pointing to data, or a raw value such as int, float, bool, string, struct etc 
 * this allows for a system where each instruction is suited for the need. 
 * 
 * 
 * @primitive   instruction such as ADD, SUB, PUSH, POP etc
 * @butterfly   which has a length and an address which points to data, strings are implemented as a butterfly as an example, structs (objects) and so on
 * @raw         symbolizes a raw value such as an int, a bool, a float - this is how floats are implemented 
 * 
*/







enum status_codes 
{
    SUCCESS,
    ERROR,
    PANIC,
    OVERFLOW_STACK,
    OVERFLOW_MEMORY,
    ASSERT_ERROR,
    UNKNOWN_OPCODE,
    BINOP_TYPE_CONFUSION
};




typedef struct 
{
    i64   version;
    i64 * memory;
}
program; /* a higher level abstraction of a program. Provides data such as compilation time, system, author and so on */




typedef struct
{
    i64      rsp; 
    i64      rip; 
    i64      rbp; 

    i64      flags;
    i64 *    memory; 
    i64      memory_size;
    i64      data_segment;

    i64      busA;
    i64      busB;

    i64      enabled;
    i64      status;
    i64      debug;
    i64      cycles;
    double   t_execution;
    double   t_begin;
    double   t_end;
}
VM;


VM *VMX_create(i64 *memory, i64 mem_size) /* in the future, create dynamic sizing for memory, stack, data etc, cant be fixed for max efficiency */
{
	VM *c           = malloc(sizeof(VM));
    c->rip          = -1;
	c->rsp          = mem_size - 1;
    c->rbp          = mem_size - 1;
    c->flags        = 0;
    c->memory       = memory;
	c->memory_size  = mem_size;
    c->data_segment = VMX_data_segment;
    c->enabled      = 1;
    c->status       = SUCCESS;
    c->debug        = DEBUG;
    c->cycles       = 0;
    c->t_execution  = 0;
    c->t_begin      = 0;
    c->t_end        = 0;
	return c;
}


/**
 * all of the CPU specific instructions to operate the actual VM
*/


void VMX_PANIC(VM *vm, i64 code)
{
    vm->enabled = 0;
    vm->status  = code;
}


void VMX_THROW_ERROR(VM *vm, i64 code, char * message)
{
    printf("Error: %s\n", message);
    VMX_PANIC(vm, code);
}

void VMX_ASSERT(VM *vm, bool condition)
{
    if(!condition)
    {
        VMX_PANIC(vm, ASSERT_ERROR);
        VMX_log_line("[VMX] ASSERT_ERROR\n");
    }
}

void VMX_tos(VM *vm) /* top of stack */
{
    if(DEBUG)
    {
        printf("RSP %lld value : %lld\n", vm->rsp, vm->memory[vm->rsp]);
    }
}


void VMX_clear_flags(VM *c) 
{
	c->flags = 0;
}

void VMX_set_flags(VM *c, i64 a, i64 b) 
{
    i64 res = a - b;
    i64 z,lz,gz;

    z  = (res == 0);
	lz = (res < 0);
	gz = (res > 0);

    i64 flg =  z  << 60;
        flg | (lz << 56);
        flg | (gz << 52);

	c->flags = flg; 
}

void secure_boundaries_(VM *vm)
{
    /* make sure instruction pointer is only in code segment */
    /* make sure a READ instruction is always in the data segment */
    /* make sure stackpointer is always in stack space */
    /* SHOULD ALLOW FOR GOOD CONTROL */
    if(vm->rip >= VMX_data_segment)
    {
        printf("RIP is in DS\n");
    }
    if(vm->rip >= VMX_stack_segment)
    {
        printf("RIP is in STACK\n");
    }

    if(vm->rip >= vm->memory_size || vm->rip < 0)
    {
        VMX_log("[VMX] Detected memory overflow - {RIP} 0x%llx {MEMORY_SIZE} 0x%llx\n", vm->rip, vm->memory_size);
        VMX_log_line("[VMX] EXIT()\n");
        VMX_PANIC(vm, OVERFLOW_MEMORY);
    }

}

double decode_float(VM *vm, i64 pointer_ds)
{

    i64 butterfly = vm->memory[pointer_ds];

    i64 length    = butterfly  & SRC_MASK;

    char * float_string = malloc(length);

    for(int i=0; i < length; i++)
    {
        i64 butterfly_value = vm->memory[pointer_ds+1+i];
        i64 bf_src_char     = butterfly_value  & SRC_MASK;

        strncat(float_string, (char *)&bf_src_char, 1);
    }

    float_string[length+1] = '\0';

    double real_value = atof(float_string);

    return real_value;
}



/**
 * memory layout of the VM : 
 * T = text .code 
 * D = variables and symb table 
 * S = stack 
 * P = program info 
 * 
 * [PP----TTTTTTTTTTTTTTTT----DDDDDDDDDD--------SSSSSSSSSSSSSSSSSS]
 * The stack grows downwards <-- 
 * always make sure to check bounds between different segments, they can never touch
*/


i64 _mk_object(i64 type, i64 op, i64 src) 
{ 
	i64    instr     = type << 56; 
	       instr     = instr | ( (i64) op << 48);
	       instr     = instr | (src & SRC_MASK);
    //printf("instruction made: %llx\n", instr);
	return instr;
}




int VMX_run_program(VM *vm)
{
    vm->rip = 0;
    while (vm->enabled)
    {
        secure_boundaries_(vm);

        i64 type =  (i64)vm->memory[vm->rip] >> 56;
        i64 op   = ((i64)vm->memory[vm->rip] & TYP_MASK) >> 48;
        i64 src  =  (i64)vm->memory[vm->rip] & SRC_MASK;


        /* checks if it is an instruction */
        if(type == (i64)INSTRUCTION)
        {
            VMX_log("\033[1;33m-FULL INSTRUCTION-\033[0m      [0x%llx]\n", vm->memory[vm->rip]);

            switch (op) 
            {
                case NOP: 
                    VMX_log_line("NOP\n");
                    break;

                case HLT: 
                    VMX_log_line("\033[1;33mHLT\033[0m\n");
                    vm->enabled = 0;
                    break;

                case PUSH: // push raw value - float     
                    VMX_log("\033[1;33mPUSH\033[0m      [0x%llx]\n", (i64)0);  
                    i64 literal_f = _mk_object(RAW_VALUE, FLOAT, src);     
                    
                    vm->rsp--; 
                    vm->memory[vm->rsp] = literal_f;   
                    break;
                case PUSH_BOOL: // push raw value - boolean     
                    VMX_log("\033[1;33mPUSH_BOOL\033[0m      [0x%llx]\n", (i64)0);  
                    
                    i64 literal_b = _mk_object(RAW_VALUE, BOOL, src);     
                    vm->rsp--; 
                    vm->memory[vm->rsp] = literal_b;   
                    break;
                case PUSH_POINTER:      
                    VMX_log("\033[1;33mPUSH_POINTER\033[0m      [0x%llx]\n", (i64)0);   

                    i64 pointer_object     = vm->memory[src];
                    i64 pointer_object_typ = pointer_object >> 56;
                    /* now we know what kind of pointer it is. Boolean, string, object, array etc. */

                    vm->rsp--;
                    vm->memory[vm->rsp] = _mk_object(ADDR_POINTER,0, src);
                    break;

                case WRITE: 
                    // WRITES WHATS ON TOP OF THE STACK
                    VMX_log("\033[1;33mWRITE \033[0m     [0x%llx]\n", VMX_data_segment+src);
                    i64 ds_object = _mk_object(RAW_VALUE, 0, vm->memory[vm->rsp]);
                    vm->memory[VMX_data_segment+src] = vm->memory[vm->rsp];
                    break;

                case READ:
                    /* check whether we are reading an addr pointer or an INTEGER value */
                    VMX_log("\033[1;33mREAD \033[0m     [0x%llx]\n", VMX_data_segment+src);

                    vm->rsp--;
                    vm->memory[vm->rsp] = vm->memory[VMX_data_segment+src];
                    break;

                case ADD:; // addition only happens on raw values for now 
                    VMX_log("\033[1;33mADD\033[0m       (VALUE 0x%llx ) \n", (i64)0);

                    
                    i64 left  = vm->memory[vm->rsp+1];
                    i64 right = vm->memory[vm->rsp];
                    
                    i64 l_typ =  left  >> 56;
                    i64 r_typ =  right >> 56;

                    // for now, we only support raw values as floats
                    if(l_typ != RAW_VALUE || r_typ != RAW_VALUE)
                    {
                        VMX_THROW_ERROR(vm, BINOP_TYPE_CONFUSION, "[ERROR] Trying to add 2 different types");
                        break;
                    }

                    i64 l_val             =  left  & SRC_MASK;
                    i64 r_val             =  right & SRC_MASK;

                    f32 l_float           = *(float *) &l_val;
                    f32 r_float           = *(float *) &r_val;

                    f32 added = l_float+r_float;

                    i32 fti         = *(i32 *)(&added);
                    i64 added_value       = _mk_object(RAW_VALUE, FLOAT, fti);

                    vm->rsp--;
                    vm->memory[vm->rsp]   = added_value;
                    
                    break; 

            case SUB:
            {
                    VMX_log("\033[1;33mSUB\033[0m       (VALUE 0x%llx ) \n", (i64)0);

                    i64 left  = vm->memory[vm->rsp+1];
                    i64 right = vm->memory[vm->rsp];
                    
                    i64 l_typ =  left  >> 56;
                    i64 r_typ =  right >> 56;

                    if(l_typ != RAW_VALUE || r_typ != RAW_VALUE)
                    {
                        VMX_THROW_ERROR(vm, BINOP_TYPE_CONFUSION, "[ERROR] Trying to add 2 different types");
                        break;
                    }

                    i64 l_val             =  left  & SRC_MASK;
                    i64 r_val             =  right & SRC_MASK;

                    f32 l_float           = *(float *) &l_val;
                    f32 r_float           = *(float *) &r_val;

                    f32 added = l_float-r_float;

                    i32 fti         = *(i32 *)(&added);
                    i64 added_value       = _mk_object(RAW_VALUE, FLOAT, fti);

                    vm->rsp--;
                    vm->memory[vm->rsp]   = added_value;
                    
                    break;          
            }
            case MUL:
            {
                    VMX_log("\033[1;33mSUB\033[0m       (VALUE 0x%llx ) \n", (i64)0);

                    i64 left  = vm->memory[vm->rsp+1];
                    i64 right = vm->memory[vm->rsp];
                    
                    i64 l_typ =  left  >> 56;
                    i64 r_typ =  right >> 56;

                    if(l_typ != RAW_VALUE || r_typ != RAW_VALUE)
                    {
                        VMX_THROW_ERROR(vm, BINOP_TYPE_CONFUSION, "[ERROR] Trying to add 2 different types");
                        break;
                    }

                    i64 l_val             =  left  & SRC_MASK;
                    i64 r_val             =  right & SRC_MASK;

                    f32 l_float           = *(float *) &l_val;
                    f32 r_float           = *(float *) &r_val;

                    f32 added = l_float*r_float;

                    i32 fti         = *(i32 *)(&added);
                    i64 added_value       = _mk_object(RAW_VALUE, FLOAT, fti);

                    vm->rsp--;
                    vm->memory[vm->rsp]   = added_value;
                    
                    break;              
            }  
            case DIV:
            {
                    VMX_log("\033[1;33mSUB\033[0m       (VALUE 0x%llx ) \n", (i64)0);

                    i64 left  = vm->memory[vm->rsp+1];
                    i64 right = vm->memory[vm->rsp];
                    
                    i64 l_typ =  left  >> 56;
                    i64 r_typ =  right >> 56;

                    if(l_typ != RAW_VALUE || r_typ != RAW_VALUE)
                    {
                        VMX_THROW_ERROR(vm, BINOP_TYPE_CONFUSION, "[ERROR] Trying to add 2 different types");
                        break;
                    }

                    i64 l_val             =  left  & SRC_MASK;
                    i64 r_val             =  right & SRC_MASK;

                    f32 l_float           = *(float *) &l_val;
                    f32 r_float           = *(float *) &r_val;

                    f32 added = l_float/r_float;

                    i32 fti         = *(i32 *)(&added);
                    i64 added_value       = _mk_object(RAW_VALUE, FLOAT, fti);

                    vm->rsp--;
                    vm->memory[vm->rsp]   = added_value;
                    
                    break;              
            }  


                case CMP:
                    VMX_log("\033[1;33mCMP\033[0m       ( %llx ) \n", vm->memory[VMX_data_segment+src]);
                    i64 cmp_variable = vm->memory[vm->rsp];
                    break;

                /* prints whats on top of the stack */
                case PRINT:;
                    VMX_log("\033[1;33mPRINT \033[0m     [0x%llx]\n", src);

                    i64 object     = vm->memory[vm->rsp];
                    i64 object_typ = object >> 56;
                    i64 object_src = object & SRC_MASK;
                    i64 origin_src = object_src;

                    if(object_typ == RAW_VALUE)
                    {
                        i64 value_type   = (object & TYP_MASK) >> 48;

                        if(value_type == FLOAT)
                        {
                            float f_val = *(float *) &object_src;
                            printf("%f\n", f_val);
                        }
                        else if(value_type == BOOL) 
                        {
                            if(object_src == 1)
                            {
                                printf("true\n");
                            }
                            else 
                            {
                                printf("false\n");
                            }
                        }
                        else if(value_type == INTEGER)
                        {
                            printf("%lld\n", object_src);
                        }
                    }
                    else if(object_typ == ADDR_POINTER)
                    {
                        object     = vm->memory[object_src];
                        object_typ = object >> 56;
                        object_src = object & SRC_MASK;

                        if(object_typ == BUTTERFLY)
                        {
                            i64 bf_length = object_src;
                            i64 bf_type   = (object & TYP_MASK) >> 48;
                            if(bf_type == STRING)
                            {
                                    for(int j=0; j < bf_length; j++)
                                    {
                                        i64 bfv        = vm->memory[origin_src+1+j];
                                        i64 bfv_val    = bfv & SRC_MASK;
                                        printf("%c", (char)bfv_val);
                                    }
                                    printf("\n");                                
                            }
                        }
                    }
                    else 
                    {
                        printf("Trying to print unknown value\n");
                        printf("object to print: %llx\n", object);
                    }

                    break; 

                default:
                    VMX_log("[PANIC] unknown instruction opcode %llx\n", vm->memory[vm->rip]);
                    VMX_PANIC(vm, UNKNOWN_OPCODE);
                    break;

            }
        }



        vm->rip++;
    }
    return vm->status;

}




#endif