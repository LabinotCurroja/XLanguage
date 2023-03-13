

#ifndef VMX_PARSER
#define VMX_PARSER
#include "lexer.h"

static struct ASTNode * term();
static struct ASTNode * factor();
static struct ASTNode * expr();



struct ASTNode
{
    int type;
    i32 val;
    struct ASTNode * left;
    struct ASTNode * right;
};

void printASTNode(struct ASTNode * n, int indent)
{
    printf("%*s \033[1;33m ASTNode \033[0m \033[1;32m %s \033[0;37m \033[1;35m -> | \033[0m \033[1;31m 0x%x \033[0m \n", indent," ", types_string[n->type], n->val);

    if(n->left != NULL)
    {
        printASTNode(n->left, indent+4);
    }
    if(n->right != NULL)
    {
        printASTNode(n->right, indent+4);
    }
}






void advance()
{
    ctoken = lex();
}

static int accept(int type)
{

    if (ctoken.type == type) 
    {
        ctoken = lex();
        return 1;
    } 
    else 
    {
        return 0;
    }
}

void expect(int type)
{
    if(!accept(type))
    {
        printf("@PARSER FATAL ERROR: expected %s - got : %s \n", types_string[type], types_string[ctoken.type]);
        exit(1);
    }
}


struct ASTNode * make_node(int type, int val, struct ASTNode *left, struct ASTNode *right)
{
    struct ASTNode *node = safe_malloc(sizeof(struct ASTNode));
    node->type           = type;
    node->val            = val;
    node->left           = left;
    node->right          = right;

    return node;
}

struct ASTNode * parse()
{
    struct ASTNode* node = safe_malloc(sizeof(struct ASTNode));
    node->type           = code_block;
    node->left           = expr();
    node->right          = NULL;
    return node;
}





////////////////////////////////////

static struct ASTNode * print_expr()
{
    if(ctoken.type == print )
    {
        advance();
        struct ASTNode * print_exp;

        if(accept(lbr))
        {
            print_exp = expr();
            expect(rbr);
            expect(semi_colon);
        }
        return make_node(print, 0, print_exp, NULL); 
    }
    return NULL;
}


static struct ASTNode * block()
{
    if(ctoken.type == lbrace)
    {
        advance();

        struct ASTNode * block_statement;
        block_statement = expr();
        expect(rbrace);
        return make_node(code_block, 0, block_statement, NULL); 
    }
    return NULL;
}

static struct ASTNode * if_statement()
{
    if(ctoken.type == keyword_if)
    {
        struct ASTNode * if_state;

        advance();
        
        if(accept(lbr))
        {

            if_state = expr();
            expect(rbr);

            struct ASTNode * if_block;
            if_block = block();
            return make_node(keyword_if, 0, if_state, if_block); 
        }
    }
    return NULL;
}

static struct ASTNode * else_statement()
{
    if(ctoken.type == keyword_else)
    {
        struct ASTNode * else_body;

        advance();
        
        else_body = block();

        return make_node(keyword_else, 0, else_body, NULL); 
        
    }
    return NULL;
}



static struct ASTNode * assign_expr()
{
        if(ctoken.type == int_type)
        {

            int assign_type = ctoken.type;
            advance();

            if(ctoken.type != identifier)
            {
                exit(0);
            }


            int name_var = ctoken.attr;
            advance();

            if(ctoken.type == assign_equal)
            {
                advance();
                struct ASTNode * rr = expr();
                struct ASTNode * pr = make_node(symbol, name_var, NULL, NULL);
                expect(semi_colon);
                return make_node(assign_int, name_var, pr, rr); 
            }
            else if(ctoken.type == semi_colon)
            {
                advance();
                return make_node(assign_int, name_var, NULL, NULL); 
            }
            else 
            {
                // must either assign or declare it. Has to end with an expresssion OR ';'
                printf("errrr must assign or declare!\n");
                return NULL;
            }
            
        }

        if(ctoken.type == float_type)
        {
            int assign_type = ctoken.type;
            advance();

            if(ctoken.type != identifier)
            {
                exit(0);
            }

            int name_var = ctoken.attr;
            advance();

            if(ctoken.type == assign_equal)
            {
                advance();
                struct ASTNode * rr = expr();
                struct ASTNode * pr = make_node(symbol, name_var, NULL, NULL);
                expect(semi_colon);
                return make_node(assign_float, name_var, pr, rr); 
            }
            else if(ctoken.type == semi_colon)
            {
                advance();
                return make_node(assign_int, name_var, NULL, NULL); 
            }
            else 
            {
                // must either assign or declare it. Has to end with an expresssion OR ';'
                printf("errrr must assign or declare!\n");
                return NULL;
            }
            
        }


