/* playlist.c

   Playlist functionality for DAMP.
   By Matt Craven
*/

/*======= i n c l u d e s =========================================*/

#include <stdio.h>
#include <malloc.h>
#include <string.h>
#include <allegro.h>

/*======= d a m p   i n c l u d e s ===============================*/

#include "damp.h"
#include "playlist.h"
#include "util.h"

/*======= g l o b a l   v a r i a b l e s =========================*/

DAMP_PLAYLIST *damp_playlist = NULL;   // We'll allocate this later
int *damp_playlist_order = NULL;       // and this...
int damp_playlist_size = 0;            // Number of entries in the playlist
int damp_playlist_random = FALSE;      // Play randomly?
int damp_playlist_entry = 0;           // Current entry
int damp_playlist_num_played = 0;      // Number of tracks we've played
int damp_playlist_num_to_play = 0;     // How many tracks to play altogether
int damp_playlist_num_to_skip = 0;     // How many tracks to skip
int damp_playlist_continuous = FALSE;   // Loop back to start of plist when end reached?

DAMP_PLAYLIST *damp_metaplaylist = NULL;// for multi-playlist support
int *damp_metaplaylist_order = NULL;   // for random support
int damp_metaplaylist_size = 0;        // Size of the metaplaylist
int damp_metaplaylist_random = FALSE;  // Play randomly?
int damp_metaplaylist_entry = 0;       // Current entry
int damp_metaplaylist_num_played = 0;  // Number of playlists we've played


/*===================================================================
  damp_playlist_read_line(fp, txt)

  Desc   : Reads one line from a text file
  Inputs : fp  - A pointer to the file
           txt - A pointer to an array of char, where you'd like the
                 line of text to be stored.
  Outputs: FALSE at end of file, TRUE otherwise.
  =================================================================*/

int damp_playlist_read_line(FILE *fp, char *txt)
{
  int p=0;
  char onechar;

  do
  {
    onechar = getc(fp);
    if(onechar == EOF) return FALSE;
    txt[p]=onechar;
    p++;
  }while(txt[p-1]!='\n' && p<256 && txt[p-1] != '\r');
  p--;
  while(p<256) txt[p++]=0;

  return TRUE;
}

/*===================================================================
  damp_playlist_entry_compute_time(filename, *entry)

  Computes track time for an entry.
  ***NB: NOT USED***
  =================================================================*/

void damp_playlist_entry_compute_time(char *filename, DAMP_PLAYLIST*entry)
{
/*
   FILE *in;
   unsigned int header;
   unsigned int bitrate,samprate,padding,framelen,mpversion,mplayer;

   in = fopen(filename,"rb");
   fread(&header, sizeof(unsigned int), 1, in);
   fclose(in);

   mpversion = 4 - ((header & 0x1800) >> 11);
   mplayer = ((header & 0x6000) >> 13);
   bitrate = (header & 0x1E0000) >> 17;

//   printf("%s: header = %u mpversion = %u mplayer = %u bitrate = %u\n\n",filename,header,mpversion,mplayer,bitrate);
*/
}

/*===================================================================
  damp_playlist_add_entry_to_already_malloced(char *filename)

  Desc   : Adds a filename to an already malloced playlist
  Inputs : filename - the filename of the file to add
  =================================================================*/

void damp_playlist_add_entry_to_already_malloced(char *filename)
{
   damp_playlist_size++;

   /* Set up this entry */

   sprintf(damp_playlist[damp_playlist_size-1].filename,"%s",filename);
//   damp_playlist[damp_playlist_size-1].filename = filename;
   damp_playlist[damp_playlist_size-1].been_played = FALSE;
   damp_playlist_entry_compute_time(filename, &damp_playlist[damp_playlist_size-1]);
}

/*===================================================================
  damp_playlist_add_entry(char *filename)

  Desc   : Adds a filename to the DAMP playlist
  Inputs : filename - The filename of the file to add
  =================================================================*/

