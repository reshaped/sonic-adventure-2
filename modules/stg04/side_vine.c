/* White Jungle's side vine, YOKOTUTA in the stage's object list: a vine
 * reaching sideways from its work's pos. a player who catches its tip swings
 * round with it. the vine throws the player once the swing passes THROW_SWING.
 * the hanging vine of stg04/hanging_vine.c is its twin, which hangs down */
#include <mathf.h>
#include <ninja.h>
#include <sg_xpt.h>
#include <shinobi.h>

#include "collision/collision.h"
#include "draw/lines.h"
#include "object/set.h"
#include "player/player.h"
#include "shared/common_object.h"
#include "sound/sound.h"
#include "stg04/side_vine.h"
#include "stg04/side_vine/text.inc"
#include "system/angle.h"
#include "system/game_state.h"
#include "system/task.h"
#include "vmu/vibration.h"

/* The fixed anchor, stretching side vine and its hanging grip. */
static NJS_TEXNAME model_textures[4] = {
#include "stg04/side_vine/model_textures.inc"
};

NJS_TEXLIST side_vine_texlist = {
#include "stg04/side_vine/side_vine_texlist.inc"
};

static Sint16 grip_polygons[255] = {
#include "stg04/side_vine/grip_polygons.inc"
};

static Sint32 grip_vertices[195] = {
#include "stg04/side_vine/grip_vertices.inc"
};

static NJS_CNK_MODEL grip_mesh = {
#include "stg04/side_vine/grip_mesh.inc"
};

static NJS_CNK_OBJECT side_vine_tip[1] = {
#include "stg04/side_vine/side_vine_tip.inc"
};

static Sint16 stem_polygons[245] = {
#include "stg04/side_vine/stem_polygons.inc"
};

static Sint32 stem_vertices[171] = {
#include "stg04/side_vine/stem_vertices.inc"
};

static NJS_CNK_MODEL stem_mesh = {
#include "stg04/side_vine/stem_mesh.inc"
};

static NJS_CNK_OBJECT stem_object = {
#include "stg04/side_vine/stem_object.inc"
};

static Sint16 anchor_polygons[401] = {
#include "stg04/side_vine/anchor_polygons.inc"
};

static Sint32 anchor_vertices[183] = {
#include "stg04/side_vine/anchor_vertices.inc"
};

static NJS_CNK_MODEL anchor_mesh = {
#include "stg04/side_vine/anchor_mesh.inc"
};

static NJS_CNK_OBJECT side_vine_object = {
#include "stg04/side_vine/side_vine_object.inc"
};

/* the vine's modes, in its work's mode */
enum side_vine_mode { MODE_START, MODE_RUN };

/* the vine's record, in its task's extra */
struct side_vine {
    /* where the vine's tip lies, as side_vine_pose last set it */
    NJS_POINT3 tip;
    NJS_POINT3 rest; /* the tip as the vine started */
    Sint32 unk_18;
    Angle swing; /* the vine's swing round from its rest */
    /* side_vine_pose's ang_x and ang_z, the turns of the heading ang.y
     * gives. nothing reads them */
    Angle ahead_ang_x;
    Angle ahead_ang_z;
    /* -1 while a player hangs from the tip. otherwise it counts the steps
     * the vine has moved since it started or let a player go */
    Sint32 timer;
    Float speed; /* the swing's speed, in degrees a step */
    /* how far the vine stretches along its length, and its turn about y.
     * side_vine_pose sets both, and the wobble then sets bend */
    Float stretch;
    Angle bend;
    NJS_VECTOR ahead; /* side_vine_pose's dir, which nothing reads */
};

/* the placement's state the start sets. the vine ends once it is 0 */
#define STATE_LIVE 1
/* the slot of the player on the vine */
#define PLAYER(work) ((work)->submode)
/* TRUE from the vine's first let-go on */
#define RELEASED(work) ((work)->tick)
/* the vine moves and draws only while a player lies within NEAR_RANGE of its
 * work's pos */
