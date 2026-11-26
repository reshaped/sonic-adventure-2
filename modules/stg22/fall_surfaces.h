#ifndef STG22_FALL_SURFACES_H
#define STG22_FALL_SURFACES_H

#include <ninja.h>

#define GADGET_FALL_MATERIAL_FLAGS                                             \
    (NJD_SA_SRC | NJD_DA_INV_SRC | NJD_FLAG_IGNORE_LIGHT |                     \
     NJD_FILTER_BILINEAR | NJD_D_100)

/* the objects of Crazy Gadget's nine kill areas, in crazy_gadget.c's list.
 * the room volumes in glow.h are boxes that gravity.c tests itself */
extern NJS_OBJECT gadget_kill_0;
extern NJS_OBJECT gadget_kill_1;
extern NJS_OBJECT gadget_kill_2;
extern NJS_OBJECT gadget_kill_3;
extern NJS_OBJECT gadget_kill_4;
extern NJS_OBJECT gadget_kill_5;
extern NJS_OBJECT gadget_kill_6;
extern NJS_OBJECT gadget_kill_7;
extern NJS_OBJECT gadget_kill_8;

#endif
