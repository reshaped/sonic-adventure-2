/* King Boom Boo's two parts: nodes of his model that tilt as his mode picks.
 * a part sways, leans toward player 0, spins, or folds to no tilt */
#ifndef BOSS_BIGBOGY_PARTS_H
#define BOSS_BIGBOGY_PARTS_H
#include <ninja.h>
#include <sg_xpt.h>

struct task;
struct task_work;
struct bigbogy_part {
    Sint8 side; /* 0 or 1, which sets the way the part rests and spins */
    Uint8 unused[7];
    NJS_CNK_OBJECT *object; /* the node whose ang the part sets */
    const struct task_work *boss; /* his work, whose mode picks the pose */
    const Sint8 *boss_submode; /* nothing reads it */
    const Angle *boss_heading; /* his ang.y */
    const NJS_POINT3 *position; /* where the node lies in the world */
};
/* make a part of *spec as parent's child. the part sets object's ang each
 * step, and puts its first ang back as its task ends. object, *boss,
 * *boss_heading and *position must outlive the task. the call returns null
 * where an allocation fails. parent must not be null, or the call returns an
 * unset pointer */
struct task *bigbogy_part_create(struct task *parent,
                                 const struct bigbogy_part *spec);
/* turn the part to fold, unless it is folded or already turning to a pose.
 * once folded, the boss's mode picks its pose again */
void bigbogy_part_fold(struct task *task);
#endif
