/* the water's bubbles: BUBBLE and BUBBLES in a stage's object list */
#ifndef SHARED_WATER_BUBBLES_H
#define SHARED_WATER_BUBBLES_H
#include <ninja.h>
#include <sg_xpt.h>

struct task;

/* the height of the water's surface over pos, or null for no water. Aquatic
 * Mine's water sets it as the water starts, and clears it as the water ends.
 * the other two stages that hold the bubbles never set it. Aquatic Mine's
 * function returns -100000 where no water lies over pos */
extern Float (*water_surface_height)(NJS_POINT3 *pos);

/* the object list's step for BUBBLE, a source of large bubbles. unless
 * placement_try_leave(task) returns TRUE, it looks for a surface over the
 * work's pos. it takes the lowest above the point scl.y over pos. it looks
 * among the world's objects within 20, or else takes the water's. it keeps the
 * surface's height in a record from the Shinobi heap. where it finds a surface
 * and gets the record, it gives task the source's own step and destructor.
 * otherwise task keeps this step, and tries again on the next.
 *
 * the source sends up a large bubble at its next step, and every 601 steps
 * after. each bubble is a child task of the source. a bubble bursts into a
 * ripple at the surface. a player's touch bursts it into pieces, and player 0
 * takes PLAYER_REQUEST_BREATHE, whichever player touched it. a bubble that does
 * neither ends after 1200 steps */
void bubble_source_init(struct task *task);
/* the object list's step for BUBBLES, a field of small bubbles. unless
 * placement_try_leave(task) returns TRUE, it gives task the field's step, draw
 * and destructor. the field's record must come from the Shinobi heap,
 * unchecked.
 *
 * scl.x is the field's radius. scl.y is the bubbles it starts a step. a scl.y
 * of n + f starts n, and one more by a chance of f. the whole part of scl.z is
 * the most bubbles the field holds at once. the field takes a pool of that many
 * from the heap where scl.z is above 0.99. a field without a pool starts none.
 * the call sets scl.z to 0 where it was above 0.99.
 *
 * a small bubble rises from the floor below the field's center, and ends at the
 * surface above it. it sways within the field's radius as it rises. it ends too
 * after 1200 steps, or near player 0 or 1. near is within 4.84 to 6.76, by its
 * size. the field leaves as placement_try_leave_by(task, r * r) says, where r
 * is its radius plus 410 */
void bubble_field_init(struct task *task);

#endif
