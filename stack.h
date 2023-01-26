#if !defined(STACK_H)
#define STACK_H

#include <stdbool.h>
#include "util.h"

typedef struct stack {
    byte_t* bytes;
    size_t top;
    size_t capacity;
} stack_t;

stack_t stack_construct();

void stack_destroy(stack_t* stack);

bool stack_is_empty(stack_t* stack);

void* stack_get_nth_from_top(stack_t* stack, size_t offset);

void* stack_get(stack_t* stack, size_t index);

void stack_push_byte(stack_t* stack, byte_t byte);

void stack_push_word(stack_t* stack, word_t word);

void stack_push_dword(stack_t* stack, dword_t word);

void stack_push_byte_array(stack_t* stack, byte_t bytes[], size_t size);

byte_t stack_pop_byte(stack_t* stack);

word_t stack_pop_word(stack_t* stack);

dword_t stack_pop_dword(stack_t* stack);

void stack_reserve(stack_t* stack, size_t n);

void stack_pop_n(stack_t* stack, size_t n);

byte_t stack_top_byte(stack_t* stack);

word_t stack_top_word(stack_t* stack);

dword_t stack_top_dword(stack_t* stack);

size_t stack_size(stack_t* stack);

#endif