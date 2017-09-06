#include "SDL_BUTTON.h"

SDL_button_t* SDL_BUTTON_create(BOOL is_active, MANAGER_agent_command_t (*cb)(), SDL_Texture* texture, SDL_Rect location)
{
    SDL_button_t* tmp = malloc(sizeof(SDL_button_t));
    tmp->is_active = is_active;
    tmp->cb = cb;
    tmp->texture = texture;
    tmp->location = location;
    return tmp;
}

void SDL_BUTTON_render(SDL_button_t* p_button, SDL_Renderer* renderer)
{
    SDL_RenderCopy(renderer, p_button->texture, NULL, &p_button->location);
}

void SDL_BUTTON_destroy(SDL_button_t* p_button)
{
    // Textures are destroyed by renderer
    free(p_button);
}
