/* the scavenger hunt of TRACK_JWL_IL and TRACK_JWL_LONG. a racer picks one of
 * SCAVENGER_ITEMS toys and brings it to one of SCAVENGER_LANES lanes. the
 * lane's panel judges the toy */
#ifndef CHAOSTGRACE_SCAVENGER_H
#define CHAOSTGRACE_SCAVENGER_H
#include <ninja.h>
#include <sg_xpt.h>
struct task;

enum { SCAVENGER_LANES = 4, SCAVENGER_ITEMS = 8 };
enum toy_kind { TOY_DOLL, TOY_ROUND, TOY_TRIANGLE, TOY_SQUARE, TOY_KINDS };
/* a toy's toy_kind, in its work */
#define TOY_KIND(work) ((work)->tick)
/* (int)(rand() * (TOY_KIND_DRAW * RAND_SCALE)) is a toy_kind at random */
#define TOY_KIND_DRAW 3.9999F
/* each item's toy while it waits to be taken, or NULL */
extern struct task *race_scavenger_items[SCAVENGER_ITEMS];
/* the toy_kind to find */
extern Sint32 race_scavenger_target;
/* whether each lane's racer has submitted a toy. the lane's panel clears it
 * as it judges the toy */
extern Bool race_scavenger_submitted[SCAVENGER_LANES];
/* make the signs and the panels, choose race_scavenger_target, and drop
 * every item's toy */
void race_scavenger_start(void);
/* drop a new toy for item index at its place, as its race_scavenger_items
 * entry */
void race_respawn_scavenger_item(int index);
#ifdef NONMATCHING
/* mark toy's item missing, where race_scavenger_items still holds toy */
void race_forget_scavenger_item(struct task *toy);
/* once race_scavenger_start has made the panels, drop a new toy for each
 * missing item. it returns whether one was missing, and TRUE before the
 * panels are made, when it drops nothing */
Bool race_retry_scavenger_items(void);
#endif
/* give chao a place at a lane's panel, or else in line behind one, and aim
 * it there. work->race_slot becomes the lane. it returns TRUE for a place at
 * the panel and FALSE for one in line. with every place taken it changes
 * nothing, and returns no value in the image; the GCC side returns FALSE */
Bool race_join_scavenger_lane(struct task *chao);
/* the racer at lane's panel, or NULL */
struct task *race_scavenger_occupant(Sint32 lane);
/* move lane's waiting racer, or NULL, to its panel */
void race_advance_scavenger_lane(int lane);
/* make item index's toy at position, facing yaw, and moving at *velocity
 * where velocity isn't NULL. its kind is mostly race_scavenger_target, and
 * else another at random. the GCC side returns NULL where it can't make the
 * task or its mover */
struct task *race_scavenger_item_create(Uint8 index, NJS_POINT3 *position,
                                        Angle yaw, NJS_VECTOR *velocity);
#endif
