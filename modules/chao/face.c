#include <ninja.h>
#include <sg_xpt.h>
#include <stdlib.h>

#include "chao/body.h"
#include "chao/face.h"
#include "chao/life.h"
#include "chao/model.h"
#include "chao/work.h"
#include "system/angle.h"
#include "system/task.h"
#include "system/util.h"

#define FACE(chao) (&CHAO_WORK(chao)->face)

/* the colors of the eye that the lids blink over. an adult takes the one of
 * its type's alignment, and each Chaos Chao type has its own */
enum eye_color {
    COLOR_NEUTRAL,
    COLOR_HERO,
    COLOR_DARK,
    COLOR_CHAOS,
    COLOR_HERO_CHAOS,
    COLOR_DARK_CHAOS
};

/* the mouth a Chaos Chao shows, whatever it is asked to */
#define CHAOS_MOUTH 0

/* under LIDS_ON the lids move over an eye of the Chao's color. a drawn eye
 * sets LIDS_OFF, which holds them open and keeps the blink off */
enum lid_mode { LIDS_ON, LIDS_OFF };

enum blink_mode { BLINK_OPENING, BLINK_CLOSING, BLINK_WAIT, BLINK_DISABLED };

/* the lids' turn each step toward their target closure, and their target
 * slant */
#define LID_SPEED 0x400
#define SLANT_SPEED 0x200
/* a blink closes the lids, and opens them again, BLINK_SPEED a step */
#define BLINK_SPEED 0x1000
/* the steps the lids wait to blink. the first wait is below
 * BLINK_FIRST_WAIT. a later wait lasts BLINK_WAIT_MIN steps and a random part
 * of BLINK_WAIT_SPAN more, with chance BLINK_LONG_CHANCE. any other wait is
 * below BLINK_SHORT_WAIT */
#define BLINK_FIRST_WAIT 300.0F
#define BLINK_LONG_CHANCE 0.85F
#define BLINK_WAIT_MIN 100.0F
#define BLINK_WAIT_SPAN 400.0F
#define BLINK_SHORT_WAIT 5.0F

/* the lids of a narrowed eye, and of an open one under a narrowed default */
#define NARROW_CLOSURE 0x3555
#define NARROW_SLOPE 0xe38
/* a half-shut eye whose lids close past HALF_SHUT_DOWN shows eye
 * HALF_SHUT_DRAWN's texture in its place */
#define HALF_SHUT_DOWN 0x3c00
#define HALF_SHUT_DRAWN 5

/* the texture of each eye, and the MOUTH_MATERIALS of each mouth, one for
 * each of its materials. eye_color_textures gives each eye color's texture */
#define MOUTH_MATERIALS 2
static const Sint16 eye_textures[] = {0, 1, 2, 3, 4,  5,  6,
                                      7, 5, 0, 0, 10, 11, 12};
static const Sint16 mouth_textures[][MOUTH_MATERIALS] = {
    {0, 0}, {2, 1},   {3, 0},   {4, 0},  {5, 0},   {6, 0},  {7, 0},
    {9, 8}, {11, 10}, {13, 12}, {14, 0}, {16, 15}, {18, 17}};
static const Sint16 eye_color_textures[] = {0, 9, 8, 10, 11, 12};

