// Header file for joystick functionality

#ifndef __JSTICK_H__
#define __JSTICK_H__

typedef struct {
   int *previous_track;
   int *next_track;
   int *next_n_tracks;
   int *previous_n_tracks;
   int *volume_up;
   int *volume_down;
   int *pause;
   int *surround_sound;
   int *random;
   int *previous_playlist;
   int *next_playlist;
   int *rewind;
   int *fast_forward;
   int *quit;
   int *exit_code_1;
   int *exit_code_2;
   int *exit_code_3;
   int *exit_code_4;
   int *exit_code_5;
   int *exit_code_6;
   int *exit_code_7;
   int *exit_code_8;
   int *exit_code_9;
}JOY_FUNCTION;

extern JOY_FUNCTION joy_function;

extern int joy_zero;
extern int joy_minus_one;

extern int jstick_analogue;

extern int jstick_volume_fine;

void jstick_get_functionality(void);

#endif

