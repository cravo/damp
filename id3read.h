// Header file for reading ID3 tags

#ifndef __ID3READ_H__
#define __ID3READ_H__

#define ID3_GENRE_MAX 148

typedef struct {
   char title[31];
   char artist[31];
   char album[31];
   char year[5];
   char comment[30];
   char track;
   char genre;
}ID3_TAG;

ID3_TAG damp_id3_tag;

extern unsigned char *id3_genre_string[ID3_GENRE_MAX];

int get_mp3_info(char *filename, ID3_TAG *infotag);

#endif

