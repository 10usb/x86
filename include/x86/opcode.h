#ifndef OPCODE_H
#define OPCODE_H

#include <x86/cpu.h>

struct x86_CPU;
struct x86_Opcode {
    unsigned char code;
    int (*callback)(struct x86_Opcode * opcode, struct x86_CPU * cpu);
    struct x86_Opcode * opcodes;
};

#endif // OPCODE_H
