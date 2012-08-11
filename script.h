/* SCRIPT.H
   Header file for script routines
*/

#ifndef __SCRIPT_H__
#define __SCRIPT_H__

/* Shared vars, for both types of driver to use */
int damp_shared_int;
char damp_shared_char[1024];
float damp_shared_float;
long damp_shared_long;

void lcd_poll(void);
void lcd_rest(int dtime);
void script_init(void);

#endif
