/* what a Chao sees: the player, other Chao, fruit, trees and toys */
#ifndef CHAO_PERCEPTION_H
#define CHAO_PERCEPTION_H
#include <ninja.h>
#include <sg_xpt.h>

struct garden_entry;
struct task;

#define CHAO_PERCEPTION_CAPACITY 16
/* the toys the garden can register */
#define CHAO_PERCEIVED_TOYS 32
/* a chao_perceived_group's bytes before its objects */
#define CHAO_GROUP_HEADER_BYTES 24

/* an object in sight. entry is NULL for the player */
struct chao_perceived_object {
    Sint16 info[4];
    Float distance;
    Bool in_sight, heard, smelled;
    struct garden_entry *entry;
};

/* the objects of one kind in sight, in objects[0] to objects[count - 1] */
struct chao_perceived_group {
    Sint16 count;
    Bool in_sight, heard, smelled;
    Float nearest_distance;
    Sint16 nearest; /* the nearest's index in objects, or -1 for none */
    struct chao_perceived_object objects[CHAO_PERCEPTION_CAPACITY];
};

struct chao_perception {
    Float sight_range;
    Angle sight_angle, sight_half_angle;
    Float hearing_range, smell_range;
    struct chao_perceived_group player, chao, fruit, tree;
    union {
        struct {
            struct chao_perceived_group toy, sound;
        } groups;
        /* the toy group's objects, widened to CHAO_PERCEIVED_TOYS. the 17th
         * on runs over the sound group */
        struct {
            Uint8 header[CHAO_GROUP_HEADER_BYTES];
            struct chao_perceived_object objects[CHAO_PERCEIVED_TOYS];
        } all_toys;
    } tail;
};

/* clear what a Chao sees, and set the reach of its senses */
void chao_perception_init(struct task *chao);
/* does nothing */
void chao_listen(struct task *chao);
/* look again for the player, other Chao, fruit, trees and toys. an object is
 * in sight within sight_range, and within half of sight_angle of the Chao's
 * heading. the result is undefined: the image's function ends without a
 * return */
int chao_notice_objects(struct task *chao);
/* the nearest object of a kind in sight, or NULL. a _task call returns its
 * task, player_tasks[0] for the player, or NULL */
struct chao_perceived_object *chao_visible_player(struct task *chao);
struct task *chao_visible_player_task(struct task *chao);
struct chao_perceived_object *chao_nearest_visible_fruit(struct task *chao);
struct task *chao_nearest_fruit_task(struct task *chao);
struct task *chao_nearest_tree_task(struct task *chao);
struct chao_perceived_object *chao_nearest_visible_toy(struct task *chao);
struct task *chao_nearest_toy_task(struct task *chao);
struct chao_perceived_object *chao_nearest_visible_tree(struct task *chao);
#endif
