#ifndef BIOS386_H
#define BIOS386_H


#include <386/cpu386.h>

void BIOS386_Install(struct CPU386 * cpu);
void BIOS386_Load(struct CPU386 * cpu);

#endif // BIOS386_H
