/* gui.h
   Header file for DAMP GUI
*/

#ifndef __GUI_H__
#define __GUI_H__

#include <allegro.h>

extern int GUI_SCREEN_W;
extern int GUI_SCREEN_H;

#define GUI_COLOUR_BACKGROUND 255
#define GUI_COLOUR_HIGHLIGHT 254
#define GUI_COLOUR_SHADOW 253
#define GUI_COLOUR_WINDOW 252

extern float gui_version;

BITMAP *gui_buffer;
BITMAP *gui_mouse;
RGB gui_palette[256];

typedef int(*GUI_BUTTON_FUNC)(void);
#define GUI_BUTTON_CLICK 0
#define GUI_BUTTON_HOLD 1
#define GUI_BUTTON_TOGGLE 2

typedef struct {
   int x,y,width,height;   /* Position & Size */
   int num_items;          /* Number of items */
   char **item;            /* The list items, array of strings */
   int sorted;             /* List is sorted? */
   int index;              /* Current index */

   int up_button;          /* index of up button */
   int down_button;        /* index of down button */
}GUI_LIST;

typedef struct {
   int x,y,width,height;   /* Position & size */
   char caption[1024];     /* Text */
   int textbox;            /* Look like a textbox? */
}GUI_LABEL;

typedef struct {
   int x,y,width,height;   /* Position & size */
   char caption[20];       /* Text */
   int type;               /* GUI_BUTTON_CLICK or GUI_BUTTON_HOLD */
   GUI_BUTTON_FUNC pressed_func; /* Pressed function */
   int pressed;

   int window;             /* Associated window */
   int list;               /* Associated list (or -1 for none) */
}GUI_BUTTON;

typedef struct __gw {
   int x,y,width,height;   /* position and size */
   char title[80];         /* Window title */
   int draggable;          /* Can window be dragged? */

   struct __gw *parent;    /* Parent of this window (or NULL) */

   int content_changed;    /* Has this windows content changed since last redraw? */

   int visible;            /* Window visible? */

   BITMAP *bmp;
   BITMAP *picture;

   int num_labels;
   GUI_LABEL *label;

   int num_buttons;
   GUI_BUTTON *button;

   int num_lists;
   GUI_LIST *list;
}GUI_WINDOW;


GUI_WINDOW *gui_window;
int gui_num_windows;
int gui_window_drag_x;
int gui_window_drag_y;
int gui_window_dragging;
int gui_window_topmost;

int gui_titlebar_height;
int gui_border_size;

int gui_event_window;
int gui_event_button;

void gui_init(void);

void gui_render(void);

void gui_handler(void);

int gui_create_window(int x, int y, int w, int h, char *title, int draggable);
void gui_create_sub_window(GUI_WINDOW *parent, int x, int y, int w, int h, char *title);
int gui_create_label(int win, int x, int y, int w, int h, char *txt, int tb);
int gui_create_button(int win, int x, int y, int w, int h, char *txt, int type, GUI_BUTTON_FUNC pressed_func);
int gui_create_list(int win, int x, int y, int w, int h, int sorted);
void gui_list_additem(int win, int list, char *txt);
void gui_list_clear(int win, int list);
void gui_playlist_update(void);

#endif
