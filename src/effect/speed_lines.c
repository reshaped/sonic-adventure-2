/* speed lines: short lines that streak back from a running player */
#include <ninja.h>
#include <sg_xpt.h>
#include <shinobi.h>
#include <stdlib.h>

#include "draw/lines.h"
#include "effect/speed_lines.h"
#include "player/player.h"
#include "system/task.h"
#include "system/util.h"

/* the task level of each line */
#define EFFECT_LEVEL 3

/* a line's count of steps left starts at LINE_STEPS. the step that finds it at
 * 0 ends the line, which is the line's step LINE_STEPS + 1 */
#define LINE_STEPS 2

/* a line's colors at its near end and its far end, as 0xAARRGGBB */
#define LINE_HEAD_COLOR 0x30b0b0b0
#define LINE_TAIL_COLOR 0x20303030

/* a line's near end lies at random within half of LINE_SPREAD of pos + dir
 * along each axis */
#define LINE_SPREAD 6.0F
/* a line's far end moves back by dir times LINE_BACK_MIN up to LINE_BACK_SPAN
 * more */
#define LINE_BACK_MIN 2.0F
#define LINE_BACK_SPAN 4.0F

/* a line's record, in its task's extra */
struct speed_line {
    Sint8 unk_00; /* nothing reads it */
    Sint8 player;
    Sint16 steps; /* left before the line ends */
    Uint32 color; /* the near end's color */
    NJS_POINT3 from; /* the near end */
    NJS_POINT3 to; /* the far end, which takes LINE_TAIL_COLOR */
};

static void speed_line_destroy(struct task *task)
{
}

static void speed_line_step(struct task *task)
{
    struct speed_line *line = task->extra.p;

    if (player_work[line->player] == NULL) {
        task_kill(task);
        return;
    }
    if (line->steps-- <= 0) {
        task_kill(task);
    }
}

static void speed_line_draw(struct task *task)
{
    struct speed_line *line = task->extra.p;
    /* @hack spelling: ends declared ahead of colors, not after them */
    NJS_POINT3 ends[2];
    NJS_COLOR colors[2];

    colors[0].color = line->color;
    colors[1].color = LINE_TAIL_COLOR;
    ends[0] = line->from;
    ends[1] = line->to;
    line_draw_now(ends, colors, LINE_ADDITIVE);
}

void speed_lines_create(int player, int count, NJS_POINT3 *pos, NJS_VECTOR *dir)
{
    struct task *task;
    struct speed_line *line;
    NJS_VECTOR offset;
    Float length;
    Float back;
    int i;

    /* @hack spelling: length holding dir's length before its root, where one
     * expression would do */
    length = njScalor(dir);
    length = njSqrt(length / player_records[player]->consts.sprint_speed);
    for (i = 0; i < count; i++) {
        /* @hack spelling: the assignment inside the test, where a statement
         * of its own would do */
        if ((task = task_create(0, EFFECT_LEVEL, speed_line_step,
                                "LineExecute")) == NULL) {
            break;
        }
        /* @hack spelling: the assignment inside the test, where a statement
         * of its own would do */
        if ((line = syCalloc(1, sizeof(*line))) == NULL) {
            task_kill(task);
            break;
        }
        line->player = player;
        line->from = *pos;
        offset.x = (0.5F - (rand() * RAND_SCALE)) * LINE_SPREAD;
        offset.y = (0.5F - (rand() * RAND_SCALE)) * LINE_SPREAD;
        offset.z = (0.5F - (rand() * RAND_SCALE)) * LINE_SPREAD;
        line->from.x += offset.x + dir->x;
        line->from.y += offset.y + dir->y;
        line->from.z += offset.z + dir->z;
        njUnitVector(&offset);
        back = (rand() * (LINE_BACK_SPAN * RAND_SCALE)) + LINE_BACK_MIN;
        line->to.x = line->from.x + (offset.x * length) - (dir->x * back);
        line->to.y = line->from.y + (offset.y * length) - (dir->y * back);
        line->to.z = line->from.z + (offset.z * length) - (dir->z * back);
        line->color = LINE_HEAD_COLOR;
        line->steps = LINE_STEPS;
        task->extra.p = line;
        task->draw = speed_line_draw;
        task->destructor = speed_line_destroy;
    }
}
