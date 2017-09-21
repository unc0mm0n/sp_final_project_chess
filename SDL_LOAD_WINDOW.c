#include "SDL_LOAD_WINDOW.h"
#include <assert.h>

/***** global variables *****/

static int gs_active_slot;
static char gs_filename_buffer[SDL_MAX_FILENAME];

/***** Private functiosn *****/

/** Button callbacks **/

SDL_BUTTON_action_t _SDL_LOAD_WINDOW_back_button_cb(int origin)
{ 
    SDL_BUTTON_action_t cmd;
    cmd.action = SDL_BUTTON_ACTION_CHANGE_STATE;
    cmd.new_state = origin;
    return cmd;
}

SDL_BUTTON_action_t _SDL_LOAD_WINDOW_load_button_cb(int origin)
{ 
    SDL_BUTTON_action_t cmd;

    SDL_UTILS_get_save_path(".", gs_active_slot, gs_filename_buffer);

    if (origin == (int) SDL_INTERFACE_STATE_MAIN_MENU)
    {
        cmd.action = SDL_BUTTON_ACTION_SEND_SETTINGS_CMD;
        cmd.settings_cmd.type = MANAGER_SETTINGS_COMMAND_TYPE_LOAD;
        cmd.settings_cmd.data.filename = gs_filename_buffer;
    }
    else
    {
        cmd.action = SDL_BUTTON_ACTION_SEND_PLAY_CMD;
        cmd.play_cmd.type = MANAGER_PLAY_COMMAND_TYPE_LOAD;
        cmd.play_cmd.data.filename = gs_filename_buffer;
    }

    return cmd;
}

SDL_BUTTON_action_t _SDL_LOAD_WINDOW_choose_slot_cb(int val)
{
    SDL_BUTTON_action_t cmd;
    cmd.action = SDL_BUTTON_ACTION_NONE;

    gs_active_slot = val;
    return cmd;
}


/***** Public functions *****/

SDL_LOAD_WINDOW_view_t* SDL_LOAD_WINDOW_create_view(SDL_INTERFACE_STATE_E origin)
{
    SDL_LOAD_WINDOW_view_t* p_view = malloc(sizeof(SDL_LOAD_WINDOW_view_t));
    if (p_view == NULL)
    {
        return NULL;
    }

    SDL_Window* window = SDL_CreateWindow("Load", SDL_WINDOWPOS_CENTERED,
            SDL_WINDOWPOS_CENTERED, LOAD_WINDOW_W, LOAD_WINDOW_H, SDL_WINDOW_OPENGL);

    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1,
            SDL_RENDERER_ACCELERATED);

    p_view->buttons = malloc(sizeof(SDL_button_t*) * LOAD_WINDOW_MAX_BUTTONS);

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
    gs_active_slot = -1;

    p_view->button_count = 0;

    SDL_Rect left = {.x= LOAD_WINDOW_BUTTON_OFFSET_X,
        .y= LOAD_WINDOW_BUTTON_OFFSET_Y,
        .h= LOAD_WINDOW_BUTTON_H,
        .w= LOAD_WINDOW_SMALL_BUTTON_W};

    SDL_Rect right = {.x= LOAD_WINDOW_BUTTON_OFFSET_X + LOAD_WINDOW_SMALL_BUTTON_W,
        .y= LOAD_WINDOW_BUTTON_OFFSET_Y,
        .h= LOAD_WINDOW_BUTTON_H,
        .w= LOAD_WINDOW_BUTTON_W};

    BOOL success = TRUE;
    char namebuff[255];
    int available_saves = SDL_UTILS_available_saves(".");
    for (int i = 0; i < SDL_MAX_SAVES; i++)
    {
        if ((available_saves & (1<<i)) > 0)
        {
            success &= SDL_LOAD_WINDOW_add_button(p_view, left, "./graphics/d5_s.bmp", "./graphics/Wking.bmp", NULL, 0);
            p_view->active_marks[i] = p_view->buttons[p_view->button_count - 1]; // remember last button added

            sprintf(namebuff, "./graphics/slot%d.bmp", i+1);
            success &= SDL_LOAD_WINDOW_add_button(p_view, right, namebuff, namebuff, _SDL_LOAD_WINDOW_choose_slot_cb, i+1);
        }
        else
        {
            p_view->active_marks[i] = NULL;
        }
        left.y += LOAD_WINDOW_BUTTON_AREA_H;
        right.y += LOAD_WINDOW_BUTTON_AREA_H;
    }
    
    right.x += (-LOAD_WINDOW_SMALL_BUTTON_W + LOAD_WINDOW_BUTTON_W + LOAD_WINDOW_BUTTON_SPACING) - 60;
    left.x -= 60;
    left.w = LOAD_WINDOW_BUTTON_W;
    success &= SDL_LOAD_WINDOW_add_button(p_view, left, "./graphics/cancel.bmp", NULL, _SDL_LOAD_WINDOW_back_button_cb, origin);
    success &= SDL_LOAD_WINDOW_add_button(p_view, right, "./graphics/loadgame.bmp", "./graphics/undo_i.bmp", _SDL_LOAD_WINDOW_load_button_cb, origin);
    p_view->load_button = p_view->buttons[p_view->button_count - 1];
    if (!success) 
    {
        SDL_LOAD_WINDOW_destroy_view(p_view);
        return NULL;
    }
    return p_view;
}

BOOL SDL_LOAD_WINDOW_add_button(SDL_LOAD_WINDOW_view_t* p_view, SDL_Rect location, const char* active_texture_fn, const char* inactive_texture_fn, SDL_BUTTON_action_t (*cb)(int), int value)
{
    assert(p_view->button_count < LOAD_WINDOW_MAX_BUTTONS);
    SDL_Texture * b_texture = SDL_UTILS_load_texture_from_bmp(active_texture_fn, p_view->renderer, FALSE);
    SDL_Texture* d_texture = SDL_UTILS_load_texture_from_bmp(inactive_texture_fn, p_view->renderer, FALSE);
    SDL_button_t* button = SDL_BUTTON_create(TRUE, cb, b_texture, d_texture, location, value);
    if (button == NULL)
    {
        return FALSE;
    }
    else
    {
        p_view->buttons[p_view->button_count] = button;
        p_view->button_count++;
        return TRUE;
    }
}

void SDL_LOAD_WINDOW_destroy_view(SDL_LOAD_WINDOW_view_t* p_view)
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

void SDL_LOAD_WINDOW_draw_view(SDL_LOAD_WINDOW_view_t* p_view)
{
    if(p_view == NULL){
        return;
    }
    
    SDL_SetRenderDrawColor(p_view->renderer, 150, 150, 150, 255);
    SDL_RenderClear(p_view->renderer);

    for (int i=0; i < SDL_MAX_SAVES; i++)
    {
        if (p_view->active_marks[i] != NULL)
        {
            p_view->active_marks[i]->is_active = FALSE;
        }
    }

    if (gs_active_slot > 0)
    {
        p_view->active_marks[gs_active_slot - 1]->is_active = TRUE;
        p_view->load_button->is_active = TRUE;
    }
    else
    {
        p_view->load_button->is_active = FALSE;
    }

    for (int i=0; i < p_view->button_count; i++)
    {
        SDL_BUTTON_render(p_view->buttons[i], p_view->renderer);
    }    
    SDL_RenderPresent(p_view->renderer);
    
}

SDL_BUTTON_action_t SDL_LOAD_WINDOW_handle_event(SDL_LOAD_WINDOW_view_t* p_view, SDL_Event* event)
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
