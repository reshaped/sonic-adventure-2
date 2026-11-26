/* the pictures a Chao draws on the garden floor, as drawing.h declares
 * them */
#include <ninja.h>
#include <sg_xpt.h>
#include <stdlib.h>

#include "chao/behavior.h"
#include "chao/body.h"
#include "chao/body_collision.h"
#include "chao/drawing.h"
#include "chao/egg.h"
#include "chao/face.h"
#include "chao/garden_field.h"
#include "chao/knowledge.h"
#include "chao/life.h"
#include "chao/motion.h"
#include "chao/motion_ids.h"
#include "chao/movement.h"
#include "chao/parts.h"
#include "chao/small_toys.h"
#include "chao/toy_art.h"
#include "chao/work.h"
#include "chao/world.h"
#include "collision/collision.h"
#include "collision/surface.h"
#include "sound/sound.h"
#include "system/task.h"
#include "system/util.h"

/* the kind of the field the Chao's crayon makes while it draws */
#define CRAYON_FIELD 169
#define CRAYON_RADIUS 2.0F
/* a picture fills in by FILL_STEPS steps, at most one each FILL_FRAMES
 * frames. it takes a step only while the crayon's field touches it. it stops
 * filling in after DRAWING_FRAMES, which the field lasts and the Chao draws
 * for */
#define FILL_STEPS 3
#define FILL_FRAMES 580
#define DRAWING_FRAMES 1800
#define DRAWING_LIMIT 100 /* the most pictures that stand at once */
#define PICTURE_TEXTURE 36 /* egg_texlist's first picture */
/* the players a Chao draws portraits of, by their index in chao_knowledge's
 * players. a portrait's picture is its player's index */
#define PORTRAIT_PLAYERS 6
#define ART_LESSONS 5
/* the pictures after the portraits: a form's, by its alignment, and each art
 * lesson's, by its index. lesson 1 draws one by the form's alignment too */
enum picture {
    PICTURE_NEUTRAL_FORM = PORTRAIT_PLAYERS,
    PICTURE_HERO_FORM,
    PICTURE_DARK_FORM,
    PICTURE_LESSON_0,
    PICTURE_LESSON_1_NEUTRAL,
    PICTURE_LESSON_1_HERO,
    PICTURE_LESSON_1_DARK,
    PICTURE_LESSON_2,
    PICTURE_LESSON_4,
    PICTURE_LESSON_3
};
/* the shape by which a picture finds the crayon's field */
#define PAPER_SHAPE 0
/* how far above a picture's place the floor's probe starts */
#define PROBE_RISE 10.0F
/* a drawing Chao may scribble aloud once each SCRIBBLE_PERIOD frames, with a
 * chance of SCRIBBLE_CHANCE */
#define SCRIBBLE_PERIOD 120
#define SCRIBBLE_CHANCE 0.3F
#define SCRIBBLE_VOLUME 120
#define PICTURE_AHEAD 3.0F /* the picture's distance ahead of the Chao */
#define PICTURE_HALF 2.0F /* half the side of the square a picture fills */
#define PICTURE_LIFT 0.1F /* how far above the floor a picture draws */
/* the least affinity of a player the Chao draws a portrait of */
#define FAVORITE_AFFINITY 50

static Sint32 drawing_count = 0;
/* clang-format off */
static const struct collision_shape paper_shape[] = {
    {0, SHAPE_SPHERE, PUSH_FIRM(PUSH_ALL_BUT_PICKUPS), SHAPE_GUARD_LEVEL(3),
     SHAPE_IGNORE_UNK_9 | SHAPE_IGNORE_PICKUPS | SHAPE_IGNORE_PLAYERS |
         SHAPE_UNTURNED,
     {0.0F, 1.0F, 0.0F}, 2.0F, 0.0F, 0.0F, 0, {0, 0, 0}}};
