#ifndef IMAGE_PROCESSING_H
#define IMAGE_PROCESSING_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

void increase_contrast(SDL_Surface *image, double con);
void convert_to_grayscale(SDL_Surface *surface);
void convert_grayscale_to_bw(SDL_Surface* surface, Uint8 threshold);

int is_black_pixel(SDL_Surface *image,int x, int y);
void set_white_pixel(SDL_Surface *image, int x, int y);
void reduce_noise(SDL_Surface *image);

#endif
