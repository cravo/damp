/* graphics.c

   Audio visualisation code for DAMP
   By Matt Craven
   (c)2000 Hedgehog Software
*/

/*=========== i n c l u d e s =====================================*/

#include <stdio.h>
#include <string.h>
#include <math.h>
#include <allegro.h>

/*=========== d a m p   i n c l u d e s ===========================*/

#include "audio.h"
#include "graphics.h"
#include "damp.h"
#include "gtext.h"

/*=========== g l o b a l   v a r i a b l e s =====================*/

GFX_FILTER gfx_filter;                 /* current filter matrix */

BITMAP *damp_gfx_buffer;               /* back-buffer */
RGB damp_gfx_palette[256];             /* current colour palette */

int damp_gfx_wave_use[DAMP_GFX_WAVEFORMS];   /* arrays of which waveforms */
int damp_gfx_mod_use[DAMP_GFX_MODIFIERS];    /* and modifiers to use      */

int damp_gfx_min_threshold = 64;       /* minimum - to stop flashing */
int damp_gfx_waveform = 0;             /* current waveform */
int damp_gfx_modifier = 0;             /* current modifier */
int damp_gfx_sync_to_beats = FALSE;    /* want sync to beats? */
int damp_gfx_randomize_modifier = TRUE;/* want modifier randomization? */
int damp_gfx_bnw_palette = FALSE;      /* Use black+white palette? */
int damp_gfx_show_name = FALSE;        /* Show track name? */
char damp_gfx_scroll_name[340];        /* For scrolling the track name */
volatile int damp_gfx_scroll_name_pos = 0;      /* position in scrolling */
volatile int damp_gfx_scroll_y = 0;    /* vertical pos of text */
volatile int damp_gfx_scroll_times = -1;        /* Number of times to scroll name (-1 = inf) */
char damp_gfx_driver[256];             /* Gfx driver name */
int damp_gfx_show_time = FALSE;        /* display time? */




/*===================================================================
  screenshot()

  Takes a screen-shot and saves it as "shotXXX.bmp" where XXX will be
  a number from 0 to 999.  It'll automatically work out what number to
  use, depending on what files already exist...
  =================================================================*/

void screenshot(void)
{
   int n=-1;
   char sfile[256];
   BITMAP *temp;

   if(screen != NULL)
   {
      do
      {
         n++;
         sprintf(sfile,"shot%03d.bmp", n);
      }while(exists(sfile));
   
      temp = create_sub_bitmap(screen,0,0,SCREEN_W,SCREEN_H);

      if(temp != NULL)
      {
         save_bitmap(sfile,temp,damp_gfx_palette);
         destroy_bitmap(temp);
      }
   }
}


/*===================================================================
  damp_gfx_get_functionality()

  Desc   : Processes the graphics options in DAMP.ini
===================================================================*/

