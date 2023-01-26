#include <stdlib.h>
#include "ijvm.h"

byte_t _ijvm_next_text_byte(ijvm_t* instance) {
    byte_t current = *(byte_t*)(instance->text + instance->program_counter);
    instance->program_counter += sizeof(byte_t);
    return current;
}

word_t _ijvm_next_text_word(ijvm_t* instance) {
    word_t current = *(word_t*)(instance->text + instance->program_counter);
    instance->program_counter += sizeof(word_t);
    return current;
}

byte_t _ijvm_pop_byte(ijvm_t* instance) {
    if(instance->frame.call_stack_size < sizeof(byte_t)) {
        instance->state = $_IJVMS_STACK_UNDERFLOW;
        return 0;
    }
    byte_t top = stack_pop_byte(&instance->stack);
    instance->frame.call_stack_size -= sizeof(byte_t);
    return top;
}

word_t _ijvm_pop_word(ijvm_t* instance) {
    if(instance->frame.call_stack_size < sizeof(word_t)) {
        instance->state = $_IJVMS_STACK_UNDERFLOW;
        return 0;
    }
    word_t top = stack_pop_word(&instance->stack);
    instance->frame.call_stack_size -= sizeof(word_t);
    return top;
}

dword_t _ijvm_pop_dword(ijvm_t* instance) {
    if(instance->frame.call_stack_size < sizeof(dword_t)) {
        instance->state = $_IJVMS_STACK_UNDERFLOW;
        return 0;
    }
    dword_t top = stack_pop_dword(&instance->stack);
    instance->frame.call_stack_size -= sizeof(dword_t);
    return top;
}

void _ijvm_push_byte(ijvm_t* instance, byte_t byte) {
    stack_push_byte(&instance->stack, byte);
    instance->frame.call_stack_size += sizeof(byte_t);
}

void _ijvm_push_word(ijvm_t* instance, word_t word) {
    stack_push_word(&instance->stack, word);
    instance->frame.call_stack_size += sizeof(word_t);
}

void _ijvm_push_dword(ijvm_t* instance, dword_t dword) {
    stack_push_dword(&instance->stack, dword);
    instance->frame.call_stack_size += sizeof(dword_t);
}

void _ijvm_jump(ijvm_t* instance, label_name_t label) {
    instance->program_counter = (instance->program_counter + label) - sizeof(byte_t) - sizeof(label_name_t);
}

void _ijvm_jump_conditional(ijvm_t* instance, bool condition, label_name_t label) {
    if(condition) {
        _ijvm_jump(instance, label);
    }
}

constant_t _ijvm_get_constant(ijvm_t* instance, constant_name_t index) {
    if(index >= instance->constant_pool_size / sizeof(constant_t)) {
        instance->state = $_IJVMS_OOB_CONSTANT_LOOKUP;
        printf("[!] Index constant out of bounds error: %d is out of bounds for range [0, %ld)\n", index, instance->constant_pool_size / sizeof(constant_t));
        exit(0);
    }
    return instance->constant_pool[index];
}

var_t* _ijvm_get_variable(ijvm_t* instance, var_name_t index) {
    if(index >= instance->frame.local_storage_size) {
        instance->state = $_IJVMS_OOB_VARIABLE_LOOKUP;
        return NULL;
    }
    return &instance->frame.variables[index];
}

void _bipush(ijvm_t* instance) {
    $_DEBUG_OPCODE(BIPUSH);
    byte_t value = _ijvm_next_text_byte(instance);

    _ijvm_push_byte(instance, value);
}

void _dup(ijvm_t* instance) {
    $_DEBUG_OPCODE(DUP);

    byte_t value = stack_top_byte(&instance->stack);

    _ijvm_push_byte(instance, value);
}

void _err(ijvm_t* instance) {
    $_DEBUG_OPCODE(ERR);
    instance->state = $_IJVMS_ERR;
}

void _goto(ijvm_t* instance) {
    $_DEBUG_OPCODE(GOTO);
    _ijvm_jump(instance, (label_name_t)swap_endianness_word(_ijvm_next_text_word(instance)));
}

