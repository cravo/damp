/* DAMP - A DOS MP3 Player 
   By Matt Craven <damp@damp-mp3.co.uk>
   (c)1999/2000 Hedgehog Software

   FILE : DAMP.C
   DESC : The main core file
*/

/*===================================================================
  I N C L U D E S
  =================================================================*/

#include <stdio.h>
#include <string.h>
#include <allegro.h>
#include <time.h>
#include <conio.h>
#include <malloc.h>
#include <dpmi.h>

/* This bit will remove some of Allegro, making the exe smaller */
BEGIN_COLOR_DEPTH_LIST
   COLOR_DEPTH_8
END_COLOR_DEPTH_LIST

/*=========================================================
  DAMP includes
  =======================================================*/

#include "damp.h"

#include "audio.h"
#include "playlist.h"
#include "graphics.h"
#include "id3read.h"
#include "jstick.h"
#include "lcd.h"
#include "indrv.h"
#include "gui.h"
#include "input.h"
#include "util.h"
#include "beautify.h"
#include "sleep.h"
#include "lyrics3.h"
#include "cd.h"
#include "gtext.h"
#include "keys.h"

/*=========================================================
  global variables
  =======================================================*/

float damp_version = 0.97;
char damp_sub_version[] = { 'W', 'I', 'P',' ','4',0  };

int damp_yield = FALSE;

int damp_gui_win_info;        /* identifiers for gui windows */
int damp_gui_win_vis;
int damp_gui_win_controls;

int damp_gui_label_filename;  /* identifiers for gui labels */
int damp_gui_label_artist;    
int damp_gui_label_title;     
int damp_gui_label_comment;   
int damp_gui_label_year;      
int damp_gui_label_genre;
int damp_gui_label_rate;      
int damp_gui_label_type;      
int damp_gui_label_trackno;
int damp_gui_label_time;

int damp_gui_button_previous;      /* identifiers for gui_buttons */
int damp_gui_button_rewind;
int damp_gui_button_pause;
int damp_gui_button_fastforward;
int damp_gui_button_next;
int damp_gui_button_surround;
int damp_gui_button_random;
int damp_gui_button_quit;

int damp_want_beep = FALSE;         /* Beep at start of new track? */

int damp_beautify_filenames = TRUE; /* Make filenames look nice? */

int damp_id3_support = TRUE;        /* Endable/Disable ID3 support */

int damp_want_scope = TRUE;         /* Enable/Disable the text-mode scope */

int damp_scope_smooth = TRUE;       /* text-mode scope smoothing? */

int start_of_new_track;             /* Used to reset sync_to_beats */
int quality=4;                      /* 1=poor, 10=excellent, 0=best */
float threshold[] = { 999.0, 8.0, 4.0, 3.0, 2.0, 1.75, 1.25, 0.90, 0.5, 0.1, -1.0, -999.0};
int current_threshold=5;            /* From 0 to 10, index into above array, 11 is user-defined */
int remember_scroll_times = -1;     /* To remember how many times to scroll the track title */
char scopechar[2];                  /* The text-mode scope character */

long damp_file_size;                /* Current file size */

int damp_display_time_remaining=FALSE;  /* Want remaining time display ? */

char damp_state;                    /* Current state: one of the DAMP_STATE_*
                                       defines in damp.h */

/*=======================================================================
  cursor_on()

  Desc:
  The following is an atexit function to make sure the cursor gets
  turned back on when DAMP quits.
  =====================================================================*/

void cursor_on(void)
{
   _setcursortype(_NORMALCURSOR);
}


/*===================================================================
  main(argc, argv)

  Desc:
  Runs the show...
  =================================================================*/

