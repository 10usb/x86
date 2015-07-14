#include <x86/dos.h>

void x86_Dos_Install(struct x86_CPU * cpu){
}

void x86_Dos_Load(struct x86_CPU * cpu, const char * file){
    struct Machine * machine = cpu->machine;
    unsigned char * comfile = Memory_GetAddress(&machine->memory, 0x100);

    printf("Failed to open COM file\n");
}
