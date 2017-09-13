#include "SDL_SETTINGS_WINDOW.h"
#include <assert.h>

/***** Private functiosn *****/

/** Button callbacks **/

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

SDL_BUTTON_action_t _SDL_SETTINGS_WINDOW_change_difficulty(int val)
{
    SDL_BUTTON_action_t cmd;
    cmd.action = SDL_BUTTON_ACTION_SEND_SETTINGS_CMD;
    cmd.settings_cmd.type = MANAGER_SETTINGS_COMMAND_TYPE_CHANGE_SETTING;
    cmd.settings_cmd.data.change_setting.setting = SETTINGS_SETTING_DIFFICULTY;
    cmd.settings_cmd.data.change_setting.value = val;
    return cmd;
}

SDL_BUTTON_action_t _SDL_SETTINGS_WINDOW_change_color(int val)
{
    
    SDL_BUTTON_action_t cmd;
    cmd.action = SDL_BUTTON_ACTION_SEND_SETTINGS_CMD;
    cmd.settings_cmd.type = MANAGER_SETTINGS_COMMAND_TYPE_CHANGE_SETTING;
    cmd.settings_cmd.data.change_setting.setting = SETTINGS_SETTING_USER_COLOR;
    cmd.settings_cmd.data.change_setting.value = val;
    return cmd;
}

SDL_BUTTON_action_t _SDL_SETTINGS_WINDOW_change_game_mode(int val)
{
    
    SDL_BUTTON_action_t cmd;
    cmd.action = SDL_BUTTON_ACTION_SEND_SETTINGS_CMD;
    cmd.settings_cmd.type = MANAGER_SETTINGS_COMMAND_TYPE_CHANGE_SETTING;
    cmd.settings_cmd.data.change_setting.setting = SETTINGS_SETTING_GAME_MODE;
    cmd.settings_cmd.data.change_setting.value = val;
    return cmd;
}

/**
 * Populate a section of the settings window with a title and option buttons.
 * The pointers to the buttons will be located at p_start and the end position of the pointer will be returned.
 * The buttons will have values with increments of 1 from range_start to range_stop (inclusive).
 * the f_active_fn and f_inactive_fn will be formatted with an integer matching the value and loaded
 */
SDL_button_t** _SDL_SETTINGS_WINDOW_populate_section(SDL_SETTINGS_WINDOW_view_t * p_view, SDL_SETTINGS_WINDOW_SECTION_E section, const char* title_fn, const char* f_active_fn, const char* f_inactive_fn, SDL_button_t** p_start, int range_start, int range_stop, SDL_BUTTON_action_t (*cb)(int))
{
    SDL_Texture* t_i, *t_a;
    t_i = SDL_UTILS_load_texture_from_bmp(title_fn, p_view->renderer, FALSE);
    SDL_Rect l= {.x= SETTINGS_WINDOW_SECTION_W * section + SETTINGS_WINDOW_OFFSET_X,
                 .y= SETTINGS_WINDOW_BUTTON_OFFSET_Y,
                 .h= SETTINGS_WINDOW_BUTTON_H,
                 .w= SETTINGS_WINDOW_BUTTON_W};
    *p_start = SDL_BUTTON_create(FALSE, NULL, NULL, t_i, l, 0);
    p_start++;
    p_view->button_count++;

    char name_buf[255];
    l.x = l.x - SETTINGS_WINDOW_OFFSET_X + SETTINGS_WINDOW_NUMBER_OFFSET_X;
    l.w = SETTINGS_WINDOW_NUMBER_BUTTON_W;
    for (int i = range_start; i <= range_stop; i++)
    {
        name_buf[0] = '\0';
        sprintf(name_buf, f_active_fn, i);
        t_a = SDL_UTILS_load_texture_from_bmp(name_buf, p_view->renderer, FALSE);

        name_buf[0] = '\0';
        sprintf(name_buf, f_inactive_fn, i);
        t_i = SDL_UTILS_load_texture_from_bmp(name_buf, p_view->renderer, FALSE);

        l.y += SETTINGS_WINDOW_BUTTON_AREA_H;
        
        *p_start = SDL_BUTTON_create(TRUE, cb, t_a, t_i, l, i);
        p_start++;
        p_view->button_count++;
    }
    return p_start;
}

