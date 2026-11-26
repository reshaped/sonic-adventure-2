/* the turntable, TERMINAL in Pumpkin Hill's object list. it turns among three
 * ways. it sends a ghost train of its own along the way it turns to. turning on
 * a way's switch takes the table to that way. the table carries the players on
 * it as it turns */
#include <ninja.h>
#include <shinobi.h>
#include <sg_xpt.h>

#include "collision/world.h"
#include "draw/lines.h"
#include "player/player.h"
#include "shared/switch.h"
#include "sound/sound.h"
#include "stg05/ghost_train.h"
#include "stg05/turntable.h"
#include "system/angle.h"
#include "system/task.h"

/* the set editor's labels for the table's placement, which no code reads */
static const char turntable_edit_angle_sw1[16] = "<- ANGLE(SW1)";
static const char turntable_edit_angle_sw2[16] = "<- ANGLE(SW2)";
static const char turntable_edit_angle_sw3[16] = "<- ANGLE(SW3)";
static const char turntable_edit_scl_len_sw1[16] = "<- SCL LEN(SW1)";
static const char turntable_edit_scl_len_sw2[16] = "<- SCL LEN(SW2)";
static const char turntable_edit_scl_len_sw3[16] = "<- SCL LEN(SW3)";

#include "stg05/turntable/text.inc"
#include "stg05/scenery_art/turntable.h"

/* the table's record, in task->motion */
struct turntable {
    /* the table's copy of turntable_collision in the collision world, placed
     * and turned as the table is */
    NJS_OBJECT object;
    Angle angle; /* the table's angle about y */
};

#define TABLE(task) ((struct turntable *)(task)->motion.p)

/* the table's modes, in its work's submode */
enum turntable_mode {
    /* it waits for another way's switch. it fades in the way's train where it
     * has no train of its own. it does so too once its own has faded out at
     * its line's end */
    MODE_WAIT,
    /* it fades out the ways' trains. once all are gone, it fades out its own,
     * and kills it once that is gone */
    MODE_CALL_BACK,
    /* it turns to the way, then sends a new train along it */
    MODE_SEND
};

/* the work's tick holds the way the table turns to, from 0 to WAYS - 1 */
#define WAYS 3
#define WAY_SCALE 10.0F
/* the id of way's switch, and of its train */
#define WAY_ID(way) ((way) + 1)
/* the table starts on HOME_WAY. a train it sends along HOME_WAY runs to and
 * fro. one it sends along another way fades out at its line's end */
#define HOME_WAY 1
#define WAY_MARGIN 80.0F
#define WAY_HEIGHT 5.0F
#define MAX_TURN NJM_DEG_ANG(1.5)
#define TABLE_HEIGHT 2.0F
/* the table takes its size class from its model's radius plus REACH_MARGIN.
 * its copy collides while a player lies within that reach of the copy */
#define REACH_MARGIN 5.0F
#define DRAW_RANGE 2000.0F
/* the table's sounds in pumpkin_hill_sounds. each step the table turns plays
 * TURN_SOUND anew, for TURN_DURATION + 1 steps. SEND_SOUND plays as a call
 * back ends, and again as the table sends its train */
#define SEND_SOUND 4
#define SEND_PRIORITY 0
#define TURN_SOUND 3
#define TURN_PRIORITY 1
#define TURN_VOLUME 50
#define TURN_DURATION 10
/* SEND_SOUND's volume as a call back ends, by whether the table had a train
 * to kill */
#define KILLED_VOLUME 50
#define NO_TRAIN_VOLUME 80
#define SEND_VOLUME 0

NJS_TEXLIST *turntable_texlists[] = {&turntable_texlist, NULL};

static void turntable_destroy(struct task *task);
static void turntable_step(struct task *task);
static void turntable_draw(struct task *task);

static Angle way_angle(struct task_work *work, int way)
{
    switch (way) {
    case 1:
        return work->ang.y;
    case 2:
        return work->ang.z;
    default:
        return work->ang.x;
    }
}

static Float way_length(struct task_work *work, int way)
{
    switch (way) {
    case 1:
        return work->scl.y * WAY_SCALE;
    case 2:
        return work->scl.z * WAY_SCALE;
    default:
        return work->scl.x * WAY_SCALE;
    }
}

/* set *start and *end to the ends of way's line, in the world's axes */
static void way_ends(struct task_work *work, NJS_POINT3 *start, NJS_POINT3 *end,
                     int way)
{
    start->x = 0.0F;
    start->y = WAY_HEIGHT;
    start->z = -WAY_MARGIN;
    end->x = 0.0F;
    end->y = WAY_HEIGHT;
    end->z = way_length(work, way) + WAY_MARGIN;
    njPushMatrixEx();
    njUnitMatrix(NULL);
    njTranslateV(NULL, &work->pos);
    njRotateY(NULL, way_angle(work, way));
    njCalcPoint(NULL, start, start);
    njCalcPoint(NULL, end, end);
    njPopMatrix(1);
}

