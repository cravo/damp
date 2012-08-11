/* gui.c

   DAMP gui code
   By Matt Craven
   (c)1999/2000 Hedgehog Software
*/

/*========= i n c l u d e s =======================================*/

#include <stdio.h>
#include <allegro.h>

/*========= d a m p   i n c l u d e s =============================*/

#include "damp.h"
#include "playlist.h"
#include "beautify.h"
#include "gui.h"
#include "graphics.h"

/*========= g l o b a l   v a r i a b l e s =======================*/

float gui_version = 1.01;
int GUI_SCREEN_W=1024;
int GUI_SCREEN_H=768;

/* mouse pointer 11x19 - 0 = trans, 1 =black, 2 = white */
char gui_mouse_data[]={ 1,0,0,0,0,0,0,0,0,0,0,
                        1,1,0,0,0,0,0,0,0,0,0,
                        1,2,1,0,0,0,0,0,0,0,0,
                        1,2,2,1,0,0,0,0,0,0,0,
                        1,2,2,2,1,0,0,0,0,0,0,
                        1,2,2,2,2,1,0,0,0,0,0,
                        1,2,2,2,2,2,1,0,0,0,0,
                        1,2,2,2,2,2,2,1,0,0,0,
                        1,2,2,2,2,2,2,2,1,0,0,
                        1,2,2,2,2,2,2,2,2,1,0,
                        1,2,2,2,2,2,1,1,1,1,1,
                        1,2,2,1,2,2,1,0,0,0,0,
                        1,2,1,0,1,2,2,1,0,0,0,
                        1,1,0,0,1,2,2,1,0,0,0,
                        1,0,0,0,0,1,2,2,1,0,0,
                        0,0,0,0,0,1,2,2,1,0,0,
                        0,0,0,0,0,0,1,2,2,1,0,
                        0,0,0,0,0,0,1,2,2,1,0,
                        0,0,0,0,0,0,0,1,1,0,0 };

/*===================================================================
  gui_init()

  This function initialises the GUI system, setting the appropriate
  graphics mode.

===================================================================*/

