/* probes of the collision world's surfaces about a point, and the shadow
 * functions. an object's reach is its model's radius times its largest
 * scale, measured from its pos */
#ifndef COLLISION_SURFACE_H
#define COLLISION_SURFACE_H
#include <ninja.h>
#include <sg_xpt.h>

#include "system/angle.h"

struct player;
struct task_work;

/* the height that stands for no floor, and negated, for no surface
 * above */
#define NO_FLOOR (-1000000.0F)

/* a surface on one side of a probe's point, as a search leaves it */
struct surface_side {
    Bool found;
    /* the world flags of the surface's object. only the searches of a
     * collision mesh set them: polygon_find_sides, polygon_find_floor,
     * polygon_find_heights and polygon_cast_ray */
    Uint32 flags;
    /* the surface's tilt about x and about z: the angles that turn the y
     * axis to its normal */
    Angle ang_x;
    Angle ang_z;
    /* the surface's coordinate on the side's axis, which runs through the
     * probe's point. it is a floor's height, or a wall's x or z */
    Float pos;
    NJS_VECTOR normal; /* a unit vector */
};

/* the indices of a surface_probe's sides, by the way each lies from pos */
#define SURFACE_NEG_X 0
#define SURFACE_POS_X 1
#define SURFACE_NEG_Y 2
#define SURFACE_POS_Y 3
#define SURFACE_NEG_Z 4
#define SURFACE_POS_Z 5
#define SURFACE_SIDES 6
/* a point, and the nearest surface on each of its six sides */
struct surface_probe {
    NJS_POINT3 pos;
    struct surface_side sides[SURFACE_SIDES];
};
/* a point, and the surfaces below it and above it */
struct height_probe {
    NJS_POINT3 pos;
    struct surface_side below;
    struct surface_side above;
};

/* a point, and a floor a search finds at its x and z, above or below it */
struct floor_probe {
    NJS_POINT3 pos;
    struct surface_side floor;
};

/* the nearest surfaces above a point and at or below it, as
 * surface_probe_ground leaves them */
struct ground_probe {
    /* the tilt of the surface below, kept from an earlier probe where no
     * surface lies below */
    Angle below_ang_x;
    Angle below_ang_z;
    /* the world flags of the two surfaces' objects, or 0 for none */
    Uint32 above_flags;
    Uint32 below_flags;
    /* their heights, or -NO_FLOOR and NO_FLOOR for none */
    Float above_y;
    Float below_y;
    /* the flags as the probe before left them */
    Uint32 last_above_flags;
    Uint32 last_below_flags;
    /* a fifth of the player's radius, which player_register sets in the
     * player's ground */
    Float unk_20;
};

/* surface_probe_floor, surface_probe_nearest, surface_probe_nearest_land and
 * surface_probe_first_floor list the objects near the point in world_objects
 * first, by world_list_near. each sets ang->x and ang->z to the tilt of the
 * floor it takes. where it takes none, it still writes them, from stack words
 * or a register it never set.
 *
 * surface_probe_floor and surface_probe_nearest take a task's object only
 * within its reach across x and z. that object's floors move by its task's
 * force->speed.y. surface_probe_floor moves each floor before it compares
 * it. surface_probe_nearest compares each floor before its move with the
 * floor it keeps, which has moved */

/* the height of the highest floor at or below the point, or NO_FLOOR. it
 * takes the objects world_list_near lists within 10 of the point. it passes
 * over those with a bit of WORLD_FLAT */
Float surface_probe_floor(Float x, Float y, Float z, struct angles *ang);
/* the height of the floor over or under the point that lies nearest the
 * point's height, or NO_FLOOR. it takes the objects world_list_near lists
 * within 5 of the point */
Float surface_probe_nearest(Float x, Float y, Float z, struct angles *ang);
/* surface_probe_nearest, of the land's objects with neither bit of
 * WORLD_FLAT */
