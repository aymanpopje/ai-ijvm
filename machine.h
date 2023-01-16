#if !defined(MACHINE_H)
#define MACHINE_H

#include <stdio.h>
#include <stdbool.h>
#include "stack.h"
#include "ijvm.h"

#define $_DEBUG_OPS

#if defined($_DEBUG_OPS)
#define $_DEBUG_CALL(op) $_DEBUG_LN("[<op>] called " #op)
#else
#define $_DEBUG_CALL(op)
#endif

typedef struct {
    uint32_t constantPoolSize;
    uint32_t constantPoolOrigin;
    constant_t* constantPool;
    uint32_t textOrigin;
    uint32_t textSize;
    byte_t* text;
    uint32_t textOffset;
    uint32_t baseTextOffset;
    uint32_t baseStackOffset;
    stack_t stack;
    bool wide;
} machine;

byte_t next_text_byte(machine* instance);

word_t next_text_word(machine* instance);

machine init_ijvm(FILE* file);

void destroy_ijvm(machine* instance);

bool is_end_of_program(machine* instance);

void step(machine* instance);

#endif