int main(int argc,char *argv[])
{
   int x,p,q,flag;               /* flags and loop counters */
   int need_menu = FALSE;
   int n;
   char *file_is_null;           /* To check return value from the playlist
                                    function which returns the next file to
                                    play */

   /* Check to see if DAMP is being used for audio CD playback */
   for(n=0;n<argc;n++)
      if(strcmp(argv[n],"-cd")==0)
         damp_using_cd = 1;

   /* Initialise Allegro */
   allegro_init();

   /* Display the DAMP title */
   display_program_title();

   
   /* Exit if the user didn't specify a filename, but give them usage
      instructions first, because we're kind like that ;o) */
   if (argc<2)
   {
      if(!damp_using_cd)
      {
         usage(get_filename(argv[0]));
         exit(-1);
      }
   }

   /* Clear the damp_ini_file string */
   for(n=0;n<1024;n++)
      damp_ini_file[n]=0;

   /* make sure we read damp.ini from the damp directory */
   sprintf(damp_ini_file,"%s",replace_filename(argv[0],argv[0],"damp.ini",1023));

   /* pad with 0s */
   x=strlen(damp_ini_file);
   for(n=x;n<1024;n++)
      damp_ini_file[n] = 0;

   if(damp_developer) printf("Calling damp_init()\n");

   /* Initialise DAMP */
   damp_init();

   /* Make sure we turn the cursor on when DAMP exits */
   atexit(cursor_on);

   /* hide the cursor */
   _setcursortype(_NOCURSOR);

   /* Process the ini file and the command-line, remembering whether or
      not we want to display the options menu */
   need_menu = process_damp_ini();
   need_menu |= process_commandline(argc,argv);

   /* say which audio engine's being used */
   if(damp_developer)
      printf("Audio engine = %s\n",audio_driver->desc);

   /* If we want the keypad hack conio input, or the input driver
      doesn't want the keyboard handler, we won't install the
      Allegro keyboard driver */
   if(!damp_use_conio_input && !indrv_keyboard_removed)
      install_keyboard();

   if(damp_developer) printf("Calling jstick_get_functionality()\n");

   /* Process DAMP.INI for joystick functionality */
   jstick_get_functionality();

   /* Init LCD if required */
   if(lcd_in_use) lcd_init();

   /* Play CD if required */
   if(damp_using_cd)
   {
      damp_cd_main();
   }

   /* okay, we're done with the CD (or we didn't play the CD at all) */
   damp_using_cd = FALSE;
   need_to_quit = FALSE;
   need_to_quit_program = FALSE;

   /* Init GUI if required */
   if(damp_using_gui)
   {
      gui_init();
      gui_open_windows();
   }

   if(damp_developer)
      printf("damp_ini_file = %s\n",damp_ini_file);

   if(damp_metaplaylist_size > 0)
   {
      if(damp_metaplaylist_size > 1)
      {
         if(damp_metaplaylist_random) damp_metaplaylist_randomize();

         printf("\nPlaylist List\n");
         printf("=============\n\n");
         for(n=0;n<damp_metaplaylist_size;n++)
         {
            printf("%3d : %s\n",n+1,damp_metaplaylist[damp_metaplaylist_order[n]].filename);
         }
         printf("\n");
      }

      using_playlist = TRUE;
      damp_metaplaylist_entry = 0;
      damp_create_playlist(damp_metaplaylist[damp_metaplaylist_order[0]].filename);
   }

   /* randomize damp_playlist_order */
   for(x=0;x<damp_playlist_size;x++)
   {
      do
      {
         p = rand()%damp_playlist_size;
         flag = FALSE;
         for(q=0; q<x; q++)
         {
            if(damp_playlist_order[q] == p)
            {
               flag = TRUE;
               break;
            }
         }
      } while (flag);
      damp_playlist_order[x] = p;
   }

   if( damp_developer )
   {
      printf("\nSOUND DRIVER:\n");
      printf("name        : %s\n",digi_driver->name);
      printf("description : %s\n",digi_driver->desc);
   }

   /* Display the menu if needed */
   if(need_menu) menu();

   /* Play a startup MP3? */
   sprintf(file,"%s",get_config_string("[options]","startup_mp3","NONE"));
   if(stricmp(file,"NONE")!=0)
   {
      if(!exists(file))
         printf("ERROR: Startup MP3 '%s' doesn't exist!\n",file);
      else
      {
         if(!audio_driver->open(file))
         {
            printf("ERROR: Couldn't load startup MP3 '%s'\n",file);
         } else
         {
            do
            {
               if(key[KEY_ESC]) break;
               if(*joy_function.quit) break;
            }while(audio_driver->poll() >= 0);
         }
      }
   }

   /* make sure the keyboard buffer is empty, and debounce the joystick
      quit button */
   clear_keybuf();
   do{}while(*joy_function.quit);

   /* Initialise graphics module, if necessary */
   if(using_graphics)
   {
      if ( damp_developer )
      {
         printf("\nmain():");
         printf("  calling damp_gfx_init()\n");
      }

      using_graphics = damp_gfx_init();
      if(!using_graphics) printf("Failed to initialise graphics module.\n");
      else if(damp_using_gui) gui_window[damp_gui_win_vis].picture = damp_gfx_buffer;
   }

   /* If the GUI is in use, but we didn't specify -g, turn off the
      visualisation window */
   if(damp_using_gui && !using_graphics)
      gui_window[damp_gui_win_vis].visible = FALSE;

   /* Set the filename if using a playlist, and also set the
      short_filename */
   if(using_playlist) sprintf(file,"%s",damp_get_next_playlist_filename());
   strcpy(short_filename, get_filename(file));

   /* Beautify filename if necessary */
   if(damp_beautify_filenames)
      beautify(short_filename);

   /* Set up title scrolling */
   sprintf(damp_gfx_scroll_name,"                                                                                %s ",short_filename);
   sprintf(lcd_scroll_text,"%*c%s\n",lcd_display_width,' ',short_filename);
   lcd_scroll_pos=0;
   damp_gfx_scroll_name_pos = 0;

   /* output any id3 tag info */
   output_id3_info(file);

   /* and deal with any Lyrics3 tag */
   lyrics3_get_tag(file);

   /* the first track is number 1 */
   damp_track_number = 1;

   /* Fill playlist scroller if using GUI */
   if(damp_using_gui)
      gui_playlist_update();


   
/* ---- Main Loop ---------------------------------------------- */
   
      /* start by loading the file... */

      while (audio_driver->open(file))
      {
         damp_file_size = file_size(file);
   
         /* Set up and output the MP3 info */
         sprintf(ratestr,"%dkbps/%dHz MPEG Version %d Layer %d (%s)",audio_driver->bitrate,audio_driver->sample_rate,audio_driver->version,audio_driver->layer,(audio_driver->stereo ? "stereo" : "mono"));
   
         if(damp_track_number < 1)
            damp_track_number = 1;

         if(!using_graphics && !lcd_in_use && !damp_using_gui)
         {
            printf("\n%d. %s\n",damp_track_number,get_filename(short_filename));
            if(damp_show_file_info)
               printf("%s\n", ratestr);
         }
   
   
         if(damp_using_gui)
         {
            /* Put stuff in the info labels */
   
            sprintf(gui_window[damp_gui_win_info].label[damp_gui_label_filename].caption,short_filename);
            sprintf(gui_window[damp_gui_win_info].label[damp_gui_label_artist].caption,damp_id3_tag.artist);
            sprintf(gui_window[damp_gui_win_info].label[damp_gui_label_title].caption,damp_id3_tag.title);
            sprintf(gui_window[damp_gui_win_info].label[damp_gui_label_comment].caption,damp_id3_tag.comment);
            sprintf(gui_window[damp_gui_win_info].label[damp_gui_label_year].caption,damp_id3_tag.year);
            if((int)damp_id3_tag.genre >= 0)
              sprintf(gui_window[damp_gui_win_info].label[damp_gui_label_genre].caption, id3_genre_string[(int)damp_id3_tag.genre]);
            else
              sprintf(gui_window[damp_gui_win_info].label[damp_gui_label_genre].caption, "NONE");
            sprintf(gui_window[damp_gui_win_info].label[damp_gui_label_rate].caption,"%dkbps / %2.1fkHz",audio_driver->bitrate,(float)(audio_driver->sample_rate)/1000.0);
            sprintf(gui_window[damp_gui_win_info].label[damp_gui_label_type].caption,"MPEG Version %d Layer %d (%s)",audio_driver->version,audio_driver->layer,(audio_driver->stereo ? "stereo" : "mono"));
   
            gui_window[damp_gui_win_info].content_changed = TRUE;
   
            gui_window[damp_gui_win_controls].button[damp_gui_button_pause].pressed = track_paused;
            gui_window[damp_gui_win_controls].content_changed = TRUE;
         }
   
         /* Choose a random graphics waveform */
         damp_gfx_waveform = rand()%DAMP_GFX_WAVEFORMS;
         damp_gfx_modifier = rand()%DAMP_GFX_MODIFIERS;
         /* If using the graphics subsystem, create a random palette */
         if(using_graphics) damp_gfx_create_palette();
   
         need_to_quit=FALSE;
         start_of_new_track = TRUE;
         damp_gfx_scroll_times = remember_scroll_times;
   
         track_sel_pos = 0;
         for(n=0;n<8;n++) track_sel[n]=0;
   
         sprintf(timestr," 0:00");
   
         if(damp_want_beep)
         {
            sound(300);
            rest(1000);
            sound(0);
         }

         while(!need_to_quit && !need_to_quit_program)
         {
            if(*joy_function.quit)
               need_to_quit_program = TRUE;
   
            if(damp_using_gui)
            {
               sprintf(gui_window[damp_gui_win_controls].label[damp_gui_label_trackno].caption,"Track %d",damp_track_number);
               sprintf(gui_window[damp_gui_win_controls].label[damp_gui_label_time].caption,"%5s",damp_display_time_remaining ? timeremainstr : timestr);
               gui_window[damp_gui_win_controls].content_changed = TRUE;
            }
   
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

            /* Poll input. If it returns non-zero then we break out of this
               loop */
            if (input_handler(0)) break;
   
            if(quality > 0)
            {
               for(n=0;n<quality;n++)
                  if(!track_paused)
                     if(audio_driver->poll() < 0)
                     {
                        need_to_quit=TRUE;
                        selecting_track_flag = FALSE;
                     }
            } else
            {
               if (!track_paused) need_to_quit=audio_driver->run();
               if (need_to_quit)
               {
                  selecting_track_flag = FALSE;
                  break;
               }
            }
   
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

               if(strcmp(track_sel,"999999")==0)
               {
                  util_quit();
                  break;
               }

               trackno = atoi(track_sel)-1;
   
               if(track_sel_shift_flag)
               {
                  if(damp_developer) printf("Selecting from meta-playlist\n");
   
                  /* selecting playlist */
                  track_sel_shift_flag = FALSE;
                  if (trackno >= 0 && trackno < damp_metaplaylist_size)
                  {
                     damp_playlist_free();
                     damp_metaplaylist[damp_metaplaylist_order[damp_metaplaylist_entry]].been_played = TRUE;
                     damp_metaplaylist_entry = trackno;
                     if(!using_graphics && !damp_using_gui) printf("\n\n");
                     damp_create_playlist(damp_metaplaylist[damp_metaplaylist_order[damp_metaplaylist_entry]].filename);
                     damp_playlist_entry=-1;
                     damp_track_number = 0;
                     break;
                  } else
                  {
                     track_sel_pos = 0;
                     for(trackno=0;trackno<8;trackno++) track_sel[trackno]=0;
                  }
               } else
               {
                  if(trackno >= 0 && trackno < damp_playlist_num_to_play)
                  {
                     sprintf(file,"%s",damp_playlist_select_track(trackno));
                     selecting_track_flag=TRUE;
                     if(damp_developer) printf("OKAY: trackno >= 0 && trackno < damp_playlist_num_to_play\nSelected track: %s\n",file);
                     damp_track_number = trackno+1;
   
                     break;
                  } else
                  {
                     if(damp_developer) printf("WELL D'OH! : trackno < 0 || trackno >= damp_playlist_num_to_play\n");
   
                     track_sel_pos = 0;
                     for(trackno=0;trackno<8;trackno++) track_sel[trackno]=0;
                  }
               }
            }


            /* render and handle the GUI if necessary */
            if(damp_using_gui)
            {
               if(using_graphics)
                  gui_window[damp_gui_win_vis].content_changed = TRUE;
   
               gui_render();
               gui_handler();
            }
   
   
            timer(0); /* Update the timer string */
   
            /* Calculate and display the scope */
            scope();
            if(!damp_using_gui && !using_graphics && damp_want_scope &&!lcd_in_use)
            {
               printf("%s %s %s\r",damp_display_time_remaining ? timeremainstr : timestr, scopestr, track_sel_pos>0 ? track_sel : "      ");
               fflush(stdout);
            }
            if(lcd_in_use) lcd_display();
   
            /* Process lyrics3 schedule */
            lyrics3_run();

            /* Give the CPU back to any multitasker */
            if(damp_yield)
               __dpmi_yield();
         }
   
         /* ---- End of track ------------------------------------------- */
   
         /* little hack to try and reduce the click at the end of a track
            I doubt it'll work - I think the click actually occurs at the
            start of the next track */
         for(n=damp_volume; n>=0; n--)
         {
            set_volume(n,-1);
            audio_driver->poll();
         }
   
         set_volume(0,-1);
         audio_driver->run();

         audio_driver->close(file);

         /* remove interrupts */
         remove_int(set_timeout_flag);
         timeout_flag = FALSE;
   
         /* If we're not using a playlist, or the ESCAPE key was pressed,
            break out to the -Exit Program- section, below. */
         if(!using_playlist || need_to_quit_program)
            break;
   
         /* Choose a new graphics waveform and palette */
         if(using_graphics)
         {
            damp_gfx_waveform = rand()%DAMP_GFX_WAVEFORMS;
            while(!damp_gfx_wave_use[damp_gfx_waveform])
               damp_gfx_waveform = (damp_gfx_waveform+1)%DAMP_GFX_WAVEFORMS;
            damp_gfx_modifier = rand()%DAMP_GFX_MODIFIERS;
            while(!damp_gfx_mod_use[damp_gfx_modifier])
               damp_gfx_modifier = (damp_gfx_modifier+1)%DAMP_GFX_MODIFIERS;
            if(using_graphics) damp_gfx_create_palette();
         }
   
         if(damp_developer) printf("\n\nselecting_track_flag = %d\n\n",selecting_track_flag);
         if(damp_developer)
         {
            printf("damp_metaplaylist_entry = %d\ndamp_metaplaylist_size-1 = %d\n",damp_metaplaylist_entry,damp_metaplaylist_size-1);
            fflush(stdout);
         }
   
         if(!selecting_track_flag)
         {
            damp_track_number++;
   
            /* If using a playlist, get the next filename */
            do
            {
               file_is_null=damp_get_next_playlist_filename();
               if(file_is_null != NULL)
                  strcpy(file,file_is_null);
               else
               {
                  /* If there's no files left to play, select next
                  metaplaylist or quit */
   
                  damp_track_number = 1;
   
                  if (damp_metaplaylist_entry >= damp_metaplaylist_size-1)
                     exit(-1);
                  else
                  {
                     damp_playlist_free();
                     damp_metaplaylist[damp_metaplaylist_order[damp_metaplaylist_entry]].been_played = TRUE;
                     damp_metaplaylist_entry++;
                     if(!using_graphics && !damp_using_gui) printf("\n\n");
                     damp_create_playlist(damp_metaplaylist[damp_metaplaylist_order[damp_metaplaylist_entry]].filename);
                     damp_playlist_entry=-1;
                     file_is_null=damp_get_next_playlist_filename();
                     if(file_is_null != NULL)
                        strcpy(file,file_is_null);
                     else
                        exit(-1);
                     gui_playlist_update();
                  }
               }
            } while(stricmp(get_extension(file) , "MP3") != 0 && stricmp(get_extension(file),"MP2") != 0);
         } else
         {
            selecting_track_flag = FALSE;
         }
   
         if(!using_graphics && !damp_using_gui) printf("\n");
         strcpy(short_filename, get_filename(file));
   
         if(damp_beautify_filenames)
            beautify(short_filename);
   
         sprintf(damp_gfx_scroll_name,"                                                                                %s ",short_filename);
         sprintf(lcd_scroll_text,"%*c%s\n",lcd_display_width,' ',short_filename);
         lcd_scroll_pos = 0;
   
         damp_gfx_scroll_name_pos = 0;
   
         /* output any id3 tag info */
         output_id3_info(file);
   
         /* And deal with any Lyrics3 tag */
         lyrics3_get_tag(file);
   
         set_volume(damp_volume,-1);
      }

   /* ---- Exit program ------------------------------------------ */

   if(lcd_in_use) lcd_deinit();
   if(indrv_in_use) indrv_deinit();
   set_leds(-1);

   if(using_graphics) damp_gfx_shutdown();
   if(!damp_using_cd)
   {
      damp_playlist_free();
      damp_metaplaylist_free();
   
      if( damp_developer )
      {
         printf("\nGFX DRIVER: %s\n",damp_gfx_driver);
      }
   
      audio_exit();
   }

   exit(0);
}

