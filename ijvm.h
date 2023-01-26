#if !defined(IJVM_H)
#define IJVM_H

#include <stdint.h>
#include <stdbool.h>
#include "stack.h"
#include "util.h"

#define $_DEBUG_OPCODE(mnemonic) $_DEBUG_LN("[<op>] Executed " #mnemonic " (" $_STRINGIFY($_##mnemonic) ") (PC: %d)", instance->program_counter - 1)
#define $_IJVM_HEADER 0x1deadfad

#define $_NOP           0x00
#define $_BIPUSH        0x10
#define $_LDC_W         0x13
#define $_ILOAD         0x15
#define $_ISTORE        0x36
#define $_POP           0x57
#define $_DUP           0x59
#define $_SWAP          0x5F
#define $_IADD          0x60
#define $_ISUB          0x64
#define $_IAND          0x7E
#define $_IINC          0x84
#define $_IFEQ          0x99
#define $_IFLT          0x9B
#define $_IF_ICMPEQ     0x9F
#define $_GOTO          0xA7
#define $_IRETURN       0xAC
#define $_IOR           0xB0
#define $_INVOKEVIRTUAL 0xB6
#define $_WIDE          0xC4
#define $_IN            0xFC
#define $_OUT           0xFD
#define $_ERR           0xFE
#define $_HALT          0xFF

#define $_IJVMS_OK                      ((const char*)NULL)
#define $_IJVMS_STACK_UNDERFLOW         "Stack Underflow"
#define $_IJVMS_END_OF_TEXT             "End of text"
#define $_IJVMS_ERR                     "Error Occurred"
#define $_IJVMS_HALT                    "Halted"
#define $_IJVMS_INVALID_BINARY_FORMAT   "Invalid Binary Format"
#define $_IJVMS_INVALID_OPERATION       "Invalid Operation"
#define $_IJVMS_OOB_CONSTANT_LOOKUP     "Out of bounds constant lookup"
#define $_IJVMS_OOB_VARIABLE_LOOKUP     "Out of bounds variable lookup"
#define $_IJVMS_INVALID_RETURN          "Tried to return, but no function was called"

typedef struct function_state {
    dword_t call_stack_size;
    dword_t return_address;

    var_t* variables;
    dword_t local_storage_size;

    struct function_state* caller;
} function_state_t;

typedef struct {
    constant_t* constant_pool;
    byte_t* text;
    stack_t stack;
    bool wide;
    dword_t nesting;

    function_state_t frame;

    dword_t text_size;
    dword_t text_origin;
    dword_t constant_pool_size;
    dword_t constant_pool_origin;

    dword_t program_counter;
    const char* state;

    FILE* in;
    FILE* out;
} ijvm_t;

ijvm_t init_ijvm(FILE* binary, FILE* in, FILE* out);

void destroy_ijvm(ijvm_t* instance);

void step(ijvm_t* instance);

bool can_step(ijvm_t* instance);

constant_t get_constant(ijvm_t* instance, constant_name_t index);

#endif