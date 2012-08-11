/* lcd.c

   LCD driver code for DAMP
   By Matt Craven
*/

/*======= i n c l u d e s =========================================*/

#include <string.h>
#include <stdio.h>
#include <pc.h>
#include <bios.h>
#include <allegro.h>
#include <seer.h>

/*======= d a m p   i n c l u d e s ===============================*/

#include "damp.h"
#include "audio.h"
#include "lcd.h"
#include "playlist.h"
#include "script.h"
#include "id3read.h"

/*======= g l o b a l   v a r i a b l e s =========================*/

int lcd_in_use = FALSE;                      /* Using the LCD part of damp? */
unsigned char lcd_line[2][41];               /* LCD lines */
unsigned char lcd_status[10];                /* LCD status string */
volatile int lcd_scroll_pos;                 /* Current scroll position */
volatile int lcd_require_update;             /* Need to update the LCD? */
unsigned char lcd_scroll_text[1024];         /* The scroll text */
int lcd_scroll_speed = 4;                    /* The speed of scrolling */
scScript lcd_driver_script;                  /* The driver script */
scInstance *lcd_driver_instance;             /* The driver instance */

char lcd_surround_symbol;                    /* LCD characters... */
char lcd_play_symbol;
char lcd_random_symbol;
char lcd_rew_symbol;
char lcd_continuous_symbol = 'C';

char lcd_driver_name[256];                   /* LCD variables, set by the driver */
char lcd_driver_author[256];
char lcd_driver_author_email[256];
char lcd_driver_description[256];
float lcd_driver_version;

char damp_id3_genre[64];                     /* Genre string */

int lcd_display_width;                       /* Width of display */
int lcd_display_lines;                       /* Number of lines */

/*========================================================================
  lcd_scroll()

  The lcd scrolling interrupt - it scrolls the track title text
  ======================================================================*/

void lcd_scroll(void)
{
   lcd_require_update = TRUE;

   lcd_scroll_pos++;
   if(lcd_scroll_pos > strlen(lcd_scroll_text))
      lcd_scroll_pos = 0;
}
END_OF_FUNCTION(lcd_scroll);



/*========================================================================
  lcd_external_function(char *func)

  Calls an external function, defined in the LCD driver.
  ======================================================================*/

void lcd_external_function(char *func)
{
   scCall_Instance(lcd_driver_instance,scGet_Symbol(lcd_driver_instance,func));
}

/*========================================================================
  lcd_load_driver(char *drivername)

  Loads the driver for the LCD display.
  ======================================================================*/

void lcd_load_driver(char *drivername)
{
   /* Initialise the shared stuff (for dampcore.sh) */

   script_init();

   /* Add the LCD-specific stuff for the driver */

   scAdd_External_Symbol("lcd_surround_symbol", &lcd_surround_symbol);
   scAdd_External_Symbol("lcd_play_symbol", &lcd_play_symbol);
   scAdd_External_Symbol("lcd_random_symbol", &lcd_random_symbol);
   scAdd_External_Symbol("lcd_rew_symbol", &lcd_rew_symbol);
   scAdd_External_Symbol("lcd_continuous_symbol", &lcd_continuous_symbol);

   scAdd_External_Symbol("lcd_driver_name", lcd_driver_name);
   scAdd_External_Symbol("lcd_driver_author", lcd_driver_author);
   scAdd_External_Symbol("lcd_driver_author_email", lcd_driver_author_email);
   scAdd_External_Symbol("lcd_driver_description", lcd_driver_description);
   scAdd_External_Symbol("lcd_driver_version", &lcd_driver_version);

   scAdd_External_Symbol("lcd_display_width", &lcd_display_width);
   scAdd_External_Symbol("lcd_display_lines", &lcd_display_lines);

   scAdd_External_Symbol("lcd_line", lcd_line);

   scAdd_External_Symbol("damp_vu", &damp_vu);
   scAdd_External_Symbol("damp_random_play", &damp_playlist_random);
   scAdd_External_Symbol("damp_surround", &audio_driver->surround);
   scAdd_External_Symbol("damp_paused", &track_paused);
   scAdd_External_Symbol("damp_volume", &damp_volume);
   scAdd_External_Symbol("damp_status", lcd_status);
   scAdd_External_Symbol("damp_filename", short_filename);
   scAdd_External_Symbol("damp_playlist_filename", damp_playlist_filename);
   scAdd_External_Symbol("damp_playlist_filename_short", damp_playlist_filename_short);
   scAdd_External_Symbol("damp_id3_title", damp_id3_tag.title);
   scAdd_External_Symbol("damp_id3_artist", damp_id3_tag.artist);
   scAdd_External_Symbol("damp_id3_album", damp_id3_tag.album);
   scAdd_External_Symbol("damp_id3_year", damp_id3_tag.year);
   scAdd_External_Symbol("damp_id3_comment", damp_id3_tag.comment);
   scAdd_External_Symbol("damp_id3_genre", damp_id3_genre);
   scAdd_External_Symbol("damp_time", timestr);
   scAdd_External_Symbol("damp_time_remaining", timeremainstr);
   scAdd_External_Symbol("damp_track_number", &damp_track_number);
   scAdd_External_Symbol("damp_selection_buffer", track_sel);
   scAdd_External_Symbol("damp_state", &damp_state);

   /* Compile the driver */

   lcd_driver_script = scCompile_File(drivername);

   /* If that failed, give an error */

   if(lcd_driver_script == NULL)
   {
      printf("Failed to load LCD driver: %s\n",drivername);
      printf("%s\n",scErrorMsg);
      exit(-1);
   }

   /* Create an instance of the driver */

   lcd_driver_instance = scCreate_Instance(lcd_driver_script,NULL);

   /* If that failed, give an error */

   if(!lcd_driver_instance)
   {
      printf("Failed to instance LCD driver: %s\n",drivername);
      printf("%s\n",scErrorMsg);
      exit(-1);
   }

}


