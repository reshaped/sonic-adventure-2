/* the water's specks: a cloud of specks that drift in the water about the
 * camera */
#ifndef SHARED_WATER_SPECKS_H
#define SHARED_WATER_SPECKS_H
#include <ninja.h>
#include <sg_xpt.h>

struct task;

/* the specks' texture list, which holds the one texture miu32_mi001 */
extern NJS_TEXLIST water_specks_texlist;

/* the cloud's modes. its owner sets one in the task's work->mode */
enum water_specks_mode { MODE_SPECKS_SHOWN, MODE_SPECKS_HIDDEN };

/* make a cloud of count specks, a task named MizugomiExec in task level 1, and
 * return it. it returns null, and makes nothing, where player_work[player] is
 * null. it returns null too where the heap has no room for the task or either
 * record. it then frees what it took. where handle isn't null, *handle takes
 * the task. the task's end sets *handle back to null while it still holds the
 * task.
 *
 * the cloud lies about the point halfway from camera 0 to player 0, whatever
 * player is. while camera 0 isn't in use, it lies about player 0. the task ends
 * on the first step that finds player_work[player] null. player_work[0] must
 * not be null before then, unchecked. the specks drift with the water, and keep
 * their places in the world as the cloud moves. in MODE_SPECKS_HIDDEN they move
 * with the cloud, and don't draw */
struct task *water_specks_create(int player, int count, struct task **handle);

#endif
