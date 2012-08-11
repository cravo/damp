/* indrv.c

   Input driver routines for DAMP
   By Matt Craven
*/

/*======= i n c l u d e s =========================================*/

#include <stdio.h>
#include <conio.h>
#include <allegro.h>
#include <seer.h>

/*======= d a m p   i n c l u d e s ===============================*/

#include "damp.h"
#include "indrv.h"
#include "script.h"
#include "util.h"

/*======= g l o b a l   v a r i a b l e s =========================*/

int indrv_in_use = FALSE;              /* Using input driver? */
int indrv_keyboard_removed = FALSE;    /* low-level keyboard removed? */

/*===================================================================
  indrv_external_function(func)

  Calls a function in the driver file
  =================================================================*/

void indrv_external_function(char *func)
{
   scCall_Instance(indrv_driver_instance,scGet_Symbol(indrv_driver_instance,func));
}

/*===================================================================
  indrv_init()

  Initialises the input driver system, and loads the driver specified
  in the ini file
  =================================================================*/

void indrv_init(void)
{
   set_config_file(damp_ini_file);

   /* Load the driver */

   indrv_load_driver(get_config_string("[input]","driver","NONE"));

   /* Call the driver's init function */

   indrv_external_function("input_driver_init");

   /* Display the driver details */

   printf("======================================================================\n");
   printf("DAMP INPUT Driver\n");
   printf("======================================================================\n");
   printf("Driver name        : %s\n",indrv_driver_name);
   printf("Driver version     : %.2f\n",indrv_driver_version);
   printf("Author name        : %s\n",indrv_driver_author);
   printf("Author email       : %s\n",indrv_driver_author_email);
   printf("Driver description : %s\n",indrv_driver_description);
   printf("\n\n");
}

/*===================================================================
  indrv_poll()

  This simply calls the input driver's poll function, and returns
  the return value that it sets.
  =================================================================*/

int indrv_poll(void)
{
   indrv_external_function("input_driver_poll");

   return indrv_return_value;
}

/*===================================================================
  indrv_remove_keyboard()

  This is for drivers to call, to remove the low-level Allegro
  keyboard handler.
  =================================================================*/

void indrv_remove_keyboard(void)
{
   indrv_keyboard_removed = TRUE;
   remove_keyboard();
}

/*===================================================================
  indrv_load_driver(drivername)

  Loads the given driver, and sets up all the functions and variables
  for it to use.
  =================================================================*/

void indrv_load_driver(char *drivername)
{
   /* Initialise shared stuff (for dampcore.sh) */

   script_init();

   /* Initialise input-driver specific stuff */

   scAdd_External_Symbol("input_driver_name", indrv_driver_name);
   scAdd_External_Symbol("input_driver_author", indrv_driver_author);
   scAdd_External_Symbol("input_driver_author_email", indrv_driver_author_email);
   scAdd_External_Symbol("input_driver_description", indrv_driver_description);
   scAdd_External_Symbol("input_driver_version", &indrv_driver_version);

   scAdd_External_Symbol("input_driver_return_value", &indrv_return_value);

   scAdd_External_Symbol("kbhit", kbhit);
   scAdd_External_Symbol("getch", getch);
   scAdd_External_Symbol("remove_keyboard", indrv_remove_keyboard);

   scAdd_External_Symbol("util_quit", util_quit);
   scAdd_External_Symbol("util_screenshot", util_screenshot);
   scAdd_External_Symbol("util_previous_track",util_previous_track);
   scAdd_External_Symbol("util_next_track",util_next_track);
   scAdd_External_Symbol("util_random_toggle",util_random_toggle);
   scAdd_External_Symbol("util_scroll_track_title",util_scroll_track_title);
   scAdd_External_Symbol("util_surround_toggle", util_surround_toggle);
   scAdd_External_Symbol("util_volume_up", util_volume_up);
   scAdd_External_Symbol("util_volume_down", util_volume_down);
   scAdd_External_Symbol("util_graphics_toggle", util_graphics_toggle);
   scAdd_External_Symbol("util_pause", util_pause);
   scAdd_External_Symbol("util_pad", util_pad);
   scAdd_External_Symbol("util_previous_playlist", util_previous_playlist);
   scAdd_External_Symbol("util_next_playlist", util_next_playlist);
   scAdd_External_Symbol("util_rewind", util_rewind);
   scAdd_External_Symbol("util_fast_forward", util_fast_forward);
   scAdd_External_Symbol("util_continuous_toggle", util_continuous_toggle);
   scAdd_External_Symbol("util_sync_toggle", util_sync_toggle);
   scAdd_External_Symbol("util_time_remiain_toggle", util_time_remain_toggle);

   /* Compile the driver */

   indrv_driver_script = scCompile_File(drivername);

   /* If that failed, give an error message */

   if(indrv_driver_script == NULL)
   {
      printf("Failed to load INPUT driver: %s\n",drivername);
      printf("%s\n",scErrorMsg);
      exit(-1);
   }

   /* Create an instance of the driver */

   indrv_driver_instance = scCreate_Instance(indrv_driver_script,NULL);

   /* If that failed, display an error */

   if(!indrv_driver_instance)
   {
      printf("Failed to instance INPUT driver: %s\n",drivername);
      printf("%s\n",scErrorMsg);
      exit(-1);
   }
}

/*===================================================================
  indrv_deinit()

  Shuts down the input driver system
  =================================================================*/

void indrv_deinit(void)
{
   scFree_Instance(indrv_driver_instance);
   free(indrv_driver_script);
}
