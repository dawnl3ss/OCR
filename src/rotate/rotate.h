#ifndef ROTATE_H
#define ROTATE_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <stdio.h>
#include <math.h>
#include <string.h>

double detectAngle(SDL_Surface *image);
SDL_Surface *rotateSurface(SDL_Surface *src, double angle);

#endif