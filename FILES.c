#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>

#include "FILES.h"
#include "DEFS.h"
#include "PIECE.h"

#define TAG_MAX_LEN (50)
#define DATA_MAX_LEN (50)
#define LINE_MAX_LEN (1024)
#define XML_HEADER ("<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n")

/***** Private functions *****/

void _FILES_create_tags(const char* tag, char in_tag[TAG_MAX_LEN], char out_tag[TAG_MAX_LEN])
{
    assert(strlen(tag) < TAG_MAX_LEN-3);
    sprintf(in_tag, "<%s>", tag);
    sprintf(out_tag, "</%s>", tag);
}

void _FILES_write_indented_line(FILE *file, const char* line, int depth)
{
    for (int i=0; i < depth; i++)
    {
        fputc('\t', file);
    }
    fputs(line, file);
    fputc('\n', file);
}

void _FILES_write_tag_line(FILE *file, const char* tag, const char* value, int depth)
{
    // indent
    for (int i=0; i < depth; i++)
    {
        fputc('\t', file);
    }

    // create tag
    char in_tag[TAG_MAX_LEN], out_tag[TAG_MAX_LEN];
    _FILES_create_tags(tag, in_tag, out_tag);

    // write data to file
    fputs(in_tag, file);
    fputs(value, file);
    fputs(out_tag, file);
    fputc('\n', file);
}

void _FILES_write_settings_to_file(FILE* file, const SETTINGS_settings_t* p_settings, int depth)
{
    char data[DATA_MAX_LEN];

    sprintf(data, "%c", p_settings->game_mode + '0');
    _FILES_write_tag_line(file, "game_mode", data, depth);

    if (p_settings->game_mode == 1)
    {

        sprintf(data, "%c", p_settings->difficulty + '0');
        _FILES_write_tag_line(file, "difficulty", data, depth);

        sprintf(data, "%c", p_settings->user_color + '0');
        _FILES_write_tag_line(file, "user_color", data, depth);
    }
}

void _FILES_write_board_to_file(FILE* file, const GAME_board_t* p_board, int depth)
{
    char in_tag[TAG_MAX_LEN], out_tag[TAG_MAX_LEN];
    char row_tag[TAG_MAX_LEN], row_data[NUM_FILES + 1];

    _FILES_create_tags("board", in_tag, out_tag);
    _FILES_write_indented_line(file, in_tag, depth);

    for (int i = NUM_RANKS; i > 0; i--)
    {
        sprintf(row_tag, "row_%d", i);
        row_data[NUM_FILES] = '\0';
        for (int j = 0; j < NUM_FILES; j++)
        {
            square sq = SQ_FROM_RANK_FILE(i-1, j);
            row_data[j] = PIECE_desc_lut[p_board->pieces[sq]].letters[p_board->colors[sq] & 1];
        }
        _FILES_write_tag_line(file, row_tag, row_data, depth + 1);
    }

    _FILES_write_indented_line(file, out_tag, depth);
}

void _FILES_read_line_to_buffer(FILE* file, char buff[LINE_MAX_LEN])
{
    int idx = 0;
    char c;
    c = fgetc(file);
    while (c != '\n' && c != EOF)
    {
        buff[idx] = c;
        idx++;
        c = fgetc(file);
    }
    buff[idx] = '\0';
}

void _FILES_extract_data_from_tag(char *line_buffer, char *data)
{
    int d_idx = 0;
    BOOL copy_start = FALSE;

    for (size_t i=0; i < strlen(line_buffer); i++)
    {
        if (copy_start)
        {
            if (line_buffer[i] == '<')
            {
                data[d_idx] = '\0';
                break;
            }
            data[d_idx] = line_buffer[i];
            d_idx++;
        }
        else if (line_buffer[i] == '>')
        {
            copy_start = TRUE;
        }
    }
}

void _FILES_read_data_from_tag_line(FILE* lfile, char* data_buffer)
{
    char line_buffer[LINE_MAX_LEN];

    _FILES_read_line_to_buffer(lfile, line_buffer); // <game_mode>
    _FILES_extract_data_from_tag(line_buffer, data_buffer);
    assert(strlen(data_buffer) > 0);
}

