#include <x86/dasm.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

void x86_DASM_Read(struct x86_Opcode * opcodes, FILE * f){
    char buffer[256];
    char * command;
    struct x86_OpcodeInfo * info;

    long opcode = -1;

    while(fgets(buffer, 256, f)!=0){
        command = buffer;

        while(command[0]==9 || command[0]==10 || command[0]==13 || command[0]==32) command++;

        long length = strlen(command);
        while(length > 0 && (command[length-1]==9 || command[length-1]==10 || command[length-1]==13 || command[length-1]==32)){
            command[length-- -1] = 0;
        }

        if(strncasecmp(command, "OPCODE ", 7)==0){
            opcode = strtol(command + 7, 0, 16);
            if(opcode < 0x00 || opcode > 0xff){
                opcode = -1;
            }else if(opcodes[opcode].info==0){
                opcodes[opcode].info = malloc(sizeof(struct x86_OpcodeInfo));
                memset(opcodes[opcode].info, 0, sizeof(struct x86_OpcodeInfo));
            }
        }else if(opcode >= 0x00 && opcode <= 0xff){
            info = opcodes[opcode].info;

            if(strncasecmp(command, "LABEL ", 6)==0){
                if(info->label==0){
                    info->label = strcpy(malloc(length + 1), command + 6);
                }
            }else if(strncasecmp(command, "ARGS ", 5)==0){
                int start = 5, end, index = 0;

                while(index < 3 && start < length){
                    while(start < length && (command[start]==9 || command[start]==10 || command[start]==13 || command[start]==32)) start++;
                    end = start;
                    while(end + 1 < length && command[end + 1]!=',') end++;

                    if(strncasecmp(command + start, "R:", 2)==0){
                        info->args[index].flags|= X86_OPCODE_ARG_R;
                        if(info->args[index].value==0){
                            info->args[index].value = strncpy(malloc((end - (start + 2) + 1) + 1), command + start + 2, (end - (start + 2) + 1) + 1);
                            info->args[index].value[end - (start + 2) + 1] = 0;
                        }
                    }else if(strncasecmp(command + start, "R8:", 3)==0){
                        info->args[index].flags|= X86_OPCODE_ARG_R|X86_OPCODE_ARG_8;
                        if(info->args[index].value==0){
                            info->args[index].value = strncpy(malloc((end - (start + 3) + 1) + 1), command + start + 3, (end - (start + 3) + 1) + 1);
                            info->args[index].value[end - (start + 3) + 1] = 0;
                        }
                    }else if(strncasecmp(command + start, "R16:", 4)==0){
                        info->args[index].flags|= X86_OPCODE_ARG_R|X86_OPCODE_ARG_16;
                        if(info->args[index].value==0){
                            info->args[index].value = strncpy(malloc((end - (start + 4) + 1) + 1), command + start + 4, (end - (start + 4) + 1) + 1);
                            info->args[index].value[end - (start + 4) + 1] = 0;
                        }
                    }else if(strncasecmp(command + start, "R32:", 4)==0){
                        info->args[index].flags|= X86_OPCODE_ARG_R|X86_OPCODE_ARG_32;
                        if(info->args[index].value==0){
                            info->args[index].value = strncpy(malloc((end - (start + 4) + 1) + 1), command + start + 4, (end - (start + 4) + 1) + 1);
                            info->args[index].value[end - (start + 4) + 1] = 0;
                        }
                    }else if(strncasecmp(command + start, "R8", 2)==0){
                        info->args[index].flags|= X86_OPCODE_ARG_R|X86_OPCODE_ARG_8;
                    }else if(strncasecmp(command + start, "R16", 3)==0){
                        info->args[index].flags|= X86_OPCODE_ARG_R|X86_OPCODE_ARG_16;
                    }else if(strncasecmp(command + start, "R32", 3)==0){
                        info->args[index].flags|= X86_OPCODE_ARG_R|X86_OPCODE_ARG_32;
                    }else if(strncasecmp(command + start, "RM8", 3)==0){
                        info->args[index].flags|= X86_OPCODE_ARG_R|X86_OPCODE_ARG_M|X86_OPCODE_ARG_8;
                    }else if(strncasecmp(command + start, "RM16", 4)==0){
                        info->args[index].flags|= X86_OPCODE_ARG_R|X86_OPCODE_ARG_M|X86_OPCODE_ARG_16;
                    }else if(strncasecmp(command + start, "RM32", 4)==0){
                        info->args[index].flags|= X86_OPCODE_ARG_R|X86_OPCODE_ARG_M|X86_OPCODE_ARG_32;
                    }else if(strncasecmp(command + start, "I8", 2)==0){
                        info->args[index].flags|= X86_OPCODE_ARG_I|X86_OPCODE_ARG_8;
                    }else if(strncasecmp(command + start, "I16", 3)==0){
                        info->args[index].flags|= X86_OPCODE_ARG_I|X86_OPCODE_ARG_16;
                    }else if(strncasecmp(command + start, "I32", 3)==0){
                        info->args[index].flags|= X86_OPCODE_ARG_I|X86_OPCODE_ARG_32;
                    }else if(strncasecmp(command + start, "RM", 2)==0){
                        info->args[index].flags|= X86_OPCODE_ARG_R|X86_OPCODE_ARG_M;
                    }else if(strncasecmp(command + start, "R", 1)==0){
                        info->args[index].flags|= X86_OPCODE_ARG_R;
                    }else if(strncasecmp(command + start, "M", 1)==0){
                        info->args[index].flags|= X86_OPCODE_ARG_M;
                    }else if(strncasecmp(command + start, "I", 1)==0){
                        info->args[index].flags|= X86_OPCODE_ARG_I;
                    }

                    start = end + 2;
                    index++;
                }

            }else if(strncasecmp(command, "TABLE", 5)==0){
                if(opcodes[opcode].opcodes==0){
                    opcodes[opcode].opcodes = x86_Opcode_Create(256);
                } // else pray it's large enough ^^

                x86_DASM_Read(opcodes[opcode].opcodes, f);
            }else if(strncasecmp(command, "ENDTABLE", 8)==0){
                return;
            }else{
                if(length > 0) printf("[%02X] '%s'\n", (unsigned int)opcode, command);
            }
        }
    }

}

