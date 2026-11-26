/* a garden Chao's icon. the ball's color mixes its form's colors by the
 * Chao's run_power, fly_swim and evolution. a child's color also leans toward
 * the hero or dark colors by its alignment */
#include <mathf.h>
#include <ninja.h>
#include <sg_xpt.h>
#include <stdlib.h>

#include "camera/camera.h"
#include "player/player.h"
#include "sound/sound.h"

#include "chao/icon.h"
#include "chao/life.h"
#include "chao/work.h"
#include "system/angle.h"
#include "system/task.h"
#include "system/util.h"

#define CHANNELS 3
struct icon_rgb {
    Uint8 channel[CHANNELS];
};

/* the colors of each form's entry in a table. BASE_COLOR is the color at an
 * evolution of 0, and GROWN_COLOR the one at 1. a positive run_power leans
 * GROWN_COLOR toward POWER_COLOR, and a negative one toward RUN_COLOR. a
 * positive fly_swim leans it toward FLY_COLOR, and a negative one toward
 * SWIM_COLOR */
enum {
    BASE_COLOR,
    GROWN_COLOR,
    SWIM_COLOR,
    FLY_COLOR,
    RUN_COLOR,
    POWER_COLOR,
    COLORS_PER_FORM
};
/* each table holds the colors of FORMS forms, a child's and then each adult
 * form's */
#define FORMS 6
#define FORM_BANK(type, first)                                                 \
    (((((type) - (first)) / CHAO_FORM_ALIGNMENTS) + 1) * COLORS_PER_FORM)
/* the ball of a Chaos Chao and a Dark Chaos, and of a Hero Chaos */
#define CHAOS_COLOR 0xffffffff
#define HERO_CHAOS_COLOR 0xffffff00
#define ARGB(color)                                                            \
    (0xff000000 | ((color).channel[0] << 16) | ((color).channel[1] << 8) |     \
     (color).channel[2])
/* an axis of 0 weighs 1e-6, so that run_power + fly_swim is never 0 */
#define NONZERO_AXIS(value)                                                    \
    if ((value) == 0.0F) {                                                     \
        (value) = 1.0e-6F;                                                     \
    }
#define CLAMP_UNIT(value)                                                      \
    if ((value) > 1.0F) {                                                      \
        (value) = 1.0F;                                                        \
    } else if ((value) < -1.0F) {                                              \
        (value) = -1.0F;                                                       \
    }
#define COLOR_WEIGHTS()                                                        \
    {                                                                          \
        run_power_rest = 1.0F - run_power;                                     \
        fly_swim_rest = 1.0F - fly_swim;                                       \
        base_share = 1.0F - growth;                                            \
        run_power_share = fly_swim_share = 1.0F / (run_power + fly_swim);      \
        run_power_share *= run_power;                                          \
        fly_swim_share *= fly_swim;                                            \
    }
/* channel i of grown, mixed toward each axis's color as far as the Chao leans
 * along the axis. the two mixes are weighed by the share of each lean. it
 * reads the caller's i, and the weights COLOR_WEIGHTS sets in the caller's
 * locals */
#define MIX_COLOR(grown, run_power_end, fly_swim_end)                          \
    ((run_power_share * (((Float)(grown)->channel[i] * run_power_rest) +       \
                         ((Float)(run_power_end)->channel[i] * run_power))) +  \
     (fly_swim_share * (((Float)(grown)->channel[i] * fly_swim_rest) +         \
                        ((Float)(fly_swim_end)->channel[i] * fly_swim))))

/* the halo's cell less CHAO_ICON_HALO, by angle. the angle lies between the
 * head's up direction and the way to the camera. it runs in whole degrees from
 * 0 to 180 */
#define HALO_VIEWS 181
static Sint32 halo_view[HALO_VIEWS] = {
#include "chao/icon/halo_view.inc"
};

static struct icon_rgb neutral_colors[FORMS * COLORS_PER_FORM] = {
#include "chao/icon/neutral_colors.inc"
};
static struct icon_rgb hero_colors[FORMS * COLORS_PER_FORM] = {
#include "chao/icon/hero_colors.inc"
};
static struct icon_rgb dark_colors[FORMS * COLORS_PER_FORM] = {
#include "chao/icon/dark_colors.inc"
};