void damp_gfx_get_functionality(void)
{
   char ini_string[256];
   char **temp_string;
   int argc;
   int n;

   sprintf(ini_string,"%s",get_config_string("[gfx_waveforms]","DOTS","yes"));
   if(stricmp(ini_string,"YES")==0)
      damp_gfx_wave_use[DAMP_GFX_WAVE_DOTS]=TRUE;
   else
      damp_gfx_wave_use[DAMP_GFX_WAVE_DOTS]=FALSE;

   sprintf(ini_string,"%s",get_config_string("[gfx_waveforms]","SOLID","yes"));
   if(stricmp(ini_string,"YES")==0)
      damp_gfx_wave_use[DAMP_GFX_WAVE_SOLID]=TRUE;
   else
      damp_gfx_wave_use[DAMP_GFX_WAVE_SOLID]=FALSE;

   sprintf(ini_string,"%s",get_config_string("[gfx_waveforms]","CIRCLE","yes"));
   if(stricmp(ini_string,"YES")==0)
      damp_gfx_wave_use[DAMP_GFX_WAVE_CIRCLE]=TRUE;
   else
      damp_gfx_wave_use[DAMP_GFX_WAVE_CIRCLE]=FALSE;

   sprintf(ini_string,"%s",get_config_string("[gfx_waveforms]","LASER","yes"));
   if(stricmp(ini_string,"YES")==0)
      damp_gfx_wave_use[DAMP_GFX_WAVE_LASER]=TRUE;
   else
      damp_gfx_wave_use[DAMP_GFX_WAVE_LASER]=FALSE;

   sprintf(ini_string,"%s",get_config_string("[gfx_waveforms]","DUALHORIZONTAL","yes"));
   if(stricmp(ini_string,"YES")==0)
      damp_gfx_wave_use[DAMP_GFX_WAVE_DUALHORIZONTAL]=TRUE;
   else
      damp_gfx_wave_use[DAMP_GFX_WAVE_DUALHORIZONTAL]=FALSE;

   sprintf(ini_string,"%s",get_config_string("[gfx_waveforms]","MULTICIRCLE","yes"));
   if(stricmp(ini_string,"YES")==0)
      damp_gfx_wave_use[DAMP_GFX_WAVE_MULTICIRCLE]=TRUE;
   else
      damp_gfx_wave_use[DAMP_GFX_WAVE_MULTICIRCLE]=FALSE;

   sprintf(ini_string,"%s",get_config_string("[gfx_waveforms]","MULTILASER","yes"));
   if(stricmp(ini_string,"YES")==0)
      damp_gfx_wave_use[DAMP_GFX_WAVE_MULTILASER]=TRUE;
   else
      damp_gfx_wave_use[DAMP_GFX_WAVE_MULTILASER]=FALSE;

   sprintf(ini_string,"%s",get_config_string("[gfx_waveforms]","CORNERS","yes"));
   if(stricmp(ini_string,"YES")==0)
      damp_gfx_wave_use[DAMP_GFX_WAVE_CORNERS]=TRUE;
   else
      damp_gfx_wave_use[DAMP_GFX_WAVE_CORNERS]=FALSE;

   sprintf(ini_string,"%s",get_config_string("[gfx_waveforms]","SPINLINES","yes"));
   if(stricmp(ini_string,"YES")==0)
      damp_gfx_wave_use[DAMP_GFX_WAVE_SPINLINES]=TRUE;
   else
      damp_gfx_wave_use[DAMP_GFX_WAVE_SPINLINES]=FALSE;

   sprintf(ini_string,"%s",get_config_string("[gfx_waveforms]","CONCENTRIC","yes"));
   if(stricmp(ini_string,"YES")==0)
      damp_gfx_wave_use[DAMP_GFX_WAVE_CONCENTRIC]=TRUE;
   else
      damp_gfx_wave_use[DAMP_GFX_WAVE_CONCENTRIC]=FALSE;

   sprintf(ini_string,"%s",get_config_string("[gfx_waveforms]","BLOBS","yes"));
   if(stricmp(ini_string,"YES")==0)
      damp_gfx_wave_use[DAMP_GFX_WAVE_BLOBS]=TRUE;
   else
      damp_gfx_wave_use[DAMP_GFX_WAVE_BLOBS]=FALSE;

   // now the modifiers

   sprintf(ini_string,"%s",get_config_string("[gfx_modifiers]","ZOOMIN","yes"));
   if(stricmp(ini_string,"YES")==0)
      damp_gfx_mod_use[DAMP_GFX_MOD_ZOOMIN]=TRUE;
   else
      damp_gfx_mod_use[DAMP_GFX_MOD_ZOOMIN]=FALSE;

   sprintf(ini_string,"%s",get_config_string("[gfx_modifiers]","FISHEYE","yes"));
   if(stricmp(ini_string,"YES")==0)
      damp_gfx_mod_use[DAMP_GFX_MOD_FISHEYE]=TRUE;
   else
      damp_gfx_mod_use[DAMP_GFX_MOD_FISHEYE]=FALSE;

   sprintf(ini_string,"%s",get_config_string("[gfx_modifiers]","ZOOMUP","yes"));
   if(stricmp(ini_string,"YES")==0)
      damp_gfx_mod_use[DAMP_GFX_MOD_ZOOMUP]=TRUE;
   else
      damp_gfx_mod_use[DAMP_GFX_MOD_ZOOMUP]=FALSE;

   sprintf(ini_string,"%s",get_config_string("[gfx_modifiers]","SCROLLUPDOWN","yes"));
   if(stricmp(ini_string,"YES")==0)
      damp_gfx_mod_use[DAMP_GFX_MOD_SCROLLUPDOWN]=TRUE;
   else
      damp_gfx_mod_use[DAMP_GFX_MOD_SCROLLUPDOWN]=FALSE;

   sprintf(ini_string,"%s",get_config_string("[gfx_modifiers]","ZOOMDOWN","yes"));
   if(stricmp(ini_string,"YES")==0)
      damp_gfx_mod_use[DAMP_GFX_MOD_ZOOMDOWN]=TRUE;
   else
      damp_gfx_mod_use[DAMP_GFX_MOD_ZOOMDOWN]=FALSE;

   sprintf(ini_string,"%s",get_config_string("[gfx_modifiers]","SCROLLRIGHTLEFT","yes"));
   if(stricmp(ini_string,"YES")==0)
      damp_gfx_mod_use[DAMP_GFX_MOD_SCROLLRIGHTLEFT]=TRUE;
   else
      damp_gfx_mod_use[DAMP_GFX_MOD_SCROLLRIGHTLEFT]=FALSE;

   sprintf(ini_string,"%s",get_config_string("[gfx_modifiers]","DOUBLEFISHEYE","yes"));
   if(stricmp(ini_string,"YES")==0)
      damp_gfx_mod_use[DAMP_GFX_MOD_DOUBLEFISHEYE]=TRUE;
   else
      damp_gfx_mod_use[DAMP_GFX_MOD_DOUBLEFISHEYE]=FALSE;

   sprintf(ini_string,"%s",get_config_string("[gfx_modifiers]","SINZOOM","yes"));
   if(stricmp(ini_string,"YES")==0)
      damp_gfx_mod_use[DAMP_GFX_MOD_SINZOOM]=TRUE;
   else
      damp_gfx_mod_use[DAMP_GFX_MOD_SINZOOM]=FALSE;

   sprintf(ini_string,"%s",get_config_string("[gfx_modifiers]","INVSINZOOM","yes"));
   if(stricmp(ini_string,"YES")==0)
      damp_gfx_mod_use[DAMP_GFX_MOD_INVSINZOOM]=TRUE;
   else
      damp_gfx_mod_use[DAMP_GFX_MOD_INVSINZOOM]=FALSE;

   sprintf(ini_string,"%s",get_config_string("[gfx_modifiers]","ZOOMOUT","yes"));
   if(stricmp(ini_string,"YES")==0)
      damp_gfx_mod_use[DAMP_GFX_MOD_ZOOMOUT]=TRUE;
   else
      damp_gfx_mod_use[DAMP_GFX_MOD_ZOOMOUT]=FALSE;

   sprintf(ini_string,"%s",get_config_string("[gfx_modifiers]","SPHERE","yes"));
   if(stricmp(ini_string,"YES")==0)
      damp_gfx_mod_use[DAMP_GFX_MOD_SPHERE]=TRUE;
   else
      damp_gfx_mod_use[DAMP_GFX_MOD_SPHERE]=FALSE;

   sprintf(ini_string,"%s",get_config_string("[gfx_modifiers]","ROTATE_LEFT","yes"));
   if(stricmp(ini_string,"YES")==0)
      damp_gfx_mod_use[DAMP_GFX_MOD_ROTATE_LEFT]=TRUE;
   else
      damp_gfx_mod_use[DAMP_GFX_MOD_ROTATE_LEFT]=FALSE;

   sprintf(ini_string,"%s",get_config_string("[gfx_modifiers]","ROTATE_RIGHT","yes"));
   if(stricmp(ini_string,"YES")==0)
      damp_gfx_mod_use[DAMP_GFX_MOD_ROTATE_RIGHT]=TRUE;
   else
      damp_gfx_mod_use[DAMP_GFX_MOD_ROTATE_RIGHT]=FALSE;

   sprintf(ini_string,"%s",get_config_string("[gfx_modifiers]","ROTATE_LEFT_ZOOM","yes"));
   if(stricmp(ini_string,"YES")==0)
      damp_gfx_mod_use[DAMP_GFX_MOD_ROTATE_LEFT_ZOOM]=TRUE;
   else
      damp_gfx_mod_use[DAMP_GFX_MOD_ROTATE_LEFT_ZOOM]=FALSE;

   sprintf(ini_string,"%s",get_config_string("[gfx_modifiers]","ROTATE_RIGHT_ZOOM","yes"));
   if(stricmp(ini_string,"YES")==0)
      damp_gfx_mod_use[DAMP_GFX_MOD_ROTATE_RIGHT_ZOOM]=TRUE;
   else
      damp_gfx_mod_use[DAMP_GFX_MOD_ROTATE_RIGHT_ZOOM]=FALSE;

   sprintf(ini_string,"%s",get_config_string("[gfx_modifiers]","SPIRAL_LEFT","yes"));
   if(stricmp(ini_string,"YES")==0)
      damp_gfx_mod_use[DAMP_GFX_MOD_SPIRAL_LEFT]=TRUE;
   else
      damp_gfx_mod_use[DAMP_GFX_MOD_SPIRAL_LEFT]=FALSE;

   sprintf(ini_string,"%s",get_config_string("[gfx_modifiers]","SPIRAL_RIGHT","yes"));
   if(stricmp(ini_string,"YES")==0)
      damp_gfx_mod_use[DAMP_GFX_MOD_SPIRAL_RIGHT]=TRUE;
   else
      damp_gfx_mod_use[DAMP_GFX_MOD_SPIRAL_RIGHT]=FALSE;

   sprintf(ini_string,"%s",get_config_string("[gfx_modifiers]","SINCOS","yes"));
   if(stricmp(ini_string,"YES")==0)
      damp_gfx_mod_use[DAMP_GFX_MOD_SINCOS]=TRUE;
   else
      damp_gfx_mod_use[DAMP_GFX_MOD_SINCOS]=FALSE;

   sprintf(ini_string,"%s",get_config_string("[gfx_modifiers]","PEARLGLASS","yes"));
   if(stricmp(ini_string,"YES")==0)
      damp_gfx_mod_use[DAMP_GFX_MOD_PEARLGLASS]=TRUE;
   else
      damp_gfx_mod_use[DAMP_GFX_MOD_PEARLGLASS]=FALSE;

   sprintf(ini_string,"%s",get_config_string("[gfx_modifiers]","TRISTAR","yes"));
   if(stricmp(ini_string,"YES")==0)
      damp_gfx_mod_use[DAMP_GFX_MOD_TRISTAR]=TRUE;
   else
      damp_gfx_mod_use[DAMP_GFX_MOD_TRISTAR]=FALSE;

   sprintf(ini_string,"%s",get_config_string("[gfx_modifiers]","SPIKES","yes"));
   if(stricmp(ini_string,"YES")==0)
      damp_gfx_mod_use[DAMP_GFX_MOD_SPIKES]=TRUE;
   else
      damp_gfx_mod_use[DAMP_GFX_MOD_SPIKES]=FALSE;

   sprintf(ini_string,"%s",get_config_string("[gfx_modifiers]","WATERFALL","yes"));
   if(stricmp(ini_string,"YES")==0)
      damp_gfx_mod_use[DAMP_GFX_MOD_WATERFALL]=TRUE;
   else
      damp_gfx_mod_use[DAMP_GFX_MOD_WATERFALL]=FALSE;

   sprintf(ini_string,"%s",get_config_string("[gfx_modifiers]","WAVYZOOM","yes"));
   if(stricmp(ini_string,"YES")==0)
      damp_gfx_mod_use[DAMP_GFX_MOD_WAVYZOOM]=TRUE;
   else
      damp_gfx_mod_use[DAMP_GFX_MOD_WAVYZOOM]=FALSE;

   sprintf(ini_string,"%s",get_config_string("[gfx_modifiers]","RIPPLE","yes"));
   if(stricmp(ini_string,"YES")==0)
      damp_gfx_mod_use[DAMP_GFX_MOD_RIPPLE]=TRUE;
   else
      damp_gfx_mod_use[DAMP_GFX_MOD_RIPPLE]=FALSE;

   sprintf(ini_string,"%s",get_config_string("[gfx_modifiers]","WARP","yes"));
   if(stricmp(ini_string,"YES")==0)
      damp_gfx_mod_use[DAMP_GFX_MOD_WARP]=TRUE;
   else
      damp_gfx_mod_use[DAMP_GFX_MOD_WARP]=FALSE;

   sprintf(ini_string,"%s",get_config_string("[gfx_modifiers]","SUCKER","yes"));
   if(stricmp(ini_string,"YES")==0)
      damp_gfx_mod_use[DAMP_GFX_MOD_SUCKER]=TRUE;
   else
      damp_gfx_mod_use[DAMP_GFX_MOD_SUCKER]=FALSE;

   sprintf(ini_string,"%s",get_config_string("[graphics]","filter","gf_blur"));
   temp_string = get_config_argv(ini_string,"0",&argc);
   for(n=0;n<3;n++)
      gfx_filter.matrix[n][0] = atoi(temp_string[n]);
   temp_string = get_config_argv(ini_string,"1",&argc);
   for(n=0;n<3;n++)
      gfx_filter.matrix[n][1] = atoi(temp_string[n]);
   temp_string = get_config_argv(ini_string,"2",&argc);
   for(n=0;n<3;n++)
      gfx_filter.matrix[n][2] = atoi(temp_string[n]);
   gfx_filter.divisor = 1.0/(float)(get_config_int(ini_string,"divisor",0));
}