/*===================================================================
  display_program_title()

  Desc: Pretty obvious what this does. It outputs the program title
  =================================================================*/

void display_program_title(void)
{
   printf("\n");
   printf("<------------------------------ D A M P ------------------------------->\n");
   printf("<  MP3 Player for DOS by Matt Craven - (c)1999/2000 Hedgehog Software  >\n");
   printf("<---------------------------------------------------------------------->\n\n");
#ifdef PRIVATE_BETA
   printf("  This is a PRIVATE beta-testers version and is not for redistribution  \n\n");
#endif
}

/*===================================================================
  usage(char *progname)

  Desc   : Outputs program usage information
  Inputs : progname - The program executable name
  =================================================================*/

void usage(char *progname)
{
   printf("Usage: \n\n");
   printf("%s song.mp3 {options}    : Play an MP3 file\n",progname);
   printf("%s song.mp2 {options}    : Play an MP2 file\n",progname);
   printf("%s song.m3u {options}    : Play a WinAmp playlist\n",progname);
   printf("%s song.pls {options}    : Play a Sonique playlist\n",progname);
   printf("\nSee README.TXT for a full list of options.\n");

/* We used to output the options, but the list got so long that it
   scrolled the bit above off the top of the screen, so it really is best
   just to look in the readme...

   printf("\nWhere {options} is one (or more, or none) of:\n\n");
   printf("-r         : When playing a playlist, play tracks randomly\n");
   printf("-g         : Enable full graphical display\n");
   printf("-p <n>     : Only play <n> tracks from the playlist\n");
   printf("-s         : Enable graphics sync to beats\n");
   printf("-s <n>     : Sync to beats, and set the threshold to <n>\n");
   printf("-q <n>     : Set playback quality to <n> - (default=4)\n");
   printf("-bnw       : Use an alternating black/white palette\n");
   printf("-n         : Display track name in graphics mode\n");
   printf("-v         : Display DAMP version number\n");
   printf("-m         : Display an options menu\n");
   printf("-z         : Don't change the zoomer when syncing to beats\n");
   printf("-c         : Continuous playback\n");
   printf("-noid3     : Disable id3 tag support\n");
   printf("-rp        : When playing multiple playlists, randomize order\n");
   printf("-noscope   : Disable the text-mode volume display\n");
   printf("-surround  : Enable surround sound\n");
   printf("-keypadhack: Use a more compatible keyboard driver\n");
   printf("-beep      : Emit a beep at the start of each track\n");
*/
}

