/* keys.h

   Header file for the redefineable keys support in DAMP
   By Matt Craven
*/

#ifndef __KEYS_H__
#define __KEYS_H__

#define KEYS_MAX 31
#define KEYS_PER_ACTION 10

#define KEYS_QUIT                            0
#define KEYS_SCREENSHOT                      1
#define KEYS_PREVIOUS_TRACK                  2
#define KEYS_NEXT_TRACK                      3
#define KEYS_PREVIOUS_N_TRACKS               4
#define KEYS_NEXT_N_TRACKS                   5
#define KEYS_RANDOM_TOGGLE                   6
#define KEYS_SCROLL_TRACK_TITLE              7
#define KEYS_SURROUND_TOGGLE                 8
#define KEYS_VOLUME_UP                       9
#define KEYS_VOLUME_DOWN                     10
#define KEYS_GRAPHICS_TOGGLE                 11
#define KEYS_PAUSE                           12
#define KEYS_PAD_0                           13
#define KEYS_PAD_1                           14
#define KEYS_PAD_2                           15
#define KEYS_PAD_3                           16
#define KEYS_PAD_4                           17
#define KEYS_PAD_5                           18
#define KEYS_PAD_6                           19
#define KEYS_PAD_7                           20
#define KEYS_PAD_8                           21
#define KEYS_PAD_9                           22
#define KEYS_PREVIOUS_PLAYLIST               23
#define KEYS_NEXT_PLAYLIST                   24
#define KEYS_REWIND                          25
#define KEYS_FAST_FORWARD                    26
#define KEYS_CONTINUOUS_TOGGLE               27
#define KEYS_SYNC_TOGGLE                     28
#define KEYS_TIME_REMAIN_TOGGLE              29
#define KEYS_HELP                            30

int key_table[KEYS_MAX][KEYS_PER_ACTION];

void keys_init(void);
void keys_exit(void);
void keys_add(int type, int key);

#endif
