#include "386/bios.h"
#include <memory.h>

int BIOS386_Int10(struct CPU386 * cpu){
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

void BIOS386_Install(struct CPU386 * cpu){
    CPU386_SetInt(cpu, 0, 0x10, BIOS386_Int10);
}

void BIOS386_Load(struct CPU386 * cpu){
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
