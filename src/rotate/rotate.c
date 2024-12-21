#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <stdio.h>
#include <math.h>
#include <string.h>

// Fonction pour détecter l'angle de rotation en fonction de la forme
double detectAngle(SDL_Surface *image) {
    int width = image->w;
    int height = image->h;
    Uint32 *pixels = (Uint32 *)image->pixels;

    int sumX = 0, sumY = 0, count = 0;

    // Seuil pour détecter les pixels de l'objet (par exemple, un carré ou rectangle)
    Uint32 threshold = SDL_MapRGB(image->format, 200, 200, 200); // Couleur claire (à ajuster)

    for (int y = 1; y < height - 1; y++) {
        for (int x = 1; x < width - 1; x++) {
            Uint32 pixel = pixels[y * width + x];
            Uint8 r, g, b, a;
            SDL_GetRGBA(pixel, image->format, &r, &g, &b, &a);

            // Si le pixel est clair (dans le cas d'un objet sur fond sombre, comme un carré)
            if (r > 200 && g > 200 && b > 200) {
                sumX += x;
                sumY += y;
                count++;
            }
        }
    }

    // Si aucun pixel clair détecté, l'angle est de 0
    if (count == 0) {
        return 0.0;
    }

    // Calcul du centre de la forme
    double centerX = (double)sumX / count;
    double centerY = (double)sumY / count;

    // Calcul de l'angle de la forme par rapport à l'horizontale
    return atan2(centerY - height / 2, centerX - width / 2) * 180.0 / M_PI;
}

// Fonction pour pivoter une surface SDL
SDL_Surface *rotateSurface(SDL_Surface *src, double angle) {
    SDL_Surface *rotated = SDL_CreateRGBSurfaceWithFormat(
        0, src->w, src->h, src->format->BitsPerPixel, src->format->format);

    if (!rotated) {
        printf("Erreur : Impossible de créer la surface pivotée.\n");
        return NULL;
    }

    double rad = angle * M_PI / 180.0;
    double cosA = cos(rad);
    double sinA = sin(rad);

    int centerX = src->w / 2;
    int centerY = src->h / 2;

    Uint32 *srcPixels = (Uint32 *)src->pixels;
    Uint32 *rotPixels = (Uint32 *)rotated->pixels;

    for (int y = 0; y < src->h; y++) {
        for (int x = 0; x < src->w; x++) {
            int relX = x - centerX;
            int relY = y - centerY;

            int newX = cosA * relX - sinA * relY + centerX;
            int newY = sinA * relX + cosA * relY + centerY;

            if (newX >= 0 && newX < src->w && newY >= 0 && newY < src->h) {
                rotPixels[y * src->w + x] = srcPixels[newY * src->w + newX];
            } else {
                rotPixels[y * src->w + x] = SDL_MapRGBA(src->format, 0, 0, 0, 0); // Remplir avec noir
            }
        }
    }

    return rotated;
}