void damp_playlist_add_entry(char *filename)
{
   damp_playlist_size++;

   /* reserve some memory for damp_playlist_order */
   if ( damp_playlist_order == NULL )
      damp_playlist_order = malloc(sizeof(int));
   else
      damp_playlist_order = realloc(damp_playlist_order, sizeof(int)*(damp_playlist_size+1));

   /* die if out of memory */
   if ( damp_playlist_order == NULL )
   {
      printf("\n   damp_playlist_add_entry(): Out of memory when allocating damp_playlist_order\n\n");
      exit(-1);
   }

   /* Reserve some memory for this entry */
   if ( damp_playlist == NULL )
      damp_playlist = malloc(sizeof(DAMP_PLAYLIST));
   else
      damp_playlist = realloc(damp_playlist, sizeof(DAMP_PLAYLIST)*(damp_playlist_size+1));

   /* Die if we run out of memory */

   if ( damp_playlist == NULL )
   {
      printf("\n   damp_playlist_add_entry(): Out of memory when allocating damp_playlist\n\n");
      exit(-1);
   }

   /* Set up this entry */

   sprintf(damp_playlist[damp_playlist_size-1].filename,"%s",filename);
   damp_playlist[damp_playlist_size-1].been_played = FALSE;
   damp_playlist_entry_compute_time(filename, &damp_playlist[damp_playlist_size-1]);
}

/*===================================================================
  damp_create_playlist(playlist_filename)

  Desc   : Creates the DAMP playlist
  Inputs : playlist_filename - The filename of the playlist
  =================================================================*/

