/* gtext.c

   Graphics mode "fake text mode" routines for DAMP
   By Matt Craven

   (c)2000 Hedgehog Software
*/

/*======= i n c l u d e s =========================================*/

#include <stdio.h>
#include <allegro.h>

/*======= d a m p   i n c l u d e s ===============================*/

#include "audio.h"
#include "gtext.h"
#include "graphics.h"
#include "damp.h"
#include "playlist.h"

/*===================================================================
  gtext_render()

  Renders the GText display
  =================================================================*/

void gtext_render(void)
{
   char tempstr[64];          /* temp variable */
   int n;                     /* loop counter */


   /* clear the buffer */
   clear(damp_gfx_buffer);


   /* display volume */
   textout(damp_gfx_buffer,font,"Volume:",0,30,makecol(255,255,0));
   sprintf(tempstr,"%d",damp_volume);
   textout(damp_gfx_buffer,font,tempstr,text_length(font,"Volume:"),30,makecol(128,128,128));

   /* display track time */
   textout(damp_gfx_buffer,font,"Time:",160,30,makecol(255,255,0));
   sprintf(tempstr,"%s",damp_display_time_remaining ? timeremainstr : timestr);
   textout(damp_gfx_buffer,font,tempstr,160+text_length(font,"Time:"),30,makecol(128,128,128));

   /* display status */
   textout(damp_gfx_buffer,font,"================ STATUS ================", 0, 50, makecol(255,128,64));
   textout(damp_gfx_buffer,font,"Surround",30,60,audio_driver->surround ? makecol(255,255,0) : makecol(64,64,64));
   textout(damp_gfx_buffer,font,"Continuous",30+text_length(font,"Surround   "),60,damp_playlist_continuous ? makecol(255,255,0) : makecol(64,64,64));
   textout(damp_gfx_buffer,font,"Random",30+text_length(font,"Surround   Continuous   "),60,damp_playlist_random ? makecol(255,255,0) : makecol(64,64,64));

   /* display scrolling track name */
   textout(damp_gfx_buffer,font,"============== NOW PLAYING =============", 0, 80, makecol(255,128,64));
   textout(damp_gfx_buffer,font,&damp_gfx_scroll_name[damp_gfx_scroll_name_pos],0,90,makecol(255,255,255));
   if(track_sel_pos > 0)
      textout_centre(damp_gfx_buffer,font,track_sel,160,100,makecol(255,255,255));

   /* display vu */
   if(damp_want_scope)
   {
      textout(damp_gfx_buffer,font,"VU-meter:",0,110,makecol(255,255,0));
      for(n=0;n<32;n++)
         textout(damp_gfx_buffer,font,n==gtext_vu_max/2 ? "I" : (n<damp_vu/2 ? "=" : "-"),n*10,120,(n < damp_vu/2) ? (n<16 ? makecol(0,255,0) : (n<24 ? makecol(255,255,0) : makecol(255,0,0))) : makecol(128,128,128));
   }

   /* display "advert" :-) */
   textout_centre(damp_gfx_buffer,font,"DAMP",160,140,makecol(255,0,0));
   textout_centre(damp_gfx_buffer,font,"Copyright 1999/2000 Hedgehog Software",160,150,makecol(255,0,0));
   textout_centre(damp_gfx_buffer,font,"http://www.damp-mp3.co.uk/",160,160,makecol(255,0,0));

   /* display it all */
   if(!damp_using_gui)
      blit(damp_gfx_buffer,screen,0,0,0,0,DAMP_GFX_SCREEN_WIDTH,DAMP_GFX_SCREEN_HEIGHT);
}
