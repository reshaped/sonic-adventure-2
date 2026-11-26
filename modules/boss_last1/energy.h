/* the Biolizard's volleys of energy. a volley charges shots and releases
 * them to orbit the arena. the first orbits low, and each after at a random
 * level. a shot fades once it has passed player 0, or nears the volley's
 * goal */
#ifndef BOSS_LAST1_ENERGY_H
#define BOSS_LAST1_ENERGY_H
#include <ninja.h>
#include <sg_xpt.h>

struct task;
struct task_work;

/* the kinds of volley, by their shots' speeds */
enum biolizard_energy_kind {
    BIOLIZARD_ENERGY_SLOW,
    BIOLIZARD_ENERGY_NORMAL,
    BIOLIZARD_ENERGY_FAST,
    BIOLIZARD_ENERGY_KINDS
};

/* a setup's count for a volley with no limit */
#define BIOLIZARD_ENERGY_NO_LIMIT 0
/* a setup's timing, whose sign alone a volley reads. BIOLIZARD_ENERGY_BY_CALL
 * leaves each shot's charge to biolizard_energy_charge. a timing of 0 or more
 * charges a shot each interval of the volley's kind */
#define BIOLIZARD_ENERGY_BY_CALL (-1)

/* a volley's order. where animation is set, a shot charges each time
 * animation's pos.z rises past trigger_frame, whatever timing holds. pos.z
 * holds a motion's frame */
struct biolizard_energy_setup {
    Sint8 count; /* the shots to charge, or BIOLIZARD_ENERGY_NO_LIMIT */
    Sint8 timing;
    Sint8 kind; /* a BIOLIZARD_ENERGY_ kind */
    /* the point the shots charge 10 above. each charging shot reads it every
     * step */
    const NJS_POINT3 *from;
    /* read once: a shot fades as it nears this point's bearing from the
     * arena's center */
    const NJS_POINT3 *goal;
    Float speed; /* kept, and never read: each shot takes its kind's speed */
    const struct task_work *animation;
    Float trigger_frame;
    Float end_frame; /* kept, and never read */
};

/* create a volley under parent. *interval gets its kind's steps between
 * shots. returns the volley's task, or null for a null parent or where the
 * heap can't give the volley. a null return leaves *interval unset.
 *
 * a volley with a count, unless left to biolizard_energy_charge, dies once
 * its shots have charged and died. any other lives until killed. each
 * volley_task below is a volley's task, or null for none */
struct task *biolizard_energy_create(struct task *parent,
                                     const struct biolizard_energy_setup *setup,
                                     Uint16 *interval);
/* charge a shot, and count it */
void biolizard_energy_charge(struct task *volley_task);
/* release every shot that is charging */
void biolizard_energy_release(struct task *volley_task);
/* biolizard_energy_remaining's result for a volley with no limit */
#define BIOLIZARD_ENERGY_UNLIMITED 255
/* the shots left to charge, BIOLIZARD_ENERGY_UNLIMITED, or -1 for none */
int biolizard_energy_remaining(struct task *volley_task);
/* 1 while the volley has a shot, else 0, or -1 for none */
Sint8 biolizard_energy_has_shot(struct task *volley_task);
/* print and step the energy test's menu: a kind, and its interval and
 * speeds. player 0's Y picks a row, and L and R change it */
void biolizard_debug_energy(void);
/* the kind the energy test's menu shows */
Sint8 biolizard_debug_energy_kind(void);
/* create a shot under parent that stands still for the energy display test.
 * a menu on the fourth pad sets how the shots' parts turn. returns its task.
 * parent must not be null: for a null parent the result is undefined. the
 * heap must give the task, unchecked */
struct task *biolizard_energy_preview(struct task *parent);
#endif
