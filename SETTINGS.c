#include <stdlib.h>

#include "SETTINGS.h"

SETTINGS_settings_t * SETTINGS_new_settings()
{
    SETTINGS_settings_t* p_settings = (SETTINGS_settings_t*) malloc(sizeof(SETTINGS_settings_t));
    if (p_settings == NULL)
    {
        return NULL;
    }
    p_settings->difficulty = SETTINGS_DEFAULT_DIFFICULTY;
    p_settings->user_color = SETTINGS_DEFAULT_USER_COLOR;
    p_settings->game_mode  = SETTINGS_DEFAULT_GAME_MODE;

    return p_settings;
}

BOOL SETTINGS_reset_to_default(SETTINGS_settings_t * p_a_settings);

void SETTINGS_free_settings(SETTINGS_settings_t* p_a_settings)
{

    if (p_a_settings == NULL)
    {
        return;
    }

    free(p_a_settings);
}

SETTINGS_CHANGE_RESULT_E SETTINGS_change_setting(SETTINGS_settings_t* p_a_settings, SETTINGS_SETTING_E a_setting, int a_new_value)
{
    switch(a_setting)
    {
        case SETTINGS_SETTING_GAME_MODE:
            {
                if (a_new_value != 1 && a_new_value != 2 && a_new_value != 66)
                {
                    return SETTINGS_CHANGE_RESULT_INVALID_MODE;
                }
                p_a_settings->game_mode = a_new_value;
                break;
            }
        case SETTINGS_SETTING_DIFFICULTY:
            {
                if (p_a_settings->game_mode != 1)
                {
                    return SETTINGS_CHANGE_RESULT_WRONG_MODE;
                }
                if (a_new_value < 1 || a_new_value > 5)
                {
                    return SETTINGS_CHANGE_RESULT_INVALID_DIFFICULTY;
                }
                p_a_settings->difficulty = a_new_value;
                break;
            }
        case SETTINGS_SETTING_USER_COLOR:
            {
                if (p_a_settings->game_mode != 1)
                {
                    return SETTINGS_CHANGE_RESULT_WRONG_MODE;
                }
                if (a_new_value != 1 && a_new_value  != 0)
                {
                    return SETTINGS_CHANGE_RESULT_INVALID_COLOR;
                }
                p_a_settings->user_color = a_new_value;
                break;
            }
    }

    return SETTINGS_CHANGE_RESULT_SUCCESS;
}

void SETTINGS_reset_settings(SETTINGS_settings_t * p_a_settings)
{
    p_a_settings->difficulty = SETTINGS_DEFAULT_DIFFICULTY;
    p_a_settings->user_color = SETTINGS_DEFAULT_USER_COLOR;
    p_a_settings->game_mode = SETTINGS_DEFAULT_GAME_MODE;

}
