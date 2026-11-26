/* the port's toggles: the chord that sets them and the save's two bits that
 * keep them. pace.c ends each frame and splits it, readout.c draws the
 * readout and wide.c applies 16:9. make dc links this into 1ST_READ.BIN
 * after the image's sources */
#include <ninja.h>
#include <sg_xpt.h>
#include <shinobi.h>

#include "engine/dc/port.h"
#include "system/game_state.h"
#include "system/pad.h"

#define CHORD (PDD_DGT_TL | PDD_DGT_TR)
/* a trigger at rest, as the peripheral server reports it */
#define TRIGGER_UP 0

int port_toggles[TOGGLES];
int port_chosen[TOGGLES];

/* TRUE from the chord's press until L, R and the d-pad are all up, and the
 * first pad's buttons as port_pad last read them */
static int chord_latched;
static Uint32 chord_old;

void port_pad(void)
{
    static const Uint32 buttons[TOGGLES] = {PDD_DGT_KU, PDD_DGT_KR, PDD_DGT_KD};
    /* the server's own record, which pad_scan copies for the game */
    PDS_PERIPHERAL *pad = (PDS_PERIPHERAL *)pdGetPeripheral(PDD_PORT_A0);
    Uint32 fresh;
    int i;

    fresh = pad->on & ~chord_old;
    chord_old = pad->on;
    if ((pad->on & CHORD) == CHORD && (fresh & PAD_DIRECTIONS)) {
        chord_latched = TRUE;
        for (i = 0; i < TOGGLES; i++) {
            if (!(fresh & buttons[i])) {
                continue;
            }
            /* the readout steps through its pages rather than flip. its
             * toggle is on while a page shows */
            port_toggles[i] =
                i == TOGGLE_READOUT ? readout_step() : !port_toggles[i];
            port_chosen[i] = TRUE;
            readout_note(i);
        }
    } else if (!(pad->on & (CHORD | PAD_DIRECTIONS))) {
        chord_latched = FALSE;
    }
    if (chord_latched) {
        pad->on &= ~(CHORD | PAD_DIRECTIONS);
        pad->press &= ~(CHORD | PAD_DIRECTIONS);
        pad->l = pad->r = TRIGGER_UP;
    }
}

int port_blank_rate(void)
{
    return game_state.display == DISPLAY_PAL ? PAL_BLANKS : NTSC_BLANKS;
}

void port_store(Uint8 *flags)
{
    *flags &= (Uint8) ~(SAVE_WIDE | SAVE_FAST);
    if (port_toggles[TOGGLE_WIDE]) {
        *flags |= SAVE_WIDE;
    }
    if (port_toggles[TOGGLE_FAST]) {
        *flags |= SAVE_FAST;
    }
}

void port_load(Uint8 flags)
{
    if (!port_chosen[TOGGLE_WIDE]) {
        port_toggles[TOGGLE_WIDE] = (flags & SAVE_WIDE) != 0;
    }
    if (!port_chosen[TOGGLE_FAST]) {
        port_toggles[TOGGLE_FAST] = (flags & SAVE_FAST) != 0;
    }
}
