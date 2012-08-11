/* cd.h

   Header file for CD player functions of DAMP
   By Matt Craven
   (c)2000 Hedgehog Software
*/

#ifndef __CD_H__
#define __CD_H__

extern int damp_using_cd;
extern volatile unsigned int damp_cd_time;
extern int damp_cd_ejected;

int damp_cd_main(void);
int damp_cd_get_track(void);
void damp_cd_previous(void);
void damp_cd_next(void);

#endif

