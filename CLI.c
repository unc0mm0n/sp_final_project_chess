#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "CLI.h"

#define MAX_INPUT_SIZE (50)
#define SPLIT_TOKEN ("\n\t\r ")

static BOOL gs_board_printed; // evil global
static BOOL gs_settings_intro_printed; // evil global
static char gs_command_buffer[MAX_INPUT_SIZE]; // holds command

square _CLI_parse_square(char* token)
{
    if (strlen(token) != 5 || token[0] != '<' || token[2] != ',' || token[4] != '>')
    {
        return GAME_NO_SQUARE;
    }

    int file = token[3] - 'A';
    int rank = token[1] - '1';

    square sq = SQ_FROM_FILE_RANK(file, rank);
    return sq;
}

void CLI_sq_to_str(square sq, char p_o_str[6])
{
    p_o_str[0] = '<';
    p_o_str[1] = SQ_TO_RANK(sq) + '1';
    p_o_str[2] = ',';
    p_o_str[3] = SQ_TO_FILE(sq) + 'A';
    p_o_str[4] = '>';
    p_o_str[5] = '\0';
}

BOOL _CLI_is_int(const char* str) {
    if (str == NULL) {
        return 0;
    }
    char *p_c = (char *)str;
    BOOL has_digits = 0; 

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
    fseek(stdin, 0, SEEK_END);
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
    printf("   A B C D E F G H\n");
} 

void CLI_print_settings(const SETTINGS_settings_t* p_settings)
{
    if (p_settings->game_mode == 1) 
    {
        printf("SETTINGS:\n");
        printf("GAME_MODE: 1\n");
        printf("DIFFICULTY_LVL: %d\n", p_settings->difficulty);
        if (p_settings->user_color == WHITE)
        {
            printf("USER_CLR: WHITE\n");
        }
        else 
        {
            printf("USER_CLR: BLACK\n");
        }
    }
    else
    {
        printf("SETTINGS:\n");
        printf("GAME_MODE: 2\n");
    }
}

MANAGER_agent_settings_command_t CLI_prompt_settings_command(const SETTINGS_settings_t* p_a_settings)
{
    char* token;
    MANAGER_agent_settings_command_t command;

    _CLI_fflush_line();
    if (!gs_settings_intro_printed)
    {
        printf("Specify game setting or type 'start' to begin a game with the current setting:\n");
        gs_settings_intro_printed = TRUE;
    }
    printf("<settings> ");
    fgets(gs_command_buffer, MAX_INPUT_SIZE, stdin);
    //_CLI_fflush_line();

    if (gs_command_buffer[0] == '\n')
    {
        command.type = MANAGER_SETTINGS_COMMAND_TYPE_NONE;    
    }
    else
    {
        token = strtok(gs_command_buffer,SPLIT_TOKEN);
        if (strcmp(token, "game_mode") == 0)
        {
            command.type = MANAGER_SETTINGS_COMMAND_TYPE_CHANGE_SETTING;
            command.data.change_setting.setting = SETTINGS_SETTING_GAME_MODE; 
            command.data.change_setting.value = atoi(strtok(NULL, SPLIT_TOKEN)); // todo verify integer
        }
        else if (strcmp(token, "difficulty") == 0)
        {
            command.type = MANAGER_SETTINGS_COMMAND_TYPE_CHANGE_SETTING;
            command.data.change_setting.setting = SETTINGS_SETTING_DIFFICULTY; 
            command.data.change_setting.value = atoi(strtok(NULL, SPLIT_TOKEN)); // todo verify integer
        }
        else if (strcmp(token, "user_color") == 0)
        {
            command.type = MANAGER_SETTINGS_COMMAND_TYPE_CHANGE_SETTING;
            command.data.change_setting.setting = SETTINGS_SETTING_USER_COLOR; 
            command.data.change_setting.value = atoi(strtok(NULL, SPLIT_TOKEN)); // todo verify integer
        }
        else if (strcmp(token, "load") == 0)
        {
            command.type = MANAGER_SETTINGS_COMMAND_TYPE_LOAD;
            command.data.filename = strtok(NULL, SPLIT_TOKEN); 
            assert(0); // not yet implemented
        }
        else if (strcmp(token, "default") == 0)
        {
            command.type = MANAGER_SETTINGS_COMMAND_TYPE_DEFAULT_SETTINGS; 
        }
        else if (strcmp(token, "quit") == 0)
        {
            printf("Exiting...\n");
            command.type = MANAGER_SETTINGS_COMMAND_TYPE_QUIT; 
        }
        else if (strcmp(token, "start") == 0)
        {
            command.type = MANAGER_SETTINGS_COMMAND_TYPE_START_GAME; 
        }
        else if (strcmp(token, "print_settings") == 0)
        {
            CLI_print_settings(p_a_settings);
            command.type = MANAGER_SETTINGS_COMMAND_TYPE_NONE; 
        }
        else
        {
            command.type = MANAGER_SETTINGS_COMMAND_TYPE_NONE;
        }
    }

    return command;
}

