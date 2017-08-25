/**
 * The settings manager handles all settings changes and the 
 * legality thereof. 
 */

#ifndef SETTINGS_IMP
#define SETTINGS_IMP

#include "DEFS.h"

// Defaults as defined in FinalProject pdf
#define SETTINGS_DEFAULT_GAME_MODE  (1)
#define SETTINGS_DEFAULT_DIFFICULTY (2)
#define SETTINGS_DEFAULT_USER_COLOR (1)

/**
 * All possible setting types to change.
 */
typedef enum SETTINGS_SETTING_S
{
    SETTINGS_SETTING_GAME_MODE,
    SETTINGS_SETTING_DIFFICULTY,
    SETTINGS_SETTING_USER_COLOR
} SETTINGS_SETTING_E;

/**
 * Possible results when attempting to change settings.
 */
typedef enum SETTINGS_CHANGE_RESULT_S
{
    SETTINGS_CHANGE_RESULT_SUCCESS,
    SETTINGS_CHANGE_RESULT_WRONG_MODE,          // For attempting to set arguments in wrong game mode
    SETTINGS_CHANGE_RESULT_INVALID_ARGUMENT,    // For invalid settings argument   
    SETTINGS_CHANGE_RESULT_NOT_SUPPORTED        // For settings that should be valid but are not supported

} SETTINGS_CHANGE_RESULT_E;

/**
 * The struct holding the actual settings.
 */
typedef struct SETTINGS_settings_s
{
    int game_mode;
    int difficulty;
    int user_color;
} SETTINGS_settings_t;

/**
 * Return a new settinsg object with default settings.
 * 
 * @return SETTINGS_settings_t* pointer to new settings object.
 */
SETTINGS_settings_t * SETTINGS_new_settings();

/**
 * Reset settings to default.
 * 
 * @param p_a_settings pointer to settings to reset to default 
 *  
 * @return BOOL TRUE on success, FALSE on fail.
 */
BOOL SETTINGS_reset_to_default(SETTINGS_settings_t * p_a_settings);

/**
 * Free settings object.
 * 
 * @param p_a_settings pointer to settings to free
 *  
 * @return BOOL TRUE on success, FALSE on fail.
 */
BOOL SETTINGS_free_settings(SETTINGS_settings_t* p_a_settings);

/**
 * Change given setting to new value if possible. Return result
 * accordingly 
 * 
 * @param p_a_settings pointer to location to change settings
 * @param a_setting setting to change
 * @param a_new_value new value of setting
 * 
 * @return SETTINGS_CHANGE_RESULT_E 
 */
SETTINGS_CHANGE_RESULT_E SETTINGS_change_setting(SETTINGS_settings_t* p_a_settings, SETTINGS_SETTING_E a_setting, int a_new_value);

#endif /*SETTINGS_IMP*/

