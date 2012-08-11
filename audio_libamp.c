/* audio_libamp.c

   Audio driver for DAMP to do MP3 playback using the LibAMP
   library.

   By Matt Craven
*/

#include <libamp.h>
#include "audio.h"

int lamp_init(void)
{
   return(install_amp());
}

int lamp_exit(void)
{
   unload_amp();

   return 1;
}

/* returns number of bytes read */

int lamp_poll(void)
{
   int x = poll_amp();

   amp_reverse_phase = audio_driver->surround;

   audio_driver->buffer_size = amp_play_len;
   audio_driver->buffer_left = amp_play_left;
   audio_driver->buffer_right = amp_play_right;
   audio_driver->time = amp_time;

   return(x);
}

int lamp_run(void)
{
   return(run_amp());
}

int lamp_ffwd(int secs)
{
   seek_amp_rel((secs*amp_samprat)/amp_pollsize); /* seek forward n seconds */

   return 1;
}

int lamp_rew(int secs)
{
   seek_amp_rel((-secs*amp_samprat)/amp_pollsize); /* seek back n seconds */

   return 1;
}

int lamp_open(char *filename)
{
   int x = load_amp(filename,0);

   audio_driver->layer = amp_layer;
   audio_driver->version = amp_mpg_ver;
   audio_driver->bitrate = amp_bitrate;
   audio_driver->sample_rate = amp_samprat;
   audio_driver->stereo = amp_stereo;

   return(x);
}

int lamp_close(char *filename)
{
   return 1;
}

