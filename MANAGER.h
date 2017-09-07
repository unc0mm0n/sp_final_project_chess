/**
 * MANAGER.h 
 *  
 * Encapsulates the GAME to manage input/output from 
 * players/computers and game settings.
 */

#ifndef MANAGER_IMP
#define MANAGER_IMP

#include "DEFS.h"
#include "GAME.h"
#include "SETTINGS.h"

#define MANAGER_UNDO_COUNT (6) // number of half moves undone

/**
 * Possible states for the manager
 */
typedef enum MANAGER_STATE_S
{
    MANAGER_STATE_INIT,      // initial state before start was called
    MANAGER_STATE_SETTINGS,  // state for settings change after manager start was called
    MANAGER_STATE_PRE_PLAY,  // state after start was called where settings are used to populate everything
    MANAGER_STATE_PLAY,      // state for playing the game after game start was called
    MANAGER_STATE_QUIT,      // state for quitting the game and shutting down the manager
    MANAGER_STATE_INVALID    // state following illegal operations [TODO: probably remove]
} MANAGER_STATE_E;

/**
 * Available command types to the manager in SETTINGS state.
 */
typedef enum MANAGER_SETTINGS_COMMAND_TYPE_S
{
    MANAGER_SETTINGS_COMMAND_TYPE_NONE,
    MANAGER_SETTINGS_COMMAND_TYPE_CHANGE_SETTING,
    MANAGER_SETTINGS_COMMAND_TYPE_DEFAULT_SETTINGS,
    MANAGER_SETTINGS_COMMAND_TYPE_START_GAME,
    MANAGER_SETTINGS_COMMAND_TYPE_LOAD,
    MANAGER_SETTINGS_COMMAND_TYPE_QUIT
} MANAGER_SETTINGS_COMMAND_TYPE_E;

/**
 * Available command types to the manager in SETTINGS state.
 */
typedef enum MANAGER_PLAY_COMMAND_TYPE_S
{
    MANAGER_PLAY_COMMAND_TYPE_NONE,
    MANAGER_PLAY_COMMAND_TYPE_MOVE,
    MANAGER_PLAY_COMMAND_TYPE_CASTLE,
    MANAGER_PLAY_COMMAND_TYPE_GET_MOVES,
    MANAGER_PLAY_COMMAND_TYPE_SAVE,
    MANAGER_PLAY_COMMAND_TYPE_UNDO,
    MANAGER_PLAY_COMMAND_TYPE_RESET, // go back to settings
    MANAGER_PLAY_COMMAND_TYPE_RESTART, // start a new game with same settings
    MANAGER_PLAY_COMMAND_TYPE_QUIT
} MANAGER_PLAY_COMMAND_TYPE_E;

/**
 * Result of manager command undo
 */
typedef enum MANAGER_UNDO_RESULT_S
{
    MANAGER_UNDO_RESULT_SUCCESS,
    MANAGER_UNDO_RESULT_FAIL_TWO_PLAYERS,
    MANAGER_UNDO_RESULT_FAIL_NO_HISTORY
} MANAGER_UNDO_RESULT_E;

/**
 * Result of manager command castle
 */
typedef enum MANAGER_CASTLE_RESULT_S
{
    MANAGER_CASTLE_RESULT_SUCCESS,
    MANAGER_CASTLE_RESULT_FAIL_NO_ROOK,
    MANAGER_CASTLE_RESULT_FAIL
} MANAGER_CASTLE_RESULT_E;

/**
 * A single agent settings command, used by the manager.
 */
typedef struct MANAGER_agent_settings_command_s
{
    MANAGER_SETTINGS_COMMAND_TYPE_E type;

    // Will hold the required data for a single command based on its type.
    union {
       // CHANGE_SETTING requires setting to change and new value
       struct {
           SETTINGS_SETTING_E setting;
           int value;
       } change_setting;

       // LOAD command requires file to attempt to load
       char*  filename;
    } data;
} MANAGER_agent_settings_command_t;

/**
 * Union of all possible results if applicable, separable by 
 * command type. 
 * 
 */
typedef struct  MANAGER_agent_settings_command_response_s
{
    BOOL has_output;      // TRUE if the command has output (which should be distinguishable by command type).
    union {
        SETTINGS_CHANGE_RESULT_E settings_change_result; // CHANGE_SETTING_COMMAND
        BOOL load_succesful; // LOAD command
    } output;
} MANAGER_agent_settings_command_response_t;

/**
 * A single agent play command, used by the manager.
 */
