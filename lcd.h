/* LCD.h
   LCD driver for DAMP
*/

#ifndef __LCD_H__
#define __LCD_H__

#include <seer.h>

/* LCD PROTOTYPES */
void lcd_init(void);
void lcd_display(void);
void lcd_load_driver(char*);
void lcd_deinit(void);

/* LCD GLOBALS */
extern int lcd_in_use;                      /* Using the LCD part of damp? */
extern unsigned char lcd_line[2][41];
extern unsigned char lcd_status[10];
extern volatile int lcd_scroll_pos;
extern volatile int lcd_require_update;
extern unsigned char lcd_scroll_text[1024];
extern int lcd_scroll_speed;
extern scScript lcd_driver_script;
extern scInstance *lcd_driver_instance;

extern char lcd_surround_symbol;
extern char lcd_play_symbol;
extern char lcd_random_symbol;
extern char lcd_rew_symbol;
extern char lcd_continuous_symbol;

extern char lcd_driver_name[256];
extern char lcd_driver_author[256];
extern char lcd_driver_author_email[256];
extern char lcd_driver_description[256];
extern float lcd_driver_version;

extern char damp_id3_genre[64];

extern int lcd_display_width;           /* Width of LCD in characters */
extern int lcd_display_lines;           /* Number of LCD lines */

#endif

