#ifndef MAIN_H
#define MAIN_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <time.h>
#include "predict_network.h"

#define IMG_WIDTH 28
#define IMG_HEIGHT 28
#define MAX_FILES 10000

void load_image(const char* filename, float* input_data, int width, int height);
int write_result_to_file(const char* filename, const char* result, int count, const char* entry);
void delete_file(const char* filename);


#endif