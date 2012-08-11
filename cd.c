/* cd.c

   CD player functions for DAMP
   By Matt Craven
   (c)2000 Hedgehog Software
*/

/*=========== i n c l u d e s =====================================*/

#include <stdlib.h>
#include <stdio.h>
#include <malloc.h>
#include <dpmi.h>
#include "libcda.h"

/*=========== d a m p    i n c l u d e s ==========================*/

#include "cd.h"
#include "damp.h"
#include "playlist.h"
#include "lcd.h"
#include "input.h"
#include "util.h"

/*=========== g l o b a l   v a r i a b l e s =====================*/

int damp_using_cd = 0;                 /* Using the CD module ? */
int damp_cd_ejected = 0;               /* Is the CD ejected? */

int _damp_cd_first_track = 0;          /* Number of first audio track */
int _damp_cd_last_track = 0;           /* Number of last audio track */

int *_damp_cd_playlist = NULL;         /* dynamically allocated */
int *_damp_cd_playlist_order = NULL;   /* Order of play, dynamically allocated */
int _damp_cd_playlist_size;            /* Size of the playlist */
int _damp_cd_current_track;            /* current track */

volatile unsigned int damp_cd_time;    /* Current track time in seconds */

/* ======== p r o t o t y p e s ====================================*/

void _damp_cd_init(void);
void _damp_cd_exit(void);

/* timer function - updates the track time every second */
void _damp_cd_timer(void)
{
   if(!track_paused && !damp_cd_ejected);
      damp_cd_time++;
}
END_OF_FUNCTION(_damp_cd_timer);

/*===================================================================
  damp_cd_main()

  This function runs the CD player module until there's no CD tracks
  to play, or the user quits.
  =================================================================*/

int damp_cd_main(void)
{
   int n;                                 /* loop counter */
   unsigned long remember_time = 0;       /* for remembering the time value */

   /* Tell DAMP we're using the CD module */
   damp_using_cd = 1;

   /* Initialise the CD module */
   _damp_cd_init();

   /* Protect the memory around the timer function and variables */
   LOCK_FUNCTION(_damp_cd_timer);
   LOCK_VARIABLE(damp_cd_time);
   LOCK_VARIABLE(damp_cd_ejected);
   LOCK_VARIABLE(track_paused);

   /* Install the interrupt that updates track time every second */
   install_int_ex(_damp_cd_timer, BPS_TO_TIMER(1));

   /* The main player loop */

   do
   {
      /* Set up DAMP's filename variables with a simple "CD TRACK n" value
         so it looks sensible on the output */
      sprintf(file,"CD TRACK %d",damp_cd_get_track());
      sprintf(short_filename,"CD TRACK %d",damp_cd_get_track());
   
      /* Set up title scrolling */
      sprintf(lcd_scroll_text,"%*c%s\n",lcd_display_width,' ',short_filename);
      lcd_scroll_pos=0;
   
      track_sel_pos = 0;
      for(n=0;n<8;n++) track_sel[n]=0;

      if(damp_want_beep)
      {
         sound(300);
         rest(1000);
         sound(0);
      }

      if(damp_developer) printf("About to play track %d\n",damp_cd_get_track());

      /* reset time and start the track playing */
      damp_cd_time = 0;
      cd_play(damp_cd_get_track());

      lcd_require_update = 1;

      do
      {
         if(!track_paused) remember_time = damp_cd_time;

         /* If the LCD is in use, update the current status */
         if(lcd_in_use)
         {
            if(!track_paused)
            {
               sprintf(lcd_status,"PLAY%c \n",lcd_play_symbol);
               damp_state = DAMP_STATE_PLAY;
            } else
            {
               sprintf(lcd_status,"PAUSE \n");
               damp_state = DAMP_STATE_PAUSE;
            }
         }

         /* Check for input, and exit if we need to */
         if (input_handler(0)) break;

         if (need_to_quit)
         {
            selecting_track_flag = FALSE;
            break;
         }

         if(track_paused && remember_time != damp_cd_time)
            damp_cd_time = remember_time;

         /* Deal with track selection via keypad */
         if(timeout_flag || track_sel_pos > 6)
         {
            int trackno;

            remove_int(set_timeout_flag);

            if(damp_developer)
            {
               printf("\n");
               if(timeout_flag) printf("timeout_flag = TRUE\n");
               if(track_sel_pos > 6) printf("track_sel_pos > 6\n");
               printf("About to select track %d\n",atoi(track_sel));
            }

            timeout_flag = FALSE;

            trackno = atoi(track_sel)-1;

            if(trackno >= 0 && trackno < _damp_cd_playlist_size)
            {
               _damp_cd_current_track = trackno-1;

               if(damp_playlist_random)
               {
                  n=0;
                  while(_damp_cd_playlist[_damp_cd_playlist_order[n]] != trackno+1)
                  {
                     if(damp_developer) printf("n = %d (%d)\n",n,_damp_cd_playlist[_damp_cd_playlist_order[n]]);
                     n++;
                  }

                  _damp_cd_current_track = n-1;
               }

               break;
            } else
            {
               track_sel_pos = 0;
               for(trackno=0;trackno<8;trackno++) track_sel[trackno]=0;
            }
         }

         /* make sure we're displaying the right track number */
         damp_track_number = damp_cd_get_track();

         /* update the time display string */
         timer(0);

         /* Output display */
         if(!lcd_in_use)
         {
            printf("%s : %s %c%s%c\r", timestr, short_filename, track_sel_pos>0 ? '[' : ' ', track_sel_pos>0 ? track_sel : "      ", track_sel_pos>0 ? ']' : ' ');
            fflush(stdout);
         }else
         {
            lcd_display();
         }

         /* If the CD stops, exit this loop */
         if(!damp_cd_ejected && !track_paused && cd_current_track() == 0)
            break;

         /* Yield CPU */
         if(damp_yield)
            __dpmi_yield();

      }while(!need_to_quit && !need_to_quit_program);

      /* move to next track */

      need_to_quit = 0;

      cd_stop();

      _damp_cd_current_track++;
      if(_damp_cd_current_track == _damp_cd_playlist_size)
      {
         if(damp_playlist_continuous)
            _damp_cd_current_track = 0;
         else
            need_to_quit_program = 1;
      }

   }while(!need_to_quit_program);

   remove_int(_damp_cd_timer);

   cd_set_volume(255, 255);

   return 0;
}

