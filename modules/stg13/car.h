/* City Escape's cars. lines of cars drive paths: CARMAN, CARMAN_NEAR and
 * CARMANC in its object list. CARKAZ and CARKAZ_S are parked cars. a car pushes
 * the players out of its box. a crash throws it as a wreck, which tumbles and
 * bounces, then fades out. the truck crashes each car it meets */
#ifndef STG13_CAR_H
#define STG13_CAR_H
#include <ninja.h>
#include <sg_xpt.h>

#include "stage/path.h"

struct task;

/* compile each kind's model at buf, under its texture list, and return buf
 * past what it compiled. a kind whose build would reach DEMO_AREA has none, and
 * draws its model uncompiled. only the first call compiles */
Uint8 *car_build(Uint8 *buf);

/* name task as the truck the cars and their wrecks meet, or NULL for none.
 * they meet it only while it has a child and a non-null force. force must
 * point at two NJS_MATRIXes: the truck's frame, then its inverse. extra holds
 * the truck's speed along the frame's z, as a Float. the caller must name NULL
 * before the task ends */
void car_set_truck(struct task *task);

/* one of the lines' paths, which the truck's table of paths holds too */
extern struct path car_path_1;

/* the texture lists of the cars, their dust and their crash's flash, for
 * texture_load_shared, ended by null */
extern NJS_TEXLIST *car_texlists[];

/* the object list's steps for a line of cars: traffic_init for CARMAN and
 * CARMAN_NEAR, and traffic_wait_init for CARMANC. each does nothing where
 * placement_try_leave(task) returns TRUE. where the heap can't give the line's
 * record, it runs again on the next step. otherwise it starts the line's cars,
 * each a child task.
 *
 * task needs a work. scl.z + 0.1, truncated and taken modulo 13, picks the
 * path. scl.x is the cars' speed along it, in units a step. scl.y + 0.1,
 * truncated, counts the cars, unless the placement's state holds a count other
 * than 0. as the line ends, the placement's state takes how many of its cars
 * live.
 *
 * the low byte of ang.x, modulo 30, names the cars' kind. kinds 2 to 5 are
 * cable cars. 3 and 5 shuttle along their paths, and 4 and 5 push no player.
 * kinds 6 to 11 are six cars, and 12 to 17 the same six pushing no player.
 * kind 0 picks one of kinds 6 to 11 at random. kind 1 picks one of 12 to 17. a
 * car of either picks again as it starts its path over. traffic_wait_init
 * hands the low byte to its cars unreduced. so its 30 and 31 name kinds 0 and
 * 1 without the random pick.
 *
 * traffic_init moves the path's center to pos, and turns the path by ang.y.
 * it spaces its cars along the path, each moved ahead a little at random. its
 * cars drive all the while. traffic_wait_init leaves the path where it lies,
 * and spaces its cars evenly up to the path's end. its cars drive only while
 * player_near finds a player within the low 16 bits of ang.z of pos.
 *
 * a shuttling car turns back at its path's end, and any other starts the path
 * over. a line's car crashes as a parked car does, and its crash kills its
 * task. it pushes a player in its box's +z half out through a side in x */
void traffic_init(struct task *task);
void traffic_wait_init(struct task *task);

/* the object list's step for CARKAZ and CARKAZ_S, a parked car. it overwrites
 * the work's submode, and gives task the car's setup as its step. the setup
 * does nothing where placement_try_leave(task) returns TRUE. where the heap
 * can't give the car's record, it runs again on the next step.
 *
 * task needs a work. pos places the car, and ang turns it about z, then x, then
 * y. scl.y + 0.1, truncated, names the car's kind, as traffic_init's ang.x
 * does. a parked car then takes the kind 12 past that, modulo 30, but for
 * kinds 2 and 3. kinds 18 to 29 are kinds 6 to 17 in one piece. a car whose
 * scl.x + 0.1 truncates to other than 0 pushes no player.
 *
 * a car pushes and crashes only while it is in view. it pushes players 0 and 1
 * out of its box, through the box's nearer side. after its first 20 steps, a
 * touch may crash it. a player's touch crashes it only while player 0 or 1
 * lies in its box. Sonic or Shadow crashes it in PLAYER_MODE_ROLL, or in a
 * mode from PLAYER_MODE_SONIC_COMBO_1 to PLAYER_MODE_SONIC_COMBO_RUNNING_3. a
 * mech's body crashes it, and so does any touch but a player's. any player's
 * touch crashes a car that pushes no player. the truck crashes any car it
 * meets.
 *
 * a crash a player makes scores 20 points. a crash kills the car's placement,
 * or its task where it has none. it does so even where the heap can't give the
 * wreck. where the heap can't give the car or its wreck a collision record,
 * its steps read through null */
void car_init(struct task *task);

#endif
