#include "stack_functions.h"

//--------------------------------------------------------------------
void stack_buff_ctor(elem_t* buff, ssize_t capacity)
{
    ON_CANARY_PROT(*(buff - sizeof(canary_t)) = CANARY_VALUE);

    for (ssize_t i = 0; i < capacity; i++) // TODO use memset
        buff[i] = POISION;
    
    ON_CANARY_PROT(buff[capacity] = CANARY_VALUE);

    return;
}
//-------------------------------------------------------------------
int very_tor(STACK* stk, FILE* log_file)
{   
    int error = 0;
    if((long)stk->buff < 400)//TODO write in file through stdout // TODO remove magic constants
        error = error | ZERO_PAGE;

    if(stk->capacity < 0)
        error = error | NEGATIVE_CAPACITY;

    if(stk->size < 0)
        error = error | NEGATIVE_SIZE;

    if(stk->capacity < stk->size)
        error = error | SIZE_MORE_CAPACITY;
    
    ON_HASH_PROT
    (
        if(stk->hash != stack_hash(stk->buff, stk->capacity))
            error = error | HASH_CHANGED;
    )

    ON_CANARY_PROT
    (
        if(stk.left_canary != CANARY_VALUE)
            error = error | LEFT_STACK_CANARY;
        if(stk.right_canary != CANARY_VALUE)
            error = error | RIGHT_STACK_CANARY;
        if(*(stk->buff - sizeof(canary_t)) != CANARY_VALUE)
            error = error | LEFT_BUFF_CANARY;
        if(stk->buff[stk->capacity] != CANARY_VALUE)
            error = error | RIGHT_BUFF_CANARY;
    )
    // fprintf(log_file, "%d\n", error);                 
    stk->error = error;
    return error;
}
//-------------------------------------------------------------------
int stack_ctor(STACK* stk, ssize_t capacity, FILE* log_file)
{
    if(capacity < 8)
        capacity = 8;

    stk->buff = (elem_t*)calloc(capacity * sizeof(elem_t) ON_CANARY_PROT(+ 2 * sizeof(canary_t)), 1) ON_CANARY_PROT(+ sizeof(canary_t));//TODO normal form of calloc
    
    stack_buff_ctor(stk->buff, capacity);

    ON_HASH_PROT(stk->hash = stack_hash(stk->buff, capacity);)
    stk->capacity = capacity;
    stk->size = 0;
    stk->error = 0;

    ON_CANARY_PROT(stk->left_canary = CANARY_VALUE;)
    ON_CANARY_PROT(stk->right_canary = CANARY_VALUE;)

    if((stk->error = very_tor(stk, log_file)) != 0)
        stack_dump(stk, log_file);

    return stk->error;
}
//-------------------------------------------------------------------
int stack_resize(STACK* stk, FILE* log_file)
{
    if((stk->error = very_tor(stk, log_file)) != 0)
        stack_dump(stk, log_file);

    size_t new_capacity = stk->capacity;
    if(stk->capacity >= stk->size * RESIZE_STEP && stk->capacity > 8)
        new_capacity = stk->capacity / RESIZE_STEP;
        
    else if(stk->capacity == stk->size)
        new_capacity = stk->capacity * RESIZE_STEP;

    elem_t* new_buff = (elem_t*)calloc(new_capacity * sizeof(elem_t) ON_CANARY_PROT(+ 2 * sizeof(canary_t)), 1) ON_CANARY_PROT(+ sizeof(canary_t));

    stack_buff_ctor(new_buff, new_capacity);

    for(ssize_t i = 0; i < stk->size; i++)
        new_buff[i] = stk->buff[i];

    stack_buff_dtor(stk->buff, stk->capacity);
    stk->capacity = new_capacity;
    stk->buff = new_buff;

    ON_HASH_PROT(stk->hash = stack_hash(stk->buff, stk->capacity);)

    if((stk->error = very_tor(stk, log_file)) != 0)
        stack_dump(stk, log_file);

    return 0;
}
//-------------------------------------------------------------------
int stack_push(STACK* stk, elem_t value, FILE* log_file)
{
    int error = 0;
    if((error = very_tor(stk, log_file)) != 0)
        stack_dump(stk, log_file);

    stack_resize(stk, log_file);
    stk->buff[stk->size++] = value;

    ON_HASH_PROT(stk->hash = stack_hash(stk->buff, stk->capacity);)

    if((error = very_tor(stk, log_file)) != 0)
        stack_dump(stk, log_file);
    return error;
}

//-------------------------------------------------------------------
elem_t stack_pop(STACK* stk, FILE* log_file)
{
    if((very_tor(stk, log_file)) != 0)
        stack_dump(stk, log_file);

    if(stk->size == 0)
    {
        // system("shutdown now");
        fprintf(log_file, "Stack is empty\n");
        return POISION;
    }

    stack_resize(stk, log_file);

    stk->size--;
    elem_t value = stk->buff[stk->size];

    ON_HASH_PROT(stk->hash = stack_hash(stk->buff, stk->capacity);)

    if((very_tor(stk, log_file)) != 0)
        stack_dump(stk, log_file);
    
    return value;
}
//-------------------------------------------------------------------
void stack_buff_dtor(elem_t* buff, ssize_t capacity)
{
    *buff = POISION;
    for (ssize_t i = 0; i < capacity; i++) // TODO use memset
        buff[i] = POISION;

    ON_CANARY_PROT
    (
        *(buff - sizeof(canary_t)) = POISION;
        buff[capacity] = POISION;
    )

    free(buff);

    return;
}
//-------------------------------------------------------------------
int stack_dtor(STACK* stk)
{
    stack_buff_dtor(stk->buff, stk->capacity);
    stk->capacity = -1;
    stk->size = -1;

    ON_HASH_PROT(stk->hash = -1;)

    ON_CANARY_PROT
    (
        stk.left_canary = POISION;
        stk.right_canary = POISION;
    )

    return 0;
}
//----------------------------------------------------------------------------
void Error_print(int error, FILE* log_file = stderr)
{

    if(error & ZERO_PAGE)
        fprintf(log_file, "pointer in zero page\n");
        
    if(error & NEGATIVE_CAPACITY)
        fprintf(log_file, "Negative capacity of the stack\n");

    if(error & NEGATIVE_SIZE)
        fprintf(log_file, "Negative size of the stack\n");

    if(error & SIZE_MORE_CAPACITY)
        fprintf(log_file, "Size of the stack more then capacity of the stack\n");

    if(error & HASH_CHANGED)
        fprintf(log_file, "Hash was changed\n");

    if(error & LEFT_STACK_CANARY)
        fprintf(log_file, "Left stack canary was attacked");

    if(error & RIGHT_STACK_CANARY)
        fprintf(log_file, "Right stack canary was attacked");

    if(error & LEFT_BUFF_CANARY)
        fprintf(log_file, "Left canary of the stack buffer was attacked");
    
    if(error & RIGHT_BUFF_CANARY)
        fprintf(log_file, "Right canary of the stack buffer was attacked");

    return;
}
//-----------------------------------------------------------------------------
void stack_dump(const STACK* const stk, FILE* log_file)
{
    printf("dump\n");
    Error_print(stk->error, log_file);
    
    return;
}
//-----------------------------------------------------------------------------
size_t stack_hash(elem_t* buff, size_t len)
{
    size_t hash = 1;
    for (size_t i = 0; i < len; i++)
        hash = hash * 33 + buff[i];
    return hash;
}
//-----------------------------------------------------------------------------