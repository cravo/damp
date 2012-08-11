/* gtext.h

   Header file for the graphics-mode "fake text mode" routines for DAMP.
   By Matt Craven.

   (c)2000 Hedgehog Software
*/

#ifndef __GTEXT_H__
#define __GTEXT_H__

int gtext_in_use;          /* Flag to say whether or not the
                              gtext system is being used */

int gtext_vu_max;          /* for falling peak on VU display */

void gtext_render(void);

#endif
