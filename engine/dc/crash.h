/* the Dreamcast's crash screen: where the game stopped, and why. it draws
 * straight into the frame buffer the screen shows, and calls nothing of Ninja's
 * or the game's. crash_report.h gives its lines.
 *
 * an address error, TRAPA or an illegal instruction draws the screen and stops
 * the game there. Shinobi's own handler for each puts back the stopped code's
 * registers and jumps to H'AC000010. what a retail console does there is not
 * established, and it may resume the game. so the screen may stop a game that a
 * retail console runs on. the game raises one of them itself, by
 * ocean_model_tint's trapa(0). it traps where the word after a model's first
 * chunk isn't NJD_CE. port_stop draws the screen for a failure the game names,
 * and stops there too.
 *
 * the vblank handler reports an unchanged scanout base after 1,800 callbacks,
 * then every 30 callbacks. The game keeps running. Masked interrupts or a
 * stalled earlier handler prevent the watch from sampling.
 *
 * Shinobi's stub pushes onto the stopped code's stack before the screen's
 * handler runs. SR.BL is set there, and turns a fault of the push into a reset.
 * so an exception whose R15 addresses no memory resets the console */
#ifndef DC_CRASH_H
#define DC_CRASH_H

#include <sg_xpt.h>

/* make the screen the handler of the exceptions above, and watch for a hang
 * from the vblank chain. call it after syHwInit2, whose syChainInit empties
 * every chain. Each call needs a crash_remove. If the chain pool is full, the
 * exception vectors still work and another install retries the watch. An
 * overlay must remove its copy before the slot takes another overlay. */
void crash_install(void);
/* undo one installation. The last removal leaves the vblank chain and
 * restores each vector still owned by this copy of the screen. */
void crash_remove(void);

/* draw the screen for a failure the game can't go on from. stop there, with
 * every interrupt masked. what names the failure in a few words, and number is
 * its code. name is what failed, or null */
void port_stop(const char *what, const char *name, Uint32 number)
    __attribute__((noreturn));

#endif