typedef struct MANAGER_agent_play_command_s
{
    MANAGER_PLAY_COMMAND_TYPE_E type;

    // Will hold the required data for a single command based on its type.
    union {

       // MOVE command requires the move to make
       GAME_move_t move;

       // GET_MOVES command requires square to get moves for
       // CASTLE command gives square of the rook
       square sq;

       // SAVE acommand requires path of file to save to
       struct {
           char * name;
           int length;
       } filename;
    } data;
} MANAGER_agent_play_command_t;

/**
 * Union of all possible results if applicable, separable by 
 * command type. 
 * 
 */
typedef struct  MANAGER_agent_play_command_response_s
{
    BOOL has_output;      // TRUE if the command has output (which should be distinguishable by command type).
    union {
        struct { // MOVE command
            GAME_move_result_t     move_result;
            GAME_RESULT_E          game_result;    
        } move_data;

        struct {  // GET_MOVES command
            BOOL display_hints;             // true if given the current difficulty hint displaying is advised.
            COLOR player_color;
            GAME_move_analysis_t * moves;   // list of possible moves, should be freed manually.
        } get_moves_data;

        struct {  // CASTLE command
            MANAGER_CASTLE_RESULT_E castle_result;
            GAME_move_result_t move;
        } castle_data;

        BOOL            save_succesful; // SAVE command

        struct {
            MANAGER_UNDO_RESULT_E      undo_result;
            GAME_move_analysis_t    undone_moves[2];   // UNDO command
        } undo_data;

    } output;
} MANAGER_agent_play_command_response_t;

/**
 * The possible command types
 */
typedef enum MANAGER_COMMAND_TYPE_S
{
    MANAGER_COMMAND_TYPE_INVALID,
    MANAGER_COMMAND_TYPE_SETTINGS_COMMAND,
    MANAGER_COMMAND_TYPE_PLAY_COMMAND
} MANAGER_COMMAND_TYPE_E;

/**
 * union of the commands for convenience
 */
typedef struct MANAGER_agent_command_u
{
    MANAGER_COMMAND_TYPE_E type;

    union
    {
        MANAGER_agent_play_command_t play_command;
        MANAGER_agent_settings_command_t settings_command;
    } cmd;
} MANAGER_agent_command_t;

/**
 * An agent the manager calls to get play related commands
 */
typedef struct MANAGER_play_agent_s
{
    // Will be called every game loop iteration while in PLAY state to get relevant command from agent.
    MANAGER_agent_play_command_t (*prompt_play_command)(const GAME_board_t* board);

     // Will be called with the output of the command above.
     void (*handle_play_command_response)(MANAGER_agent_play_command_t command, MANAGER_agent_play_command_response_t response);
} MANAGER_play_agent_t;

/**
 * An agent the manager calls to get settings related commands
 */
typedef struct MANAGER_settings_agent_s
{
    // Will be called every game loop iteration while in SETTINGS state to get relevant command from agent.
    MANAGER_agent_settings_command_t (*prompt_settings_command)(const SETTINGS_settings_t *settings); 

    // Will be called with the output of the command above.
    void (*handle_settigns_command_response)(MANAGER_agent_settings_command_t command, MANAGER_agent_settings_command_response_t response);

    // Will be called when game start is called, the settings agent should give it's accompanying play agent.
    MANAGER_play_agent_t (*get_play_agent)();
} MANAGER_settings_agent_t;


/**
 * The game managing object.
 */
typedef struct MANAGER_managed_game_s
{
    MANAGER_STATE_E state;                    // current state of the manager
    GAME_board_t* p_board;                    // board holding the 
    SETTINGS_settings_t* p_settings;          // settings used in the game
    MANAGER_settings_agent_t settings_agent;           // agent used in settings state
    MANAGER_play_agent_t play_agents[NUM_PLAYERS]; // agent BLACK and agent WHITE will be called respectively
    void (*handle_quit)();                         // Called when the manager quits and frees itself
    int undo_count;                                // how many undos are available.

} MANAGER_managed_game_t;

/**
 * Create and return a new managed game. 
 * The play_agents will be automatically filled according to the 
 * settings, using either an ai agent or the agent given as 
 * settings_agent when start_game is called. 
 * 
 * @param settings_agent the agent used to set the settings.
 * @paeam quit function that frees all relevant resources.
 * 
 * @return MANAGER_managed_game_t* 
 */
MANAGER_managed_game_t * MANAGER_new_managed_game(MANAGER_settings_agent_t settings_agent, void (*quit)());

/**
 * Free a managed game and all accompanying resources.
 * 
 * @param p_a_manager pointer to manager to free.
 */
void MANAGER_free_managed_game(MANAGER_managed_game_t * p_a_manager); 

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
