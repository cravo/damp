/* Lyrics3.c

   Routines for loading and displaying Lyrics3 Tags
   By Matt Craven - (c)2000 Hedgehog Software

   Lyrics3 Tags were devised by Petr Strnad and Alon Gingold.

   *** THIS CODE IS INCOMPLETE ***
*/

#include <stdio.h>
#include <string.h>
#include <malloc.h>

#include "damp.h"
#include "lyrics3.h"

unsigned long _lyrics3_count;   /* Number of lyrics lines */

void _lyrics3_v1_read(char *filename)
{
   printf("Version 1 of the Lyrics3 tag is currently unsupported.\n");
}

void _lyrics3_v2_read(char *filename)
{
   FILE *fp;
   char numbuf[7];
   int inumbuf;
   char header[12];
   char fieldname[4];
   char fieldsize[6];
   int ifieldsize;
   char *inbuf = NULL;
   int counter = 0;

   if(damp_developer)
      printf("Reading Lyrics3 v2.00 Tag\n");

   // Read the size of the data
   fp = fopen(filename,"rb");
   fseek(fp,-137-6,SEEK_END);
   fread(numbuf,sizeof(char),6,fp);
   numbuf[6]=0;
   inumbuf = atoi(numbuf);
   fclose(fp);

   // Go to start of tag and check its LYRICSBEGIN
   fp = fopen(filename,"rb");
   fseek(fp,-137-6-inumbuf,SEEK_END);
   fread(header,sizeof(char),11,fp);
   header[11]=0;

   if(stricmp(header,"LYRICSBEGIN") != 0)
   {
      printf("WARNING: Invalid Lyrics3 tag - No LYRICSBEGIN found.\n");

      fclose(fp);

      return;
   } else
   {
      do
      {
        /* Read fields */
        fread(fieldname,sizeof(char),3,fp);
        fieldname[4]=0;
        fread(fieldsize,sizeof(char),5,fp);
        fieldsize[6]=0;
        ifieldsize = atoi(fieldsize);

        inbuf = malloc(sizeof(char)*(ifieldsize+1));
        if(inbuf == NULL)
        {
          printf("Out of memory whilst reading Lyrics3 tag.\n");
          exit(-1);
        }

        fread(inbuf,sizeof(char),ifieldsize,fp);
        inbuf[ifieldsize] = 0;
     
        if(stricmp(fieldname,"IND") == 0)
        {
           if(inbuf[0] == 0)
              printf("There are no lyrics in the Lyrics3 tag.\n");
           if(inbuf[1] == 0)
              printf("There are no timestamps used in the Lyrics3 tag.\n");
        }
        else if(stricmp(fieldname,"LYR") == 0)
        {
           int n;
           for(n=0;n<ifieldsize;n++)
              printf("%c",inbuf[n]);
           printf("\n");
           //printf(inbuf);
        }
        else if(stricmp(fieldname,"INF") == 0)
        {
           printf("Information: %s\n",inbuf);
        }
        else if(stricmp(fieldname,"AUT") == 0)
        {
           printf("Author     : %s\n",inbuf);
        }
        else if(stricmp(fieldname,"EAL") == 0)
        {
           printf("Album Name : %s\n",inbuf);
        }
        else if(stricmp(fieldname,"EAR") == 0)
        {
           printf("Artist     : %s\n",inbuf);
        }
        else if(stricmp(fieldname,"ETT") == 0)
        {
           printf("Track Title: %s\n",inbuf);
        }
        else if(stricmp(fieldname,"IMG") == 0)
        {
           /* inbuf contains image filenames (BMP, JPG, GIF)
              image lines include filename, description, and timestamp,
              separated by нн (ASCII 124 x 2) - See the official spec
              for more details...
              currently not supported in DAMP */
        }

        free(inbuf);

        counter += ifieldsize;

      }while(counter < inumbuf);

      fclose(fp);
   }
}

/*===================================================================
  lyrics3_get_tag(filename)

  Looks for a Lyrics3 tag in the given file, and loads the lyrics if
  they exist.
  =================================================================*/

void lyrics3_get_tag(char *filename)
{
    FILE *fp;
    char inbuf[10];

    /* This code is unstable, so we'll just return and do nothing for
       now */

    return;

    /* Open the file */
    fp = fopen(filename,"rb");
    if(fp==NULL)
    {
//       printf("Error opening file: %s\n",filename);    
//       exit(-1);
      return;
    }

    // look for Lyrics3 end tag (137 bytes from the end of the file)
    if(fseek(fp,-137,SEEK_END))
    {
      fclose(fp);
      return;
    }
    // Read the end tag
    fread(inbuf,sizeof(char),9,fp);
    inbuf[9]=0;
    fclose(fp);

    _lyrics3_count = 0;

    if(stricmp(inbuf,"LYRICSEND") == 0)
       _lyrics3_v1_read(filename);
    else if(stricmp(inbuf,"LYRICS200") == 0)
       _lyrics3_v2_read(filename);
    else if(damp_developer)
       printf("File contains no Lyrics3 Tag\n");
}

/*===================================================================
  lyrics3_run()

  This routine sees if it's time to display some lyrics, and does
  so if necessary.
  =================================================================*/

void lyrics3_run(void)
{
   if(_lyrics3_count > 0)
   {
   }
}
