#include <stdio.h>
#include <stdlib.h>
#include "ijvm.h"
#include "util.h"
#include "stack.h"

int main(int argv, char **argc)
{
    if(argv != 2) {
        printf("[!] Unexpected command line parameter count given: expected 1, got %d\n", (argv == 0) ? 0 : argv - 1);
        return 0;
    }

    FILE* file = fopen(argc[1], "rb");

    if(!file) {
        printf("[!] Couldn't open file: %s", argc[1]);
        return 0;
    }

    ijvm_t instance = init_ijvm(file, stdin, stdout);

    while(can_step(&instance)) {
        step(&instance);
    }

    destroy_ijvm(&instance); 

    return 0;
}