void chao_face_init(struct task *chao)
{
    struct chao_work *work = CHAO_WORK(chao);
    struct chao_shape_work *shape = &work->shape;
    struct chao_face *face = &work->face;
    struct chao_parameters *parameters = &work->profile.parameters;

    face->eye_x = 0.0F;
    face->eye_y = 0.0F;
    face->eye_scale_x = 1.0F;
    face->eye_scale_y = 1.0F;
    face->lid_mode = LIDS_ON;
    face->default_closure = 0;
    face->closure = 0;
    face->target_closure = 0;
    face->default_slope = 0;
    face->slope = 0;
    face->target_slope = 0;
    /* @hack spelling: the bound CHAO_CHAOS - 1 under <=, where < CHAO_CHAOS
     * would do */
    if (parameters->type >= CHAO_NORMAL_ADULT &&
        parameters->type <= CHAO_CHAOS - 1) {
        switch ((parameters->type - CHAO_NORMAL_ADULT) % CHAO_FORM_ALIGNMENTS) {
        case 0:
            face->eye_color = COLOR_NEUTRAL;
            break;
        case 1:
            face->eye_color = COLOR_HERO;
            break;
        case 2:
            face->eye_color = COLOR_DARK;
            break;
        }
    } else {
        face->eye_color = COLOR_NEUTRAL;
        switch (parameters->type) {
        case CHAO_CHAOS:
            face->eye_color = COLOR_CHAOS;
            break;
        case CHAO_HERO_CHAOS:
            face->eye_color = COLOR_HERO_CHAOS;
            break;
        case CHAO_DARK_CHAOS:
            face->eye_color = COLOR_DARK_CHAOS;
            break;
        }
    }
    if (chao_is_dark(chao)) {
        parameters->eye = CHAO_EYE_NARROW;
    }
    switch (parameters->type) {
    case CHAO_CHAOS:
        face->eye = face->default_eye = CHAO_EYE_CHAOS;
        face->mouth = face->default_mouth = CHAOS_MOUTH;
        break;
    case CHAO_HERO_CHAOS:
        face->eye = face->default_eye = CHAO_EYE_HERO_CHAOS;
        face->mouth = face->default_mouth = CHAOS_MOUTH;
        break;
    case CHAO_DARK_CHAOS:
        face->eye = face->default_eye = CHAO_EYE_DARK_CHAOS;
        face->mouth = face->default_mouth = CHAOS_MOUTH;
        break;
    default:
        face->eye = face->default_eye = (Uint8)parameters->eye;
        face->mouth = face->default_mouth = (Uint8)parameters->mouth;
        break;
    }
    face->eye_node[0] = shape->nodes[CHAO_NODE_EYE_0];
    face->eye_node[1] = shape->nodes[CHAO_NODE_EYE_1];
    face->mouth_node = shape->nodes[CHAO_NODE_MOUTH];
    face->blink_mode = BLINK_WAIT;
    face->blink_timer = rand() * (BLINK_FIRST_WAIT * RAND_SCALE);
    face->blink_angle = 0;
    chao_set_eye(chao, face->eye);
    chao_set_mouth(chao, face->mouth);
}

Angle chao_eyelid_angle(struct task *chao)
{
    struct chao_face *face = FACE(chao);

    return face->blink_angle + face->closure - QUARTER_TURN;
}

Angle chao_eyelid_slope(struct task *chao)
{
    struct chao_face *face = FACE(chao);

    return face->slope;
}

static void step_eyelids(struct task *chao)
{
    struct chao_face *face = FACE(chao);

    face->closure =
        angle_approach(face->closure, face->target_closure, LID_SPEED);
    face->slope = angle_approach(face->slope, face->target_slope, SLANT_SPEED);
    switch (face->lid_mode) {
    case LIDS_ON:
        switch (chao_current_eye(chao)) {
        case CHAO_EYE_OPEN:
        case CHAO_EYE_HALF_SHUT:
        case CHAO_EYE_NARROW:
            break;
        default:
            face->lid_mode = LIDS_OFF;
            face->closure = 0;
            face->blink_mode = BLINK_DISABLED;
            face->blink_angle = 0;
            break;
        }
        break;
    case LIDS_OFF:
        switch (chao_current_eye(chao)) {
        case CHAO_EYE_OPEN:
        case CHAO_EYE_HALF_SHUT:
        case CHAO_EYE_NARROW:
            face->lid_mode = LIDS_ON;
            break;
        }
        break;
    }
    switch (face->blink_mode) {
    case BLINK_OPENING:
        if (face->blink_angle < BLINK_SPEED) {
            face->blink_angle = 0;
            face->blink_mode = BLINK_WAIT;
            if (rand() * RAND_SCALE < BLINK_LONG_CHANCE) {
                face->blink_timer =
                    (rand() * (BLINK_WAIT_SPAN * RAND_SCALE)) + BLINK_WAIT_MIN;
            } else {
                face->blink_timer = rand() * (BLINK_SHORT_WAIT * RAND_SCALE);
            }
        } else {
            face->blink_angle -= BLINK_SPEED;
        }
        break;
    case BLINK_CLOSING:
        face->blink_angle += BLINK_SPEED;
        if (face->closure + face->blink_angle > HALF_TURN) {
            face->blink_angle = HALF_TURN - face->closure;
            face->blink_mode = BLINK_OPENING;
        }
        break;
    case BLINK_WAIT:
        if (face->blink_timer-- <= 0) {
            face->blink_timer = 0;
            face->blink_mode = BLINK_CLOSING;
        }
        break;
    case BLINK_DISABLED:
        if (face->lid_mode != LIDS_OFF) {
            face->blink_mode = BLINK_WAIT;
        }
        break;
    }
}

