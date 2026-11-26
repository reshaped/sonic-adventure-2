/* sparks: tasks that fly from a point, fall and end */
#ifndef SHARED_SPARK_H
#define SHARED_SPARK_H
#include <ninja.h>

/* throw count sparks from pos, each a task of its own at task level 3. a
 * spark flies at spd plus a random part. where spd is null, it flies at a
 * random rising speed. it falls, and ends some steps later. the call stops at
 * the first spark the heap has no room for */
void spark_throw(int count, NJS_POINT3 *pos, NJS_VECTOR *spd);

#endif
