/**
 * FILES.h
 *
 * Handles files for save and load functions
 **/

#ifndef FILES_IMP
#define FILES_IMP

#include "SETTINGS.h"
#include "GAME.h"

/**
 * Load file to settings and board.
 *
 * @param p_settings pointer to settinsg object to load to
 * @param p_board pointer to board object to load to
 *
 * @return int 0 on success or error code on failure
 */
int FILES_load_file(const char* fname, SETTINGS_settings_t* p_settings, GAME_board_t* p_board);

/**
 * Save settings and board to file.
 *
 * @param p_settings pointer to settinsg object to save from
 * @param p_board pointer to board object to save from
 *
 * @return int 0 on success or error code on failure
 */
int FILES_save_file(const char* fname, const SETTINGS_settings_t* p_settings, const GAME_board_t* p_board);

/**
 * Return true if fname is present.
 *
 * @param fname name of file
 *
 * @return BOOL TRUE if file is present
 */
BOOL FILES_is_available(const char* fname);

#endif /*FILES_IMP*/