void _halt(ijvm_t* instance) {
    $_DEBUG_OPCODE(HALT);
    instance->state = $_IJVMS_HALT;
}

void _iadd(ijvm_t* instance) {
    $_DEBUG_OPCODE(IADD);
    ibyte_t top0 = _ijvm_pop_byte(instance);
    ibyte_t top1 = _ijvm_pop_byte(instance);
    _ijvm_push_byte(instance, top1 + top0);
}

void _iand(ijvm_t* instance) {
    $_DEBUG_OPCODE(IAND);
    byte_t top0 = _ijvm_pop_byte(instance);
    byte_t top1 = _ijvm_pop_byte(instance);
    _ijvm_push_byte(instance, top0 & top1);
}

void _ifeq(ijvm_t* instance) {
    $_DEBUG_OPCODE(IFEQ);
    byte_t top = _ijvm_pop_byte(instance);
    _ijvm_jump_conditional(instance, top == 0, (label_name_t)swap_endianness_word(_ijvm_next_text_word(instance)));
}

void _iflt(ijvm_t* instance) {
    $_DEBUG_OPCODE(IFLT);
    ibyte_t top = _ijvm_pop_byte(instance);
    _ijvm_jump_conditional(instance, top < 0, (label_name_t)swap_endianness_word(_ijvm_next_text_word(instance)));
}

void _if_icmpeq(ijvm_t* instance) {
    $_DEBUG_OPCODE(IF_ICMPEQ);
    byte_t top0 = _ijvm_pop_byte(instance);
    byte_t top1 = _ijvm_pop_byte(instance); 
    _ijvm_jump_conditional(instance, top0 == top1, (label_name_t)swap_endianness_word(_ijvm_next_text_word(instance)));
}

void _iinc(ijvm_t* instance) {
    $_DEBUG_OPCODE(IINC);
    byte_t index = _ijvm_next_text_byte(instance);
    var_t* variable = _ijvm_get_variable(instance, index);
    byte_t inc = _ijvm_next_text_byte(instance);
    if(variable != NULL) {
        variable += inc;
    }
}

void _iload(ijvm_t* instance) {
    $_DEBUG_OPCODE(ILOAD);
    byte_t index =  _ijvm_next_text_byte(instance);
    var_t* variable = _ijvm_get_variable(instance, index);
    if(variable != NULL) {
        _ijvm_push_byte(instance, *variable);
    }
}

void _in(ijvm_t* instance) {
    $_DEBUG_OPCODE(IN);
    byte_t in = fgetc(instance->in);
    _ijvm_push_byte(instance, (in == (byte_t)EOF) ? 0 : in);
}

// TODO this shit is scuffed:
void _invokevirtual(ijvm_t* instance) {
    $_DEBUG_OPCODE(INVOKEVIRTUAL);
    
    function_state_t* prev_frame = malloc(sizeof(function_state_t));
    *prev_frame = instance->frame;

    dword_t return_address = instance->program_counter + sizeof(word_t);

    word_t label = swap_endianness_word(_ijvm_next_text_word(instance));
    dword_t function_address = swap_endianness_dword(_ijvm_get_constant(instance, label));

    instance->program_counter = function_address; 

    word_t arg_count = swap_endianness_word(_ijvm_next_text_word(instance));
    word_t local_var_count = swap_endianness_word(_ijvm_next_text_word(instance));

    instance->frame.caller = prev_frame;
    instance->frame.local_storage_size = arg_count + local_var_count;
    instance->frame.return_address = return_address;
    instance->frame.variables = calloc(instance->frame.local_storage_size, sizeof(var_t));

    for(word_t i = arg_count; i != 0; i--) {
        instance->frame.variables[i - 1] = _ijvm_pop_byte(instance);
    }
    _ijvm_pop_byte(instance);
    prev_frame->call_stack_size = instance->frame.call_stack_size;

    instance->frame.call_stack_size = 0;

    instance->nesting++;
}

