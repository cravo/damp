/* script.c

   Common scripting things for DAMP (for dampcore.sh)
   By Matt Craven
*/

/*======= i n c l u d e s =========================================*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <conio.h>
#include <bios.h>
#include <seer.h>

/*======= d a m p   i n c l u d e s ===============================*/

#include "damp.h"
#include "audio.h"
#include "script.h"
#include "input.h"

/*========================================================================
  lcd_poll()

  Calls poll_amp() and input_handler(), since rest_callback() requires
  a single void function.
  ======================================================================*/

void lcd_poll(void)
{
   if(!track_paused) audio_driver->poll();
   input_handler(1);
}

/*========================================================================
  lcd_rest(int dtime)

  Rests for the given amount of milliseconds, calling lcd_poll() whilst
  doing so.
  ======================================================================*/

void lcd_rest(int dtime)
{
   rest_callback(dtime,lcd_poll);
}

/*===================================================================
  script_init()

  Inits all the shared stuff (for dampcore.sh) that drivers can use.
  =================================================================*/

void script_init(void)
{
   static int script_virgin = 1;    /* Has this function already been called? */

   if(script_virgin)
   {
      script_virgin = 0;

      /* Initialise SeeR */

      scInit_SeeR();

      /* export common functions for dampcore.sh */

      scAdd_External_Symbol("printf", printf);
      scAdd_External_Symbol("sprintf", sprintf);
      scAdd_External_Symbol("strcmp", strcmp);
      scAdd_External_Symbol("strlen", strlen);
      scAdd_External_Symbol("gotoxy", gotoxy);
      scAdd_External_Symbol("wherey", wherey);
      scAdd_External_Symbol("inportb", inportb);
      scAdd_External_Symbol("outportb", outportb);
      scAdd_External_Symbol("delay", lcd_rest);
      scAdd_External_Symbol("bioscom", bioscom);

      /* and common variables */

      scAdd_External_Symbol("damp_developer", &damp_developer);
      scAdd_External_Symbol("damp_shared_int", &damp_shared_int);
      scAdd_External_Symbol("damp_shared_char", damp_shared_char);
      scAdd_External_Symbol("damp_shared_long", &damp_shared_long);
      scAdd_External_Symbol("damp_shared_float", &damp_shared_float);
   }
}
