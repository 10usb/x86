#ifndef x86_BIOS_H
#define x86_BIOS_H


#include <x86/cpu.h>

void x86_BIOS_Install(struct x86_CPU * cpu);
void x86_BIOS_Load(struct x86_CPU * cpu);

#endif // x86_BIOS_H