#define NEAR_RANGE 3000.0F
/* a player catches the vine where its first shape's squared distance from the
 * tip is below CATCH_RANGE. the catch waits until the record's timer has
 * counted CATCH_WAIT steps */
#define CATCH_RANGE (10.0F * 10.0F)
#define CATCH_WAIT 30
/* a catch adds CATCH_PUSH times |scl.z| to the swing's speed. each step a
 * player hangs, the speed keeps SPEED_KEEP of itself and gains |scl.z|. up to
 * a swing of SLOW_SWING, it gains only SLOW_GAIN of that */
#define CATCH_PUSH 10.0F
#define SPEED_KEEP 0.99F
#define SLOW_GAIN 0.3F
#define SLOW_SWING NJM_DEG_ANG(15)
/* the vine throws its player once the swing passes THROW_SWING. the throw's
 * speed is |scl.z| times the length of side_vine_tip's offset, times
 * THROW_SCALE, plus scl.y. the player then takes no stick for
 * THROW_STICK_LOCK steps. the throw rests the swing at 0, and turns its speed
 * back by THROW_BACK. the next step's swing then falls below 0, so the vine
 * rests again, which zeroes the speed */
#define THROW_SWING NJM_DEG_ANG(100)
#define THROW_SCALE (1.0F / 60.0F)
#define THROW_STICK_LOCK 60
#define THROW_BACK (-0.95F)
/* from the vine's first let-go on, its bend wobbles while it rests, up to
 * WOBBLE_SIZE either way. the work's timer counts the wobble's steps. the
 * wobble's phase moves WOBBLE_RATE degrees a step */
#define WOBBLE_STEP(work) ((work)->timer)
#define WOBBLE_RATE 4
#define WOBBLE_SIZE (3.0F * ANGLE_PER_DEGREE)
/* the sound of a catch, of the stage's sound list */
#define CATCH_SOUND 0
#define SOUND_PRIORITY 1
#define SOUND_VOLUME 120
#define SOUND_DURATION 80
/* the pad shakes at the catch and at the throw */
#define CATCH_POWER 2
#define CATCH_FREQ 15
#define CATCH_TIME 1
#define THROW_POWER 4
#define THROW_FREQ 15
/* the vine's two children stand POST_OFFSET either side of the tip's rest,
 * along z as ang.y turns it. each takes the POST_SHAPES shapes of
 * side_vine_shape */
#define POST_OFFSET 13.0F
#define POST_SHAPES 1
/* the draw puts the tip's part TIP_DROP below the tip's rest, wherever the
 * vine swings. it turns the part about z by TIP_TURN, side_vine_tip's own
 * ang[2]. that is a whole turn, which leaves the part as it was */
#define TIP_DROP 7.25F
#define TIP_TURN NJM_DEG_ANG(-360)

/* the debug preview poses the vine PREVIEW_STEPS times, a step of its speed
 * apart. it draws one pose in PREVIEW_DRAW_EVERY. its speed then gains
 * PREVIEW_SLOW_GAIN of |scl.z| up to a swing of SLOW_SWING */
#define PREVIEW_STEPS 16
#define PREVIEW_DRAW_EVERY 2
#define PREVIEW_SLOW_GAIN 0.2F

/* a hanging player lies at hang_offset from the tip, upright */
static NJS_POINT3 hang_offset = {
#include "stg04/side_vine/hang_offset.inc"
};
/* three unread Angles of 0, where the hanging vine keeps player_turn */
static Angle dat_8c5a398c[3] = {0, 0, 0};
static struct collision_shape side_vine_shape[POST_SHAPES] = {
#include "stg04/side_vine/side_vine_shape.inc"
};

static void side_vine_start(struct task *task);
static void side_vine_run(struct task *task);
static void side_vine_draw(struct task *task);
static void side_vine_destroy(struct task *task);

static Float side_vine_distance_squared(NJS_POINT3 *from, NJS_POINT3 *to)
{
    Float dx = from->x - to->x;
    Float dy = from->y - to->y;
    Float dz = from->z - to->z;

    return (dx * dx) + (dy * dy) + (dz * dz);
}

/* the first of players 0 and 1 whose first shape's center lies within range
 * of pos. -1 is neither. range is the square of a distance */
