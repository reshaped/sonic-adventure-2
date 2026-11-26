/* the Finalhazard's sparks: from its weak point as it is hit, and from its
 * body in its defeat */
#ifndef BOSS_LAST2_PANEL_SPARKS_H
#define BOSS_LAST2_PANEL_SPARKS_H
#include <ninja.h>
#include <sg_xpt.h>
struct task;
struct angles;

/* a pointer may be null only where a comment says so */

/* the sparks' settings. a spark that flies out starts within scatter of its
 * emitter on each axis. one toward the camera starts within camera_scatter */
struct panel_spark_tuning {
    Float scatter;
    Float camera_scatter;
    Float speed_range;
    Float speed; /* a spark's push, plus up to speed_range */
    Float size; /* the least size a spark starts at, and half the most */
    Float growth_range;
    Float size_limit; /* the size past which a spark grows no more */
    Float growth; /* a spark's growth a step, plus up to growth_range */
    Float drag; /* the share of its push a spark keeps each step */
    Float fade; /* a spark's loss of brightness a step, once it fades */
    Float camera_speed_range;
    /* the push of a spark toward the camera, plus up to camera_speed_range */
    Float camera_speed;
};
extern struct panel_spark_tuning finalhazard_spark_tuning;
/* a spark's work: its size, its growth a step and its brightness in scl.x,
 * scl.y and scl.z. submode holds the texture it shows, counted past the
 * first, and tick its kind */
#define SPARK_SIZE(work) ((work)->scl.x)
#define SPARK_GROWTH(work) ((work)->scl.y)
#define SPARK_BRIGHTNESS(work) ((work)->scl.z)
#define SPARK_TEXTURE(work) ((work)->submode)
#define SPARK_KIND(work) ((work)->tick)
/* a spark's step, which panels.c holds */
void finalhazard_panel_spark_step(struct task *task);
/* create a child of parent that throws sparks from *front for 60 steps. it
 * moves with front, which must outlive it. angles isn't read. returns the
 * task, or null without parent or where the heap can't give it */
struct task *finalhazard_hit_sparks(struct task *parent,
                                    const NJS_POINT3 *front,
                                    struct angles *angles);
/* create a child of parent that throws sparks for 30 steps from a slot's
 * front. a front is the point before a weak point there. the slot is one of
 * the first five of a row, picked at random. returns the task, or null as
 * finalhazard_hit_sparks does */
struct task *finalhazard_body_sparks(struct task *parent);
/* make an explosion at a slot picked as finalhazard_body_sparks picks one.
 * it starts within 10 of the slot's front along the slot's own x and z. it
 * flies away from the origin, finalhazard_center */
void finalhazard_body_explosion(void);
#endif
