/* audio_xaudio.c

   Audio driver for DAMP to do MP3 playback using the XAudio
   library.

   By Matt Craven
*/

#include <allegro.h>
#include <xaudio.h>
#include "audio.h"

#include "decoder.h"
#include "file_input.h"

#define AUDIO_STREAM_BUFFER_SIZE  2304

AUDIOSTREAM *stream;

XA_DecoderInfo *decoder;
XA_InputModule module;

unsigned short xa_buffer_left[1024];
unsigned short xa_buffer_right[1024];
   
int xamp_init(void)
{
   /* Create a decoder */
   if(decoder_new(&decoder) != XA_SUCCESS)
   {
      return 0;
   }

   /* Register decoder input modules */
   file_input_module_register(&module);
   decoder_input_module_register(decoder, &module);

   return 1;
}

int xamp_exit(void)
{
   decoder_delete(decoder);

   return 1;
}

/* returns -1 if there's nothing to play, 0 if the playback buffer
   is already full, and 1 if decoding took place */

int xamp_poll(void)
{
   int i;
   int n = 0;

   /* does the stream need any more data yet? */
   unsigned short *p = get_audio_stream_buffer(stream);

   if (p)
   {
      /* if so, generate a bit more of our waveform... */


      if(decoder_decode(decoder, p) != XA_SUCCESS)
      {
         return -1;
      }else
      {
       if(decoder->output_buffer->size == 0)
         return -1;

       for (i=0; i<AUDIO_STREAM_BUFFER_SIZE; i++)
       {
         p[i] ^= 0x8000;

         if(n<1024)
         {
            if(i % 2 == 0)
               xa_buffer_left[n] = p[i];
            else
            {
               xa_buffer_right[n] = p[i];
               n++;
            }
         }

         if(audio_driver->surround)
            if(i % 2 == 0) p[i] = 65535-p[i];

       }
      }

      free_audio_stream_buffer(stream);
     
      audio_driver->buffer_size = 1024;
      audio_driver->buffer_left = xa_buffer_left;
      audio_driver->buffer_right = xa_buffer_right;
      audio_driver->time = (decoder->status->timecode.m*60)
                         + decoder->status->timecode.s;

      return 1;
   }

   return 0;
}

int xamp_run(void)
{
   int x;

   while((x=xamp_poll()) == 1);

   return(x);
}

int xamp_ffwd(int secs)
{
   decoder_input_seek_to_time(decoder,
       (decoder->status->timecode.m*60)+decoder->status->timecode.s+secs);

   return 1;
}

int xamp_rew(int secs)
{
   decoder_input_seek_to_time(decoder,
       (decoder->status->timecode.m*60)+decoder->status->timecode.s-secs);

   return 1;
}

int xamp_open(char *filename)
{
   /* Open the MP3 */
   decoder_input_new(decoder, filename, XA_DECODER_INPUT_AUTOSELECT);
   if(decoder_input_open(decoder) != XA_SUCCESS)
   {
      return 0;
   }

   /* create an audio stream */
   stream = play_audio_stream(AUDIO_STREAM_BUFFER_SIZE/2, 16, TRUE, 44100, 255, 128);
   if (!stream) {
      return 0;
   }

   audio_driver->layer = decoder->status->info.layer;
   audio_driver->version = decoder->status->info.level;
   audio_driver->bitrate = decoder->status->info.bitrate;
   audio_driver->sample_rate = decoder->status->info.frequency;
   audio_driver->stereo = decoder->output_buffer->stereo;

   return 1;
}

int xamp_close(char *filename)
{
   stop_audio_stream(stream);
   
   decoder_input_close(decoder);

   return 1;
}