void gui_init(void)
{
   int x,y,n;                 /* Loop counters */
   char ini_colours[256];     /* For reading in colour values from the ini */
   char **ini_colvals;        /* The array of ini colour values */
   int argc;                  /* The number of ini colour values */

   /* We need the mouse, so we'll init that */

   install_mouse();

   /* Read the desired width & height from the ini file */

   GUI_SCREEN_W = get_config_int("[gui]","width",640);
   GUI_SCREEN_H = get_config_int("[gui]","height",480);

   /* Set the graphics mode */

   set_gfx_mode(GFX_AUTODETECT,GUI_SCREEN_W,GUI_SCREEN_H,0,0);
   clear(screen);

   /* Create the back-buffer */

   gui_buffer = create_bitmap(GUI_SCREEN_W, GUI_SCREEN_H);

   /* Create the mouse pointer bitmap */

   gui_mouse = create_bitmap(11,19);

   /* Read in the colours from the ini file */

   damp_gfx_palette[GUI_COLOUR_BACKGROUND].r = damp_gfx_palette[GUI_COLOUR_BACKGROUND].g = damp_gfx_palette[GUI_COLOUR_BACKGROUND].b = 32;
   damp_gfx_palette[GUI_COLOUR_SHADOW].r = damp_gfx_palette[GUI_COLOUR_SHADOW].g = damp_gfx_palette[GUI_COLOUR_SHADOW].b = 0;
   damp_gfx_palette[GUI_COLOUR_HIGHLIGHT].r = damp_gfx_palette[GUI_COLOUR_HIGHLIGHT].g = damp_gfx_palette[GUI_COLOUR_HIGHLIGHT].b = 63;
   damp_gfx_palette[GUI_COLOUR_WINDOW].r = 63; damp_gfx_palette[GUI_COLOUR_WINDOW].g = damp_gfx_palette[GUI_COLOUR_WINDOW].b = 0;

   sprintf(ini_colours,get_config_string("[gui]","colours","gui_c_default"));

   ini_colvals = get_config_argv(ini_colours,"background",&argc);
   damp_gfx_palette[GUI_COLOUR_BACKGROUND].r = atoi(ini_colvals[0]) / 4;
   damp_gfx_palette[GUI_COLOUR_BACKGROUND].g = atoi(ini_colvals[1]) / 4;
   damp_gfx_palette[GUI_COLOUR_BACKGROUND].b = atoi(ini_colvals[2]) / 4;

   ini_colvals = get_config_argv(ini_colours,"titlebar",&argc);
   damp_gfx_palette[GUI_COLOUR_WINDOW].r = atoi(ini_colvals[0]) / 4;
   damp_gfx_palette[GUI_COLOUR_WINDOW].g = atoi(ini_colvals[1]) / 4;
   damp_gfx_palette[GUI_COLOUR_WINDOW].b = atoi(ini_colvals[2]) / 4;

   ini_colvals = get_config_argv(ini_colours,"highlight",&argc);
   damp_gfx_palette[GUI_COLOUR_HIGHLIGHT].r = atoi(ini_colvals[0]) / 4;
   damp_gfx_palette[GUI_COLOUR_HIGHLIGHT].g = atoi(ini_colvals[1]) / 4;
   damp_gfx_palette[GUI_COLOUR_HIGHLIGHT].b = atoi(ini_colvals[2]) / 4;

   ini_colvals = get_config_argv(ini_colours,"shadow",&argc);
   damp_gfx_palette[GUI_COLOUR_SHADOW].r = atoi(ini_colvals[0]) / 4;
   damp_gfx_palette[GUI_COLOUR_SHADOW].g = atoi(ini_colvals[1]) / 4;
   damp_gfx_palette[GUI_COLOUR_SHADOW].b = atoi(ini_colvals[2]) / 4;

   set_palette(damp_gfx_palette);

   /* Initialise the variables */

   gui_num_windows = 0;
   gui_window_dragging = -1;
   gui_titlebar_height = text_height(font)+4;
   gui_border_size = 4;

   /* Initialise the mouse pointer */

   clear(gui_mouse);
   n=0;
   for(y=0;y<19;y++)
      for(x=0;x<11;x++)
      {
         if(gui_mouse_data[n] == 0)
            putpixel(gui_mouse,x,y,0);
         else if(gui_mouse_data[n] == 1)
            putpixel(gui_mouse,x,y,GUI_COLOUR_SHADOW);
         else
            putpixel(gui_mouse,x,y,GUI_COLOUR_HIGHLIGHT);

         n++;
      }

}


/*===================================================================
  gui_render_window()

  This function renders the given window.
  =================================================================*/

