/* garden objects' headings, moves and blends, and whether the camera sees
 * them */
#ifndef CHAO_MOVEMENT_H
#define CHAO_MOVEMENT_H
#include <ninja.h>
#include <sg_xpt.h>

struct task;
struct angles;

/* blend the three angles from and to into result, share of the way to to.
 * each angle turns the short way round */
void garden_blend_angles(const Angle *from, const Angle *to, Float share,
                         Angle *result);
/* set angles->y and angles->x, the turns about y and then x, to carry the +z
 * axis onto direction. angles->z keeps its value */
void garden_aim_angles(NJS_VECTOR *direction, struct angles *angles);
/* whether the Chao stands on a floor at a horizontal speed below 0.04 */
Bool chao_settled(struct task *chao);
/* whether the camera may see a box about the point height above task's pos.
 * the box reaches half_width and half_height either side of the point.
 * garden_visible_at_height takes a square box, and garden_object_visible a
 * square box about pos.
 *
 * the point's distance from the camera sizes the box on the screen. the call
 * writes the distance to task's garden entry, where it has one. a task behind
 * the camera's horizontal facing is out of view, whatever the camera's pitch.
 * so is a box wholly outside the screen's top 440 lines */
Bool garden_visible_box(struct task *task, Float half_width, Float half_height,
                        Float height);
Bool garden_visible_at_height(struct task *task, Float radius, Float height);
Bool garden_object_visible(struct task *task, Float radius);
/* blend the trees from and to, share of the way to to, into result. where
 * result has a model, a child or a sibling, so must from and to. each model's
 * vertex list must open with an NJD_CV_D8 chunk. result's chunk may hold no
 * more vertices than theirs. points and colors blend, each color held from 0
 * to 255. a node's pos and ang blend too, its angles as plain numbers, not the
 * short way round */
void garden_blend_models(const NJS_CNK_OBJECT *from, const NJS_CNK_OBJECT *to,
                         NJS_CNK_OBJECT *result, Float share);
/* the heading from task's pos toward other's */
Angle garden_heading_to_object(struct task *task, struct task *other);
/* aim task's mover at other, and turn ang.y toward it by step at most. return
 * the turn left, from 0 to 0x8000, or 0 where either task is null */
int garden_turn_to_object(struct task *task, struct task *other, int step);
/* the distance between the two tasks' pos, or -1 where either is null */
Float garden_object_distance(struct task *task, struct task *other);
/* move the task's pos distance along its heading */
void garden_move_forward(struct task *task, Float distance);
/* link the walk the Chao's run skill earns. below
 * chao_tuning.walk_slow_points it is CHAO_ANIM_HAIHAI, and below walk_points
 * CHAO_ANIM_ARUKU. below walk_fast_points it is CHAO_ANIM_HASIRU, and from
 * there up CHAO_ANIM_HASIRU_HIROGE */
void chao_link_walk_motion(struct task *chao);
/* the task of the first shape with id kind that touches object's shape
 * index, or null. each call starts the scan over, so it never returns a
 * second */
struct task *garden_hit_kind(struct task *object, int index, Uint8 kind);
/* while task lies below its mover's flat_y, the water's surface, buoy it and
 * slow it. now and then it ripples the surface. the lift grows with the depth.
 * it matches gravity at depth below the surface, and stays at twice gravity
 * from twice depth down */
void garden_apply_buoyancy(struct task *task, Float depth);

/* the square of the horizontal distance from the Chao to its mover's target */
Float chao_target_distance_squared(struct task *chao);
/* set the mover's acc in x and z to acceleration along the Chao's heading */
void chao_set_forward_acceleration(struct task *chao, Float acceleration);
/* scale the mover's spd by scale while it stands on a floor */
void chao_slow_on_floor(struct task *chao, Float scale);
#endif
