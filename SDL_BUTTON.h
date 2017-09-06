/**
 * Defines a button in the SDL interface
 */

#ifndef SDL_BUTTON_IMP
#define SDL_BUTTON_IMP

#include <SDL.h>

#include "DEFS.h"
#include "MANAGER.h"

#define BUTTON_WIDTH        (180)                                                                                 
#define BUTTON_HEIGHT       (60)                                                                                  
#define BUTTON_X            (10)                                                                                  
#define BUTTON_SPACING      (20)
#define BUTTON_AREA_HEIGHT  (BUTTON_HEIGHT + BUTTON_SPACING)
#define BUTTON_PADDING      (10)

/**
 * A simple button with callback.
 * Calling the handler on a mouse_up event will call the callback only if the button is set to active.
 */
typedef struct SDL_button_s
{
    BOOL is_active;
    MANAGER_agent_command_t (*cb)();   // function that happens when button is pressed.
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
SDL_button_t* SDL_BUTTON_create(BOOL is_active, MANAGER_agent_command_t (*cb)(), SDL_Texture* a_texture, SDL_Texture* i_texture, SDL_Rect location);

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
 * @return MANAGER_agent_command_t command make by button, if no command is made the type will be invalid
 */
MANAGER_agent_command_t SDL_BUTTON_handle_event(SDL_button_t* p_button, SDL_Event* event);

/**
 * Render the button in given renderer, based on it's given textures, location and active state
 */
void SDL_BUTTON_render(SDL_button_t* p_button, SDL_Renderer* renderer);

#endif
