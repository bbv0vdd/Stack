#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <math.h>
#include <errno.h>
#include <string.h>

typedef double elem_t;
typedef unsigned long long canary_t;

enum STACK_ERRORS
{
    ZERO_PAGE = 1,
    NEGATIVE_CAPACITY = 2,
    NEGATIVE_SIZE = 4,
    SIZE_MORE_CAPACITY = 8,
    HASH_CHANGED = 16,
    LEFT_STACK_CANARY = 32,
    RIGHT_STACK_CANARY = 64,
    LEFT_BUFF_CANARY = 128,
    RIGHT_BUFF_CANARY = 256,
};
#define POISION NAN // enum
#define CANARY_VALUE 1000
#define RESIZE_STEP 2

#ifdef CANARY_PROT
    #define ON_CANARY_PROT(...) __VA_ARGS__
#else
    #define ON_CANARY_PROT(...)
#endif


#ifdef HASH_PROT
    #define ON_HASH_PROT(...) __VA_ARGS__
#else
    #define ON_HASH_PROT(...)
#endif


struct STACK
{
    ON_CANARY_PROT(elem_t left_canary;)
    elem_t* buff;
    ssize_t capacity;
    ssize_t size;
    ON_HASH_PROT(size_t hash;)
    int error;
    ON_CANARY_PROT(elem_t right_canary;)
};
size_t stack_hash(elem_t* buff, size_t len);
void stack_dump(const STACK* const stk, FILE* log_file);
void stack_buff_ctor(elem_t* buff, ssize_t capacity);
int very_tor(STACK* stk, FILE* log_file);
int stack_ctor(STACK* pointer_stk, ssize_t capacity, FILE* log_file);
int stack_resize(STACK* stk, FILE* log_file);
int stack_push(STACK* stk, elem_t value, FILE* log_file);
elem_t stack_pop(STACK* stk, FILE* log_file);
void stack_buff_dtor(elem_t* buff, ssize_t capacity);
int stack_dtor(STACK* stk);