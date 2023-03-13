#ifndef VMX_UTIL
#define VMX_UTIL





void VMX_LEX_FATAL(char *msg)
{
    fprintf(stderr, "%s\n", msg);
    exit(EXIT_FAILURE);
}


void *safe_malloc(size_t size)
{
    void *const mem = malloc(size);
    if (size != 0 && !mem)
        VMX_LEX_FATAL("Out of memory");
    return mem;
}

void VMX_printf(char * data)
{
    
}


#endif