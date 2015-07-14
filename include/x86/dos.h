#ifndef X86_DOS_H
#define X86_DOS_H


#include <x86/cpu.h>

void x86_Dos_Install(struct x86_CPU * cpu);
void x86_Dos_Load(struct x86_CPU * cpu, const char * file);

#endif // X86_DOS_H
