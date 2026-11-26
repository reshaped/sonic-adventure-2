/* the menus' helpers: their color, steps toward a target, sequences, the
 * constant material and a point's move */
#include <ninja.h>
#include <sg_xpt.h>

#include "advertise/menu_util.h"
#include "draw/lines.h"
#include "main.h"
#include "system/flags.h"
#include "system/game_state.h"
#include "system/sequence.h"
#include "system/util.h"

#define COLOR_STEP (1.0F / 30)

/* The step at which menu_start_sequence_at found the running sequence. */
static Sint32 menu_kept_sequence_step;

NJS_ARGB menu_color = {
#include "advertise/menu_util/menu_color.inc"
};
static NJS_ARGB menu_colors[MENU_THEMES] = {
#include "advertise/menu_util/menu_colors.inc"
};

void menu_color_step(void)
{
    menu_color.a = 1.0F;
    menu_approach(&menu_color.r, menu_colors[game_state.theme].r, COLOR_STEP);
    menu_approach(&menu_color.g, menu_colors[game_state.theme].g, COLOR_STEP);
    menu_approach(&menu_color.b, menu_colors[game_state.theme].b, COLOR_STEP);
}

Bool menu_approach(Float *value, Float target, Float step)
{
    if (*value < target) {
        *value += step;
        if (*value > target) {
            *value = target;
        }
        return TRUE;
    }
    if (*value > target) {
        *value -= step;
        if (*value < target) {
            *value = target;
        }
        return TRUE;
    }
    return FALSE;
}

Bool menu_accelerate(const Float *value, Float target, Float *speed, Float step)
{
    if (*value < target) {
        *speed += step;
        return TRUE;
        /* @hack spelling: the else after a branch that returns, where an if of
         * its own would do */
    } else if (*value > target) {
        *speed -= step;
        return TRUE;
    }
    return FALSE;
}

Bool menu_approach_int(Sint32 *value, Sint32 target, Sint32 step)
{
    if (*value < target) {
        *value += step;
        if (*value > target) {
            *value = target;
        }
        return TRUE;
        /* @hack spelling: the else after a branch that returns, where an if of
         * its own would do */
    } else if (*value > target) {
        *value -= step;
        if (*value < target) {
            *value = target;
        }
        return TRUE;
    }
    return FALSE;
}

void menu_play_taiken_event(void)
{
    sequence_start(SEQUENCE_TAIKEN_EVENT);
    game_state.next_mode = GAME_MODE_SEQUENCE;
}

void menu_play_sequence(int sequence)
{
    sequence_start(sequence);
    game_state.next_mode = GAME_MODE_SEQUENCE;
}

void menu_start_sequence_at(int sequence, int step)
{
    menu_kept_sequence_step = game_flags[FLAG_SEQUENCE_STEP];
    sequence_start(sequence);
    game_flags[FLAG_SEQUENCE_STEP] = step;
    if (step > 0) {
        game_flags[FLAG_SEQUENCE_STEP_COPY] = step - 1;
    } else {
        game_flags[FLAG_SEQUENCE_STEP_COPY] = step;
    }
}

void menu_to_sequence(void)
{
    game_state.next_mode = GAME_MODE_SEQUENCE;
}

void menu_restore_sequence_step(void)
{
    game_flags[FLAG_SEQUENCE_STEP] = menu_kept_sequence_step;
}

void menu_fade_begin(Float level)
{
    draw_attr_save();
    constant_material_set(level, level, level, level);
}

void menu_tint_begin(Float alpha, Float red, Float green, Float blue)
{
    draw_attr_save();
    constant_material_set(alpha, red, green, blue);
}

void menu_tint_end(void)
{
    constant_material_set(0.0F, 0.0F, 0.0F, 0.0F);
    draw_attr_restore();
}

void menu_lerp(const struct menu_lerp *lerp)
{
    lerp->out->x =
        ((lerp->time - lerp->from_time) * (lerp->to->x - lerp->from->x) /
         (lerp->to_time - lerp->from_time)) +
        lerp->from->x;
    lerp->out->y =
        ((lerp->time - lerp->from_time) * (lerp->to->y - lerp->from->y) /
         (lerp->to_time - lerp->from_time)) +
        lerp->from->y;
    lerp->out->z =
        ((lerp->time - lerp->from_time) * (lerp->to->z - lerp->from->z) /
         (lerp->to_time - lerp->from_time)) +
        lerp->from->z;
}