/***** Public functions *****/
int FILES_load_file(const char* fname, SETTINGS_settings_t* p_settings, GAME_board_t* p_board)
{
    FILE *lfile = fopen(fname, "r");
    if (lfile == NULL)
    {
        return -1;
    }

    char line_buffer[LINE_MAX_LEN];
    char data[LINE_MAX_LEN];
    _FILES_read_line_to_buffer(lfile, line_buffer); // XML header
    if (!strcmp(line_buffer, XML_HEADER)) {return -1;}

    _FILES_read_line_to_buffer(lfile, line_buffer); // <game>

    _FILES_read_data_from_tag_line(lfile, data); //<current_turn>
    p_board->turn = 1 + (!(data[0] - '0')); // 1 for white, 2 for black
    p_board->history[p_board->turn].ep = GAME_NO_EP;


    _FILES_read_data_from_tag_line(lfile, data); //<game_mode>
    p_settings->game_mode = (data[0] - '0');

    if (p_settings->game_mode == 1) // single player
    {
        _FILES_read_data_from_tag_line(lfile, data); //<difficulty>
        p_settings->difficulty = (data[0] - '0');

        _FILES_read_data_from_tag_line(lfile, data); //<user_color>
        p_settings->user_color = (data[0] - '0');
    }

    _FILES_read_line_to_buffer(lfile, line_buffer); // <board>

    for (int i = NUM_RANKS - 1; i >= 0; i--)
    {
        _FILES_read_data_from_tag_line(lfile, data); // <row_i>
        for (int j = 0; j < NUM_FILES; j++)
        {
            char c = data[j];
            square sq = SQ_FROM_FILE_RANK(j, i);
            if (PIECE_desc_lut[PIECE_TYPE_EMPTY].letters[0] == c)
            {
                p_board->pieces[sq] = PIECE_TYPE_EMPTY;
                p_board->colors[sq] = NO_COLOR;
            }
            else {

                for (COLOR i = BLACK; i <= WHITE; i++)
                {
                    for (PIECE_TYPE_E p = PIECE_TYPE_PAWN; p <= PIECE_TYPE_KING; p++)
                    {
                        if (c == PIECE_desc_lut[p].letters[i])
                        {
                            p_board->pieces[sq] = p;
                            p_board->colors[sq] = i;
                        }
                    }
                }
            }
        }
    }

    // maybe read from settings file in the future.
    p_board->ep = GAME_NO_EP;
    p_board->castle_bm[0] = GAME_CASTLE_ALL;
    p_board->castle_bm[1] = GAME_CASTLE_ALL;

    // disable castles if king/rook not in place
    if (p_board->pieces[E1] != PIECE_TYPE_KING || p_board->colors[E1] != WHITE)
    {
        GAME_DISABLE_CASTLE(p_board->castle_bm[WHITE], GAME_CASTLE_ALL);
    }
    else 
    {
        if (p_board->pieces[A1] != PIECE_TYPE_ROOK || p_board->colors[A1] != WHITE)
        {
            GAME_DISABLE_CASTLE(p_board->castle_bm[WHITE], GAME_CASTLE_QUEENSIDE);
        }
        if (p_board->pieces[H1] != PIECE_TYPE_ROOK || p_board->colors[H1] != WHITE)
        {
            GAME_DISABLE_CASTLE(p_board->castle_bm[WHITE], GAME_CASTLE_KINGSIDE);
        }
    }
    if (p_board->pieces[E8] != PIECE_TYPE_KING || p_board->colors[E8] != BLACK)
    {
        GAME_DISABLE_CASTLE(p_board->castle_bm[BLACK], GAME_CASTLE_ALL);
    }
    else 
    {
        if (p_board->pieces[A8] != PIECE_TYPE_ROOK || p_board->colors[A8] != BLACK)
        {
            GAME_DISABLE_CASTLE(p_board->castle_bm[BLACK], GAME_CASTLE_QUEENSIDE);
        }
        if (p_board->pieces[H8] != PIECE_TYPE_ROOK || p_board->colors[H8] != BLACK)
        {
            GAME_DISABLE_CASTLE(p_board->castle_bm[BLACK], GAME_CASTLE_KINGSIDE);
        }
    }

    fclose(lfile);

    return 0;
}

int FILES_save_file(const char* fname, const SETTINGS_settings_t* p_settings, const GAME_board_t* p_board)
{
    FILE *save_file;
    char in_tag[TAG_MAX_LEN], out_tag[TAG_MAX_LEN];
    char data[DATA_MAX_LEN];

    save_file = fopen(fname, "w");
    if (save_file == NULL)
    {
        return -1;
    }
    fputs(XML_HEADER, save_file);

    _FILES_create_tags("game", in_tag, out_tag);
    _FILES_write_indented_line(save_file, in_tag, 0);

    sprintf(data, "%c", GAME_current_player(p_board) + '0');
    _FILES_write_tag_line(save_file, "current_turn", data, 1);
    _FILES_write_settings_to_file(save_file, p_settings, 1);
    _FILES_write_board_to_file(save_file, p_board, 1);

    _FILES_write_indented_line(save_file, out_tag, 0);

    fclose(save_file);
    return 0;
}

BOOL FILES_is_available(const char* fname)
{
    return (access(fname,F_OK) != -1);
}


