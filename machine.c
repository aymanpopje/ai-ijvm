#include "machine.h"
#include "ijvm.h"
#include "util.h"
#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>

byte_t next_text_byte(machine* instance) {
    byte_t next = *(instance->text + instance->textOffset);
    instance->textOffset += sizeof(byte_t);
    return next;
}

word_t next_text_word(machine* instance) {
    word_t next = *((word_t*) (instance->text + instance->textOffset));
    instance->textOffset += sizeof(word_t);
    return next;
}

machine init_ijvm(FILE* file) {
    machine machine;
    machine.stack = stack_construct();

    fread(&machine.constantPoolOrigin, sizeof(uint32_t), 1, file);
    machine.constantPoolOrigin = swapEndianness(machine.constantPoolOrigin);
    $_DEBUG_INFO("Constant Pool Offset: %u", machine.constantPoolOrigin);

    fread(&machine.constantPoolSize, sizeof(uint32_t), 1, file);
    machine.constantPoolSize = swapEndianness(machine.constantPoolSize);
    $_DEBUG_INFO("Constant Pool Size: %u", machine.constantPoolSize);

    machine.constantPool = malloc(machine.constantPoolSize);
    fread(machine.constantPool, sizeof(byte_t), machine.constantPoolSize, file);
    $_DEBUG_INFO("Constant Pool Read. Read %u bytes.", machine.constantPoolSize);

    fread(&machine.textOrigin, sizeof(uint32_t), 1, file);
    machine.textOrigin = swapEndianness(machine.textOrigin);
    $_DEBUG_INFO("Read Text Offset: %u", machine.textOrigin);

    fread(&machine.textSize, sizeof(uint32_t), 1, file);
    machine.textSize = swapEndianness(machine.textSize);
    $_DEBUG_INFO("Read Text Size: %u", machine.textSize);

    machine.text = malloc(machine.textSize);
    fread(machine.text, sizeof(byte_t), machine.textSize, file);
    $_DEBUG_INFO("Read Instructions: %u bytes", machine.textSize);

    machine.baseTextOffset = 0;
    machine.textOffset = 0;

    return machine;
}

void destroy_ijvm(machine* instance) {
    free(instance->constantPool);
    $_DEBUG_INFO("Freed Constant Pool Memory");
    free(instance->text);
    $_DEBUG_INFO("Freed Instructions Pool");
    stack_destroy(&instance->stack);
    $_DEBUG_INFO("Freed Stack");
}

bool is_end_of_program(machine* instance) {
    return instance->textSize == instance->textOffset;
}

void _machine_push_byte(machine* instance, byte_t byte) {
    stack_push_byte(&instance->stack, byte);
    instance->baseStackOffset += sizeof(byte_t);
}

void _machine_push_word(machine* instance, word_t word) {
    stack_push_word(&instance->stack, word);
    instance->baseStackOffset += sizeof(word_t);
}

void _machine_push_dword(machine* instance, dword_t dword) {
    stack_push_dword(&instance->stack, dword);
    instance->baseStackOffset += sizeof(dword_t);
}

byte_t _machine_pop_byte(machine* instance) {
    instance->baseStackOffset -= sizeof(byte_t);
    return stack_pop_byte(&instance->stack);
}

word_t _machine_pop_word(machine* instance) {
    instance->baseStackOffset -= sizeof(word_t);
    return stack_pop_word(&instance->stack);
}

constant_t _machine_get_constant(machine* instance, word_t index) {
    $_ASSERT(index >= instance->constantPoolSize / sizeof(constant_t), "Constant Index Out of bounds");
    return instance->constantPool[index];
}

#define $_NO_IMPL(func) $_ASSERT(false, #func " not implemented");

void _bipush(machine* instance) {
    $_DEBUG_CALL(BIPUSH);
    _machine_push_byte(instance, next_text_byte(instance));
}

void _dup(machine* instance) {
    $_DEBUG_CALL(DUP);
    word_t top = _machine_pop_word(instance);
    _machine_push_word(instance, top);
    _machine_push_word(instance, top);
}

void _err(machine* instance) {
    $_DEBUG_CALL(ERR);
    exit(-1); // TODO Add exit handler for custom error codes
}

void _goto_impl(machine* instance) {
    word_t top = _machine_pop_word(instance);
    instance->textOffset--;
    instance->textOffset += top;
}

void _goto(machine* instance) {
    $_DEBUG_CALL(GOTO);
    _goto_impl(instance);
}

void _halt(machine* instance) {
    $_DEBUG_CALL(HALT);
    exit(0);
}

void _iadd(machine* instance) {
    $_DEBUG_CALL(IADD);
    _machine_push_word(instance, swapEndianness(swapEndianness(_machine_pop_word(instance)) + swapEndianness(_machine_pop_word(instance))));
}