/* the angle of the work's way, which the table turns to */
static Angle aim_angle(struct task_work *work)
{
    switch (work->tick) {
    case 1:
        return work->ang.y;
    case 2:
        return work->ang.z;
    default:
        return work->ang.x;
    }
}

void turntable_init(struct task *task)
{
    struct task_work *work = task->work;
    NJS_OBJECT *object;
    Float reach;
    Uint32 flags;

    task->motion.p = syCalloc(1, sizeof(struct turntable));
    if (task->motion.p == NULL) {
        return;
    }
    task->draw = turntable_draw;
    task->step = turntable_step;
    task->destructor = turntable_destroy;
    work->submode = MODE_CALL_BACK;
    work->tick = HOME_WAY;
    TABLE(task)->angle = aim_angle(work);
    object = &TABLE(task)->object;
    *object = turntable_collision;
    object->evalflags &= ~(NJD_EVAL_UNIT_POS | NJD_EVAL_UNIT_ANG);
    object->ang[1] = TABLE(task)->angle;
    object->ang[2] = 0;
    object->ang[0] = 0;
    object->pos[0] = work->pos.x;
    object->pos[1] = work->pos.y + TABLE_HEIGHT;
    object->pos[2] = work->pos.z;
    flags = WORLD_SOLID | WORLD_MOVING;
    reach = object->model->r + REACH_MARGIN;
    if (reach <= WORLD_SMALL_SIZE_CLASS_1_MAX) {
        if (reach > WORLD_SMALL_SIZE_CLASS_2_MAX) {
            flags |= WORLD_SIZE_CLASS(1);
        } else if (reach > WORLD_SMALL_SIZE_CLASS_3_MAX) {
            flags |= WORLD_SIZE_CLASS(2);
        } else {
            flags |= WORLD_SIZE_CLASS(3);
        }
    }
    world_add(flags, task, object);
}

static void turntable_destroy(struct task *task)
{
    if (TABLE(task)->object.model != NULL) {
        world_remove(task, &TABLE(task)->object);
    }
    syFree(task->motion.p);
    task->motion.p = NULL;
}

