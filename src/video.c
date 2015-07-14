#include "video.h"
#include <string.h>
#include <stdlib.h>

void Video_Init(struct Video * video){
    memset(video, 0, sizeof(struct Video));
    video->mode = -1;

    video->font.palette[0x0].value = 0x000000;
    video->font.palette[0x1].value = 0x0000AA;
    video->font.palette[0x2].value = 0x00AA00;
    video->font.palette[0x3].value = 0x00AAAA;
    video->font.palette[0x4].value = 0xAA0000;
    video->font.palette[0x5].value = 0xAA00AA;
    video->font.palette[0x6].value = 0xAA5500;
    video->font.palette[0x7].value = 0xAAAAAA;
    video->font.palette[0x8].value = 0x555555;
    video->font.palette[0x9].value = 0x5555FF;
    video->font.palette[0xA].value = 0x55FF55;
    video->font.palette[0xB].value = 0x55FFFF;
    video->font.palette[0xC].value = 0xFF5555;
    video->font.palette[0xD].value = 0xFF55FF;
    video->font.palette[0xE].value = 0xFFFF55;
    video->font.palette[0xF].value = 0xFFFFFF;
}

void Video_SetFont(struct Video * video, const unsigned char * data, int width, int height){
    video->font.width       = width;
    video->font.height      = height;
    video->font.scanline    = width * 16;

    if(video->font.data) free(video->font.data);
    video->font.data        = malloc(width * height * 256);
    memcpy(video->font.data, data, width * height * 256);

    // TODO check if current mode is still valid and/or padding needs to be adjusted
}

void Video_SetResolution(struct Video * video, int width, int height){
    video->resolution.width     = width;
    video->resolution.height    = height;
    video->resolution.scanline  = width * 4;

    // TODO check if current mode is still valid and/or padding needs to be adjusted
}

int Video_SetTextMode(struct Video * video, int width, int height){
    // TODO check if target mode is valid otherwise return 0

    video->mode = 0;
    video->source.bits      = 16;
    video->source.width     = width;
    video->source.height    = height;
    video->source.scanline  = width * 2;

    if(video->data) free(video->data);
    video->data = malloc(video->source.scanline * video->source.height);

    int x, y;
    for(y=0;  y < video->source.height; y++){
        for(x = 0; x < video->source.width; x++){
            video->data[x * 2 + y * video->source.scanline + 0] = 0x00;
            video->data[x * 2 + y * video->source.scanline + 1] = 0x0F;
        }
    }
    // TODO adjust padding

    return 1;
}

int Video_SetGraphicMode(struct Video * video, int width, int height){
    return 0;
}

int Video_Update(struct Video * video, const unsigned char * source){
    return 0;
}

inline void Video_Printchar(struct Video * video, unsigned char c, unsigned char color, int tx, int ty, unsigned char * target){
    int x = 0, y = 0;
    int sx = (c % 16) * video->font.width;
    int sy = (c / 16) * video->font.height * video->font.scanline;

    int to = 0;
    tx = tx * video->font.width;
    ty = ty * video->font.height;

    union VideoRGBA foreground = video->font.palette[color & 0xf];
    union VideoRGBA background = video->font.palette[(color >> 4) & 0xf];

    while(y < video->font.height){
        x = 0;
        while(x < video->font.width){
            to = (tx + x) * 4 + (ty + y) * video->resolution.scanline;

            if(!video->font.data[(sx + x) + (sy + y * video->font.scanline)]){
                target[to]       = (((tx + x) * 0xff / 640) % 0xff / 4) + (background.b / 4 * 3);
                target[to + 1]   = (((tx + x) + (ty + y)) % 0xff / 4) + (background.g / 4 * 3);
                target[to + 2]   = (((ty + y) * 0xff / 480) % 0xff / 4) + (background.r / 4 * 3);
            }else{
                target[to]       = foreground.b;
                target[to + 1]   = foreground.g;
                target[to + 2]   = foreground.r;
            }
            x++;
        }
        y++;
    }
}

void Video_Render(struct Video * video, unsigned char * target){
    switch(video->mode){
        case 0:{
            int x, y;
            for(y=0;  y < video->source.height; y++){
                for(x = 0; x < video->source.width; x++){
                    Video_Printchar(video, video->data[x * 2 + y * video->source.scanline + 0], video->data[x * 2 + y * video->source.scanline + 1], x, y, target);
                }
            }
        }break;
        default:{
            int x, y;
            for(y=0;  y < video->resolution.height; y++){
                for(x = 0; x < video->resolution.width; x++){
                    target[x * 4 + y * video->resolution.scanline + 0] = (x * 0xff / video->resolution.width) % 0xff;
                    target[x * 4 + y * video->resolution.scanline + 1] = (x + y) % 0xff;
                    target[x * 4 + y * video->resolution.scanline + 2] = (y * 0xff / video->resolution.height) % 0xff;
                }
            }
        }break;
    }
}

