/* input.c
   Input routines for DAMP
   By Matt Craven
*/

/*======= i n c l u d e s =========================================*/

#include <stdio.h>
#include <conio.h>
#include <allegro.h>

/*======= d a m p   i n c l u d e s ===============================*/

#include "damp.h"
#include "input.h"
#include "util.h"
#include "indrv.h"
#include "jstick.h"
#include "graphics.h"
#include "keys.h"

/*===================================================================
  input_handler(whos_calling)

  This polls for input, and takes appropriate action.
  whos_calling lets us know whether it's being called from the
  main loop or not.

  Returns non-zero if a break from the main program loop is required
  =================================================================*/

#define KEYS_HANDLE(action,func) \
         k=0; \
         while(k<KEYS_PER_ACTION) \
         { \
            if(key[key_table[action][k]]) \
            { \
               clear_keybuf(); \
               return(func); \
            } \
            k++; \
         }

int input_handler(int whos_calling)
{
      int k;
      int retval=0;

      /* Poll the joystick */

      poll_joystick();

      /* See if we need to quit */

      if(*joy_function.quit)
      {
         need_to_quit_program = TRUE;
         return 0;
      }

      /* deal with -keypadhack */

      if(damp_use_conio_input)
         if(kbhit())
            keypad_hack();

      /* If using an input driver, call its poll function */

      if(indrv_in_use)
         retval = indrv_poll();

      /* ============ JOYSTICK INPUT ===========================*/

      if(*joy_function.previous_track)
      {
         if(damp_developer) printf("Previous track\n");
         do{poll_joystick();}while(*joy_function.previous_track);

         return (util_previous_track());
      }

      if(*joy_function.previous_n_tracks)
      {
         if(damp_developer) printf("Previous n tracks\n");
         do{poll_joystick();}while(*joy_function.previous_n_tracks);

         return (util_previous_n_tracks());
      }

      if(*joy_function.next_n_tracks)
      {
         if(damp_developer) printf("Next n tracks\n");
         do{poll_joystick();}while(*joy_function.next_n_tracks);

         return (util_next_n_tracks());
      }

      if(*joy_function.random)
      {
         if(damp_developer) printf("Random\n");
         do{poll_joystick();}while(*joy_function.random);

         return (util_random_toggle());
      }

      if(*joy_function.surround_sound)
      {
         if(damp_developer) printf("Surround\n");
         do{poll_joystick();}while(*joy_function.surround_sound);

         return (util_surround_toggle());
      }

      if((*joy_function.volume_up) && damp_volume < 255 && !track_paused)
      {
         if(damp_developer) printf("Volume up\n");
         if(jstick_volume_fine) do{poll_joystick();}while(*joy_function.volume_up);

         return (util_volume_up());
      }

      if((*joy_function.volume_down) && damp_volume > 0 && !track_paused)
      {
         if(damp_developer) printf("Volume down\n");
         if(jstick_volume_fine) do{poll_joystick();}while(*joy_function.volume_down);

         return (util_volume_down());
      }

      if(*joy_function.pause)
      {
         if(damp_developer) printf("Pause\n");
         do{poll_joystick();}while(*joy_function.pause);

         return (util_pause());
      }

      if(*joy_function.next_track)   /* Skip to the next track */
      {
         if(damp_developer) printf("Next track\n");
         do{poll_joystick();}while(*joy_function.next_track);

         return (util_next_track());
      }

      if(*joy_function.previous_playlist) /* previous playlist */
      {
         if(damp_developer) printf("Previous playlist\n");
         do{poll_joystick();}while(*joy_function.previous_playlist);

         return (util_previous_playlist());
      }

      if(*joy_function.next_playlist) /* next playlist */
      {
         if(damp_developer) printf("Next playlist\n");
         do{poll_joystick();}while(*joy_function.next_playlist);

         return (util_next_playlist());
      }

      if(*joy_function.exit_code_1)
         exit(1);
      if(*joy_function.exit_code_2)
         exit(2);
      if(*joy_function.exit_code_3)
         exit(3);
      if(*joy_function.exit_code_4)
         exit(4);
      if(*joy_function.exit_code_5)
         exit(5);
      if(*joy_function.exit_code_6)
         exit(6);
      if(*joy_function.exit_code_7)
         exit(7);
      if(*joy_function.exit_code_8)
         exit(8);
      if(*joy_function.exit_code_9)
         exit(9);

      if(whos_calling == 0)
      {
         /* called by main loop, not by lcd_poll */

         if(*joy_function.rewind)
         {
            if(damp_developer) printf("Rewind\n");
            return (util_rewind());
         }
   
         if(*joy_function.fast_forward)
         {
            if(damp_developer) printf("Fast-forward\n");
            return (util_fast_forward());
         }
      }


      /*============= KEYBOARD INPUT ======================*/


      if(!indrv_keyboard_removed)
      {
         KEYS_HANDLE(KEYS_HELP, util_help());
         KEYS_HANDLE(KEYS_QUIT, util_quit());
         KEYS_HANDLE(KEYS_SCREENSHOT, util_screenshot());
         KEYS_HANDLE(KEYS_PREVIOUS_TRACK, util_previous_track());
         KEYS_HANDLE(KEYS_RANDOM_TOGGLE, util_random_toggle());
         KEYS_HANDLE(KEYS_PAD_0, key[KEY_LCONTROL] ? util_pad(0,TRUE) : util_pad(0,FALSE));
         KEYS_HANDLE(KEYS_PAD_1, key[KEY_LCONTROL] ? util_pad(1,TRUE) : util_pad(1,FALSE));
         KEYS_HANDLE(KEYS_PAD_2, key[KEY_LCONTROL] ? util_pad(2,TRUE) : util_pad(2,FALSE));
         KEYS_HANDLE(KEYS_PAD_3, key[KEY_LCONTROL] ? util_pad(3,TRUE) : util_pad(3,FALSE));
         KEYS_HANDLE(KEYS_PAD_4, key[KEY_LCONTROL] ? util_pad(4,TRUE) : util_pad(4,FALSE));
         KEYS_HANDLE(KEYS_PAD_5, key[KEY_LCONTROL] ? util_pad(5,TRUE) : util_pad(5,FALSE));
         KEYS_HANDLE(KEYS_PAD_6, key[KEY_LCONTROL] ? util_pad(6,TRUE) : util_pad(6,FALSE));
         KEYS_HANDLE(KEYS_PAD_7, key[KEY_LCONTROL] ? util_pad(7,TRUE) : util_pad(7,FALSE));
         KEYS_HANDLE(KEYS_PAD_8, key[KEY_LCONTROL] ? util_pad(8,TRUE) : util_pad(8,FALSE));
         KEYS_HANDLE(KEYS_PAD_9, key[KEY_LCONTROL] ? util_pad(9,TRUE) : util_pad(9,FALSE));
         KEYS_HANDLE(KEYS_SCROLL_TRACK_TITLE, util_scroll_track_title());
         KEYS_HANDLE(KEYS_SURROUND_TOGGLE, util_surround_toggle());
         KEYS_HANDLE(KEYS_VOLUME_UP, util_volume_up());
         KEYS_HANDLE(KEYS_VOLUME_DOWN, util_volume_down());
         KEYS_HANDLE(KEYS_GRAPHICS_TOGGLE, util_graphics_toggle());
         KEYS_HANDLE(KEYS_PAUSE, util_pause());
         KEYS_HANDLE(KEYS_NEXT_TRACK, util_next_track());
         KEYS_HANDLE(KEYS_NEXT_N_TRACKS, util_next_n_tracks());
         KEYS_HANDLE(KEYS_PREVIOUS_N_TRACKS, util_previous_n_tracks());
         KEYS_HANDLE(KEYS_PREVIOUS_PLAYLIST, util_previous_playlist());
         KEYS_HANDLE(KEYS_NEXT_PLAYLIST, util_next_playlist());
   
         if(whos_calling == 0)
         {
            /* called by main loop, not by lcd_poll */
   
            KEYS_HANDLE(KEYS_REWIND, util_rewind());
            KEYS_HANDLE(KEYS_FAST_FORWARD, util_fast_forward());
            KEYS_HANDLE(KEYS_CONTINUOUS_TOGGLE, util_continuous_toggle());
            KEYS_HANDLE(KEYS_SYNC_TOGGLE, util_sync_toggle());
            KEYS_HANDLE(KEYS_TIME_REMAIN_TOGGLE, util_time_remain_toggle());
         }
//
//         if(key[KEY_N] && using_graphics) /* Choose a new gfx output */
//         {
//            damp_gfx_waveform = rand()%DAMP_GFX_WAVEFORMS;
//            while(!damp_gfx_wave_use[damp_gfx_waveform])
//               damp_gfx_waveform = (damp_gfx_waveform+1)%DAMP_GFX_WAVEFORMS;
//            damp_gfx_create_palette();
//            clear_keybuf();
//         }
//   
//         if(key[KEY_M] && using_graphics) /* Choose a new gfx modifier */
//         {
//            damp_gfx_modifier = rand()%DAMP_GFX_MODIFIERS;
//            while(!damp_gfx_mod_use[damp_gfx_modifier])
//               damp_gfx_modifier = (damp_gfx_modifier+1)%DAMP_GFX_MODIFIERS;
//            clear_keybuf();
//         }
   
   }

   /* Return the appropriate value */

   return(retval);
}

