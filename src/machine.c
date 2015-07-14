#include "machine.h"


void Machine_LoadFloppy(struct Machine * machine, const char * file){
    machine->floppy = fopen(file, "rb+");
}
