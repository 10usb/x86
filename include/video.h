#ifndef VIDEO_H
#define VIDEO_H

union VideoRGBA {
    struct {
        unsigned char b;
        unsigned char g;
        unsigned char r;
        unsigned char a;
    };
    unsigned int value;
};

struct Video {
    int mode; // 0 Text, 1 Graphic
    unsigned char * data;
    union VideoRGBA palette[256];
    struct {
        unsigned int row;
        unsigned int column;
    } cursor;
    struct {
        unsigned char bits;
        unsigned int width;
        unsigned int height;
        unsigned int scanline;
    } source;
    struct {
        unsigned int width;
        unsigned int height;
        unsigned int scanline;
    } resolution;
    struct {
        unsigned char * data;
        unsigned int width;
        unsigned int height;
        unsigned int scanline;
        union VideoRGBA palette[16];
    } font;
};

void Video_Init(struct Video * video);
void Video_SetFont(struct Video * video, const unsigned char * data, int width, int height);
void Video_SetResolution(struct Video * video, int width, int height);
int Video_SetTextMode(struct Video * video, int width, int height);
int Video_SetGraphicMode(struct Video * video, int width, int height);
int Video_Update(struct Video * video, const unsigned char * source);
void Video_Render(struct Video * video, unsigned char * target);



#endif // VIDEO_H