/* Render the section starting at p_start with the given number of buttons.
 * Assumes *p_start is the title and is therefore never active,
 * in addition marks the button with value chosen_val as inactive.
 */
void _SDL_render_section(SDL_SETTINGS_WINDOW_view_t* p_view, SDL_button_t** p_start, int button_count, int chosen_val)
{
    SDL_BUTTON_render(*p_start, p_view->renderer);
    p_start++;
    for (int i=0; i < button_count; i++)
    {
        (*p_start)->is_active = ((*p_start)->value != chosen_val);
        SDL_BUTTON_render(*p_start, p_view->renderer);
        p_start++;
    }
}

/***** Public functions *****/

SDL_SETTINGS_WINDOW_view_t* SDL_SETTINGS_WINDOW_create_view()
{
    SDL_SETTINGS_WINDOW_view_t* p_view = malloc(sizeof(SDL_SETTINGS_WINDOW_view_t));
    if (p_view == NULL)
    {
        return NULL;
    }

    SDL_Window* window = SDL_CreateWindow("Tests", SDL_WINDOWPOS_CENTERED,
            SDL_WINDOWPOS_CENTERED, SETTINGS_WINDOW_W, SETTINGS_WINDOW_H, SDL_WINDOW_OPENGL);

    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1,
            SDL_RENDERER_ACCELERATED);
    
    p_view->buttons = malloc(sizeof(SDL_button_t*) * SETTINGS_WINDOW_MAX_BUTTONS);

    if (p_view == NULL || window == NULL || renderer == NULL || p_view->buttons == NULL) 
    {
        free(p_view->buttons);
        free(p_view);
        SDL_DestroyRenderer(renderer); //NULL safe
        SDL_DestroyWindow(window); //NULL safe
        return NULL ;
    }

    p_view->window = window;
    p_view->renderer = renderer;

    p_view->button_count = 0;
    
    SDL_button_t** tmp = p_view->buttons;

    p_view->mode_buttons = tmp;
    tmp = _SDL_SETTINGS_WINDOW_populate_section(p_view, 
            SDL_SETTINGS_WINDOW_SECTION_MODE, 
            "./graphics/gamemode.bmp", 
            "./graphics/%d.bmp", 
            "./graphics/%d_s.bmp", 
            tmp, 
            1, 
            2, 
            _SDL_SETTINGS_WINDOW_change_game_mode);
    
    p_view->difficulty_buttons = tmp;
    tmp = _SDL_SETTINGS_WINDOW_populate_section(p_view, 
            SDL_SETTINGS_WINDOW_SECTION_DIFFICULTY, 
            "./graphics/difficulty.bmp", 
            "./graphics/d%d.bmp", 
            "./graphics/d%d_s.bmp", 
            tmp, 
            1, 
            5, 
            _SDL_SETTINGS_WINDOW_change_difficulty);

    p_view->color_buttons = tmp;
    
    tmp = _SDL_SETTINGS_WINDOW_populate_section(p_view, 
            SDL_SETTINGS_WINDOW_SECTION_COLOR, 
            "./graphics/player_color.bmp", 
            "./graphics/C%d.bmp", 
            "./graphics/C%d_s.bmp", 
            tmp, 
            0, 
            1, 
            _SDL_SETTINGS_WINDOW_change_color);

    SDL_Rect location3 = {.x= SETTINGS_WINDOW_SECTION_W * SDL_SETTINGS_WINDOW_SECTION_BUTTONS + SETTINGS_WINDOW_OFFSET_X,
                         .y= SETTINGS_WINDOW_BUTTON_OFFSET_Y + 5* SETTINGS_WINDOW_BUTTON_AREA_H,
                         .h= SETTINGS_WINDOW_BUTTON_H,
                         .w= SETTINGS_WINDOW_BUTTON_W};
    SDL_Texture* active_texture = SDL_UTILS_load_texture_from_bmp("./graphics/cancel.bmp", p_view->renderer, FALSE);
    *tmp = SDL_BUTTON_create(TRUE, _SDL_SETTINGS_WINDOW_back_button_cb, active_texture, NULL, location3, 3);
    p_view->back_button = *tmp;
    p_view->button_count++;
    tmp++;

    SDL_Rect location4 = {.x= SETTINGS_WINDOW_SECTION_W * SDL_SETTINGS_WINDOW_SECTION_BUTTONS + SETTINGS_WINDOW_OFFSET_X,
                         .y= SETTINGS_WINDOW_BUTTON_OFFSET_Y,
                         .h= SETTINGS_WINDOW_BUTTON_H,
                         .w= SETTINGS_WINDOW_BUTTON_W};
    active_texture = SDL_UTILS_load_texture_from_bmp("./graphics/newgame.bmp", p_view->renderer, FALSE);
    *tmp = SDL_BUTTON_create(TRUE, _SDL_SETTINGS_WINDOW_start_button_cb, active_texture, NULL, location4, 4);
    p_view->start_button = *tmp;
    p_view->button_count++;
    tmp++;

    return p_view;
}