static int side_vine_player_within(NJS_POINT3 *pos, Float range)
{
    if (player_work[0] != NULL &&
        side_vine_distance_squared(&player_work[0]->collision->shapes->center,
                                   pos) < range) {
        return 0;
    }
    if (player_work[1] != NULL &&
        side_vine_distance_squared(&player_work[1]->collision->shapes->center,
                                   pos) < range) {
        return 1;
    }
    return -1;
}

/* the vine's pose at swing. stretch gets how far it stretches along its
 * length, and bend its turn about y. scale stretches the swing along z, so
 * that the tip runs on an ellipse. the stretch is 1 at a swing of 0 */
static void side_vine_reach(Angle swing, Float scale, Float *stretch,
                            Angle *bend)
{
    NJS_VECTOR point;

    point.x = 1.0F;
    point.y = 0.0F;
    point.z = 0.0F;
    njPushMatrix(NULL);
    njUnitMatrix(NULL);
    njScale(NULL, 1.0F, 1.0F, scale);
    njRotateY(NULL, swing);
    njCalcVector(NULL, &point, &point);
    njPopMatrix(1);
    *stretch = njScalor(&point);
    *bend = (Angle)(atan2f(point.z, point.x) * ANGLE_PER_RADIAN);
}

/* pose the vine at swing, in its plane turned by turn about x. tip gets the
 * tip's place, level with the top in the vine's axes. stretch and bend get
 * side_vine_reach's for swing. while RELEASED, stretch gets the one at
 * THROW_SWING instead. dir gets ang.y's
 * turn of (0, 0, 1), and ang_x and ang_z get that direction's turns */
static void side_vine_pose(struct task *task, Float scale, Angle swing,
                           Angle turn, NJS_POINT3 *tip, Angle *ang_x,
                           Angle *ang_z, Float *stretch, Angle *bend,
                           NJS_VECTOR *dir)
{
    struct task_work *work = task->work;
    NJS_VECTOR ahead;
    NJS_POINT3 offset;
    Float throw_stretch;
    Angle throw_bend;

    if (RELEASED(work) == FALSE) {
        side_vine_reach(swing, scale, stretch, bend);
    } else {
        side_vine_reach(THROW_SWING, scale, &throw_stretch, &throw_bend);
        side_vine_reach(swing, scale, stretch, bend);
        *stretch = throw_stretch;
    }
    njPushMatrix(NULL);
    njUnitMatrix(NULL);
    njTranslateV(NULL, &work->pos);
    njRotateY(NULL, work->ang.y);
    njRotateZ(NULL, work->ang.z);
    njRotateX(NULL, turn);
    njRotateY(NULL, *bend);
    njScale(NULL, *stretch, 1.0F, 1.0F);
    offset = *(NJS_POINT3 *)side_vine_tip[0].pos;
    offset.y = 0.0F;
    njCalcPoint(NULL, &offset, tip);
    njUnitMatrix(NULL);
    njTranslateV(NULL, &work->pos);
    njRotateY(NULL, work->ang.y);
    ahead.x = 0.0F;
    ahead.y = 0.0F;
    ahead.z = 1.0F;
    njCalcVector(NULL, &ahead, &ahead);
    njPopMatrix(1);
    *ang_z = (Angle)(atan2f(ahead.x, ahead.y) * ANGLE_PER_RADIAN);
    *ang_x = (Angle)(atan2f(ahead.z, njSqrt(1.0F - (ahead.z * ahead.z))) *
                     ANGLE_PER_RADIAN);
    *dir = ahead;
}

void side_vine_init(struct task *task)
{
    struct task_work *work = task->work;

    switch (work->mode) {
    case MODE_START:
        task->destructor = side_vine_destroy;
        task->draw = side_vine_draw;
        side_vine_start(task);
        break;
    case MODE_RUN:
        side_vine_run(task);
        break;
    default:
        work->mode = MODE_START;
        break;
    }
}

