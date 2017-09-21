#include "SDL_MAIN_WINDOW.h"
#include <assert.h>

/***** Private functions *****/

/** Button callbacks **/
SDL_BUTTON_action_t _SDL_MAIN_WINDOW_new_game_button_cb()
{
    SDL_BUTTON_action_t cmd;
    cmd.action = SDL_BUTTON_ACTION_CHANGE_STATE;
    cmd.new_state = SDL_INTERFACE_STATE_SETTINGS;

    return cmd;
}

SDL_BUTTON_action_t _SDL_MAIN_WINDOW_load_game_button_cb()
{
    SDL_BUTTON_action_t cmd;
    cmd.action = SDL_BUTTON_ACTION_CHANGE_STATE;
    cmd.new_state = SDL_INTERFACE_STATE_LOAD;

    return cmd;
}

SDL_BUTTON_action_t _SDL_MAIN_WINDOW_quit_button_cb()
{
    SDL_BUTTON_action_t cmd;
    cmd.action = SDL_BUTTON_ACTION_SEND_SETTINGS_CMD;
    cmd.settings_cmd.type = MANAGER_SETTINGS_COMMAND_TYPE_QUIT;
    return cmd;
}

/***** Public functions *****/

SDL_MAIN_WINDOW_view_t* SDL_MAIN_WINDOW_create_view()
{
    SDL_MAIN_WINDOW_view_t* p_view = malloc(sizeof(SDL_MAIN_WINDOW_view_t));
    if (p_view == NULL)
    {
        return NULL;
    }

    SDL_Window* window = SDL_CreateWindow("Main", SDL_WINDOWPOS_CENTERED,
            SDL_WINDOWPOS_CENTERED, MAIN_WINDOW_W, MAIN_WINDOW_H, SDL_WINDOW_OPENGL);

    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1,
            SDL_RENDERER_ACCELERATED);
    
    p_view->buttons = malloc(sizeof(SDL_button_t) * MAIN_WINDOW_MAX_BUTTONS);
    
    if (window == NULL || renderer == NULL || p_view->buttons == NULL) 
    {
        free(p_view->buttons);
        free(p_view);
        //We first destroy the renderer
        SDL_DestroyRenderer(renderer); //NULL safe
        SDL_DestroyWindow(window); //NULL safe
        return NULL ;
    }

    p_view->window = window;
    p_view->renderer = renderer;
    p_view->bg_texture = SDL_UTILS_load_texture_from_bmp("./graphics/bg.bmp", renderer, FALSE);
    p_view->button_count = 0;
    
    SDL_MAIN_WINDOW_add_button(p_view, "./graphics/newgame.bmp", NULL, _SDL_MAIN_WINDOW_new_game_button_cb); 
    SDL_MAIN_WINDOW_add_button(p_view, "./graphics/loadgame.bmp", NULL, _SDL_MAIN_WINDOW_load_game_button_cb); 
    SDL_MAIN_WINDOW_add_button(p_view, "./graphics/quit.bmp", NULL, _SDL_MAIN_WINDOW_quit_button_cb); 
    
    return p_view;
}

void SDL_MAIN_WINDOW_add_button(SDL_MAIN_WINDOW_view_t* p_view, const char* active_texture_fn, const char* inactive_texture_fn, SDL_BUTTON_action_t (*cb) (int))
{
    assert(p_view->button_count < MAIN_WINDOW_MAX_BUTTONS);
    SDL_Rect location = {.x=MAIN_WINDOW_BUTTON_OFFSET_X,
        .y=MAIN_WINDOW_BUTTON_PADDING + p_view->button_count * MAIN_WINDOW_BUTTON_AREA_H,
        .h=MAIN_WINDOW_BUTTON_H,
        .w=MAIN_WINDOW_BUTTON_W};
    SDL_Texture * b_texture = SDL_UTILS_load_texture_from_bmp(active_texture_fn, p_view->renderer, FALSE);
    SDL_Texture* d_texture = SDL_UTILS_load_texture_from_bmp(inactive_texture_fn, p_view->renderer, FALSE);
    p_view->buttons[p_view->button_count] = SDL_BUTTON_create(TRUE, cb, b_texture, d_texture, location, 0);
    p_view->button_count++;
}

void SDL_MAIN_WINDOW_destroy_view(SDL_MAIN_WINDOW_view_t* p_view)
{
    // All textures are destoyed by renderer
    SDL_DestroyRenderer(p_view->renderer);
    SDL_DestroyWindow(p_view->window);

    for (int i = 0 ; i < p_view->button_count; i++)
    {
        SDL_BUTTON_destroy(p_view->buttons[i]);
    }
    free(p_view->buttons);

    free(p_view);
}

void SDL_MAIN_WINDOW_draw_view(SDL_MAIN_WINDOW_view_t* p_view)
{
    if(p_view == NULL){
        return;
    }
    SDL_Rect rec = { .x = 0, .y = 0, .w = MAIN_WINDOW_W, .h = MAIN_WINDOW_H };
    SDL_SetRenderDrawColor(p_view->renderer, 150, 150, 150, 255);
    SDL_RenderClear(p_view->renderer);
    SDL_RenderCopy(p_view->renderer, p_view->bg_texture, NULL, &rec);

    for (int i=0; i < p_view->button_count; i++)
    {
        SDL_BUTTON_render(p_view->buttons[i], p_view->renderer);
    }    
    SDL_RenderPresent(p_view->renderer);
}

SDL_BUTTON_action_t SDL_MAIN_WINDOW_handle_event(SDL_MAIN_WINDOW_view_t* p_view, SDL_Event* event)
{
    SDL_BUTTON_action_t act;
    act.action = SDL_BUTTON_ACTION_NONE;
    if (event->type == SDL_QUIT) // X button pressed
    {
        act.action = SDL_BUTTON_ACTION_SEND_SETTINGS_CMD;
        act.settings_cmd.type = MANAGER_SETTINGS_COMMAND_TYPE_QUIT;
    }
    if (event->type == SDL_MOUSEBUTTONUP)
    {
        for (int i=0; i < p_view->button_count; i++)
        {
            SDL_BUTTON_action_t c = SDL_BUTTON_handle_event(p_view->buttons[i], event);
            if (c.action != SDL_BUTTON_ACTION_NONE)
            {
                return c;
            }
        }
    }
    return act;
}
