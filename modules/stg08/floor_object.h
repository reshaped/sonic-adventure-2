/* the floor test and the world classes of Security Hall's DOBJF, CONT and
 * STAGE */
#ifndef STG08_FLOOR_OBJECT_H
#define STG08_FLOOR_OBJECT_H

/* height divides the two floors. where work lies on another floor than
 * camera 0, the macro returns from the step. first it clears the flags that
 * inactive_mask lacks and sets tick to 0. otherwise it sets tick to 1 */
#define SELECT_FLOOR(work, height, inactive_mask)                              \
    if (camera_views[0]->pos.y > (height)) {                                   \
        if ((work)->pos.y > (height)) {                                        \
            (work)->tick = 1;                                                  \
        } else {                                                               \
            (work)->flags &= (inactive_mask);                                  \
            (work)->tick = 0;                                                  \
            return;                                                            \
        }                                                                      \
    } else if ((work)->pos.y > (height)) {                                     \
        (work)->flags &= (inactive_mask);                                      \
        (work)->tick = 0;                                                      \
        return;                                                                \
    } else {                                                                   \
        (work)->tick = 1;                                                      \
    }

#define CLASSIFY_WORLD(flags, radius, base)                                    \
    if (!((radius) > WORLD_SIZE_CLASS_1_MAX)) {                                \
        if ((radius) > WORLD_SIZE_CLASS_2_MAX) {                               \
            (flags) = WORLD_SIZE_CLASS(1) | (base);                            \
        } else if ((radius) > WORLD_SIZE_CLASS_3_MAX) {                        \
            (flags) = WORLD_SIZE_CLASS(2) | (base);                            \
        } else {                                                               \
            (flags) = WORLD_SIZE_CLASS(3) | (base);                            \
        }                                                                      \
    }

#define CLASSIFY_SOLID(flags, radius) CLASSIFY_WORLD(flags, radius, WORLD_SOLID)

#endif
