#include "SDL_UTILS.h"

SDL_Texture* SDL_UTILS_load_texture_from_bmp(const char* filename, SDL_Renderer* renderer, BOOL use_color_key)
{
    if (filename == NULL)
    {
        return NULL;
    }
    SDL_Texture* p_texture;

    SDL_Surface* loadingSurface = SDL_LoadBMP(filename);
    if (loadingSurface == NULL ) {
        printf("Could not create a surface: %s\n", SDL_GetError());
        return NULL;
    }

    if (use_color_key)
    {
        SDL_SetColorKey(loadingSurface, SDL_TRUE, SDL_MapRGB(loadingSurface->format, 0xFF, 0xFF, 0xFF));
    }

    p_texture = SDL_CreateTextureFromSurface(renderer, loadingSurface);
    if (p_texture == NULL ) {
        printf("Could not create a texture: %s\n", SDL_GetError());
        return NULL;
    }
    SDL_FreeSurface(loadingSurface); //We finished with the surface -> delete it
    return p_texture;
}
