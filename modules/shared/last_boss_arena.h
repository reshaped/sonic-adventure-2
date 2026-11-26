/* the arena geometry of the two last fights, the Biolizard's and Finalhazard's.
 * each fight's arena.c defines these calls from last_boss_orbit.h and
 * last_boss_aim.h, with its own center and boss task. a bearing is the angle
 * about the center from +z toward +x */
#ifndef SHARED_LAST_BOSS_ARENA_H
#define SHARED_LAST_BOSS_ARENA_H
#include <ninja.h>
#include <sg_xpt.h>

struct collision_hit;
struct task;

/* return position's distance from the center in x and z. where offset isn't
 * null, *offset takes position less the center, y included. last_boss_along and
 * last_boss_at_radius scale a vector to distance in all three axes, not in x
 * and z alone */
Float last_boss_radius(const NJS_POINT3 *position, NJS_VECTOR *offset);
void last_boss_world_point(const NJS_VECTOR *offset, NJS_POINT3 *out);
void last_boss_along(const NJS_VECTOR *direction, Float distance,
                     NJS_POINT3 *out);
void last_boss_at_angle(Angle angle, Float radius, NJS_POINT3 *out);
void last_boss_at_radius(const NJS_POINT3 *position, Float distance,
                         NJS_POINT3 *out);
Float last_boss_distance(const NJS_POINT3 *first, const NJS_POINT3 *second,
                         NJS_VECTOR *offset);
/* the tangent is the bearing plus a quarter turn, the way the boss circles.
 * where the Biolizard's orbit is reversed, the tangent turns half a turn more
 */
Angle last_boss_tangent(const NJS_POINT3 *position);
void last_boss_tangent_vector(const NJS_POINT3 *position, NJS_VECTOR *out);
Angle last_boss_angle_distance(const NJS_POINT3 *first,
                               const NJS_POINT3 *second);
Angle last_boss_angle_difference(const NJS_POINT3 *first,
                                 const NJS_POINT3 *second);
Angle last_boss_angle_magnitude(Angle angle);
Angle last_boss_advance(const NJS_POINT3 *position, Angle step,
                        NJS_POINT3 *out);
Angle last_boss_turn(const NJS_POINT3 *position, Angle rotation,
                     NJS_POINT3 *out);
Angle last_boss_bearing(const NJS_POINT3 *position);
Angle last_boss_step_angle(Angle angle, Angle step);
/* set *out to direction scaled to length. where length is 0, or direction has
 * length 0, *out takes direction as it is */
void last_boss_vector_length(NJS_VECTOR *direction, Float length,
                             NJS_VECTOR *out);

Float last_boss_target_distance(struct task *task, const NJS_POINT3 *target);
Float last_boss_target_distance2(struct task *task, const NJS_POINT3 *target);
Angle last_boss_target_yaw(struct task *task, const NJS_POINT3 *target);
Angle last_boss_target_pitch(struct task *task, const NJS_POINT3 *target,
                             Angle offset);
/* turn the task's ang.y by step toward the yaw from its pos to target, the
 * short way round. return the size of the turn that remains */
Angle last_boss_face_target(struct task *task, const NJS_POINT3 *target,
                            Angle step);
int last_boss_hit_penetrates(struct task *task,
                             const struct collision_hit *hit);

#endif
