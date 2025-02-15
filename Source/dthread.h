/**
 * @file dthread.h
 *
 * Interface of functions for updating game state from network commands.
 */
#pragma once

namespace devilution {

#ifdef __cplusplus
extern "C" {
#endif

void dthread_remove_player(int pnum);
void dthread_send_delta(int pnum, char cmd, void *pbSrc, int dwLen);
void dthread_start();
void dthread_cleanup();

/* data */
#ifdef __cplusplus
}
#endif

}