void SDL_SETTINGS_WINDOW_destroy_view(SDL_SETTINGS_WINDOW_view_t* p_view)
{
    // All textures are destoyed by renderer
    SDL_DestroyRenderer(p_view->renderer);
    SDL_DestroyWindow(p_view->window);

    for (int i=0; i < p_view->button_count; i++)
    {
        SDL_BUTTON_destroy(p_view->buttons[i]);
    }
    free(p_view->buttons);
    free(p_view);
}

void SDL_SETTINGS_WINDOW_draw_view(SDL_SETTINGS_WINDOW_view_t* p_view, const SETTINGS_settings_t* p_settings)
{
    if(p_view == NULL){
        return;
    }
    SDL_SetRenderDrawColor(p_view->renderer, 150, 150, 150, 255);
    SDL_RenderClear(p_view->renderer);
    
    if (SETTINGS_is_active(p_settings, SETTINGS_SETTING_GAME_MODE))
    {
        _SDL_render_section(p_view, p_view->mode_buttons, 2, p_settings->game_mode);
    }
    
    if (SETTINGS_is_active(p_settings, SETTINGS_SETTING_DIFFICULTY))
    {
        _SDL_render_section(p_view, p_view->difficulty_buttons, 5, p_settings->difficulty);
    }

    if (SETTINGS_is_active(p_settings, SETTINGS_SETTING_USER_COLOR))
    {
        _SDL_render_section(p_view, p_view->color_buttons, 2, p_settings->user_color);
    }

    SDL_BUTTON_render(p_view->start_button, p_view->renderer);
    SDL_BUTTON_render(p_view->back_button, p_view->renderer);
    SDL_RenderPresent(p_view->renderer);
}

SDL_BUTTON_action_t SDL_SETTINGS_WINDOW_handle_event(SDL_SETTINGS_WINDOW_view_t* p_view, SDL_Event* event)
{
    SDL_BUTTON_action_t cmd;
    SDL_BUTTON_action_t t_cmd;
    cmd.action = SDL_BUTTON_ACTION_NONE;
    if (event->type == SDL_QUIT) // X button pressed
    {
        cmd.action = SDL_BUTTON_ACTION_SEND_SETTINGS_CMD;
        cmd.settings_cmd.type = MANAGER_SETTINGS_COMMAND_TYPE_QUIT;
    }
    else if (event->type == SDL_MOUSEBUTTONUP)
    {
        for (int i=0; i < p_view->button_count; i++)
        {
            t_cmd = SDL_BUTTON_handle_event(p_view->buttons[i], event);
            if (t_cmd.action != SDL_BUTTON_ACTION_NONE)
            {
                return t_cmd;
            }   
        }
    }
    return cmd;
}
