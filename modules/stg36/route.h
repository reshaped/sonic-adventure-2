/* Tails's route: its kill areas, its floor, and its machinery's models, motions
 * and texture lists */
#ifndef STG36_ROUTE_H
#define STG36_ROUTE_H
#include <ninja.h>
#include <sg_xpt.h>

#include "player/player.h"
#include "shared/core_machine_lights.h"

extern struct kill_area core_tails_kill_areas[];
extern NJS_OBJECT tails_floor_template;
extern Uint32 tails_pipe_tint;
extern NJS_TEXLIST tails_panel_texlist;
extern NJS_TEXLIST tails_joint_texlist;
extern NJS_TEXLIST tails_pipe_texlist;
extern NJS_TEXLIST tails_rotor_texlist;
extern NJS_TEXLIST tails_glow_texlist;
extern NJS_TEXLIST tails_strip_texlist;
extern NJS_CNK_OBJECT tails_joint;
extern NJS_CNK_OBJECT tails_outlet;
extern NJS_CNK_OBJECT tails_elbow;
extern NJS_CNK_OBJECT tails_end_joint;
extern NJS_MOTION tails_joint_motion, tails_joint_shape;
extern NJS_MOTION tails_outlet_motion, tails_outlet_shape;
extern NJS_MOTION tails_elbow_motion, tails_elbow_shape;
extern NJS_MOTION tails_end_joint_motion, tails_end_joint_shape;
extern NJS_CNK_OBJECT tails_rotor;
extern NJS_MOTION tails_pipe_shape;
/* @hack type: the pipe, the rotor's placements and the two motions declared
 * as arrays of one, where single objects would do */
extern NJS_CNK_OBJECT tails_pipe[];
extern NJS_CNK_OBJECT tails_rotor_placements[];
extern NJS_MOTION tails_pipe_motion[], tails_rotor_motion[];
extern NJS_CNK_OBJECT **tails_entrance_lamps[CORE_STRIPS];

#endif
