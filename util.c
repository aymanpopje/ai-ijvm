#include "util.h"

uint32_t swapEndianness(uint32_t num) {
    return ((num >> 24) & 0xff)      |
           ((num << 8 ) & 0xff0000)  |
           ((num >> 8 ) & 0xff00)    |
           ((num << 24) & 0xff000000);
}