/*===================================================================
  scope()

  Desc: Sets up the text-based scope string, based on the
        current libAmp output. And, if the graphics subsystem is
        being used, it calls the graphics rendering function.
  =================================================================*/


void scope(void)
{
    static int max_total = 0;
    static int waited = 0;
    static int last_total = 0;
    unsigned short*this_left=audio_driver->buffer_left,*this_right=audio_driver->buffer_right;
    int n;
    int minitotal=0;
 
    unsigned short maxval,minval;
    unsigned short sigavg;

    minval = 65535;
    maxval = 0;

    if(start_of_new_track)
    {
      max_total = 0;
      waited = 0;
      start_of_new_track = FALSE;
    }

    if(audio_driver->buffer_size > 0 && this_left != NULL && this_right != NULL)
    {
       for(n=0;n<audio_driver->buffer_size;n++)
       {
          sigavg = (this_left[n]+this_right[n])/2;
          if(sigavg > maxval) maxval = sigavg;
          if(sigavg < minval) minval = sigavg;
       }
    }

    minitotal = (maxval-minval)*64/65535;

    if(!using_graphics && damp_scope_smooth)
    {
      minitotal = (7*last_total + minitotal) >> 3;
    }

    if (minitotal < 0 ) minitotal = 0;
    if (minitotal > 64) minitotal = 64;

    /* Stuff to make the text mode vu-meter "max level" indicator
       drop after a while */

    if (minitotal > max_total)
    {
      max_total = minitotal;
      waited=0;
    }
    else
    {
      waited++;
      if(waited >= 1000)
      {
         max_total--;
         waited=950;
      }
    }

    damp_vu = minitotal;
    gtext_vu_max = max_total;

    /* Build the vu string */
    sprintf(scopestr,"[");
    for(n=0;n<minitotal;n++)
      if ( n == max_total)
         strcat(scopestr,"|");
      else
         strcat(scopestr,scopechar);
    for(n=minitotal;n<64;n++)
      if ( n == max_total)
         strcat(scopestr,"|");
      else
         strcat(scopestr,"-");

    strcat(scopestr,"]");

   if(using_graphics && !track_paused)
   {
//      if(damp_gfx_sync_to_beats && minitotal > 40*threshold[current_threshold])
      if(damp_gfx_sync_to_beats && minitotal > 8 && minitotal > last_total*threshold[current_threshold])
      {
         /* Beat detected, so change stuff */
         damp_gfx_waveform = rand()%DAMP_GFX_WAVEFORMS;
         while(!damp_gfx_wave_use[damp_gfx_waveform])
            damp_gfx_waveform = (damp_gfx_waveform+1)%DAMP_GFX_WAVEFORMS;

         if(damp_gfx_randomize_modifier) damp_gfx_modifier = rand()%DAMP_GFX_MODIFIERS;
         while(!damp_gfx_mod_use[damp_gfx_modifier])
            damp_gfx_modifier = (damp_gfx_modifier+1)%DAMP_GFX_MODIFIERS;

         damp_gfx_create_palette();
      }

      damp_gfx_render_frame(9999);
   }

   last_total = minitotal;
}

/*===================================================================
  timer()

  Desc: Sets up the time string, so the current track time can be
        displayed. Also sets up the remaining time string
  =================================================================*/

void timer(int forceupdate)
{
   static int last_time = -999;
   int this_time=audio_driver->time;
   int remaintime;
   float totaltime=0;
   float framelen;

   if(damp_using_cd)
   {
      this_time = damp_cd_time;
      remaintime = 0;
   }else
   {
      framelen = 144.0 * ((float)audio_driver->bitrate*1000) / (float)audio_driver->sample_rate;
      if(framelen != 0.0)
      {
        totaltime = (float)damp_file_size / (float)framelen;
        totaltime /= 1152.0;
        totaltime *= 60.0;
        if(audio_driver->stereo) totaltime /= 2.0;
      }
      remaintime = (totaltime - this_time);
   }

   if(remaintime<0) remaintime = 0;

   if(this_time > last_time || forceupdate)
   {
      sprintf(timestr,"%2d:%02d",this_time/60,this_time%60);
      sprintf(timeremainstr,"%2d:%02d",remaintime/60,remaintime%60);
   }

   timestr[5] = 0;
   timeremainstr[5] = 0;

   if (this_time!=last_time)
      last_time=this_time;
}

/*===================================================================
  menu()

  Desc: Displays a menu from which you can set some of the options.
        It's very old now - perhaps even so out of date it should
        be removed - but maybe someone uses it, so it's better left
        in.
  =================================================================*/

void menu(void)
{
   int memy;

   printf("DAMP Options Menu\n");
   printf("=================\n");
   printf("\n");
   printf("This allows you to set some of the non-specialised options by\n");
   printf("pressing the appropriate number to toggle the option ON or OFF.\n");
   printf("Where a value is required, hold down the appropriate number, and \n");
   printf("use the + and - keys (not keypad) to alter the value.\n\n");
   printf("Press ENTER when you are ready to begin, or press ESC to quit.\n\n");

   printf("1. Random play        : %s\n", damp_playlist_random ? "ON " : "OFF");
   printf("2. Graphical output   : %s\n", using_graphics ? "ON " : "OFF");
   printf("3. Sync to beats      : %s\n", damp_gfx_sync_to_beats ? "ON " : "OFF");
   printf("4. Black & White mode : %s\n", damp_gfx_bnw_palette ? "ON " : "OFF");
   printf("5. Tracks to play     : %d      \n",damp_playlist_num_to_play);
   printf("6. Playback quality   : %d      \n",quality);

   do
   {
      if(key[KEY_1])
      {
         damp_playlist_random = !damp_playlist_random;

         memy=wherey();
         gotoxy(1,wherey()-6);
         printf("1. Random play        : %s\n", damp_playlist_random ? "ON " : "OFF");
         gotoxy(1,memy);
         do{}while(key[KEY_1]);
      }

      if(key[KEY_2])
      {
         using_graphics = !using_graphics;

         memy=wherey();
         gotoxy(1,wherey()-5);
         printf("2. Graphical output   : %s\n", using_graphics ? "ON " : "OFF");
         gotoxy(1,memy);
         do{}while(key[KEY_2]);
      }

      if(key[KEY_3])
      {
         damp_gfx_sync_to_beats = !damp_gfx_sync_to_beats;

         memy=wherey();
         gotoxy(1,wherey()-4);
         printf("3. Sync to beats      : %s\n", damp_gfx_sync_to_beats ? "ON " : "OFF");
         gotoxy(1,memy);
         do{}while(key[KEY_3]);
      }

      if(key[KEY_4])
      {
         damp_gfx_bnw_palette = !damp_gfx_bnw_palette;

         memy=wherey();
         gotoxy(1,wherey()-3);
         printf("4. Black & White mode : %s\n", damp_gfx_bnw_palette ? "ON " : "OFF");
         gotoxy(1,memy);
         do{}while(key[KEY_4]);
      }

      if(key[KEY_5])
      {
         if(key[KEY_EQUALS] && damp_playlist_num_to_play < damp_playlist_size)
         {
            damp_playlist_num_to_play++;
            do{}while(key[KEY_EQUALS]);
         }
         if(key[KEY_MINUS] && damp_playlist_num_to_play > 1)
         {
            damp_playlist_num_to_play--;
            do{}while(key[KEY_MINUS]);
         }

         memy=wherey();
         gotoxy(1,wherey()-2);
         printf("5. Tracks to play     : %d      \n",damp_playlist_num_to_play);
         gotoxy(1,memy);
      }

      if(key[KEY_6])
      {
         if(key[KEY_EQUALS] && quality < 50)
         {
            quality++;
            do{}while(key[KEY_EQUALS]);
         }
         if(key[KEY_MINUS] && quality > 0)
         {
            quality--;
            do{}while(key[KEY_MINUS]);
         }

         memy=wherey();
         gotoxy(1,wherey()-1);
         printf("6. Playback quality   : %d   %s   \n",quality,quality==0?"HQ":"  ");
         gotoxy(1,memy);
      }

      if(key[KEY_ESC]) break;
   }while(!key[KEY_ENTER]);

   if(key[KEY_ESC]) exit(0);

   do{}while(key[KEY_ENTER]);
   clear_keybuf();
}