/*========================================================================
  lcd_init()

  Initialises the LCD display driver system
  ======================================================================*/

void lcd_init(void)
{
   set_config_file(damp_ini_file);

   /* Load the driver */

   lcd_load_driver(get_config_string("[lcd]","driver","NONE"));

   /* Call the driver's init function */

   lcd_external_function("lcd_driver_init");

   /* Read LCD variables from the ini file */

   lcd_scroll_speed = get_config_int("[lcd]","scroll_speed",4);
   lcd_display_width = get_config_int("[lcd]","display_width",16);
   lcd_display_lines = get_config_int("[lcd]","display_lines",2);
   lcd_require_update = FALSE;

   /* Set up the scroller */

   lcd_scroll_pos = 0;
   sprintf(lcd_scroll_text,"%*c\n",lcd_display_width,' ');

   LOCK_FUNCTION(lcd_scroll);
   LOCK_VARIABLE(lcd_scroll_pos);
   LOCK_VARIABLE(lcd_require_update);
   install_int_ex(lcd_scroll,BPS_TO_TIMER(lcd_scroll_speed));

   /* Display driver details */

   printf("======================================================================\n");
   printf("DAMP LCD Driver\n");
   printf("======================================================================\n");
   printf("Driver name        : %s\n",lcd_driver_name);
   printf("Driver version     : %.2f\n",lcd_driver_version);
   printf("Author name        : %s\n",lcd_driver_author);
   printf("Author email       : %s\n",lcd_driver_author_email);
   printf("Driver description : %s\n",lcd_driver_description);
   printf("\n\n");

}

/*=========================================================================
  lcd_deinit()

  Shuts down the LCD display driver
  =======================================================================*/

void lcd_deinit(void)
{
   remove_int(lcd_scroll);
   lcd_external_function("lcd_clear");

   scFree_Instance(lcd_driver_instance);
   free(lcd_driver_script);
}


/*=======================================================================
  lcd_display()

  Builds and shows the stuff on the LCD
  =====================================================================*/

void lcd_display(void)
{
   if(lcd_require_update)
   {
      /* Only if an update is required */

      int n;
      int lcd_sp = lcd_scroll_pos;
   
      /* put track name in first chars */
   
      for(n=0;n<lcd_display_width-3;n++)
      {
         if(n+lcd_sp<strlen(lcd_scroll_text))
         {
            if(lcd_scroll_text[n+lcd_sp] > 32)
               lcd_line[0][n]=lcd_scroll_text[n+lcd_sp];
            else
               lcd_line[0][n]=' ';
          }
         else
            lcd_line[0][n]=' ';
      }
   
      /* display continuous, random and surround indicators */
      if(damp_playlist_continuous)
         lcd_line[0][lcd_display_width-3] = lcd_continuous_symbol;
      else
         lcd_line[0][lcd_display_width-3] = ' ';
      if(damp_playlist_random)
         lcd_line[0][lcd_display_width-2] = lcd_random_symbol;
      else
         lcd_line[0][lcd_display_width-2] = ' ';
      if(audio_driver->surround)
         lcd_line[0][lcd_display_width-1] = lcd_surround_symbol;
      else
         lcd_line[0][lcd_display_width-1] = ' ';
   
      /* display track time */
      for(n=0;n<5;n++)
         lcd_line[1][n] = damp_display_time_remaining? timeremainstr[n] : timestr[n];
   
      lcd_line[1][5]=' ';
   
      /* display status */
      for(n=0;n<6;n++)
         lcd_line[1][n+6] = lcd_status[n];
   
      for(n=12;n<lcd_display_width;n++)
         lcd_line[1][n] = ' ';
   
      if(track_sel_pos < 1)
      {
         /* display "damp" */
         lcd_line[1][lcd_display_width-4]='d';
         lcd_line[1][lcd_display_width-3]='a';
         lcd_line[1][lcd_display_width-2]='m';
         lcd_line[1][lcd_display_width-1]='p';
      } else
      {
         /* display track selection */
         for(n=0;n<track_sel_pos;n++)
         {
            lcd_line[1][lcd_display_width-track_sel_pos+n] = track_sel[n];
         }
      }
   
      lcd_line[0][lcd_display_width] = 0;
      lcd_line[1][lcd_display_width] = 0;
   
//      lcd_external_function("lcd_driver_clear");
      lcd_external_function("lcd_driver_home");

      lcd_external_function("lcd_driver_printf");
   
      lcd_require_update = FALSE;
   }
}
