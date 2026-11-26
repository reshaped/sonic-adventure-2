#ifndef EVENT_TEXTURE_H
#define EVENT_TEXTURE_H
#include <ninja.h>
#include <sg_xpt.h>

/* a texture chunk of a model. chunk_word is the halfword that holds its
 * texture's index, and texture the index the scene file gives it */
struct event_texture_use {
    Sint32 texture;
    Uint16 *chunk_word;
};

/* the count uses of model's texture chunks. a list of sets ends at one whose
 * model is null */
struct event_texture_set {
    NJS_CNK_OBJECT *model;
    Sint32 count;
    struct event_texture_use **uses;
};

/* point each chunk word that a use of texture in sets names at the texture
 * frame instead. the word keeps its filter mode, its NJD_FFM_MASK bits, and
 * frame takes the place of the rest. it returns the count of words it sets */
int event_texture_frame(struct event_texture_set *sets, Sint32 texture,
                        Sint32 frame);

#endif