//========================================================================
// output_id3_info(filename)
//
// A generic routine to output id3 info to stdout
//========================================================================

void output_id3_info(char *filename)
{
   char trim_title[31];
   char trim_artist[31];
   int p;

   sprintf(damp_id3_tag.title,"**NOID3**");

   if(damp_id3_support)
   {
      if(get_mp3_info(filename,&damp_id3_tag))
      {
         if(!using_graphics && !damp_using_gui)
         {
            printf("\n==== id3 tag info ====\n");
            printf("Title  : %s\n",damp_id3_tag.title);
            printf("Artist : %s\n",damp_id3_tag.artist);
            printf("Album  : %s\n",damp_id3_tag.album);
            printf("Year   : %s\n",damp_id3_tag.year);
            printf("Comment: %s\n",damp_id3_tag.comment);
//            printf("Track  : %c\n",damp_id3_tag.track);
            if((int)damp_id3_tag.genre >= 0)
            {
              sprintf(damp_id3_genre, "%s", id3_genre_string[(int)damp_id3_tag.genre]);
              printf("Genre  : %s\n",damp_id3_genre);
            }else
            {
              printf("Genre  : NONE\n");
              sprintf(damp_id3_genre, "NONE");
            }

            printf("\n");
         }
   
         for(p=0;p<31;p++)
         {
            trim_title[p]=0;
            trim_artist[p]=0;
         }
   
         p=30;
         while(damp_id3_tag.title[p]<=32 && p>0)
         {
            damp_id3_tag.title[p]=0;
            p--;
         }
         while(p>=0) { trim_title[p] = damp_id3_tag.title[p]; p--; }

         p=30;
         while(damp_id3_tag.artist[p]<=32 && p>0)
         {
            damp_id3_tag.artist[p]=0;
            p--;
         }

         while(p>=0) { trim_artist[p] = damp_id3_tag.artist[p]; p--; }
   
         sprintf(damp_gfx_scroll_name,"                                                                                %s - %s",trim_artist,trim_title);
         sprintf(lcd_scroll_text,"%*c%s - %s\n",lcd_display_width,' ',trim_artist,trim_title);
         lcd_scroll_pos=0;
      }
   }
}

//===================================================================
// gui_open_windows()
// Opens the gui windows
//===================================================================

void gui_open_windows(void)
{
   char msg[256];

   sprintf(msg,"DAMP v%1.2f %s - (c)1999/2000 Hedgehog Software",damp_version, damp_sub_version);

   gui_create_window(0,0,GUI_SCREEN_W-8,GUI_SCREEN_H-20,msg,FALSE);
   damp_gui_win_vis = gui_create_window(0,140,320,200,"Visualisation",TRUE);
   damp_gui_win_info = gui_create_window(20,32,600,80,"Information",TRUE);
   damp_gui_win_controls = gui_create_window(20, 420, 480, 24, "Controls", TRUE);
   damp_gui_win_playlist = gui_create_window(320, 140, 300, 200, "Playlist", TRUE);

   /* create the labels in the info window */

   gui_create_label(damp_gui_win_info, 2,2, 76,11, "Filename:", FALSE);
   damp_gui_label_filename = gui_create_label(damp_gui_win_info, 78,2, 520,11, "Big Long Filename Here.mp3", TRUE);
   gui_create_label(damp_gui_win_info, 2,15, 76,11, "Artist  :", FALSE);
   damp_gui_label_artist = gui_create_label(damp_gui_win_info, 78,15, 320,11, "012345678901234567890123456789", TRUE);
   gui_create_label(damp_gui_win_info, 2,28, 76,11, "Title   :", FALSE);
   damp_gui_label_title = gui_create_label(damp_gui_win_info, 78,28, 320,11, "012345678901234567890123456789", TRUE);
   gui_create_label(damp_gui_win_info, 2,41, 76,11, "Comment :", FALSE);
   damp_gui_label_comment = gui_create_label(damp_gui_win_info, 78,41, 320,11, "012345678901234567890123456789", TRUE);
   gui_create_label(damp_gui_win_info, 2,54, 76,11, "Year    :", FALSE);
   damp_gui_label_year = gui_create_label(damp_gui_win_info, 78,54, 50,11, "9999", TRUE);
   gui_create_label(damp_gui_win_info, 320+78+2,15, 76,11, "Genre   :", FALSE);
   damp_gui_label_genre = gui_create_label(damp_gui_win_info, 320+78+2+76,15, 122,11, "9999", TRUE);
   gui_create_label(damp_gui_win_info, 140,54, 76,11, "Rate    :", FALSE);
   damp_gui_label_rate = gui_create_label(damp_gui_win_info, 140+78,54, 180,11, "128kbps / 44.1kHz", TRUE);
   gui_create_label(damp_gui_win_info, 2,67, 76,11, "Type    :", FALSE);
   damp_gui_label_type = gui_create_label(damp_gui_win_info, 78,67, 320,11, "Version 1 Layer 3 (stereo)", TRUE);

   /* Create buttons & labels in controls window */
   damp_gui_label_trackno = gui_create_label(damp_gui_win_controls, 322,2, 100,11, "Track 0", FALSE);
   damp_gui_label_time = gui_create_label(damp_gui_win_controls, 424,2, 50,11, "00:00", FALSE);

   damp_gui_button_previous = gui_create_button(damp_gui_win_controls, 2, 2, 32, 16, "|<", GUI_BUTTON_CLICK, util_previous_track);
   damp_gui_button_rewind = gui_create_button(damp_gui_win_controls, 34, 2, 32, 16, "<<", GUI_BUTTON_HOLD, util_rewind);
   damp_gui_button_pause = gui_create_button(damp_gui_win_controls, 66, 2, 32, 16, "||", GUI_BUTTON_TOGGLE, util_pause);
   damp_gui_button_fastforward = gui_create_button(damp_gui_win_controls, 98, 2, 32, 16, ">>", GUI_BUTTON_HOLD, util_fast_forward);
   damp_gui_button_next = gui_create_button(damp_gui_win_controls, 130, 2, 32, 16, ">|", GUI_BUTTON_CLICK, util_next_track);
   damp_gui_button_surround = gui_create_button(damp_gui_win_controls, 162, 2, 32, 16, "S", GUI_BUTTON_TOGGLE, util_surround_toggle);
   damp_gui_button_random = gui_create_button(damp_gui_win_controls, 194, 2, 32, 16, "R", GUI_BUTTON_TOGGLE, util_random_toggle);
   damp_gui_button_quit = gui_create_button(damp_gui_win_controls, 226, 2, 32, 16, "x", GUI_BUTTON_CLICK, util_quit);

   gui_window[damp_gui_win_controls].button[damp_gui_button_random].pressed = damp_playlist_random;
   gui_window[damp_gui_win_controls].button[damp_gui_button_surround].pressed = audio_driver->surround;

   /* Create stuff for playlist window */
   damp_gui_list_playlist = gui_create_list(damp_gui_win_playlist, 10, 10, 280, 180, 1);
}

//===================================================================
// process_commandline(argc,argv)
//
// Processes the command line arguments
// Returns TRUE if the menu was requested, FALSE otherwise
//===================================================================