void gui_render_window(int n)
{
   int lb, b, x, y, w, h, li; /* Temp variables and loop counters */


   /* Only render the window if its content has changed */

   if(gui_window[n].content_changed)
   {
      /* reset the content_changed flag */

      gui_window[n].content_changed = FALSE;

      /* Render the window to its bitmap */

      clear_to_color(gui_window[n].bmp,GUI_COLOUR_BACKGROUND);

      /* If the window has a background picture, copy it in */

      if(gui_window[n].picture != NULL)
      {
         blit(gui_window[n].picture,gui_window[n].bmp,0,0,gui_border_size,gui_border_size+gui_titlebar_height,gui_window[n].picture->w,gui_window[n].picture->h);
      }
   
      /* draw border */
      rect(gui_window[n].bmp, 0,0, gui_window[n].bmp->w-1, gui_window[n].bmp->h-1, GUI_COLOUR_SHADOW);
      rect(gui_window[n].bmp, 1,1, gui_window[n].bmp->w-2, gui_window[n].bmp->h-2, GUI_COLOUR_SHADOW);
      line(gui_window[n].bmp, 1,1, gui_window[n].bmp->w-2, 1, GUI_COLOUR_HIGHLIGHT);
      line(gui_window[n].bmp, 1,1, 1, gui_window[n].bmp->h-2, GUI_COLOUR_HIGHLIGHT);
      rect(gui_window[n].bmp, 2,2, gui_window[n].bmp->w-3, gui_window[n].bmp->h-3, GUI_COLOUR_WINDOW);
      rect(gui_window[n].bmp, 3,3, gui_window[n].bmp->w-4, gui_window[n].bmp->h-4, GUI_COLOUR_SHADOW);
      line(gui_window[n].bmp, gui_window[n].bmp->w-4, 3, gui_window[n].bmp->w-4,gui_window[n].bmp->h-4, GUI_COLOUR_HIGHLIGHT);
      line(gui_window[n].bmp, gui_window[n].bmp->w-4, gui_window[n].bmp->h-4,3,gui_window[n].bmp->h-4,GUI_COLOUR_HIGHLIGHT);
   
      /* draw titlebar */
      rectfill(gui_window[n].bmp, 4,4, gui_window[n].bmp->w-5, gui_titlebar_height+4, GUI_COLOUR_WINDOW);
      line(gui_window[n].bmp, 4,gui_titlebar_height+3, gui_window[n].bmp->w-5, gui_titlebar_height+3, GUI_COLOUR_HIGHLIGHT);
      line(gui_window[n].bmp, 4,gui_titlebar_height+4, gui_window[n].bmp->w-5, gui_titlebar_height+4, GUI_COLOUR_SHADOW);
      text_mode(-1);
      textprintf(gui_window[n].bmp, font, 7, 7, GUI_COLOUR_SHADOW, "%s", gui_window[n].title);
      textprintf(gui_window[n].bmp, font, 6, 6, GUI_COLOUR_HIGHLIGHT, "%s", gui_window[n].title);
   
      /* Draw labels */
      for(lb=0;lb<gui_window[n].num_labels;lb++)
      {
         rectfill(gui_window[n].bmp, gui_window[n].label[lb].x+gui_border_size, gui_window[n].label[lb].y+gui_titlebar_height+gui_border_size,
              gui_window[n].label[lb].x+gui_window[n].label[lb].width+gui_border_size, gui_window[n].label[lb].y+gui_window[n].label[lb].height+gui_titlebar_height+gui_border_size, GUI_COLOUR_BACKGROUND);
   
         if(gui_window[n].label[lb].textbox)
         {
            rectfill(gui_window[n].bmp, gui_window[n].label[lb].x+gui_border_size, gui_window[n].label[lb].y+gui_titlebar_height+gui_border_size,
                 gui_window[n].label[lb].x+gui_window[n].label[lb].width+gui_border_size, gui_window[n].label[lb].y+gui_window[n].label[lb].height+gui_titlebar_height+gui_border_size, GUI_COLOUR_HIGHLIGHT);
   
            rect(gui_window[n].bmp, gui_window[n].label[lb].x+gui_border_size, gui_window[n].label[lb].y+gui_titlebar_height+gui_border_size,
                 gui_window[n].label[lb].x+gui_window[n].label[lb].width+gui_border_size, gui_window[n].label[lb].y+gui_window[n].label[lb].height+gui_titlebar_height+gui_border_size, GUI_COLOUR_SHADOW);
         }
   
         textprintf(gui_window[n].bmp, font,
                    gui_window[n].label[lb].x+gui_border_size+2, gui_window[n].label[lb].y+gui_titlebar_height+gui_border_size+2,
                    GUI_COLOUR_SHADOW, "%s", gui_window[n].label[lb].caption);
      }

      /* Draw buttons */
      for(b=0;b<gui_window[n].num_buttons;b++)
      {
         rectfill(gui_window[n].bmp, gui_window[n].button[b].x+gui_border_size, gui_window[n].button[b].y+gui_titlebar_height+gui_border_size,
              gui_window[n].button[b].x+gui_window[n].button[b].width+gui_border_size, gui_window[n].button[b].y+gui_window[n].button[b].height+gui_titlebar_height+gui_border_size, GUI_COLOUR_BACKGROUND);
   
         rect(gui_window[n].bmp, gui_window[n].button[b].x+gui_border_size, gui_window[n].button[b].y+gui_titlebar_height+gui_border_size,
              gui_window[n].button[b].x+gui_window[n].button[b].width+gui_border_size, gui_window[n].button[b].y+gui_window[n].button[b].height+gui_titlebar_height+gui_border_size, GUI_COLOUR_SHADOW);
         rect(gui_window[n].bmp, gui_window[n].button[b].x+gui_border_size+1, gui_window[n].button[b].y+gui_titlebar_height+gui_border_size+1,
              gui_window[n].button[b].x+gui_window[n].button[b].width+gui_border_size-1, gui_window[n].button[b].y+gui_window[n].button[b].height+gui_titlebar_height+gui_border_size-1, gui_window[n].button[b].pressed ? GUI_COLOUR_HIGHLIGHT : GUI_COLOUR_SHADOW);

         line(gui_window[n].bmp, gui_window[n].button[b].x+gui_border_size+1, gui_window[n].button[b].y+gui_titlebar_height+gui_border_size+1,
                                 gui_window[n].button[b].x+gui_border_size+1, gui_window[n].button[b].y+gui_titlebar_height+gui_border_size+gui_window[n].button[b].height-1, gui_window[n].button[b].pressed ? GUI_COLOUR_SHADOW : GUI_COLOUR_HIGHLIGHT);
         line(gui_window[n].bmp, gui_window[n].button[b].x+gui_border_size+1, gui_window[n].button[b].y+gui_titlebar_height+gui_border_size+1,
                                 gui_window[n].button[b].x+gui_border_size+1+gui_window[n].button[b].width-2, gui_window[n].button[b].y+gui_titlebar_height+gui_border_size+1, gui_window[n].button[b].pressed ? GUI_COLOUR_SHADOW : GUI_COLOUR_HIGHLIGHT);

         w = text_length(font, gui_window[n].button[n].caption);
         h = text_height(font);
         x = gui_window[n].button[b].x + (gui_window[n].button[b].width/2);
         y = gui_window[n].button[b].y + (gui_window[n].button[b].height/2);
         textprintf(gui_window[n].bmp, font, (x-(w/2))+gui_border_size, (y-(h/2))+gui_titlebar_height+gui_border_size, GUI_COLOUR_SHADOW, gui_window[n].button[b].caption);
      }

      /* draw lists */
      for(b=0;b<gui_window[n].num_lists;b++)
      {
         /* list */
         rectfill(gui_window[n].bmp, gui_window[n].list[b].x+gui_border_size, gui_window[n].list[b].y+gui_titlebar_height+gui_border_size,
              gui_window[n].list[b].x+gui_window[n].list[b].width-10+gui_border_size, gui_window[n].list[b].y+gui_window[n].list[b].height+gui_titlebar_height+gui_border_size, GUI_COLOUR_HIGHLIGHT);
   
         rect(gui_window[n].bmp, gui_window[n].list[b].x+gui_border_size, gui_window[n].list[b].y+gui_titlebar_height+gui_border_size,
              gui_window[n].list[b].x+gui_window[n].list[b].width-10+gui_border_size, gui_window[n].list[b].y+gui_window[n].list[b].height+gui_titlebar_height+gui_border_size, GUI_COLOUR_SHADOW);

         /* slider background */
         rectfill(gui_window[n].bmp, gui_window[n].list[b].x+gui_window[n].list[b].width-10+gui_border_size, gui_window[n].list[b].y+10+gui_titlebar_height+gui_border_size,
              gui_window[n].list[b].x+gui_window[n].list[b].width+gui_border_size, gui_window[n].list[b].y+gui_window[n].list[b].height-10+gui_titlebar_height+gui_border_size, GUI_COLOUR_SHADOW);

         /* slider */
         if(gui_window[n].list[b].num_items == 0)
         {
            rectfill(gui_window[n].bmp, gui_window[n].list[b].x+1+gui_window[n].list[b].width-10+gui_border_size, gui_window[n].list[b].y+1+10+gui_titlebar_height+gui_border_size,
                 gui_window[n].list[b].x-1+gui_window[n].list[b].width+gui_border_size, gui_window[n].list[b].y+gui_window[n].list[b].height-1-10+gui_titlebar_height+gui_border_size, GUI_COLOUR_HIGHLIGHT);
         }else
         {
            y = (gui_window[n].list[b].height-20) * ((float)((float)gui_window[n].list[b].index / (float)gui_window[n].list[b].num_items));
            if(y > gui_window[n].list[b].height - 31)
               y = gui_window[n].list[b].height - 31;
            rectfill(gui_window[n].bmp, gui_window[n].list[b].x+1+gui_window[n].list[b].width-10+gui_border_size, gui_window[n].list[b].y+1+10+y+gui_titlebar_height+gui_border_size,
                 gui_window[n].list[b].x-1+gui_window[n].list[b].width+gui_border_size, gui_window[n].list[b].y+1+10+y+10+gui_titlebar_height+gui_border_size, GUI_COLOUR_HIGHLIGHT);           
         }

         /* list text */
         h = gui_window[n].list[b].index + (gui_window[n].list[b].height/10);
         if(h > gui_window[n].list[b].num_items)
            h = gui_window[n].list[b].num_items;
         x = 0;
         for(li = gui_window[n].list[b].index; li < h; li++)
         {
            w = text_length(font, gui_window[n].list[b].item[li]);
            y = gui_window[n].list[b].y + 2 + (x*10);

            if(w > gui_window[n].list[b].width - 12)
               gui_window[n].list[b].item[li][(gui_window[n].list[b].width/9)+2] = 0;

            textprintf(gui_window[n].bmp, font, gui_window[n].list[b].x + 2+gui_border_size, y+gui_titlebar_height+gui_border_size, GUI_COLOUR_SHADOW, gui_window[n].list[b].item[li]);

            x++;
         }
      }
   }

   /* Copy the window to the gui back-buffer */

   blit(gui_window[n].bmp, gui_buffer, 0,0, gui_window[n].x, gui_window[n].y, gui_window[n].bmp->w, gui_window[n].bmp->h);
}


