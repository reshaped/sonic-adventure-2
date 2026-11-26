/* The SH4ZAM splash shown by OTHERPRINT.PRS in the Dreamcast GCC build.
 * The 384x256 artwork is stored as a 48-color RLE bitmap inside the overlay
 * and unpacked into a temporary 512x256 RGB565 PVM archive while the splash
 * is on screen. */
#ifndef ENGINE_DC_SH4ZAM_SPLASH_H
#define ENGINE_DC_SH4ZAM_SPLASH_H

#include <sg_xpt.h>

/* Byte size of the unpacked 512x256 RGB565 PVM archive. */
Uint32 sh4zam_splash_pvm_size(void);
/* Build the PVM archive into out (which must hold at least
 * sh4zam_splash_pvm_size() bytes). Returns FALSE on invalid arguments or a
 * truncated stream. */
Bool sh4zam_splash_build_pvm(Uint8 *out, Uint32 capacity);

/* Decode and upload the splash texture into VRAM. Returns TRUE when ready to
 * draw, or FALSE if allocation or texture upload fails. */
Bool sh4zam_splash_load(void);
/* Draw the splash on the 640x480 screen, above its middle, under the current
 * screen_draw_begin() fade level. */
void sh4zam_splash_draw(void);
/* Release the splash texture from VRAM if loaded. Safe to call repeatedly. */
void sh4zam_splash_release(void);

#endif
