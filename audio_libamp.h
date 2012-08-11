/* audio_libamp.h

   Header for audio driver for DAMP to do MP3 playback using the LibAMP
   library.

   By Matt Craven
*/

#ifndef __AUDIO_LIBAMP_H__
#define __AUDIO_LIBAMP_H__

int lamp_init(void);
int lamp_exit(void);
int lamp_poll(void);
int lamp_run(void);
int lamp_ffwd(int);
int lamp_rew(int);
int lamp_open(char*);
int lamp_close(char*);

#endif