/*===================================================================
  damp_gfx_spread(start, end, r1, g1, b1, r2, g2, b2)

  Desc   : Spreads between two colours in the damp_gfx_palette
  Inputs : start    - The palette entry to spread from
           end      - The palette entry to spread to
           r1,g1,b1 - The colour to spread from
           r2,g2,b2 - The colour to spread to
  =================================================================*/

void damp_gfx_spread(int start, int end, float r1, float g1, float b1, float r2, float g2, float b2)
{
   float rstep,gstep,bstep;
   int n;
   float r,g,b;

   rstep=(r2-r1)/(end-start);
   gstep=(g2-g1)/(end-start);
   bstep=(b2-b1)/(end-start);

   r=r1; g=g1; b=b1;

   for(n=start; n<=end; n++)
   {
      damp_gfx_palette[n].r = r;
      damp_gfx_palette[n].g = g;
      damp_gfx_palette[n].b = b;

      r+=rstep; g+=gstep; b+=bstep;
   }
}

/*===================================================================
  damp_gfx_create_palette()

  Desc: Creates a random, but nice-looking palette in
        damp_gfx_palette
  =================================================================*/

void damp_gfx_create_palette(void)
{
   int n;

   damp_gfx_palette[0].r =
   damp_gfx_palette[0].g =
   damp_gfx_palette[0].b = 0;

   if(gtext_in_use)
   {
      /* set the palette for gtext mode */
      damp_gfx_palette[1].r = damp_gfx_palette[1].g = 63; damp_gfx_palette[1].b = 0;
      damp_gfx_palette[2].r = damp_gfx_palette[2].g = damp_gfx_palette[2].b = 32;
      damp_gfx_palette[3].r = damp_gfx_palette[3].g = damp_gfx_palette[3].b = 16;
      damp_gfx_palette[4].r = 63; damp_gfx_palette[4].g = damp_gfx_palette[4].b = 0;
      damp_gfx_palette[5].r = 63; damp_gfx_palette[5].g = 32; damp_gfx_palette[5].b = 16;
      damp_gfx_palette[6].r = 0; damp_gfx_palette[6].g = 63; damp_gfx_palette[6].b = 0;
      damp_gfx_palette[7].r = damp_gfx_palette[7].g = damp_gfx_palette[7].b = 63;
      set_palette(damp_gfx_palette);
      return;
   }

   if(!damp_gfx_bnw_palette)
   {
      damp_gfx_spread(1,64,rand()%16,rand()%16,rand()%16,rand()%64,rand()%64,rand()%64);
      damp_gfx_spread(64,128,damp_gfx_palette[64].r,damp_gfx_palette[64].g,damp_gfx_palette[64].b,rand()%64,rand()%64,rand()%64);
      damp_gfx_spread(128,192,damp_gfx_palette[128].r,damp_gfx_palette[128].g,damp_gfx_palette[128].b,rand()%16,rand()%16,rand()%16);
      damp_gfx_spread(192,251,damp_gfx_palette[192].r,damp_gfx_palette[192].g,damp_gfx_palette[192].b,rand()%64,rand()%64,rand()%64);
   }
   else
   {
      for(n=0;n<252;n++)
         if(n%2 == 0)
            damp_gfx_palette[n].r =
            damp_gfx_palette[n].g =
            damp_gfx_palette[n].b = 0;
         else
            damp_gfx_palette[n].r =
            damp_gfx_palette[n].g =
            damp_gfx_palette[n].b = 63;
   }

   if(!damp_using_gui)
      damp_gfx_palette[255].r=
      damp_gfx_palette[255].g=
      damp_gfx_palette[255].b=63;

   /* posterize the palette */
   if(damp_gfx_posterize)
      for(n=1;n<252;n++)
      {
         damp_gfx_palette[n].r=((int)(damp_gfx_palette[n].r/16))*16;
         damp_gfx_palette[n].g=((int)(damp_gfx_palette[n].g/16))*16;
         damp_gfx_palette[n].b=((int)(damp_gfx_palette[n].b/16))*16;
      }

   set_palette(damp_gfx_palette);
}