void _ior(ijvm_t* instance) {
    $_DEBUG_OPCODE(IOR);
    byte_t top0 = _ijvm_pop_byte(instance);
    byte_t top1 = _ijvm_pop_byte(instance);
    _ijvm_push_byte(instance, top0 | top1);
}

void _ireturn(ijvm_t* instance) {
    $_DEBUG_OPCODE(IRETURN);
    
    if(instance->frame.caller == NULL) {
        instance->state = $_IJVMS_INVALID_RETURN;
        return;
    }

    function_state_t caller = *instance->frame.caller;
    byte_t return_value = _ijvm_pop_byte(instance);
    dword_t return_address = instance->frame.return_address;

    stack_pop_n(&instance->stack, instance->frame.call_stack_size);

    free(instance->frame.variables);
    free(instance->frame.caller);

    instance->program_counter = return_address;
    instance->frame = caller;

    instance->nesting--;
}

void _istore(ijvm_t* instance) {
    $_DEBUG_OPCODE(ISTORE);
    var_name_t index = _ijvm_next_text_byte(instance);
    byte_t value = _ijvm_pop_byte(instance);
    var_t* variable = _ijvm_get_variable(instance, index);
    if(variable != NULL) {
        *variable = value;
    }
}

void _isub(ijvm_t* instance) {
    $_DEBUG_OPCODE(ISUB);
    ibyte_t top0 = (ibyte_t) _ijvm_pop_byte(instance);
    ibyte_t top1 = (ibyte_t) _ijvm_pop_byte(instance);
    _ijvm_push_byte(instance, top1 - top0);
}

void _ldc_w(ijvm_t* instance) {
    $_DEBUG_OPCODE(LDC_W);
    _ijvm_push_dword(instance, swap_endianness_dword(_ijvm_get_constant(instance, swap_endianness_word(_ijvm_next_text_word(instance)))));
}

void _nop(ijvm_t* instance) {
    $_DEBUG_OPCODE(NOP);
}

void _out(ijvm_t* instance) {
    $_DEBUG_OPCODE(OUT);
    fputc(_ijvm_pop_byte(instance), instance->out);
    fflush(instance->out);
}

void _pop(ijvm_t* instance) {
    $_DEBUG_OPCODE(POP);
    _ijvm_pop_byte(instance);
}

void _swap(ijvm_t* instance) {
    $_DEBUG_OPCODE(SWAP);
    byte_t top0 = _ijvm_pop_byte(instance);
    byte_t top1 = _ijvm_pop_byte(instance);
    _ijvm_push_byte(instance, top0);
    _ijvm_push_byte(instance, top1);
}

void _wide(ijvm_t* instance) {
    $_DEBUG_OPCODE(WIDE);
    instance->wide = true;
}

void _invalid_opcode(ijvm_t* instance) {
    instance->state = $_IJVMS_INVALID_OPERATION;
    instance->program_counter -= sizeof(byte_t);
    printf("[!] Invalid opcode 0x%hhx\n", *(instance->text + instance->program_counter));
}

