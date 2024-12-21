#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <time.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#include "main.h"

#include "predict_network.h"

#define IMG_WIDTH 28
#define IMG_HEIGHT 28
#define MAX_FILES 10000

void delete_file(const char* filename)
{
    FILE* file = fopen(filename, "w");
    if (!file) {
        perror("Failed to open output file");
        //return EXIT_FAILURE;
    }
    fclose(file);
}

void load_image(const char* filename, float* input_data, int width, int height)
{
    if (SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        printf("SDL could not initialize. SDL_Error: %s\n", SDL_GetError());
        exit(1);
    }

    SDL_Surface* img = IMG_Load(filename);
    if (!img)
    {
        printf("Unable to load image %s. SDL_image Error: %s\n", filename, 
               IMG_GetError());
        SDL_Quit();
        exit(1);
    }

    SDL_Surface* resized_img = SDL_CreateRGBSurface(0, width, height, 32, 
                                                     0x00FF0000, 0x0000FF00, 
                                                     0x000000FF, 0xFF000000);
    SDL_BlitScaled(img, NULL, resized_img, NULL);

    Uint32* pixels = (Uint32*)resized_img->pixels;
    for (int i = 0; i < height; ++i)
    {
        for (int j = 0; j < width; ++j)
        {
            Uint32 pixel = pixels[i * width + j];
            Uint8 r, g, b;
            SDL_GetRGB(pixel, resized_img->format, &r, &g, &b);
            input_data[i * width + j] = (r + g + b) / (3.0 * 255.0);
        }
    }

    SDL_FreeSurface(img);
    SDL_FreeSurface(resized_img);
    SDL_Quit();
}

int write_result_to_file(const char* filename, const char* result, int count, 
                         const char* entry)
{
    int first = entry[0] - '0';
    int second = entry[1] - '0';
    int res = first * 10 + second;
    
    FILE* file = fopen(filename, "a");
    if (!file)
    {
        perror("Failed to open file");
        exit(EXIT_FAILURE);
    }
    if (first > count)
    {
        count++;
        fprintf(file, "\n");
    }
    fprintf(file, "%s", result);
    fclose(file);
    return count;
}

int compare(const void* a, const void* b)
{
    return strcmp(*(const char**)a, *(const char**)b);
}

int main()
{
    srand(time(NULL));

    const char* folder = "../../output/Grid";
    const char* output_file = "grid.txt";
    const char* network_file = "network_state.bin";

    const char* folder2 = "../../output/Word";
    const char* output_file2 = "words.txt";

    delete_file(output_file);
    delete_file(output_file2);

    load_trained_network(network_file);

    DIR* dir = opendir(folder);
    if (!dir)
    {
        perror("Failed to open directory");
        return EXIT_FAILURE;
    }

    char* filenames[MAX_FILES];
    int file_count = 0;

    struct dirent* entry;
    while ((entry = readdir(dir)) != NULL)
    {
        if (entry->d_type == DT_REG)
        {
            filenames[file_count] = strdup(entry->d_name);
            file_count++;
        }
    }
    closedir(dir);

    qsort(filenames, file_count, sizeof(char*), compare);

    int count = 0;
    for (int i = 0; i < file_count; i++)
    {
        char filepath[512];
        snprintf(filepath, sizeof(filepath), "%s/%s", folder, filenames[i]);

        float input_data[IMG_WIDTH * IMG_HEIGHT];

        load_image(filepath, input_data, IMG_WIDTH, IMG_HEIGHT);

        char result = predict_with_network(input_data);
        char result_str[2] = {result, '\0'};
        count = write_result_to_file(output_file, result_str, count,
                                                                filenames[i]);

        free(filenames[i]);
    }

    DIR* dir2 = opendir(folder2);
    if (!dir2)
    {
        perror("Failed to open directory");
        return EXIT_FAILURE;
    }

    char* filenames2[MAX_FILES];
    int file_count2 = 0;

    struct dirent* entry2;
    while ((entry2 = readdir(dir2)) != NULL)
    {
        if (entry2->d_type == DT_REG)
        {
            filenames2[file_count2] = strdup(entry2->d_name);
            file_count2++;
        }
    }
    closedir(dir2);

    qsort(filenames2, file_count2, sizeof(char*), compare);

    count = 0;
    for (int i = 0; i < file_count2; i++)
    {
        char filepath[512];
        snprintf(filepath, sizeof(filepath), "%s/%s", folder2, filenames2[i]);

        float input_data[IMG_WIDTH * IMG_HEIGHT];

        load_image(filepath, input_data, IMG_WIDTH, IMG_HEIGHT);

        char result = predict_with_network(input_data);
        char result_str[2] = {result, '\0'};
        count = write_result_to_file(output_file2, result_str, count,
                                                                filenames2[i]);

        free(filenames2[i]);
    }

    return 0;
}