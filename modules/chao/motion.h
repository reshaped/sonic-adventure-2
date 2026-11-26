/* a garden Chao's motion, which the control in its work plays from
 * chao_motion_table */
#ifndef CHAO_MOTION_H
#define CHAO_MOTION_H
#include <sg_xpt.h>

#include "chao/motion_ids.h"
#include "draw/motion.h"

struct task;

/* the entries and their labels, one of each for each enum chao_motion_id */
#define CHAO_MOTIONS CHAO_ANIM_COUNT
extern struct motion_table_entry chao_motion_table[CHAO_MOTIONS];
extern const char *chao_motion_names[CHAO_MOTIONS];

/* an entry's pose, which chao_motion_pose returns */
enum chao_posture {
    CHAO_STANDING,
    CHAO_SITTING,
    CHAO_ON_BACK,
    CHAO_ON_FRONT,
    CHAO_OTHER_POSE
};

/* each call passes the Chao's control to its match in draw/motion.h, as
 * chao_link_motion does to motion_link. chao_motion_init gives the control
 * chao_motion_table. chao_motion_step steps it only while the Chao's flags
 * hold CHAO_ANIMATES */
void chao_motion_init(struct task *chao);
void chao_motion_step(struct task *chao);
void chao_link_motion(struct task *chao, Sint32 index);
void chao_link_motion_frames(struct task *chao, Sint32 index, Uint16 frames);
void chao_link_motion_synced(struct task *chao, Sint32 index);
void chao_set_motion(struct task *chao, Sint32 index);
void chao_set_motion_speed(struct task *chao, Float scale);
void chao_set_motion_frame(struct task *chao, Float frame);
void chao_set_motion_share(struct task *chao, Float share);
Float chao_motion_frame(struct task *chao);
Float chao_motion_share(struct task *chao);
Sint32 chao_motion_index(struct task *chao);
int chao_motion_pose(struct task *chao);
Bool chao_motion_ended(struct task *chao);
Bool chao_motion_held(struct task *chao);
void chao_set_motion_table(struct task *chao,
                           const struct motion_table_entry *table);

#endif