static void side_vine_start(struct task *task)
{
    struct task_work *work = task->work;
    struct side_vine *vine;
    struct task *child;
    NJS_POINT3 offset;

    vine = task->extra.p = syCalloc(1, sizeof(struct side_vine));
    work->mode = MODE_RUN;
    vine->swing = 0;
    vine->timer = 0;
    side_vine_pose(task, work->scl.x, vine->swing, work->ang.x, &vine->tip,
                   &vine->ahead_ang_x, &vine->ahead_ang_z, &vine->stretch,
                   &vine->bend, &vine->ahead);
    vine->rest = vine->tip;
    vine->speed = 0.0F;
    RELEASED(work) = FALSE;
    if (task->placement) {
        placement_set_state(task, STATE_LIVE);
    }
    njPushMatrixEx();
    njUnitMatrix(NULL);
    njTranslateEx(&vine->rest);
    njRotateY(NULL, work->ang.y);
    child =
        model_object_create(task, NULL, NULL, side_vine_shape, POST_SHAPES, 0);
    if (child) {
        offset.z = POST_OFFSET;
        offset.y = 0.0F;
        offset.x = 0.0F;
        njCalcPoint(NULL, &offset, &child->work->pos);
        child->work->ang.x = 0;
        child->work->ang.y = 0;
        child->work->ang.z = 0;
    }
    child =
        model_object_create(task, NULL, NULL, side_vine_shape, POST_SHAPES, 0);
    if (child) {
        offset.z = -POST_OFFSET;
        offset.y = 0.0F;
        offset.x = 0.0F;
        njCalcPoint(NULL, &offset, &child->work->pos);
        child->work->ang.x = 0;
        child->work->ang.y = 0;
        child->work->ang.z = 0;
    }
    njPopMatrixEx();
}

