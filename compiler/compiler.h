
/**
 * 
 * NOTES 
 * compiler butterflies 
 * 
*/



#ifndef VMX_COMPILER
#define VMX_COMPILER 

#include "parser.h"
#include "../vm/types.h"



void compile(struct ASTNode * root); 



i64 * XCC_PROGRAM;

void init_compiler()
{
    XCC_PROGRAM = create_program();
    prepare_program(XCC_PROGRAM);
}

void write_symtable()
{
    for(int i=0; i < table_size; i++)
    {
        //printf("writing ::: \"%s\" 0x%llx\n", symbol_table[i]->name, (i64)symbol_table[i]->val);
        i64 bf = symbol_table[i]->val;
        write_data_segment_butterfly(XCC_PROGRAM, bf);
    }
}

void compilation_end()
{
    write_symtable();
    finalize_program(XCC_PROGRAM);
    write_program_disc(XCC_PROGRAM, "tprogram.xc");
}

void gen(i64 opcode, i64 src)
{
     i64 inst                = VMX_mk_instruction(opcode, src);
     write_instruction(XCC_PROGRAM, inst);
}

void compile_both(struct ASTNode * n)
{
    compile(n->left); 
    compile(n->right);
}

void compile(struct ASTNode * root)
{
        if (NULL == root)
        {
            return;
        }

        switch (root->type) 
        {
            case code_block: 
                    compile_both(root);                                             
                    break; 
            case assign_int: 
                    compile(root->right); 
                    gen(WRITE, root->left->val);               
                    break; 
            case assign_float: 
                    compile(root->right); 
                    gen(WRITE, root->left->val);               
                    break; 
            case assign_str:
                    compile(root->right); 
                    gen(WRITE, root->left->val);    
                    break;
            case assign_bool: 
                    compile(root->right); 
                    gen(WRITE, root->left->val);               
                    break;                     
            case identifier: 
                    gen(READ,root->val);                      
                    break;
            case keyword_if:
                    gen(CMP, root->val);
                    break; 
            case str_literal:;
                    long long dx = VMX_mk_object(ADDR_POINTER,0, VMX_data_segment+root->val);
                    gen(PUSH_POINTER, dx ); 
                    break;
            case int_literal:;
                    long long i_literal = root->val;
                    gen(PUSH, i_literal);                                          
                    break; 
            case bool_literal:;
                    long long bool_object = VMX_mk_object(RAW_VALUE, BOOL, root->val);
                    gen(PUSH_BOOL, bool_object);                                          
                    break; 
            case float_literal:;
                    long long float_object = VMX_mk_object(RAW_VALUE, FLOAT, root->val);
                    gen(PUSH, float_object);                                          
                    break; 
            case bin_add: 
                    compile_both(root);  
                    gen(ADD, 0);                                  
                    break;
            case bin_sub: 
                    compile_both(root);  
                    gen(SUB, 0);                                  
                    break;
            case bin_mul:
                    compile_both(root);  
                    gen(MUL, 0);                                  
                    break;
            case bin_div: 
                    compile_both(root);  
                    gen(DIV, 0);                                  
                    break;
            case end_program: 
                    compile_both(root);  
                    gen(RET, 0);                                  
                    break;
            case print:
                    compile(root->left); 
                    gen(PRINT,0);

            default:
                    //printf("COMPILATION ERROR\n");
                    //printASTNode(root,0);
                    break;
        }
}




#endif
