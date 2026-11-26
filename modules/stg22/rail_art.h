/* the transport rails' exit and intake: their models, texture lists,
 * collision objects and the intake belt's scroll */
#ifndef STG22_RAIL_ART_H
#define STG22_RAIL_ART_H
#include <ninja.h>
#include "draw/texture_anim.h"

extern NJS_TEXLIST rail_exit_texlist;
extern NJS_CNK_OBJECT rail_exit_panel;
extern NJS_CNK_OBJECT rail_exit_root;
extern NJS_OBJECT rail_exit_collision;
extern NJS_OBJECT rail_intake_collision;
extern NJS_TEXLIST gadget_intake_texlist;
extern NJS_CNK_OBJECT gadget_intake_rib_a[];
extern NJS_CNK_OBJECT gadget_intake_rib_b[];
extern NJS_CNK_OBJECT gadget_intake_rib_c[];
extern NJS_CNK_OBJECT gadget_intake_cover[];
extern NJS_CNK_OBJECT gadget_intake_lamp[];
extern NJS_CNK_OBJECT gadget_intake_glow[];
extern NJS_CNK_OBJECT gadget_intake_belt[];
extern NJS_CNK_OBJECT gadget_intake_base[];
extern NJS_TEXLIST gadget_intake_shard_a_texlist;
extern NJS_CNK_MODEL gadget_intake_shard_a[];
extern NJS_TEXLIST gadget_intake_shard_b_texlist;
extern NJS_CNK_MODEL gadget_intake_shard_b[];
extern struct texture_anim gadget_intake_scroll;
extern NJS_TEXLIST *gadget_intake_texlists[];

#endif
