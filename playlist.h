/* playlist.h
   Header file for playlist functionality for DAMP.
*/

#ifndef __PLAYLIST_H__
#define __PLAYLIST_H__

#define DAMP_PLAYLIST_M3U 0
#define DAMP_PLAYLIST_PLS 1

/*###################################################################
  T Y P E D E F S
  #################################################################*/

typedef struct {
   char filename[512];        // Filename of the MP3
   int been_played;           // Has it been played? (for random mode)
   int total_time;            // Track total time
}DAMP_PLAYLIST;

/*###################################################################
  G L O B A L S
  #################################################################*/

extern DAMP_PLAYLIST *damp_playlist;   // We'll allocate this later
extern int *damp_playlist_order;       // and this...
extern int damp_playlist_size;            // Number of entries in the playlist
extern int damp_playlist_random;      // Play randomly?
extern int damp_playlist_entry;           // Current entry
extern int damp_playlist_num_played;      // Number of tracks we've played
extern int damp_playlist_num_to_play;     // How many tracks to play altogether
extern int damp_playlist_num_to_skip;     // How many tracks to skip
extern int damp_playlist_continuous;   // Loop back to start of plist when end reached?

extern DAMP_PLAYLIST *damp_metaplaylist;// for multi-playlist support
extern int *damp_metaplaylist_order;   // for random support
extern int damp_metaplaylist_size;        // Size of the metaplaylist
extern int damp_metaplaylist_random;  // Play randomly?
extern int damp_metaplaylist_entry;       // Current entry
extern int damp_metaplaylist_num_played;  // Number of playlists we've played

/*###################################################################
  P R O T O T Y P E S
  #################################################################*/

void damp_playlist_entry_compute_time(char *filename, DAMP_PLAYLIST *entry);
void damp_create_playlist(char *playlist_filename);
char *damp_get_next_playlist_filename(void);
void damp_playlist_free(void);
void damp_metaplaylist_add_entry(char *filename);
void damp_metaplaylist_randomize(void);
void damp_metaplaylist_free(void);
char *damp_playlist_select_track(int num);

#endif
