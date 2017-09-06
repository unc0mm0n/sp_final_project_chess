/**
 * CLI.h
 *
 * The CLI interface of the game.
 */

#ifndef CLI_IMP
#define CLI_IMP

#include "MANAGER.h"
#include "SETTINGS.h"
#include "GAME.h"

/**
 * Handle the event of a manager quitting, by quitting the application.
 */
void CLI_handle_quit();

/**
 * return a suitable settings_agent for the CLI.
 *
 * @return MANAGER_settings_agent_t a settings agent for the CLI.
 */
MANAGER_settings_agent_t CLI_get_settings_agent();

/**
 * return a suitable play_agent for the CLI.
 *
 * @return MANAGER_play_agent_t a play agent for the CLI.
 */
MANAGER_play_agent_t CLI_get_play_agent();

/**
 * print a snapshot of the game.
 */
void CLI_print_board(const GAME_board_t* p_board);

/**
 * prompt the user for a single settings command through the CLI.
 * Blocks until a recognized command is given.
 *
 * @param p_a_settings the current game settings
 */
MANAGER_agent_settings_command_t CLI_prompt_settings_command(const SETTINGS_settings_t* p_a_settings);

/**
 * prompt the user for a single play command through the CLI.
 * Blocks until a recognized command is given.
 *
 * @param p_a_board the board on which the user should play.
 */
MANAGER_agent_play_command_t CLI_prompt_play_command(const GAME_board_t* p_a_board);

/**
 * Handle the result of a settings command
 *
 * @param command the command that was issued
 * @param response the response to the commmand
 */
void CLI_handle_settings_command_response(MANAGER_agent_settings_command_t command, MANAGER_agent_settings_command_response_t response);

/**
 * Handle the result of a play command
 *
 * @param command the command that was issued
 * @param response the response to the commmand
 */
void CLI_handle_play_command_response(MANAGER_agent_play_command_t command, MANAGER_agent_play_command_response_t response);

/**
 * Convert square to str notation <rank,file> with upper case 
 * files, which is the same notation used in move input, and 
 * place it in given buffer. 
 * 
 * @param sq 
 * @param output 
 */
void CLI_sq_to_str(square sq, char output[6]);

#endif /* CLI_IMP */