void chao_update_icon_color(struct task *chao)
{
    struct chao_work *work;
    struct chao_parameters *parameters;
    int i;
    /* @bug uninitialized: the default case reads bank unset where the type is
     * no adult form, as CHAO_EGG is (0x0c50d392) */
#ifdef NONMATCHING
    /* this side repairs the bug tagged above: bank starts at neutral_colors */
    struct icon_rgb *bank = neutral_colors;
#else
    struct icon_rgb *bank;
#endif
    work = (struct chao_work *)chao->work;
    parameters = &work->profile.parameters;
    switch (parameters->type) {
    case CHAO_CHILD:
        break;
    case CHAO_NORMAL_ADULT:
    case CHAO_SWIM_ADULT:
    case CHAO_FLY_ADULT:
    case CHAO_RUN_ADULT:
    case CHAO_POWER_ADULT:
        bank = &neutral_colors[FORM_BANK(parameters->type, CHAO_NORMAL_ADULT)];
        break;
    case CHAO_HERO_TYPE(CHAO_NORMAL_ADULT):
    case CHAO_HERO_TYPE(CHAO_SWIM_ADULT):
    case CHAO_HERO_TYPE(CHAO_FLY_ADULT):
    case CHAO_HERO_TYPE(CHAO_RUN_ADULT):
    case CHAO_HERO_TYPE(CHAO_POWER_ADULT):
        bank = &hero_colors[FORM_BANK(parameters->type, CHAO_FIRST_HERO_TYPE)];
        break;
    case CHAO_DARK_TYPE(CHAO_NORMAL_ADULT):
    case CHAO_DARK_TYPE(CHAO_SWIM_ADULT):
    case CHAO_DARK_TYPE(CHAO_FLY_ADULT):
    case CHAO_DARK_TYPE(CHAO_RUN_ADULT):
    case CHAO_DARK_TYPE(CHAO_POWER_ADULT):
        bank = &dark_colors[FORM_BANK(parameters->type, CHAO_FIRST_DARK_TYPE)];
        break;
    }
    switch (parameters->type) {
    case CHAO_CHILD: {
        Float run_power;
        Float fly_swim;
        Float growth;
        Float alignment;
        Float neutral_share;
        Float run_power_rest;
        Float fly_swim_rest;
        Float base_share;
        Float run_power_share;
        Float fly_swim_share;
        Float neutral;
        Float other;
        struct icon_rgb *base;
        struct icon_rgb *grown;
        struct icon_rgb *run_power_end;
        struct icon_rgb *fly_swim_end;
        struct icon_rgb *other_base;
        struct icon_rgb *other_grown;
        struct icon_rgb *other_run_power;
        struct icon_rgb *other_fly_swim;
        struct icon_rgb result;
        base = &neutral_colors[BASE_COLOR];
        grown = &neutral_colors[GROWN_COLOR];
        run_power_end = &neutral_colors[POWER_COLOR];
        fly_swim_end = &neutral_colors[FLY_COLOR];
        run_power = parameters->run_power;
        fly_swim = parameters->fly_swim;
        growth = parameters->evolution;
        alignment = parameters->alignment;
        if (alignment >= 0.0F) {
            if (alignment > 1.0F) {
                alignment = 1.0F;
            }
            neutral_share = 1.0F - alignment;
            other_base = &hero_colors[BASE_COLOR];
            other_grown = &hero_colors[GROWN_COLOR];
            other_run_power = &hero_colors[POWER_COLOR];
            other_fly_swim = &hero_colors[FLY_COLOR];
        } else {
            if (alignment < -1.0F) {
                alignment = -1.0F;
            }
            alignment *= -1.0F;
            neutral_share = 1.0F - alignment;
            other_base = &dark_colors[BASE_COLOR];
            other_grown = &dark_colors[GROWN_COLOR];
            other_run_power = &dark_colors[POWER_COLOR];
            other_fly_swim = &dark_colors[FLY_COLOR];
        }
        if (growth > 1.0F) {
            growth = 1.0F;
        }
        NONZERO_AXIS(run_power);
        NONZERO_AXIS(fly_swim);
        CLAMP_UNIT(run_power);
        if (run_power < 0.0F) {
            /* @hack spelling: the two pointers stepped back from POWER_COLOR
             * to RUN_COLOR by assignments, where -- would do */
            run_power_end = run_power_end - 1;
            other_run_power = other_run_power - 1;
            run_power = -run_power;
        }
        CLAMP_UNIT(fly_swim);
        if (fly_swim < 0.0F) {
            /* @hack spelling: the two pointers stepped back from FLY_COLOR to
             * SWIM_COLOR by assignments, where -- would do */
            fly_swim_end = fly_swim_end - 1;
            other_fly_swim = other_fly_swim - 1;
            fly_swim = -fly_swim;
        }
        COLOR_WEIGHTS();
        for (i = 0; i < CHANNELS; i++) {
            neutral = MIX_COLOR(grown, run_power_end, fly_swim_end);
            neutral =
                ((Float)base->channel[i] * base_share) + (growth * neutral);
            other = MIX_COLOR(other_grown, other_run_power, other_fly_swim);
            other =
                ((Float)other_base->channel[i] * base_share) + (growth * other);
            result.channel[i] = (neutral_share * neutral) + (alignment * other);
        }
        work->icon.color = ARGB(result);
        break;
    }
    case CHAO_CHAOS:
        work->icon.color = CHAOS_COLOR;
        break;
    case CHAO_HERO_CHAOS:
        work->icon.color = HERO_CHAOS_COLOR;
        break;
    case CHAO_DARK_CHAOS:
        work->icon.color = CHAOS_COLOR;
        break;
    default: {
        Float run_power;
        Float fly_swim;
        Float growth;
        Float run_power_rest;
        Float fly_swim_rest;
        Float base_share;
        Float run_power_share;
        Float fly_swim_share;
        Float mixed;
        struct icon_rgb *base;
        struct icon_rgb *grown;
        struct icon_rgb *run_power_end;
        struct icon_rgb *fly_swim_end;
        struct icon_rgb result;
        base = &bank[BASE_COLOR];
        grown = &bank[GROWN_COLOR];
        run_power_end = &bank[POWER_COLOR];
        fly_swim_end = &bank[FLY_COLOR];
        run_power = parameters->run_power;
        fly_swim = parameters->fly_swim;
        growth = parameters->evolution;
        if (growth > 1.0F) {
            growth = 1.0F;
        }
        NONZERO_AXIS(run_power);
        NONZERO_AXIS(fly_swim);
        CLAMP_UNIT(run_power);
        if (run_power < 0.0F) {
            run_power_end = &bank[RUN_COLOR];
            run_power = -run_power;
        }
        CLAMP_UNIT(fly_swim);
        if (fly_swim < 0.0F) {
            fly_swim_end = &bank[SWIM_COLOR];
            fly_swim = -fly_swim;
        }
        COLOR_WEIGHTS();
        for (i = 0; i < CHANNELS; i++) {
            mixed = MIX_COLOR(grown, run_power_end, fly_swim_end);
            result.channel[i] =
                ((Float)base->channel[i] * base_share) + (growth * mixed);
        }
        work->icon.color = ARGB(result);
        break;
    }
    }
}
#define ICON(chao) (&((struct chao_work *)(chao)->work)->icon)
#define NO_NEXT_MODE (-1)
#define CLEAR_VECTOR(point)                                                    \
    {                                                                          \
        (point).x = 0.0F;                                                      \
        (point).y = 0.0F;                                                      \
        (point).z = 0.0F;                                                      \
    }
