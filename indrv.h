/* indrv.h

   Header file for input driver routines for DAMP
*/

#ifndef __INDRV_H__
#define __INDRV_H__

#include <seer.h>

#define KEY_SLASH2 KEY_SLASH_PAD

void indrv_init(void);
int indrv_poll(void);
void indrv_load_driver(char *);
void indrv_deinit(void);

extern int indrv_in_use;        /* Using input driver? */
extern int indrv_keyboard_removed;

scScript indrv_driver_script;
scInstance *indrv_driver_instance;

char indrv_driver_name[256];
char indrv_driver_author[256];
char indrv_driver_author_email[256];
char indrv_driver_description[256];
float indrv_driver_version;

int indrv_return_value;

#endif

