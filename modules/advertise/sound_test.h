/* the sound test, MENU_MODE_SOUND_TEST. it plays the songs that music_heard
 * says are heard */
#ifndef ADVERTISE_SOUND_TEST_H
#define ADVERTISE_SOUND_TEST_H
#include <sg_xpt.h>

/* start the sound test at its first step, select its textures, and fill its
 * lists of songs. it leaves menu_control.mode to the caller */
void menu_sound_test_start(void);
/* the sound test's step, under MENU_MODE_SOUND_TEST. as it goes back to the
 * options, it starts them and sets menu_control.mode to MENU_MODE_OPTIONS.
 * it returns FALSE: the sound test never ends the menus */
Bool menu_sound_test_step(void);

#endif
