#include <x86/dos.h>
#include <stdio.h>

void x86_Dos_Install(struct x86_CPU * cpu){
}

void x86_Dos_Load(struct x86_CPU * cpu, const char * file){
    struct Machine * machine = cpu->machine;

    FILE * f = fopen(file, "rb");
    if(f){
        unsigned char * comfile = Memory_GetAddress(&machine->memory, 0x100);

        fseek(f, 0, SEEK_END);
        long size = ftell(f);
        fseek(f, 0, SEEK_SET);

        fread(comfile, 1, size, f);
        fclose(f);

        cpu->eip = 0x100;

        return;
    }

    printf("Failed to open COM file\n");
}
