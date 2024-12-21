#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#include "image_processing.h"

#define RADIUS 1                    
#define WHITE_NEIGHBORS_THRESHOLD 5 

void convert_to_grayscale(SDL_Surface *surface) 
{
    Uint32* pixels = (Uint32 *)surface->pixels;
    int width = surface->w;
    int height = surface->h;

    for (int y = 0; y < height; ++y) 
    {
        for (int x = 0; x < width; ++x) 
        {
            Uint32 pixel = pixels[y*width+x];
            Uint8 r,g,b;
            SDL_GetRGB(pixel, surface->format, &r, &g, &b);
            Uint8 grayscale = 0.3 * r + 0.59 * g + 0.11 * b;
            Uint32 grayPixel =SDL_MapRGB(surface->format, grayscale, grayscale, grayscale);
            pixels[y*width+x]= grayPixel;
        }
    }
    
    if (IMG_SavePNG(surface, "output/result/OCR_grayscale.png") != 0) 
        fprintf(stderr, "Erreur sauvegarde image: %s\n", IMG_GetError());
    else 
        printf("Image convertie en noir et blanc et sauvegardee sous 'output/result/OCR_grayscale.png'.\n");
    
}


void convert_grayscale_to_bw(SDL_Surface* surface, Uint8 threshold) 
{
    Uint32* pixels = (Uint32 *)surface->pixels;
    int width = surface->w;
    int height = surface->h;

    for (int y = 0; y < height; y++) 
    {
        for (int x = 0; x < width; x++) 
        {
            Uint32 pixel = pixels[y*width+x];
            Uint8 r, g, b;
            SDL_GetRGB(pixel, surface->format, &r, &g, &b);
            Uint8 grayValue = r; //les valeurs de gris sont pareils pour r, g et b

            Uint32 newPixel;
            if (grayValue < threshold) 
            {
                newPixel = SDL_MapRGB(surface->format, 255, 255, 255);
            } 
            else 
            {
                newPixel = SDL_MapRGB(surface->format, 0, 0, 0);
            }

            pixels[y*width+x] = newPixel;
        }
    }

    if (IMG_SavePNG(surface, "output/result/OCR_bw.png") != 0) 
        fprintf(stderr, "Erreur sauvegarde image: %s\n", IMG_GetError());
    else 
        printf("Image convertie en nuances de gris et sauvegardee sous 'output/result/OCR_bw.png'.\n");
    
}


void increase_contrast(SDL_Surface *image, double con) {
    if (image)
    {
        Uint32 *pixels = (Uint32 *)image->pixels;
        int width = image->w;
        int height = image->h;

        for (int y = 0; y < height; y++) 
        {
            for (int x = 0; x < width; x++) 
            {
                Uint32 pixel = pixels[y*width + x];
                Uint8 r, g, b;
                SDL_GetRGB(pixel, image->format, &r, &g, &b);

                r = (Uint8) SDL_clamp(((r-200)*con+128), 0, 255);
                g = (Uint8) SDL_clamp(((g-200)*con+128), 0, 255);
                b = (Uint8) SDL_clamp(((b-200)*con+128), 0, 255);

                pixel = SDL_MapRGB(image->format, r, g, b);
                pixels[y*width+x] = pixel;
            }
        }
    }

    if (IMG_SavePNG(image, "output/result/OCR_contrast.png") != 0) 
        fprintf(stderr, "Erreur sauvegarde image: %s\n", IMG_GetError());
    else 
        printf("Image convertie en nuances de gris et sauvegardee sous 'output/result/OCR_contrast.png'.\n");
    
}
int is_black_pixel(SDL_Surface *image,int x, int y){

    Uint32 *pixels =image->pixels;

    Uint8 r, g, b;
    SDL_GetRGB(pixels[y * image->w + x], image->format, &r, &g, &b);

    return (r == 0 && g == 0 && b == 0); 
}

void set_white_pixel(SDL_Surface *image, int x, int y){

    Uint32 white_pixel = SDL_MapRGB(image->format, 255, 255, 255);

    Uint32 *pixels =image->pixels;
    pixels[y * image->w + x] = white_pixel;

}

void reduce_noise(SDL_Surface *image){

    int width = image->w;
    int height = image->h;
    Uint32 *pixels = image->pixels;

    Uint32 *pixels_copy = malloc(width * height * sizeof(Uint32));
    memcpy(pixels_copy, pixels, width * height * sizeof(Uint32));

    for (int y = RADIUS;y <height - RADIUS; y++){
        for (int x= RADIUS; x < width - RADIUS;x++){
            
            if (is_black_pixel(image, x, y)){

                int white_neighbors = 0;

                for (int dy= -RADIUS;dy <= RADIUS; dy++){       
                    for (int dx = -RADIUS; dx<= RADIUS;dx++){   
                        
                        int nx = x + dx;
                        int ny = y + dy;
 
                        if (nx>= 0 && nx < width && ny >=0 && ny <height){

                            Uint8 r, g, b;

                            SDL_GetRGB(pixels_copy[ny * width + nx], image->format, &r, &g, &b);

                            if (r == 255 && g ==255 && b== 255){
                                white_neighbors++;
                            }
                        }
                    }
                }

                if (white_neighbors>= WHITE_NEIGHBORS_THRESHOLD){

                    set_white_pixel(image, x, y);
                }
            }
        }
    }
    if (IMG_SavePNG(image, "output/result/OCR_denoise.png") != 0)
            fprintf(stderr, "Erreur sauvegarde image: %s\n", IMG_GetError());
    else
        printf("Image sauvegardee sous 'output/result/OCR_denoise.png'.\n");
    
    free(pixels_copy);
}