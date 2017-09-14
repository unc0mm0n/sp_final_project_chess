#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "CLI.h"

#define MAX_INPUT_SIZE (1024)
#define SPLIT_TOKEN ("\n\t\r ")

/** Global variables **/

// we allow ourselves more freedom with globals here as no more than one terminal CLI can run at a time anyway.
static BOOL gs_board_printed; // evil global
static BOOL gs_settings_intro_printed; // evil global
static char gs_command_buffer[MAX_INPUT_SIZE]; // holds command

/***** Private functions *****/

// Parse a string into a square.
square _CLI_parse_square(char *token)
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

// return true if and only if the string is an integer
BOOL _CLI_is_int(const char *str)
{
    if (str == NULL)
    {
        return 0;
    }
    char *p_c = (char *)str;
    BOOL has_digits = 0;

    if (*p_c == '-') // We only accept p_c at the start.
    {
        p_c++;
    }

    while (*p_c != '\0' && *p_c != '\n') // Make sure all characters are digits
    {
        if (*p_c < '0' || *p_c > '9')
        {
            return 0;
        }

        p_c++;
        has_digits++;
    }
    return (has_digits > 0); // We want to verify at least one character is a digit.
}

/***** Public functions *****/

void CLI_handle_quit(GAME_RESULT_E result)
{
    if (result == GAME_RESULT_PLAYING)
    {
        result = 0;
    }
    // CLI doesn't allocate resources, therefore handling quit is easy.
    printf("Exiting...\n");
    return;
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

void CLI_print_board(const GAME_board_t *p_a_board)
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
        printf("%d| ", i + 1);
        for (int j = 0; j < NUM_RANKS; j++)
        {
            printf("%c ", GAME_piece_letter_at(p_a_board, SQ_FROM_FILE_RANK(j, i)));
        }
        printf("|\n");
    }
    printf("  -----------------\n");
    printf("   A B C D E F G H\n");
}

void CLI_print_settings(const SETTINGS_settings_t *p_settings)
{
    if (p_settings->game_mode == 1)
    {
        printf("SETTINGS:\n");
        printf("GAME_MODE: 1\n");
        printf("DIFFICULTY_LVL: %d\n", p_settings->difficulty);
        printf("USER_CLR: %s\n", COLOR_STR_AC(p_settings->user_color));
    } else
    {
        printf("SETTINGS:\n");
        printf("GAME_MODE: 2\n");
    }
}

MANAGER_agent_settings_command_t CLI_prompt_settings_command(const SETTINGS_settings_t *p_a_settings)
{
    char *token;
    MANAGER_agent_settings_command_t command;

    if (!gs_settings_intro_printed)
    {
        printf("Specify game setting or type 'start' to begin a game with the current setting:\n");
        gs_settings_intro_printed = TRUE;
    }
    printf("<settings> ");
    fgets(gs_command_buffer, MAX_INPUT_SIZE, stdin);

    if (gs_command_buffer[0] == '\n')
    {
        command.type = MANAGER_SETTINGS_COMMAND_TYPE_NONE;
    } else
    {
        token = strtok(gs_command_buffer, SPLIT_TOKEN);
        if (strcmp(token, "game_mode") == 0)
        {
            command.type = MANAGER_SETTINGS_COMMAND_TYPE_CHANGE_SETTING;
            command.data.change_setting.setting = SETTINGS_SETTING_GAME_MODE;
            token = strtok(NULL, SPLIT_TOKEN);
            if (!_CLI_is_int(token))
            {
                token = "-1"; // input not an integer
            }
            command.data.change_setting.value = atoi(token);
        } 
        else if (strcmp(token, "difficulty") == 0)
        {
            command.type = MANAGER_SETTINGS_COMMAND_TYPE_CHANGE_SETTING;
            command.data.change_setting.setting = SETTINGS_SETTING_DIFFICULTY;
            token = strtok(NULL, SPLIT_TOKEN);
            if (!_CLI_is_int(token))
            {
                token = "-1"; // input not an integer
            }
            command.data.change_setting.value = atoi(token);
        } else if (strcmp(token, "user_color") == 0)
        {
            command.type = MANAGER_SETTINGS_COMMAND_TYPE_CHANGE_SETTING;
            command.data.change_setting.setting = SETTINGS_SETTING_USER_COLOR;
            token = strtok(NULL, SPLIT_TOKEN);
            if (!_CLI_is_int(token))
            {
                token = "-1"; // Input not an integer
            }
            command.data.change_setting.value = atoi(token); // todo verify integer
        } else if (strcmp(token, "load") == 0)
        {
            command.type = MANAGER_SETTINGS_COMMAND_TYPE_LOAD;
            command.data.filename = strtok(NULL, SPLIT_TOKEN);
            assert(0); // not yet implemented
        } else if (strcmp(token, "default") == 0)
        {
            command.type = MANAGER_SETTINGS_COMMAND_TYPE_DEFAULT_SETTINGS;
        } else if (strcmp(token, "quit") == 0)
        {
            command.type = MANAGER_SETTINGS_COMMAND_TYPE_QUIT;
        } else if (strcmp(token, "start") == 0)
        {
            command.type = MANAGER_SETTINGS_COMMAND_TYPE_START_GAME;
        } else if (strcmp(token, "print_settings") == 0)
        {
            CLI_print_settings(p_a_settings);
            command.type = MANAGER_SETTINGS_COMMAND_TYPE_NONE;
        } else
        {
            command.type = MANAGER_SETTINGS_COMMAND_TYPE_NONE;
        }
    }

    return command;
}

