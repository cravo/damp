/* keys.c

   Redefineable keyboard support for DAMP
   By Matt Craven
*/

/*======= i n c l u d e s =========================================*/

#include <stdio.h>
#include <string.h>
#include <allegro.h>

/*======= d a m p   i n c l u d e s ===============================*/

#include "keys.h"

/*======= p r o t o t y p e s =====================================*/

int _keys_translate(char *txt);



/*===================================================================
  keys_init()

  Initialises the keyboard setup, reading the mapping from DAMP.ini
  =================================================================*/

#define _KEYS_PROCESS(txt,key) \
   sprintf(action,get_config_string("[keyboard]",txt,"NONE")); \
   if(stricmp(action,"NONE") != 0) \
      keys_add(_keys_translate(action), key);

void keys_init(void)
{
   int n,k;
   char action[256];

   /* initialise the key table */

   for(n=0;n<KEYS_MAX;n++)
      for(k=0;k<KEYS_PER_ACTION;k++)
         key_table[n][k] = 0;

   /* read in all possible keys, and see if they have an action
      and if they have, add it to the key table */

   _KEYS_PROCESS("ESC",KEY_ESC);
   _KEYS_PROCESS("F1", KEY_F1);
   _KEYS_PROCESS("F2", KEY_F2);
   _KEYS_PROCESS("F3", KEY_F3);
   _KEYS_PROCESS("F4", KEY_F4);
   _KEYS_PROCESS("F5", KEY_F5);
   _KEYS_PROCESS("F6", KEY_F6);
   _KEYS_PROCESS("F7", KEY_F7);
   _KEYS_PROCESS("F8", KEY_F8);
   _KEYS_PROCESS("F9", KEY_F9);
   _KEYS_PROCESS("F10", KEY_F10);
   _KEYS_PROCESS("F11", KEY_F11);
   _KEYS_PROCESS("F12", KEY_F12);
   _KEYS_PROCESS("TILDE", KEY_TILDE);
   _KEYS_PROCESS("1", KEY_1);
   _KEYS_PROCESS("2", KEY_2);
   _KEYS_PROCESS("3", KEY_3);
   _KEYS_PROCESS("4", KEY_4);
   _KEYS_PROCESS("5", KEY_5);
   _KEYS_PROCESS("6", KEY_6);
   _KEYS_PROCESS("7", KEY_7);
   _KEYS_PROCESS("8", KEY_8);
   _KEYS_PROCESS("9", KEY_9);
   _KEYS_PROCESS("0", KEY_0);
   _KEYS_PROCESS("MINUS", KEY_MINUS);
   _KEYS_PROCESS("EQUALS", KEY_EQUALS);
   _KEYS_PROCESS("BACKSPACE", KEY_BACKSPACE);
   _KEYS_PROCESS("TAB", KEY_TAB);
   _KEYS_PROCESS("Q", KEY_Q);
   _KEYS_PROCESS("W", KEY_W);
   _KEYS_PROCESS("E", KEY_E);
   _KEYS_PROCESS("R", KEY_R);
   _KEYS_PROCESS("T", KEY_T);
   _KEYS_PROCESS("Y", KEY_Y);
   _KEYS_PROCESS("U", KEY_U);
   _KEYS_PROCESS("I", KEY_I);
   _KEYS_PROCESS("O", KEY_O);
   _KEYS_PROCESS("P", KEY_P);
   _KEYS_PROCESS("OPENBRACE", KEY_OPENBRACE);  // "["
   _KEYS_PROCESS("CLOSEBRACE", KEY_CLOSEBRACE); // "]"
   _KEYS_PROCESS("A", KEY_A);
   _KEYS_PROCESS("S", KEY_S);
   _KEYS_PROCESS("D", KEY_D);
   _KEYS_PROCESS("F", KEY_F);
   _KEYS_PROCESS("G", KEY_G);
   _KEYS_PROCESS("H", KEY_H);
   _KEYS_PROCESS("J", KEY_J);
   _KEYS_PROCESS("K", KEY_K);
   _KEYS_PROCESS("L", KEY_L);
   _KEYS_PROCESS("SEMICOLON", KEY_COLON);
   _KEYS_PROCESS("APOSTROPHE", KEY_QUOTE);
   _KEYS_PROCESS("ENTER", KEY_ENTER);
   _KEYS_PROCESS("LEFT_SHIFT", KEY_LSHIFT);
   _KEYS_PROCESS("BACKSLASH", KEY_BACKSLASH);
   _KEYS_PROCESS("Z", KEY_Z);
   _KEYS_PROCESS("X", KEY_X);
   _KEYS_PROCESS("C", KEY_C);
   _KEYS_PROCESS("V", KEY_V);
   _KEYS_PROCESS("B", KEY_B);
   _KEYS_PROCESS("N", KEY_N);
   _KEYS_PROCESS("M", KEY_M);
   _KEYS_PROCESS("COMMA", KEY_COMMA);
   _KEYS_PROCESS("FULLSTOP", KEY_STOP);
   _KEYS_PROCESS("SLASH", KEY_SLASH);
   _KEYS_PROCESS("RIGHT_SHIFT", KEY_RSHIFT);
   _KEYS_PROCESS("SPACE", KEY_SPACE);
   _KEYS_PROCESS("PRINTSCREEN", KEY_PRTSCR);
   _KEYS_PROCESS("SCROLL_LOCK", KEY_SCRLOCK);
   _KEYS_PROCESS("PAUSE", KEY_PAUSE);
   _KEYS_PROCESS("INSERT", KEY_INSERT);
   _KEYS_PROCESS("HOME", KEY_HOME);
   _KEYS_PROCESS("PAGE_UP", KEY_PGUP);
   _KEYS_PROCESS("DELETE", KEY_DEL);
   _KEYS_PROCESS("END", KEY_END);
   _KEYS_PROCESS("PAGE_DOWN", KEY_PGDN);
   _KEYS_PROCESS("CURSOR_UP", KEY_UP);
   _KEYS_PROCESS("CURSOR_LEFT", KEY_LEFT);
   _KEYS_PROCESS("CURSOR_DOWN", KEY_DOWN);
   _KEYS_PROCESS("CURSOR_RIGHT", KEY_RIGHT);
   _KEYS_PROCESS("NUM_LOCK", KEY_NUMLOCK);
   _KEYS_PROCESS("SLASH_PAD", KEY_SLASH_PAD);
   _KEYS_PROCESS("ASTERISK", KEY_ASTERISK);
   _KEYS_PROCESS("MINUS_PAD", KEY_MINUS_PAD);
   _KEYS_PROCESS("7_PAD", KEY_7_PAD);
   _KEYS_PROCESS("8_PAD", KEY_8_PAD);
   _KEYS_PROCESS("9_PAD", KEY_9_PAD);
   _KEYS_PROCESS("4_PAD", KEY_4_PAD);
   _KEYS_PROCESS("5_PAD", KEY_5_PAD);
   _KEYS_PROCESS("6_PAD", KEY_6_PAD);
   _KEYS_PROCESS("1_PAD", KEY_1_PAD);
   _KEYS_PROCESS("2_PAD", KEY_2_PAD);
   _KEYS_PROCESS("3_PAD", KEY_3_PAD);
   _KEYS_PROCESS("PLUS_PAD", KEY_PLUS_PAD);
   _KEYS_PROCESS("ENTER_PAD", KEY_ENTER_PAD);
   _KEYS_PROCESS("0_PAD", KEY_0_PAD);
   _KEYS_PROCESS("DEL_PAD", KEY_DEL_PAD);

   atexit(keys_exit);
}

