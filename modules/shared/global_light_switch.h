/* the global light switch, G LIGHT SW in a stage's object list. its volumes
 * give light 5 to a player inside them */
#ifndef SHARED_GLOBAL_LIGHT_SWITCH_H
#define SHARED_GLOBAL_LIGHT_SWITCH_H

struct task;

/* the object list's step for a volume. where placement_try_leave(task) returns
 * TRUE, it does nothing more. the first volume creates the manager, a task
 * named ObjectGlobalLightManager at task level 1. where the heap can't give
 * the manager, this stays task's step and tries again on the next. otherwise
 * it gives task a collision record of one shape, and the volume's own step and
 * draw. the heap must have room for the record, unchecked.
 *
 * task needs a work, whose pos places the volume. the low two bits of ang.z
 * pick its shape: a box, a sphere or one of two cylinders. scl.x, scl.y and
 * scl.z give the shape's radius, size_y and size_z. the call clears ang.x and
 * the low byte of ang.z. the rest of ang.z turns a box about z.
 *
 * a player inside no volume who comes into one takes light 5. a player who
 * comes out of a volume into none takes light_camera, which draw/light.h
 * declares. the stage selects that light for the players it places. the
 * manager makes both changes at its step */
void global_light_switch_init(struct task *task);

#endif
