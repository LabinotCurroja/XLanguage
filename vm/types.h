
#ifndef VMX_TYPES
#define VMX_TYPES 


#define i64     unsigned long long
#define i32     uint32_t 

#define f64     double
#define f32     float 


#define INS_MASK 0xFF00000000000000
#define TYP_MASK 0x00FF000000000000
#define SRC_MASK 0x0000FFFFFFFFFFFF

#define ZEF_MASK 0xF000000000000000
#define NZF_MASK 0x0F00000000000000
#define GZF_MASK 0x00F0000000000000


enum types 
{
    EMPTY,
    INSTRUCTION,
    INTEGER,
    STRING,
    BOOL,
    FLOAT,
    CHAR,
    STRUCT,
    BUTTERFLY,
    BUTTERFLY_VALUE,
    ADDR_POINTER,
    RAW_VALUE
};

enum sign
{
    POSITIVE,
    NEGATIVE
};

typedef enum { false, true } bool;


#define true  1
#define false 0





#endif