/*===================================================================
  keypad_hack()

  Just as it says, a hack to use conio input instead of Allegro's
  input driver.  This was added before the input driver system, and
  is left in for backwards-compatibility.  Its behaviour is superceeded
  by using the ICONSOLE.sc input driver...
  =================================================================*/

void keypad_hack(void)
{
   char conio_key;

   conio_key = getch();

   switch(conio_key)
   {
      case 27:
         key[KEY_ESC] = TRUE;
         break;
      case 13:
         key[KEY_ENTER] = TRUE;
         break;
      case 47:
         key[KEY_SLASH2] = TRUE;
         break;
      case 42:
         key[KEY_ASTERISK] = TRUE;
         break;
      case 45:
         key[KEY_MINUS_PAD] = TRUE;
         break;
      case 55:
         key[KEY_HOME] = TRUE;
         break;
      case 56:
         key[KEY_UP] = TRUE;
         break;
      case 57:
         key[KEY_PGUP] = TRUE;
         break;
      case 52:
         key[KEY_LEFT] = TRUE;
         break;
      case 53:
         key[KEY_5_PAD] = TRUE;
         break;
      case 54:
         key[KEY_RIGHT] = TRUE;
         break;
      case 49:
         key[KEY_END] = TRUE;
         break;
      case 50:
         key[KEY_DOWN] = TRUE;
         break;
      case 51:
         key[KEY_PGDN] = TRUE;
         break;
      case 48:
         key[KEY_INSERT] = TRUE;
         break;
      case 46:
         key[KEY_DEL] = TRUE;
         break;
      case 43:
         key[KEY_PLUS_PAD] = TRUE;
         break;
      case 'm':
      case 'M':
         key[KEY_M] = TRUE;
         break;
      case 't':
      case 'T':
         key[KEY_T] = TRUE;
         break;
      case 's':
      case 'S':
         key[KEY_S] = TRUE;
         break;
      case 'g':
      case 'G':
         key[KEY_G] = TRUE;
         break;
      case 'p':
      case 'P':
         key[KEY_P] = TRUE;
         break;
      case 'n':
      case 'N':
         key[KEY_N] = TRUE;
         break;
      case 'z':
      case 'Z':
         key[KEY_Z] = TRUE;
         break;
      case 'x':
      case 'X':
         key[KEY_X] = TRUE;
         break;
      case 9:
         key[KEY_TAB] = TRUE;
         break;
      case 61:
         key[KEY_EQUALS] = TRUE;
         break;
      case 8:
         key[KEY_NUMLOCK] = TRUE;
         break;
      case 92:
         key[KEY_LCONTROL] = TRUE;
         break;
      case 91:
         key[KEY_OPENBRACE] = TRUE;
         break;
      case 93:
         key[KEY_CLOSEBRACE] = TRUE;
         break;
   }
}
