#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "SDL_GAME_WINDOW.h"
#include "PIECE.h"
#include "GAME.h"

/***** Gloal variables *****/

static const SDL_Color GAME_WINDOW_DARK_SQ_C = {.r = 50, .g= 50, .b= 50, .a=255};
static const SDL_Color GAME_WINDOW_LIGHT_SQ_C = {.r = 205, .g= 205, .b= 205, .a=255};
static const SDL_Color GAME_WINDOW_BG_C = {.r = 155, .g= 155, .b= 155, .a=255};
static const SDL_Color GAME_WINDOW_ACTIVE_SQ_C = {.r = 0, .g = 255, .b = 255, .a=255};
static const SDL_Color GAME_WINDOW_CHECK_SQ_C = {.r = 255, .g= 100, .b= 100, .a=255};

static const SDL_Color GAME_WINDOW_MARKED_SQ_C = {.r = 255, .g= 255, .b= 255, .a=255};
static const SDL_Color GAME_WINDOW_MARKED_CASTLE_SQ_C = {.r = 0, .g= 0, .b=255, .a=255};
static const SDL_Color GAME_WINDOW_MARKED_UNDER_ATTACK_SQ_C = {.r = 255, .g= 255, .b= 100, .a=255};
static const SDL_Color GAME_WINDOW_MARKED_CAPTURE_SQ_C = {.r = 100, .g= 255, .b= 100, .a=255};

static BOOL gs_saved;
static char gs_filename_buffer[SDL_MAX_FILENAME];

/***** Private methods *****/

int _SDL_GAME_WINDOW_prompt_leave();

// button callbacks
SDL_BUTTON_action_t _SDL_GAME_WINDOW_play_save_button_cb()
{
    SDL_BUTTON_action_t cmd;
    cmd.action = SDL_BUTTON_ACTION_SEND_PLAY_CMD;
    cmd.play_cmd.type = MANAGER_PLAY_COMMAND_TYPE_SAVE;
    SDL_UTILS_get_save_path(".", 1, gs_filename_buffer);
    cmd.play_cmd.data.filename = gs_filename_buffer;
    SDL_UTILS_roll_saves(".");
    return cmd;
}

SDL_BUTTON_action_t _SDL_GAME_WINDOW_play_new_game_button_cb()
{
    SDL_BUTTON_action_t cmd;
    cmd.action = SDL_BUTTON_ACTION_SEND_PLAY_CMD;
    int action = GAME_WINDOW_SAVE_PROMPT_NO;
    if (!gs_saved)
    {
        action = _SDL_GAME_WINDOW_prompt_leave();
    }
    if (action == GAME_WINDOW_SAVE_PROMPT_YES)
    {
        cmd.action = SDL_BUTTON_ACTION_SEND_PLAY_CMD_PAIR;
        cmd.play_cmds[0] = _SDL_GAME_WINDOW_play_save_button_cb().play_cmd;
        cmd.play_cmds[1].type = MANAGER_PLAY_COMMAND_TYPE_RESTART;
    }
    else if (action == GAME_WINDOW_SAVE_PROMPT_NO)
    {
        cmd.action = SDL_BUTTON_ACTION_SEND_PLAY_CMD;
        cmd.play_cmd.type = MANAGER_PLAY_COMMAND_TYPE_RESTART;
    }
    else
    {
        cmd.action = SDL_BUTTON_ACTION_NONE;
    }
    return cmd;
}

SDL_BUTTON_action_t _SDL_GAME_WINDOW_play_main_menu_button_cb()
{
    SDL_BUTTON_action_t cmd;
    cmd.action = SDL_BUTTON_ACTION_SEND_PLAY_CMD;
    int action = GAME_WINDOW_SAVE_PROMPT_NO;
    if (!gs_saved)
    {
        action = _SDL_GAME_WINDOW_prompt_leave();
    }
    if (action == GAME_WINDOW_SAVE_PROMPT_YES)
    {
        cmd.action = SDL_BUTTON_ACTION_SEND_PLAY_CMD_PAIR;
        cmd.play_cmds[0] = _SDL_GAME_WINDOW_play_save_button_cb().play_cmd;
        cmd.play_cmds[1].type = MANAGER_PLAY_COMMAND_TYPE_RESET;
    }
    else if (action == GAME_WINDOW_SAVE_PROMPT_NO)
    {
        cmd.action = SDL_BUTTON_ACTION_SEND_PLAY_CMD;
        cmd.play_cmd.type = MANAGER_PLAY_COMMAND_TYPE_RESET;
    }
    else
    {
        cmd.action = SDL_BUTTON_ACTION_NONE;
    }
    return cmd;
}

