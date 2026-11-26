/* the gravity field, GRAV_SW in Mad Space's object list: a sphere or a
 * capsule. within its reach, it adds a way down to field_sample_at's sample */
#ifndef STG44_GRAVITY_H
#define STG44_GRAVITY_H

struct task;

/* the object list's step for GRAV_SW. it does nothing where task leaves at its
 * placement's range. otherwise it gives task the field's step, draw and
 * destructor, and makes task a field with field_add. task needs a work, and
 * a task->extra of null. each later step tests the range as the first did.
 *
 * the low 4 bits of ang.x, taken modulo 3, pick the field's shape. shape 0 is a
 * sphere about pos. shape 1 is a capsule about a segment that runs scl.y each
 * way from pos along y. ang.z turns the segment about z, and then ang.y about
 * y. shape 2 is a sphere about pos too. each shape reaches scl.x + 10 from its
 * center or its segment.
 *
 * within its reach, shape 0 or 1 adds a way down to a sample that holds none
 * yet. the way down points toward its center or its segment. shape 2 sets the
 * sample's way down to -y, whatever it holds. while player 0 lies within a
 * capsule's cylinder, the capsule plays sound 10 of the stage's sound list. now
 * and then it plays sound 11 too. the cylinder has a radius of scl.x, and runs
 * scl.y + scl.x each way along the segment. player 0's work must exist */
void mad_space_gravity_init(struct task *task);

#endif