void _execute(ijvm_t* instance) {
    // Wanted to make this a jump table, but C doesn't support it (portably).
    // Opted for a switch that has a case for every value, so the compiler will (hopefully) turn it into a jump table anyway.
    switch (_ijvm_next_text_byte(instance)) {
        case $_NOP: { _nop(instance); break; }
        case 0x01: { _invalid_opcode(instance); break; }
        case 0x02: { _invalid_opcode(instance); break; }
        case 0x03: { _invalid_opcode(instance); break; }
        case 0x04: { _invalid_opcode(instance); break; }
        case 0x05: { _invalid_opcode(instance); break; }
        case 0x06: { _invalid_opcode(instance); break; }
        case 0x07: { _invalid_opcode(instance); break; }
        case 0x08: { _invalid_opcode(instance); break; }
        case 0x09: { _invalid_opcode(instance); break; }
        case 0x0A: { _invalid_opcode(instance); break; }
        case 0x0B: { _invalid_opcode(instance); break; }
        case 0x0C: { _invalid_opcode(instance); break; }
        case 0x0D: { _invalid_opcode(instance); break; }
        case 0x0E: { _invalid_opcode(instance); break; }
        case 0x0F: { _invalid_opcode(instance); break; }
        case $_BIPUSH: { _bipush(instance); break; }
        case 0x11: { _invalid_opcode(instance); break; }
        case 0x12: { _invalid_opcode(instance); break; }
        case $_LDC_W: { _ldc_w(instance); break; }
        case 0x14: { _invalid_opcode(instance); break; }
        case $_ILOAD: { _iload(instance); break; }
        case 0x16: { _invalid_opcode(instance); break; }
        case 0x17: { _invalid_opcode(instance); break; }
        case 0x18: { _invalid_opcode(instance); break; }
        case 0x19: { _invalid_opcode(instance); break; }
        case 0x1F: { _invalid_opcode(instance); break; }
        case 0x20: { _invalid_opcode(instance); break; }
        case 0x21: { _invalid_opcode(instance); break; }
        case 0x22: { _invalid_opcode(instance); break; }
        case 0x23: { _invalid_opcode(instance); break; }
        case 0x24: { _invalid_opcode(instance); break; }
        case 0x25: { _invalid_opcode(instance); break; }
        case 0x26: { _invalid_opcode(instance); break; }
        case 0x27: { _invalid_opcode(instance); break; }
        case 0x28: { _invalid_opcode(instance); break; }
        case 0x29: { _invalid_opcode(instance); break; }
        case 0x2A: { _invalid_opcode(instance); break; }
        case 0x2B: { _invalid_opcode(instance); break; }
        case 0x2C: { _invalid_opcode(instance); break; }
        case 0x2D: { _invalid_opcode(instance); break; }
        case 0x2E: { _invalid_opcode(instance); break; }
        case 0x2F: { _invalid_opcode(instance); break; }
        case 0x30: { _invalid_opcode(instance); break; }
        case 0x31: { _invalid_opcode(instance); break; }
        case 0x32: { _invalid_opcode(instance); break; }
        case 0x33: { _invalid_opcode(instance); break; }
        case 0x34: { _invalid_opcode(instance); break; }
        case 0x35: { _invalid_opcode(instance); break; }
        case $_ISTORE: { _istore(instance); break; }
        case 0x37: { _invalid_opcode(instance); break; }
        case 0x38: { _invalid_opcode(instance); break; }
        case 0x39: { _invalid_opcode(instance); break; }
        case 0x3A: { _invalid_opcode(instance); break; }
        case 0x3B: { _invalid_opcode(instance); break; }
        case 0x3C: { _invalid_opcode(instance); break; }
        case 0x3D: { _invalid_opcode(instance); break; }
        case 0x3E: { _invalid_opcode(instance); break; }
        case 0x3F: { _invalid_opcode(instance); break; }
        case 0x40: { _invalid_opcode(instance); break; }
        case 0x41: { _invalid_opcode(instance); break; }
        case 0x42: { _invalid_opcode(instance); break; }
        case 0x43: { _invalid_opcode(instance); break; }
        case 0x44: { _invalid_opcode(instance); break; }
        case 0x45: { _invalid_opcode(instance); break; }
        case 0x46: { _invalid_opcode(instance); break; }
        case 0x47: { _invalid_opcode(instance); break; }
        case 0x48: { _invalid_opcode(instance); break; }
        case 0x49: { _invalid_opcode(instance); break; }
        case 0x4A: { _invalid_opcode(instance); break; }
        case 0x4B: { _invalid_opcode(instance); break; }
        case 0x4C: { _invalid_opcode(instance); break; }
        case 0x4D: { _invalid_opcode(instance); break; }
        case 0x4E: { _invalid_opcode(instance); break; }
        case 0x4F: { _invalid_opcode(instance); break; }
        case 0x50: { _invalid_opcode(instance); break; }
        case 0x51: { _invalid_opcode(instance); break; }
        case 0x52: { _invalid_opcode(instance); break; }
        case 0x53: { _invalid_opcode(instance); break; }
        case 0x54: { _invalid_opcode(instance); break; }
        case 0x55: { _invalid_opcode(instance); break; }
        case 0x56: { _invalid_opcode(instance); break; }
        case $_POP: { _pop(instance); break; }
        case 0x58: { _invalid_opcode(instance); break; }
        case $_DUP: { _dup(instance); break; }
        case 0x5A: { _invalid_opcode(instance); break; }
        case 0x5B: { _invalid_opcode(instance); break; }
        case 0x5C: { _invalid_opcode(instance); break; }
        case 0x5D: { _invalid_opcode(instance); break; }
        case 0x5E: { _invalid_opcode(instance); break; }
        case $_SWAP: { _swap(instance); break; }
        case $_IADD: { _iadd(instance); break; }
        case 0x61: { _invalid_opcode(instance); break; }
        case 0x62: { _invalid_opcode(instance); break; }
        case 0x63: { _invalid_opcode(instance); break; }
        case $_ISUB: { _isub(instance); break; }
        case 0x65: { _invalid_opcode(instance); break; }
        case 0x66: { _invalid_opcode(instance); break; }
        case 0x67: { _invalid_opcode(instance); break; }
        case 0x68: { _invalid_opcode(instance); break; }
        case 0x69: { _invalid_opcode(instance); break; }
        case 0x6A: { _invalid_opcode(instance); break; }
        case 0x6B: { _invalid_opcode(instance); break; }
        case 0x6C: { _invalid_opcode(instance); break; }
        case 0x6D: { _invalid_opcode(instance); break; }
        case 0x6E: { _invalid_opcode(instance); break; }
        case 0x6F: { _invalid_opcode(instance); break; }
        case 0x70: { _invalid_opcode(instance); break; }
        case 0x71: { _invalid_opcode(instance); break; }
        case 0x72: { _invalid_opcode(instance); break; }
        case 0x73: { _invalid_opcode(instance); break; }
        case 0x74: { _invalid_opcode(instance); break; }
        case 0x75: { _invalid_opcode(instance); break; }
        case 0x76: { _invalid_opcode(instance); break; }
        case 0x77: { _invalid_opcode(instance); break; }
        case 0x78: { _invalid_opcode(instance); break; }
        case 0x79: { _invalid_opcode(instance); break; }
        case 0x7A: { _invalid_opcode(instance); break; }
        case 0x7B: { _invalid_opcode(instance); break; }
        case 0x7C: { _invalid_opcode(instance); break; }
        case 0x7D: { _invalid_opcode(instance); break; }
        case $_IAND: { _iand(instance); break; }
        case 0x7F: { _invalid_opcode(instance); break; }
        case 0x80: { _invalid_opcode(instance); break; }
        case 0x81: { _invalid_opcode(instance); break; }
        case 0x82: { _invalid_opcode(instance); break; }
        case 0x83: { _invalid_opcode(instance); break; }
        case $_IINC: { _iinc(instance); break; }
        case 0x85: { _invalid_opcode(instance); break; }
        case 0x86: { _invalid_opcode(instance); break; }
        case 0x87: { _invalid_opcode(instance); break; }
        case 0x88: { _invalid_opcode(instance); break; }
        case 0x89: { _invalid_opcode(instance); break; }
        case 0x8A: { _invalid_opcode(instance); break; }
        case 0x8B: { _invalid_opcode(instance); break; }
        case 0x8C: { _invalid_opcode(instance); break; }
        case 0x8D: { _invalid_opcode(instance); break; }
        case 0x8E: { _invalid_opcode(instance); break; }
        case 0x8F: { _invalid_opcode(instance); break; }
        case 0x90: { _invalid_opcode(instance); break; }
        case 0x91: { _invalid_opcode(instance); break; }
        case 0x92: { _invalid_opcode(instance); break; }
        case 0x93: { _invalid_opcode(instance); break; }
        case 0x94: { _invalid_opcode(instance); break; }
        case 0x95: { _invalid_opcode(instance); break; }
        case 0x96: { _invalid_opcode(instance); break; }
        case 0x97: { _invalid_opcode(instance); break; }
        case 0x98: { _invalid_opcode(instance); break; }
        case $_IFEQ: { _ifeq(instance); break; }
        case 0x9A: { _invalid_opcode(instance); break; }
        case $_IFLT: { _iflt(instance); break; }
        case 0x9C: { _invalid_opcode(instance); break; }
        case 0x9D: { _invalid_opcode(instance); break; }
        case 0x9E: { _invalid_opcode(instance); break; }
        case $_IF_ICMPEQ: { _if_icmpeq(instance); break; }
        case 0xA0: { _invalid_opcode(instance); break; }
        case 0xA1: { _invalid_opcode(instance); break; }
        case 0xA2: { _invalid_opcode(instance); break; }
        case 0xA3: { _invalid_opcode(instance); break; }
        case 0xA4: { _invalid_opcode(instance); break; }
        case 0xA5: { _invalid_opcode(instance); break; }
        case 0xA6: { _invalid_opcode(instance); break; }
        case $_GOTO: { _goto(instance); break; }
        case 0xA8: { _invalid_opcode(instance); break; }
        case 0xA9: { _invalid_opcode(instance); break; }
        case 0xAA: { _invalid_opcode(instance); break; }
        case 0xAB: { _invalid_opcode(instance); break; }
        case $_IRETURN: { _ireturn(instance); break; }
        case 0xAD: { _invalid_opcode(instance); break; }
        case 0xAE: { _invalid_opcode(instance); break; }
        case 0xAF: { _invalid_opcode(instance); break; }
        case $_IOR: { _ior(instance); break; }
        case 0xB1: { _invalid_opcode(instance); break; }
        case 0xB2: { _invalid_opcode(instance); break; }
        case 0xB3: { _invalid_opcode(instance); break; }
        case 0xB4: { _invalid_opcode(instance); break; }
        case 0xB5: { _invalid_opcode(instance); break; }
        case $_INVOKEVIRTUAL: { _invokevirtual(instance); break; }
        case 0xB7: { _invalid_opcode(instance); break; }
        case 0xB8: { _invalid_opcode(instance); break; }
        case 0xB9: { _invalid_opcode(instance); break; }
        case 0xBA: { _invalid_opcode(instance); break; }
        case 0xBB: { _invalid_opcode(instance); break; }
        case 0xBC: { _invalid_opcode(instance); break; }
        case 0xBD: { _invalid_opcode(instance); break; }
        case 0xBE: { _invalid_opcode(instance); break; }
        case 0xBF: { _invalid_opcode(instance); break; }
        case 0xC0: { _invalid_opcode(instance); break; }
        case 0xC1: { _invalid_opcode(instance); break; }
        case 0xC2: { _invalid_opcode(instance); break; }
        case 0xC3: { _invalid_opcode(instance); break; }
        case $_WIDE: { _wide(instance); break; }
        case 0xC5: { _invalid_opcode(instance); break; }
        case 0xC6: { _invalid_opcode(instance); break; }
        case 0xC7: { _invalid_opcode(instance); break; }
        case 0xC8: { _invalid_opcode(instance); break; }
        case 0xC9: { _invalid_opcode(instance); break; }
        case 0xCA: { _invalid_opcode(instance); break; }
        case 0xCB: { _invalid_opcode(instance); break; }
        case 0xCC: { _invalid_opcode(instance); break; }
        case 0xCD: { _invalid_opcode(instance); break; }
        case 0xCE: { _invalid_opcode(instance); break; }
        case 0xCF: { _invalid_opcode(instance); break; }
        case 0xD0: { _invalid_opcode(instance); break; }
        case 0xD1: { _invalid_opcode(instance); break; }
        case 0xD2: { _invalid_opcode(instance); break; }
        case 0xD3: { _invalid_opcode(instance); break; }
        case 0xD4: { _invalid_opcode(instance); break; }
        case 0xD5: { _invalid_opcode(instance); break; }
        case 0xD6: { _invalid_opcode(instance); break; }
        case 0xD7: { _invalid_opcode(instance); break; }
        case 0xD8: { _invalid_opcode(instance); break; }
        case 0xD9: { _invalid_opcode(instance); break; }
        case 0xDA: { _invalid_opcode(instance); break; }
        case 0xDB: { _invalid_opcode(instance); break; }
        case 0xDC: { _invalid_opcode(instance); break; }
        case 0xDD: { _invalid_opcode(instance); break; }
        case 0xDE: { _invalid_opcode(instance); break; }
        case 0xDF: { _invalid_opcode(instance); break; }
        case 0xE0: { _invalid_opcode(instance); break; }
        case 0xE1: { _invalid_opcode(instance); break; }
        case 0xE2: { _invalid_opcode(instance); break; }
        case 0xE3: { _invalid_opcode(instance); break; }
        case 0xE4: { _invalid_opcode(instance); break; }
        case 0xE5: { _invalid_opcode(instance); break; }
        case 0xE6: { _invalid_opcode(instance); break; }
        case 0xE7: { _invalid_opcode(instance); break; }
        case 0xE8: { _invalid_opcode(instance); break; }
        case 0xE9: { _invalid_opcode(instance); break; }
        case 0xEA: { _invalid_opcode(instance); break; }
        case 0xEB: { _invalid_opcode(instance); break; }
        case 0xEC: { _invalid_opcode(instance); break; }
        case 0xED: { _invalid_opcode(instance); break; }
        case 0xEE: { _invalid_opcode(instance); break; }
        case 0xEF: { _invalid_opcode(instance); break; }
        case 0xF0: { _invalid_opcode(instance); break; }
        case 0xF1: { _invalid_opcode(instance); break; }
        case 0xF2: { _invalid_opcode(instance); break; }
        case 0xF3: { _invalid_opcode(instance); break; }
        case 0xF4: { _invalid_opcode(instance); break; }
        case 0xF5: { _invalid_opcode(instance); break; }
        case 0xF6: { _invalid_opcode(instance); break; }
        case 0xF7: { _invalid_opcode(instance); break; }
        case 0xF8: { _invalid_opcode(instance); break; }
        case 0xF9: { _invalid_opcode(instance); break; }
        case 0xFA: { _invalid_opcode(instance); break; }
        case 0xFB: { _invalid_opcode(instance); break; }
        case $_IN: { _in(instance); break; }
        case $_OUT: { _out(instance); break; }
        case $_ERR: { _err(instance); break; }
        case $_HALT: { _halt(instance); break; }
    }
}

