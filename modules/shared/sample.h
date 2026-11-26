/* the samples, SAMPLE S, SAMPLE M, SAMPLE L and SAMPLESW in a boss stage's
 * object list. a sample is a stand-in that falls as its fall kind says, and
 * draws as debug_draw_model's cylinder */
#ifndef SHARED_SAMPLE_H
#define SHARED_SAMPLE_H

struct task;

/* the object list's steps for the samples. each gives task a collision
 * record of two upright cylinders and the fall kind named below. it also
 * gives task the sample's step, draw and destructor. task needs a work and a
 * motion, unchecked.
 *
 * a step does nothing more where placement_try_leave(task) returns TRUE.
 * otherwise it runs the fall step named below. the sample draws as a
 * cylinder of the size named below, standing on pos */
/* FALL_KIND_NORMAL and fall_step_small: a radius of 2.5, 5 high. a held
 * sample draws tilted as its holder is */
void sample_s_init(struct task *task);
/* FALL_KIND_11 and fall_step_medium: a radius of 5, 10 high */
void sample_m_init(struct task *task);
/* FALL_KIND_HEAVY and fall_step_large: a radius of 15, 30 high */
void sample_l_init(struct task *task);
/* FALL_KIND_SHAKE and fall_step_fixed: a radius of 2.5, 40 high, moved by
 * the motion's acc */
void sample_sw_init(struct task *task);

#endif
