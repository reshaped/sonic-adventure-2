/* the instruments a Chao learns, and the band it plays them in */
#ifndef CHAO_INSTRUMENTS_H
#define CHAO_INSTRUMENTS_H
#include <sg_xpt.h>
#include "chao/behavior.h"

/* each instrument's bit in what a Chao knows, as chao_knows_instrument takes
 * it */
enum chao_instrument {
    INSTRUMENT_BELL,
    INSTRUMENT_CASTANETS,
    INSTRUMENT_CYMBAL,
    INSTRUMENT_DRUM,
    INSTRUMENT_FLUTE,
    INSTRUMENT_MARACAS,
    INSTRUMENT_TRUMPET,
    INSTRUMENT_TAMBOURINE,
    CHAO_INSTRUMENTS
};
/* each instrument's behavior in a band, by its enum chao_instrument. each
 * behavior finishes once no band's field reaches the Chao */
extern chao_behavior_fn instrument_actions[CHAO_INSTRUMENTS];
/* bid to join a band whose field reaches the Chao. a Chao that knows an
 * instrument turns to the field's center and plays one. a Chao that knows
 * none turns to the band's starter, sits and listens */
void score_nearby_instrument(struct task *chao, Float *best);
/* start a band, and return whether the Chao started one. a Chao that knows no
 * instrument, or that a band's field reaches already, starts none. the Chao
 * plays one of the instruments it knows. the band's field reaches 20 units
 * around a point 8 units ahead of the Chao. it lasts 1800 to 4200 frames */
int chao_start_band(struct task *chao);
#endif
