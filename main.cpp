#include "stack_functions.h"

int main()
{
    STACK stk = {};
    int capacity = 8;
    FILE* log_file = fopen("log_file.txt", "a");
    stack_ctor(&stk, capacity, log_file);
    elem_t value = 10;
    for (int i = 0; i < 20; i++)
        stk.error = stack_push(&stk, value, log_file);

    for(int i = 0; i < 20; i++)
        stack_pop(&stk, log_file);
    stack_dtor(&stk);//TODO Stack_Dump()
    fclose(log_file);
    return 0;
}