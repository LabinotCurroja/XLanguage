

#ifndef VMX_OBJECTS
#define VMX_OBJECTS 

#include "types.h"
#include "opcodes.h"
#include "opcodes.h"
#include <stdlib.h>

i64 VMX_mk_object(i64 type, i64 op, i64 src) 
{ 
	i64    instr     = type << 56; 
	       instr     = instr | ( (i64) op << 48);
	       instr     = instr | (src & SRC_MASK);
	return instr;
}

i64 VMX_mk_instruction(i64 op, i64 src) 
{ 
    return VMX_mk_object(INSTRUCTION, op, src);
}

i64 VMX_mk_butterfly(i64 bftype, i64 length)
{
    return VMX_mk_object(BUTTERFLY, bftype, length);
}

i64 VMX_mk_butterfly_value(i64 bftype, i64 value)
{
    return VMX_mk_object(BUTTERFLY_VALUE, bftype, value);
}

#endif