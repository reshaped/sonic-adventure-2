/* the switches of a garden Chao's collision shapes, and the flag that pauses
 * the Chao's step */
#ifndef CHAO_BODY_COLLISION_H
#define CHAO_BODY_COLLISION_H
#include <sg_xpt.h>

struct task;

/* the Chao's first three collision shapes, by index. garden_hit_kind finds a
 * field by whether it touches CHAO_PUSH_SHAPE */
enum chao_collision_shape {
    CHAO_CONTACT_SHAPE,
    CHAO_PUSH_SHAPE,
    CHAO_SWIM_SHAPE
};
/* the id of CHAO_PUSH_SHAPE, the sphere about a Chao's body, by which a ball
 * or another Chao finds it */
#define CHAO_BODY_HIT 129

extern Bool garden_objects_enabled;
void garden_enable_objects(void);
void garden_disable_objects(void);
/* garden_objects_enabled. object is unread */
Bool garden_objects_running(struct task *object);

/* start or stop the Chao pushing players, enemies and objects */
void chao_enable_push(struct task *chao);
void chao_disable_push(struct task *chao);
/* stop or let players, enemies and objects push the Chao */
void chao_resist_displacement(struct task *chao);
void chao_allow_displacement(struct task *chao);
/* turn on or off the Chao's two sensor shapes. each also sets or clears the
 * work flags that offer the Chao to a player to lift */
void chao_enable_body_shapes(struct task *chao);
void chao_disable_body_shapes(struct task *chao);
/* what chao_disable_swim_shape does. no code calls it */
void chao_swim_shape_off(struct task *chao);
/* turn off the smaller of the Chao's two sensor shapes, until
 * chao_enable_body_shapes */
void chao_disable_swim_shape(struct task *chao);

#endif
