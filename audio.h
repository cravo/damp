/* audio.h

   Header file for audio driver for DAMP
   By Matt Craven
*/

#ifndef __AUDIO_H__
#define __AUDIO_H__


typedef int(*AUDIO_CALLBACK)(void);

typedef int(*AUDIO_ACTION)(int);

typedef int(*AUDIO_FILEIO)(char*);

typedef struct {

   int id;              /* Driver ID */

   char desc[256];      /* Short driver description */

   int buffer_size;     /* Audio buffer size */

   void *buffer_left;   /* Pointer to waveform for left channel */

   void *buffer_right;  /* Pointer to waveform for right channel */

   int sample_rate;     /* Sample rate */

   int bitrate;         /* Bit-rate */

   int layer;           /* For MPEG, this is the layer */

   int version;         /* For MPEG, this is the version */

   int stereo;          /* Is source in stereo? */

   int surround;        /* Want surround-sound? */

   int time;            /* Track time in seconds */

   AUDIO_CALLBACK init; /* Pointer to initialisation function */

   AUDIO_CALLBACK exit; /* Pointer to exit function */

   AUDIO_CALLBACK poll; /* Pointer to polling function */

   AUDIO_CALLBACK run;  /* Pointer to run function */

   AUDIO_ACTION ffwd;   /* Fast-forward function */

   AUDIO_ACTION rew;    /* Rewind function */

   AUDIO_FILEIO open;   /* Open a file */

   AUDIO_FILEIO close;  /* Close the file at end of playback */

}AUDIO_DRIVER;


AUDIO_DRIVER *audio_driver;   /* Pointer to the audio driver */


int audio_init(int driver);
void audio_exit(void);

#endif