#define RISE_TO(value, step, limit)                                            \
    {                                                                          \
        (value) += (step);                                                     \
        if ((value) > (limit)) {                                               \
            (value) = (limit);                                                 \
        }                                                                      \
    }
#define FALL_TO(value, step, limit)                                            \
    {                                                                          \
        (value) -= (step);                                                     \
        if ((value) < (limit)) {                                               \
            (value) = (limit);                                                 \
        }                                                                      \
    }

static int idle_icon(struct task *chao);
static int exclamation_icon(struct task *chao);
static int held_icon(struct task *chao);
static int question_icon(struct task *chao);
static int heart_icon(struct task *chao);
static int anger_icon(struct task *chao);

/* each mode's step, in enum chao_mark's order. a step returns TRUE once its
 * mode is done */
static int (*icon_modes[])(struct task *) = {idle_icon,  exclamation_icon,
                                             held_icon,  question_icon,
                                             heart_icon, anger_icon};
#define DARK_ICON_FRAMES 16
static Uint16 dark_icon_frames[DARK_ICON_FRAMES] = {
#include "chao/icon/dark_icon_frames.inc"
};
static Uint16 dark_icon_frames_count = DARK_ICON_FRAMES;

/* the height above pos the icon starts at, and the mark's scale until its
 * first mode sets one */
