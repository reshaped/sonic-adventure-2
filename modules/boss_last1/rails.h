/* the Biolizard's rails, turned copies of its paths. a rail's task takes a
 * player onto it, and lights run along it */
#ifndef BOSS_LAST1_RAILS_H
#define BOSS_LAST1_RAILS_H
#include <ninja.h>
#include <sg_xpt.h>
struct path;
struct task;

/* the Biolizard's six rail paths, then a null */
extern struct path **biolizard_rail_paths;
/* create a set of rails, one for each of the count paths from paths[0]. each
 * rail is its path's copy, turned by angle about the y axis. each path must
 * be PATH_LAYOUT_SLOPES and have a step, unchecked. paths must hold an entry
 * past the last, which the call reads. a set that starts at the first or the
 * second of biolizard_rail_paths has a hazard too. the hazard is a sphere
 * that hurts players, at the first rail's fourth point.
 *
 * returns the first rail's task, or null for a null paths or while another
 * set lives. the first rail kills the rest as it dies. as the set's last rail
 * dies, player 0 leaves its path. the heap must give each rail's copy, task
 * and link: a failure leaks or reads through null */
struct task *biolizard_rails_create(struct path **paths, Sint8 count,
                                    Angle angle);
/* the set's first rail's path, or null without a set */
struct path *biolizard_active_rail(void);
#endif