/* clang-format on */
static struct chao_placement crayon_case = {
    &chao_crayon_case, &garden_toy_texlist, {0.0F, 0.0F, 0.0F}, {0, 0, 0}};
static struct chao_placement crayon = {
    &chao_crayon, &garden_toy_texlist, {0.0F, 0.0F, 0.0F}, {0, 0, 0}};

static void drawing_step(struct task *drawing)
{
    struct task_work *work = drawing->work;
    if (work->timer++ % FILL_FRAMES == 0) {
        if (work->tick < FILL_STEPS &&
            garden_hit_kind(drawing, PAPER_SHAPE, CRAYON_FIELD)) {
            work->tick++;
        }
    }
    if (work->tick < FILL_STEPS && work->timer < DRAWING_FRAMES) {
        collision_register(drawing);
    } else {
        collision_reset(drawing);
    }
}

#define CORNER(i, xpos, zpos, ucoord, vcoord)                                  \
    vertices[i].x = (xpos);                                                    \
    vertices[i].y = PICTURE_LIFT;                                              \
    vertices[i].z = (zpos);                                                    \
    vertices[i].u = (ucoord);                                                  \
    vertices[i].v = (vcoord);                                                  \
    vertices[i].col = color

static void drawing_draw(struct task *drawing)
{
    struct task_work *work = drawing->work;
    Uint32 opacity[FILL_STEPS + 1] = {0x00ffffff, 0x3fffffff, 0xafffffff,
                                      0xffffffff};
    NJS_TEXTURE_VTX vertices[4];
    Uint32 color = opacity[work->tick];
    CORNER(0, -PICTURE_HALF, PICTURE_HALF, 0.0F, 0.0F);
    CORNER(1, -PICTURE_HALF, -PICTURE_HALF, 0.0F, 1.0F);
    CORNER(2, PICTURE_HALF, PICTURE_HALF, 1.0F, 0.0F);
    CORNER(3, PICTURE_HALF, -PICTURE_HALF, 1.0F, 1.0F);
    njSetTexture(&egg_texlist);
    njSetTextureNum(work->submode + PICTURE_TEXTURE);
    njPushMatrixEx();
    njTranslateEx(&work->pos);
    njRotateZ(NULL, work->ang.z);
    njRotateX(NULL, work->ang.x);
    njRotateY(NULL, work->ang.y);
    njDrawTexture3DEx(vertices, 4, 1);
    njPopMatrixEx();
}

static void drawing_destroy(struct task *drawing)
{
    drawing_count--;
}

static void create_drawing(int picture, NJS_POINT3 *position, Angle yaw)
{
    struct task *drawing =
        task_create(TASK_WORK, 2, drawing_step, "ALO_RakugakiExecutor");
    struct task_work *work;
    drawing_count++;
    work = drawing->work;
    collision_init(drawing, paper_shape,
                   sizeof(paper_shape) / sizeof(paper_shape[0]),
                   COLLISION_OBJECT_MUTUAL);
    work->pos = *position;
    work->pos.y = surface_probe_floor(position->x, position->y + PROBE_RISE,
                                      position->z, &work->ang);
    work->ang.y = yaw;
    work->submode = picture;
    drawing->draw = drawing_draw;
    drawing->destructor = drawing_destroy;
}