static void side_vine_run(struct task *task)
{
    struct task_work *work = task->work;
    struct side_vine *vine = task->extra.p;
    Float throw_speed;
    int slot;
    /* @hack spelling: state, a local for &game_state, where game_state read
     * in place would do */
    struct game_state *state = &game_state;
    /* @hack spelling: one, a local for 1.0F, where 1.0F in place would do */
    Float one = 1.0F;

    /* @hack redundant: a debug preview no path reaches, which draws the vine
     * at poses along its swing and a line along the tip's path. SHC keeps its
     * loop, and places its strings in this source's constants, the textures'
     * names among them. its first three calls and 37 stores leave no code */
    if (0) {
        NJS_POINT3 prev;
        NJS_POINT3 cur;
        Angle saved;
        Angle step;
        int i;

        (void)SIDE_VINE_EDITOR_ANGLE_MOVE_X;
        (void)SIDE_VINE_EDITOR_ANGLE_ROT_POS_Y;
        (void)SIDE_VINE_EDITOR_ANGLE_HANDLE;
        (void)SIDE_VINE_EDITOR_SCL_MOVE_SCL;
        (void)SIDE_VINE_EDITOR_ADD_SPD;
        (void)SIDE_VINE_EDITOR_MOVE_SPD;
        njSin(0);
        njSin(0);
        njSin(0);
        state->cleared = 0;
        state->cleared = 0;
        vine->speed = one;
        vine->speed = one;
        vine->speed = one;
        vine->speed = one;
        vine->speed = one;
        vine->speed = one;
        vine->speed = one;
        vine->speed = one;
        vine->speed = one;
        vine->speed = one;
        vine->speed = one;
        vine->speed = one;
        vine->speed = one;
        vine->speed = one;
        vine->speed = one;
        vine->speed = one;
        vine->speed = one;
        vine->speed = one;
        vine->speed = one;
        vine->speed = one;
        vine->speed = one;
        vine->speed = one;
        vine->speed = 182.044449F;
        vine->speed = 182.044449F;
        vine->speed = 182.044449F;
        vine->speed = 182.044449F;
        vine->speed = 182.044449F;
        vine->speed = 182.044449F;
        vine->speed = 182.044449F;
        vine->speed = 182.044449F;
        vine->speed = 182.044449F;
        vine->speed = 182.044449F;
        vine->speed = 0.99F;
        vine->speed = 0.99F;
        vine->speed = 0.99F;
        saved = vine->swing;
        step = (Angle)(vine->speed * ANGLE_PER_DEGREE);
        for (i = 0; i < PREVIEW_STEPS; i++) {
            prev = vine->tip;
            side_vine_pose(task, work->scl.x, vine->swing, work->ang.x,
                           &vine->tip, &vine->ahead_ang_x, &vine->ahead_ang_z,
                           &vine->stretch, &vine->bend, &vine->ahead);
            cur = vine->tip;
            if (i == 0) {
                vine->rest = vine->tip;
            }
            if (i % PREVIEW_DRAW_EVERY == 0) {
                side_vine_draw(task);
            }
            if (i > 0) {
                line_draw(prev.x, prev.y, prev.z, cur.x, cur.y, cur.z);
            }
            vine->swing += step;
        }
        vine->swing = (Angle)(vine->speed * ANGLE_PER_DEGREE) + saved;
        side_vine_pose(task, work->scl.x, vine->swing, work->ang.x, &vine->tip,
                       &vine->ahead_ang_x, &vine->ahead_ang_z, &vine->stretch,
                       &vine->bend, &vine->ahead);
        if (vine->swing > SLOW_SWING) {
            vine->speed =
                (SPEED_KEEP * vine->speed) + _builtin_fabsf(work->scl.z);
        } else {
            vine->speed = (SPEED_KEEP * vine->speed) +
                          _builtin_fabsf(work->scl.z * PREVIEW_SLOW_GAIN);
        }
        if (vine->swing > THROW_SWING) {
            vine->swing = 0;
            vine->speed = _builtin_fabsf(work->scl.z) * CATCH_PUSH;
        }
    } else {
        if (task->placement && !placement_get_state(task)) {
            task_kill(task);
            return;
        }
        if (!player_near(&work->pos, NEAR_RANGE)) {
            return;
        }
        if (state->cleared == 1 ||
            (player_records[PLAYER(work)]->flags & PLAYER_MISSED)) {
            if (vine->timer < 0) {
                player_request(PLAYER(work), PLAYER_REQUEST_RELEASE, 0);
                RELEASED(work) = TRUE;
                WOBBLE_STEP(work) = 0;
                vine->timer = 0;
            }
        }
        vine->swing += (Angle)(vine->speed * ANGLE_PER_DEGREE);
        /* @hack redundant: a block no path reaches, of twelve stores to the
         * speed */
        if (0) {
            vine->speed = 0.0F;
            vine->speed = 0.0F;
            vine->speed = 0.0F;
            vine->speed = 0.0F;
            vine->speed = 0.0F;
            vine->speed = 0.0F;
            vine->speed = 0.0F;
            vine->speed = 0.0F;
            vine->speed = 0.0F;
            vine->speed = 0.0F;
            vine->speed = 0.0F;
            vine->speed = 0.0F;
        }
        if (vine->timer < 0) {
            if (vine->swing > SLOW_SWING) {
                vine->speed =
                    (SPEED_KEEP * vine->speed) + _builtin_fabsf(work->scl.z);
            } else {
                vine->speed = (SPEED_KEEP * vine->speed) +
                              _builtin_fabsf(work->scl.z * SLOW_GAIN);
            }
            if (vine->swing > THROW_SWING) {
                NJS_VECTOR dir;

                vine->swing = 0;
                vine->timer = 0;
                player_request(PLAYER(work), PLAYER_REQUEST_RELEASE, 0);
                vibration_start(PLAYER(work), THROW_POWER, THROW_FREQ,
                                VIBRATION_UNTIMED);
                njPushMatrix(NULL);
                njUnitMatrix(NULL);
                njRotateY(NULL, work->ang.y);
                njRotateZ(NULL, work->ang.z);
                njRotateX(NULL, work->ang.x);
                njRotateY(NULL, QUARTER_TURN);
                dir.x = 0.0F;
                dir.y = 0.0F;
                dir.z = one;
                njCalcVector(NULL, &dir, &dir);
                njPopMatrix(1);
                throw_speed = (_builtin_fabsf(work->scl.z) *
                               njScalor((NJS_VECTOR *)side_vine_tip[0].pos) *
                               THROW_SCALE) +
                              work->scl.y;
                player_push(PLAYER(work), dir.x * throw_speed,
                            dir.y * throw_speed, dir.z * throw_speed);
                player_records[PLAYER(work)]->stick_lock = THROW_STICK_LOCK;
                vine->speed *= THROW_BACK;
                RELEASED(work) = TRUE;
                WOBBLE_STEP(work) = 0;
            } else if (vine->swing < 0) {
                vine->swing = 0;
                vine->speed = 0.0F;
            }
        } else if (vine->swing < 0) {
            vine->swing = 0;
            vine->speed = 0.0F;
        }
        side_vine_pose(task, work->scl.x, vine->swing, work->ang.x, &vine->tip,
                       &vine->ahead_ang_x, &vine->ahead_ang_z, &vine->stretch,
                       &vine->bend, &vine->ahead);
        if (RELEASED(work) && vine->swing == 0) {
            WOBBLE_STEP(work)++;
            vine->bend = njSin((Angle)(WOBBLE_STEP(work) * WOBBLE_RATE *
                                       ANGLE_PER_DEGREE)) *
                         WOBBLE_SIZE;
        }
        if (vine->timer >= 0) {
            vine->timer++;
            if (vine->timer >= CATCH_WAIT) {
                slot = side_vine_player_within(&vine->tip, CATCH_RANGE);
                if (slot >= 0) {
                    PLAYER(work) = slot;
                    vine->timer = -1;
                    vine->speed += _builtin_fabsf(work->scl.z) * CATCH_PUSH;
                    player_request(slot, PLAYER_REQUEST_ANIM, ANIM_VINE_HANG);
                    sound_play_for(CATCH_SOUND, work, SOUND_PRIORITY,
                                   SOUND_VOLUME, SOUND_DURATION);
                    vibration_start(slot, CATCH_POWER, CATCH_FREQ, CATCH_TIME);
                }
            }
        } else {
            NJS_POINT3 pos;

            njPushMatrix(NULL);
            njUnitMatrix(NULL);
            njTranslateV(NULL, &vine->tip);
            njRotateY(NULL, -work->ang.y);
            njCalcPoint(NULL, &hang_offset, &pos);
            njPopMatrixEx();
            player_set_pos(PLAYER(work), pos.x, pos.y, pos.z);
            player_work[PLAYER(work)]->ang.x = 0;
            player_work[PLAYER(work)]->ang.y = -work->ang.y;
            player_work[PLAYER(work)]->ang.z = 0;
        }
    }
}