void _iand(machine* instance) {
    $_DEBUG_CALL(IAND);
    _machine_push_word(instance, _machine_pop_word(instance) & _machine_pop_word(instance));
}

void _if_icmpeq(machine* instance) {
    $_DEBUG_CALL(IF_ICMPEQ);
    if(_machine_pop_word(instance) == _machine_pop_word(instance)) {
        _goto_impl(instance);
    }
}

void _ifeq(machine* instance) {
    $_DEBUG_CALL(IFEQ);
    if(_machine_pop_word(instance) == 0) {
        _goto_impl(instance);
    }
}

void _iflt(machine* instance) {
    $_DEBUG_CALL(IFLT);
    if((iword_t)swapEndianness(_machine_pop_word(instance)) < 0) {
        _goto_impl(instance);
    }
}

void _iinc(machine* instance) {
    $_DEBUG_CALL(IINC);
    $_NO_IMPL(_iinc);
}

void _iload(machine* instance) {
    $_DEBUG_CALL(ILOAD);
    $_NO_IMPL(_iload);
}

void _in(machine* instance) {
    $_DEBUG_CALL(IN);
    _machine_push_byte(instance, getchar());
}

void _invokevirtual(machine* instance) {
    $_DEBUG_CALL(INVOKEVIRTUAL);
    $_NO_IMPL(_invokevirtual);
}

void _ior(machine* instance) {
    $_DEBUG_CALL(IOR);
    _machine_push_word(instance, _machine_pop_word(instance) | _machine_pop_word(instance));
}

void _ireturn(machine* instance) {
    $_DEBUG_CALL(IRETURN);
    $_NO_IMPL(_ireturn);
}

void _istore(machine* instance) {
    $_DEBUG_CALL(ISTORE);
    $_NO_IMPL(_istore);
}

void _isub(machine* instance) {
    $_DEBUG_CALL(ISUB);
    _machine_push_word(instance, swapEndianness(swapEndianness(_machine_pop_word(instance)) - swapEndianness(_machine_pop_word(instance))));
}

void _ldc_w(machine* instance) {
    $_DEBUG_CALL(LDC_W);
    _machine_push_dword(instance, _machine_get_constant(instance, _machine_pop_word(instance)));
}

void _nop(machine* instance) {
    $_DEBUG_CALL(NOP);
}

void _out(machine* instance) {
    $_DEBUG_CALL(OUT);
    printf("%d\n", (iword_t)_machine_pop_word(instance));
}

void _pop(machine* instance) {
    $_DEBUG_CALL(POP);
    _machine_pop_word(instance);
}

void _swap(machine* instance) {
    $_DEBUG_CALL(SWAP);
    word_t top0 = _machine_pop_word(instance);
    word_t top1 = _machine_pop_word(instance);
    _machine_push_word(instance, top0);
    _machine_push_word(instance, top1);
}

void _wide(machine* instance) {
    $_DEBUG_CALL(WIDE);
    instance->wide = true;
}

void step(machine* instance) {
    switch(next_text_byte(instance)) {
        case $_IJVM_BIPUSH:         _bipush(instance); break;
        case $_IJVM_DUP:            _dup(instance); break;
        case $_IJVM_ERR:            _err(instance); break;
        case $_IJVM_GOTO:           _goto(instance); break;
        case $_IJVM_HALT:           _halt(instance); break;
        case $_IJVM_IADD:           _iadd(instance); break;
        case $_IJVM_IAND:           _iand(instance); break;
        case $_IJVM_IF_ICMPEQ:      _if_icmpeq(instance); break;
        case $_IJVM_IFEQ:           _ifeq(instance); break;
        case $_IJVM_IFLT:           _iflt(instance); break;
        case $_IJVM_IINC:           _iinc(instance); break;
        case $_IJVM_ILOAD:          _iload(instance); break;
        case $_IJVM_IN:             _in(instance); break;
        case $_IJVM_INVOKEVIRTUAL:  _invokevirtual(instance); break;
        case $_IJVM_IOR:            _ior(instance); break;
        case $_IJVM_IRETURN:        _ireturn(instance); break;
        case $_IJVM_ISTORE:         _istore(instance); break;
        case $_IJVM_ISUB:           _isub(instance); break;
        case $_IJVM_LDC_W:          _ldc_w(instance); break;
        case $_IJVM_NOP:            _nop(instance); break;
        case $_IJVM_OUT:            _out(instance); break;
        case $_IJVM_POP:            _pop(instance); break;
        case $_IJVM_SWAP:           _swap(instance); break;
        case $_IJVM_WIDE:           _wide(instance); break;
    }
}

void stepWide(machine* instance) {
    instance->wide = false;
    $_ASSERT(true, "Wide not implemented");
}