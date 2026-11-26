/* the lights that run along a rail's path */
#ifndef BOSS_LAST1_PATH_LIGHTS_H
#define BOSS_LAST1_PATH_LIGHTS_H
struct path;
struct task;

/* create an emitter under parent, which sends a light along path from its
 * first point every 11 steps. a light dies at the path's last point. path must
 * be PATH_LAYOUT_SLOPES, of 2 to 127 points, and outlive the emitter. returns
 * the emitter's task, or null for a null parent or where the heap can't give
 * it */
struct task *biolizard_path_lights_create(struct task *parent,
                                          struct path *path);
#endif
