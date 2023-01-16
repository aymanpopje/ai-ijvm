#include "stack.h"

#include <stdlib.h>

void _stack_realloc(stack_t* stack, size_t elementSize) {
    if(stack->top + elementSize > stack->capacity) {
        size_t halfCap = stack->capacity / 2;
        stack->capacity += (elementSize > halfCap) ? elementSize : halfCap;
        stack->bytes = realloc(stack->bytes, stack->capacity);
    }
}

void _push_byte(stack_t* stack, byte_t byte) {
    stack->bytes[stack->top++] = byte;
}

stack_t stack_construct() {
    return ((stack_t) { .capacity = sizeof(byte_t) * 16, .bytes = malloc(sizeof(byte_t) * 16), .top = 0  });
}

void stack_destroy(stack_t* stack) {
    free(stack->bytes);
    stack->top = stack->capacity = 0;
}

bool stack_is_empty(stack_t* stack) {
    return stack->top == 0;
}

void stack_push_byte(stack_t* stack, byte_t byte) {
    _stack_realloc(stack, sizeof(byte_t));
    _push_byte(stack, byte);
}

void stack_push_word(stack_t* stack, word_t word) {
    _stack_realloc(stack, sizeof(word_t));
    *((word_t*) (stack->bytes + stack->top)) = word;
    stack->top += sizeof(word_t);
}

void stack_push_dword(stack_t* stack, dword_t word) {
    _stack_realloc(stack, sizeof(dword_t));
    *((dword_t*) (stack->bytes + stack->top)) = word;
    stack->top += sizeof(dword_t);
}

void stack_push_byte_array(stack_t* stack, byte_t bytes[], size_t size) {
    _stack_realloc(stack, size);
    for(size_t i = 0; i != size; i++) {
        _push_byte(stack, bytes[i]);
    }
}

void _assert_no_underflow(size_t top, size_t requiredSize) {
    //$_ASSERT(top >= requiredSize, "Stack Underflow Error");
}

void _shrink_capacity(stack_t* stack) {
    if(stack->top < stack->capacity / 2 && stack->top > 16) {
        stack->bytes = realloc(stack->bytes, stack->top);
    }
}

byte_t stack_pop_byte(stack_t* stack) {
    _assert_no_underflow(stack->top, sizeof(byte_t));
    byte_t top = stack->bytes[--stack->top];
    _shrink_capacity(stack);
    return top;
}

word_t stack_pop_word(stack_t* stack) {
    _assert_no_underflow(stack->top, sizeof(word_t));
    word_t top = *((word_t*) (stack->bytes + (stack->top -= sizeof(word_t))));
    _shrink_capacity(stack);
    return top;
}

dword_t stack_pop_dword(stack_t* stack) {
    _assert_no_underflow(stack->top, sizeof(dword_t));
    dword_t top = *((dword_t*) (stack->bytes + (stack->top -= sizeof(dword_t))));
    _shrink_capacity(stack);
    return top;
}