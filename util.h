/* UTIL.H
   Header file for utility functions
*/

#ifndef __UTIL_H__
#define __UTIL_H__

int util_large_skip_amount;     /* How many tracks to skip when using
                                   next/prev "n" tracks function */

void set_timeout_flag(void);
void set_timeout_flag_end(void);

int util_quit(void);
int util_screenshot(void);
int util_previous_track(void);
int util_next_track(void);
int util_previous_n_tracks(void);
int util_next_n_tracks(void);
int util_random_toggle(void);
int util_scroll_track_title(void);
int util_surround_toggle(void);
int util_volume_up(void);
int util_volume_down(void);
int util_graphics_toggle(void);
int util_pause(void);
int util_pad(int num,int playlist_flag);
int util_previous_playlist(void);
int util_next_playlist(void);
int util_rewind(void);
int util_fast_forward(void);
int util_continuous_toggle(void);
int util_sync_toggle(void);
int util_time_remain_toggle(void);
int util_help(void);
int util_cd_eject_button(void);

#endif