MANAGER_agent_play_command_t CLI_prompt_play_command(const GAME_board_t *p_a_board, BOOL can_undo)
{
    COLOR player = GAME_current_player(p_a_board);
    assert(can_undo >= 0);
    if (!gs_board_printed)
    {
        CLI_print_board(p_a_board);
        gs_board_printed = TRUE;
    }

    printf("%s player - enter your move:\n", COLOR_STR(GAME_current_player(p_a_board)));

    MANAGER_agent_play_command_t command;
    char *token;

    fgets(gs_command_buffer, MAX_INPUT_SIZE, stdin);

    if (gs_command_buffer[0] == '\n')
    {
        command.type = MANAGER_PLAY_COMMAND_TYPE_NONE;
    }
    else
    {
        token = strtok(gs_command_buffer, SPLIT_TOKEN);
        if (strcmp(token, "move") == 0)
        {
            GAME_move_t move;
            command.type = MANAGER_PLAY_COMMAND_TYPE_MOVE;
            square sq_from = _CLI_parse_square(strtok(NULL, SPLIT_TOKEN));
            move.from = sq_from;
            token = strtok(NULL, SPLIT_TOKEN);
            if ((token == NULL) || ((strcmp(token, "to")) != 0))
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
                    printf("Pawn promotion- please replace the pawn by queen, rook, knight, bishop or pawn:\n"); // todo - remove the pawn.

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
        else if (strcmp(token, "save") == 0)
        {
            command.type = MANAGER_PLAY_COMMAND_TYPE_SAVE;
            assert(0); // not yet supported.
        }
        else if (strcmp(token, "get_moves") == 0)
        {
            command.type = MANAGER_PLAY_COMMAND_TYPE_GET_MOVES;
            square sq_from = _CLI_parse_square(strtok(NULL, SPLIT_TOKEN));
            if (!SQ_IS_LEGAL(sq_from))
            {
                printf("Invalid position on the board\n");
                command.type = MANAGER_PLAY_COMMAND_TYPE_NONE;
                return command;
            }
            command.data.sq = sq_from;
        }
        else if (strcmp(token, "castle") == 0)
        {
            command.type = MANAGER_PLAY_COMMAND_TYPE_CASTLE;
            square sq_from = _CLI_parse_square(strtok(NULL, SPLIT_TOKEN));
            if (!SQ_IS_LEGAL(sq_from) || p_a_board->pieces[sq_from] != PIECE_TYPE_ROOK)
            {
                printf("Wrong position for a rook\n");
                command.type = MANAGER_PLAY_COMMAND_TYPE_NONE;
                return command;
            }
            command.data.sq = sq_from;
        }
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
        switch (response.output.settings_change_result)
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
    assert(command.type >= 0);

    switch (command.type)
    {
    case MANAGER_PLAY_COMMAND_TYPE_MOVE:
        {
            GAME_move_analysis_t analysis = response.output.move_data.move_result.move_analysis;
            switch (analysis.verdict)
            {
            case GAME_MOVE_VERDICT_LEGAL: // legal move prints
                {
                    switch (response.output.move_data.game_result)
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
                            } else
                            {
                                printf("Check: white King is threatend!\n");
                            }
                        }
                        break;
                    }

                    gs_board_printed = FALSE;
                    break;
                } // case GAME_MOVE_VERDICT_LEGAL:
            case GAME_MOVE_VERDICT_ILLEGAL_SQUARE: // original square is invalid
                {
                    printf("Invalid position on the board\n");
                    break;
                }
            case GAME_MOVE_VERDICT_NO_PIECE: // original square has friendly piece
                {
                    printf("The specified position does not contain your piece\n");
                    break;
                }
            default: // move is illegal for some other reason
                {
                    printf("Illegal move\n");
                    break;
                }
            } // switch(analysis.verdict)
            break;
        } // case MANAGER_PLAY_COMMAND_TYPE_MOVE
    case MANAGER_PLAY_COMMAND_TYPE_UNDO:
        {

            switch (response.output.undo_data.undo_result)
            {
            case MANAGER_UNDO_RESULT_SUCCESS: // undo was succesfull
                {
                    gs_board_printed = FALSE;
                    char from_str[6], to_str[6];
                    GAME_move_analysis_t analysis;
                    for (int i = 0; i < 2; i++)
                    {
                        analysis = response.output.undo_data.undone_moves[i];
                        CLI_sq_to_str(analysis.move.from, from_str);
                        CLI_sq_to_str(analysis.move.to, to_str);
                        printf("Undo ");
                        if (analysis.special_bm & GAME_SPECIAL_CASTLE)
                        {
                            printf("castle");
                        }
                        else if (analysis.special_bm & GAME_SPECIAL_PROMOTE)
                        {
                            printf("promotion");
                        }
                        else
                        {
                            printf("move");
                        }
                        printf(" for player %s: %s -> %s\n", COLOR_STR(analysis.color), to_str, from_str);
                    }
                    break;
                }
            case MANAGER_UNDO_RESULT_FAIL_TWO_PLAYERS: // invalid mode for undo
                {
                    printf("Undo command not available in 2 players mode\n");
                    break;
                }
            case MANAGER_UNDO_RESULT_FAIL_NO_HISTORY: // no undos are possible
                {
                    printf("Empty history, move cannot be undone\n");
                    break;
                }
                break;
            } // switch(response.output.undo_data.undo_result)
        } // case MANAGER_PLAY_COMMAND_TYPE_UNDO
    case MANAGER_PLAY_COMMAND_TYPE_QUIT:
        {
            break;
        }
    case MANAGER_PLAY_COMMAND_TYPE_RESET:
        {
            gs_board_printed = FALSE;
            gs_settings_intro_printed = FALSE;
            printf("Restarting...\n");
            break;
        }
    case MANAGER_PLAY_COMMAND_TYPE_GET_MOVES:
        {
            GAME_move_analysis_t *analyses = response.output.get_moves_data.moves;

            if (analyses == NULL)
            {
                printf("The specified position does not contain %s player piece\n", COLOR_STR(response.output.get_moves_data.player_color));
                break;
            }

            GAME_move_analysis_t *tmp = analyses;
            char to_str[6];
            while (tmp->verdict == GAME_MOVE_VERDICT_LEGAL) // loop one on moves
            {
                //printf("analyzing\n");
                if (tmp->special_bm & GAME_SPECIAL_CASTLE)
                {
                    tmp++;
                    continue; // castles are handled in the second loop
                }

                CLI_sq_to_str(tmp->move.to, to_str);

                printf("%s", to_str);
                if (response.output.get_moves_data.display_hints)
                {
                    if (tmp->special_bm & GAME_SPECIAL_UNDER_ATTACK)
                    {
                        printf("*");
                    }
                    if (tmp->special_bm & GAME_SPECIAL_CAPTURE)
                    {
                        printf("^");
                    }
                }
                printf(" ");
                tmp++;
            }
            tmp = analyses;
            while (tmp->verdict == GAME_MOVE_VERDICT_LEGAL) // loop 2 on castles
            {
                if (!(tmp->special_bm & GAME_SPECIAL_CASTLE))
                {
                    tmp++;
                    continue;
                }

                if (SQ_TO_FILE(tmp->move.to) == SQ_TO_FILE(C1)) // queenside castle
                {
                    CLI_sq_to_str(SQ_LEFT(SQ_LEFT(tmp->move.to)), to_str);
                } else
                {
                    CLI_sq_to_str(SQ_RIGHT(tmp->move.to), to_str);
                }

                printf("castle %s", to_str);
                printf(" ");
                tmp++;
            }
            printf("\n");
            free(analyses);
            break;
        } //case MANAGERT_PLAY_COMMAND_TYPE_GET_MOVES:
    case MANAGER_PLAY_COMMAND_TYPE_CASTLE:
        {
            switch (response.output.castle_data.castle_result) 
            {
            case MANAGER_CASTLE_RESULT_FAIL_NO_ROOK:
                {
                    printf("Wrong position for rook\n");
                    break;
                }
            case MANAGER_CASTLE_RESULT_FAIL:
                {
                    printf("Illegal castling move\n");
                    break;
                }
            case MANAGER_CASTLE_RESULT_SUCCESS:
                gs_board_printed = FALSE;
                break;
            }
        }
    case MANAGER_PLAY_COMMAND_TYPE_NONE:
    case MANAGER_PLAY_COMMAND_TYPE_RESTART:
        break; // commands not relevant to CLI mode
    } // switch(command.type)
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

