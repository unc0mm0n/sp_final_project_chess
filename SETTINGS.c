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

SETTINGS_CHANGE_RESULT_E SETTINGS_change_setting(SETTINGS_settings_t* p_a_settings, SETTINGS_SETTING_E a_setting, int a_new_value);
