/**
 * @file gmenu.h
 *
 * Interface of the in-game navigation and interaction.
 */
#pragma once

#include "engine.h"

namespace devilution {

#ifdef __cplusplus
extern "C" {
#endif

typedef struct TMenuItem {
	Uint32 dwFlags;
	const char *pszStr;
	void (*fnMenu)(BOOL);
} TMenuItem;

extern TMenuItem *sgpCurrentMenu;

void gmenu_draw_pause(CelOutputBuffer out);
void FreeGMenu();
void gmenu_init_menu();
BOOL gmenu_is_active();
void gmenu_set_items(TMenuItem *pItem, void (*gmFunc)(TMenuItem *));
void gmenu_draw(CelOutputBuffer out);
BOOL gmenu_presskeys(int vkey);
BOOL gmenu_on_mouse_move();
BOOL gmenu_left_mouse(BOOL isDown);
void gmenu_enable(TMenuItem *pMenuItem, BOOL enable);
void gmenu_slider_set(TMenuItem *pItem, int min, int max, int gamma);
int gmenu_slider_get(TMenuItem *pItem, int min, int max);
void gmenu_slider_steps(TMenuItem *pItem, int dwTicks);

#ifdef __cplusplus
}
#endif

}
