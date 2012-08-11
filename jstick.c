/* jstick.c

   Reads in the joystick functionality from the ini file
   By Matt Craven
*/

/*======= i n c l u d e s =========================================*/

#include <stdio.h>
#include <string.h>
#include <allegro.h>

/*======= d a m p   i n c l u d e s ===============================*/

#include "damp.h"
#include "jstick.h"

/*======= g l o b a l   v a r i a b l e s =========================*/

JOY_FUNCTION joy_function;    /* Somewhere to store the functionality */

int joy_zero = 0;             /* A special "zero" value */
int joy_minus_one = -1;       /* A special "-1" value */

int jstick_analogue = FALSE;  /* Is the joystick analogue? (not really used) */

int jstick_volume_fine = TRUE;   /* Want fine volume control? */


/*===================================================================
  jstick_assign_function(action, type)

  Assigns the given type of functionality to the approppriate action,
  and stores it in the joy_function datatype
  =================================================================*/

void jstick_assign_function(int *joy_action, char *type)
{
   if (stricmp(type,"PREVIOUS_TRACK")==0)
      joy_function.previous_track = joy_action;
   else if (stricmp(type,"NEXT_TRACK")==0)
      joy_function.next_track = joy_action;
   else if (stricmp(type,"NEXT_N_TRACKS")==0)
      joy_function.next_n_tracks = joy_action;
   else if (stricmp(type,"PREVIOUS_N_TRACKS")==0)
      joy_function.previous_n_tracks = joy_action;
   else if (stricmp(type,"VOLUME_UP")==0)
      joy_function.volume_up = joy_action;
   else if (stricmp(type,"VOLUME_DOWN")==0)
      joy_function.volume_down = joy_action;
   else if (stricmp(type,"PAUSE")==0)
      joy_function.pause = joy_action;
   else if (stricmp(type,"SURROUND_SOUND")==0)
      joy_function.surround_sound = joy_action;
   else if (stricmp(type,"RANDOM")==0)
      joy_function.random = joy_action;
   else if (stricmp(type,"PREVIOUS_PLAYLIST")==0)
      joy_function.previous_playlist = joy_action;
   else if (stricmp(type,"NEXT_PLAYLIST")==0)
      joy_function.next_playlist = joy_action;
   else if (stricmp(type,"REWIND")==0)
      joy_function.rewind = joy_action;
   else if (stricmp(type,"FAST_FORWARD")==0)
      joy_function.fast_forward = joy_action;
   else if (stricmp(type,"QUIT")==0)
      joy_function.quit = joy_action;
   else if (stricmp(type,"EXIT_CODE_1")==0)
      joy_function.exit_code_1 = joy_action;
   else if (stricmp(type,"EXIT_CODE_2")==0)
      joy_function.exit_code_2 = joy_action;
   else if (stricmp(type,"EXIT_CODE_3")==0)
      joy_function.exit_code_3 = joy_action;
   else if (stricmp(type,"EXIT_CODE_4")==0)
      joy_function.exit_code_4 = joy_action;
   else if (stricmp(type,"EXIT_CODE_5")==0)
      joy_function.exit_code_5 = joy_action;
   else if (stricmp(type,"EXIT_CODE_6")==0)
      joy_function.exit_code_6 = joy_action;
   else if (stricmp(type,"EXIT_CODE_7")==0)
      joy_function.exit_code_7 = joy_action;
   else if (stricmp(type,"EXIT_CODE_8")==0)
      joy_function.exit_code_8 = joy_action;
   else if (stricmp(type,"EXIT_CODE_9")==0)
      joy_function.exit_code_9 = joy_action;
}

/*===================================================================
  jstick_get_functionality()

  This functions reads the required functionality from the ini file,
  and initialises the joystick system
  =================================================================*/