/*===================================================================
  gui_render()

  This function renders the GUI and displays it on the screen.
  =================================================================*/

void gui_render(void)
{
   int n;      /* Loop counter */

   /* Clear the back-buffer */

   clear_to_color(gui_buffer,GUI_COLOUR_BACKGROUND);

   /* render the windows */

   for(n=0; n<gui_num_windows; n++)
   {
      if(gui_window[n].visible) gui_render_window(n);
   }

   /* If the topmost window is visible, render it again (to make it topmost!) */

   if(gui_window[gui_window_topmost].visible)
      gui_render_window(gui_window_topmost);

   /* Display the mouse pointer */

   draw_sprite(gui_buffer, gui_mouse, mouse_x, mouse_y);

   /* Copy the GUI onto the screen */

   blit(gui_buffer, screen,0,0,0,0,gui_buffer->w,gui_buffer->h);
}

/*===================================================================
  gui_handler()

  This should be called every frame, as it traps mouse action, deals
  with window dragging etc.
  =================================================================*/

void gui_handler(void)
{
   int n,mx,my,b,rx,ry;    /* temp and loop variables */
   static int can_press_mouse = TRUE;  /* Remeber mouse state */

   /* Get the mouse position */

   mx = mouse_x;
   my = mouse_y;

   /* If the mouse button is pressed, we'd better do something... */

   if(mouse_b & 1)
   {
      /* Check for window drag */

      if(gui_window_dragging >= 0)
      {
         gui_window[gui_window_dragging].x = mx - gui_window_drag_x;
         gui_window[gui_window_dragging].y = my - gui_window_drag_y;
      } else
      {
         for(n=0;n<gui_num_windows;n++)
         {
            if(gui_window[n].draggable)
            {
               if(mx > gui_window[n].x && mx < gui_window[n].x+gui_window[n].width
               && my > gui_window[n].y && my < gui_window[n].y+gui_window[n].height+gui_titlebar_height+gui_border_size)
               {
                  if(my < gui_window[n].y+gui_titlebar_height+gui_border_size)
                  {
                     gui_window_dragging = n;
                     gui_window_drag_x = mx - gui_window[n].x;
                     gui_window_drag_y = my - gui_window[n].y;
                     gui_window_topmost = n;
                  }
                  else
                  {
                     /* check for button presses */
                     rx = mx - gui_window[n].x - gui_border_size;
                     ry = my - gui_window[n].y - gui_border_size - gui_titlebar_height;

                     for(b = 0; b < gui_window[n].num_buttons; b++)
                     {
                        if(rx > gui_window[n].button[b].x && rx < gui_window[n].button[b].x+gui_window[n].button[b].width
                        && ry > gui_window[n].button[b].y && ry < gui_window[n].button[b].y+gui_window[n].button[b].height)
                        {
                           if(gui_window[n].button[b].type == GUI_BUTTON_HOLD)
                           {
                              gui_event_window = n;
                              gui_event_button = b;
                              gui_window[n].button[b].pressed = TRUE;
                              if(gui_window[n].button[b].pressed_func != NULL)
                                 gui_window[n].button[b].pressed_func();
                           } else
                           {
                              if(gui_window[n].button[b].type == GUI_BUTTON_TOGGLE && can_press_mouse)
                              {
                                 gui_event_window = n;
                                 gui_event_button = b;
                                 gui_window[n].button[b].pressed = !gui_window[n].button[b].pressed;
                                 can_press_mouse = FALSE;
                                 if(gui_window[n].button[b].pressed_func != NULL)
                                    gui_window[n].button[b].pressed_func();
                              }
                              else
                              {
                                 if(can_press_mouse)
                                 {
                                    gui_event_window = n;
                                    gui_event_button = b;
                                    gui_window[n].button[b].pressed = TRUE;
                                    can_press_mouse = FALSE;
                                    if(gui_window[n].button[b].pressed_func != NULL)
                                       gui_window[n].button[b].pressed_func();
                                 }
                              }
                           }
                           gui_window[n].content_changed = TRUE;
                        }
                     }
                  }
               }
            }
         }
      }
   } else
   {
      /* if mouse button not pressed, we can't be dragging, and we
         can press the mouse */

      gui_window_dragging = -1;
      can_press_mouse = TRUE;

      /* We also can't be clicking any non-toggle buttons, so make
         sure they're reset */

      for(n=0;n<gui_num_windows;n++)
         for(b=0;b<gui_window[n].num_buttons;b++)
            if(gui_window[n].button[b].type != GUI_BUTTON_TOGGLE)
            {
               if(gui_window[n].button[b].pressed)
               {
                  gui_window[n].button[b].pressed = FALSE;
                  gui_window[n].content_changed = TRUE;
               }
            }
   }
}