#define START_HEIGHT 5.0F
#define FIRST_MARK_SCALE 3.5F
void chao_icon_init(struct task *chao)
{
    struct task_work *work = chao->work;
    struct chao_icon *icon = &((struct chao_work *)work)->icon;
    struct chao_icon_part *mark = &icon->upper;
    struct chao_icon_part *ball = &icon->lower;
    icon->squash = RANDOM_TURN();
    icon->bob = RANDOM_TURN();
    CLEAR_VECTOR(mark->velocity);
    CLEAR_VECTOR(mark->offset);
    mark->scale.x = FIRST_MARK_SCALE;
    mark->scale.y = FIRST_MARK_SCALE;
    mark->scale.z = 1.0F;
    icon->anchor.x = ball->position.x = mark->position.x = work->pos.x;
    icon->anchor.y = ball->position.y = mark->position.y =
        work->pos.y + START_HEIGHT;
    icon->anchor.z = ball->position.z = mark->position.z = work->pos.z;
    CLEAR_VECTOR(ball->velocity);
    CLEAR_VECTOR(ball->offset);
    ball->scale.x = 1.0F;
    ball->scale.y = 1.0F;
    ball->scale.z = 1.0F;
    mark->cell = CHAO_ICON_NO_MARK;
    ball->cell = CHAO_ICON_BALL;
    chao_clear_emotion_mark(chao);
    chao_icon_place(chao);
}
#define START_MARK(which)                                                      \
    {                                                                          \
        mark->cell = (which);                                                  \
        CLEAR_VECTOR(mark->offset);                                            \
        mark->scale.x = 0.0F;                                                  \
        mark->scale.y = 0.0F;                                                  \
        mark->phase++;                                                         \
    }
#define START_BALL()                                                           \
    {                                                                          \
        ball->cell = CHAO_ICON_BALL;                                           \
        CLEAR_VECTOR(ball->offset);                                            \
        ball->phase++;                                                         \
    }
#define STEADY_BALL()                                                          \
    switch (ball->phase) {                                                     \
    case 0:                                                                    \
        START_BALL();                                                          \
        break;                                                                 \
    }
#define GROW_MARK(step, size)                                                  \
    {                                                                          \
        RISE_TO(mark->scale.x, step, size);                                    \
        RISE_TO(mark->scale.y, step, size);                                    \
    }
#define SHRINK_MARK(step)                                                      \
    {                                                                          \
        FALL_TO(mark->scale.x, step, 0.0F);                                    \
        FALL_TO(mark->scale.y, step, 0.0F);                                    \
    }
#define WAIT_FOR_MARK()                                                        \
    if (icon->timer <= 0) {                                                    \
        mark->phase++;                                                         \
    }

/* a heart's or anger's ball: it shrinks away while the mark holds, and grows
 * back after. the mode is done once the ball is whole again */
#define REPLACE_BALL()                                                         \
    switch (ball->phase) {                                                     \
    case 0:                                                                    \
        START_BALL();                                                          \
        break;                                                                 \
    case 1:                                                                    \
        FALL_TO(ball->scale.x, 0.1F, 0.0F);                                    \
        FALL_TO(ball->scale.y, 0.1F, 0.0F);                                    \
        if (icon->timer <= 0) {                                                \
            ball->phase++;                                                     \
        }                                                                      \
        break;                                                                 \
    case 2:                                                                    \
        ball->scale.x += 0.05F;                                                \
        if (ball->scale.x > 1.0F) {                                            \
            ball->scale.x = 1.0F;                                              \
            return TRUE;                                                       \
        }                                                                      \
        RISE_TO(ball->scale.y, 0.05F, 1.0F);                                   \
        break;                                                                 \
    }