static int draw_picture(struct task *chao)
{
    struct chao_behavior *behavior = &CHAO_WORK(chao)->behavior;
    struct task_work *work;
    NJS_POINT3 *center;
    Angle yaw;
    NJS_POINT3 position;
    switch (behavior->mode) {
    case 0:
        chao_resist_displacement(chao);
        chao_set_timed_eye(chao, CHAO_EYE_GLAD, CHAO_FACE_HOLD);
        chao_link_motion_frames(chao, CHAO_ANIM_RAKUGAKI_JYUNBI, 20);
        chao_equip_node_at(chao, CHAO_NODE_HAND_4, &crayon_case);
        behavior->mode++;
        /* fall through */
    case 1:
        if (chao_motion_ended(chao)) {
            chao_equip_node_at(chao, CHAO_NODE_HAND_11, &crayon);
            behavior->mode++;
        }
        break;
    case 2:
        if (chao_motion_held(chao)) {
            work = chao->work;
            center = &work->pos;
            yaw = work->ang.y;
            position.x = center->x + (njSin(yaw) * PICTURE_AHEAD);
            position.y = center->y;
            position.z = center->z + (njCos(yaw) * PICTURE_AHEAD);
            garden_create_timed_field(chao, CRAYON_FIELD, &position,
                                      CRAYON_RADIUS, DRAWING_FRAMES);
            create_drawing(chao_behavior_value(chao), &position, yaw);
            chao_link_motion_frames(chao, CHAO_ANIM_RAKUGAKI, 20);
            behavior->timer = DRAWING_FRAMES;
            behavior->mode++;
        }
        break;
    case 3:
        if (behavior->timer-- <= 0) {
            return BEHAVIOR_FINISH;
        }
        if (behavior->count++ % SCRIBBLE_PERIOD == 0 &&
            RANDOM_UNIT() < SCRIBBLE_CHANCE) {
            sound_play_at(0x1026, NULL, 0, SCRIBBLE_VOLUME, &chao->work->pos);
        }
        break;
    }
    return BEHAVIOR_CONTINUE;
}

#define FORM_PICTURE(type, neutral, hero, dark, result)                        \
    switch (type) {                                                            \
    case CHAO_HERO_TYPE(CHAO_NORMAL_ADULT):                                    \
    case CHAO_HERO_TYPE(CHAO_SWIM_ADULT):                                      \
    case CHAO_HERO_TYPE(CHAO_FLY_ADULT):                                       \
    case CHAO_HERO_TYPE(CHAO_RUN_ADULT):                                       \
    case CHAO_HERO_TYPE(CHAO_POWER_ADULT):                                     \
    case CHAO_HERO_CHAOS:                                                      \
        (result) = (hero);                                                     \
        break;                                                                 \
    case CHAO_DARK_TYPE(CHAO_NORMAL_ADULT):                                    \
    case CHAO_DARK_TYPE(CHAO_SWIM_ADULT):                                      \
    case CHAO_DARK_TYPE(CHAO_FLY_ADULT):                                       \
    case CHAO_DARK_TYPE(CHAO_RUN_ADULT):                                       \
    case CHAO_DARK_TYPE(CHAO_POWER_ADULT):                                     \
    case CHAO_DARK_CHAOS:                                                      \
        (result) = (dark);                                                     \
        break;                                                                 \
    default:                                                                   \
        (result) = (neutral);                                                  \
        break;                                                                 \
    }