// Helper define to stop us checking args that don't exist.
#define CLAMPARG(x) (x >= argc ? argc-1 : x)

int process_commandline(int argc, char **argv)
{
   int arg;
   int need_menu = FALSE;
   int plist_count,n;
   int need_gui = FALSE;

   /* special check for -developer switch */
   for(arg = 1; arg<argc; arg++)
   {
      if(stricmp(argv[arg],"-developer")==0)
      {
         printf("\n** DAMP IS RUNNING IN DEVELOPER MODE **\n");
         printf("Allegro version: %s\n",allegro_id);
         damp_developer = TRUE;
      }
   }

   if(damp_developer) printf("Processing commandline...\n");

   // NOW we can process the args.

   for(arg = 1; arg<argc; arg++)
   {
      if(damp_developer) printf("  option or file: %s\n", argv[arg]);

      if(strcmp(argv[arg],"-c")==0 || strcmp(argv[arg],"-continuous")==0)
         damp_playlist_continuous = TRUE;

      if(strcmp(argv[arg],"-r")==0 || strcmp(argv[arg],"-random")==0 )
         damp_playlist_random = TRUE;

      if(strcmp(argv[arg],"-rp")==0 || strcmp(argv[arg],"-randomplaylists")==0 )
         damp_metaplaylist_random = TRUE;

      if(strcmp(argv[arg],"-p")==0 || strcmp(argv[arg],"-play")==0 )
      {
         damp_playlist_num_to_play = atoi(argv[CLAMPARG(arg+1)]);
         if(damp_playlist_num_to_play > damp_playlist_size)
            damp_playlist_num_to_play = damp_playlist_size;
      }

      if(strcmp(argv[arg],"-skip")==0 )
      {
         damp_playlist_num_to_skip = atoi(argv[CLAMPARG(arg+1)]);
      }

      if(strcmp(argv[arg],"-g")==0 || strcmp(argv[arg],"-graphics")==0 )
      {
         using_graphics = TRUE;
      }

      if(strcmp(argv[arg],"-s")==0 || strcmp(argv[arg],"-sync_to_beats")==0 )
      {
         if(atoi(argv[CLAMPARG(arg+1)]) != 0 || strcmp(argv[CLAMPARG(arg+1)],"0")==0)
         {
            current_threshold = atoi(argv[arg+1]);
            if(current_threshold<0) current_threshold=0;
            if(current_threshold>10) current_threshold=10;

            if(damp_developer) printf("\ncurrent_threshold set to %d by commandline argument (-s).\n",current_threshold);
         }

         damp_gfx_sync_to_beats = TRUE;
      }

      if(strcmp(argv[arg],"-q")==0 || strcmp(argv[arg],"-quality")==0 )
      {
         quality = atoi(argv[CLAMPARG(arg+1)]);
         if(damp_developer) printf("\nQuality set to %d by commandline argument.\n",quality);
      }

      if(strcmp(argv[arg],"-bnw")==0 || strcmp(argv[arg],"-black_and_white")==0 )
      {
         damp_gfx_bnw_palette = TRUE;
      }

      if(strcmp(argv[arg],"-n")==0 || strcmp(argv[arg],"-name")==0 )
      {
         damp_gfx_scroll_times = atoi(argv[CLAMPARG(arg+1)]);
         remember_scroll_times = damp_gfx_scroll_times;
         damp_gfx_show_name = TRUE;
      }

      if(strcmp(argv[arg],"-t")==0 || strcmp(argv[arg],"-threshold")==0 )
      {
         current_threshold = atoi(argv[CLAMPARG(arg+1)]);
         if(current_threshold<0) current_threshold=0;
         if(current_threshold>10) current_threshold=10;
      }

      if(strcmp(argv[arg],"-m")==0 || strcmp(argv[arg],"-menu")==0 )
      {
         need_menu = TRUE;
      }

      if(strcmp(argv[arg],"-v")==0 || strcmp(argv[arg],"-version")==0 )
      {
         printf("\nDAMP version %1.2f %s (%s - %s)\n\n",damp_version,damp_sub_version,__DATE__,__TIME__);
      }

      if(strcmp(argv[arg],"-z")==0 || strcmp(argv[arg],"-zoomchange")==0 )
      {
         damp_gfx_randomize_modifier = FALSE;
      }

      if( strcmp(argv[arg],"-noid3")==0 )
      {
         damp_id3_support = FALSE;
      }
      
      if( strcmp(argv[arg],"-noscope")==0 )
      {
         damp_want_scope = FALSE;
      }

      if( strcmp(argv[arg],"-surround")==0 )
      {
         audio_driver->surround = TRUE;
      }

      if( strcmp(argv[arg],"-keypadhack")==0 )
      {
         damp_use_conio_input = TRUE;
         remove_keyboard();
      }

      if( strcmp(argv[arg],"-beatsync")==0 )
      {
         threshold[11] = atof(argv[CLAMPARG(arg+1)]);
         current_threshold = 11;
         damp_gfx_sync_to_beats = TRUE;
         if(damp_developer)
            printf("User manual beat sync: %f\n",threshold[11]);
      }

      if( strcmp(argv[arg],"-lcd")==0 )
      {
         lcd_in_use = TRUE;
      }

      if( strcmp(argv[arg],"-indrv")==0 )
      {
         indrv_in_use = TRUE;
         indrv_init();
      }

      if( strcmp(argv[arg],"-beep")==0 )
      {
         damp_want_beep = TRUE;
      }

      if( strcmp(argv[arg],"-fnfnoquit")==0 )
      {
         if(damp_developer) printf("Using -fnfnoquit option.\n");
         damp_fnfnoquit = TRUE;
      }

      if( strcmp(argv[arg],"-gui")==0 )
      {
         if(damp_developer) printf("Using gui.\n");
         need_gui = TRUE;
      }

      if( strcmp(argv[arg],"-paused")==0 )
      {
         if(damp_developer) printf("Starting paused.\n");
         util_pause();
      }

      if( strcmp(argv[arg],"-remaining")==0 )
      {
         if(damp_developer) printf("Displaying time remaining rather than time elapsed.\n");
         damp_display_time_remaining = TRUE;
      }

      if( strcmp(argv[arg],"-sleep")==0 )
      {
         sleep_init(atoi(argv[CLAMPARG(arg+1)]));
      }

      if( strcmp(argv[arg],"-yield")==0 )
      {
         damp_yield = TRUE;
      }
   }

   if(damp_developer) printf("All arguments parsed\n");

   if(need_gui)
   {
      if(damp_developer) printf("GUI option enabled\n");
      damp_using_gui = TRUE;
   }

   if(damp_developer) printf("Reading MP3 files from command line\n");

   // Hack to speed up loading of *.MP3 under plain DOS - malloc the
   // playlist in one go rather than on a per-track basis:
   plist_count=0;
   for(n=1;n<argc;n++)
      if(stricmp(get_extension(argv[n]),"MP3")==0 || stricmp(get_extension(argv[n]),"MP2")==0)
      {
         if(damp_developer) printf("Verified: %s\n",argv[n]);
         plist_count++;
      }
   if(plist_count != 0)
   {
      if(damp_developer) printf("*.MP3 : DAMP is about to allocate %lu bytes for playlist.\n",sizeof(DAMP_PLAYLIST)*plist_count);

      damp_playlist = malloc(sizeof(DAMP_PLAYLIST)*plist_count);
      damp_playlist_order = malloc(sizeof(int)*plist_count);
      if (damp_playlist == NULL || damp_playlist_order == NULL)
      {
         printf("main(): Out of memory whilst allocating playlist\n\n");
         exit(-1);
      }

      using_playlist=TRUE;
      for(n=1;n<argc;n++)
         if(stricmp(get_extension(argv[n]),"MP3")==0 || stricmp(get_extension(argv[n]),"MP2")==0)
         {
            if(!exists(argv[n]))
            {
               if(!damp_fnfnoquit)
               {
                  if(damp_developer) printf("damp_fnfnoquit = FALSE\n");
                  printf("Error: File '%s' does not exist.\n\n",argv[n]);
                  exit(-1);
               }
            } else
            {
               if(damp_developer) printf("*.MP3 : (%d) Adding %s\n",n,argv[n]);
   
               /* Set up this entry */
      
               sprintf(damp_playlist[damp_playlist_size].filename,"%s",argv[n]);
               damp_playlist[damp_playlist_size].been_played = FALSE;
               damp_playlist_entry_compute_time(argv[n],&damp_playlist[damp_playlist_size]);
               damp_playlist_size++;
   //         damp_playlist_add_entry_to_already_malloced(argv[n]);
            }
         }

   }

   damp_playlist_num_to_play = damp_playlist_size;
   damp_playlist_entry = -1;

   if(damp_developer) printf("Looking for playlist files on the command line\n");

   // Look for playlist files and add them to the metaplaylist
   for(n=1;n<argc;n++)
   {
      if(stricmp(get_extension(argv[n]),"M3U")==0 || stricmp(get_extension(argv[n]),"PLS")==0)
      {
         damp_metaplaylist_add_entry(argv[n]);
      }
   }


   return need_menu;
}