static int idle_icon(struct task *chao)
{
    struct chao_icon *icon = ICON(chao);
    struct chao_icon_part *mark = &icon->upper;
    struct chao_icon_part *ball = &icon->lower;
    switch (mark->phase) {
    case 0:
        START_MARK(CHAO_ICON_NO_MARK);
        break;
    }
    STEADY_BALL();
    if (icon->next_mode != NO_NEXT_MODE) {
        return TRUE;
    }
    return FALSE;
}
static int exclamation_icon(struct task *chao)
{
    struct chao_icon *icon = ICON(chao);
    struct chao_icon_part *mark = &icon->upper;
    struct chao_icon_part *ball = &icon->lower;
    switch (mark->phase) {
    case 0:
        START_MARK(CHAO_ICON_EXCLAMATION);
        break;
    case 1:
        RISE_TO(mark->offset.y, 0.48F, 2.0F);
        GROW_MARK(0.23F, 3.2F);
        WAIT_FOR_MARK();
        break;
    case 2:
        mark->offset.y -= 0.22F;
        if (mark->offset.y < 0.0F) {
            mark->offset.y = 0.0F;
            return TRUE;
        }
        SHRINK_MARK(0.28F);
        break;
    }
    STEADY_BALL();
    return FALSE;
}
static int held_icon(struct task *chao)
{
    struct chao_icon *icon = ICON(chao);
    struct chao_icon_part *mark = &icon->upper;
    struct chao_icon_part *ball = &icon->lower;
    switch (mark->phase) {
    case 0:
        mark->phase++;
        break;
    }
    switch (ball->phase) {
    case 0:
        ball->phase++;
        break;
    }
    return FALSE;
}
static int question_icon(struct task *chao)
{
    struct chao_icon *icon = ICON(chao);
    struct chao_icon_part *mark = &icon->upper;
    struct chao_icon_part *ball = &icon->lower;
    switch (mark->phase) {
    case 0:
        START_MARK(CHAO_ICON_QUESTION);
        break;
    case 1:
        RISE_TO(mark->offset.y, 0.48F, 2.5F);
        GROW_MARK(0.21F, 4.0F);
        WAIT_FOR_MARK();
        break;
    case 2:
        mark->offset.y -= 0.2F;
        if (mark->offset.y < 0.0F) {
            mark->offset.y = 0.0F;
            return TRUE;
        }
        SHRINK_MARK(0.3F);
        break;
    }
    STEADY_BALL();
    return FALSE;
}
static int heart_icon(struct task *chao)
{
    struct chao_icon *icon = ICON(chao);
    struct chao_icon_part *mark = &icon->upper;
    struct chao_icon_part *ball = &icon->lower;
    switch (mark->phase) {
    case 0:
        START_MARK(CHAO_ICON_HEART);
        break;
    case 1:
        RISE_TO(mark->offset.y, 0.1F, 0.7F);
        GROW_MARK(0.2F, 2.8F);
        WAIT_FOR_MARK();
        break;
    case 2:
        FALL_TO(mark->offset.y, 0.08F, 0.0F);
        SHRINK_MARK(0.25F);
        break;
    }
    REPLACE_BALL();
    return FALSE;
}
static int anger_icon(struct task *chao)
{
    struct chao_icon *icon = ICON(chao);
    struct chao_icon_part *mark = &icon->upper;
    struct chao_icon_part *ball = &icon->lower;
    switch (mark->phase) {
    case 0:
        START_MARK(CHAO_ICON_ANGER);
        break;
    case 1:
        RISE_TO(mark->offset.y, 0.1F, 1.3F);
        FALL_TO(mark->offset.x, 0.1F, -0.9F);
        GROW_MARK(0.23F, 3.0F);
        WAIT_FOR_MARK();
        break;
    case 2:
        FALL_TO(mark->offset.y, 0.09F, 0.0F);
        RISE_TO(mark->offset.x, 0.08F, 0.0F);
        SHRINK_MARK(0.25F);
        break;
    }
    REPLACE_BALL();
    return FALSE;
}
static void step_icon_mode(struct task *chao)
{
    struct chao_work *work = (struct chao_work *)chao->work;
    struct chao_icon *icon = &work->icon;
    struct chao_icon_part *mark = &icon->upper;
    struct chao_icon_part *ball = &icon->lower;
    int finished;
    finished = icon_modes[icon->mode](chao);
    if (finished == TRUE) {
        if (icon->next_mode == NO_NEXT_MODE) {
            icon->mode = CHAO_MARK_NONE;
            icon->timer = 0;
        } else {
            icon->mode = icon->next_mode;
            icon->next_mode = NO_NEXT_MODE;
            icon->timer = icon->next_timer;
        }
        mark->phase = 0;
        mark->frame = 0;
        ball->phase = 0;
        ball->frame = 0;
    }
    if (icon->timer > 0) {
        icon->timer--;
    }
    ball->frame++;
    mark->frame++;
}
void chao_icon_place(struct task *chao)
{
    struct chao_icon *icon = ICON(chao);
    struct chao_icon_part *mark = &icon->upper;
    struct chao_icon_part *ball = &icon->lower;
    NJS_POINT3 *ball_pos = &ball->position;
    NJS_VECTOR *ball_speed = &ball->velocity;
    NJS_POINT3 *mark_pos = &mark->position;
    NJS_VECTOR *mark_speed = &mark->velocity;
    NJS_POINT3 target;
    step_icon_mode(chao);
    target.x = icon->anchor.x;
    target.y = icon->anchor.y + ball->offset.y;
    target.z = icon->anchor.z;
    *ball_pos = target;
    CLEAR_VECTOR(*ball_speed);
    target.x = ball_pos->x;
    target.y = ball_pos->y + mark->offset.y;
    target.z = ball_pos->z;
    *mark_pos = target;
    CLEAR_VECTOR(*mark_speed);
}

/* the squash's turn in a step, and the steps each entry of dark_icon_frames
 * shows */
#define SQUASH_STEP 0x780
#define TEXTURE_STEPS 3
/* clamp the part's coordinate to no more than under below target's, and no
 * more than over above it. delta, target's coordinate less the part's, is
 * clamped with it */
