#include "util.h"

dword_t swap_endianness_dword(dword_t num) {
    return ((num >> 24) & 0xff)      |
           ((num << 8 ) & 0xff0000)  |
           ((num >> 8 ) & 0xff00)    |
           ((num << 24) & 0xff000000);
}

word_t swap_endianness_word(word_t num) {
    return (num >> 8) | (num << 8);
}