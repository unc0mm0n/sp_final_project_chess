#include <assert.h>
#include <stdio.h>
#include <string.h>

#include "CLI.h"

#define MAX_INPUT_SIZE (20)
#define SPLIT_TOKEN ("\n\t\r ")

static BOOL gs_board_printed; // evil global
static BOOL gs_settings_intro_printed; // evil global
static char gs_command_buffer[MAX_INPUT_SIZE]; // holds command

BOOL _CLI_is_int(const char* str) {
    if (str == NULL) {
        return 0;
    }
    char *p_c = (char *)str;
    bool has_digits = 0; 

    if (*p_c == '-') { // We only accept p_c at the start.
        p_c++;
    }

    while (*p_c != '\0' && *p_c != '\n') { // Make sure all characters are digits
        if (*p_c < '0' || *p_c > '9') {
            return 0;
        }

        p_c++;
        has_digits++;
    }
    return (has_digits > 0); // We want to verify at least one character is a digit.
}

/* flush a line from STDIN */
void _CLI_fflush_line()
{
    char c;
    while ((c = getchar()) && c != EOF && c != '\n');
}

MANAGER_settings_agent_t CLI_get_settings_agent()
{
    gs_settings_intro_printed = FALSE;
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
    /* debug color print * /
    printf("\n");
    for (int i=NUM_RANKS-1; i >= 0; i--)                                                                      
    {                                                                                                         
        for (int j=0; j < NUM_RANKS; j++)                                                                     
        {                                                                                                     
            printf("%d ", p_a_board->colors[ SQ_FROM_FILE_RANK(j,i)]);                           
        }                                                                                                     
        printf("\n");                                                                                         
    }                                                                                                         
    printf("\n");
    */

    for (int i = NUM_FILES - 1; i >= 0; i--)                                     
    {
        printf("%d| ", i+1);                                                                           
        for (int j=0; j < NUM_RANKS; j++)
        {                                                                                                     
            printf("%c ", GAME_piece_letter_at(p_a_board, SQ_FROM_FILE_RANK(j,i)));                           
        }
        printf("|\n");
    }
    printf("  -----------------\n");
    printf("    A B C D E F G H\n");
} 

void CLI_print_settings(const SETTINGS_settings_t* p_settings)
{
    if (p_settings->game_mode == 1) 
    {
        printf("SETTINGS:\n");
        printf("GAME_MODE: 1\n");
    }
    else
    {
        printf("SETTINGS:\n");
        printf("GAME_MODE: 2\n");
        printf("DIFFICULTY_LVL: %d\n", p_settings->difficulty);
        if (p_settings->user_clr == WHITE)
        {
            printf("USER_CLR: WHITE\n");
        }
        else 
        {
            printf("USER_CLR: BLACK\n");
        }
    }
}

MANAGER_agent_settings_command_t CLI_prompt_settings_command(const SETTINGS_settings_t* p_a_settings)
{
    MANAGER_agent_settings_command_t command;

    if (!gs_settings_intro_printed)
    {
        printf("Specify game setting or type 'start' to begin a game with the current setting:\n");
        gs_settings_intro_printed = TRUE;
    }
    
    fgets(gs_command_buffer, MAX_INPUT_SIZE, stdin);

    if (*str == '\n') {
        command.type = MANAGER_SETTINGS_COMMAND_TYPE_NONE;    
    }
    else
    {
        token = strtok(gs_command_buffer,SPLIT_TOKEN);
    }

    if (strcmp(token, "game_mode"))
    {
        command.type = MANAGER_SETTINGS_COMMAND_TYPE_CHANGE_SETTING;
        command.data.change_setting.setting = SETTINGS_SETTING_GAME_MODE; 
        command.data.change_setting.value = atoi(strtok(NULL, SPLIT_TOKEN)); // todo verify integer
    }
    else if (strcmp(token, "difficulty"))
    {
        command.type = MANAGER_SETTINGS_COMMAND_TYPE_CHANGE_SETTING;
        command.data.change_setting.setting = SETTINGS_SETTING_DIFFICULTY; 
        command.data.change_setting.value = atoi(strtok(NULL, SPLIT_TOKEN)); // todo verify integer
    }
    else if (strcmp(token, "user_color"))
    {
        command.type = MANAGER_SETTINGS_COMMAND_TYPE_CHANGE_SETTING;
        command.data.change_setting.setting = SETTINGS_SETTING_USER_COLOR; 
        command.data.change_setting.value = atoi(strtok(NULL, SPLIT_TOKEN)); // todo verify integer
    }
    else if (strcmp(token, "load"))
    {
        command.type = MANAGER_SETTINGS_COMMAND_TYPE_LOAD;
        command.data.filename = strtok(NULL, SPLIT_TOKEN); 
        assert(0); // not yet implemented
    }
    else if (strcmp(token, "default"))
    {
        command.type = MANAGER_SETTINGS_COMMAND_TYPE_DEFAULT_SETTINGS; 
    }
    else if (strcmp(token, "quit"))
    {
        command.type = MANAGER_SETTINGS_COMMAND_TYPE_QUIT; 
    }
    else if (strcmp(token, "start"))
    {
        command.type = MANAGER_SETTINGS_COMMAND_TYPE_START_GAME; 
    }
    else if (strcmp(token, "print_settings"))
    {
        _CLI_print_settings(p_a_settings);
        command.type = MANAGER_SETTINGS_COMMAND_TYPE_NONE; 
    }
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
        command.type = MANAGER_PLAY_COMMAND_TYPE_UNDO;
        return command;
    }
    move.from = SQ_FROM_FILE_RANK(move_digits / 1000 - 1, move_digits / 100 % 10 - 1);
    move.to = SQ_FROM_FILE_RANK(move_digits / 10 % 10 - 1, move_digits % 10 - 1);
    move.promote = PIECE_TYPE_QUEEN;
    command.type = MANAGER_PLAY_COMMAND_TYPE_MOVE;
    command.data.move = move;
    return command;
}

PIECE_TYPE_E _CLI_prompt_promote_piece(const GAME_board_t* p_a_board, GAME_move_result_t move_result)
{
    /* TODO: tmp just to test game */
    assert (p_a_board->turn >= 0);
    assert(move_result.played == FALSE);  // will actually stay, to make sure we are not prompted
                                                 // on a played move.

    int move_digits;
    printf("Enter piece promote: ");
    scanf("%d", &move_digits);
    _CLI_fflush_line();
    // ugly hack for easy testing

    return move_digits;
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
    
    if (response.has_output)
    {
        gs_board_printed = FALSE;
    }
    if (response.has_output)
    {
     //   printf("response: from %x to %x verdict %d\n", response.output.move_result.move_analysis.move.from, response.output.move_result.move_analysis.move.to, response.output.move_result.move_analysis.verdict);
    }
}

void CLI_sq_to_string(square sq, char output[3])
{

    char file = SQ_TO_FILE(sq);
    char rank = SQ_TO_RANK(sq);
    
    output[0] = file + 'A';
    output[1] = rank + '1';
    output[2] = '\0';
}
 