static int choose_portrait(struct task *chao)
{
    struct chao_knowledge *knowledge = &CHAO_WORK(chao)->profile.knowledge;
    struct chao_parameters *parameters;
    int favorites[PORTRAIT_PLAYERS];
    int count = 0;
    int best = FAVORITE_AFFINITY;
    int i;
    for (i = 0; i < PORTRAIT_PLAYERS; i++) {
        if (knowledge->players[i].affinity >= best) {
            /* @bug logic: the test sets the player's affinity to best where
             * it means to compare them, so each player at 50 or more falls to
             * 50 and joins favorites as a tie (0x0c5293a0) */
#ifdef NONMATCHING
            /* this side repairs the bug tagged above: it compares, where the
             * matching side assigns */
            if (knowledge->players[i].affinity == best) {
#else
            if ((knowledge->players[i].affinity = best)) {
#endif
                favorites[count] = i;
                count++;
            } else {
                favorites[0] = i;
                count = 1;
                best = knowledge->players[i].affinity;
            }
        }
    }
    if (count > 0 && RANDOM_UNIT() < 0.8F) {
        return favorites[(Uint32)RANDOM_BELOW(count)];
    }
    parameters = &CHAO_WORK(chao)->profile.parameters;
    /* @hack spelling: FORM_PICTURE's switch written out, with a return in
     * each arm, where FORM_PICTURE would do */
    switch (parameters->type) {
    case CHAO_HERO_TYPE(CHAO_NORMAL_ADULT):
    case CHAO_HERO_TYPE(CHAO_SWIM_ADULT):
    case CHAO_HERO_TYPE(CHAO_FLY_ADULT):
    case CHAO_HERO_TYPE(CHAO_RUN_ADULT):
    case CHAO_HERO_TYPE(CHAO_POWER_ADULT):
    case CHAO_HERO_CHAOS:
        return PICTURE_HERO_FORM;
    case CHAO_DARK_TYPE(CHAO_NORMAL_ADULT):
    case CHAO_DARK_TYPE(CHAO_SWIM_ADULT):
    case CHAO_DARK_TYPE(CHAO_FLY_ADULT):
    case CHAO_DARK_TYPE(CHAO_RUN_ADULT):
    case CHAO_DARK_TYPE(CHAO_POWER_ADULT):
    case CHAO_DARK_CHAOS:
        return PICTURE_DARK_FORM;
    default:
        return PICTURE_NEUTRAL_FORM;
    }
}

Bool chao_try_drawing(struct task *chao)
{
    struct task_work *work = chao->work;
    int kind;
    /* @hack type: lessons holds eight, where ART_LESSONS would do */
    int lessons[8];
    int count = 0;
    int i;
    int picture;
    struct chao_parameters *parameters;
    if (drawing_count >= DRAWING_LIMIT) {
        return FALSE;
    }
    for (i = 0; i < ART_LESSONS; i++) {
        if (chao_knows_art(chao, i)) {
            lessons[count] = i;
            count++;
        }
    }
    if (count > 0 && RANDOM_UNIT() < 0.5F) {
        NJS_POINT3 position;
        Angle yaw;
        kind = lessons[(int)(rand() * (count - 0.00001F) * RAND_SCALE)];
        /* @hack redundant: a place 8 ahead of the Chao, which
         * nothing reads */
        yaw = work->ang.y;
        position.x = work->pos.x + (njSin(yaw) * 8.0F);
        position.y = work->pos.y;
        position.z = work->pos.z + (njCos(yaw) * 8.0F);
        chao_set_behavior(chao, draw_picture);
        switch (kind) {
        case 0:
            picture = PICTURE_LESSON_0;
            break;
        case 1:
            parameters = &CHAO_WORK(chao)->profile.parameters;
            FORM_PICTURE(parameters->type, PICTURE_LESSON_1_NEUTRAL,
                         PICTURE_LESSON_1_HERO, PICTURE_LESSON_1_DARK, picture);
            break;
        case 2:
            picture = PICTURE_LESSON_2;
            break;
        case 3:
            picture = PICTURE_LESSON_3;
            break;
        case 4:
            picture = PICTURE_LESSON_4;
            break;
            /* lessons holds only 0 to 4, so kind meets one of the cases
             * above */
#ifdef NONMATCHING
        /* this side returns FALSE for any other kind. the matching side would
         * go on with picture unset */
        default:
            return FALSE;
#endif
        }
        chao_set_behavior_value(chao, picture);
        return TRUE;
    } else if (chao_has_small_toy(chao, CHAO_CRAYON_TOY)) {
        NJS_POINT3 position;
        /* @hack redundant: a place 8 ahead of the Chao, which
         * nothing reads */
        Angle yaw = work->ang.y;
        position.x = work->pos.x + (njSin(yaw) * 8.0F);
        position.y = work->pos.y;
        position.z = work->pos.z + (njCos(yaw) * 8.0F);
        chao_set_behavior(chao, draw_picture);
        chao_set_behavior_value(chao, choose_portrait(chao));
        return TRUE;
    }
    return FALSE;
}
