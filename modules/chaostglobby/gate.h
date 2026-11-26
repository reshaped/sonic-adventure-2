/* the Chao Lobby's gates to the three gardens and the kindergarten, and what
 * their sources share */
#ifndef CHAOSTGLOBBY_GATE_H
#define CHAOSTGLOBBY_GATE_H
#include <ninja.h>
#include "collision/collision.h"
#include "system/task.h"
#include "system/util.h"

#define GATE_SHADOW (NJD_CONTROL_3D_SHADOW | NJD_CONTROL_3D_TRANS_MODIFIER)
#define GATE_SHAPES 8
/* the shape that takes a player who touches it through the gate */
#define GATE_DOORWAY_SHAPE 0

extern NJS_TEXLIST lobby_texlist;
extern NJS_CNK_OBJECT lobby_threshold;
extern struct collision_shape lobby_neutral_shapes[GATE_SHAPES];
extern int lobby_gate_shape_count;

/* each makes a gate at pos, turned by yaw. a player who touches its
 * GATE_DOORWAY_SHAPE goes to the gate's area by chao_change_area. each
 * gate's collision record takes GATE_SHAPES shapes, whatever its own array
 * holds. the hero gate takes lobby_neutral_shapes. each returns its gate's
 * task, or null when task_create fails */
struct task *lobby_dark_gate_create(NJS_POINT3 *pos, Angle yaw);
struct task *lobby_hero_gate_create(NJS_POINT3 *pos, Angle yaw);
struct task *lobby_kinder_gate_create(NJS_POINT3 *pos, Angle yaw);
struct task *lobby_neutral_gate_create(NJS_POINT3 *pos, Angle yaw);

#define GATE_DRAW_BEGIN(work)                                                  \
    do {                                                                       \
        njSetTexture(&lobby_texlist);                                          \
        draw_control_set(GATE_SHADOW);                                         \
        njPushMatrixEx();                                                      \
        njTranslateEx(&(work)->pos);                                           \
        njRotateY(NULL, (work)->ang.y);                                        \
    } while (0)

#define GATE_INSTALL(task, work, position, yaw, shapes, count, display,        \
                     destroy)                                                  \
    do {                                                                       \
        collision_init((task), (shapes), (count), COLLISION_OBJECT);           \
        (task)->draw = (display);                                              \
        (task)->destructor = (destroy);                                        \
        (work)->pos = *(position);                                             \
        (work)->ang.y = (yaw);                                                 \
    } while (0)

#endif
