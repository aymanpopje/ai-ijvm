#if !defined(IJVM_H)
#define IJVM_H

#include <stdint.h>
#include "util.h"

#define $_IJVM_HEADER 0x1deadfad

#define $_IJVM_OPCODE(code) ((byte_t) code)

#define $_IJVM_BIPUSH        $_IJVM_OPCODE(0x10)
#define $_IJVM_DUP           $_IJVM_OPCODE(0x59)
#define $_IJVM_ERR           $_IJVM_OPCODE(0xFE)
#define $_IJVM_GOTO          $_IJVM_OPCODE(0xA7)
#define $_IJVM_HALT          $_IJVM_OPCODE(0xFF)
#define $_IJVM_IADD          $_IJVM_OPCODE(0x60)
#define $_IJVM_IAND          $_IJVM_OPCODE(0x7E)
#define $_IJVM_IFEQ          $_IJVM_OPCODE(0x99)
#define $_IJVM_IFLT          $_IJVM_OPCODE(0x9B)
#define $_IJVM_IF_ICMPEQ     $_IJVM_OPCODE(0x9F)
#define $_IJVM_IINC          $_IJVM_OPCODE(0x84)
#define $_IJVM_ILOAD         $_IJVM_OPCODE(0x15)
#define $_IJVM_IN            $_IJVM_OPCODE(0xFC)
#define $_IJVM_INVOKEVIRTUAL $_IJVM_OPCODE(0xB6)
#define $_IJVM_IOR           $_IJVM_OPCODE(0x80)
#define $_IJVM_IRETURN       $_IJVM_OPCODE(0xAC)
#define $_IJVM_ISTORE        $_IJVM_OPCODE(0x36)
#define $_IJVM_ISUB          $_IJVM_OPCODE(0x64)
#define $_IJVM_LDC_W         $_IJVM_OPCODE(0x13)
#define $_IJVM_NOP           $_IJVM_OPCODE(0x00)
#define $_IJVM_OUT           $_IJVM_OPCODE(0xFD)
#define $_IJVM_POP           $_IJVM_OPCODE(0x57)
#define $_IJVM_SWAP          $_IJVM_OPCODE(0x5F)
#define $_IJVM_WIDE          $_IJVM_OPCODE(0xC4)

#endif