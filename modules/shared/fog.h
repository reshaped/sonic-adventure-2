/* fog: a stage's fog settings, and the two tasks that apply them to Ninja's
 * fog at each step */
#ifndef SHARED_FOG_H
#define SHARED_FOG_H
#include <ninja.h>
#include <sg_xpt.h>

/* the one bit of a setting's flags that the fog task reads. with it, a
 * scroll moves the table's entries. without it, a scroll moves their slopes.
 * an entry's slope is the entry less the next one, and entry 127's slope is
 * its own value. each entry of the scrolled table is the sum of the moved
 * slopes from it to entry 127 */
#define FOG_SCROLL_ENTRIES 0x1

/* a fog setting, as a stage's fog file holds it */
struct fog_setting {
    Uint32 flags;
    Uint32 color; /* as njSetFogColor takes it */
    /* the fog task sets the fog density for this depth, as
     * njGenerateFogTable3 does for its far depth. far must not be below 0,
     * unchecked */
    Float far;
    /* the scroll's speed, in entries a step of game_state.steps. 0 holds the
     * table still. the scroll starts over every 128 / |speed| steps, cut to
     * an integer. speed must lie from -128 to 128, unchecked. past that, the
     * count of steps cuts to 0. the runtime gives a remainder by 0 as 0, so
     * the table then holds where the scroll starts.
     *
     * under FOG_SCROLL_ENTRIES, a speed above 0 moves the entries toward
     * entry 0. without the flag, a speed above 0 moves the slopes toward
     * entry 127. a speed below 0 moves either the other way. each scrolled
     * entry is held to -1 through 1. without FOG_SCROLL_ENTRIES, a table of
     * entries from 0 to 1 can scroll to entries below 0 */
    Float speed;
    NJS_FOG_TABLE table;
};

/* clear setting to no flags, color 0, a far of 100000, speed 0 and a table
 * of zeros */
void fog_clear(struct fog_setting *setting);
/* read the file name into WORK_AREA, and copy its first
 * sizeof(struct fog_setting) bytes to setting. the port keeps the previous
 * setting and returns FALSE if the read fails or the file is too short. the
 * original checks only the open */
Bool fog_read(const char *name, struct fog_setting *setting);

/* the fog is two tasks, a manager at task level 1 and the fog task at
 * level 6. the manager creates the fog task at each of its steps that finds
 * none. each step of the fog task applies its latest setting or blend to
 * Ninja's fog. the calls below keep pointers to their settings, which must
 * stay in place while the fog runs */

/* start the fog with no setting to apply. it calls njFogDisable, so Ninja's
 * fog shows only once the caller calls njFogEnable. where the fog runs
 * already, it does nothing. where the heap can't give the manager's task,
 * the fog stays stopped */
void fog_start(void);
/* start the fog as fog_start does, but with setting to apply. the call
 * clears setting with fog_clear, then reads it from the file name. a file
 * that won't open leaves setting clear. where the fog runs already, setting
 * stays as it was */
void fog_start_file(const char *name, struct fog_setting *setting);
/* destroy the manager's task, if there is one, and with it the fog task.
 * Ninja's fog keeps what the fog task last applied */
void fog_stop(void);
/* make the fog task apply setting, with no blend, from the task's next step */
void fog_set(struct fog_setting *setting);
/* make the fog task apply the blend of from and to by mix, from the task's
 * next step. the task holds mix to 0 through 1. far, the color's bytes and
 * the table's entries each blend as from's + (to's - from's) * mix. each
 * color byte is then cut toward 0.
 *
 * a mix above 0.999 takes to's table as it is. where from's speed is 0, a mix
 * of 0.001 or less takes from's table as it is. otherwise the task scrolls
 * from's table by its speed before the blend. to's table never scrolls */
void fog_set_blend(struct fog_setting *from, struct fog_setting *to, Float mix);

#endif