SDL_BUTTON_action_t _SDL_GAME_WINDOW_play_last_move_button_cb()
{
    SDL_BUTTON_action_t cmd;
    cmd.action = SDL_BUTTON_ACTION_SEND_PLAY_CMD;
    cmd.play_cmd.type = MANAGER_PLAY_COMMAND_TYPE_UNDO;
    return cmd;
}



SDL_BUTTON_action_t _SDL_GAME_WINDOW_play_quit_button_cb()
{
    SDL_BUTTON_action_t cmd;
    int action = GAME_WINDOW_SAVE_PROMPT_NO;
    if (!gs_saved)
    {
        action = _SDL_GAME_WINDOW_prompt_leave();
    }
    if (action == GAME_WINDOW_SAVE_PROMPT_YES)
    {
        cmd.action = SDL_BUTTON_ACTION_SEND_PLAY_CMD_PAIR;
        cmd.play_cmds[0] = _SDL_GAME_WINDOW_play_save_button_cb().play_cmd;
        cmd.play_cmds[1].type = MANAGER_PLAY_COMMAND_TYPE_QUIT;
    }
    else if (action == GAME_WINDOW_SAVE_PROMPT_NO)
    {
        cmd.action = SDL_BUTTON_ACTION_SEND_PLAY_CMD;
        cmd.play_cmd.type = MANAGER_PLAY_COMMAND_TYPE_QUIT;
    }
    else
    {
        cmd.action = SDL_BUTTON_ACTION_NONE;
    }
    return cmd;
}

SDL_BUTTON_action_t _SDL_GAME_WINDOW_play_load_button_cb()
{
    SDL_BUTTON_action_t cmd;
    cmd.action = SDL_BUTTON_ACTION_CHANGE_STATE;
    cmd.new_state = SDL_INTERFACE_STATE_LOAD;
    return cmd;
}

// Get square based on board location clicked
square _SDL_GAME_WINDOW_get_sq_from_x_y(int x, int y)
{
    return SQ_FROM_FILE_RANK( (x - BOARD_OFFSET_H) / CELL_SIZE, 7 - (y / CELL_SIZE));
}

int _SDL_GAME_WINDOW_prompt_leave()
{
    SDL_MessageBoxButtonData buttons[] = 
    {
        {SDL_MESSAGEBOX_BUTTON_RETURNKEY_DEFAULT, GAME_WINDOW_SAVE_PROMPT_YES, "yes"},
        {0, GAME_WINDOW_SAVE_PROMPT_NO, "no"},
        {SDL_MESSAGEBOX_BUTTON_ESCAPEKEY_DEFAULT, GAME_WINDOW_SAVE_PROMPT_CANCEL, "cancel"}
    };

    const SDL_MessageBoxColorScheme colorScheme = {
        { /* .colors (.r, .g, .b) */
            /* [SDL_MESSAGEBOX_COLOR_BACKGROUND] */
            { GAME_WINDOW_BG_C.r, GAME_WINDOW_BG_C.g, GAME_WINDOW_BG_C.b },
            /* [SDL_MESSAGEBOX_COLOR_TEXT] */
            { 255, 255, 255 },
            /* [SDL_MESSAGEBOX_COLOR_BUTTON_BORDER] */
            { 0, 0, 0 },
            /* [SDL_MESSAGEBOX_COLOR_BUTTON_BACKGROUND] */
            { 0, 0, 0 },
            /* [SDL_MESSAGEBOX_COLOR_BUTTON_SELECTED] */
            { 150, 150, 150 }
        }
    };
    const SDL_MessageBoxData messageboxdata = {
        SDL_MESSAGEBOX_INFORMATION, /* .flags */
        NULL, /* .window */
        "Save", /* .title */
        "The game is not saved! Would you like to save?", /* .message */
        3, /* .numbuttons */
        buttons, /* .buttons */
        &colorScheme /* .colorScheme */
    };

    int buttonid;
    if (SDL_ShowMessageBox(&messageboxdata, &buttonid) < 0) {
        printf("ERROR: displaying promotion message box");
        return 0;
    }
    if (buttonid == -1) {
        printf("ERROR: no selection");
    }

    return buttonid;
}
// Prompt the screen for a promoted piece
PIECE_TYPE_E _SDL_GAME_WINDOW_prompt_promote(SDL_GAME_WINDOW_view_t* p_view)
{
    SDL_MessageBoxButtonData buttons[PIECE_TYPE_MAX];
    int cnt = 0;

    for (PIECE_TYPE_E i=PIECE_TYPE_EMPTY; i < PIECE_TYPE_MAX; i++)
    {
        PIECE_desc_t desc = PIECE_desc_lut[i];
        if (desc.can_promote_to)
        {
            buttons[cnt].buttonid = i;
            buttons[cnt].text = desc.name;
            cnt++;
        }
    }

    const SDL_MessageBoxColorScheme colorScheme = {
        { /* .colors (.r, .g, .b) */
            /* [SDL_MESSAGEBOX_COLOR_BACKGROUND] */
            { GAME_WINDOW_BG_C.r, GAME_WINDOW_BG_C.g, GAME_WINDOW_BG_C.b },
            /* [SDL_MESSAGEBOX_COLOR_TEXT] */
            { 255, 255, 255 },
            /* [SDL_MESSAGEBOX_COLOR_BUTTON_BORDER] */
            { 0, 0, 0 },
            /* [SDL_MESSAGEBOX_COLOR_BUTTON_BACKGROUND] */
            { 0, 0, 0 },
            /* [SDL_MESSAGEBOX_COLOR_BUTTON_SELECTED] */
            { 150, 150, 150 }
        }
    };
    const SDL_MessageBoxData messageboxdata = {
        SDL_MESSAGEBOX_INFORMATION, /* .flags */
        p_view->window, /* .window */
        "Promote", /* .title */
        "Please choose a piece to promote:", /* .message */
        cnt, /* .numbuttons */
        buttons, /* .buttons */
        &colorScheme /* .colorScheme */
    };

    int buttonid;
    if (SDL_ShowMessageBox(&messageboxdata, &buttonid) < 0) {
        printf("ERROR: displaying promotion message box");
        return 0;
    }
    if (buttonid == -1) {
        printf("ERROR: no selection");
    }

    return buttonid;
}

