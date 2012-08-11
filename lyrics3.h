/* Lyrics3.h

   Header file for routines for loading and displaying Lyrics3 Tags
   By Matt Craven - (c)2000 Hedgehog Software

   Lyrics3 Tags were devised by Petr Strnad and Alon Gingold.
*/

#ifndef __LYRICS3_H__
#define __LYRICS3_H__

typedef struct {

   unsigned long timestamp;     /* When the lyric should occur, in seconds */

   char *lyric;                 /* The lyric text */

   int displayed;               /* Been displayed yet? */

}LYRICS3_SCHEDULE;

LYRICS3_SCHEDULE *lyrics3_schedule;     /* Array of scheduled lyrics */

void lyrics3_get_tag(char *filename);

void lyrics3_run(void);

#endif /* ifndef __LYRICS3_H__ */

