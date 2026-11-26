/* relocating Ninja data loaded from a file */
#ifndef SYSTEM_RELOCATE_H
#define SYSTEM_RELOCATE_H
#include <ninja.h>
#include <sg_xpt.h>

/* data loaded from a file holds offsets where it will hold pointers.
 * relocating a pointer adds base to it. a null pointer stays null, and one at
 * 0x0c000000 or above, where the Dreamcast's memory starts, is taken for an
 * address already and stays as it is */

/* relocate the model, child and sibling pointers of object and of every node
 * below and beside it. the walk follows a child or a sibling that is an
 * address already. relocating a model relocates its vlist and plist too,
 * even where they are addresses already. two nodes must not share a model,
 * or its lists are relocated twice. object may be null */
void relocate_cnk_object(NJS_CNK_OBJECT *object, Uint32 base);
/* relocate motion's mdata, and the key pointers in as many of its records as
 * nodes gives. mdata holds a record for each node: a pointer to each factor's
 * keys, then each factor's key count. a null motion changes nothing, and
 * neither does one whose mdata is null or an address already.
 *
 * the keys of an NJD_MTYPE_VERT_4 or NJD_MTYPE_NORM_5 factor point to their
 * data, and are relocated too. under NJD_MTYPE_TARGET_3, so are the keys of
 * the factor past a camera's. that factor's index counts the bits of
 * NJD_MTYPE_POS_0, NJD_MTYPE_TARGET_3, NJD_MTYPE_ROLL_6 and NJD_MTYPE_ANGLE_7
 * that motion's type sets. such a motion must have it, unchecked */
void relocate_motion(NJS_MOTION *motion, Uint32 base, int nodes);

#endif
