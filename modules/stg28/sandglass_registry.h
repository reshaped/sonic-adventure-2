/* Pyramid Cave's registry of its sandglasses: shared/sandglass_registry.h's,
 * under the cave_ names. each name here is the name there with cave_ before
 * it, and does what that one does */
#ifndef STG28_SANDGLASS_REGISTRY_H
#define STG28_SANDGLASS_REGISTRY_H
#include <ninja.h>
#include <sg_xpt.h>

struct task;

extern struct task **cave_sandglasses;
extern Sint32 cave_sandglass_count;

void cave_sandglass_take_mode(struct task *task);
void cave_sandglass_set_mode(struct task *task, int mode);
struct task **cave_sandglass_register(struct task *task);
void cave_sandglass_unregister(struct task *task);
struct task *cave_sandglass_find(int id);
Bool cave_sandglass_running(int id);
Float cave_sandglass_time_left(int id);
Bool cave_sandglass_pick_up(struct task *task, NJS_POINT3 *pos, Angle ang);
void cave_sandglass_carry(struct task *task, NJS_POINT3 *pos, Angle ang);
void cave_sandglass_throw(struct task *task, Angle ang);

#endif