void chao_face_step(struct task *chao)
{
    struct chao_face *face = FACE(chao);
    int texture;

    /* @hack redundant: the empty branches for a held eye and a held mouth,
     * where the tests of 0 alone would do */
    if (face->eye_timer > 0) {
        face->eye_timer--;
    } else if (face->eye_timer == CHAO_FACE_HOLD) {
    } else if (face->eye_timer == 0) {
        chao_restore_eyes(chao);
    }
    if (face->mouth_timer > 0) {
        face->mouth_timer--;
    } else if (face->mouth_timer == CHAO_FACE_HOLD) {
    } else if (face->mouth_timer == 0) {
        chao_restore_mouth(chao);
    }
    step_eyelids(chao);
    switch (CHAO_WORK(chao)->profile.parameters.type) {
    case CHAO_CHAOS:
    case CHAO_HERO_CHAOS:
    case CHAO_DARK_CHAOS:
        break;
    default:
        if (chao_current_eye(chao) == CHAO_EYE_HALF_SHUT &&
            face->closure > HALF_SHUT_DOWN) {
            texture = eye_textures[HALF_SHUT_DRAWN];
            garden_set_model_texture(face->eye_node[0], texture);
            garden_set_model_texture(face->eye_node[1], texture);
        }
        break;
    }
}

Sint16 chao_current_eye(struct task *chao)
{
    struct chao_face *face = FACE(chao);

    return face->eye;
}

void chao_set_eye(struct task *chao, int eye)
{
    chao_set_timed_eye(chao, eye, CHAO_FACE_HOLD);
}

void chao_set_timed_eye(struct task *chao, int eye, int frames)
{
    struct chao_face *face = FACE(chao);
    int texture;

    face->eye = eye;
    face->eye_timer = frames;
    switch (chao_current_eye(chao)) {
    default:
        face->target_closure = 0;
        face->target_slope = 0;
        break;
    case CHAO_EYE_HALF_SHUT:
        face->target_closure = QUARTER_TURN;
        face->target_slope = 0;
        break;
    case CHAO_EYE_OPEN:
        if (face->default_eye == CHAO_EYE_NARROW) {
            face->target_closure = NARROW_CLOSURE;
            face->target_slope = NARROW_SLOPE;
        } else {
            face->target_closure = 0;
            face->target_slope = 0;
        }
        break;
    case CHAO_EYE_NARROW:
        face->target_closure = NARROW_CLOSURE;
        face->target_slope = NARROW_SLOPE;
        break;
    }
    switch (chao_current_eye(chao)) {
    case CHAO_EYE_OPEN:
    case CHAO_EYE_HALF_SHUT:
    case CHAO_EYE_NARROW:
        texture = eye_color_textures[face->eye_color];
        break;
    default:
        texture = eye_textures[eye];
        break;
    }
    switch (CHAO_WORK(chao)->profile.parameters.type) {
    case CHAO_CHAOS:
    case CHAO_HERO_CHAOS:
    case CHAO_DARK_CHAOS:
        if (chao_current_eye(chao) == CHAO_EYE_ASLEEP) {
            texture = eye_textures[eye];
        } else {
            texture = eye_color_textures[face->eye_color];
        }
        break;
    }
    garden_set_model_texture(face->eye_node[0], texture);
    garden_set_model_texture(face->eye_node[1], texture);
}

void chao_set_mouth(struct task *chao, int mouth)
{
    chao_set_timed_mouth(chao, mouth, CHAO_FACE_HOLD);
}

void chao_set_timed_mouth(struct task *chao, int mouth, int frames)
{
    struct chao_face *face = FACE(chao);
    const Sint16 *textures;

    face->mouth_timer = frames;
    /* @hack spelling: a call in each case, where one after the switch would
     * do */
    switch (CHAO_WORK(chao)->profile.parameters.type) {
    case CHAO_CHAOS:
    case CHAO_HERO_CHAOS:
    case CHAO_DARK_CHAOS:
        textures = mouth_textures[CHAOS_MOUTH];
        garden_set_model_textures(face->mouth_node, textures, MOUTH_MATERIALS);
        break;
    default:
        textures = mouth_textures[mouth];
        garden_set_model_textures(face->mouth_node, textures, MOUTH_MATERIALS);
        break;
    }
}

int chao_default_eye(struct task *chao)
{
    struct chao_work *work = CHAO_WORK(chao);
    struct chao_face *face = &work->face;

    return face->default_eye;
}

int chao_default_mouth(struct task *chao)
{
    struct chao_work *work = CHAO_WORK(chao);
    struct chao_face *face = &work->face;

    return face->default_mouth;
}

void chao_restore_eyes(struct task *chao)
{
    struct chao_face *face = FACE(chao);

    chao_set_eye(chao, face->default_eye);
}

void chao_restore_mouth(struct task *chao)
{
    struct chao_face *face = FACE(chao);

    chao_set_mouth(chao, face->default_mouth);
}
