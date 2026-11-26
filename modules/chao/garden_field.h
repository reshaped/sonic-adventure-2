/* the garden's collision fields: boxes its SET files place, and spheres for
 * others to find by id */
#ifndef CHAO_GARDEN_FIELD_H
#define CHAO_GARDEN_FIELD_H
#include <ninja.h>
#include <sg_xpt.h>

struct task;

/* the id of a climb field's box. a Chao that walks or swims into a wall
 * inside the box climbs the wall */
#define GARDEN_CLIMB_FIELD_ID 203

/* the steps of the boxes a garden's object placement (SET) files place. each
 * box's half sizes are its placement's scale plus 10 */
/* a box whose id, from 178 to 193, is its x angle modulo 16 */
void garden_race_field(struct task *task);
/* a box turned by its angles, which pushes players */
void garden_cube_field(struct task *task);
void garden_climb_field(struct task *task);
/* boxes that take a player who touches them to the lobby, or to the entrance */
void garden_lobby_trigger(struct task *task);
void garden_entrance_trigger(struct task *task);

/* set the radius chao_tuning.show_fields draws the field at */
void garden_field_set_draw_radius(struct task *field, Float radius);
/* the field lives frames more steps. with frames 0 or less, it lives until
 * something destroys it */
void garden_field_set_lifetime(struct task *field, int frames);
void garden_field_set_kind(struct task *field, int kind);

/* make a sphere field of id kind and radius about position, and return it.
 * an object field and an enemy field join that collision list. a timed field
 * lives frames steps, and an oriented one keeps pitch in its ang.x. the GCC
 * side returns NULL where it can't make the task or its collision */
struct task *garden_object_field(int kind, NJS_POINT3 *position, Float radius);
struct task *garden_timed_object_field(int kind, NJS_POINT3 *position,
                                       Float radius, int frames);
struct task *garden_oriented_object_field(int kind, NJS_POINT3 *position,
                                          Float radius, Angle pitch,
                                          int frames);
struct task *garden_enemy_field(int kind, NJS_POINT3 *position, Float radius);
struct task *garden_timed_enemy_field(int kind, NJS_POINT3 *position,
                                      Float radius, int frames);
struct task *garden_oriented_enemy_field(int kind, NJS_POINT3 *position,
                                         Float radius, Angle pitch, int frames);
/* garden_enemy_field and garden_timed_enemy_field for a field that is chao's
 * child, and chao's field until another replaces it. where the GCC side
 * returns NULL, chao's field stays as it was */
struct task *garden_create_child_field(struct task *chao, int kind,
                                       NJS_POINT3 *position, Float radius);
struct task *garden_create_timed_field(struct task *chao, int kind,
                                       NJS_POINT3 *position, Float radius,
                                       int frames);
#endif
