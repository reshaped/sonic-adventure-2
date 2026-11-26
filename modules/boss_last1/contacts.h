/* the tasks that follow the Biolizard's joints for its splashes and the
 * sounds of its swings. the header also declares the energy spark's init,
 * which spark.c defines */
#ifndef BOSS_LAST1_CONTACTS_H
#define BOSS_LAST1_CONTACTS_H
#include <ninja.h>
#include <sg_xpt.h>
struct task;

/* each tracker reads its points every step, so they must outlive it. each
 * returns its task, or null for a null parent or where the heap can't give
 * the task */

/* splash where point's height plus offset falls below 1. it splashes again
 * only once that height has risen above 4. the first splash waits for that
 * too */
struct task *biolizard_track_splash(struct task *parent,
                                    const NJS_POINT3 *point, Float offset);
/* sound once second, having swung across first's bearing from the arena's
 * center, lies 512 past it. sound again as second, once over 100 above
 * first, falls back below first's height plus 5 */
struct task *biolizard_track_swing(struct task *parent, const NJS_POINT3 *first,
                                   const NJS_POINT3 *second);
/* the init of an energy spark, the set object whose kind object_kinds.c
 * names ENESPARK. its work's submode picks its model, and must be 0 or 1,
 * unchecked */
void biolizard_spark_init(struct task *task);
#endif
