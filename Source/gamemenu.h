/**
 * @file gamemenu.h
 *
 * Interface of the in-game menu functions.
 */
#pragma once

namespace devilution {

#ifdef __cplusplus
extern "C" {
#endif

extern bool gbRunInTown;

void gamemenu_on();
void gamemenu_off();
void gamemenu_handle_previous();
void gamemenu_previous(BOOL bActivate);
void gamemenu_new_game(BOOL bActivate);
void gamemenu_quit_game(BOOL bActivate);
void gamemenu_load_game(BOOL bActivate);
void gamemenu_save_game(BOOL bActivate);
void gamemenu_restart_town(BOOL bActivate);
void gamemenu_options(BOOL bActivate);
void gamemenu_music_volume(BOOL bActivate);
void gamemenu_sound_volume(BOOL bActivate);
void gamemenu_loadjog(BOOL bActivate);
void gamemenu_gamma(BOOL bActivate);
void gamemenu_speed(BOOL bActivate);
void gamemenu_color_cycling(BOOL bActivate);

#ifdef __cplusplus
}
#endif

}
