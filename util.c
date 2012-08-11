/* util.c

   All the operations, like quit, pause etc.
   By Matt Craven
*/

/*======= i n c l u d e s =========================================*/

#include <stdio.h>
#include <conio.h>

/*======= d a m p   i n c l u d e s ===============================*/

#include "damp.h"
#include "audio.h"
#include "util.h"
#include "playlist.h"
#include "gui.h"
#include "lcd.h"
#include "graphics.h"
#include "cd.h"
#include "libcda.h"


//========================================================================
// The following is for the timeout when pressing the keypad keys
//========================================================================

void set_timeout_flag(void)
{
   timeout_flag = TRUE;
}
END_OF_FUNCTION(set_timeout_flag);

/*===================================================================

  Here are the util_* functions.  It's pretty self-explanatory what
  each one does, so I've not commented them all

  =================================================================*/

int util_quit(void)
{
   need_to_quit_program = TRUE;
   return TRUE;
}

int util_screenshot(void)
{
   screenshot();
   return FALSE;
}

int util_previous_track(void)
{
   lcd_require_update = TRUE;

   if(track_sel_pos > 0)
   {
      int n;

      remove_int(set_timeout_flag);

      for(n=0;n<7;n++) track_sel[n]=0;
      track_sel_pos = 0;
      return FALSE;

   } else

   if(damp_using_cd)
   {
      damp_cd_previous();
      return TRUE;
   }else
   {
      if(damp_playlist_entry > 0)
      {
         damp_track_number--;
         sprintf(file,"%s",damp_playlist_select_track(damp_playlist_entry-1));
         selecting_track_flag=TRUE;
         need_to_quit = TRUE;
         return TRUE;
      }
   }

   return FALSE;
}

int util_next_track(void)
{
   if(track_sel_pos>0)
   {
      remove_int(set_timeout_flag);
      timeout_flag=TRUE;
      return FALSE;
   }
   else
   {
      if(damp_using_cd)
         damp_cd_next();
      else
         need_to_quit=TRUE;

      return TRUE;
   }
}

int util_previous_n_tracks(void)
{
   int count = 0;

   if(damp_using_cd)
   {
      printf("\n\nThis feature has not been implemented\n");
      return FALSE;
   }

   while(damp_playlist_entry > 0 && count < util_large_skip_amount)
   {
      damp_track_number--;
      sprintf(file,"%s",damp_playlist_select_track(damp_playlist_entry-1));
      count++;
   }   

   selecting_track_flag=TRUE;
   need_to_quit = TRUE;
   return TRUE;
}

int util_next_n_tracks(void)
{
   int count = 0;

   if(damp_using_cd)
   {
      printf("\n\nThis feature has not been implemented\n");
      return FALSE;
   }

   while(damp_playlist_entry < damp_playlist_num_to_play-1 && count < util_large_skip_amount)
   {
      damp_track_number++;
      sprintf(file,"%s",damp_playlist_select_track(damp_playlist_entry+1));
      count++;
   }   

   selecting_track_flag=TRUE;
   need_to_quit = TRUE;
   return TRUE;
}

int util_random_toggle(void)
{
   lcd_require_update = TRUE;

   if(!damp_using_cd)
   {
      if ( damp_playlist_random )
      {
         damp_playlist_entry = damp_playlist_order[damp_playlist_entry];
         damp_track_number = damp_playlist_entry+1;
      }
      else
      {
         damp_playlist_entry = damp_playlist_num_played;
         damp_track_number = damp_playlist_entry;
      }
   }

   damp_playlist_random = !damp_playlist_random;

   gui_playlist_update();

   return FALSE;
}

int util_scroll_track_title(void)
{
   if(using_graphics && damp_gfx_scroll_times <= 0)
   {
      damp_gfx_scroll_name_pos = 0;
      damp_gfx_show_name = TRUE;
      damp_gfx_scroll_times = 1;
   }

   return FALSE;
}

int util_surround_toggle(void)
{
   if(!damp_using_cd)
   {
      lcd_require_update = TRUE;
   
      audio_driver->surround = !audio_driver->surround;
   }

   return FALSE;
}

int util_volume_up(void)
{
   if(!track_paused)
   {
      damp_volume += damp_vol_up_down_speed;
      if(damp_volume > 255) damp_volume = 255;
   
      if(damp_using_cd)
         cd_set_volume(damp_volume, damp_volume);
      else
         set_volume(damp_volume,-1);
   
      sprintf(lcd_status,"VOL + \n");
      damp_state = DAMP_STATE_VOL_UP;
   
      lcd_require_update = TRUE;
   }

   return FALSE;
}

int util_volume_down(void)
{

   if(!track_paused)
   {
      damp_volume -= damp_vol_up_down_speed;
      if(damp_volume < 0) damp_volume = 0;
   
      if(damp_using_cd)
         cd_set_volume(damp_volume, damp_volume);
      else
         set_volume(damp_volume,-1);
   
      sprintf(lcd_status,"VOL - \n");
      damp_state = DAMP_STATE_VOL_DOWN;
   
      lcd_require_update = TRUE;
   }

   return FALSE;
}

int util_graphics_toggle(void)
{
   if(!damp_using_gui && !damp_using_cd)
   {
      if(using_graphics)
      {
         using_graphics = FALSE;
         damp_gfx_shutdown();
         printf("%s\n",short_filename);
         if(damp_show_file_info)
            printf("%s\n",ratestr);
      }
      else
      {
         using_graphics = TRUE;
         damp_gfx_init();
      }
   }

   return FALSE;
}

