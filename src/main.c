#include <SDL.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <video.h>
#include <machine.h>
#include <x86/cpu.h>
#include <x86/bios.h>
#include <x86/dos.h>

static int MachineThread(void * ptr){
    struct Machine * machine = ptr;
    if(machine->cpu) machine->cpu->run(machine->cpu);
    return 0;
}

int main(int argc, char* args[]){
	// Initialize SDL
	if(SDL_Init(SDL_INIT_VIDEO) < 0){
		printf("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
        return 1;
	}
	printf("SDL initialized\n");

    // Create window
    SDL_Window * window = SDL_CreateWindow("x86", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 640, 480, SDL_WINDOW_SHOWN);
    if(!window){
        printf("Window could not be created! SDL_Error: %s\n", SDL_GetError());
        return 1;
    }

    // Get window surface
    SDL_Surface * screen = SDL_GetWindowSurface(window);

    if(screen->format->BitsPerPixel < 32 || screen->format->format!=SDL_PIXELFORMAT_RGB888){
        printf("Required video mode not met\n");
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    // Setup video card
    struct Video video;
    Video_Init(&video);
    Video_SetResolution(&video, screen->w, screen->h);

    // Render initial screen
    SDL_LockSurface(screen);
    Video_Render(&video, screen->pixels);
    SDL_UnlockSurface(screen);


    // Load font
    FILE * f = fopen("console.font", "rb");
    if(f){
        fseek(f, 0, SEEK_END);
        long fsize = ftell(f);
        fseek(f, 0, SEEK_SET);
        unsigned char * fdata = malloc(fsize);
        fread(fdata, 1, fsize, f);
        fclose(f);

        Video_SetFont(&video, fdata, 8, 12);
        free(fdata);
        printf("Loaded font\n");
    }

    // Initialize default text mode
    Video_SetTextMode(&video, 80, 25);

    // Creating the machine
    struct Machine machine;
    memset(&machine, 0, sizeof(struct Machine));
    Machine_LoadFloppy(&machine, "floppy.img");

    Memory_Init(&machine.memory, 0x100000); // 1MB
    machine.video = &video;

    x86_CPU_Attach(&machine);
    x86_BIOS_Install((struct x86_CPU*)machine.cpu);
    //x86_BIOS_Load((struct x86_CPU*)machine.cpu);
    x86_Dos_Load((struct x86_CPU*)machine.cpu, "bounce.com");

    SDL_Thread * thread = SDL_CreateThread(MachineThread, "MachineThread", (void*)&machine);
    if(!thread) {
        printf("Failed to create machine thread\n");
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    // Main loop flag
    int running = 1;

    // Event handler
    SDL_Event e;

    // While application is running
    while(running){
        // Handle events on queue
        while(SDL_PollEvent(&e) != 0){
            // User requests quit
            if(e.type == SDL_QUIT){
                running = 0;
            }
        }



        {
            unsigned char * vimem = Memory_GetAddress(&machine.memory, 0xB8000);
            int x, y;
            for(y=0;  y < video.source.height; y++){
                for(x = 0; x < video.source.width; x++){
                    video.data[x * 2 + y * video.source.scanline + 0] = vimem[x * 2 + y * video.source.scanline + 0];
                    video.data[x * 2 + y * video.source.scanline + 1] = vimem[x * 2 + y * video.source.scanline + 1];
                }
            }

            SDL_LockSurface(screen);
            Video_Render(&video, screen->pixels);
            SDL_UnlockSurface(screen);
        }

        // Update the surface
        SDL_UpdateWindowSurface(window);
        SDL_Delay(100);
    }

	//Destroy window
	SDL_DestroyWindow(window);

    int threadReturnValue;
    SDL_WaitThread(thread, &threadReturnValue);
    printf("\nThread returned value: %d", threadReturnValue);

	SDL_Quit();
	return 0;
}