// draw given piece with given color at the given rectangle
void _SDL_GAME_WINDOW_draw_piece_at_rect(SDL_GAME_WINDOW_view_t* p_view, PIECE_TYPE_E piece, COLOR color, SDL_Rect* rec)
{
    switch (color) // render piece
    {
        case BLACK:
            {
                SDL_RenderCopy(p_view->renderer, p_view->pieces_texture_black[piece], NULL, rec);
                break;
            }
        case WHITE:
            {
                SDL_RenderCopy(p_view->renderer, p_view->pieces_texture_white[piece], NULL, rec);
                break;
            }
        default:
            break;
    }
}

/***** Public functions *****/

SDL_GAME_WINDOW_view_t* SDL_GAME_WINDOW_create_view()
{
    SDL_GAME_WINDOW_view_t* p_view = malloc(sizeof(SDL_GAME_WINDOW_view_t));

    if (p_view == NULL)
    {
        return NULL;
    }
    SDL_Window* window = SDL_CreateWindow("Game", SDL_WINDOWPOS_CENTERED,
            SDL_WINDOWPOS_CENTERED, GAME_WINDOW_W, GAME_WINDOW_H, SDL_WINDOW_OPENGL);

    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1,
            SDL_RENDERER_ACCELERATED);

    p_view->pieces_texture_white = malloc(sizeof(SDL_Texture*) * PIECE_TYPE_MAX);
    p_view->pieces_texture_black = malloc(sizeof(SDL_Texture*) * PIECE_TYPE_MAX);

    p_view->buttons = malloc(sizeof(SDL_button_t) * GAME_WINDOW_MAX_BUTTONS);

    p_view->bg_texture = SDL_UTILS_load_texture_from_bmp("./graphics/bg.bmp", renderer, FALSE);
    gs_saved = TRUE;


    p_view->window = window;
    p_view->renderer = renderer;

    p_view->active_sq = GAME_WINDOW_NO_ACTIVE_SQ;
    p_view->marked_moves = NULL;
    p_view->fixed_castle = TRUE;

    char wname[255];
    char bname[255];
    for (int i = PIECE_TYPE_PAWN; i <= PIECE_TYPE_KING; i++)
    {
        wname[0] = '\0';
        strcat(wname, "./graphics/W");
        strcat(wname, PIECE_desc_lut[i].name);
        strcat(wname, ".bmp");
        p_view->pieces_texture_white[i] = SDL_UTILS_load_texture_from_bmp(wname, renderer, TRUE);

        bname[0] = '\0';
        strcat(bname, "./graphics/B");
        strcat(bname, PIECE_desc_lut[i].name);
        strcat(bname, ".bmp");

        p_view->pieces_texture_black[i] = SDL_UTILS_load_texture_from_bmp(bname, renderer, TRUE);
    }

    p_view->button_count = 0;
    BOOL success = TRUE; // check if any of the operations failed
    success &= SDL_GAME_WINDOW_add_button(p_view, "./graphics/newgame.bmp", NULL, _SDL_GAME_WINDOW_play_new_game_button_cb);
    success &= SDL_GAME_WINDOW_add_button(p_view, "./graphics/loadgame.bmp", NULL, _SDL_GAME_WINDOW_play_load_button_cb); 
    success &= SDL_GAME_WINDOW_add_button(p_view, "./graphics/savegame.bmp", NULL, _SDL_GAME_WINDOW_play_save_button_cb); 
    success &= SDL_GAME_WINDOW_add_button(p_view, "./graphics/mainmenu.bmp", NULL, _SDL_GAME_WINDOW_play_main_menu_button_cb); 
    success &= SDL_GAME_WINDOW_add_button(p_view, "./graphics/last_move.bmp", "./graphics/undo_i.bmp", _SDL_GAME_WINDOW_play_last_move_button_cb); 
    p_view->undo_button = p_view->buttons[p_view->button_count-1];
    success &= SDL_GAME_WINDOW_add_button(p_view, "./graphics/quit.bmp", NULL, _SDL_GAME_WINDOW_play_quit_button_cb); 

    if (window == NULL || renderer == NULL || p_view->pieces_texture_white == NULL || p_view->pieces_texture_black == NULL
            || p_view->buttons == NULL || p_view->bg_texture == NULL || (!success)) 
    {
        SDL_GAME_WINDOW_destroy_view(p_view);
        return NULL ;
    }

    return p_view;
}

