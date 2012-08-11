/* id3read.c

   ID3 reading code
   By Matt Craven
*/

/*======= i n c l u d e s =========================================*/

#include <stdio.h>
#include <string.h>

/*======= d a m p   i n c l u d e s ===============================*/

#include "id3read.h"

/*======= g l o b a l   v a r i a b l e s =========================*/

/* ID3 genre array */
unsigned char *id3_genre_string[ID3_GENRE_MAX] = {
              "Blues", "Classic Rock", "Country",
              "Dance", "Disco", "Funk", "Grunge", "Hip-Hop", "Jazz",
              "Metal", "New Age", "Oldies", "Other", "Pop", "R&B", "Rap",
              "Reggae", "Rock", "Techno", "Industrial", "Alternative",
              "Ska", "Death Metal", "Pranks", "Soundtrack", "Euro-Techno",
              "Ambient", "Trip-Hop", "Vocal", "Jazz+Funk", "Fusion",
              "Trance", "Classical", "Instrumental", "Acid", "House", "Game",
              "Sound Clip", "Gospel", "Noise", "Alternative Rock", "Bass",
              "Soul", "Punk", "Space", "Meditative", "Instrumental Pop",
              "Instrumental Rock", "Ethnic", "Gothic", "Darkwave",
              "Techno-Industrial", "Electronic", "Pop-Folk", "Eurodance",
              "Dream", "Southern Rock", "Comedy", "Cult", "Gangsta", "Top 40",
              "Christian Rap", "Pop/Funk", "Jungle", "Native US", "Cabaret",
              "New Wave", "Psychadelic", "Rave", "Showtunes", "Trailer",
              "Lo-Fi", "Tribal", "Acid Punk", "Acid Jazz", "Polka", "Retro",
              "Musical", "Rock & Roll", "Hard Rock",
              /* now the additional winamp ones */
              "Folk", "Folk-Rock", "National Folk", "Swing", "Fast Fusion",
              "Bebop", "Latin", "Revival", "Celtic", "Bluegrass",
              "Avantgarde", "Gothic Rock", "Progressive Rock",
              "Psychadelic Rock", "Symphonic Rock", "Slow Rock", "Big Band",
              "Chorus", "Easy Listening", "Acoustic", "Humour", "Speech",
              "Chanson", "Opera", "Chamber Music", "Sonata", "Symphony",
              "Booty Bass", "Primus", "Porn Groove", "Satire",
              "Slow Jam", "Club", "Tabgo", "Samba", "Folklore",
              "Ballad", "Power Ballad", "Ryhtmic Soul", "Freestyle", "Duet",
              "Punk Rock", "Drum Solo","Acapella", "Euro-House",
              "Dance Hall", "Goa", "Drum & Bass", "Club-House", "Hardcore",
              "Terror", "Indie", "BritPop", "Negerpunk", "Polsk Punk",
              "Beat", "Christian Gangsta Rap", "Heavy Metal", "Black Metal",
              "Crossover", "Contemporary Christian", "Christian Rock",
              "Merengue", "Salsa", "Trash Metal", "Anime", "Jpop",
              "Synthpop" };

/*===================================================================
  get_mp3_info(filename, infotag)

  This gets the ID3 info from the given file and stores it in infotag
  It returns 0 if there was no tag, non-zero otherwise
  =================================================================*/

int get_mp3_info(char *filename, ID3_TAG *infotag)
{
    char inbuf[256]; /* Input buffer */
    FILE *fp;        /* File pointer */
    int tagflag;     /* Was there a tag? */
    int x;           /* Loop counter */

    /* Clear the tag */
    for(x=0;x<30;x++) infotag->title[x] = ' ';
    for(x=0;x<30;x++) infotag->artist[x] = ' ';
    for(x=0;x<30;x++) infotag->album[x] = ' ';
    for(x=0;x<4;x++) infotag->year[x] = ' ';
    for(x=0;x<29;x++) infotag->comment[x] = ' ';

    /* Open the file */
    fp = fopen(filename,"rb");
    if(fp==NULL)
    {
      /* If there was an error opening the file, just say there was
         no tag */

      return 0;
    }

    /* look for ID tag (128 bytes from the end of the file) */

    if(fseek(fp,-128,SEEK_END))
    {
      /* If that fails, the file was < 128 bytes, so it can't be tagged */
      fclose(fp);
      return(0);
    }

    /* Read the (potential) ID3 tag */

    fread(inbuf,sizeof(char),128,fp);
    inbuf[129]=0;
    fclose(fp);

    /* check there actually is a tag: */

    tagflag=0;
    if( (inbuf[0]=='T' || inbuf[0]=='t')
    &&  (inbuf[1]=='A' || inbuf[1]=='a')
    &&  (inbuf[2]=='G' || inbuf[2]=='g'))
      tagflag=1;

    if(!tagflag)
    {
      /* There was no tag... */
      return(0);
    }

    /* Put the TAG info into the infotag fields */

    for(x=0;x<30;x++) infotag->title[x] = inbuf[3+x];
    for(x=0;x<30;x++) infotag->artist[x] = inbuf[33+x];
    for(x=0;x<30;x++) infotag->album[x] = inbuf[63+x];
    for(x=0;x<4;x++) infotag->year[x] = inbuf[93+x];
    for(x=0;x<29;x++) infotag->comment[x] = inbuf[97+x];
    infotag->track = inbuf[126];
    infotag->genre = inbuf[127];

    /* null-terminate the strings */

    infotag->title[30]=0;
    infotag->artist[30]=0;
    infotag->album[30]=0;
    infotag->year[4]=0;
    infotag->comment[29]=0;

    /* There was a tag, so return non-zero */

    return 1;
}
