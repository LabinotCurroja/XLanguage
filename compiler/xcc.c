#include "compiler.h"


/**
 * compilation of a program :
 * each program always starts at offset 8 of memory pointer. 
 * the first 8 i64s are the header of the program with things such as the 
 * - version
 * - compilation time 
 * - symbols 
 */

int main(int argc, char* argv[])
{
    if(argc < 2)
    {
        printf("xcc fatal error: no input files.\nCompilation terminated.\n");
        exit(0);
    }


    char * fn = argv[1];
    open_file(fn);


    initialize_lexer();
    
    init_compiler();
    
    while(ctoken.type != end_program)
    {
        struct ASTNode * root = parse();

        if(DEBUG)
        {
            printASTNode(root, 0);
        }

        compile(root);
        free(root);

    }
    
    compilation_end();
    
    close_file();
    return 0;
}