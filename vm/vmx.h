#ifndef VMX_LIBRARY
#define VMX_LIBRARY

#include "vm.h"
#include "object.h"
#include "types.h"
#include <string.h>



/**
 * functions for the VMX such as writing to the data segment, reading from it and so on
 * this header is used by the compiler to generate an actual binary for the VM to run. 
 * it can create a program and fill the data segment, text segment and so on
 * especially useful for objects as strings, structures, array and so on
*/


/* starts at the top of data segments and moves to higher addr */
i64 dsi  = VMX_data_segment; 
i64 isi  = 0;                 /* current instruction index */


i64 * create_program()
{
    i64 *  program           = malloc( VMX_default_program_mem * sizeof(i64) );
    return program;
}

void prepare_program(i64 * program)
{
    /* insert VMX program headers / date, compile time and so on */
}

void write_program_disc(i64 * program, const char * name)
{
    FILE *f = fopen(name, "wb");
    //printf("--- %lld\n", isi);
    fwrite(program, sizeof(i64), VMX_default_program_mem, f);
    fclose(f);
}

i64 * read_program_disc(const char * name)
{
    i64 * program = create_program();
    FILE *f       = fopen(name, "rb");
    if(f== NULL)
    {
        printf("[XCC] input program not found!\n");
        exit(0);
    }

    while (1) 
    {
        fread(program, sizeof(i64) ,VMX_default_program_mem, f); // has to be dynamic of course depending on size of bytes / 8
        if(feof(f)) 
        {
            break;
        }
    }
    fclose(f);
    
    return program;
}

void close_program(i64 * program)
{
    free(program);
}



void write_instruction(i64 *program, i64 instruction)
{
    program[isi] = instruction;
    isi++;
}

void write_data_segment_butterfly(i64 *program, i64 bf)
{
    //printf("writing to data segment : %lld", dsi);
    program[dsi] = bf;
    dsi++;
}

void finalize_program(i64 * program)
{
    i64 ins = VMX_mk_instruction(HLT,0);
    write_instruction(program, ins);
}









/**
 * DEBUG FUNCTIONALITY 
 * @TODO: add ability to see machine running like really visualize it
*/

void __DEBUG__SHOW_STACK(VM * vm, int n)
{
    printf("[STACK::RSP::STACK_POINTER::0x%llx]  =@VALUE %llx \n", vm->rsp, vm->memory[vm->rsp]);
    for(int i=vm->memory_size-1; i > vm->memory_size-16; i--)
    {
        printf("0x%llx      0x%llx\n", (i64)i,  vm->memory[i]);
    }
}

void __DEBUG__SHOW_DS(VM * vm)
{
    printf("[DATA SEGMENT]\n");
    for(int i=0; i < 16; i++)
    {
        printf("0x%llx 0x%llx\n",(i64)VMX_data_segment+i, vm->memory[VMX_data_segment+i]);
    }
}


#endif