int util_pause(void)
{
   lcd_require_update = TRUE;

   track_paused = !track_paused;
   if(!track_paused)
   {
      if(damp_using_cd)
      {
         cd_resume();
      }else
      {
         if(using_graphics && !damp_using_gui) fade_from(black_palette,damp_gfx_palette,4);
         set_volume(damp_volume,-1);
      }
   }
   else
   {
      if(damp_using_cd)
      {
         cd_pause();
      }else
      {
         set_volume(0,-1);
         if(using_graphics && !damp_using_gui) fade_out(4);
      }
   }

   return FALSE;
}

int util_pad(int num,int playlist_flag)
{
   lcd_require_update = TRUE;

   track_sel_shift_flag |= playlist_flag;

   if(track_sel_pos == 0 && num==0)
      return FALSE;

   remove_int(set_timeout_flag);
   track_sel[track_sel_pos] = num+'0';
   track_sel_pos++;
   install_int_ex(set_timeout_flag,SECS_TO_TIMER(damp_keypad_timeout));

   return FALSE;
}

int util_previous_playlist(void)
{
   if(!damp_using_cd)
   {
      lcd_require_update = TRUE;
   
      if(damp_metaplaylist_entry > 0)
      {
         damp_playlist_free();
         damp_metaplaylist[damp_metaplaylist_order[damp_metaplaylist_entry]].been_played = TRUE;
         damp_metaplaylist_entry--;
         if(!using_graphics) printf("\n\n");
         damp_create_playlist(damp_metaplaylist[damp_metaplaylist_order[damp_metaplaylist_entry]].filename);
         damp_playlist_entry=-1;
         damp_track_number = 0;
   
         return TRUE;
      }
   
      gui_playlist_update();
   }

   return FALSE;
}

int util_next_playlist(void)
{
   if(!damp_using_cd)
   {
      lcd_require_update = TRUE;

      if(damp_metaplaylist_entry == damp_metaplaylist_size - 1)
         if(!damp_playlist_continuous)
            return FALSE;

      if(damp_metaplaylist_entry < damp_metaplaylist_size-1 || damp_playlist_continuous)
      {
         damp_playlist_free();
         damp_metaplaylist[damp_metaplaylist_order[damp_metaplaylist_entry]].been_played = TRUE;
         damp_metaplaylist_entry++;
         if(damp_metaplaylist_entry == damp_metaplaylist_size)
            damp_metaplaylist_entry = 0;
         if(!using_graphics) printf("\n\n");
         damp_create_playlist(damp_metaplaylist[damp_metaplaylist_order[damp_metaplaylist_entry]].filename);
         damp_playlist_entry=-1;
         damp_track_number = 0;

         gui_playlist_update();
   
         return TRUE;
      }
   }

   return FALSE;
}

int util_rewind(void)
{
   if(!damp_using_cd)
   {
      audio_driver->rew(damp_ffwd_rew_speed); /* seek back n seconds */
   
      sprintf(lcd_status,"%c%cREW \n",lcd_rew_symbol,lcd_rew_symbol);
      damp_state = DAMP_STATE_REW;
   
      timer(1);
   
      lcd_require_update = TRUE;
   }

   return FALSE;
}

int util_fast_forward(void)
{
   if(!damp_using_cd)
   {
      audio_driver->ffwd(damp_ffwd_rew_speed); /* seek forward n seconds */
   
      sprintf(lcd_status,"FFWD%c%c\n",lcd_play_symbol,lcd_play_symbol);
      damp_state = DAMP_STATE_FFWD;
   
      timer(1);
   
      lcd_require_update = TRUE;
   }

   return FALSE;
}

int util_continuous_toggle(void)
{
   damp_playlist_continuous = !damp_playlist_continuous;
   return FALSE;
}

int util_sync_toggle(void)
{
   damp_gfx_sync_to_beats = !damp_gfx_sync_to_beats;
   return FALSE;
}

int util_time_remain_toggle(void)
{
   damp_display_time_remaining = !damp_display_time_remaining;
   return FALSE;
}

int util_help(void)
{
   if(!using_graphics && !lcd_in_use && !damp_using_gui)
   {
      clrscr();
      textcolor(YELLOW);
      cprintf("Standard keys:\n\r");
      printf("ESC     = Quit DAMP                          F1 = Display this help   \n");
      printf("ENTER   = Next track                  BACKSPACE = Previous track      \n");
      printf("RIGHT   = Forward several tracks           LEFT = back several tracks \n");
      printf("X       = Next playlist                       Z = Previous playlist   \n");
      printf("[       = Rewind                              ] = Fast-forward        \n");
      printf("+       = Increase Volume                     - = Decrease volume     \n");
      printf("S       = Surround on/off                     G = Graphics on/off     \n");
      printf("NUMLOCK = Random on/off                       C = Continuous on/off   \n");
      printf("V       = Toggle time elapsed/remaining       P = Pause/Unpause       \n");
      printf("\n");
      cprintf("Keypad:\n\r");
      printf("Use numbers to select a track.\n");
      printf("/       = Surround on/off                     * = Pause/Unpause       \n");
      printf("+       = Increase volume                     - = Decrease volume     \n");
      printf("ENTER   = Next track                          . = Previous track      \n");
      printf("\n");
      cprintf("Whilst in graphics mode:\n\r");
      printf("T       = Scroll track title once             B = Sync to beats on/off\n");
      printf("N       = New waveform                        M = new palette         \n");
      printf("TAB     = Save screenshot\n");
      printf("\n");

      printf("\n%d. %s\n",damp_track_number,get_filename(short_filename));

      textcolor(LIGHTGRAY);
   }
   return FALSE;
}

int util_cd_eject_button(void)
{
/* DISABLED FOR NOW - ISSUES WITH INSERTING UNDER WINDOWS, TIMER ETC.

   if(damp_cd_ejected)
      cd_close();
   else
      cd_eject();

   damp_cd_ejected = !damp_cd_ejected;
*/

   return FALSE;
}

