/* dust: puffs that drift and grow as they age, and the tasks that raise them
 * at the players' feet */
#ifndef SHARED_DUST_H
#define SHARED_DUST_H
#include <ninja.h>
#include <sg_xpt.h>

#include "effect/particle.h"
#include "system/angle.h"

struct task;

/* dust01 to dust08: a puff's DUST_FRAMES frames, in the order it shows them */
#define DUST_FRAMES 8
extern NJS_TEXLIST dust_texlist;

/* make a puff at pos, moving at spd, of size scale. each returns the puff's
 * particle, or null where the heap can't give one.
 *
 * a puff starts turned up to 10 degrees either way. it turns up to half a
 * degree a step either way. it shows dust_texlist's textures in turn, from a
 * random point in the first. each step slows the puff, grows it and speeds it
 * upward. a puff that fades holds the last texture while its alpha halves
 * eight times, to 0. dust_smoke_create's and dust_sand_create's puffs, of
 * 0xff706058 and 0xff504840, lie over what is behind them. the others are
 * white, and add their light to it.
 *
 * - dust_create's puff shows the textures over 64 to 73 steps, then fades out
 *   over 70 more.
 * - dust_smoke_create's puff speeds upward more slowly. it shows the textures
 *   over 47 to 54 steps, then fades out over 49 more.
 * - dust_sand_create's puff speeds downward instead. it shows the textures
 *   over 117 to 134 steps, then fades out over 119 more.
 * - dust_short_create's puff shows the textures over 36 to 41 steps, then
 *   ends without a fade.
 * - dust_steady_create's puff lasts as dust_short_create's does, and draws
 *   nearer by depth_bias. the system skips it at a smaller size on the
 *   screen. its group's size_limit is 14400, and dust_short_create's 22500.
 * - dust_steady_gravity_create's puff is dust_steady_create's, but speeds
 *   against world_gravity rather than up y.
 *
 * the first four kinds swing edge_angle up to 8 degrees either side of a
 * quarter turn, and back. their quads thus skew. a swing from one side to the
 * other takes 60 steps. the steady kinds' quads stay square */
struct particle *dust_create(NJS_POINT3 *pos, NJS_VECTOR *spd, Float scale);
struct particle *dust_smoke_create(NJS_POINT3 *pos, NJS_VECTOR *spd,
                                   Float scale);
struct particle *dust_sand_create(NJS_POINT3 *pos, NJS_VECTOR *spd,
                                  Float scale);
struct particle *dust_short_create(NJS_POINT3 *pos, NJS_VECTOR *spd,
                                   Float scale);
struct particle *dust_steady_create(NJS_POINT3 *pos, NJS_VECTOR *spd,
                                    Float scale, Float depth_bias);
struct particle *dust_steady_gravity_create(NJS_POINT3 *pos, NJS_VECTOR *spd,
                                            Float scale, Float depth_bias);

/* make puffs of size scale along the edges of a rectangle centered on pos.
 * unturned, the rectangle is 2 * half_x wide in x and 2 * half_z deep in z.
 * each edge is cut into spans of 10 / density, from its end of lesser x or z.
 * a puff lies at the start of each whole span. each puff flies straight out
 * from pos at speed. where half_x or half_z is 0, a puff can lie at pos itself.
 * that puff's speed is then NaN, from a division by its distance of 0.
 *
 * ang turns the rectangle by njRotateZ, njRotateY and njRotateX, in that
 * order. dust_short_rect turns it by njRotateY then njRotateZ, and ignores
 * ang->x.
 *
 * dust_rect makes dust_create's puffs, dust_smoke_rect dust_smoke_create's,
 * and dust_short_rect dust_short_create's */
void dust_rect(NJS_POINT3 *pos, Float half_x, Float half_z, struct angles *ang,
               Float scale, Float density, Float speed);
void dust_smoke_rect(NJS_POINT3 *pos, Float half_x, Float half_z,
                     struct angles *ang, Float scale, Float density,
                     Float speed);
void dust_short_rect(NJS_POINT3 *pos, Float half_x, Float half_z,
                     struct angles *ang, Float scale, Float density,
                     Float speed);
/* make a puff of size scale at a random point of the sphere of radius about
 * pos. the point lies no more than 120 degrees from the sphere's top. the puff
 * flies straight out from pos at speed. dust_sphere makes one of
 * dust_create's puffs, and dust_short_sphere one of dust_short_create's */
void dust_sphere(NJS_POINT3 *pos, Float radius, Float speed, Float scale);
void dust_short_sphere(NJS_POINT3 *pos, Float radius, Float speed, Float scale);
/* make count puffs of size scale on a level ring of radius about pos.
 * dust_ring makes dust_create's puffs, and dust_short_ring
 * dust_short_create's. each puff flies out from pos at 0.2 times scale, and
 * up at 0.03 times scale.
 *
 * the puffs don't lie evenly round the ring, as dust.c's dust_ring says. they
 * count from 0. puff k lies (k + 1) * start + k(k + 1) / 2 * step around the
 * ring. start is a random angle, and step is a full turn / count. count must be
 * from 1 to 255. the loop counts the puffs in a Uint8, so a call with a larger
 * count never returns */
void dust_ring(NJS_POINT3 *pos, Float radius, Float scale, int count);
void dust_short_ring(NJS_POINT3 *pos, Float radius, Float scale, int count);
/* make count of dust_smoke_create's puffs of size scale on half a level ring
 * of radius about pos. the half is centered on the current camera's y angle.
 * the puffs split the half into equal shares, and each lies at the middle of
 * its share. each flies out from pos at 0.2 times scale, and up at 0.03 times
 * scale. count must be from 1 to 255, as for dust_ring */
void dust_smoke_arc(NJS_POINT3 *pos, Float radius, Float scale, int count);

/* make player's smoke task or sand task, and return it. the task is parent's
 * child, or at task level 1 where parent is null. player must not be below 0.
 * each call returns null for a player above 1, and for one that has such a
 * task already. it also returns null where the heap can't give the task.
 *
 * the tasks raise rings of puffs as their player lands. they raise them too as
 * its animation starts over while it runs or brakes on the ground. the smoke
 * task also does so in a Sonic's or a Shadow's PLAYER_MODE_SONIC_COMBO_WAIT. a
 * ring needs the probe player_records[player]->ahead to find a WORLD_DUSTY
 * floor below. the sand task also needs WORLD_FLAT water over that floor, and
 * the smoke task needs no water there.
 *
 * the smoke task also raises plumes, on a floor or off one. it raises them in
 * its player's PLAYER_MODE_UNK_38, and while a Sonic or a mech sinks. a plume
 * plays a sound on four stages, and can shake the player's pad.
 *
 * each task ends once its player has no work. the smoke task also ends once
 * its player has no record. a sand task's player must have a record while it
 * has a work, unchecked */
struct task *smoke_task_create(int player, struct task *parent);
struct task *sand_task_create(int player, struct task *parent);

#endif
