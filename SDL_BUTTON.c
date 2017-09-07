#include "SDL_BUTTON.h"

SDL_button_t* SDL_BUTTON_create(BOOL is_active, MANAGER_agent_command_t (*cb)(), SDL_Texture* a_texture, SDL_Texture* i_texture, SDL_Rect location)
{
    SDL_button_t* tmp = malloc(sizeof(SDL_button_t));
    tmp->is_active = is_active;
    tmp->cb = cb;
    tmp->active_texture = a_texture;
    tmp->inactive_texture = i_texture;
    tmp->location = location;
    return tmp;
}

void SDL_BUTTON_destroy(SDL_button_t* p_button)
{
    // Textures are destroyed by renderer
    free(p_button);
}

MANAGER_agent_command_t SDL_BUTTON_handle_event(SDL_button_t* p_button, SDL_Event* event)
{
    MANAGER_agent_command_t res;
    if ((!p_button->is_active) || (p_button->cb == NULL))
    {
        res.type = MANAGER_COMMAND_TYPE_INVALID;
    }
    else if (event->type == SDL_MOUSEBUTTONUP) {

		SDL_Point point;
		point.x = event->button.x;
		point.y = event->button.y;
		if (SDL_PointInRect(&point, &p_button->location)) 
		{
			res = p_button->cb();
		}
		else
		{
			res.type = MANAGER_COMMAND_TYPE_INVALID;
		}

	} 
	else 
	{
		res.type = MANAGER_COMMAND_TYPE_INVALID;
    }
    return res;
}

void SDL_BUTTON_render(SDL_button_t* p_button, SDL_Renderer* renderer)
{
    if (p_button->is_active)
    {
        SDL_RenderCopy(renderer, p_button->active_texture, NULL, &p_button->location);
    }
    else
    {
        SDL_RenderCopy(renderer, p_button->inactive_texture, NULL, &p_button->location);
    }
}

