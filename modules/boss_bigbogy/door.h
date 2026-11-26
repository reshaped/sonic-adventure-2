/* King Boom Boo's sandglass door, DOOR in the stage's object list. it opens
 * while the sandglass of its id runs, and shuts in notches as the sand runs
 * out */
#ifndef BOSS_BIGBOGY_DOOR_H
#define BOSS_BIGBOGY_DOOR_H

struct task;

/* the object list's step for DOOR. it does nothing more where
 * placement_try_leave(task) returns TRUE. task needs a work and a motion
 * record. a door with an ang.x of 0 stands in a wall and rises. any other
 * lies in the floor and slides along ang.y. ang.y turns the door. ang.z is
 * the id of the sandglass it follows, from 0 to 255. where no registered
 * sandglass has that id, the door stays where it stands */
void bigbogy_door_init(struct task *task);
#endif