void damp_create_playlist(char *playlist_filename)
{
   char text_line[512];
   FILE *fp;
   int x;
   int non_existent = 0;
   int type_of_pl = DAMP_PLAYLIST_M3U;
   char playlist_path[256];
   int p,q,flag;

   if(playlist_filename == NULL)
   {
      printf("damp_create_playlist(): **ERROR** playlist_filename == NULL\n");
      exit(-1);
   }

   for(p=0;p<256;p++)
      playlist_path[p]=0;

   if(!using_graphics && !damp_using_gui) printf("Reading playlist from %s ...\n", playlist_filename);

   /* Open the file */
   fp = fopen( playlist_filename , "r" );

   /* Die if we couldn't open it */
   if( fp == NULL)
   {
      if(!damp_fnfnoquit)
      {
         printf("\n   damp_create_playlist(): Cannot open playlist '%s'\n\n", playlist_filename);
         exit(-1);
      } else
      {
         if(!using_graphics && !damp_using_gui)
            printf("Not found, but ignoring due to -fnfnoquit option.\n");
         return;
      }
   }

   /* Decide what type it is */
   if (stricmp(get_extension(playlist_filename),"M3U")==0)
      type_of_pl = DAMP_PLAYLIST_M3U;
   if (stricmp(get_extension(playlist_filename),"PLS")==0)
      type_of_pl = DAMP_PLAYLIST_PLS;

   /* Read the file... */

   if( damp_developer )
      printf("Determining playlist_path\n");

   p=strlen(playlist_filename);
   p=p-strlen(get_filename(playlist_filename));
   p=p-1;

   if ( damp_developer )
      printf("p = %d. Attempting strncpy()\n",p);

   if(p>0)
   {
      strncpy(playlist_path,playlist_filename,p);
      put_backslash(playlist_path);
   }
   else
      sprintf(playlist_path,"%c",0);

   if( damp_developer )
      printf("Playlist path is %s\n",playlist_path);

   do
   {
      /* Read one line */
      if(damp_playlist_read_line(fp, text_line))
      {
         /* Strip the File1= from Sonique PLS files */
         if(type_of_pl == DAMP_PLAYLIST_PLS)
         {
            int eq=0;
            while(eq<strlen(text_line) && text_line[eq]!='=')
               eq++;

            if(text_line[eq]=='=')
               sprintf(text_line,"%s",&text_line[eq+1]);
         }

         /* Don't bother if it's not an MP3 or MP2 */
         /* But add it to the metaplaylist if it's a playlist */
         if(stricmp(get_extension(text_line),"MP3") == 0
         || stricmp(get_extension(text_line),"M3U") == 0
         || stricmp(get_extension(text_line),"PLS") == 0
         || stricmp(get_extension(text_line),"MP2") == 0)
         {   
            /* Deal with relative path */
            if(text_line[0] == '\\' && playlist_filename[1] == ':')
            {
               for(x=255;x > 1; x--)
                  text_line[x] = text_line[x-2];
   
               text_line[0] = playlist_filename[0];
               text_line[1] = playlist_filename[1];
            }

            /* deal with no path */
            if(text_line[1] != ':')
            {
               char temp_string[512];
               int tsc;
               for(tsc=0;tsc<512;tsc++)
                  temp_string[tsc] = 0;

               if(strlen(playlist_path) > 0)
                  sprintf(temp_string,"%s",playlist_path);

               strcat(temp_string,text_line);
               sprintf(text_line,"%s",temp_string);

               if( damp_developer )
               {
                  printf("Path added: %s\n",text_line);
               }
            }

            /* Skip it if it doesn't exist */

            if(!exists(text_line))
            {
               if( damp_developer )
               {
                  printf("\ndamp_create_playlist():\n");
                  printf("  Skipping non-existent file: %s\n",text_line);
               }

               non_existent++;
            }
            else
            {
               if(stricmp(get_extension(text_line),"MP3") == 0 || stricmp(get_extension(text_line),"MP2")==0)
                  damp_playlist_add_entry(text_line);
               else
               {
                  if(damp_developer) printf("Adding metaplaylist %s from playlist\n",text_line);
                  damp_metaplaylist_add_entry(text_line);
               }
            }
         }
      }
      else
        break;
   }while(fp != NULL);

   if(!using_graphics && !damp_using_gui)
      printf("Read %d entries.\n\n",damp_playlist_size);

   if(non_existent != 0)
      if(!using_graphics && !damp_using_gui)
         printf("HELPFUL NOTE: %d of the files in the playlist file do not exist!\n\n",non_existent);

   damp_playlist_num_to_play = damp_playlist_size;
   damp_playlist_entry = -1;

   /* randomize damp_playlist_order */
   for(x=0;x<damp_playlist_size;x++)
   {
      do
      {
         p = rand()%damp_playlist_size;
         flag = FALSE;
         for(q=0; q<x; q++)
         {
            if(damp_playlist_order[q] == p)
            {
               flag = TRUE;
               break;
            }
         }
      } while (flag);
      damp_playlist_order[x] = p;
   }

   sprintf(damp_playlist_filename,"%s",playlist_filename);
   sprintf(damp_playlist_filename_short,"%s",get_filename(playlist_filename));
}

/*===================================================================
  damp_get_next_playlist_filename()

  Desc   : Return the next filename to play
  Outputs: The filename to play, or NULL if there are no files
           left to play.
  =================================================================*/

char *damp_get_next_playlist_filename(void)
{
   int temp;

   if( damp_developer )
   {
      printf("\ndamp_get_next_playlist_filename():\n");
      printf("  damp_playlist_num_played  = %d\n",damp_playlist_num_played);
      printf("  damp_playlist_num_to_play = %d\n",damp_playlist_num_to_play);
      printf("  damp_playlist_entry       = %d\n",damp_playlist_entry);
   }

   if(damp_playlist_entry == -1)
      damp_playlist_entry += damp_playlist_num_to_skip;

//   if ( damp_playlist_num_played >= damp_playlist_num_to_play || damp_playlist_entry>=damp_playlist_num_to_play-1)
   if ( damp_playlist_entry>=damp_playlist_num_to_play-1)
   {
      if ( damp_playlist_continuous )
      {
         damp_playlist_entry = -1;
         damp_playlist_num_played = 0;
         if(damp_metaplaylist_size > 0)
            util_next_playlist();
      } else
      {
         if( damp_developer )
         {
            printf("\ndamp_get_next_playlist_filename():\n");
            printf("  end of playlist reached.\n");
         }
   
         return NULL;
      }
   }

   damp_playlist_entry++;
   temp = damp_playlist_entry;

   if ( damp_developer )
      printf("\ndamp_get_next_playlist_filename(): temp = %d\n",temp);

   if ( damp_playlist_random )
      temp = damp_playlist_order[temp];

   damp_playlist_num_played++;
   damp_playlist[temp].been_played = TRUE;
   return damp_playlist[temp].filename;
}