/*===================================================================
  gui_create_window(x,y,w,h,title,draggable)

  This will create a window at x,y with dimensions w by h, with the
  given title.  draggable indicates whether or not the window can
  be dragged.
  =================================================================*/

int gui_create_window(int x, int y, int w, int h, char *title, int draggable)
{
   /* Note there's a new window, and create it */

   gui_num_windows++;

   gui_window = realloc(gui_window,sizeof(GUI_WINDOW)*gui_num_windows);

   /* Setup its parameters */

   gui_window[gui_num_windows-1].x = x;
   gui_window[gui_num_windows-1].y = y;
   gui_window[gui_num_windows-1].width = w;
   gui_window[gui_num_windows-1].height = h;
   gui_window[gui_num_windows-1].content_changed = TRUE;
   gui_window[gui_num_windows-1].visible = TRUE;
   sprintf(gui_window[gui_num_windows-1].title,"%s",title);

   /* create its rendering bitmap */

   gui_window[gui_num_windows-1].bmp = create_bitmap(w+(gui_border_size*2),h+(gui_border_size*2)+gui_titlebar_height);

   /* Init the rest of its properties */

   gui_window[gui_num_windows-1].draggable = draggable;
   gui_window[gui_num_windows-1].picture = NULL;
   gui_window[gui_num_windows-1].num_labels = 0;
   gui_window[gui_num_windows-1].label = NULL;
   gui_window[gui_num_windows-1].num_buttons = 0;
   gui_window[gui_num_windows-1].button = NULL;
   gui_window[gui_num_windows-1].num_lists = 0;
   gui_window[gui_num_windows-1].list = NULL;

   /* Make this window topmost */

   gui_window_topmost = gui_num_windows-1;

   /* Return this window's ID */

   return gui_num_windows-1;
}