/*===================================================================
  damp_gfx_update_scroll()

  Updates the scrolling text position
  =================================================================*/

void damp_gfx_update_scroll(void)
{
   damp_gfx_scroll_name_pos++;
   if(damp_gfx_scroll_name_pos > strlen(damp_gfx_scroll_name))
   {
      damp_gfx_scroll_name_pos = 0;
      damp_gfx_scroll_y = rand()%(DAMP_GFX_SCREEN_HEIGHT-10);
      if(damp_gfx_scroll_times > 0) damp_gfx_scroll_times--;
   }
}
END_OF_FUNCTION(damp_gfx_update_scroll);


/*===================================================================
  damp_gfx_init()

  Desc   : Initialises the graphics subsystem
  Outputs: FALSE if it fails, TRUE if it succeeds.
  Notes  : Should be modified to return false if set_gfx_mode or
           create_bitmap fails.
  =================================================================*/

#define SQR(x) ((x)*(x))
int damp_gfx_init(void)
{
   float x,y,r,ang;
   int a,b,cx,cy;

   cx = DAMP_GFX_SCREEN_WIDTH/2;
   cy = DAMP_GFX_SCREEN_HEIGHT/2;

   /* init modifier arrays */

   /* spiral left */
   for(b=0;b<DAMP_GFX_SCREEN_HEIGHT;b++)
   {
      for(a=0;a<DAMP_GFX_SCREEN_WIDTH; a++)
      {
         r = sqrt(SQR(a-cx)+SQR(b-cy));
         r-=(r/10.0);
         if(r<0) r = 0;
         ang = atan2(b-cy,a-cx) + ((r/10.0)*3.142/180.0);
         x = (float)(cx)+r*cos(ang);
         y = (float)(cy)+r*sin(ang);
         if(cx == a && cy == b)
         {
            x = cx; y = cy;
         }

         damp_gfx_mod[DAMP_GFX_MOD_SPIRAL_LEFT-15][a][b].x = abs((int)x)%DAMP_GFX_SCREEN_WIDTH;
         damp_gfx_mod[DAMP_GFX_MOD_SPIRAL_LEFT-15][a][b].y = abs((int)y)%DAMP_GFX_SCREEN_HEIGHT;
      }
   }

   /* spiral right */
   for(b=0;b<DAMP_GFX_SCREEN_HEIGHT;b++)
   {
      for(a=0;a<DAMP_GFX_SCREEN_WIDTH; a++)
      {
         r = sqrt(SQR(a-cx)+SQR(b-cy));
         r-=(r/10.0);
         if(r<0) r = 0;
         ang = atan2(b-cy,a-cx) - ((r/10.0)*3.142/180.0);
         x = (float)(cx)+r*cos(ang);
         y = (float)(cy)+r*sin(ang);
         if(cx == a && cy == b)
         {
            x = cx; y = cy;
         }

         damp_gfx_mod[DAMP_GFX_MOD_SPIRAL_RIGHT-15][a][b].x = abs((int)x)%DAMP_GFX_SCREEN_WIDTH;
         damp_gfx_mod[DAMP_GFX_MOD_SPIRAL_RIGHT-15][a][b].y = abs((int)y)%DAMP_GFX_SCREEN_HEIGHT;
      }
   }

   /* sincos */
   for(b=0;b<DAMP_GFX_SCREEN_HEIGHT;b++)
   {
      for(a=0;a<DAMP_GFX_SCREEN_WIDTH; a++)
      {
         cx = ((a/64)*64)+32;
         cy = ((b/40)*40)+20;
         r = sqrt(SQR(a-cx)+SQR(b-cy));

         if((cx-32)%128 == 0 && (cy-20)%80 == 0)
            ang = atan2(b-cy,a-cx) + ((r/2.0)*3.142/180.0);
         else
            ang = atan2(b-cy,a-cx) - ((r/2.0)*3.142/180.0);

         x = (float)(cx)+r*cos(ang);
         y = (float)(cy)+r*sin(ang);
         if(cx == a && cy == b)
         {
            x = cx; y = cy;
         }

         damp_gfx_mod[DAMP_GFX_MOD_SINCOS-15][a][b].x = abs((int)x)%DAMP_GFX_SCREEN_WIDTH;
         damp_gfx_mod[DAMP_GFX_MOD_SINCOS-15][a][b].y = abs((int)y)%DAMP_GFX_SCREEN_HEIGHT;
      }
   }

   /* pearlglass */
   for(b=0;b<DAMP_GFX_SCREEN_HEIGHT;b++)
   {
      for(a=0;a<DAMP_GFX_SCREEN_WIDTH; a++)
      {
         x = a + rand()%16 - 8;
         y = b + rand()%16 - 8;
         damp_gfx_mod[DAMP_GFX_MOD_PEARLGLASS-15][a][b].x = abs((int)x)%DAMP_GFX_SCREEN_WIDTH;
         damp_gfx_mod[DAMP_GFX_MOD_PEARLGLASS-15][a][b].y = abs((int)y)%DAMP_GFX_SCREEN_HEIGHT;
      }
   }

   /* tristar */
   cx = DAMP_GFX_SCREEN_WIDTH/2;
   cy = DAMP_GFX_SCREEN_HEIGHT/2;
   for(b=0;b<DAMP_GFX_SCREEN_HEIGHT;b++)
   {
      for(a=0;a<DAMP_GFX_SCREEN_WIDTH; a++)
      {
         r = sqrt(SQR(a-cx)+SQR(b-cy));
         ang = atan2(b-cy,a-cx);
         r-=(fabs(sin(ang*1.5))*(r/10));
         if(r<0) r = 0;
         x = (float)(cx)+r*cos(ang);
         y = (float)(cy)+r*sin(ang);
         if(cx == a && cy == b)
         {
            x = cx; y = cy;
         }

         damp_gfx_mod[DAMP_GFX_MOD_TRISTAR-15][a][b].x = abs((int)x)%DAMP_GFX_SCREEN_WIDTH;
         damp_gfx_mod[DAMP_GFX_MOD_TRISTAR-15][a][b].y = abs((int)y)%DAMP_GFX_SCREEN_HEIGHT;
      }
   }

   /* spikes */
   cx = DAMP_GFX_SCREEN_WIDTH/2;
   cy = DAMP_GFX_SCREEN_HEIGHT/2;
   for(b=0;b<DAMP_GFX_SCREEN_HEIGHT;b++)
   {
      for(a=0;a<DAMP_GFX_SCREEN_WIDTH; a++)
      {
         r = sqrt(SQR(a-cx)+SQR(b-cy));
         ang = atan2(b-cy,a-cx);
         r-=(fabs(sin(ang*10))*(r/5));
         if(r<0) r = 0;
         x = (float)(cx)+r*cos(ang);
         y = (float)(cy)+r*sin(ang);
         if(cx == a && cy == b)
         {
            x = cx; y = cy;
         }

         damp_gfx_mod[DAMP_GFX_MOD_SPIKES-15][a][b].x = abs((int)x)%DAMP_GFX_SCREEN_WIDTH;
         damp_gfx_mod[DAMP_GFX_MOD_SPIKES-15][a][b].y = abs((int)y)%DAMP_GFX_SCREEN_HEIGHT;
      }
   }

   /* waterfall */
   for(b=0;b<DAMP_GFX_SCREEN_HEIGHT;b++)
   {
      for(a=0;a<DAMP_GFX_SCREEN_WIDTH; a++)
      {
         x = a;
         y = b - (((float)b/(float)DAMP_GFX_SCREEN_HEIGHT)*16);
         damp_gfx_mod[DAMP_GFX_MOD_WATERFALL-15][a][b].x = abs((int)x)%DAMP_GFX_SCREEN_WIDTH;
         damp_gfx_mod[DAMP_GFX_MOD_WATERFALL-15][a][b].y = abs((int)y)%DAMP_GFX_SCREEN_HEIGHT;
      }
   }

   /* wavyzoom */
   for(b=0;b<DAMP_GFX_SCREEN_HEIGHT;b++)
   {
      for(a=0;a<DAMP_GFX_SCREEN_WIDTH; a++)
      {
         r = sqrt(SQR(a-cx)+SQR(b-cy));
         r-=(r/10.0);
         if(r<0) r = 0;
         ang = atan2(b-cy,a-cx) + (sin((r*4.0)*3.142/180.0)*((r/10.0)*3.142/180.0));
         x = (float)(cx)+r*cos(ang);
         y = (float)(cy)+r*sin(ang);
         if(cx == a && cy == b)
         {
            x = cx; y = cy;
         }

         damp_gfx_mod[DAMP_GFX_MOD_WAVYZOOM-15][a][b].x = abs((int)x)%DAMP_GFX_SCREEN_WIDTH;
         damp_gfx_mod[DAMP_GFX_MOD_WAVYZOOM-15][a][b].y = abs((int)y)%DAMP_GFX_SCREEN_HEIGHT;
      }
   }

   /* ripple */
   for(b=0;b<DAMP_GFX_SCREEN_HEIGHT;b++)
   {
      for(a=0;a<DAMP_GFX_SCREEN_WIDTH; a++)
      {
         r = sqrt(SQR(a-cx)+SQR(b-cy));
         ang = atan2(b-cy,a-cx);
         r -= fabs(sin((r*10.0)*3.142/180.0)*(r/10.0));
         //r -= 10.0;
         if(r<0) r = 0;
         x = (float)(cx)+r*cos(ang);
         y = (float)(cy)+r*sin(ang);
         if(cx == a && cy == b)
         {
            x = cx; y = cy;
         }

         damp_gfx_mod[DAMP_GFX_MOD_RIPPLE-15][a][b].x = abs((int)x)%DAMP_GFX_SCREEN_WIDTH;
         damp_gfx_mod[DAMP_GFX_MOD_RIPPLE-15][a][b].y = abs((int)y)%DAMP_GFX_SCREEN_HEIGHT;
      }
   }

   /* warp */
   for(b=0;b<DAMP_GFX_SCREEN_HEIGHT;b++)
   {
      for(a=0;a<DAMP_GFX_SCREEN_WIDTH; a++)
      {
         r = sqrt(SQR(a-cx)+SQR(b-cy));
         ang = atan2(b-cy,a-cx);
         r -= ((r/2)-(ang/40));
         if(r<0) r = 0;
         x = (float)(cx)+r*cos(ang);
         y = (float)(cy)+r*sin(ang);
         if(cx == a && cy == b)
         {
            x = cx; y = cy;
         }

         damp_gfx_mod[DAMP_GFX_MOD_WARP-15][a][b].x = abs((int)x)%DAMP_GFX_SCREEN_WIDTH;
         damp_gfx_mod[DAMP_GFX_MOD_WARP-15][a][b].y = abs((int)y)%DAMP_GFX_SCREEN_HEIGHT;
      }
   }

   /* sucker */
   for(b=0;b<DAMP_GFX_SCREEN_HEIGHT;b++)
   {
      for(a=0;a<DAMP_GFX_SCREEN_WIDTH; a++)
      {
         r = sqrt(SQR(a-cx)+SQR(b-cy));
         ang = atan2(b-cy,a-cx);
         r += ((float)(a)/10.0);
         if(r<0) r = 0;
         x = (float)(cx)+r*cos(ang);
         y = (float)(cy)+r*sin(ang);
         if(cx == a && cy == b)
         {
            x = cx; y = cy;
         }

         damp_gfx_mod[DAMP_GFX_MOD_SUCKER-15][a][b].x = abs((int)x)%DAMP_GFX_SCREEN_WIDTH;
         damp_gfx_mod[DAMP_GFX_MOD_SUCKER-15][a][b].y = abs((int)y)%DAMP_GFX_SCREEN_HEIGHT;
      }
   }

   if(!damp_using_gui)
   {
      /* only set the screen mode if the GUI hasn't already done so... */

      if(set_gfx_mode(GFX_AUTODETECT,DAMP_GFX_SCREEN_WIDTH,DAMP_GFX_SCREEN_HEIGHT,0,0)<0)
      {
         if ( damp_developer )
            printf("\ndamp_gfx_init():");
   
         printf("\nUnable to set gfx mode: %s\n\n",allegro_error);
         return FALSE;
      }
   }

   sprintf(damp_gfx_driver,"%s",gfx_driver->name);

   /* create the back-buffer */

   damp_gfx_buffer = create_bitmap(DAMP_GFX_SCREEN_WIDTH,DAMP_GFX_SCREEN_HEIGHT);
   if ( damp_gfx_buffer == NULL )
   {
      if ( damp_developer )
      {         
         printf("\ndamp_gfx_init():");
         printf("  damp_gfx_buffer == NULL\n\n");
      }
      return FALSE;
   }

   clear(damp_gfx_buffer);

   damp_gfx_create_palette();

   /* initialise timer for scrolling */
   i_love_bill = TRUE;
   install_timer();
   LOCK_FUNCTION(damp_gfx_update_scroll);
   LOCK_VARIABLE(damp_gfx_scroll_name);
   LOCK_VARIABLE(damp_gfx_scroll_name_pos);
   LOCK_VARIABLE(damp_gfx_scroll_y);
   LOCK_VARIABLE(damp_gfx_scroll_times);
   install_int_ex(damp_gfx_update_scroll,BPS_TO_TIMER(8));

   damp_gfx_scroll_y = rand()%(DAMP_GFX_SCREEN_HEIGHT-10);

   damp_gfx_waveform = rand()%DAMP_GFX_WAVEFORMS;
   while(!damp_gfx_wave_use[damp_gfx_waveform])
      damp_gfx_waveform = rand()%DAMP_GFX_WAVEFORMS;

   damp_gfx_modifier = rand()%DAMP_GFX_MODIFIERS;
   while(!damp_gfx_mod_use[damp_gfx_modifier])
      damp_gfx_modifier = rand()%DAMP_GFX_MODIFIERS;

   return TRUE;
}

