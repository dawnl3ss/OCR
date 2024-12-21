#ifndef GRID_DETECTION_H
#define GRID_DETECTION_H

#include <stdlib.h>
#include <stdio.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#define MAX_LABELS 10000
#define SPACING_THRESHOLD 4

typedef struct {
    int x_min, y_min;
    int x_max, y_max;
} Box;

int is_white(Uint8 r, Uint8 g, Uint8 b);
Box detect_grid(Box *boxes, int box_count);
void detect_letters(SDL_Surface *image, Box *boxes, int *box_count);
void is_grid_letter_grid(Box *boxes, int box_count, Box grid_box, int *Is_grid, int tolerance);
void Image_creator(Box box, const char *dossier, SDL_Surface *image, int x,int y);
void adjust_grid_box_with_tolerance(Box *grid_box, int tolerance, int image_width, int image_height);
int get_largeur(Box *boxes, int *Is_grid);
int get_wide_word(Box *boxes, int *Is_grid,int i);
void draw_boxes(SDL_Surface *image, Box *boxes, int box_count, Box grid_box);

#endif 