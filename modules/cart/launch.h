/* the launch that the jump ramp and the dash panel share. in the object's
 * work, scl.x is the launch's speed and scl.y the ramp's rise. a launch
 * that plays its driver's sound sets scl.z to 30. scl.z then counts down a
 * frame at a time. no launch plays one while it is above 0 */
#ifndef CART_LAUNCH_H
#define CART_LAUNCH_H
#include <ninja.h>
#include <sg_xpt.h>

#include "cart/vehicle.h"
#include "collision/collision.h"
#include "system/task.h"
#include "vmu/vibration.h"

/* define launch_contacts. each frame it pushes every racer the work's shape
 * touches by (0, rise, forward). the push turns with the work's yaw, and a
 * player's kart vibrates. each object's source defines it static, for SHC
 * to inline into the step */
#define DEFINE_LAUNCH_CONTACTS(rise, forward)                                  \
    void launch_contacts(struct task_work *work)                               \
    {                                                                          \
        struct task *other;                                                    \
        struct cart_vehicle *kart;                                             \
        NJS_VECTOR push;                                                       \
        int i;                                                                 \
                                                                               \
        work->scl.z -= 1.0F;                                                   \
        if (work->scl.z < 0.0F) {                                              \
            work->scl.z = 0.0F;                                                \
        }                                                                      \
        if (work->collision->flags & COLLISION_TOUCHED) {                      \
            for (i = 0; i < work->collision->hit_count; i++) {                 \
                other = work->collision->hits[i].task;                         \
                if (cart_is_racer(other)) {                                    \
                    push.x = 0.0F;                                             \
                    push.y = (rise);                                           \
                    push.z = (forward);                                        \
                    njPushMatrixEx();                                          \
                    njUnitMatrix(NULL);                                        \
                    njRotateY(NULL, work->ang.y);                              \
                    njCalcVector(NULL, &push, &push);                          \
                    njPopMatrixEx();                                           \
                    cart_launch(other, &push);                                 \
                    kart = other->motion.p;                                    \
                    if (kart->start.player_controlled) {                       \
                        vibration_start(kart->driver, 4, 15, 0);               \
                    }                                                          \
                    if (work->scl.z == 0.0F) {                                 \
                        if (cart_driver_sound(other, CART_LAUNCH_SOUND)) {     \
                            work->scl.z = 30.0F;                               \
                        }                                                      \
                    }                                                          \
                }                                                              \
            }                                                                  \
        }                                                                      \
    }
#endif