//=========================================================================
// process_damp_ini()
//
// Processes the DAMP.ini file for options
// Returns TRUE if the menu was requested, FALSE otherwise.
//=========================================================================

int process_damp_ini(void)
{
   char ini_string[256];
   int need_menu = FALSE;
   int need_gui = FALSE;
   int n;

   for(n=0;n<256;n++) ini_string[n]=0;

   set_config_file(damp_ini_file);

   damp_volume=get_config_int("[sound]","digi_volume",damp_volume);

   sprintf(ini_string,"%s",get_config_string("[options]","random_play","no"));
   if(stricmp(ini_string,"NO")==0 || stricmp(ini_string,"OFF")==0 || stricmp(ini_string,"FALSE")==0 || stricmp(ini_string,"0")==0)
      damp_playlist_random = FALSE;
   if(stricmp(ini_string,"YES")==0 || stricmp(ini_string,"ON")==0 || stricmp(ini_string,"TRUE")==0 || stricmp(ini_string,"1")==0)
      damp_playlist_random = TRUE;

   sprintf(ini_string,"%s",get_config_string("[options]","random_playlists","no"));
   if(stricmp(ini_string,"NO")==0 || stricmp(ini_string,"OFF")==0 || stricmp(ini_string,"FALSE")==0 || stricmp(ini_string,"0")==0)
      damp_metaplaylist_random = FALSE;
   if(stricmp(ini_string,"YES")==0 || stricmp(ini_string,"ON")==0 || stricmp(ini_string,"TRUE")==0 || stricmp(ini_string,"1")==0)
      damp_metaplaylist_random = TRUE;

   sprintf(ini_string,"%s",get_config_string("[options]","graphical_output","no"));
   if(stricmp(ini_string,"NO")==0 || stricmp(ini_string,"OFF")==0 || stricmp(ini_string,"FALSE")==0 || stricmp(ini_string,"0")==0)
      using_graphics = FALSE;
   if(stricmp(ini_string,"YES")==0 || stricmp(ini_string,"ON")==0 || stricmp(ini_string,"TRUE")==0 || stricmp(ini_string,"1")==0)
      using_graphics = TRUE;

   sprintf(ini_string,"%s",get_config_string("[options]","graphical_text_mode","no"));
   if(stricmp(ini_string,"NO")==0 || stricmp(ini_string,"OFF")==0 || stricmp(ini_string,"FALSE")==0 || stricmp(ini_string,"0")==0)
      gtext_in_use = FALSE;
   if(stricmp(ini_string,"YES")==0 || stricmp(ini_string,"ON")==0 || stricmp(ini_string,"TRUE")==0 || stricmp(ini_string,"1")==0)
      gtext_in_use = TRUE;

   sprintf(ini_string,"%s",get_config_string("[options]","black_and_white","no"));
   if(stricmp(ini_string,"NO")==0 || stricmp(ini_string,"OFF")==0 || stricmp(ini_string,"FALSE")==0 || stricmp(ini_string,"0")==0)
      damp_gfx_bnw_palette = FALSE;
   if(stricmp(ini_string,"YES")==0 || stricmp(ini_string,"ON")==0 || stricmp(ini_string,"TRUE")==0 || stricmp(ini_string,"1")==0)
      damp_gfx_bnw_palette = TRUE;

   sprintf(ini_string,"%s",get_config_string("[options]","display_track_title_times","0"));
   {
      damp_gfx_scroll_times = atoi(ini_string);
      remember_scroll_times = damp_gfx_scroll_times;
      damp_gfx_show_name = TRUE;
   }

   sprintf(ini_string,"%s",get_config_string("[options]","sync_to_beats","no"));
   if(stricmp(ini_string,"NO")==0 || stricmp(ini_string,"OFF")==0 || stricmp(ini_string,"FALSE")==0 || stricmp(ini_string,"0")==0)
      damp_gfx_sync_to_beats = FALSE;
   if(stricmp(ini_string,"YES")==0 || stricmp(ini_string,"ON")==0 || stricmp(ini_string,"TRUE")==0 || stricmp(ini_string,"1")==0)
      damp_gfx_sync_to_beats = TRUE;

   sprintf(ini_string,"%s",get_config_string("[options]","sync_threshold","5"));
   {
      current_threshold = atoi(ini_string);
      if(current_threshold<0) current_threshold=0;
      if(current_threshold>10) current_threshold=10;
   }

   sprintf(ini_string,"%s",get_config_string("[options]","change_zoom_method","yes"));
   if(stricmp(ini_string,"NO")==0 || stricmp(ini_string,"OFF")==0 || stricmp(ini_string,"FALSE")==0 || stricmp(ini_string,"0")==0)
      damp_gfx_randomize_modifier = FALSE;
   if(stricmp(ini_string,"YES")==0 || stricmp(ini_string,"ON")==0 || stricmp(ini_string,"TRUE")==0 || stricmp(ini_string,"1")==0)
      damp_gfx_randomize_modifier = TRUE;

   sprintf(ini_string,"%s",get_config_string("[options]","tracks_to_play","-1"));
   {
      damp_playlist_num_to_play = atoi(ini_string);
      if(strlen(ini_string)==0 || damp_playlist_num_to_play > damp_playlist_size || damp_playlist_num_to_play<0)
         damp_playlist_num_to_play = damp_playlist_size;
   }

   sprintf(ini_string,"%s",get_config_string("[options]","playback_quality","4"));
   {
      quality = atoi(ini_string);
   }

   sprintf(ini_string,"%s",get_config_string("[options]","display_menu","no"));
   if(stricmp(ini_string,"NO")==0 || stricmp(ini_string,"OFF")==0 || stricmp(ini_string,"FALSE")==0 || stricmp(ini_string,"0")==0)
      need_menu = FALSE;
   if(stricmp(ini_string,"YES")==0 || stricmp(ini_string,"ON")==0 || stricmp(ini_string,"TRUE")==0 || stricmp(ini_string,"1")==0)
      need_menu = TRUE;

   sprintf(ini_string,"%s",get_config_string("[options]","display_version","no"));
   if(stricmp(ini_string,"YES")==0 || stricmp(ini_string,"ON")==0 || stricmp(ini_string,"TRUE")==0 || stricmp(ini_string,"1")==0)
      printf("DAMP version %1.2f (%s - %s)\n\n",damp_version,__DATE__,__TIME__);

   sprintf(ini_string,"%s",get_config_string("[options]","continuous_play","no"));
   if(stricmp(ini_string,"NO")==0 || stricmp(ini_string,"OFF")==0 || stricmp(ini_string,"FALSE")==0 || stricmp(ini_string,"0")==0)
      damp_playlist_continuous = FALSE;
   if(stricmp(ini_string,"YES")==0 || stricmp(ini_string,"ON")==0 || stricmp(ini_string,"TRUE")==0 || stricmp(ini_string,"1")==0)
      damp_playlist_continuous = TRUE;

   sprintf(ini_string,"%s",get_config_string("[options]","surround_sound","no"));
   if(stricmp(ini_string,"NO")==0 || stricmp(ini_string,"OFF")==0 || stricmp(ini_string,"FALSE")==0 || stricmp(ini_string,"0")==0)
      audio_driver->surround = FALSE;
   if(stricmp(ini_string,"YES")==0 || stricmp(ini_string,"ON")==0 || stricmp(ini_string,"TRUE")==0 || stricmp(ini_string,"1")==0)
      audio_driver->surround = TRUE;

   sprintf(ini_string,"%s",get_config_string("[options]","show_time_in_gfx_mode","no"));
   if(stricmp(ini_string,"NO")==0 || stricmp(ini_string,"OFF")==0 || stricmp(ini_string,"FALSE")==0 || stricmp(ini_string,"0")==0)
      damp_gfx_show_time = FALSE;
   if(stricmp(ini_string,"YES")==0 || stricmp(ini_string,"ON")==0 || stricmp(ini_string,"TRUE")==0 || stricmp(ini_string,"1")==0)
      damp_gfx_show_time = TRUE;

   scopechar[0] = get_config_int("[options]","text_scope_char",254);
   scopechar[1] = 0;

   sprintf(ini_string,"%s",get_config_string("[options]","text_scope_smooth","yes"));
   if(stricmp(ini_string,"NO")==0 || stricmp(ini_string,"OFF")==0 || stricmp(ini_string,"FALSE")==0 || stricmp(ini_string,"0")==0)
      damp_scope_smooth = FALSE;
   if(stricmp(ini_string,"YES")==0 || stricmp(ini_string,"ON")==0 || stricmp(ini_string,"TRUE")==0 || stricmp(ini_string,"1")==0)
      damp_scope_smooth = TRUE;

   sprintf(ini_string,"%s",get_config_string("[options]","beautify_filenames","yes"));
   if(stricmp(ini_string,"NO")==0 || stricmp(ini_string,"OFF")==0 || stricmp(ini_string,"FALSE")==0 || stricmp(ini_string,"0")==0)
      damp_beautify_filenames = FALSE;
   if(stricmp(ini_string,"YES")==0 || stricmp(ini_string,"ON")==0 || stricmp(ini_string,"TRUE")==0 || stricmp(ini_string,"1")==0)
      damp_beautify_filenames = TRUE;

   sprintf(ini_string,"%s",get_config_string("[options]","show_file_info","no"));
   if(stricmp(ini_string,"NO")==0 || stricmp(ini_string,"OFF")==0 || stricmp(ini_string,"FALSE")==0 || stricmp(ini_string,"0")==0)
      damp_show_file_info = FALSE;
   if(stricmp(ini_string,"YES")==0 || stricmp(ini_string,"ON")==0 || stricmp(ini_string,"TRUE")==0 || stricmp(ini_string,"1")==0)
      damp_show_file_info = TRUE;

   sprintf(ini_string,"%s",get_config_string("[options]","use_gui","no"));
   if(stricmp(ini_string,"NO")==0 || stricmp(ini_string,"OFF")==0 || stricmp(ini_string,"FALSE")==0 || stricmp(ini_string,"0")==0)
      damp_using_gui = FALSE;
   if(stricmp(ini_string,"YES")==0 || stricmp(ini_string,"ON")==0 || stricmp(ini_string,"TRUE")==0 || stricmp(ini_string,"1")==0)
      need_gui = TRUE;

   sprintf(ini_string,"%s",get_config_string("[options]","posterize_graphics","no"));
   if(stricmp(ini_string,"NO")==0 || stricmp(ini_string,"OFF")==0 || stricmp(ini_string,"FALSE")==0 || stricmp(ini_string,"0")==0)
      damp_gfx_posterize = FALSE;
   if(stricmp(ini_string,"YES")==0 || stricmp(ini_string,"ON")==0 || stricmp(ini_string,"TRUE")==0 || stricmp(ini_string,"1")==0)
      damp_gfx_posterize = TRUE;

   damp_keypad_timeout = get_config_int("[options]","keypad_timeout",2);

   damp_ffwd_rew_speed = get_config_int("[options]","ffwd_rew_speed",1);
   damp_vol_up_down_speed = get_config_int("[options]","vol_up_down_speed",1);

   sprintf(ini_string,"%s",get_config_string("[options]","start_paused","no"));
//   if(stricmp(ini_string,"NO")==0 || stricmp(ini_string,"OFF")==0 || stricmp(ini_string,"FALSE")==0 || stricmp(ini_string,"0")==0)
   if(stricmp(ini_string,"YES")==0 || stricmp(ini_string,"ON")==0 || stricmp(ini_string,"TRUE")==0 || stricmp(ini_string,"1")==0)
      util_pause();

   sprintf(ini_string,"%s",get_config_string("[options]","yield_cpu","no"));
   if(stricmp(ini_string,"YES")==0 || stricmp(ini_string,"ON")==0 || stricmp(ini_string,"TRUE")==0 || stricmp(ini_string,"1")==0)
      damp_yield = TRUE;

   util_large_skip_amount = get_config_int("[options]","large_skip",10);


   damp_gfx_get_functionality();

   if(need_gui)
   {
      damp_using_gui = TRUE;
   }

   return need_menu;
}