static void side_vine_draw(struct task *task)
{
    struct task_work *work = task->work;
    struct side_vine *vine = task->extra.p;
    NJS_CNK_OBJECT *object;
    NJS_CNK_OBJECT *tip;

    if (player_near(&work->pos, NEAR_RANGE)) {
        object = &side_vine_object;
        njSetTexture(&side_vine_texlist);
        njPushMatrix(NULL);
        njTranslateV(NULL, &work->pos);
        njRotateY(NULL, work->ang.y);
        njCnkEasyDrawModel(object->model);
        object = object->child;
        njRotateZ(NULL, work->ang.z);
        njRotateX(NULL, work->ang.x);
        njRotateY(NULL, vine->bend);
        njRotateX(NULL, -work->ang.x);
        njScale(NULL, vine->stretch, 1.0F, 1.0F);
        njCnkEasyDrawModel(object->model);
        njPopMatrix(1);
        tip = object->child;
        njPushMatrix(NULL);
        njTranslate(NULL, vine->rest.x, vine->rest.y - TIP_DROP, vine->rest.z);
        njRotateY(NULL, work->ang.y);
        njRotateZ(NULL, TIP_TURN);
        njCnkEasyDrawModel(tip->model);
        njPopMatrix(1);
    }
}

static void side_vine_destroy(struct task *task)
{
    /* @hack redundant: work, a local nothing reads */
    struct task_work *work = task->work;

    syFree(task->extra.p);
    task->extra.p = NULL;
}
