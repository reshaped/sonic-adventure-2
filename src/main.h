/* the main loop: the game's start, its modes, and its exit to the browser
 * or the boot ROM */
#ifndef MAIN_H
#define MAIN_H
#include <ninja.h>

/* the values of game_state.mode, which game_mode_step runs. a mode that
 * starts a stage or a program goes on to its _RUN mode. that mode steps it
 * once a frame until it ends. a program is a PRS file read into STAGE_SLOT */
enum game_mode {
    /* check the memory cards, at the game's start. a failed check goes on
     * to GAME_MODE_CARD_WARNING, and a passed one runs GAME_MODE_START in
     * the same frame */
    GAME_MODE_BOOT = 0,
    /* go on to GAME_MODE_TO_MENU, after the card check or the card warning.
     * while game_state.display_pending is 1, go straight to GAME_MODE_MENU
     * instead, and clear skip_menus so that the menus open */
    GAME_MODE_START = 1,
    GAME_MODE_TO_MENU = 2, /* wait a frame, then go on to GAME_MODE_MENU */
    /* start the current stage with no rounds won, outside the relay */
    GAME_MODE_STAGE = 4,
    GAME_MODE_STAGE_RUN = 5,
    /* GAME_MODE_STAGE, but keeping the rounds won and the relay */
    GAME_MODE_STAGE_AGAIN = 6,
    GAME_MODE_EVENT = 7, /* EVENT.PRS, then game_state.next_mode */
    GAME_MODE_EVENT_RUN = 8,
    /* start the current stage on MISSION_FIRST, with no rounds won. its
     * end goes on to GAME_MODE_TO_MENU, and clears game_state.demo unless
     * it is a reset */
    GAME_MODE_DEMO = 9,
    GAME_MODE_DEMO_RUN = 10,
    GAME_MODE_SEQUENCE = 12, /* sequence_apply_step sets the next mode */
    /* ADVERTISE.PRS, the title and the menus, then game_state.next_mode */
    GAME_MODE_MENU = 13,
    GAME_MODE_MENU_RUN = 14,
    GAME_MODE_ENDING = 15, /* ENDING.PRS, then game_state.next_mode */
    GAME_MODE_ENDING_RUN = 16,
    GAME_MODE_EMBLEM = 17, /* EMBLEMGET.PRS, then game_state.resume_mode */
    GAME_MODE_EMBLEM_RUN = 18,
    GAME_MODE_CARD_WARNING = 19, /* MCWARN.PRS, then GAME_MODE_START */
    GAME_MODE_CARD_WARNING_RUN = 20,
    /* OTHERPRINT.PRS, then game_state.resume_mode */
    GAME_MODE_OTHER_PRINT = 21,
    GAME_MODE_OTHER_PRINT_RUN = 22
};

/* what stage_step and a program's step return */
enum overlay_result {
    /* the pads asked for a reset. game_mode_step goes back to the menus, but
     * ends the game from a demo, the card warning and OTHERPRINT.PRS. it ends
     * the game from an event too while game_state.demo is DEMO_ON */
    OVERLAY_RESET = -2,
    /* from the menus, leave the game for the browser. from a stage, go back
     * to the menus. from any other program, end as OVERLAY_DONE does */
    OVERLAY_BACK = -1,
    OVERLAY_RUNNING = 0,
    OVERLAY_DONE = 1
};

/* Ninja's texture memory list, of TEXTURE_COUNT entries, which
 * display_reset_full gives njInitTexture. it is null until system_boot_init
 * allocates it, and again once system_shutdown frees it */
#define TEXTURE_COUNT 2048
extern NJS_TEXMEMLIST *texmemlist;
/* the camera and the screen that display_reset_full sets Ninja up with */
extern NJS_CAMERA camera;
extern NJS_SCREEN screen;
/* the fog table display_reset_full gives Ninja. a stage's load fills it
 * again for the stage's fog */
extern NJS_FOG_TABLE fog_table;
/* the menu at which ADVERTISE.PRS's menus resume, one of its MENU_RESUME_
 * values, or 0 for none. GAME_MODE_MENU skips the menus only while it is 0. */
extern int menu_resume_target;

/* have main end each frame with wait. null gives frame_wait back */
void game_set_frame_wait(void (*wait)(void));
/* start the system, the memory cards and the fonts, and have
 * loader_note_error take the disc's errors. game_state.mode starts at
 * GAME_MODE_BOOT */
void game_init(void);
/* run game_state.mode for one frame, then draw the fade, fill the memory
 * cards' screens and poll for the network information. it returns -1 where
 * the game is to end, and 0 otherwise */
int game_mode_step(void);
/* free the fonts, release the textures, and end the sound and the system */
void game_shutdown(void);
/* release the texture sets and end Ninja's textures. display_reset_full
 * starts Ninja's textures again */
void game_release_textures(void);
/* set Ninja up as every stage and program starts from. it resets the
 * matrix stack, the screen, the camera and the texture list. it sets the
 * drawing modes, the lights and the fog, and seeds rand with stage.h's
 * RAND_SEED */
void display_reset_full(void);

/* DISPLAY_PAL's mode: 640 by 480, stretched over PAL's taller picture.
 * system_boot_init and ADVERTISE.PRS's change of display set it */
#define RESOLUTION_PAL                                                         \
    (NJD_DSP_PAL | NJD_DSP_PSINTER | NJD_DSP_640x480_EXT |                     \
     NJD_PALEXT_HEIGHT_RATIO_1_133)
/* the words of njInitVertexBuffer's lists that system_boot_init gives. a
 * change of display in ADVERTISE.PRS or in an event's movie gives the same.
 * the opaque list's -1 gives it no buffer, so Kamui runs with its 2V
 * latency */
#define OPAQUE_WORDS (-1)
#define OPAQUE_MODIFIER_WORDS 0x8000
#define TRANSLUCENT_WORDS 0x1c000
#define TRANSLUCENT_MODIFIER_WORDS 0x8000
#define PUNCH_THROUGH_WORDS 0x4000
/* the debug print's size, in pixels. display_reset_full sets it, as does a
 * change of display in ADVERTISE.PRS or in an event's movie */
#define PRINT_SIZE 16
/* start the system, Ninja, the sound and the pads. set game_state's cable,
 * display, languages and sound mode too. a VGA cable takes DISPLAY_VGA. any
 * other takes DISPLAY_PAL where the ntUtl flag keeps it, and DISPLAY_NTSC
 * otherwise. a PAL cable with no display kept takes DISPLAY_PAL too, and
 * sets display_pending */
void system_boot_init(void);
/* stop the sound and ntUtl, and free the texts, the line batches and
 * texmemlist */
void system_shutdown(void);
/* send the game through a screen before the mode just set, where one is
 * due. while game_state.edition is EDITION_TENTO, a two-player game shows
 * OTHERPRINT.PRS's OTHER_PRINT_ENDING, then the menus in place of that mode.
 * under any other edition, the emblem screen comes first where save_data's
 * count of emblems differs from game_state's. it never comes in sequence 15 */
void game_mode_divert(void);
/* set game_state.network_info_found once ntInfIsNetworkInfo finds the
 * console's network information. it asks only in the game's first steps */
void network_info_poll(void);

#endif
