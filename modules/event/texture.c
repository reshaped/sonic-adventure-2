#include <ninja.h>
#include <sg_xpt.h>

#include "event/texture.h"

int event_texture_frame(struct event_texture_set *sets, Sint32 texture,
                        Sint32 frame)
{
    struct event_texture_use *use;
    Uint16 *word;
    int count = 0;
    int i;

    for (; sets->model; sets++) {
        for (i = 0; i < sets->count; i++) {
            if (sets->uses[i]->texture == texture) {
                /* @hack spelling: the assignment to use inside the
                 * expression, where a statement of its own would do */
                word = (use = sets->uses[i])->chunk_word;
                *word = (*use->chunk_word & NJD_FFM_MASK) + frame;
                count++;
            }
        }
    }
    return count;
}
