/**
 * @file pfile.h
 *
 * Interface of the save game encoding functionality.
 */
#pragma once

#include "player.h"
#include "../DiabloUI/diabloui.h"

namespace devilution {

#ifdef __cplusplus
extern "C" {
#endif

extern BOOL gbValidSaveFile;

void pfile_write_hero();
BOOL pfile_create_player_description();
void pfile_flush_W();
BOOL pfile_ui_set_hero_infos(BOOL (*ui_add_hero_info)(_uiheroinfo *));
BOOL pfile_archive_contains_game(HANDLE hsArchive, DWORD save_num);
void pfile_ui_set_class_stats(unsigned int player_class_nr, _uidefaultstats *class_stats);
BOOL pfile_ui_save_create(_uiheroinfo *heroinfo);
BOOL pfile_get_file_name(DWORD lvl, char *dst);
BOOL pfile_delete_save(_uiheroinfo *hero_info);
void pfile_read_player_from_save();
bool LevelFileExists();
void GetTempLevelNames(char *szTemp);
void GetPermLevelNames(char *szPerm);
void pfile_remove_temp_files();
void pfile_rename_temp_to_perm();
void pfile_write_save_file(const char *pszName, BYTE *pbData, DWORD dwLen, DWORD qwLen);
BYTE *pfile_read(const char *pszName, DWORD *pdwLen);
void pfile_update(bool force_save);

#ifdef __cplusplus
}
#endif

}