BOOL SDL_GAME_WINDOW_add_button(SDL_GAME_WINDOW_view_t* p_view, const char* active_texture_fn, const char* inactive_texture_fn, SDL_BUTTON_action_t (*cb)())
{
    assert(p_view->button_count < GAME_WINDOW_MAX_BUTTONS);
    SDL_Rect location = {.x=BUTTON_X, .y=BUTTON_PADDING + p_view->button_count * BUTTON_AREA_HEIGHT, .h=BUTTON_HEIGHT, .w=BUTTON_WIDTH};
    SDL_Texture * b_texture = SDL_UTILS_load_texture_from_bmp(active_texture_fn, p_view->renderer, FALSE);
    SDL_Texture* d_texture = SDL_UTILS_load_texture_from_bmp(inactive_texture_fn, p_view->renderer, FALSE);
    SDL_button_t* button = SDL_BUTTON_create(TRUE, cb, b_texture, d_texture, location, 0);
    if (button == NULL)
    {
        return FALSE;
    }
    else
    {
        p_view->buttons[p_view->button_count] = button;
        p_view->button_count++;
        return TRUE;
    }
}

void SDL_GAME_WINDOW_destroy_view(SDL_GAME_WINDOW_view_t* p_view)
{
    free(p_view->pieces_texture_white);
    free(p_view->pieces_texture_black);

    // All textures are destoyed by renderer
    SDL_DestroyRenderer(p_view->renderer);
    SDL_DestroyWindow(p_view->window);

    for (int i=0; i < p_view->button_count; i++)
    {
        SDL_BUTTON_destroy(p_view->buttons[i]);
    }
    free(p_view->buttons);

    if (p_view->marked_moves != NULL)
    {
        free(p_view->marked_moves);
    }

    free(p_view);
}