MANAGER_agent_play_command_t CLI_prompt_play_command(const GAME_board_t* p_a_board)
{
    COLOR player = GAME_current_player(p_a_board);

    if (!gs_board_printed)
    {
        CLI_print_board(p_a_board);
        gs_board_printed = TRUE;
    }

    if (player == WHITE)
    {
        printf("white player - enter your move:\n");
    }
    else
    {
        printf("black player - enter your move:\n");
    }

    MANAGER_agent_play_command_t command;
    char* token;

    fgets(gs_command_buffer, MAX_INPUT_SIZE, stdin);
    //_CLI_fflush_line();
    _CLI_fflush_line();

    if (gs_command_buffer[0] == '\n')
    {
        command.type = MANAGER_PLAY_COMMAND_TYPE_NONE; 
    }
    else
    {
        token = strtok(gs_command_buffer,SPLIT_TOKEN);
        if (strcmp(token, "move") == 0)
        {
            GAME_move_t move;
            command.type = MANAGER_PLAY_COMMAND_TYPE_MOVE;
            square sq_from = _CLI_parse_square(strtok(NULL, SPLIT_TOKEN)); // todo verify integer
            move.from = sq_from;
            token = strtok(NULL, SPLIT_TOKEN);
            if ((strcmp(token, "to")) !=0 || sq_from == 0x88)
            {
                command.type = MANAGER_PLAY_COMMAND_TYPE_NONE;
                return command;
            }
            square sq_to = _CLI_parse_square(strtok(NULL, SPLIT_TOKEN));
            move.to = sq_to;
            int before_back = PAWN_RANK(OTHER_COLOR(player));
            int back = LAST_RANK(player);
            if (SQ_TO_RANK(sq_from) == before_back && SQ_TO_RANK(sq_to) == back && p_a_board->pieces[sq_from] == PIECE_TYPE_PAWN)
            {
                BOOL chose_piece = FALSE;
                while (!chose_piece)
                {
                    printf("Pawn promotion- please replace the pawn by queen, rook, knight, bishop or pawn:\n"); // todo - remove the from pawn.

                    fgets(gs_command_buffer, MAX_INPUT_SIZE, stdin);
                    token = strtok(gs_command_buffer, SPLIT_TOKEN);
                    for (int piece = PIECE_TYPE_PAWN; piece <= PIECE_TYPE_QUEEN; piece++)
                    {
                        if ((strcmp(token, PIECE_desc_lut[piece].name) == 0) && PIECE_desc_lut[piece].can_promote_to)
                        {
                            move.promote = piece;            
                            chose_piece = TRUE;
                        }
                    }
                    if (!chose_piece)
                    {
                        printf("Invalid Type\n");
                    }
                }
            }
            command.data.move = move;

        }

        else if (strcmp(token, "undo") == 0)
        {
            command.type = MANAGER_PLAY_COMMAND_TYPE_UNDO;
        }
        else if (strcmp(token, "quit") == 0)
        {
            command.type = MANAGER_PLAY_COMMAND_TYPE_QUIT;
        }
        else if (strcmp(token, "reset") == 0)
        {
            command.type = MANAGER_PLAY_COMMAND_TYPE_RESET;
        }
        /*     else if (strcmp(token, "default") == 0)
               {
               command.type = MANAGER_SETTINGS_COMMAND_TYPE_DEFAULT_SETTINGS; 
               }
               else if (strcmp(token, "quit") == 0)
               {
               printf("Exiting...\n");
               command.type = MANAGER_SETTINGS_COMMAND_TYPE_QUIT; 
               }
               else if (strcmp(token, "start") == 0)
               {
               command.type = MANAGER_SETTINGS_COMMAND_TYPE_START_GAME; 
               }
               else if (strcmp(token, "print_settings") == 0)
               {
               command.type = MANAGER_PLAY_COMMAND_TYPE_NONE; 
               }*/
        else
        {
            printf("Invalid command.\n");
            command.type = MANAGER_PLAY_COMMAND_TYPE_NONE;
        }
    }

    return command;
}