// selects a given track from the playlist.
char *damp_playlist_select_track(int num)
{
   int temp;

   damp_playlist_entry = num;
   if ( damp_playlist_random )
      temp = damp_playlist_order[damp_playlist_entry];
   else
      temp = damp_playlist_entry;

   if(!damp_playlist[temp].been_played)
      damp_playlist_num_played++;

   damp_playlist[temp].been_played = TRUE;
   return damp_playlist[temp].filename;
}

//=======================================================================
// damp_playlist_free()
//
// Frees the memory allocated to the playlist and resets necessary
// variables
//=======================================================================

void damp_playlist_free(void)
{
   if(damp_playlist != NULL)
      free(damp_playlist);

   if(damp_playlist_order != NULL)
      free(damp_playlist_order);

   damp_playlist = NULL;
   damp_playlist_order = NULL;

   damp_playlist_num_played=0;
   damp_playlist_num_to_play = 0;
   damp_playlist_entry = -1;
   damp_playlist_size = 0;
}

//=======================================================================
// damp_metaplaylist_add_entry(filename)
//
// Adds a new playlist to the metaplaylist
//=======================================================================

void damp_metaplaylist_add_entry(char *filename)
{
   damp_metaplaylist_size++;

   /* Reserve some memory for the metaplaylist_order entry */
   if ( damp_metaplaylist_order == NULL )
      damp_metaplaylist_order = malloc(sizeof(int));
   else
      damp_metaplaylist_order = realloc(damp_metaplaylist_order, sizeof(int)*(damp_metaplaylist_size+1));

   /* Die if we run out of memory */
   if ( damp_metaplaylist_order == NULL )
   {
      printf("\n   damp_metaplaylist_add_entry(): Out of memory when allocating damp_metaplaylist_order\n\n");
      exit(-1);
   }

   /* Reserve some memory for this entry */
   if ( damp_metaplaylist == NULL )
      damp_metaplaylist = malloc(sizeof(DAMP_PLAYLIST));
   else
      damp_metaplaylist = realloc(damp_metaplaylist, sizeof(DAMP_PLAYLIST)*(damp_metaplaylist_size+1));

   /* Die if we run out of memory */
   if ( damp_metaplaylist == NULL )
   {
      printf("\n   damp_metaplaylist_add_entry(): Out of memory when allocating damp_metaplaylist\n\n");
      exit(-1);
   }

   /* Set up this entry */
   sprintf(damp_metaplaylist[damp_metaplaylist_size-1].filename,"%s",filename);
   damp_metaplaylist[damp_metaplaylist_size-1].been_played = FALSE;
   damp_metaplaylist_order[damp_metaplaylist_size-1] = damp_metaplaylist_size-1;
}

//=======================================================================
// damp_metaplaylist_randomize()
//
// Makes the metaplaylist order random
//=======================================================================
void damp_metaplaylist_randomize(void)
{
   int x,p,q,flag;

   for(x=0;x<damp_metaplaylist_size;x++)
   {
      do
      {
         p = rand()%damp_metaplaylist_size;
         flag = FALSE;
         for(q=0; q<x; q++)
         {
            if(damp_metaplaylist_order[q] == p)
            {
               flag = TRUE;
               break;
            }
         }
      } while (flag);
      damp_metaplaylist_order[x] = p;
   }
}

//=======================================================================
// damp_metaplaylist_free()
//
// Frees the memory allocated to the metaplaylist
//=======================================================================

void damp_metaplaylist_free(void)
{
   if( damp_metaplaylist != NULL )
      free(damp_metaplaylist);

   if( damp_metaplaylist_order != NULL )
      free(damp_metaplaylist_order);
}

