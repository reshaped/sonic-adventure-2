/* the menus' helpers: their color, steps toward a target, sequences, the
 * constant material and a point's move */
#ifndef ADVERTISE_MENU_UTIL_H
#define ADVERTISE_MENU_UTIL_H
#include <ninja.h>
#include <sg_xpt.h>

#include "system/menu_snapshot.h" /* IWYU pragma: export */
/* the menus' color, which menu_color_step moves toward the theme's */
extern NJS_ARGB menu_color;

/* move menu_color's red, green and blue each 1/30 toward game_state.theme's
 * color, and set its alpha to 1 */
void menu_color_step(void);

/* move *value toward target by step, stopping at target. each returns FALSE
 * if *value was target already, and TRUE otherwise. step must not be
 * negative, unchecked */
Bool menu_approach(Float *value, Float target, Float step);
Bool menu_approach_int(Sint32 *value, Sint32 target, Sint32 step);
/* add step to *speed if *value is below target, or take step from it if
 * *value is above. it returns FALSE if *value is target, and TRUE
 * otherwise */
Bool menu_accelerate(const Float *value, Float target, Float *speed,
                     Float step);

/* start sequence, as sequence_start does, and set game_state.next_mode to
 * GAME_MODE_SEQUENCE. the sequence plays once the menus end.
 * menu_play_taiken_event is menu_play_sequence of SEQUENCE_TAIKEN_EVENT */
void menu_play_taiken_event(void);
void menu_play_sequence(int sequence);
/* start sequence at step, and keep the step the running sequence had
 * reached for menu_restore_sequence_step. FLAG_SEQUENCE_STEP_COPY becomes
 * step less 1 where step is above 0, and step otherwise.
 * game_state.next_mode stays as it was */
void menu_start_sequence_at(int sequence, int step);
/* menu_play_sequence's second half, for a sequence menu_start_sequence_at
 * started */
void menu_to_sequence(void);
/* give FLAG_SEQUENCE_STEP back the step menu_start_sequence_at kept. the
 * sequence stays the one it started */
void menu_restore_sequence_step(void);

/* menu_fade_begin sets each channel of the constant material to level.
 * menu_tint_begin sets them to alpha, red, green and blue. each first saves
 * the constant attributes with draw_attr_save, which keeps one save. so
 * call menu_tint_end before the next begin, unchecked. menu_tint_end sets
 * the material to 0, and restores the attributes */
void menu_fade_begin(Float level);
void menu_tint_begin(Float alpha, Float red, Float green, Float blue);
void menu_tint_end(void);

/* a point's straight move from *from at from_time to *to at to_time */
struct menu_lerp {
    const NJS_POINT3 *from;
    Float from_time;
    const NJS_POINT3 *to;
    Float to_time;
    Float time;
    NJS_POINT3 *out;
};
/* set *out to the move's point at time. a time outside the two gives a
 * point past *from or *to. from_time and to_time must differ, unchecked */
void menu_lerp(const struct menu_lerp *lerp);

#endif