void CLI_handle_settings_command_response(MANAGER_agent_settings_command_t command, MANAGER_agent_settings_command_response_t response)
{
    if (command.type == MANAGER_SETTINGS_COMMAND_TYPE_CHANGE_SETTING)
    {
        assert(response.has_output);
        switch(response.output.settings_change_result)
        {
            case SETTINGS_CHANGE_RESULT_INVALID_MODE:
                {
                    printf("Wrong game mode\n");
                    break;
                }
            case SETTINGS_CHANGE_RESULT_INVALID_COLOR:
                {
                    printf("Invalid color\n");
                    break;
                }
            case SETTINGS_CHANGE_RESULT_INVALID_DIFFICULTY:
                {
                    printf("Wrong difficulty level. The value should be between 1 to 5\n");
                    break;
                }
            case SETTINGS_CHANGE_RESULT_WRONG_MODE:
                {
                    printf("Command is not supported in current game mode\n");
                    break;
                }
            default:
                if (command.data.change_setting.setting == SETTINGS_SETTING_GAME_MODE)
                {
                    if (command.data.change_setting.value == 1)
                    {
                        printf("Game mode is set to 1 player\n");
                    }
                    else if (command.data.change_setting.value == 2)
                    {
                        printf("Game mode is set to 2 players\n");
                    }
                    else
                    {
                        assert(0);
                    }
                }

        }
    }
}

void CLI_handle_play_command_response(MANAGER_agent_play_command_t command, MANAGER_agent_play_command_response_t response)
{
    assert (command.type >= 0);

    switch(command.type)
    {
        case MANAGER_PLAY_COMMAND_TYPE_MOVE:
            {
                GAME_move_analysis_t analysis = response.output.move_data.move_result.move_analysis;
                switch(analysis.verdict)
                {
                    case GAME_MOVE_VERDICT_LEGAL: // legal move prints
                        {
                            switch( response.output.move_data.game_result)
                            {
                                case GAME_RESULT_DRAW:
                                    printf("The game ends in a tie\n");
                                    break;
                                case GAME_RESULT_BLACK_WINS:
                                    printf("Checkmate! black player wins the game\n");
                                    break;
                                case GAME_RESULT_WHITE_WINS:
                                    printf("Checkmate! white player wins the game\n");
                                    break;
                                default:
                                    if ((analysis.special_bm & GAME_SPECIAL_CHECK) > 0)
                                    {
                                        if (analysis.color == WHITE)
                                        {
                                            printf("Check: black King is threatend!\n");
                                        }
                                        else
                                        {
                                            printf("Check: white King is threatend!\n");
                                        }
                                    }
                                    break;
                            }

                            gs_board_printed = FALSE;
                            break;
                        }
                    case GAME_MOVE_VERDICT_ILLEGAL_SQUARE: // original square is invalid
                        {
                            printf("Invalid position on the board\n");
                            break;
                        }
                    case GAME_MOVE_VERDICT_NO_PIECE:
                        {
                            printf("The specified position does not contain your piece\n");
                            break;
                        }
                    default:
                        {
                            printf("Illegal move\n");
                            break;
                        }
                }
                break;
            }
        case MANAGER_PLAY_COMMAND_TYPE_UNDO:
            {

                switch(response.output.undo_data.undo_result)
                {
                    case MANAGER_UNDO_RESULT_SUCCESS:
                        { 
                            gs_board_printed = FALSE;
                            char from_str[6], to_str[6];
                            GAME_move_analysis_t analysis;
                            for (int i=0; i < 2; i++)
                            {
                                analysis = response.output.undo_data.undone_moves[i];
                                CLI_sq_to_str(analysis.move.from, from_str);
                                CLI_sq_to_str(analysis.move.to, to_str);
                                if (analysis.color == WHITE)
                                {
                                    printf("Undo move for player white: %s -> %s\n", to_str, from_str);
                                }
                                else
                                {
                                    printf("Undo move for player black: %s -> %s\n", to_str, from_str);
                                }
                            }
                            break;
                        }
                    case MANAGER_UNDO_RESULT_FAIL_TWO_PLAYERS:
                        {
                            printf("Undo command not available in 2 players mode\n");
                            break;
                        }
                    case MANAGER_UNDO_RESULT_FAIL_NO_HISTORY:
                        {
                            printf("Empty history, move cannot be undone\n");
                            break;
                        }
                    break;
                }
            }
        case MANAGER_PLAY_COMMAND_TYPE_QUIT:
            {
                printf("Exiting...\n");
                break;
            }
        case MANAGER_PLAY_COMMAND_TYPE_RESET:
            {
                gs_settings_intro_printed = FALSE;
                printf("Restarting...\n");
                break;
            }
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

