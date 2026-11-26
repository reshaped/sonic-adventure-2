/* the clear switch and the goal door of Rouge's and Knuckles's routes in
 * Cannon's Core. their kinds are CLEAR SW and GOAL DOOR */
#ifndef SHARED_CORE_GOAL_H
#define SHARED_CORE_GOAL_H

#include <ninja.h>

struct task;

struct core_door_material_names {
    char frame[8];
    char door[20];
    char glass[24];
    char shaft[24];
    char side[24];
    char metal128[16];
    char metal256[16];
    char metal32[12];
};

extern const struct core_door_material_names core_door_material_names;
extern NJS_TEXLIST *core_door_texlists[];
extern NJS_CNK_OBJECT *core_door_models[1];
extern NJS_MOTION core_door_motion[1];
void core_clear_switch_init(struct task *task);
void core_goal_door_init(struct task *task);

#endif