#define LIMIT_STRETCH(delta, position, coordinate, under, over)                \
    if ((delta) > (under)) {                                                   \
        (position)->coordinate = target.coordinate - (under);                  \
        (delta) = (under);                                                     \
    } else if ((delta) < -(over)) {                                            \
        (position)->coordinate = target.coordinate + (over);                   \
        (delta) = -(over);                                                     \
    }
/* each step the ball's speed gains BALL_PULL times its distance to its
 * target. the speed then keeps BALL_DAMPING of itself. the ball's height stays
 * within BALL_SLACK_BELOW below the target's and BALL_SLACK_ABOVE above it.
 * the mark springs toward the ball alike, within MARK_SLACK on each axis */
#define BALL_PULL 0.11F
#define BALL_PULL_Y 0.1F
#define BALL_DAMPING 0.9F
#define BALL_DAMPING_Y 0.95F
#define BALL_SLACK_BELOW 0.8F
#define BALL_SLACK_ABOVE 1.0F
#define MARK_PULL 0.5F
#define MARK_PULL_Y 0.1F
#define MARK_DAMPING 0.45F
#define MARK_DAMPING_Y 0.8F
#define MARK_SLACK 0.7F
void chao_icon_step(struct task *chao)
{
    struct chao_icon *icon = ICON(chao);
    struct chao_icon_part *mark = &icon->upper;
    struct chao_icon_part *ball = &icon->lower;
    NJS_POINT3 *ball_pos = &ball->position;
    NJS_VECTOR *ball_speed = &ball->velocity;
    NJS_POINT3 *mark_pos = &mark->position;
    NJS_VECTOR *mark_speed = &mark->velocity;
    NJS_POINT3 target;
    Float dx;
    Float dy;
    Float dz;
    step_icon_mode(chao);
    target.x = icon->anchor.x;
    target.y = icon->anchor.y + ball->offset.y;
    target.z = icon->anchor.z;
    dx = target.x - ball_pos->x;
    dy = target.y - ball_pos->y;
    dz = target.z - ball_pos->z;
    LIMIT_STRETCH(dy, ball_pos, y, BALL_SLACK_BELOW, BALL_SLACK_ABOVE);
    ball_speed->x += dx * BALL_PULL;
    ball_speed->y += dy * BALL_PULL_Y;
    ball_speed->z += dz * BALL_PULL;
    ball_speed->x *= BALL_DAMPING;
    ball_speed->y *= BALL_DAMPING_Y;
    ball_speed->z *= BALL_DAMPING;
    ball_pos->x += ball_speed->x;
    ball_pos->y += ball_speed->y;
    ball_pos->z += ball_speed->z;
    target.x = ball_pos->x;
    target.y = ball_pos->y + mark->offset.y;
    target.z = ball_pos->z;
    dx = target.x - mark_pos->x;
    dy = target.y - mark_pos->y;
    dz = target.z - mark_pos->z;
    LIMIT_STRETCH(dx, mark_pos, x, MARK_SLACK, MARK_SLACK);
    LIMIT_STRETCH(dy, mark_pos, y, MARK_SLACK, MARK_SLACK);
    LIMIT_STRETCH(dz, mark_pos, z, MARK_SLACK, MARK_SLACK);
    mark_speed->x += dx * MARK_PULL;
    mark_speed->y += dy * MARK_PULL_Y;
    mark_speed->z += dz * MARK_PULL;
    mark_speed->x *= MARK_DAMPING;
    mark_speed->y *= MARK_DAMPING_Y;
    mark_speed->z *= MARK_DAMPING;
    mark_pos->x += mark_speed->x;
    mark_pos->y += mark_speed->y;
    mark_pos->z += mark_speed->z;
    icon->squash += SQUASH_STEP;
    if (++icon->texture_frame > TEXTURE_STEPS - 1) {
        icon->texture_frame = 0;
        icon->texture++;
        if (icon->texture > dark_icon_frames_count - 1) {
            icon->texture = 0;
        }
    }
    icon->random_angle = RANDOM_TURN();
}

/* a quad's half width and height, and its depth in view space */
#define QUAD_HALF 0.55F
#define QUAD_DEPTH (-0.3F)
#define ICON_VERTEX(corner, horizontal, vertical, u_coord, v_coord)            \
    {                                                                          \
        (corner).x = (horizontal);                                             \
        (corner).y = (vertical);                                               \
        (corner).z = QUAD_DEPTH;                                               \
        (corner).u = (u_coord);                                                \
        (corner).v = (v_coord);                                                \
        (corner).col = color.color;                                            \
    }
#define ICON_BLEND(destination)                                                \
    {                                                                          \
        njColorBlendingMode(NJD_SOURCE_COLOR, NJD_COLOR_BLENDING_SRCALPHA);    \
        njColorBlendingMode(NJD_DESTINATION_COLOR, (destination));             \
    }
