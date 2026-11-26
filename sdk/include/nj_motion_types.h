/* the state Ninja's model, camera and light motion walkers share */
#ifndef NJ_MOTION_TYPES_H
#define NJ_MOTION_TYPES_H
#include <nj_render_types.h>
#include <sg_xpt.h>

/* where a motion's walk stands as it poses a tree, a node at a time */
struct nj_motion_info {
    void *mdata; /* the next node's NJS_MDATA entry */
    /* the key lists of the last node's entry, one for each factor: position,
     * angle, scale and so on */
    void **p;
    Uint32 *nb; /* each of those lists' count of keys */
    Float frame; /* the frame the motion is posed at */
    Uint32 nbFrame; /* the motion's count of frames */
    Uint32 factor; /* the factor the walk reads next, an index into p and nb */
    /* the matrix the last transform call was given, or NULL. each node's step
     * multiplies it onto the current matrix after the node's pose */
    NJS_MATRIX *m;
};

/* the floats each _nj_motion_link_ array holds. each of two slots has a value
 * of three components and a tangent of three. a quaternion array holds four
 * components for each slot, and no tangent */
#define LINK_PLACES 12
#define LINK_QUA_PLACES 8

#endif