/*===================================================================
  gui_create_sub_window(parent, x, y, w, h, title)

  THIS FUNCTION IS NOT IMPLEMENTED
  =================================================================*/

void gui_create_sub_window(GUI_WINDOW *parent, int x, int y, int w, int h, char *title)
{
}

/*===================================================================
  gui_create_label(win, x, y, w, h, txt, tb)

  This function creates a label on the given window, at x,y with
  dimensions w by h.  The label will have whatever's in txt as
  its caption.  If tb is not 0, then the label has a "text-box"
  style (ie it has an outline, and a highlighted background).
  =================================================================*/

int gui_create_label(int win, int x, int y, int w, int h, char *txt, int tb)
{
   /* Note that the window's content has changed */

   gui_window[win].content_changed = TRUE;

   /* Note there's a new label, and create it */

   gui_window[win].num_labels++;

   gui_window[win].label = realloc(gui_window[win].label, sizeof(GUI_LABEL)*gui_window[win].num_labels);

   /* Fill in its properties */

   gui_window[win].label[gui_window[win].num_labels-1].x = x;
   gui_window[win].label[gui_window[win].num_labels-1].y = y;
   gui_window[win].label[gui_window[win].num_labels-1].width = w;
   gui_window[win].label[gui_window[win].num_labels-1].height = h;
   gui_window[win].label[gui_window[win].num_labels-1].textbox = tb;
   sprintf(gui_window[win].label[gui_window[win].num_labels-1].caption,"%s",txt);

   /* Return it's ID */

   return(gui_window[win].num_labels-1);
}

