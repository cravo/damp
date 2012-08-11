/* audio_xaudio.h

   Header for audio driver for DAMP to do MP3 playback using the XAudio
   library.

   By Matt Craven
*/

#ifndef __AUDIO_XAUDIO_H__
#define __AUDIO_XAUDIO_H__

int xamp_init(void);
int xamp_exit(void);
int xamp_poll(void);
int xamp_run(void);
int xamp_ffwd(int);
int xamp_rew(int);
int xamp_open(char*);
int xamp_close(char*);

#endif