Float surface_probe_nearest_land(Float x, Float y, Float z, struct angles *ang);
/* the height of the first floor over or under the work's pos, or 4000 for
 * none. it takes the objects world_list_near lists within 20 of the pos. a
 * land object counts only within its reach across x and z. it returns 3000
 * instead where the floor lies no more than 1.5 below a stack word. no code
 * writes that word. with no object near, the height it returns is a stack word
 * too. no image calls it */
Float surface_probe_first_floor(struct task_work *work, struct angles *ang);

/* the shadow functions that draw nothing. shadow_at_height and shadow_at do
 * nothing at all. shadow_floor returns the height of the floor
 * surface_probe_floor finds from 4 above the work's pos. shadow_floor_slope
 * finds it from the pos itself, and sets slope to the floor's tilt. no image
 * calls any of them */
void shadow_at_height(struct task_work *work, Float height);
Float shadow_floor(struct task_work *work);
void shadow_at(void);
Float shadow_floor_slope(struct task_work *work, struct angles *slope);
/* draw the player's shadow while player->ground.below_y lies less than
 * 20000 under the work's pos. the shadow is shadow_model, drawn as a
 * modifier volume under DRAW_CONTROL_SHADOW. it stands at player->unk_218's
 * x and z, 0.3 above the work's pos. it turns about y by half a turn less
 * the work's ang.y */
void shadow_draw_player(struct task_work *work, struct player *player);

/* probe the surfaces about the point 1 above pos. it takes the objects in
 * world_objects, which it doesn't list anew. it first moves ground's
 * above_flags and below_flags to last_above_flags and last_below_flags.
 * above_y then takes the height of the nearest surface above the point.
 * below_y takes that of the highest at or below it.
 *
 * an object with a bit of WORLD_FLAT gives only a surface above, of its own
 * model. in STAGE_RELAY_SONIC, such a surface counts as any other does. in
 * every other stage, above_y takes the last such surface listed, nearer or
 * not, over every other object's. every other object gives the surfaces of
 * its collision mesh. of these objects, a task's counts only
 * within its reach plus 30, across x and z. under WORLD_TURNED, that reach
 * runs from its pos plus its model's center turned by the object's angles.
 * its surfaces move by its task's force->speed.y */
void surface_probe_ground(NJS_POINT3 *pos, struct ground_probe *ground);
/* probe the surfaces about the player. it lists in world_objects the objects
 * world_list_near lists within 200 of the work's pos. player->ground then takes
 * the surfaces about the work's pos, by surface_probe_ground. player->ahead
 * takes those about a point consts.radius + 2 ahead and consts.height + 2 up.
 * the offset points along the heading player_read_stick gives, or else along
 * the work's ang.y. the work's ang.x and ang.z then turn it.
 *
 * where ground's above_flags has a bit of WORLD_FLAT, it first ripples that
 * surface straight above the work's pos. it splashes there too where
 * ground's last_above_flags has none */
void surface_probe_player(struct task_work *work, struct player *player);

/* take for each of probe's sides the nearest surface on that side of its
 * pos, by polygon_find_sides. it takes the objects world_list_near lists
 * within 1 of pos. a task's object counts only within its reach in x, y and
 * z. a side where none is found has found FALSE. its pos is NO_FLOOR toward
 * -x, -y or -z, and -NO_FLOOR toward +x, +y or +z.
 *
 * surface_probe_sides passes over an object with a bit of WORLD_FLAT. it
 * returns 1 where it finds any surface, else 0. surface_probe_sides_flat
 * takes those objects too. it returns 0, or 1 where a surface of such an
 * object ever took probe's SURFACE_NEG_Y side. a nearer surface may have
 * taken the side since */
int surface_probe_sides(struct surface_probe *probe);
int surface_probe_sides_flat(struct surface_probe *probe);

/* the shadow's modifier volume. shadow_object's model is shadow_model */
extern NJS_CNK_OBJECT shadow_object;
/* A single model in the Ninja chunk array used by the character drawers. */
extern NJS_CNK_MODEL shadow_model[1];

#endif