/*===================================================================
  gui_create_button(win, x, y, w, h, txt, type, pressed_func)

  This function creates a button on window win at x,y with dimensions
  w by h.  The button has whatever's in txt as its caption. type
  determines the button's behaviour, and should be one of the
  GUI_BUTTON_* defines from gui.h
  The pressed_func is a pointer to the function that gets called when
  the button is pressed.
  =================================================================*/

int gui_create_button(int win, int x, int y, int w, int h, char *txt, int type, GUI_BUTTON_FUNC pressed_func)
{
   /* Note that the content has changed */

   gui_window[win].content_changed = TRUE;

   /* Note there's a new button, and create it */

   gui_window[win].num_buttons++;

   gui_window[win].button = realloc(gui_window[win].button, sizeof(GUI_BUTTON)*gui_window[win].num_buttons);

   /* Fill in its properties */

   gui_window[win].button[gui_window[win].num_buttons-1].x = x;
   gui_window[win].button[gui_window[win].num_buttons-1].y = y;
   gui_window[win].button[gui_window[win].num_buttons-1].width = w;
   gui_window[win].button[gui_window[win].num_buttons-1].height = h;
   gui_window[win].button[gui_window[win].num_buttons-1].type = type;
   gui_window[win].button[gui_window[win].num_buttons-1].pressed_func = pressed_func;
   sprintf(gui_window[win].button[gui_window[win].num_buttons-1].caption,"%s",txt);
   gui_window[win].button[gui_window[win].num_buttons-1].pressed = FALSE;
   gui_window[win].button[gui_window[win].num_buttons-1].window = win;

   /* Return it's ID */

   return(gui_window[win].num_buttons-1);
}

/*===================================================================
  _gui_list_up()

  This is a private function for moving a list up
  =================================================================*/

int _gui_list_up(void)
{
   if(gui_window[gui_event_window].list[gui_window[gui_event_window].button[gui_event_button].list].index
      > 0)
      gui_window[gui_event_window].list[gui_window[gui_event_window].button[gui_event_button].list].index--;

   return 1;
}

/*====================================================================
  _gui_list_down()

  This is a private function for moving a list down
  ==================================================================*/

int _gui_list_down(void)
{
   if(gui_window[gui_event_window].list[gui_window[gui_event_window].button[gui_event_button].list].index
      < gui_window[gui_event_window].list[gui_window[gui_event_window].button[gui_event_button].list].num_items-1)
   gui_window[gui_event_window].list[gui_window[gui_event_window].button[gui_event_button].list].index++;

   return 1;
}

/*===================================================================
  gui_create_list(win, x, y, w, h, sorted)

  This creates a list on window win at x,y with dimensions w by h.
  The sorted parameter indicates whether the list should be sorted
  or not (NOTE: List sorting remains unimplemented)
  =================================================================*/

