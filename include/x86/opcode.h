#ifndef X86_OPCODE_H
#define X86_OPCODE_H

#include <x86/cpu.h>

#define X86_OPCODE_ARG_R    0x0001
#define X86_OPCODE_ARG_M    0x0002
#define X86_OPCODE_ARG_I    0x0004
#define X86_OPCODE_ARG_8    0x0008
#define X86_OPCODE_ARG_16   0x0010
#define X86_OPCODE_ARG_32   0x0020

#define X86_OPCODE_ARG_S    0x0040

struct x86_OpcodeInfo {
    char * label;
    unsigned int flags;
    struct {
        unsigned int flags;
        char * value;
    } args[3];
};

struct x86_CPU;
struct x86_Opcode {
    unsigned char code;
    int (*callback)(struct x86_Opcode * opcode, struct x86_CPU * cpu);
    struct x86_Opcode * opcodes;
    struct x86_OpcodeInfo * info;
};

struct x86_Opcode *  x86_Opcode_Create(int size);
int x86_InvalidOpcode(struct x86_Opcode * opcode, struct x86_CPU * cpu);

#endif // X86_OPCODE_H
