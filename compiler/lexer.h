

#ifndef VMX_LEXER
#define VMX_LEXER

#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include "symbol_table.h"
#include "util.h"
#include "../vm/vmx.h"


#define MAX_IDENTIFIER_NAME_LEN    64
#define VMX_STANDARD_STRING_LENGTH 256
#define MAX_LEXEMES                1024

struct Token ctoken;


FILE *file;


char *read_file(char *fileName) 
{
    FILE *file = fopen(fileName, "r");
    char *code;
    size_t n = 0;
    int c;

    if (file == NULL) return NULL; //could not open file
    fseek(file, 0, SEEK_END);
    long f_size = ftell(file);
    fseek(file, 0, SEEK_SET);
    code = malloc(f_size);

    while ((c = fgetc(file)) != EOF) {
        code[n++] = (char)c;
    }

    code[n] = '\0';        

    return code;
}

void open_file(const char *filename)
{
	file = fopen(filename, "rb");
	if (NULL == file) 
    {
		VMX_LEX_FATAL("xcc input: File doesn't exist!");
	}
}

FILE *get_file()
{
	return file;
}

void close_file()
{
	fclose(file);
}

int add_butterfly_float(double value)
{
    char * decimal_string;
    gcvt(value, 8, decimal_string);


    unsigned int len       = (unsigned int)strlen(decimal_string);

    i64 float_bf          = VMX_mk_butterfly(FLOAT,len); 
    int bf_offset         = add_butterfly_symbol(float_bf);

    for(size_t i=0; i < len; i++)
    {
        i64 bfv = VMX_mk_butterfly_value(FLOAT, decimal_string[i] ); 
        i64 len = add_butterfly_symbol(bfv);
    }

    return bf_offset;
}

int add_butterfly_string(char * string)
{
    //printf("adding butterfly symbol string %s\n", string);

    unsigned int len       = (unsigned int)strlen(string);
    i64 string_bf          = VMX_mk_butterfly(STRING,len); // butterfly for string object with length and type of string
    int bf_offset          = add_butterfly_symbol(string_bf);

    //printf("index> %d\n",bf_offset);

    for(size_t i=0; i < len; i++)
    {
        //printf("CHAR : %c\n", string[i]);
        i64 bfv = VMX_mk_butterfly_value(CHAR, string[i] ); // offsets for each character 
        i64 len = add_butterfly_symbol(bfv);
    }
    return bf_offset;
}





struct Token
{
    int    type;   /* type of the actual token, int_literal | plus | etc*/
    i32    attr;   /* the value of the token : int_literal with a value of 42 */
    char * literal;
};

struct Token lexemes[MAX_LEXEMES];
int total_current_tokens = 0;

struct Token get_last_token()
{
    return lexemes[total_current_tokens-1];
}



enum types_lex 
{
    int_type,
    float_type,
    str_type,
    bool_type,
    bin_add,
    bin_sub,
    bin_mul,
    bin_div,
    int_literal,
    float_literal,
    str_literal,
    bool_literal,
    identifier,
    assign_equal,
    semi_colon,
    code_block,
    assign_int,
    assign_float,
    assign_str,
    assign_bool,
    symbol,
    print,
    lbr,
    rbr,
    lbrace,
    rbrace,
    keyword_if,
    keyword_else,
    end_program,
    comment,
    test
};


static const char *types_string[] = 
{
    "int_type",
    "float_type",
    "str_type",
    "bool_type",
    "bin_add",
    "bin_sub",
    "bin_mul",
    "bin_div",
    "int_literal",
    "float_literal",
    "str_literal",
    "bool_literal",
    "identifier",
    "assign_equal",
    "semi_colon",
    "code_block",
    "assign_int",
    "assign_float",
    "assign_str",
    "assign_bool",
    "symbol",
    "print",
    "lbr",
    "rbr",
    "lbrace",
    "rbrace",
    "keyword_if",
    "keyword_else",
    "end_program",
    "comment",
    "test"
};

void print_token(struct Token t)
{
    printf("Token structure type {%s} attribute {%x}\n", types_string[t.type], t.attr);
}



struct Token lex()
{
    int ch = 0;
    struct Token tok;

