#include <assert.h>
#include <stdio.h>

#include "CLI.h"

static BOOL gs_board_printed; // evil global

/* flush a line from STDIN */
void _CLI_fflush_line()
{
    char c;
    while ((c = getchar()) && c != EOF && c != '\n');
}

MANAGER_settings_agent_t CLI_get_settings_agent()
{
    MANAGER_settings_agent_t agent;
    agent.prompt_settings_command = CLI_prompt_settings_command;
    agent.handle_settigns_command_response = CLI_handle_settings_command_response;
    agent.get_play_agent = CLI_get_play_agent;

    return agent;
}

MANAGER_play_agent_t CLI_get_play_agent()
{
    MANAGER_play_agent_t agent;
    agent.prompt_play_command = CLI_prompt_play_command;
    agent.handle_play_command_response = CLI_handle_play_command_response;

    return agent;
}

void CLI_print_board(const GAME_board_t* p_a_board)
{
    printf("\n");
    for (int i=NUM_RANKS-1; i >= 0; i--)                                                                      
    {                                                                                                         
        for (int j=0; j < NUM_RANKS; j++)                                                                     
        {                                                                                                     
            printf("%c ", GAME_piece_letter_at(p_a_board, SQ_FROM_FILE_RANK(j,i)));                           
        }                                                                                                     
        printf("\n");                                                                                         
    }                                                                                                         
} 

MANAGER_agent_settings_command_t CLI_prompt_settings_command(const SETTINGS_settings_t* p_a_settings)
{
    /* TODO: tmp to just call game start */
    assert (p_a_settings->difficulty >= 0);
    MANAGER_agent_settings_command_t command;
    command.type = MANAGER_SETTINGS_COMMAND_TYPE_START_GAME;
    return command;
}

MANAGER_agent_play_command_t CLI_prompt_play_command(const GAME_board_t* p_a_board)
{
    /* TODO: tmp just to test game */
    assert (p_a_board->turn >= 0);
    printf("Game result: %d\n", GAME_get_result(p_a_board));
    int move_digits;
    GAME_move_t move;
    MANAGER_agent_play_command_t command;

    if (!gs_board_printed)
    {
        CLI_print_board(p_a_board);
        gs_board_printed = TRUE;
    }
    printf("Enter move: ");
    scanf("%d", &move_digits);
    _CLI_fflush_line();
    // ugly hack for easy testing
    if (move_digits == 0)
    {
        command.type = MANAGER_PLAY_COMMAND_TYPE_QUIT;
        return command;
    }
    move.from = SQ_FROM_FILE_RANK(move_digits / 1000 - 1, move_digits / 100 % 10 - 1);
    move.to = SQ_FROM_FILE_RANK(move_digits / 10 % 10 - 1, move_digits % 10 - 1);
    move.promote = PIECE_TYPE_QUEEN;
    command.type = MANAGER_PLAY_COMMAND_TYPE_MOVE;
    command.data.move = move;
    return command;
}

void CLI_handle_settings_command_response(MANAGER_agent_settings_command_t command, MANAGER_agent_settings_command_response_t response)
{
    assert(command.type >= 0);
    assert(response.has_output >= 0);
    /* TODO: implement this */
    return;
}

void CLI_handle_play_command_response(MANAGER_agent_play_command_t command, MANAGER_agent_play_command_response_t response)
{
    assert (command.type >= 0);
    if (response.has_output && response.output.move_result == GAME_MOVE_RESULT_TYPE_SUCCESS)
    {
        gs_board_printed = FALSE;
    }
}
 
