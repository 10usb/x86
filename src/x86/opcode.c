#include "opcode.h"

int x86_InvalidOpcode(struct x86_Opcode * opcode, struct x86_CPU * cpu){
    printf("Invalid opcode 0x%X\n", opcode->code);
    return 1;
}
