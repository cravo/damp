/* audio.c

   Audio driver routines for DAMP
   By Matt Craven
*/

#include <stdio.h>
#include "audio.h"

#include "audio_libamp.h"
#include "audio_xaudio.h"

#define AUDIO_ID(a,b,c,d) (a<<24 | b<<16 | c<<8 | d)

#define AUDIO_MAX_DRIVERS 2

AUDIO_DRIVER _audio_driver[AUDIO_MAX_DRIVERS];

#define AUDIO_DEC(index, _id, _desc, _init, _exit, _poll, _run, _ffwd, _rew, _open, _close) \
                  _audio_driver[index].id = _id; \
                  sprintf(_audio_driver[index].desc,_desc); \
                  _audio_driver[index].init = _init; \
                  _audio_driver[index].exit = _exit; \
                  _audio_driver[index].poll = _poll; \
                  _audio_driver[index].run = _run; \
                  _audio_driver[index].ffwd = _ffwd; \
                  _audio_driver[index].rew = _rew; \
                  _audio_driver[index].open = _open; \
                  _audio_driver[index].close = _close;

int audio_init(int driver)
{
   AUDIO_DEC(0,AUDIO_ID('L','A','M','P'), "MP3 driver using LibAMP", lamp_init, lamp_exit, lamp_poll, lamp_run, lamp_ffwd, lamp_rew, lamp_open, lamp_close);
   AUDIO_DEC(1,AUDIO_ID('X','A','M','P'), "MP3 driver using XAudio", xamp_init, xamp_exit, xamp_poll, xamp_run, xamp_ffwd, xamp_rew, xamp_open, xamp_close);

   if(driver >= AUDIO_MAX_DRIVERS)
   {
      printf("audio_init(): Driver specified is not valid\n");
      exit(-1);
   }

   audio_driver = &_audio_driver[driver];

   return (audio_driver->init());
}

void audio_exit(void)
{
   if(!audio_driver->exit())
      printf("audio_exit(): Warning! Error shutting down audio driver\n");
}