        /* string declaration parsing */
        else if(ctoken.type == str_type )
        {
            int assign_type = ctoken.type;
            advance();

            if(ctoken.type != identifier)
            {
                printf("identifier expected after STRING keyword\n");
                exit(0);
            }

            int name_var = ctoken.attr;
            advance();
            
            if(ctoken.type == assign_equal)
            {
                advance();
                struct ASTNode * rr = factor();
                struct ASTNode * pr = make_node(identifier, name_var, NULL, NULL);
                expect(semi_colon);
                return make_node(assign_str, name_var, pr, rr); 
            }
        }
        else if(ctoken.type == bool_type)
        {
            int assign_type = ctoken.type;
            advance();

            if(ctoken.type != identifier)
            {
                printf("identifier expected after STRING keyword\n");
                exit(0);
            }

            int name_var = ctoken.attr;
            advance();

            if(ctoken.type == assign_equal)
            {
                advance();
                struct ASTNode * rr = factor();
                struct ASTNode * pr = make_node(symbol, name_var, NULL, NULL);
                expect(semi_colon);
                return make_node(assign_bool, name_var, pr, rr); 
            }

        }
        
        else if(ctoken.type == identifier)
        {
            /**
             * we have a fundemental problem here - 
             * when a keyword like IF or PRINT is used, 
             * we end up here, and here we return NULL..  
            */
            int name_var = ctoken.attr;
            advance();

            if(ctoken.type == assign_equal)
            {
                advance();
                struct ASTNode * rr = expr();
                struct ASTNode * pr = make_node(symbol, name_var, NULL, NULL);
                expect(semi_colon);
                return make_node(assign_int, name_var, pr, rr); 
            }
            else 
            {
                // used in print statement
                struct ASTNode * pr = make_node(identifier, name_var, NULL, NULL);
                return pr;

            }
        }
        
        
        
        return NULL;
}





static struct ASTNode * term()
{
    struct ASTNode * left = factor();
    while(ctoken.type == bin_mul || ctoken.type == bin_div )
    {
       struct ASTNode * optoken =  make_node(ctoken.type, ctoken.attr, NULL, NULL);
       advance();
       struct ASTNode * right   =  factor();
       left = make_node(optoken->type, optoken->val, left, right);
    }
    return left;
}

static struct ASTNode * expr()
{
    if(ctoken.type == comment)
    {
        return NULL;
    }

    if(ctoken.type == print)
    {
        struct ASTNode * prt_expr = print_expr();
        return prt_expr;
    }

    if(ctoken.type == int_type || ctoken.type == str_type || ctoken.type == bool_type || ctoken.type == float_type)
    {
        struct ASTNode * assigning = assign_expr();
        return assigning;
    }

    if(ctoken.type == keyword_if)
    {
        struct ASTNode * if_stating = if_statement();
        return if_stating;
    }

    if(ctoken.type == int_literal || ctoken.type == str_literal || ctoken.type == identifier || ctoken.type == float_literal)
    {
        struct ASTNode * left = term();
        while(ctoken.type == bin_add || ctoken.type == bin_sub )
        {
            struct ASTNode * optoken =  make_node(ctoken.type, ctoken.attr, NULL, NULL);
            advance();
            struct ASTNode * right   =  term();
            left = make_node(optoken->type, optoken->val, left, right);
        }
        return left;
    }

    printf("returning NULL @ token type : %s\n", types_string[ctoken.type]);
    return NULL;

}


static struct ASTNode * factor()
{
    struct ASTNode* node = safe_malloc(sizeof(struct ASTNode));
    node->left           = NULL;
    node->right          = NULL;

    if (ctoken.type == identifier) 
    {
        node->type = identifier;
        node->val  = ctoken.attr;
        advance();
    } 
    else if(ctoken.type == int_literal) 
    {
        node->type = int_literal;
        node->val  = ctoken.attr;
        advance();
    }  
    else if(ctoken.type == float_literal) 
    {
        node->type = float_literal;
        node->val  = ctoken.attr;
        advance();
    }     
    else if(ctoken.type == str_literal)
    {
        node->type = str_literal;
        node->val  = ctoken.attr;
        advance();
    }
    else if(ctoken.type == bool_literal)
    {
        node->type = bool_literal;
        node->val  = ctoken.attr;
        advance();
    }
    else
    {
        printf("PARSER::ERROR %s %d\n", types_string[ctoken.type], ctoken.type);
        exit(1);
    }
    return node; 

}






#endif