void SDL_GAME_WINDOW_draw_view(SDL_GAME_WINDOW_view_t* p_view, const GAME_board_t* p_board, BOOL can_undo)
{
    if(p_view == NULL){
        return;
    }
    SDL_Rect rec = { .x = BOARD_OFFSET_H, .y = 0, .w = BOARD_SIZE, .h = BOARD_SIZE };
    SDL_SetRenderDrawColor(p_view->renderer, GAME_WINDOW_BG_C.r, GAME_WINDOW_BG_C.g, GAME_WINDOW_BG_C.b, GAME_WINDOW_BG_C.a);
    SDL_RenderClear(p_view->renderer);
    SDL_RenderCopy(p_view->renderer, p_view->bg_texture, NULL, &rec);

    rec.w = CELL_SIZE;
    rec.h = CELL_SIZE;
    GAME_move_analysis_t* tmp;
    tmp = p_view->marked_moves;
    while (!p_view->fixed_castle && tmp != NULL && tmp->verdict == GAME_MOVE_VERDICT_LEGAL) // fix castle moves in the mix
    {
        if (tmp->special_bm & GAME_SPECIAL_CASTLE)
        {
            if (tmp->move.to == SQ_LEFT(SQ_LEFT(tmp->move.from))) // queenside castle
            {
                tmp->move.to = SQ_LEFT(SQ_LEFT(tmp->move.to));
            }
            else
            {
                tmp->move.to = SQ_RIGHT(tmp->move.to);
            }
        }
        tmp++;
    }

    p_view->fixed_castle = TRUE;

    SDL_Color sq_col;

    for (int rank = 0; rank < NUM_RANKS; rank++)
    {
        for (int file = 0; file < NUM_FILES; file++)
        {
            rec.x = BOARD_OFFSET_H + file * CELL_SIZE;
            rec.y = (7 - rank) * CELL_SIZE;

            if ((rank + file) % 2) // basic board colors
            {
                sq_col = GAME_WINDOW_DARK_SQ_C;
            } 
            else
            {
                sq_col = GAME_WINDOW_LIGHT_SQ_C;
            }

            square sq = SQ_FROM_FILE_RANK(file,rank);

            tmp = p_view->marked_moves; // iterate over marked moves

            while (tmp != NULL && tmp->verdict == GAME_MOVE_VERDICT_LEGAL) // choose color for marked squares
            {
                if (sq == tmp->move.to)
                {
                    if ((tmp->special_bm & GAME_SPECIAL_CASTLE) > 0)
                    {
                        sq_col = GAME_WINDOW_MARKED_CASTLE_SQ_C;
                    }
                    else if (p_view->marked_hints && (tmp->special_bm & GAME_SPECIAL_UNDER_ATTACK) > 0)
                    {
                        sq_col = GAME_WINDOW_MARKED_UNDER_ATTACK_SQ_C;
                    }
                    else if (p_view->marked_hints && (tmp->special_bm & GAME_SPECIAL_CAPTURE) > 0)
                    {
                        sq_col = GAME_WINDOW_MARKED_CAPTURE_SQ_C;
                    }
                    else
                    {
                        sq_col = GAME_WINDOW_MARKED_SQ_C;
                    }
                    break;
                }
                tmp++;
            }

            int color = p_board->colors[sq];
            PIECE_TYPE_E piece = p_board->pieces[sq];

            if (p_board->pieces[sq] == PIECE_TYPE_KING && GAME_is_checked(p_board, color)) // mark check
            {
                sq_col = GAME_WINDOW_CHECK_SQ_C;
            }
            if (sq == p_view->active_sq) // mark active square
            {
                sq_col = GAME_WINDOW_ACTIVE_SQ_C;
            }

            SDL_SetRenderDrawColor(p_view->renderer, sq_col.r, sq_col.g, sq_col.b, sq_col.a);
            SDL_RenderFillRect(p_view->renderer, &rec);

            if (!(sq == p_view->active_sq)) // do not draw piece at active square
            {
                _SDL_GAME_WINDOW_draw_piece_at_rect(p_view, piece, color, &rec);
            }

        }
    }

    if (p_view->active_sq != GAME_WINDOW_NO_ACTIVE_SQ) // render dragged piece
    {
        SDL_GetMouseState(&(rec.x), &(rec.y)); // draw piece in mouse position instead of square
        rec.x -= CELL_SIZE / 2; // centralize piece on mouse
        rec.y -= CELL_SIZE / 2;

        int color = p_board->colors[p_view->active_sq];

        PIECE_TYPE_E piece = p_board->pieces[p_view->active_sq];

        _SDL_GAME_WINDOW_draw_piece_at_rect(p_view, piece, color, &rec);
    }

    if (!can_undo) // toggle undo button
    {
        p_view->undo_button->is_active = FALSE;
    }
    else
    {
        p_view->undo_button->is_active = TRUE;
    }

    for (int i=0; i < p_view->button_count; i++) // draw all buttons
    {
        SDL_BUTTON_render(p_view->buttons[i], p_view->renderer);
    }    
    SDL_RenderPresent(p_view->renderer);
}