void jstick_get_functionality(void)
{
   char ini_string[256];
   int i;

   /* Install joystick, giving appropriate messages for success or
      failure */

   if(install_joystick(JOY_TYPE_AUTODETECT))
   {
      printf("Warning: No joystick found.\n");
      printf("         Joystick functionality will be disabled.\n");
   } else if(damp_developer)
   {
      printf("Joystick detected as %s\n",joystick_driver->name);
      printf("  (%s)\n",joystick_driver->desc);
   }

   //Do we need to calibrate?
//   if(load_joystick_data(NULL))
//   {
      //Yes, calibrate
      for(i=0; i<num_joysticks; i++)
      {
         while(joy[i].flags & JOYFLAG_CALIBRATE)
         {
            char *msg = calibrate_joystick_name(i);
            printf("%s, and press button 1\n",msg);
            do{poll_joystick();}while(!joy[i].button[0].b);
            do{poll_joystick();}while(joy[i].button[0].b);
            if(calibrate_joystick(i) != 0)
            {
               printf("Error calibrating joystick!\n");
               exit(-1);
            }
         }
      }
      save_joystick_data(NULL);
//   }

   /* Are we using an analogue stick? */
   if(jstick_analogue)
   {
     /* not implemented */
   }

   /* First set everything to JOY_ZERO */

   joy_function.previous_track = &joy_zero;
   joy_function.next_track = &joy_zero;
   joy_function.previous_n_tracks = &joy_zero;
   joy_function.next_n_tracks = &joy_zero;
   joy_function.volume_up = &joy_zero;
   joy_function.volume_down = &joy_zero;
   joy_function.pause = &joy_zero;
   joy_function.surround_sound = &joy_zero;
   joy_function.random = &joy_zero;
   joy_function.previous_playlist = &joy_zero;
   joy_function.next_playlist = &joy_zero;
   joy_function.rewind = &joy_zero;
   joy_function.fast_forward = &joy_zero;
   joy_function.quit = &joy_zero;
   joy_function.exit_code_1 = &joy_zero;
   joy_function.exit_code_2 = &joy_zero;
   joy_function.exit_code_3 = &joy_zero;
   joy_function.exit_code_4 = &joy_zero;
   joy_function.exit_code_5 = &joy_zero;
   joy_function.exit_code_6 = &joy_zero;
   joy_function.exit_code_7 = &joy_zero;
   joy_function.exit_code_8 = &joy_zero;
   joy_function.exit_code_9 = &joy_zero;

   /* Read functionality from INI file */

   sprintf(ini_string,"%s",get_config_string("[joystick_control]","volume_fine","yes"));
   if(stricmp(ini_string, "YES") == 0)
      jstick_volume_fine = TRUE;
   else
      jstick_volume_fine = FALSE;

   /* Assign the functions as appropriate */

   sprintf(ini_string,"%s",get_config_string("[joystick_control]","up","NONE"));
   jstick_assign_function(&joy_up,ini_string);
   sprintf(ini_string,"%s",get_config_string("[joystick_control]","down","NONE"));
   jstick_assign_function(&joy_down,ini_string);
   sprintf(ini_string,"%s",get_config_string("[joystick_control]","left","NONE"));
   jstick_assign_function(&joy_left,ini_string);
   sprintf(ini_string,"%s",get_config_string("[joystick_control]","right","NONE"));
   jstick_assign_function(&joy_right,ini_string);
   sprintf(ini_string,"%s",get_config_string("[joystick_control]","fire1","NONE"));
   jstick_assign_function(&joy_b1,ini_string);
   sprintf(ini_string,"%s",get_config_string("[joystick_control]","fire2","NONE"));
   jstick_assign_function(&joy_b2,ini_string);
   sprintf(ini_string,"%s",get_config_string("[joystick_control]","fire3","NONE"));
   jstick_assign_function(&joy_b3,ini_string);
   sprintf(ini_string,"%s",get_config_string("[joystick_control]","fire4","NONE"));
   jstick_assign_function(&joy_b4,ini_string);
   sprintf(ini_string,"%s",get_config_string("[joystick_control]","fire5","NONE"));
   jstick_assign_function(&joy_b5,ini_string);
   sprintf(ini_string,"%s",get_config_string("[joystick_control]","fire6","NONE"));
   jstick_assign_function(&joy_b6,ini_string);
   sprintf(ini_string,"%s",get_config_string("[joystick_control]","fire7","NONE"));
   jstick_assign_function(&joy_b7,ini_string);
   sprintf(ini_string,"%s",get_config_string("[joystick_control]","fire8","NONE"));
   jstick_assign_function(&joy_b8,ini_string);
   sprintf(ini_string,"%s",get_config_string("[joystick_control]","up2","NONE"));
   jstick_assign_function(&joy2_up,ini_string);
   sprintf(ini_string,"%s",get_config_string("[joystick_control]","down2","NONE"));
   jstick_assign_function(&joy2_down,ini_string);
   sprintf(ini_string,"%s",get_config_string("[joystick_control]","left2","NONE"));
   jstick_assign_function(&joy2_left,ini_string);
   sprintf(ini_string,"%s",get_config_string("[joystick_control]","right2","NONE"));
   jstick_assign_function(&joy2_right,ini_string);
   sprintf(ini_string,"%s",get_config_string("[joystick_control]","fire21","NONE"));
   jstick_assign_function(&joy2_b1,ini_string);
   sprintf(ini_string,"%s",get_config_string("[joystick_control]","fire22","NONE"));
   jstick_assign_function(&joy2_b2,ini_string);
}
