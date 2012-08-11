/* sleep.c

   Sleep-timer code for DAMP.
   By Matt Craven

   (c)2000 Hedgehog Software
*/

/*======= i n c l u d e s =========================================*/

#include <stdio.h>
#include <string.h>
#include <allegro.h>

/*======= d a m p   i n c l u d e s ===============================*/

#include "sleep.h"
#include "damp.h"
#include "util.h"

/*======= g l o b a l   v a r i a b l e s =========================*/

int sleep_want_fade = FALSE;     /* Want a fade-out? */

/*===================================================================
  sleep_fadeout()

  The fade function - it's a timer callback
  =================================================================*/

void sleep_fadeout(void)
{
   if(damp_volume > 0)
      util_volume_down();
   else
      need_to_quit_program = TRUE;
}
END_OF_FUNCTION(sleep_fadeout);

/*===================================================================
  sleep_timeout()

  Another timer function.  It either quits DAMP, or starts the
  fade-out timer, depending on whether we wanted fading or not
  =================================================================*/

void sleep_timeout(void)
{
   remove_int(sleep_timeout);
   if(sleep_want_fade)
      install_int_ex(sleep_fadeout, BPS_TO_TIMER(2));
   else
      need_to_quit_program = TRUE;
}
END_OF_FUNCTION(sleep_timeout);

/*===================================================================
  sleep_init(mins)

  Initialises the sleep timer, ready to quit/fade after mins minutes
  =================================================================*/

void sleep_init(int mins)
{
   char txt[256];

   LOCK_FUNCTION(sleep_timeout);
   LOCK_FUNCTION(sleep_fadeout);

   /* See if fading is required */

   sprintf(txt,get_config_string("[options]","sleep_fade","yes"));

   if(stricmp(txt,"YES") == 0)
      sleep_want_fade = TRUE;

   install_int_ex(sleep_timeout, SECS_TO_TIMER(mins*60));
}