SDL_BUTTON_action_t SDL_GAME_WINDOW_handle_event(SDL_GAME_WINDOW_view_t* p_view, SDL_Event* event, const GAME_board_t* p_board)
{
    MANAGER_agent_play_command_t cmd;
    SDL_BUTTON_action_t act;

    act.action = SDL_BUTTON_ACTION_NONE;
    cmd.type = MANAGER_PLAY_COMMAND_TYPE_NONE;
    if (event->type == SDL_QUIT) // X button pressed
    {
        act.action = SDL_BUTTON_ACTION_SEND_PLAY_CMD;
        cmd.type = MANAGER_PLAY_COMMAND_TYPE_QUIT;
    }
    else if(event->type == SDL_MOUSEBUTTONUP) //SDL_MouseButtonEvent
    {
        if (event->button.button == SDL_BUTTON_LEFT)
        {
            if (event->button.x > BOARD_OFFSET_H) // board click
            {
                square sq = _SDL_GAME_WINDOW_get_sq_from_x_y(event->button.x, event->button.y);
                if (sq != p_view->active_sq)
                {
                    act.action = SDL_BUTTON_ACTION_SEND_PLAY_CMD;
                    if (p_board->pieces[p_view->active_sq] == PIECE_TYPE_KING && 
                            (abs(SQ_TO_FILE(sq) - SQ_TO_FILE(p_view->active_sq)) > 1))
                    { // king moving more than one square, can only be legal if it's castle
                        cmd.type = MANAGER_PLAY_COMMAND_TYPE_CASTLE;
                        cmd.data.sq = sq;
                    }
                    else if (p_board->pieces[sq] == PIECE_TYPE_KING)
                    { // king "capture" is only possible if trying to castle
                        cmd.type = MANAGER_PLAY_COMMAND_TYPE_CASTLE;
                        cmd.data.sq = p_view->active_sq;
                    }
                    else // a normal move
                    {
                        cmd.type = MANAGER_PLAY_COMMAND_TYPE_MOVE;
                        GAME_move_t move = {.from = p_view->active_sq, .to = sq, .promote = PIECE_TYPE_QUEEN};

                        cmd.data.move = move;
                        // handle promotion attempts by prompting for piece
                        if (p_board->pieces[p_view->active_sq] == PIECE_TYPE_PAWN 
                                && SQ_TO_RANK(sq) == LAST_RANK(GAME_current_player(p_board))
                                && SQ_TO_RANK(p_view->active_sq) == PAWN_RANK(OTHER_COLOR(GAME_current_player(p_board))))
                        {
                            cmd.data.move.promote = _SDL_GAME_WINDOW_prompt_promote(p_view);
                        }
                    }
                }
                p_view->active_sq = GAME_WINDOW_NO_ACTIVE_SQ;
            }
            else // button click, or empty space
            {
                for (int i=0; i < p_view->button_count; i++)
                {
                    SDL_BUTTON_action_t c = SDL_BUTTON_handle_event(p_view->buttons[i], event);
                    if (c.action != SDL_BUTTON_ACTION_NONE)
                    {
                        return c;
                    }
                }
            }
            p_view->active_sq = GAME_WINDOW_NO_ACTIVE_SQ;
        }
        if (p_view->marked_moves != NULL)
        {
            free(p_view->marked_moves);
        }
        p_view->marked_moves = NULL;
        p_view->fixed_castle = FALSE;
    }
    else if (event->type == SDL_MOUSEBUTTONDOWN)
    {
        if (event->button.x > BOARD_OFFSET_H) // board click
        {
            if (event->button.button == SDL_BUTTON_LEFT)
            {
                square sq = _SDL_GAME_WINDOW_get_sq_from_x_y(event->button.x, event->button.y);
                if (p_board->colors[sq] == GAME_current_player(p_board))
                {
                    p_view->active_sq = sq;
                }
            }

            act.action = SDL_BUTTON_ACTION_SEND_PLAY_CMD;
            square sq = _SDL_GAME_WINDOW_get_sq_from_x_y(event->button.x, event->button.y);
            cmd.type = MANAGER_PLAY_COMMAND_TYPE_GET_MOVES;
            cmd.data.sq = sq;
        }
    }
    act.play_cmd = cmd;
    return act;
}

void SDL_GAME_WINDOW_toggle_save(BOOL state)
{
    gs_saved = state;
}