/* a part is raised HELD_RAISE in view space while the player holds the Chao,
 * and REST_RAISE otherwise */
#define HELD_RAISE 0.7F
#define REST_RAISE (-0.3F)
#define LIFT_ICON()                                                            \
    {                                                                          \
        y = position.y;                                                        \
        if (chao->work->flags & WORK_HELD_BY_PLAYER) {                         \
            y += HELD_RAISE;                                                   \
        } else {                                                               \
            y += REST_RAISE;                                                   \
        }                                                                      \
        position.y = y;                                                        \
    }
/* a part's width, from 0.92 to 1.08 by the sine of angle. its height is 2.0F
 * minus its width */
#define SQUASH_WIDTH(angle) (((njSin(angle) + 1.0F) * 0.08F) + 0.92F)

/* the values of parameters.emotion_ball: the ball the Chao's type gives, or
 * the frames of dark_icon_frames */
enum { FORM_BALL, ANIMATED_BALL };

/* the sizes of a hero form's halo, a dark form's ball and a Chaos Chao's
 * halo. each is a multiple of the ball's */
#define HALO_SCALE 2.0F
#define DARK_BALL_SCALE 1.5F
#define CHAOS_HALO_SCALE 1.5F
#define QUAD_VERTICES 4
void chao_draw_icon(struct task *chao)
{
    struct chao_work *work = (struct chao_work *)chao->work;
    struct chao_icon *icon = &work->icon;
    struct chao_icon_part *mark = &icon->upper;
    struct chao_icon_part *ball = &icon->lower;
    NJS_POINT3 *ball_pos = &ball->position;
    struct chao_parameters *parameters;
    struct camera_place *camera;
    Float sx;
    Float sy;
    Float dx;
    Float dy;
    Float dot;
    Float y;
    NJS_TEXTURE_VTX *vertex;
    Angle angle;
    Sint32 degrees;
    NJS_POINT2 base;
    NJS_POINT2 tip;
    NJS_COLOR color;
    NJS_TEXTURE_VTX quad[QUAD_VERTICES];
    NJS_POINT3 position;
    NJS_VECTOR direction;
    position.x = ball_pos->x + icon->direction.x;
    position.y = ball_pos->y + icon->direction.y;
    position.z = ball_pos->z + icon->direction.z;
    njProjectScreen(NULL, ball_pos, &base);
    njProjectScreen(NULL, &position, &tip);
    color.color = work->icon.color;
    ICON_BLEND(NJD_COLOR_BLENDING_INVSRCALPHA);
    ICON_VERTEX(quad[0], -QUAD_HALF, -QUAD_HALF, 0.0F, 0.0F);
    ICON_VERTEX(quad[1], -QUAD_HALF, QUAD_HALF, 0.0F, 1.0F);
    /* @hack spelling: quad[2] set through a pointer, where quad[2] itself
     * would do */
    vertex = &quad[2];
    ICON_VERTEX(*vertex, QUAD_HALF, -QUAD_HALF, 1.0F, 0.0F);
    ICON_VERTEX(quad[3], QUAD_HALF, QUAD_HALF, 1.0F, 1.0F);
    if (mark->cell != CHAO_ICON_NO_MARK) {
        njCalcPoint(NULL, &mark->position, &position);
        LIFT_ICON();
        njPushMatrix(&_nj_unit_matrix_);
        /* the mark's squash runs a quarter turn ahead of the ball's */
        sx = SQUASH_WIDTH(icon->squash + QUARTER_TURN);
        sy = 2.0F - sx;
        sx *= mark->scale.x;
        sy *= mark->scale.y;
        position.x += mark->offset.x;
        njSetTexture(&chao_icon_texlist);
        njSetTextureNum(mark->cell);
        njTranslateV(NULL, &position);
        njScale(NULL, sx, sy, 0.1F);
        njDrawTexture3DEx(quad, QUAD_VERTICES, TRUE);
        njPopMatrixEx();
    }
    njCalcPoint(NULL, &ball->position, &position);
    LIFT_ICON();
    njPushMatrix(&_nj_unit_matrix_);
    parameters = &((struct chao_work *)chao->work)->profile.parameters;
    sx = SQUASH_WIDTH(icon->squash);
    sy = 2.0F - sx;
    sx *= ball->scale.x;
    sy *= ball->scale.y;
    njTranslateV(NULL, &position);
    njSetTexture(&chao_icon_texlist);
    switch ((Uint8)parameters->emotion_ball) {
    case FORM_BALL:
        /* a hero form, Hero Chaos included, wears a halo. its cell shows the
         * halo from the camera's side. its quad turns on the screen to stand
         * along the head's up direction */
        if (parameters->type >= CHAO_FIRST_HERO_TYPE &&
            (parameters->type - CHAO_FIRST_HERO_TYPE) % CHAO_FORM_ALIGNMENTS ==
                0) {
            camera = camera_views[0];
            direction.x = ball_pos->x - camera->pos.x;
            direction.y = ball_pos->y - camera->pos.y;
            direction.z = ball_pos->z - camera->pos.z;
            if (njScalor(&direction) > 1.0e-7F) {
                njUnitVector(&direction);
            } else {
                direction.x = 0.0F;
                direction.y = 1.0F;
                direction.z = 0.0F;
            }
            dot = njInnerProduct(&direction, &icon->direction);
            /* @bug memory: halo_view can be read outside its 181 entries.
             * acosf takes the dot unclamped, and rounding can take the dot of
             * two unit vectors past 1 or -1 (0x0c50e44c) */
#ifdef NONMATCHING
            /* this side repairs the bug tagged above: it clamps the dot to the
             * unit range */
            CLAMP_UNIT(dot);
#endif
            angle = (Angle)(acosf(-dot) * (Float)ANGLE_PER_RADIAN);
            sx *= HALO_SCALE;
            sy *= HALO_SCALE;
            degrees = (Sint32)((angle * DEGREES_PER_ANGLE) + 0.5F);
            njSetTextureNum(halo_view[degrees] + CHAO_ICON_HALO);
            dx = tip.x - base.x;
            dy = tip.y - base.y;
            angle = (Angle)(atan2f(-dx, -dy) * (Float)ANGLE_PER_RADIAN);
            njRotateZ(NULL, -angle);
            njScale(NULL, sx, sy, 1.0F);
            if (parameters->type == CHAO_HERO_CHAOS) {
                ICON_BLEND(NJD_COLOR_BLENDING_ONE);
            }
            njDrawTexture3DEx(quad, QUAD_VERTICES, TRUE);
        } else if (parameters->type >= CHAO_FIRST_DARK_TYPE &&
                   (parameters->type - CHAO_FIRST_DARK_TYPE) %
                           CHAO_FORM_ALIGNMENTS ==
                       0) {
            sx *= DARK_BALL_SCALE;
            sy *= DARK_BALL_SCALE;
            if (parameters->type != CHAO_DARK_CHAOS) {
                njSetTextureNum(CHAO_ICON_DARK_BALL);
                njScale(NULL, sx, sy, 1.0F);
                njDrawTexture3DEx(quad, QUAD_VERTICES, TRUE);
            } else {
                njSetTextureNum(dark_icon_frames[icon->texture]);
                njScale(NULL, sx, sy, 1.0F);
                ICON_BLEND(NJD_COLOR_BLENDING_ONE);
                njDrawTexture3DEx(quad, QUAD_VERTICES, TRUE);
            }
        } else if (parameters->type != CHAO_CHAOS) {
            njSetTextureNum(ball->cell);
            njScale(NULL, sx, sy, 1.0F);
            njDrawTexture3DEx(quad, QUAD_VERTICES, TRUE);
        } else {
            njSetTextureNum(CHAO_ICON_HALO);
            ICON_BLEND(NJD_COLOR_BLENDING_ONE);
            njRotateZ(NULL, icon->random_angle);
            njScale(NULL, sx * CHAOS_HALO_SCALE, sy * CHAOS_HALO_SCALE, 1.0F);
            njDrawTexture3DEx(quad, QUAD_VERTICES, TRUE);
        }
        break;
    case ANIMATED_BALL:
        njSetTextureNum(dark_icon_frames[icon->texture]);
        njScale(NULL, sx, sy, 1.0F);
        ICON_BLEND(NJD_COLOR_BLENDING_ONE);
        njDrawTexture3DEx(quad, QUAD_VERTICES, TRUE);
        break;
    }
    njPopMatrixEx();
    ICON_BLEND(NJD_COLOR_BLENDING_INVSRCALPHA);
}
#define MARK_SOUND 0x100e
void chao_show_mark(struct task *chao, Sint8 mark, int frames)
{
    struct chao_icon *icon = ICON(chao);
    icon->next_mode = mark;
    icon->timer = 0;
    icon->next_timer = frames;
    sound_play_at(MARK_SOUND, NULL, 0, 0, &chao->work->pos);
}
void chao_clear_emotion_mark(struct task *chao)
{
    struct chao_icon *icon = ICON(chao);
    icon->next_mode = CHAO_MARK_NONE;
    icon->timer = 0;
    icon->next_timer = 0;
}
