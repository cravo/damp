/* graphics.h
   Header file for audio visualisation code for DAMP */

#ifndef __GRAPHICS_H__
#define __GRAPHICS_H__

#define DAMP_GFX_SCREEN_WIDTH 320
#define DAMP_GFX_SCREEN_HEIGHT 200

#define DAMP_GFX_MODIFIERS 26
#define DAMP_GFX_MOD_ZOOMIN 0
#define DAMP_GFX_MOD_FISHEYE 1
#define DAMP_GFX_MOD_ZOOMUP 2
#define DAMP_GFX_MOD_SCROLLUPDOWN 3
#define DAMP_GFX_MOD_ZOOMDOWN 4
#define DAMP_GFX_MOD_SCROLLRIGHTLEFT 5
#define DAMP_GFX_MOD_DOUBLEFISHEYE 6
#define DAMP_GFX_MOD_SINZOOM 7
#define DAMP_GFX_MOD_INVSINZOOM 8
#define DAMP_GFX_MOD_ZOOMOUT 9
#define DAMP_GFX_MOD_SPHERE 10
#define DAMP_GFX_MOD_ROTATE_LEFT 11
#define DAMP_GFX_MOD_ROTATE_RIGHT 12
#define DAMP_GFX_MOD_ROTATE_LEFT_ZOOM 13
#define DAMP_GFX_MOD_ROTATE_RIGHT_ZOOM 14
#define DAMP_GFX_MOD_SPIRAL_LEFT 15
#define DAMP_GFX_MOD_SPIRAL_RIGHT 16
#define DAMP_GFX_MOD_SINCOS 17
#define DAMP_GFX_MOD_PEARLGLASS 18
#define DAMP_GFX_MOD_TRISTAR 19
#define DAMP_GFX_MOD_SPIKES 20
#define DAMP_GFX_MOD_WATERFALL 21
#define DAMP_GFX_MOD_WAVYZOOM 22
#define DAMP_GFX_MOD_RIPPLE 23
#define DAMP_GFX_MOD_WARP 24
#define DAMP_GFX_MOD_SUCKER 25

#define DAMP_GFX_WAVEFORMS 11
#define DAMP_GFX_WAVE_DOTS 0
#define DAMP_GFX_WAVE_SOLID 1
#define DAMP_GFX_WAVE_CIRCLE 2
#define DAMP_GFX_WAVE_LASER 3
#define DAMP_GFX_WAVE_DUALHORIZONTAL 4
#define DAMP_GFX_WAVE_MULTICIRCLE 5
#define DAMP_GFX_WAVE_MULTILASER 6
#define DAMP_GFX_WAVE_CORNERS 7
#define DAMP_GFX_WAVE_SPINLINES 8
#define DAMP_GFX_WAVE_CONCENTRIC 9
#define DAMP_GFX_WAVE_BLOBS 10

typedef struct {
   int matrix[3][3];
   float divisor;
}GFX_FILTER;

typedef struct {
   int x,y;
}GFX_COORD;

extern BITMAP *damp_gfx_buffer;
extern RGB damp_gfx_palette[256];

extern int damp_gfx_wave_use[DAMP_GFX_WAVEFORMS];
extern int damp_gfx_mod_use[DAMP_GFX_MODIFIERS];
GFX_COORD damp_gfx_mod[DAMP_GFX_MODIFIERS-15][DAMP_GFX_SCREEN_WIDTH][DAMP_GFX_SCREEN_HEIGHT];

extern int damp_gfx_min_threshold;       /* minimum - to stop flashing */
extern int damp_gfx_waveform;
extern int damp_gfx_modifier;
extern int damp_gfx_sync_to_beats;
extern int damp_gfx_randomize_modifier;
extern int damp_gfx_bnw_palette;      /* Use black+white palette? */
extern int damp_gfx_show_name;        /* Show track name? */
extern char damp_gfx_scroll_name[340];        /* For scrolling the track name */
extern volatile int damp_gfx_scroll_name_pos;      /* position in scrolling */
extern volatile int damp_gfx_scroll_y;    /* vertical pos of text */
extern volatile int damp_gfx_scroll_times;        /* Number of times to scroll name (-1 = inf) */
extern char damp_gfx_driver[256];             /* Gfx driver name */
extern int damp_gfx_show_time;         /* display time? */

int damp_gfx_init(void);
void damp_gfx_render_frame(int threshold);
void damp_gfx_get_functionality(void);
void damp_gfx_create_palette(void);
void damp_gfx_shutdown(void);

void screenshot(void);

#endif