static void turntable_step(struct task *task)
{
    struct task_work *work = task->work;
    struct task *train;
    struct task *sent;
    struct turntable *table;
    NJS_OBJECT *object;
    NJS_POINT3 start;
    NJS_POINT3 end;
    Float distance;
    Sint16 step;
    Angle turn;
    Angle last;
    int calling;
    int way;
    int i;

    /* @hack redundant: a block no path reaches, which draws each way's line
     * between its ends */
    if (0) {
        for (i = 0; i < WAYS; i++) {
            way_ends(work, &start, &end, i);
            line_draw_points(&start, &end);
        }
        return;
    }
    switch (work->submode) {
    case MODE_CALL_BACK:
        calling = 0;
        /* @hack spelling: the count written i = i + 1, where i++ would do */
        for (i = 1; i < WAYS + 1; i = i + 1) {
            train = ghost_train_find(i);
            if (train != NULL && train->work->mode != GHOST_TRAIN_GONE) {
                if (ghost_train_fade_out(train)) {
                    calling++;
                }
            }
        }
        if (!calling) {
            if (task->child != NULL) {
                if (task->child->work->mode == GHOST_TRAIN_GONE) {
                    task_kill(task->child);
                    work->submode = MODE_SEND;
                    sound_play_at(SEND_SOUND, NULL, SEND_PRIORITY,
                                  KILLED_VOLUME, &work->pos);
                } else {
                    ghost_train_fade_out(task->child);
                }
            } else {
                work->submode = MODE_SEND;
                sound_play_at(SEND_SOUND, NULL, SEND_PRIORITY, NO_TRAIN_VOLUME,
                              &work->pos);
            }
        }
        break;
    case MODE_SEND:
        if (task->child == NULL) {
            /* the table's angle can pass a whole turn, so the angles compare
             * by their low 16 bits */
            if ((Uint16)aim_angle(work) == (Uint16)TABLE(task)->angle) {
                sent = task_create_child(TASK_WORK | TASK_FORCE,
                                         ghost_train_init, task);
                if (sent != NULL) {
                    sound_play_at(SEND_SOUND, NULL, SEND_PRIORITY, SEND_VOLUME,
                                  &work->pos);
                    way_ends(work, &start, &end, work->tick);
                    distance = njDistanceP2P(&start, &end);
                    sent->work->pos.x = (start.x + end.x) * 0.5F;
                    sent->work->pos.y = (start.y + end.y) * 0.5F;
                    sent->work->pos.z = (start.z + end.z) * 0.5F;
                    sent->work->ang.x = GHOST_TRAIN_LINE;
                    sent->work->ang.y = aim_angle(work);
                    sent->work->ang.z = GHOST_TRAIN_TURNTABLE;
                    sent->work->scl.x = GHOST_TRAIN_BASE_SPEED;
                    sent->work->scl.y = GHOST_TRAIN_NO_PIECE;
                    sent->work->scl.z =
                        (distance * (1.0F / GHOST_TRAIN_LINE_SCALE)) - 1.0F;
                    if (work->tick == HOME_WAY) {
                        sent->work->submode = GHOST_TRAIN_FADES;
                    } else {
                        sent->work->submode = GHOST_TRAIN_FADES_AT_END;
                    }
                }
            }
        } else {
            work->submode = MODE_WAIT;
        }
        break;
    case MODE_WAIT:
        if (task->child == NULL ||
            (task->child->work->submode == GHOST_TRAIN_FADES_AT_END &&
             task->child->work->mode == GHOST_TRAIN_GONE)) {
            train = ghost_train_find(WAY_ID(work->tick));
            if (train != NULL) {
                switch (train->work->mode) {
                case GHOST_TRAIN_FADING_OUT:
                case GHOST_TRAIN_GONE:
                    ghost_train_fade_in(train);
                    break;
                }
            }
        }
        way = -1;
        for (i = 0; i < WAYS; i++) {
            if (work->tick != i && switch_is_on(WAY_ID(i))) {
                way = i;
            }
        }
        /* @hack type: the tick and the way cast to Sint8 for the test, where
         * work->tick != way would do */
        if (way >= 0 && (Sint8)work->tick != (Sint8)way) {
            work->submode = MODE_CALL_BACK;
            work->tick = way;
        }
        break;
    }
    for (i = 0; i < WAYS; i++) {
        switch_set(WAY_ID(i), work->tick == i);
    }
    if (work->submode != MODE_CALL_BACK) {
        /* as a Sint16, the difference turns the table the shorter way round */
        step = aim_angle(work) - TABLE(task)->angle;
        if (step > 0) {
            if (step > MAX_TURN) {
                step = MAX_TURN;
            }
        } else if (step < -MAX_TURN) {
            step = -MAX_TURN;
        }
        turn = step;
        if (turn) {
            sound_play_at_for(TURN_SOUND, work, TURN_PRIORITY, TURN_VOLUME,
                              TURN_DURATION, &work->pos);
        }
        TABLE(task)->angle += turn;
    }
    if (TABLE(task)->object.model != NULL) {
        table = TABLE(task);
        object = &table->object;
        /* @hack spelling: the copy's angle read as table->object.ang[1], where
         * object->ang[1] would do */
        last = table->object.ang[1];
        turn = table->angle - last;
        njPushMatrixEx();
        njUnitMatrix(NULL);
        njTranslate(NULL, work->pos.x, work->pos.y, work->pos.z);
        njRotateY(NULL, turn);
        njTranslate(NULL, -work->pos.x, -work->pos.y, -work->pos.z);
        for (i = 0; i < TASK_FORCES; i++) {
            if (player_work[i] != NULL) {
                njCalcPoint(NULL, &player_work[i]->pos, &start);
                TASK_FORCES_OF(task)[i].turn.y = -turn;
                TASK_FORCES_OF(task)
                [i].speed.x = start.x - player_work[i]->pos.x;
                TASK_FORCES_OF(task)
                [i].speed.y = start.y - player_work[i]->pos.y;
                TASK_FORCES_OF(task)
                [i].speed.z = start.z - player_work[i]->pos.z;
            }
        }
        njPopMatrix(1);
        /* @hack spelling: the table read again through task, where
         * object->ang[1] = table->angle would do */
        TABLE(task)->object.ang[1] = TABLE(task)->angle;
        if (player_near((NJS_POINT3 *)object->pos,
                        object->model->r + REACH_MARGIN)) {
            work->flags |= WORLD_ADDED;
        } else {
            work->flags &= ~WORLD_ADDED;
        }
    }
}

static void turntable_draw(struct task *task)
{
    struct task_work *work = task->work;

    if (player_near(&work->pos, DRAW_RANGE)) {
        njSetTexture(&turntable_texlist);
        njPushMatrix(NULL);
        njTranslate(NULL, work->pos.x, work->pos.y + TABLE_HEIGHT, work->pos.z);
        njRotateY(NULL, TABLE(task)->angle);
        njCnkEasyDrawModel(&turntable_model);
        njPopMatrix(1);
    }
}
