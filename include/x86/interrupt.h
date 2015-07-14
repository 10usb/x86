#ifndef INTERRUPT_H
#define INTERRUPT_H

#include <x86/cpu.h>

struct x86_CPU;
struct x86_Interrupt {
    int (*callback)(struct x86_CPU * cpu);
    unsigned char type;
    unsigned short segment;
    unsigned int offset;
};

#endif // INTERRUPT_H