ijvm_t init_ijvm(FILE* binary, FILE* in, FILE* out) {
    uint32_t header;
    fread(&header, sizeof(uint32_t), 1, binary);
    if(header != swap_endianness_dword($_IJVM_HEADER)) {
        printf("[!] File does not have the correct header. expected: 0x1DEADFAD, got: 0x%hhX", swap_endianness_dword(header));
        exit(0);
    } else if(ferror(binary)) {
        printf("[!] Error occurred while trying to read file header (first 4 bytes)");
        exit(0);
    }
    $_DEBUG_INFO("File contains header 0x1DEADFAD");

    ijvm_t instance = { .program_counter = 0, 
             .state = $_IJVMS_OK, .state = $_IJVMS_INVALID_BINARY_FORMAT, .wide = false, .nesting = 0, .in = stdin, .out = stdout, .stack = stack_construct() };

    fread(&instance.constant_pool_origin, sizeof(uint32_t), 1, binary);
    if(ferror(binary)) {
        printf("[!] Error occurred while trying to read constant pool origin (5th to 8th bytes)");
        exit(0);
    }
    instance.constant_pool_origin = swap_endianness_dword(instance.constant_pool_origin);
    $_DEBUG_INFO("Constant Pool Origin: %d", instance.constant_pool_origin);

    fread(&instance.constant_pool_size, sizeof(uint32_t), 1, binary);
    if(ferror(binary)) {
        printf("[!] Error occurred while trying to read constant pool size (9th to 12th bytes)");
        exit(0);
    }
    instance.constant_pool_size = swap_endianness_dword(instance.constant_pool_size);
    $_DEBUG_INFO("Constant Pool Size: %d", instance.constant_pool_size);

    instance.constant_pool = malloc(instance.constant_pool_size);

    if(instance.constant_pool == NULL) {
        printf("[!] Error occured while trying to allocate memory for the constant pool (%ld bytes)", instance.constant_pool_size / sizeof(constant_t));
        exit(0);
    }

    fread(instance.constant_pool, sizeof(constant_t), instance.constant_pool_size / sizeof(constant_t), binary);
    if(ferror(binary)) {
        printf("[!] Error occurred while trying to read constant pool. bytes: [13, %d]", 13 + instance.constant_pool_size);
        exit(0);
    }
    $_DEBUG_INFO("Loaded Constant Pool (%d bytes)", instance.constant_pool_size);

    fread(&instance.text_origin, sizeof(uint32_t), 1, binary);
    if(ferror(binary)) {
        printf("[!] Error occurred while trying to read text origin. bytes: [%d, %d]", 13 + instance.constant_pool_size + 1, 13 + instance.constant_pool_size + 5);
        exit(0);
    }
    instance.text_origin = swap_endianness_dword(instance.text_origin);
    $_DEBUG_INFO("Text Origin: %d", instance.text_origin);

    fread(&instance.text_size, sizeof(uint32_t), 1, binary);
    if(ferror(binary)) {
        printf("[!] Error occurred while trying to read text size. bytes: [%d, %d]", 13 + instance.constant_pool_size + 6, 13 + instance.constant_pool_size + 10);
        exit(0);
    }
    instance.text_size = swap_endianness_dword(instance.text_size);

    $_DEBUG_INFO("Text Size: %d", instance.text_size);

    instance.text = malloc(instance.text_size);

    if(instance.text == NULL) {
        printf("[!] Error occured while trying to allocate memory for the constant pool (%d bytes)", instance.text_size);
        exit(0);
    }

    fread(instance.text, sizeof(byte_t), instance.text_size, binary);
    if(ferror(binary)) {
        printf("[!] Error occurred while trying to read text. bytes: [%d, %d]", 13 + instance.constant_pool_size + 11, 13 + instance.constant_pool_size + 13 + instance.text_size);
        exit(0);
    }
    $_DEBUG_INFO("Loaded Text (%d bytes)", instance.text_size);
    $_DEBUG("\n");

    instance.frame.call_stack_size = 0;
    instance.frame.caller = NULL;
    instance.frame.local_storage_size = 256;
    instance.frame.return_address = instance.text_size;
    instance.frame.variables = calloc(256, sizeof(byte_t));
    instance.nesting = 0;

    instance.state = (instance.text_size == 0) ? $_IJVMS_END_OF_TEXT : $_IJVMS_OK;

    return instance;
}