int x86_DASM_Load(struct x86_Opcode * opcodes, const char * file){
    FILE * f = fopen(file, "rb");
    if(!f) return 1;

    x86_DASM_Read(opcodes, f);

    fclose(f);
    return 0;
}

int x86_DASM_Disassemble(const struct x86_Opcode * opcodes, int addresssize, int operandsize, const unsigned char * data, char * buffer, int size){
    int length = 0;
    const struct x86_Opcode * opcode;

    do {
        opcode = &opcodes[*(data + length++)];
        if(!opcode->info) return 0;

        if(!opcode->info->label) return 0;
        snprintf(buffer, size, "%s", opcode->info->label);


        if((opcode->info->args[0].flags|opcode->info->args[1].flags) & X86_OPCODE_ARG_M){
            //snprintf(buffer + strlen(buffer), size, " rm, r%d", 1);
        }

        int index = 0;

        while(index < 3){
            if(opcode->info->args[index].flags & X86_OPCODE_ARG_R && opcode->info->args[index].value){
                int readsize = opcode->info->args[index].flags & (X86_OPCODE_ARG_8|X86_OPCODE_ARG_16|X86_OPCODE_ARG_32);
                if(!readsize) readsize = operandsize;

                snprintf(buffer + strlen(buffer), size, index > 0 ? ", %s(%d)" : " %s(%d)", opcode->info->args[index].value, readsize);
            }else if(opcode->info->args[index].flags & X86_OPCODE_ARG_I){
                int readsize = opcode->info->args[index].flags & (X86_OPCODE_ARG_8|X86_OPCODE_ARG_16|X86_OPCODE_ARG_32);
                if(!readsize) readsize = operandsize;

                unsigned int value;
                switch(readsize){
                    case X86_OPCODE_ARG_8:
                        value = *((unsigned char *)(data + length));
                        length+=1;
                    break;
                    case X86_OPCODE_ARG_16:
                        value = *((unsigned short *)(data + length));
                        length+=2;
                    break;
                    case X86_OPCODE_ARG_32:
                        value = *((unsigned int *)(data + length));
                        length+=4;
                    break;
                    default: return 0;
                }
                snprintf(buffer + strlen(buffer), size, index > 0 ? ", 0x%02X" : " 0x%02X", value);

            }
            index++;
        }
    }while(0);

    return length;
}
