/* damp.h
   Header file for main DAMP program */

#ifndef __DAMP_H__
#define __DAMP_H__

#include <allegro.h>

// Comment the next line when it goes public
//#define PRIVATE_BETA

#define DAMP_TRACK_SELECT_BPS 1     /* For keypad track selection */

/*===================================================================
  G L O B A L S
  =================================================================*/

extern float damp_version;
extern char damp_sub_version[6];

extern int damp_yield;       /* Should DAMP yield CPU ? */

char track_sel[8];           /* to store selected track number */
int track_sel_pos;           /* for indexing into track_sel */
int selecting_track_flag;   /* For using keypad to select tracks */
int track_sel_shift_flag;   /* for selecting playlists */
volatile int timeout_flag;

int damp_ffwd_rew_speed;  /* Fast-forward/rewind speed */
int damp_vol_up_down_speed;  /* Volume change speed */

int damp_track_number;        /* Current Track Number */

int damp_using_gui;   /* Using the gui? */
extern int damp_gui_win_info;        /* gui windows */
extern int damp_gui_win_vis;
extern int damp_gui_win_controls;
int damp_gui_win_playlist;

extern int damp_gui_label_filename;  /* gui labels */
extern int damp_gui_label_artist;    
extern int damp_gui_label_title;     
extern int damp_gui_label_comment;   
extern int damp_gui_label_year;      
extern int damp_gui_label_genre;
extern int damp_gui_label_rate;      
extern int damp_gui_label_type;      
extern int damp_gui_label_trackno;
extern int damp_gui_label_time;

extern int damp_gui_button_previous;      /* gui_buttons */
extern int damp_gui_button_rewind;
extern int damp_gui_button_pause;
extern int damp_gui_button_fastforward;
extern int damp_gui_button_next;
extern int damp_gui_button_surround;
extern int damp_gui_button_random;
extern int damp_gui_button_quit;

int damp_gui_list_playlist;

int damp_keypad_timeout;       /* Seconds before keypad timeout */

int damp_gfx_posterize;  /* Posterize gfx palette? */

int damp_fnfnoquit;   /* No-quit on file not found? */

int damp_vu;                  /* Current VU meter level */

extern int damp_want_beep;         /* Beep at start of new track? */

extern int damp_beautify_filenames; /* Make filenames look nice? */
int damp_show_file_info;    /* Display bitrate etc? */

int damp_developer;   /* Enable developer mode? (verbose output) */

extern int damp_id3_support;  /* Endable/Disable ID3 support */

extern int damp_want_scope;   /* Enable/Disable the text-mode scope */

extern int damp_scope_smooth; /* text-mode scope smoothing */

char damp_ini_file[1024];     /* Full path+filename of DAMP.ini */

int damp_use_conio_input;

char file[1024];    /* The current filename */
char short_filename[256];     /* Just the filename, without the path */
char damp_playlist_filename[1024]; /* current playlist filename */
char damp_playlist_filename_short[256]; /* current playlist filename (without the path) */

extern int damp_display_time_remaining;  /* Want remaining time? */
char timestr[5];
char timeremainstr[5];
char ratestr[256];
char scopestr[70];  /* Used to output info */

int using_graphics;   /* Using the graphics subsystem? */
int using_playlist;   /* Using the playlist subsystem? */
int need_to_quit;
int need_to_quit_program;
extern int start_of_new_track;       /* Used to reset sync_to_beats */
int track_paused;
extern int quality;                /* 1=poor, 10=excellent, 0=best */
int damp_volume;
//float threshold[] = { 999.0, 4.0, 2.0, 1.5, 1.2, 1.1, 0.99, 0.90, 0.5, 0.1, -1.0, -999.0};
extern float threshold[12];
extern int current_threshold;      /* Form 0 to 10, index into above array, 11 is user-defined */
extern int remember_scroll_times;
extern char scopechar[2];            /* The text-mode scope character */

extern char damp_state;              /* The current state of DAMP */

#define DAMP_STATE_PLAY       0
#define DAMP_STATE_PAUSE      1
#define DAMP_STATE_REW        2
#define DAMP_STATE_FFWD       3
#define DAMP_STATE_VOL_UP     4
#define DAMP_STATE_VOL_DOWN   5

/*===================================================================
  P R O T O T Y P E S
  =================================================================*/

void scope(void);
void timer(int forceupdate);
void usage(char *progname);
void display_program_title(void);
void menu(void);
void output_id3_info(char *filename);
int process_commandline(int argc, char **argv);
int process_damp_ini(void);
void damp_init(void);
void screenshot(void);

void gui_open_windows(void);

#endif

