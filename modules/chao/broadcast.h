/* what the radio and the television share. a broadcast's field reaches
 * BROADCAST_RADIUS from the toy and lasts BROADCAST_FRAMES, a minute */
#ifndef CHAO_BROADCAST_H
#define CHAO_BROADCAST_H
#include <ninja.h>
#include <sg_xpt.h>
#include <stdlib.h>

#include "system/angle.h"
#include "system/task.h"
#include "system/util.h"

#define BROADCAST_FRAMES 3600
#define BROADCAST_RADIUS 25.5F
/* define name, the seat call of the toy whose task the pointer toy holds.
 * with a toy, the call sets *position 10 to 13 units in front of it, and
 * returns 1. the point lies within 40 degrees of straight ahead. without a
 * toy, the call returns 0 */
#define BROADCAST_SEAT(name, toy)                                              \
    int name(NJS_POINT3 *position)                                             \
    {                                                                          \
        struct task_work *work;                                                \
        Float radius;                                                          \
        Angle yaw;                                                             \
        if (!(toy)) {                                                          \
            return 0;                                                          \
        } else {                                                               \
            work = (toy)->work;                                                \
            radius = (rand() * (3.0F * RAND_SCALE)) + 10.0F;                   \
            yaw = (Angle)(((rand() * (80.0F * RAND_SCALE)) - 40.0F) *          \
                          ANGLE_PER_DEGREE) +                                  \
                  work->ang.y + HALF_TURN;                                     \
            position->x = (njSin(yaw) * radius) + work->pos.x;                 \
            position->y = work->pos.y;                                         \
            position->z = (njCos(yaw) * radius) + work->pos.z;                 \
            return 1;                                                          \
        }                                                                      \
    }

#endif