int gui_create_list(int win, int x, int y, int w, int h, int sorted)
{
   /* Note that this window's content has changed */

   gui_window[win].content_changed = TRUE;

   /* Note there's a new list, and create it */

   gui_window[win].num_lists++;

   gui_window[win].list = realloc(gui_window[win].list, sizeof(GUI_LIST)*gui_window[win].num_lists);

   /* Fill in the list's parameters */

   gui_window[win].list[gui_window[win].num_lists-1].x = x;
   gui_window[win].list[gui_window[win].num_lists-1].y = y;
   gui_window[win].list[gui_window[win].num_lists-1].width = w;
   gui_window[win].list[gui_window[win].num_lists-1].height = h;
   gui_window[win].list[gui_window[win].num_lists-1].sorted = sorted;
   gui_window[win].list[gui_window[win].num_lists-1].num_items = 0;
   gui_window[win].list[gui_window[win].num_lists-1].item = NULL;
   gui_window[win].list[gui_window[win].num_lists-1].index = 0;

   /* Make buttons for its up and down */

   gui_window[win].list[gui_window[win].num_lists-1].up_button =
      gui_create_button(win, x+w-10, y, 10, 10, "", GUI_BUTTON_HOLD, _gui_list_up);
   gui_window[win].button[gui_window[win].list[gui_window[win].num_lists-1].up_button].list = gui_window[win].num_lists - 1;
   gui_window[win].list[gui_window[win].num_lists-1].down_button =
      gui_create_button(win, x+w-10, y+h-10, 10, 10, "", GUI_BUTTON_HOLD, _gui_list_down);
   gui_window[win].button[gui_window[win].list[gui_window[win].num_lists-1].down_button].list = gui_window[win].num_lists - 1;

   /* Return its ID */

   return(gui_window[win].num_lists-1);
}

/*===================================================================
  gui_list_additem(win, list, txt)

  This adds the line of text in txt to the given list, in the given
  window, win.

  Note: This function actually adds the item to the last list in
  the window, not the list specified. This would need altering if
  any window had more than one list...
  =================================================================*/

void gui_list_additem(int win, int list, char *txt)
{
   gui_window[win].list[gui_window[win].num_lists-1].num_items++;

   gui_window[win].list[gui_window[win].num_lists-1].item =
      realloc(gui_window[win].list[gui_window[win].num_lists-1].item,sizeof(char *)*gui_window[win].list[gui_window[win].num_lists-1].num_items);

   gui_window[win].list[gui_window[win].num_lists-1].item[gui_window[win].list[gui_window[win].num_lists-1].num_items-1] =
      malloc(sizeof(char)*(strlen(txt)+1));

   sprintf(gui_window[win].list[gui_window[win].num_lists-1].item[gui_window[win].list[gui_window[win].num_lists-1].num_items-1],"%s",txt);

   /* This is where list sorting code should be inserted: */
   
}

/*===================================================================
  gui_list_clear(win, list)

  This clears the given list on the given window.
  =================================================================*/

void gui_list_clear(int win, int list)
{
   int n;

   for(n=0;n<gui_window[win].list[list].num_items; n++)
   {
      if(gui_window[win].list[list].item[n] != NULL)
         free(gui_window[win].list[list].item[n]);
   }

   if(gui_window[win].list[list].item != NULL)
      free(gui_window[win].list[list].item);

   gui_window[win].list[list].item = NULL;
   gui_window[win].list[list].num_items = 0;
   gui_window[win].list[list].index = 0;
}

/*===================================================================
  gui_playlist_update()

  This function updates the playlist display in the GUI.
  =================================================================*/

void gui_playlist_update(void)
{
   int x;
   char guitemp[1024];

   if(damp_using_gui)
   {
      gui_list_clear(damp_gui_win_playlist, damp_gui_list_playlist);
      for(x=0;x<damp_playlist_size;x++)
      {
         if(damp_playlist_random)
            sprintf(guitemp,get_filename(damp_playlist[damp_playlist_order[x]].filename));
         else
            sprintf(guitemp,get_filename(damp_playlist[x].filename));

         if(damp_beautify_filenames)
         {
            beautify(guitemp);
            gui_list_additem(damp_gui_win_playlist, damp_gui_list_playlist, guitemp);
         }
         else
            gui_list_additem(damp_gui_win_playlist, damp_gui_list_playlist, guitemp);
      }
      gui_window[damp_gui_win_playlist].content_changed = TRUE;
   }
}