void destroy_ijvm(ijvm_t* instance) {
    $_DEBUG("\n");
    free(instance->constant_pool);
    $_DEBUG_INFO("Freed constant pool");
    free(instance->text);
    $_DEBUG_INFO("Freed text");

    $_DEBUG_INFO("Printing Stack:");
    $_DEBUG_LN("/--------------------------/");
    while(!stack_is_empty(&instance->stack)) {
        $_DEBUG_LN("%d", stack_pop_byte(&instance->stack));
        $_DEBUG_LN("/--------------------------/");
    }
    stack_destroy(&instance->stack);
    $_DEBUG_INFO("Freed stack memory");
    fclose(instance->in);
    $_DEBUG_INFO("Closed input file stream");
    if(instance->out != stdout) {
        fclose(instance->out);
        $_DEBUG_INFO("Closed output file stream");
    }
    $_DEBUG_INFO("Exited with state: %s", (instance->state == NULL) ? "Ok!" : instance->state);
}

void step(ijvm_t* instance) {
    _execute(instance);

    if(instance->program_counter == instance->text_size) {
        instance->state = $_IJVMS_END_OF_TEXT;
        return;
    }
}

bool can_step(ijvm_t* instance) {
    return instance->state == NULL;
}

constant_t get_constant(ijvm_t* instance, constant_name_t index) {
    return _ijvm_get_constant(instance, index);
}

#undef $_DEBUG_OPCODE