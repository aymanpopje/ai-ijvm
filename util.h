#if !defined(UTIL_H)
#define UTIL_H

#include <stdint.h>
#include <assert.h>
#include <stdio.h>

typedef uint8_t byte_t;
typedef uint32_t word_t;
typedef uint64_t dword_t;

typedef int8_t ibyte_t;
typedef int32_t iword_t;
typedef int64_t idword_t;

typedef word_t label_name_t;
typedef byte_t var_name_t;
typedef word_t method_name_t;
typedef word_t constant_name_t;
typedef dword_t constant_t;

#define $_STRINGIFY_DIRECT(...) #__VA_ARGS__
#define $_STRINGIFY(...) $_STRINGIFY_DIRECT(__VA_ARGS__)

#ifndef NDEBUG
#define $_DEBUG(fmt, ...) printf(fmt __VA_OPT__(,) __VA_ARGS__)
#else
#define $_DEBUG(...)
#endif

#define $_DEBUG_LN(fmt, ...) $_DEBUG(fmt "\n" __VA_OPT__(,) __VA_ARGS__)

#define $_DEBUG_INFO(fmt, ...) $_DEBUG_LN("[i] " fmt __VA_OPT__(,) __VA_ARGS__)
#define $_DEBUG_ERROR(fmt, ...) $_DEBUG_LN("[!] " fmt __VA_OPT__(,) __VA_ARGS__)

#define $_ASSERT(condition, message) assert((condition) && message)

uint32_t swapEndianness(uint32_t num);

#endif