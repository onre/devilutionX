/**
 * @file effects.h
 *
 * Interface of functions for loading and playing sounds.
 */
#pragma once

#include "sound.h"

namespace devilution {

#ifdef __cplusplus
extern "C" {
#endif

typedef struct TSFX {
	Uint8 bFlags;
	const char *pszName;
	TSnd *pSnd;
} TSFX;

extern int sfxdelay;
extern int sfxdnum;

BOOL effect_is_playing(int nSFX);
void stream_stop();
void InitMonsterSND(int monst);
void FreeMonsterSnd();
BOOL calc_snd_position(int x, int y, int *plVolume, int *plPan);
void PlayEffect(int i, int mode);
void PlaySFX(int psfx, bool randomizeByCategory = true);
void PlaySfxLoc(int psfx, int x, int y);
void sound_stop();
void sound_update();
void effects_cleanup_sfx();
void sound_init();
void ui_sound_init();
void effects_play_sound(const char *snd_file);
int GetSFXLength(int nSFX);

#ifdef __cplusplus
}
#endif

}
