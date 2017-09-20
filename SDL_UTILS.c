#include <stdio.h>

#include "SDL_UTILS.h"
#include "FILES.h"

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

void SDL_UTILS_get_save_path(const char* basedir, int savenum, char* buff)
{
    char savename[SDL_MAX_FILENAME];
    sprintf(savename, SDL_SAVE_NAME_TEMPLATE, savenum);
    strcpy(buff, basedir);
    strcat(buff, "/");
    strcat(buff, savename);
}

void SDL_UTILS_roll_saves(const char* basedir)
{
    char filename[SDL_MAX_FILENAME], newfilename[SDL_MAX_FILENAME];
    for (int i=SDL_MAX_SAVES; i > 0; i--)
    {
        SDL_UTILS_get_save_path(basedir, i, filename);
        if (FILES_is_available(filename))
        {
            SDL_UTILS_get_save_path(basedir, i+1, newfilename);
            rename(filename, newfilename);
        }
    }
}

void SDL_UTILS_unroll_saves(const char* basedir)
{
    char filename[SDL_MAX_FILENAME], newfilename[SDL_MAX_FILENAME];
    for (int i=2; i <= SDL_MAX_SAVES + 1; i++)
    {
        SDL_UTILS_get_save_path(basedir, i, filename);
        if (FILES_is_available(filename))
        {
            SDL_UTILS_get_save_path(basedir, i-1, newfilename);
            rename(filename, newfilename);
        }
    }
}

int SDL_UTILS_available_saves(const char* basedir)
{
    int res = 0;
    char buff[SDL_MAX_FILENAME];
    for (int i=0; i < SDL_MAX_SAVES; i++)
    {
        SDL_UTILS_get_save_path(basedir, i+1, buff);
        if (FILES_is_available(buff))
        {
            res |= (1<<i);
        }
    }

    return res;
}