//==========================================================================
// damp_init()
//
// This just does the basic initialisation.
//==========================================================================

void damp_init(void)
{
   set_config_file(damp_ini_file);

//   if(!damp_using_cd)
//   {
      reserve_voices(2,-1);
      /* Quit if we can't set up the sound driver */
      if (install_sound(DIGI_AUTODETECT,MIDI_NONE,NULL))
      {
         printf("Unable to install Allegro sound driver\n%s\n",allegro_error);
         exit(-1);
      }

   damp_volume=get_config_int("[sound]","digi_volume",damp_volume);
   
      voice_set_volume(0,damp_volume);
      voice_set_volume(1,damp_volume);
   
      if(!audio_init(get_config_int("[audio_engine]","engine",0)))
      {
         printf("Unable to initialise MP3 playback routines.\n");
         exit(-1);
      }
//   }

   keys_init();

   //i_love_bill=TRUE;
   LOCK_FUNCTION(set_timeout_flag);
   LOCK_VARIABLE(timeout_flag);
   install_timer();

   srand(time(NULL));

   sprintf(damp_playlist_filename,"NONE");
   sprintf(damp_playlist_filename_short,"NONE");

   damp_ffwd_rew_speed = 1;  /* Fast-forward/rewind speed */
   damp_vol_up_down_speed = 1;  /* Volume change speed */
   using_graphics = FALSE;   /* Using the graphics subsystem? */
   using_playlist = FALSE;   /* Using the playlist subsystem? */
   track_sel_pos=0;         /* for indexing into track_sel */
   damp_keypad_timeout=2;       /* Seconds before keypad timeout */
   track_sel_shift_flag = FALSE;   /* for selecting playlists */
   damp_using_gui = FALSE;   /* Using the gui? */
   track_paused = FALSE;
   damp_show_file_info=FALSE;
   selecting_track_flag = FALSE;
   damp_developer=FALSE;
   damp_use_conio_input=FALSE;
   damp_gfx_posterize=FALSE;
   need_to_quit = FALSE;
   need_to_quit_program=FALSE;
   timeout_flag = FALSE;
   damp_fnfnoquit = FALSE;
}