    eat:
    switch (ch = fgetc(get_file())) 
    {
        
        case ' ': case '\n':                                                  goto eat;
        case EOF: tok.type = end_program;  tok.attr = 0;                         break;
        case '+': tok.type = bin_add;      tok.attr = 0;                         break;
        case '-': tok.type = bin_sub;      tok.attr = 0;                         break;
        case '*': tok.type = bin_mul;      tok.attr = 0;                         break;
        case '/': tok.type = bin_div;      tok.attr = 0;                         break;       
        case '=': tok.type = assign_equal; tok.attr = 0;                         break;
        case ';': tok.type = semi_colon;   tok.attr = 0;                         break;
        case '(': tok.type = lbr;          tok.attr = 0;                         break;
        case ')': tok.type = rbr;          tok.attr = 0;                         break;
        case '{': tok.type = lbrace;       tok.attr = 0;                         break;
        case '}': tok.type = rbrace;       tok.attr = 0;                         break;
        case '#':;
            // single line comments are handled with a # as // are not pleasant to look at 
            tok.type = comment;
            tok.attr = 0;
            while (ch != '\n')
            {
                ch = fgetc(get_file());
            }
            break;

        case '"':;
            char *str_lit  = safe_malloc(VMX_STANDARD_STRING_LENGTH);
            int len        = 0;
            str_lit[len++] = ch = fgetc(get_file());
            
            do 
            {
                if (VMX_STANDARD_STRING_LENGTH == len)
                {
                    VMX_LEX_FATAL("Lexer: String name is too long. MAX length: 256");
                }
                str_lit[len++] = (ch = fgetc(get_file()));
            } 
            while (ch != '"'); 
            str_lit[len - 1] = '\0';


            //printf("<%s>\n", str_lit);
            tok.type = str_literal;
            tok.attr = add_butterfly_string(str_lit); // the reference where it is at in memory 
            break;

        default: 

            // keyword or identifier
            if (isalpha(ch) || (char)ch == '_') 
            {
                char *id_name  = safe_malloc(MAX_IDENTIFIER_NAME_LEN);
                int len        = 0;
                id_name[len++] = ch;

                do 
                {
                    if (MAX_IDENTIFIER_NAME_LEN == len)
                    {
                        VMX_LEX_FATAL("Lexer: string length is too long");
                    }
                    id_name[len++] = (ch = fgetc(get_file()));
                } 
                while (isalpha(ch) || isdigit(ch) || (char)ch == '_'); 
                id_name[len - 1] = '\0';

                // reserved keywords checking - this could be done better, but whatever, for now its fine 
                if(strcmp(id_name, "int") == 0) 
                {
                    tok.type = int_type;
                    tok.attr = 0;
                }
                else if(strcmp(id_name, "float") == 0)
                {
                    tok.type = float_type;
                    tok.attr = 0;
                }
                else if(strcmp(id_name, "string") == 0)
                {
                    tok.type = str_type;
                    tok.attr = 0;
                }
                else if(strcmp(id_name, "bool") == 0) 
                {
                    tok.type = bool_type;
                    tok.attr = 0;
                }
                else if(strcmp(id_name, "print") == 0) 
                {
                    tok.type = print;
                    tok.attr = 0;
                }
                else if(strcmp(id_name, "if") == 0) 
                {
                    tok.type = keyword_if;
                    tok.attr = 0;
                }
                else if(strcmp(id_name, "else") == 0) 
                {
                    tok.type = keyword_else;
                    tok.attr = 0;
                }
                else if(strcmp(id_name, "true") == 0) 
                {
                    tok.type = bool_literal;
                    tok.attr = 1;
                }    
                else if(strcmp(id_name, "false") == 0) 
                {
                    tok.type = bool_literal;
                    tok.attr = 0;
                }             
                else
                {
                    tok.type = identifier;
                    tok.attr = add_symbol(id_name); 
                }


            // NUM
            } 
            else if (isdigit(ch)) 
            {


                float val = ch - '0';
                char  * literal = malloc(10);

                strcat(literal, (char *)&ch);

                while (isdigit(ch = fgetc(get_file())))
                {
                    val = val * 10 + (ch - '0');
                    strcat(literal, (char *)&ch);
                }

                // now check if the next token is a '.' -> if it is, we are trying to describe a float
                if((int)ch == '.')
                {
                    strcat(literal, (char *)&ch);

                    while (isdigit(ch = fgetc(get_file())))
                    {
                        strcat(literal, (char *)&ch);
                    }

                    double f_l = strtod(literal, NULL);
                    float  flf = (float) f_l;

                    tok.type = float_literal;
                    tok.attr = *(i32 *)(&flf);
                }
                else 
                {
                    double f_l = strtod(literal, NULL);
                    float  flf = (float) f_l;

                    tok.type = float_literal;
                    tok.attr = *(i32 *)(&flf);
                }


            
            
            } 

            else 
            {
                VMX_LEX_FATAL("Lexer: Unexpected symbol\n");
                printf("-> %c\n", ch);
            }

            ungetc(ch, get_file());
    }
    lexemes[total_current_tokens] = tok;
    total_current_tokens++;
    return tok;
}

void initialize_lexer()
{
    ctoken = lex();
}






#endif