/*===================================================================
  keys_exit()

  Shuts down the redefineable keys system.  You don't normally need
  to call this as it's added as an atexit() routine by keys_init()
  =================================================================*/

void keys_exit(void)
{
   /* currently here for completeness - no action required */
}

/*===================================================================
  keys_add(type, key)

  Adds the given key to the given action type
  =================================================================*/

void keys_add(int type, int key)
{
   int n;

   n = 0;

   while(key_table[type][n] != 0)
   {
      n++;

      if(n==KEYS_PER_ACTION)
      {
         printf("FATAL ERROR: You have tried to assign more than %d keys to a particular action!\n",KEYS_PER_ACTION);
         exit(-1);
      }
   }

   key_table[type][n] = key;
}

/*===================================================================
  _keys_translate(txt)

  Translates the text-based representation of an action to the
  numerical KEYS_* constant from keys.h
  =================================================================*/

#define _KEYS_TRANS(ktxt,map) \
   if(stricmp(txt,ktxt)==0) return map;

int _keys_translate(char *txt)
{
   _KEYS_TRANS("QUIT",KEYS_QUIT);
   _KEYS_TRANS("SCREENSHOT", KEYS_SCREENSHOT);
   _KEYS_TRANS("PREVIOUS_TRACK", KEYS_PREVIOUS_TRACK);
   _KEYS_TRANS("NEXT_TRACK", KEYS_NEXT_TRACK);
   _KEYS_TRANS("PREVIOUS_N_TRACKS", KEYS_PREVIOUS_N_TRACKS);
   _KEYS_TRANS("NEXT_N_TRACKS", KEYS_NEXT_N_TRACKS);
   _KEYS_TRANS("RANDOM_TOGGLE", KEYS_RANDOM_TOGGLE);
   _KEYS_TRANS("SCROLL_TRACK_TITLE", KEYS_SCROLL_TRACK_TITLE);
   _KEYS_TRANS("SURROUND_TOGGLE", KEYS_SURROUND_TOGGLE);
   _KEYS_TRANS("VOLUME_UP", KEYS_VOLUME_UP);
   _KEYS_TRANS("VOLUME_DOWN", KEYS_VOLUME_DOWN);
   _KEYS_TRANS("GRAPHICS_TOGGLE", KEYS_GRAPHICS_TOGGLE);
   _KEYS_TRANS("PAUSE", KEYS_PAUSE);
   _KEYS_TRANS("PAD_0", KEYS_PAD_0);
   _KEYS_TRANS("PAD_1", KEYS_PAD_1);
   _KEYS_TRANS("PAD_2", KEYS_PAD_2);
   _KEYS_TRANS("PAD_3", KEYS_PAD_3);
   _KEYS_TRANS("PAD_4", KEYS_PAD_4);
   _KEYS_TRANS("PAD_5", KEYS_PAD_5);
   _KEYS_TRANS("PAD_6", KEYS_PAD_6);
   _KEYS_TRANS("PAD_7", KEYS_PAD_7);
   _KEYS_TRANS("PAD_8", KEYS_PAD_8);
   _KEYS_TRANS("PAD_9", KEYS_PAD_9);
   _KEYS_TRANS("PREVIOUS_PLAYLIST", KEYS_PREVIOUS_PLAYLIST);
   _KEYS_TRANS("NEXT_PLAYLIST", KEYS_NEXT_PLAYLIST);
   _KEYS_TRANS("REWIND", KEYS_REWIND);
   _KEYS_TRANS("FAST_FORWARD", KEYS_FAST_FORWARD);
   _KEYS_TRANS("CONTINUOUS_TOGGLE", KEYS_CONTINUOUS_TOGGLE);
   _KEYS_TRANS("SYNC_TOGGLE", KEYS_SYNC_TOGGLE);
   _KEYS_TRANS("TIME_REMAIN_TOGGLE", KEYS_TIME_REMAIN_TOGGLE);
   _KEYS_TRANS("HELP", KEYS_HELP);

   printf("_keys_translate(): Unrecognized action '%s'\n",txt);
   exit(-1);
}
