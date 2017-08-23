/**
 * MANAGER.h 
 *  
 * Encapsulates the GAME to manage input/output from 
 * players/computers and game settings.
 */

#ifndef MANAGER_IMP
#define MANAGER_IMP

#include "GAME.h"
#include "SETTINGS.h"

/**
 * Possible states for the manager
 */
typedef enum MANAGER_STATE_S
{
    MANAGER_STATE_SETTINGS,
    MANAGER_STATE_PLAY,
    MANAGER_STATE_INVALID
} MANAGER_STATE_E;

/**
 * All available command types to the manager.
 */
typedef enum MANAGER_COMMAND_TYPES_S
{
    MANAGER_COMMAND_TYPE_CHANGE_SETTING,
    MANAGER_COMMAND_TYPE_START_GAME,
    MANAGER_COMMAND_TYPE_MOVE,
    MANAGER_COMMAND_TYPE_GET_MOVES,
    MANAGER_COMMAND_TYPE_LOAD,
    MANAGER_COMMAND_TYPE_SAVE,
    MANAGER_COMMAND_TYPE_UNDO,
    MANAGER_COMMAND_TYPE_RESET,
    MANAGER_COMMAND_TYPE_QUIT
} MANAGER_COMMAND_TYPES_E;

/**
 * A single agent command, used by the manager.
 */
typedef struct MANAGER_agent_command_s
{
    MANAGER_COMMAND_TYPES_E type;

    // Will hold the required data for a single command based on its type.
    union {
       // CHANGE_SETTING requires setting to change and new value
       struct {
           SETTINGS_SETTING_E setting;
           int value;
       } change_setting;

       // MOVE command requires the move to make
       GAME_move_t move;

       // GET_MOVES command requires square to get moves for
       square sq;

       // SAVE and LOAD commands attempt to save and load a file
       struct {
           char * name;
           int length;
       } filename;
    } data;
} MANAGER_agent_command_t;

/**
 * Union of all possible results if applicable, separable by 
 * command type. 
 * 
 */
typedef struct  MANAGER_agent_command_output_s
{
    BOOL has_output;
    union {
        SETTINGS_CHANGE_RESULT_E settings_change_result; // CHANGE_SETTING_COMMAND
        GAME_MOVE_RESULTS_E move_result; // MOVE command
        GAME_move_full_t * possible_moves; // GET_MOVES command
        BOOL load_succesful; // LOAD command (not yet supported)
        BOOL save_succesful; // SAVE command (not yet supported)
        GAME_move_full_t * undone_moves; // UNDO command
    } output;
} MANAGER_agent_command_output_t;

/**
 * An agent the manager calls to get required information
 */
typedef struct MANAGER_agent_s
{
    // Will be called every game loop iteration to get command from agent. Borad will be NULL if not in PLAY state.
    MANAGER_agent_command_t (*prompt_command)(MANAGER_STATE_E state, const GAME_board_t* board, const SETTINGS_settings_t* settings); 

     // Will be called with the output of the command above, the has_output flag specifies whether any output was given.
     void (*handle_command_result)(MANAGER_agent_command_t command, MANAGER_agent_command_output_t result);
} MANAGER_agent_t;


/**
 * The game managing object.
 */
typedef struct MANAGER_managed_game_s
{
    MANAGER_STATE_E state;                    // current state of the manager
    GAME_board_t board;                       // board holding the 
    SETTINGS_settings_t settings;             // settings used in the game
    MANAGER_agent_t settings_agent;           // agent used in settings state
    MANAGER_agent_t play_agents[NUM_PLAYERS]; // agent BLACK and agent WHITE will be called respectively

} MANAGER_managed_game_t;

/**
 * Create and return a new managed game. 
 * The play_agents will be automatically filled according to the 
 * settings, using either an ai agent or the agent given as 
 * settings_agent. 
 * 
 * @param settings_agent the agent used to set the settings.
 * 
 * @return MANAGER_managed_game_t* 
 */
MANAGER_managed_game_t * MANAGER_new_managed_game(MANAGER_agent_t settings_agent);

/**
 * Start a new game.
 * This is the main game loop! This function will only return 
 * when the game should be over and all game resources freed 
 * (but not agent resources). 
 * 
 * @param p_a_manager pointer to game manager.
 */
void MANAGER_start_game(MANAGER_managed_game_t * p_a_manager);

/**
 * Return true if an undo command is valid. 
 * This is required for the GUI to know whether to activate the 
 * undo button without actually trying to undo. 
 * 
 * @param p_a_manager 
 * 
 * @return BOOL 
 */
BOOL MANAGER_can_undo(MANAGER_managed_game_t * p_a_manager);

#endif /*MANAGER_IMP*/
