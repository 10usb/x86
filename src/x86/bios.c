#include <x86/bios.h>
#include <memory.h>
#include <video.h>

int x86_BIOS_Int10(struct x86_CPU * cpu){
    struct Video * video = cpu->machine->video;

    switch(cpu->ah){
        case 0x0E:{
            unsigned char * data = Memory_GetAddress(&cpu->machine->memory, 0xB8000);

            if(cpu->al==10){
                video->cursor.row++;
            }else if(cpu->al==13){
                video->cursor.column = 0;
            }else{
                data[video->cursor.column * 2 + video->cursor.row * video->source.scanline + 0] = cpu->al;
                data[video->cursor.column * 2 + video->cursor.row * video->source.scanline + 1] = cpu->bl;
                video->cursor.column++;
            }

            if(video->cursor.column >= video->source.width){
                video->cursor.column = 0;
                video->cursor.row++;
            }
            if(video->cursor.row >= video->source.height){
                video->cursor.row = 0;
            }

            return 0;
        }break;
    }
    return 1;
}

void x86_BIOS_Install(struct x86_CPU * cpu){
    x86_CPU_SetInterrupt(cpu, 0, 0x10, x86_BIOS_Int10);
}

void x86_BIOS_Load(struct x86_CPU * cpu){
    struct Machine * machine = cpu->machine;
    unsigned char * bootcode = Memory_GetAddress(&machine->memory, 0x7C00);

    if(machine->floppy){
        fseek(machine->floppy, 0, SEEK_SET);
        fread(bootcode, 1, 512, machine->floppy);

        if(bootcode[0x1FE]==0x55 && bootcode[0x1FF]==0xAA){
            cpu->eip = 0x7C00;
            return;
        }
    }

    printf("No bootable media found\n");
}
