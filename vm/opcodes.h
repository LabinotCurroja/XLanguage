#ifndef VMX_OPCODES
#define VMX_OPCODES


enum instructions 
{
	NOP,
    LEA,                         		// load immediate addr content to top of stack - used for loading variables from memory
    CLF,                         		// clear flags 
	CMP,   CMPI,                 		// compare
	STI,   LDI,                  		// move info between mem and registers 
	LII,   LIF,                  		// load immediate int
	PUSH,  PUSH_POINTER, PUSH_BOOL,		// push different types
	POP,                  		 		// push and pop to the stack 
	INC,   DEC,                  		// increment and decrement 
	ADD,   SUB, MUL, DIV, MOD,   		// arithmetic 
	JLZ,   JGZ, JEZ, JNZ, JMP,   		// jmp less zero, greater than zero, equal zero, not zero, unconditional jmp
	SHL,   SHR,                  		// shift left & right
	BAND,  BOR, BNOT, BXOR,      		// bitwise binary operations 
	LAND,  LOR, LNOT,            		// logical binary operations 
	HLT,                         		// halting 
    CALL, EXIT,                  		// call and exit functions
    WRITE, READ,                 		// write and load variables from data segment 
    INT,						 		// CPU interrupts 
	PRINT, 					     		// high level print function
	FOPEN, FCLOSE,   		     		// system interrupts,
	RET,
	NUM_INSTRUCTIONS
};



#endif