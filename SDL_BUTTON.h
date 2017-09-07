/**
 * Defines a button in the SDL interface
 */

#ifndef SDL_BUTTON_IMP
#define SDL_BUTTON_IMP

#include <SDL.h>

#include "DEFS.h"
#include "MANAGER.h"
#include "SDL_UTILS.h"

#define BUTTON_WIDTH        (180)                                                                                 
#define BUTTON_HEIGHT       (60)                                                                                  
#define BUTTON_X            (10)                                                                                  
#define BUTTON_SPACING      (20)
#define BUTTON_AREA_HEIGHT  (BUTTON_HEIGHT + BUTTON_SPACING)
#define BUTTON_PADDING      (10)

/**
 *  Possible actions envoked by a button press
 */
typedef enum SDL_BUTTON_ACTION_TYPE_S
{
    SDL_BUTTON_ACTION_NONE,              // do nothing
    SDL_BUTTON_ACTION_CHANGE_STATE,      // Change the interface state
    SDL_BUTTON_ACTION_SEND_SETTINGS_CMD, // send settings command
    SDL_BUTTON_ACTION_SEND_PLAY_CMD      // send play command
} SDL_BUTTON_ACTION_TYPE_E;

/**
 * The result returned by a callback
 */
typedef struct SDL_BUTTON_action_s
{
    SDL_BUTTON_ACTION_TYPE_E action;
    union
    {
        MANAGER_agent_settings_command_t settings_cmd;
        MANAGER_agent_play_command_t play_cmd;
        SDL_INTERFACE_STATE_E new_state;
    };
} SDL_BUTTON_action_t;

/**
 * A simple button with callback.
 * Calling the handler on a mouse_up event will call the callback only if the button is set to active.
 */
typedef struct SDL_button_s
{
    BOOL is_active;
    SDL_BUTTON_action_t (*cb)(int);   // function that happens when button is pressed.
    int value;                            // will be passed to the callback
    SDL_Texture* active_texture;
    SDL_Texture* inactive_texture;
    SDL_Rect location;
} SDL_button_t;

/**
 * return a pointer to a button
 *
 * @param is_active if the button is initially active
 * @param cb callback to be called if the button is active
 * @param a_texture pointer to texture of the button when active
 * @param i_texture pointer to texture of the button when inactive
 * @param location rect of the location and dimensions of button
 *
 * return SDL_button_t* pointer to button
 */
SDL_button_t* SDL_BUTTON_create(BOOL is_active, SDL_BUTTON_action_t (*cb)(int), SDL_Texture* a_texture, SDL_Texture* i_texture, SDL_Rect location);

/**
 * destroy button, freeing it.
 * NULL sage.
 *
 * @param p_button pointer to button
 */
void SDL_BUTTON_destroy(SDL_button_t* p_button);

/**
 * Handle even returning an agent_command or no if there is no CB, the button is inactive, or wasn't pressed.
 *
 * @param p_button pointer to button.
 * @param event the event that occured.
 * 
 * @return SDL_BUTTON_action_t action to be made by the button press.
 */
SDL_BUTTON_action_t SDL_BUTTON_handle_event(SDL_button_t* p_button, SDL_Event* event);

/**
 * Render the button in given renderer, based on it's given textures, location and active state
 */
void SDL_BUTTON_render(SDL_button_t* p_button, SDL_Renderer* renderer);

#endif
