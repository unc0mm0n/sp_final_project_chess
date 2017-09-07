#include "SDL_SETTINGS_WINDOW.h"
#include <assert.h>

SDL_BUTTON_action_t _SDL_SETTINGS_WINDOW_back_button_cb()
{ 
    SDL_BUTTON_action_t cmd;
    cmd.action = SDL_BUTTON_ACTION_CHANGE_STATE;
    cmd.new_state = SDL_INTERFACE_STATE_MAIN_MENU;
    return cmd;
}

SDL_BUTTON_action_t _SDL_SETTINGS_WINDOW_start_button_cb()
{ 
    SDL_BUTTON_action_t cmd;
    cmd.action = SDL_BUTTON_ACTION_SEND_SETTINGS_CMD;
    cmd.settings_cmd.type = MANAGER_SETTINGS_COMMAND_TYPE_START_GAME;

    return cmd;
}

SDL_SETTINGS_WINDOW_view_t* SDL_SETTINGS_WINDOW_create_view()
{
    SDL_SETTINGS_WINDOW_view_t* p_view = malloc(sizeof(SDL_SETTINGS_WINDOW_view_t));
    SDL_Window* window = SDL_CreateWindow("Tests", SDL_WINDOWPOS_CENTERED,
            SDL_WINDOWPOS_CENTERED, SETTINGS_WINDOW_W, SETTINGS_WINDOW_H, SDL_WINDOW_OPENGL);

    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1,
            SDL_RENDERER_ACCELERATED);
    
    if (p_view == NULL || window == NULL || renderer == NULL) 
    {
        free(p_view);
        //We first destroy the renderer
        SDL_DestroyRenderer(renderer); //NULL safe
        SDL_DestroyWindow(window); //NULL safe
        return NULL ;
    }

    p_view->window = window;
    p_view->renderer = renderer;

    SDL_Rect location = {.x= SETTINGS_WINDOW_SECTION_W * SDL_SETTINGS_WINDOW_SECTION_MODE + SETTINGS_WINDOW_NUMBER_OFFSET_X,
                         .y= SETTINGS_WINDOW_BUTTON_OFFSET_Y,
                         .h= SETTINGS_WINDOW_BUTTON_H,
                         .w= SETTINGS_WINDOW_NUMBER_BUTTON_W};
    SDL_Texture * active_texture = SDL_UTILS_load_texture_from_bmp("./graphics/1.bmp", p_view->renderer, FALSE);
    SDL_Texture * inactive_texture = SDL_UTILS_load_texture_from_bmp("./graphics/1_s.bmp", p_view->renderer, FALSE);
    p_view->mode_buttons[0] = SDL_BUTTON_create(TRUE, NULL, active_texture, inactive_texture, location);

    SDL_Rect location2 = {.x= SETTINGS_WINDOW_SECTION_W * SDL_SETTINGS_WINDOW_SECTION_MODE + SETTINGS_WINDOW_NUMBER_OFFSET_X,
                          .y= SETTINGS_WINDOW_BUTTON_OFFSET_Y + SETTINGS_WINDOW_BUTTON_AREA_H,
                          .h= SETTINGS_WINDOW_BUTTON_H,
                          .w= SETTINGS_WINDOW_NUMBER_BUTTON_W};
    active_texture = SDL_UTILS_load_texture_from_bmp("./graphics/2.bmp", p_view->renderer, FALSE);
    inactive_texture = SDL_UTILS_load_texture_from_bmp("./graphics/2_s.bmp", p_view->renderer, FALSE);
    p_view->mode_buttons[1] = SDL_BUTTON_create(TRUE, NULL, active_texture, inactive_texture, location2);
    
    SDL_Rect location3 = {.x= SETTINGS_WINDOW_SECTION_W * SDL_SETTINGS_WINDOW_SECTION_BUTTONS + SETTINGS_WINDOW_OFFSET_X,
                         .y= SETTINGS_WINDOW_BUTTON_OFFSET_Y + 2* SETTINGS_WINDOW_BUTTON_AREA_H,
                         .h= SETTINGS_WINDOW_BUTTON_H,
                         .w= SETTINGS_WINDOW_BUTTON_W};
    active_texture = SDL_UTILS_load_texture_from_bmp("./graphics/cancel.bmp", p_view->renderer, FALSE);
    p_view->back_button = SDL_BUTTON_create(TRUE, _SDL_SETTINGS_WINDOW_back_button_cb, active_texture, NULL, location3);

    SDL_Rect location4 = {.x= SETTINGS_WINDOW_SECTION_W * SDL_SETTINGS_WINDOW_SECTION_BUTTONS + SETTINGS_WINDOW_OFFSET_X,
                         .y= SETTINGS_WINDOW_BUTTON_OFFSET_Y,
                         .h= SETTINGS_WINDOW_BUTTON_H,
                         .w= SETTINGS_WINDOW_BUTTON_W};
    active_texture = SDL_UTILS_load_texture_from_bmp("./graphics/newgame.bmp", p_view->renderer, FALSE);
    p_view->start_button = SDL_BUTTON_create(TRUE, _SDL_SETTINGS_WINDOW_start_button_cb, active_texture, NULL, location4);

    return p_view;
}

void SDL_SETTINGS_WINDOW_destroy_view(SDL_SETTINGS_WINDOW_view_t* p_view)
{
    // All textures are destoyed by renderer
    SDL_DestroyRenderer(p_view->renderer);
    SDL_DestroyWindow(p_view->window);

    free(p_view);
}

void SDL_SETTINGS_WINDOW_draw_view(SDL_SETTINGS_WINDOW_view_t* p_view, const SETTINGS_settings_t* p_settings)
{
    if(p_view == NULL){
        return;
    }
    SDL_SetRenderDrawColor(p_view->renderer, 150, 150, 150, 255);
    SDL_RenderClear(p_view->renderer);
    
    SDL_BUTTON_render(p_view->mode_buttons[0], p_view->renderer);
    SDL_BUTTON_render(p_view->mode_buttons[1], p_view->renderer);

    SDL_BUTTON_render(p_view->back_button, p_view->renderer);
    SDL_BUTTON_render(p_view->start_button, p_view->renderer);
    SDL_RenderPresent(p_view->renderer);
}

SDL_BUTTON_action_t SDL_SETTINGS_WINDOW_handle_event(SDL_SETTINGS_WINDOW_view_t* p_view, SDL_Event* event)
{
    SDL_BUTTON_action_t cmd;
    SDL_BUTTON_action_t t_cmd;
    cmd.action = SDL_BUTTON_ACTION_NONE;
    if (event->type == SDL_MOUSEBUTTONUP)
    {
        t_cmd = SDL_BUTTON_handle_event(p_view->back_button,event);
        if (t_cmd.action == SDL_BUTTON_ACTION_CHANGE_STATE)
        {
            return t_cmd;
        }
        t_cmd = SDL_BUTTON_handle_event(p_view->start_button,event);
        if (t_cmd.action == SDL_BUTTON_ACTION_SEND_SETTINGS_CMD)
        {
            return t_cmd;
        }
    }
    return cmd;
}
