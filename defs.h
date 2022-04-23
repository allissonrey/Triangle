#ifndef DEFS_H
#define DEFS_H

#define WIDTH 640
#define HEIGHT 480

#include <SDL2/SDL_stdinc.h>

void setattrb(void);

void setColor(Uint32 color);

void deleteShaders(unsigned int vs, unsigned int fs);


#endif