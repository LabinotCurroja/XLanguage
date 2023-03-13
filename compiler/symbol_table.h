#ifndef VMX_SYM_TABLE
#define VMX_SYM_TABLE

#define MAX_SYMBOL_TABLE_SIZE 256

#include <stdlib.h>
#include <string.h>
#include "util.h"
#include "../vm/types.h"



struct Table
{
    i64   id;
    i64   val;
    i64   type;
    char *name;
};


struct Table * symbol_table[MAX_SYMBOL_TABLE_SIZE];
int table_size = 0;


struct Table * get_symbol(int id)
{
    return symbol_table[id];
}

i64 add_butterfly_symbol(i64 butterfly)
{
    // this should add a butterfly value into a symbol 
    
    struct Table *item       = safe_malloc(sizeof(struct Table));
    item->id                 = table_size;
    item->name               = "_BUTTERFLY_VMX_"; 
    item->val                = butterfly;
    symbol_table[table_size] = item;
    
    table_size++;
    return table_size-1; 
    
}

i64 add_symbol(char *name)
{
    int i;
    for (i = 0; i < table_size; i++) 
    {
        if (strcmp(symbol_table[i]->name, name) == 0)
            return symbol_table[i]->id;
    }

    struct Table *item       = safe_malloc(sizeof(struct Table));
    item->id                 = table_size;
    item->name               = name;
    item->val                = 0;
    symbol_table[table_size] = item;

    table_size++;
    //printf("add_symbol() bf symbol tsize %d  - %s\n", table_size, name);

    return table_size-1; 
}

void set_sym(int id, int val)
{
    symbol_table[id]->val = val;
}

int get_table_size()
{
    return table_size;
}





#endif