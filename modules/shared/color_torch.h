/* the colored torch: a flickering light on the players near it. it draws
 * nothing. Death Chamber's and Egg Quarters' object lists call it TAIMATSU,
 * and Pyramid Cave's calls it LIGHT */
#ifndef SHARED_COLOR_TORCH_H
#define SHARED_COLOR_TORCH_H

struct task;

/* the object list's step for the torch. task needs a work, whose scl.x is
 * the light's range. the whole part of |scl.z| modulo 3 picks the light's
 * color. the light is pink for 0, pale yellow for 1 and orange for 2.
 *
 * the call and each step first test placement_try_leave_by(task, scl.x
 * squared), and do nothing more where it returns TRUE. the call then gives
 * task the torch's own step, draw and destructor. each step lights each
 * player within range of the work's pos, as object_light_add_near does. it
 * jitters each light it adds.
 *
 * Egg Quarters' torch lights no player whose flag is set in
 * hiding_light_contacts or hiding_light_previous */
void color_torch_init(struct task *task);

#endif
