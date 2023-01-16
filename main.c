#include <stdio.h>
#include <stdlib.h>
#include "ijvm.h"
#include "machine.h"
#include "util.h"
#include "stack.h"

int main(int argv, char **argc)
{
    if(argv != 2) {
        $_DEBUG_ERROR("Unexpected command line parameter count given: expected 1, got %d\n", (argv == 0) ? 0 : argv - 1);
        return 0;
    }

    FILE* file = fopen(argc[1], "rb");

    if(!file) {
        $_DEBUG_ERROR("Couldn't open file: %s", argc[1]);
        return 0;
    }

    uint32_t header;
    fread(&header, sizeof(uint32_t), 1, file);
    header = swapEndianness(header);
    $_DEBUG_INFO("Header: 0x%x. Reading binary...", header);

    if(header != $_IJVM_HEADER) {
        $_DEBUG_ERROR("File is not a JAS file: expected header \"" $_STRINGIFY($_JAS_HEADER) "\", got header \"0x%hhx\"", header);
        return 0;
    }

    machine ijvm = init_ijvm(file);

    while(!is_end_of_program(&ijvm)) {
        step(&ijvm);
    }

    fclose(file);
    $_DEBUG_INFO("Closed file");

    destroy_ijvm(&ijvm);
    return 0;
}