/*===================================================================
  damp_gfx_shutdown()

  Desc: Closes the graphics subsystem, returning to text mode
  =================================================================*/

void damp_gfx_shutdown(void)
{
   remove_int(damp_gfx_update_scroll);

   set_gfx_mode(GFX_TEXT,0,0,0,0);
   destroy_bitmap(damp_gfx_buffer);
}

/*===================================================================
  damp_gfx_render_frame(threshold)

  Desc   : Renders some nice graphics to go with the music
  Inputs : threshold - This parameter is no longer used.
  =================================================================*/

void damp_gfx_render_frame(int threshold)
{
   static BITMAP *temp_buffer;         /* For doing the zoom/fade */
   static unsigned short*last_left;    /* Remember where we were up to */

   /* Pointers to the waveform data: */
   unsigned short *this_left =audio_driver->buffer_left;
   unsigned short *this_right=audio_driver->buffer_right;

   int cnt;                            /* Loop counter */
   int d_left,d_right;                 /* Current left and right values */
   int XSH=2,XSS=XSH; /* 1024 samples => 256 pixels */
   int x,y;                            /* General coordinate variables */
   static float rotang=0;              /* For waveforms requiring an angle */
   int changed = FALSE;                /* So we don't change the waveform TOO often */
   float modang;                       /* Angle variable for modifier */

   int filter_colour;                  /* filter variables */
   int f_dx,f_dy;

   static int x_offset = (DAMP_GFX_SCREEN_WIDTH-256)/2;

   /* if the GTEXT module is in use, call it's render function, and
      don't do any rendering ourselves... */
   if(gtext_in_use)
   {
      gtext_render();
      return;
   }

   if(rotang>360) rotang -=360;

   /* If we haven't created the temporary buffer, we'd better do it */
   if(temp_buffer==NULL)
      temp_buffer = create_bitmap(DAMP_GFX_SCREEN_WIDTH, DAMP_GFX_SCREEN_HEIGHT);

   /* blur the buffer */
      for(y=1;y<DAMP_GFX_SCREEN_HEIGHT-1;y++)
         for(x=1;x<DAMP_GFX_SCREEN_WIDTH-1;x++)
         {
            filter_colour = 0;
            for(f_dx = -1; f_dx <= 1; f_dx++)
            {
               for(f_dy = -1; f_dy <= 1; f_dy++)
               {
                  filter_colour += ( damp_gfx_buffer->line[y+f_dy][x+f_dx]
                                   * gfx_filter.matrix[f_dx+1][f_dy+1]);
               }
            }
            filter_colour *= gfx_filter.divisor;
            if(filter_colour > 251) filter_colour = 251;
            if(filter_colour < 0) filter_colour = 0;
            damp_gfx_buffer->line[y][x] = filter_colour;
         }

  /* perform the image modifier */

  if(damp_gfx_modifier > 14)
  {
   for(y=0;y<DAMP_GFX_SCREEN_HEIGHT;y++)
      for(x=0;x<DAMP_GFX_SCREEN_WIDTH;x++)
         temp_buffer->line[y][x] = damp_gfx_buffer->line[damp_gfx_mod[damp_gfx_modifier-15][x][y].y][damp_gfx_mod[damp_gfx_modifier-15][x][y].x];
  }else
  {
   switch(damp_gfx_modifier)
   {
      case DAMP_GFX_MOD_INVSINZOOM:
         for(y=0;y<DAMP_GFX_SCREEN_HEIGHT;y+=2)
         {
            x = (DAMP_GFX_SCREEN_WIDTH/4) + (DAMP_GFX_SCREEN_WIDTH/4)*sin(((rotang+(y/10))/180.0)*3.142);
            stretch_blit(damp_gfx_buffer,temp_buffer,0,y,DAMP_GFX_SCREEN_WIDTH-1,2,x,y,(DAMP_GFX_SCREEN_WIDTH-(x*2))-1,2);
            rotang+=1;
         }
         break;

      case DAMP_GFX_MOD_SINZOOM:
         for(y=0;y<DAMP_GFX_SCREEN_HEIGHT;y+=2)
         {
            x = (DAMP_GFX_SCREEN_WIDTH/4) + (DAMP_GFX_SCREEN_WIDTH/4)*sin(((rotang+y)/180.0)*3.142);
            stretch_blit(damp_gfx_buffer,temp_buffer,x,y,(DAMP_GFX_SCREEN_WIDTH-(x*2))-1,2,0,y,DAMP_GFX_SCREEN_WIDTH,2);
            rotang+=4;
         }
         break;

      case DAMP_GFX_MOD_ZOOMIN:
         stretch_blit(damp_gfx_buffer,temp_buffer,8,4,DAMP_GFX_SCREEN_WIDTH-15,DAMP_GFX_SCREEN_HEIGHT-7,0,0,DAMP_GFX_SCREEN_WIDTH,DAMP_GFX_SCREEN_HEIGHT);
         break;
      case DAMP_GFX_MOD_ZOOMOUT:
         stretch_blit(damp_gfx_buffer,temp_buffer,0,0,DAMP_GFX_SCREEN_WIDTH,DAMP_GFX_SCREEN_HEIGHT,8,4,DAMP_GFX_SCREEN_WIDTH-15,DAMP_GFX_SCREEN_HEIGHT-7);
         break;
      case DAMP_GFX_MOD_FISHEYE:
         clear(temp_buffer);
         for(modang=180;modang<360;modang++)
         {
            x = (DAMP_GFX_SCREEN_WIDTH/2) + (DAMP_GFX_SCREEN_WIDTH/2)*sin((modang/180.0)*3.142);
            y = (DAMP_GFX_SCREEN_HEIGHT/2) + (DAMP_GFX_SCREEN_HEIGHT/2)*cos((modang/180.0)*3.142);
            x = x*2;
            stretch_blit(damp_gfx_buffer,temp_buffer,0,y,DAMP_GFX_SCREEN_WIDTH,2,x,y,(DAMP_GFX_SCREEN_WIDTH-x)-x,2);
         }
         break;
      case DAMP_GFX_MOD_DOUBLEFISHEYE:
         clear(temp_buffer);
         for(modang=180;modang<360;modang++)
         {
            y = (DAMP_GFX_SCREEN_HEIGHT/2) + (DAMP_GFX_SCREEN_HEIGHT/2)*cos((modang/180.0)*3.142);
            x = (DAMP_GFX_SCREEN_WIDTH/4) + (DAMP_GFX_SCREEN_WIDTH/4)*sin((modang/180.0)*3.142);
            stretch_blit(damp_gfx_buffer,temp_buffer,0,y,DAMP_GFX_SCREEN_WIDTH/2,2,x,y,((DAMP_GFX_SCREEN_WIDTH/2)-x)-x,2);
            stretch_blit(damp_gfx_buffer,temp_buffer,DAMP_GFX_SCREEN_WIDTH/2,y,DAMP_GFX_SCREEN_WIDTH/2,2,x+(DAMP_GFX_SCREEN_WIDTH/2),y,((DAMP_GFX_SCREEN_WIDTH/2)-x)-x,2);
         }
         break;
      case DAMP_GFX_MOD_ZOOMUP:
         stretch_blit(damp_gfx_buffer,temp_buffer,8,16,DAMP_GFX_SCREEN_WIDTH-15,DAMP_GFX_SCREEN_HEIGHT-16,0,0,DAMP_GFX_SCREEN_WIDTH,DAMP_GFX_SCREEN_HEIGHT);
         break;
      case DAMP_GFX_MOD_ZOOMDOWN:
         stretch_blit(damp_gfx_buffer,temp_buffer,8,0,DAMP_GFX_SCREEN_WIDTH-15,DAMP_GFX_SCREEN_HEIGHT-16,0,0,DAMP_GFX_SCREEN_WIDTH,DAMP_GFX_SCREEN_HEIGHT);
         break;
      case DAMP_GFX_MOD_SCROLLUPDOWN:
         blit(damp_gfx_buffer,temp_buffer,0,4,0,0,DAMP_GFX_SCREEN_WIDTH,DAMP_GFX_SCREEN_HEIGHT/2);
         blit(damp_gfx_buffer,temp_buffer,0,DAMP_GFX_SCREEN_HEIGHT/2,0,(DAMP_GFX_SCREEN_HEIGHT/2)+4,DAMP_GFX_SCREEN_WIDTH,DAMP_GFX_SCREEN_HEIGHT/2);
         break;
      case DAMP_GFX_MOD_SCROLLRIGHTLEFT:
         blit(damp_gfx_buffer,temp_buffer,4,0,0,0,DAMP_GFX_SCREEN_WIDTH/2,DAMP_GFX_SCREEN_HEIGHT);
         blit(damp_gfx_buffer,temp_buffer,DAMP_GFX_SCREEN_WIDTH/2,0,(DAMP_GFX_SCREEN_WIDTH/2)+4,0,DAMP_GFX_SCREEN_WIDTH/2,DAMP_GFX_SCREEN_HEIGHT);
         break;
      case DAMP_GFX_MOD_SPHERE:
         clear(temp_buffer);
         for(modang=180;modang<360;modang++)
         {
            x = (DAMP_GFX_SCREEN_WIDTH/2) + (DAMP_GFX_SCREEN_WIDTH/2)*sin((modang/180.0)*3.142);
            y = (DAMP_GFX_SCREEN_HEIGHT/2) + (DAMP_GFX_SCREEN_WIDTH/2)*cos((modang/180.0)*3.142);
            x = x*2;
            stretch_blit(damp_gfx_buffer,temp_buffer,0,y,DAMP_GFX_SCREEN_WIDTH,5,x,y,(DAMP_GFX_SCREEN_WIDTH-x)-x,5);
         }
         break;
      case DAMP_GFX_MOD_ROTATE_LEFT:
         clear(temp_buffer);
         rotate_sprite(temp_buffer, damp_gfx_buffer, 0, 0, itofix(-4));
         break;
      case DAMP_GFX_MOD_ROTATE_RIGHT:
         clear(temp_buffer);
         rotate_sprite(temp_buffer, damp_gfx_buffer, 0, 0, itofix(4));
         break;
      case DAMP_GFX_MOD_ROTATE_LEFT_ZOOM:
         clear(temp_buffer);
         rotate_scaled_sprite(temp_buffer, damp_gfx_buffer, 0, 0, itofix(-4), ftofix(1.1));
         break;
      case DAMP_GFX_MOD_ROTATE_RIGHT_ZOOM:
         clear(temp_buffer);
         rotate_scaled_sprite(temp_buffer, damp_gfx_buffer, 0, 0, itofix(4), ftofix(1.1));
         break;
   }
  }

   /* Only bother doing anything if we've changed since last time, and
      if the data we have in our hand is valid */

   if (1) //(this_left!=last_left)&&(this_left))
   {
      /* Loop through the waveform samples */
//      for (cnt=0; cnt<audio_driver->buffer_size>>XSH; cnt++)
      for (cnt=0; cnt<256; cnt++)
      {
         audio_driver->poll();

          d_left=this_left[cnt*4]/512;
          d_right=this_right[cnt*4]/512;

          if(d_left < 0) d_left = 0;
          if(d_left > 255) d_left = 255;
          if(d_right < 0) d_right = 0;
          if(d_right > 255) d_right = 255;

          /* Change waveform and palette if threshold exceeded */
/*
          if( damp_gfx_sync_to_beats && abs(d_left+d_right) > threshold && (d_left+d_right) > damp_gfx_min_threshold && !changed)
          {
              changed = TRUE;
              damp_gfx_waveform = rand()%DAMP_GFX_WAVEFORMS;
              damp_gfx_create_palette();
          }
*/

          /* Display something nice */

          switch(damp_gfx_waveform)
          {
            case DAMP_GFX_WAVE_BLOBS:
                circlefill(temp_buffer,cnt+x_offset,(rand()%DAMP_GFX_SCREEN_HEIGHT/2)+(DAMP_GFX_SCREEN_HEIGHT/4),(d_left+d_right)/64, d_left+d_right);
                break;

            case DAMP_GFX_WAVE_CONCENTRIC:
               circle(temp_buffer,DAMP_GFX_SCREEN_WIDTH/2,DAMP_GFX_SCREEN_HEIGHT/2,(d_left+d_right)/2,(d_left+d_right));
               break;

            case DAMP_GFX_WAVE_SPINLINES:
               line(temp_buffer,DAMP_GFX_SCREEN_WIDTH/2,DAMP_GFX_SCREEN_HEIGHT/2,
                    (DAMP_GFX_SCREEN_WIDTH/2)+((float)(d_left))*sin((rotang/180.0)*3.142),(DAMP_GFX_SCREEN_HEIGHT/2)+((float)(d_right))*cos((rotang/180.0)*3.142),251);
               line(temp_buffer,DAMP_GFX_SCREEN_WIDTH/2,DAMP_GFX_SCREEN_HEIGHT/2,
                    (DAMP_GFX_SCREEN_WIDTH/2)+((float)(d_left))*sin(((rotang+180.0)/180.0)*3.142),(DAMP_GFX_SCREEN_HEIGHT/2)+((float)(d_right))*cos(((rotang+180.0)/180.0)*3.142),128);
               rotang+=0.1;
               break;

            case DAMP_GFX_WAVE_CORNERS:
                line(temp_buffer,cnt+x_offset,d_left,
                DAMP_GFX_SCREEN_WIDTH-(cnt+x_offset),DAMP_GFX_SCREEN_HEIGHT-d_right,d_left+d_right);
                break;

            case DAMP_GFX_WAVE_DUALHORIZONTAL:
                temp_buffer->line[d_left+18][cnt+x_offset] = 251;
                temp_buffer->line[d_right+37+18][cnt+x_offset] = 251;
                break;

            case DAMP_GFX_WAVE_SOLID:
                vline(temp_buffer,cnt+x_offset,100,d_left+37,d_left*2);
                vline(temp_buffer,cnt+x_offset,100,d_right+37,d_right*2);
                break;

            case DAMP_GFX_WAVE_DOTS:
                temp_buffer->line[d_left+37][cnt+x_offset] = 251;
                temp_buffer->line[d_right+37][cnt+x_offset] = 251;
                break;

            case DAMP_GFX_WAVE_CIRCLE:
               putpixel(temp_buffer,160+((float)(d_left/2))*sin((rotang/180.0)*3.142),100+((float)(d_right/2))*cos((rotang/180.0)*3.142),251);
               rotang+=(360.0/256.0);
               break;

            case DAMP_GFX_WAVE_MULTICIRCLE:
               putpixel(temp_buffer,80+((float)(d_left/2))*sin((rotang/180.0)*3.142),100+((float)(d_right/2))*cos((rotang/180.0)*3.142),251);
               putpixel(temp_buffer,240+((float)(d_left/2))*sin((rotang/180.0)*3.142),100+((float)(d_right/2))*cos((rotang/180.0)*3.142),251);
               putpixel(temp_buffer,160+((float)(d_left/2))*sin((rotang/180.0)*3.142),100+((float)(d_right/2))*cos((rotang/180.0)*3.142),251);
               rotang+=(360.0/256.0);
               break;

            case DAMP_GFX_WAVE_LASER:
               line(temp_buffer,160,100,160+((float)(d_left/2))*sin((rotang/180.0)*3.142),100+((float)(d_right/2))*cos((rotang/180.0)*3.142),d_left+d_right);
               rotang+=(360.0/256.0);
               break;

            case DAMP_GFX_WAVE_MULTILASER:
               line(temp_buffer,160,100,160+((float)(d_left/2))*sin((rotang/180.0)*3.142),100+((float)(d_right/2))*cos((rotang/180.0)*3.142),d_left+d_right);
               line(temp_buffer,80,100,80+((float)(d_left/2))*sin((rotang/180.0)*3.142),100+((float)(d_right/2))*cos((rotang/180.0)*3.142),d_left+d_right);
               line(temp_buffer,240,100,240+((float)(d_left/2))*sin((rotang/180.0)*3.142),100+((float)(d_right/2))*cos((rotang/180.0)*3.142),d_left+d_right);
               line(temp_buffer,160,50,160+((float)(d_left/2))*sin((rotang/180.0)*3.142),50+((float)(d_right/2))*cos((rotang/180.0)*3.142),d_left+d_right);
               line(temp_buffer,160,150,160+((float)(d_left/2))*sin((rotang/180.0)*3.142),150+((float)(d_right/2))*cos((rotang/180.0)*3.142),d_left+d_right);
               rotang+=(360.0/256.0);
               break;
         }
      }

      /* Copy it all onto the actual buffer, and copy that to the screen */
      blit(temp_buffer,damp_gfx_buffer,0,0,0,0,DAMP_GFX_SCREEN_WIDTH, DAMP_GFX_SCREEN_HEIGHT);



      /* Remember where we got up to */
      last_left=this_left;

  }

  /* show track title if necessary */

      if(damp_gfx_show_name && damp_gfx_scroll_times != 0)
      {
         int ty = damp_gfx_scroll_y;

         text_mode(-1);
         textout(damp_gfx_buffer,font,&damp_gfx_scroll_name[damp_gfx_scroll_name_pos],-1,ty,0);
         textout(damp_gfx_buffer,font,&damp_gfx_scroll_name[damp_gfx_scroll_name_pos],1,ty,0);
         textout(damp_gfx_buffer,font,&damp_gfx_scroll_name[damp_gfx_scroll_name_pos],-1,ty-1,0);
         textout(damp_gfx_buffer,font,&damp_gfx_scroll_name[damp_gfx_scroll_name_pos],1,ty-1,0);
         textout(damp_gfx_buffer,font,&damp_gfx_scroll_name[damp_gfx_scroll_name_pos],-1,ty+1,0);
         textout(damp_gfx_buffer,font,&damp_gfx_scroll_name[damp_gfx_scroll_name_pos],+1,ty+1,0);
         textout(damp_gfx_buffer,font,&damp_gfx_scroll_name[damp_gfx_scroll_name_pos],0,ty-1,0);
         textout(damp_gfx_buffer,font,&damp_gfx_scroll_name[damp_gfx_scroll_name_pos],0,ty+1,0);

         textout(damp_gfx_buffer,font,&damp_gfx_scroll_name[damp_gfx_scroll_name_pos],0,ty,makecol(255,255,255));
      }

  /* show track time if necessary */

      if(damp_gfx_show_time)
      {
         int ty = DAMP_GFX_SCREEN_HEIGHT-12;
         int tx = DAMP_GFX_SCREEN_WIDTH/2;

         timer(0);

         text_mode(-1);
         textout_centre(damp_gfx_buffer,font,damp_display_time_remaining ? timeremainstr : timestr,tx-1,ty-1,0);
         textout_centre(damp_gfx_buffer,font,damp_display_time_remaining ? timeremainstr : timestr,tx-1,ty,0);
         textout_centre(damp_gfx_buffer,font,damp_display_time_remaining ? timeremainstr : timestr,tx-1,ty+1,0);
         textout_centre(damp_gfx_buffer,font,damp_display_time_remaining ? timeremainstr : timestr,tx,ty-1,0);
         textout_centre(damp_gfx_buffer,font,damp_display_time_remaining ? timeremainstr : timestr,tx,ty+1,0);
         textout_centre(damp_gfx_buffer,font,damp_display_time_remaining ? timeremainstr : timestr,tx+1,ty-1,0);
         textout_centre(damp_gfx_buffer,font,damp_display_time_remaining ? timeremainstr : timestr,tx+1,ty,0);
         textout_centre(damp_gfx_buffer,font,damp_display_time_remaining ? timeremainstr : timestr,tx+1,ty+1,0);

         textout_centre(damp_gfx_buffer,font,damp_display_time_remaining ? timeremainstr : timestr,tx,ty,makecol(255,255,255));
      }

   /* if we're not using the GUI, copy the gfx to the screen, for all the
      world to look at and go "wow!" :-) */

      if(!damp_using_gui)
         blit(damp_gfx_buffer,screen,0,0,0,0,DAMP_GFX_SCREEN_WIDTH,DAMP_GFX_SCREEN_HEIGHT);
}