/*===================================================================
  _damp_cd_init()

  This functions initialises CD playback.  If the CD is not an
  audio CD, or anything else goes wrong, it will quit and display
  an error message.
  =================================================================*/

void _damp_cd_init(void)
{
   int audio_count = 0;  /* number of tracks that are audio */
   int n,p, swap;        /* loop and temp variables */

   /* init CD functions */

   if(cd_init() != 0)
   {
      printf("_damp_cd_init(): Error initialising CD module.\n");
      exit(-1);
   }else
   {
      if(damp_developer) printf("_damp_cd_init(): CD Initialised successfully.\n");
   }

   /* Install _damp_cd_exit() as an atexit() routine */

   atexit(_damp_cd_exit);

   /* Get the numbers of the first and last tracks */

   if( cd_get_tracks( &_damp_cd_first_track, &_damp_cd_last_track ) != 0)
   {
      printf("Unable to determine number of tracks on the CD\nCheck there is a CD inserted and the drive is\ninstalled properly.\n");
      exit(-1);
   }else
   {
      if(damp_developer)
         printf("_damp_cd_init(): First = %d Last = %d\n",_damp_cd_first_track, _damp_cd_last_track);
   }

   /* Determine which tracks are audio and create the playlist */
   for(n = _damp_cd_first_track; n <= _damp_cd_last_track; n++)
   {
      if(cd_is_audio(n))
      {
         audio_count++;

         _damp_cd_playlist = realloc(_damp_cd_playlist,
                                     sizeof(int)*audio_count);

         if(_damp_cd_playlist == NULL)
         {
            printf("_damp_cd_init(): Out of memory creating CD playlist\n");
         }

         _damp_cd_playlist[audio_count-1] = n;
      }
   }

   if(damp_developer)
      printf("_damp_cd_init(): %d of the tracks are audio.\n", audio_count);

   /* Allocate playlist order */
   _damp_cd_playlist_size = audio_count;
   _damp_cd_playlist_order = malloc(sizeof(int)*audio_count);
   for(n=0; n<audio_count; n++)
      _damp_cd_playlist_order[n] = n;

   /* Randomize the order (since it isn't used if we're not in random mode) */
   for(n=0;n<audio_count; n++)
   {
      p=rand()%audio_count;

      swap = _damp_cd_playlist_order[n];
      _damp_cd_playlist_order[n] = _damp_cd_playlist_order[p];
      _damp_cd_playlist_order[p] = swap;
   }

   _damp_cd_current_track = 0;

   if(audio_count == 0)
   {
      printf("There are no audio tracks on the CD\n");
      exit(-1);
   }

   cd_set_volume(damp_volume, damp_volume);

   if(damp_developer) printf("_damp_cd_init(): Initialisation complete\n");
}

/*===================================================================
  damp_cd_get_track()

  Returns current track number.
  =================================================================*/

int damp_cd_get_track(void)
{
   if(!damp_playlist_random)
      return(_damp_cd_playlist[_damp_cd_current_track]);
   else
      return(_damp_cd_playlist[_damp_cd_playlist_order[_damp_cd_current_track]]);
}

/*===================================================================
  damp_cd_previous()

  Goes to the previous track
  =================================================================*/

void damp_cd_previous(void)
{
   if(_damp_cd_current_track == 0)
      _damp_cd_current_track = _damp_cd_playlist_size;

   _damp_cd_current_track -= 2;

   need_to_quit = 1;
}

/*===================================================================
  damp_cd_next()

  Goes to the next track
  =================================================================*/

void damp_cd_next(void)
{
   if(_damp_cd_current_track == _damp_cd_playlist_size - 1)
      if(!damp_playlist_continuous)
         need_to_quit_program = 1;

   need_to_quit = 1;
}

/*===================================================================
  _damp_cd_exit()

  Shuts down the CD player system.  You don't normally need to call
  this as its installed as an atexit() routine by _damp_cd_init()
  =================================================================*/

void _damp_cd_exit(void)
{
   cd_stop();
   cd_set_volume(255,255);
   cd_exit();
}

