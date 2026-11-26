/* relocating Ninja data loaded from a file */
#include <ninja.h>
#include <sg_xpt.h>

#include "system/relocate.h"

/* the address relocate.h's rule tests: a pointer below it is an offset */
#define MEMORY_FIRST 0x0c000000

/* the factor count in a motion's inp_fn */
#define FACTORS_MASK 0xf
/* a factor's bytes in a node's record: the pointer to its keys, and their
 * count */
#define FACTOR_SIZE 8

void relocate_cnk_object(NJS_CNK_OBJECT *object, Uint32 base)
{
    NJS_CNK_MODEL *model;
    NJS_CNK_OBJECT *child;
    NJS_CNK_OBJECT *sibling;
    /* @hack spelling: first, a local for MEMORY_FIRST, where MEMORY_FIRST in
     * each test would do */
    Uint32 first;

    if (object == NULL) {
        return;
    }
    model = object->model;
    child = object->child;
    sibling = object->sibling;
    first = MEMORY_FIRST;
    if (model != NULL && (Uint32)model < first) {
        model = (NJS_CNK_MODEL *)((Uint32)model + base);
        object->model = model;
        if (model->vlist != NULL) {
            model->vlist = (Sint32 *)((Uint32)model->vlist + base);
        }
        if (model->plist != NULL) {
            model->plist = (Sint16 *)((Uint32)model->plist + base);
        }
    }
    if (child != NULL) {
        if ((Uint32)child < first) {
            child = (NJS_CNK_OBJECT *)((Uint32)child + base);
            object->child = child;
        }
        relocate_cnk_object(child, base);
    }
    if (sibling != NULL) {
        if ((Uint32)sibling < first) {
            sibling = (NJS_CNK_OBJECT *)((Uint32)sibling + base);
            object->sibling = sibling;
        }
        relocate_cnk_object(sibling, base);
    }
}

void relocate_motion(NJS_MOTION *motion, Uint32 base, int nodes)
{
    /* the record of the node in hand */
    Uint32 *mdata;
    /* @hack spelling: keys, a local for mdata, where mdata read in place
     * would do */
    Uint32 *keys;
    NJS_MKEY_P *key;
    int factors;
    int type;
    int i;
    /* the indexes, among a record's factors, of those whose keys point to
     * their data, or -1 for none. a factor's index counts the factors before
     * it: vert is NJD_MTYPE_VERT_4's, and norm NJD_MTYPE_NORM_5's.
     * past_camera, set only under NJD_MTYPE_TARGET_3, is the factor's past a
     * camera's, and counts the four bits it tests */
    int vert = -1;
    int norm = -1;
    int past_camera = -1;

    if (motion == NULL) {
        return;
    }
    mdata = motion->mdata;
    if (mdata == NULL) {
        return;
    }
    if ((Uint32)mdata >= MEMORY_FIRST) {
        return;
    }
    mdata = (Uint32 *)((Uint32)mdata + base);
    motion->mdata = mdata;
    factors = motion->inp_fn & FACTORS_MASK;
    type = motion->type;
    if (type & NJD_MTYPE_VERT_4) {
        vert = 0;
        /* @hack spelling: the first factor counted by = 1, here and in the
         * two counts below, where ++ would do */
        if (type & NJD_MTYPE_POS_0) {
            vert = 1;
        }
        if (type & NJD_MTYPE_ANG_1) {
            vert++;
        }
        if (type & NJD_MTYPE_SCL_2) {
            vert++;
        }
        if (type & NJD_MTYPE_VEC_3) {
            vert++;
        }
    }
    if (type & NJD_MTYPE_NORM_5) {
        norm = 0;
        if (type & NJD_MTYPE_POS_0) {
            norm = 1;
        }
        if (type & NJD_MTYPE_ANG_1) {
            norm++;
        }
        if (type & NJD_MTYPE_SCL_2) {
            norm++;
        }
        if (type & NJD_MTYPE_VEC_3) {
            norm++;
        }
        if (type & NJD_MTYPE_VERT_4) {
            norm++;
        }
    }
    if (type & NJD_MTYPE_TARGET_3) {
        past_camera = 0;
        if (type & NJD_MTYPE_POS_0) {
            past_camera = 1;
        }
        /* @hack redundant: NJD_MTYPE_TARGET_3 tested again inside its own
         * test */
        if (type & NJD_MTYPE_TARGET_3) {
            past_camera++;
        }
        if (type & NJD_MTYPE_ROLL_6) {
            past_camera++;
        }
        if (type & NJD_MTYPE_ANGLE_7) {
            past_camera++;
        }
    }
    for (; nodes > 0; nodes--) {
        keys = mdata;
        for (i = 0; i < factors; i++) {
            if (keys[i] < MEMORY_FIRST && keys[i] != 0) {
                keys[i] += base;
            }
        }
        if (vert >= 0) {
            key = (NJS_MKEY_P *)keys[vert];
            if (key != NULL) {
                for (i = keys[factors + vert]; i > 0; i--) {
                    if ((Uint32)key->key < MEMORY_FIRST && key->key != NULL) {
                        key->key = (void *)((Uint32)key->key + base);
                    }
                    key++;
                }
            }
        }
        if (norm >= 0) {
            key = (NJS_MKEY_P *)keys[norm];
            if (key != NULL) {
                for (i = keys[factors + norm]; i > 0; i--) {
                    if ((Uint32)key->key < MEMORY_FIRST && key->key != NULL) {
                        key->key = (void *)((Uint32)key->key + base);
                    }
                    key++;
                }
            }
        }
        if (past_camera >= 0) {
            key = (NJS_MKEY_P *)keys[past_camera];
            if (key != NULL) {
                for (i = keys[factors + past_camera]; i > 0; i--) {
                    if ((Uint32)key->key < MEMORY_FIRST && key->key != NULL) {
                        key->key = (void *)((Uint32)key->key + base);
                    }
                    key++;
                }
            }
        }
        /* @hack spelling: the next record's address worked out in bytes,
         * where mdata += factors * 2 would do */
        mdata = (Uint32 *)((Uint32)mdata + (factors * FACTOR_SIZE));
    }
}
