/* beautify.c

   Filename beautification routine for DAMP.
   By Matt Craven.

   (c)2000 Hedgehog Software
*/

#include <string.h>

/*===================================================================
  beautify(txt)

  Beautifies the given string.
  Returns the pointer to txt
  =================================================================*/

char *beautify(char *txt)
{
   int n;

      /* Strip the extension */
      txt[strlen(txt)-4] = 0;

      /* Convert _ to ' ' and make all words start with caps */
      for(n=0;n<strlen(txt);n++)
      {
         if(txt[n] == '_')
            txt[n] = ' ';

         if(n>0)
         {
            if(txt[n-1] == ' ')
            {
               if(txt[n] >= 'a' && txt[n] <= 'z')
               {
                  txt[n] = (txt[n] - 'a') + 'A';
               }
            } else
            {
               if((txt[n-1] >= 'A' && txt[n-1] <= 'Z')
               || (txt[n-1] >= 'a' && txt[n-1] <= 'z'))
               {
                  if(txt[n] >= 'A' && txt[n] <= 'Z')
                  {
                     txt[n] = (txt[n] - 'A') + 'a';
                  }
               }
            }
         }
      }

   return txt;
}

