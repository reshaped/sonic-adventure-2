/* the garden's registry, and the garden's controller, which empties the
 * registry as it starts and as it ends */
#include <ninja.h>
#include <sg_xpt.h>
#include <stdlib.h>

#include "chao/tree.h"
#include "chao/body_collision.h"
#include "chao/garden.h"
#include "chao/garden_save.h"
#include "chao/garden_scene.h"
#include "chao/life.h"
#include "chao/movement.h"
#include "chao/residents.h"
#include "chao/work.h"
#include "chao/world.h"
#include "chao/world_slots.h"
#ifdef NONMATCHING
/* garden_prepare_pickup's declarations and mover_alloc, by which this side
 * makes a pickup */
#include "chao/pickup.h"
#include "object/mover.h"
#endif
#include "player/player.h"
#include "stage/stage.h"
#include "system/task.h"
#include "system/util.h"

#define GARDEN_ENTRY_HELD 1
#define GARDEN_ENTRY_LOCKED 2
#define NO_PAIR (-1)
#define NO_STATE (-1)
#define PAIR_IDS 256
/* no dig turns up headgear while the garden holds this many */
#define HEADGEAR_LIMIT 8
/* a dig turns up headgear with a chance of DIG_CHANCE / (count + 1)
 * squared. count is the headgear the garden holds */
#define DIG_CHANCE 0.1F

/* the debug page's columns and rows, and its colors */
#define NAME_COLUMN 1
#define COUNT_COLUMN 7
#define SLOT_COLUMN 10
#define FIRST_ROW 3
#define SAVE_ROW 14
#define SAVE_COLUMN 1
#define PAIR_COLORS 8
#define TEXT_WHITE 0xffffffff
#define TEXT_GREY 0xffcccccc
#define NEUTRAL_COLOR 0xff7fff7f
#define HERO_COLOR 0xff7f7fff
#define DARK_COLOR 0xffff7f7f

/* the controller's modes. it reads the area once, and in a garden then steps
 * the ages and the digs. every step advances the fruit counts */
enum { CONTROL_START, CONTROL_READ_AREA, CONTROL_GARDEN, CONTROL_ELSEWHERE };

#ifdef NONMATCHING
/* the non-matching build's table is flat. toy slots 16 to 31 reach the seed
 * row's entries there too, as in the image. the subscript stays within the
 * table's bound */
#define CATEGORY_ENTRIES(category)                                             \
    (&garden_entries[(category) * GARDEN_ROW_SLOTS])
/* the pickup that holds each entry from garden_prepare_pickup until the
 * pickup registers or is destroyed, or null */
static struct task
    *pickup_reservations[GARDEN_OBJECT_CATEGORIES * GARDEN_ROW_SLOTS];
#else
#define CATEGORY_ENTRIES(category) garden_entries[(category)]
#endif

static Uint16 next_pair_id = 0;
static Sint32 last_player_mode = -1;
static Bool gathered_on_pause = FALSE;

static void clear_entry(struct garden_entry *entry)
{
    if (entry != NULL) {
        entry->kind = 0;
        entry->flags = 0;
        entry->command = 0;
        entry->state = NO_STATE;
        entry->unk_34 = 0;
        entry->task = NULL;
        entry->partner = NULL;
        entry->pair_id = NO_PAIR;
    }
}

static void clear_entries(void)
{
    int category;
    int slot;
    for (category = 0; category < GARDEN_OBJECT_CATEGORIES; category++) {
        garden_object_counts[category] = 0;
        for (slot = 0; slot < GARDEN_ROW_SLOTS; slot++) {
            CATEGORY_ENTRIES(category)[slot].category = category;
            CATEGORY_ENTRIES(category)[slot].slot = slot;
            clear_entry(&CATEGORY_ENTRIES(category)[slot]);
#ifdef NONMATCHING
            /* this side drops each reservation with its entry */
            pickup_reservations[category * GARDEN_ROW_SLOTS + slot] = NULL;
#endif
        }
    }
}

/* @bug memory: toy slots 16 to 31 lie past the toys' row of
 * GARDEN_ROW_SLOTS, in the seed row's entries. the toys' capacity of 32 is at
 * 0x0c56693c, and garden_register_object (0x0c51e40e) fills slots up to it */
const Uint32 garden_object_capacities[GARDEN_OBJECT_CATEGORIES] = {
    16, 16, 10, 16, 8, 6, 32, 10, 16, 16};

struct task *garden_find_object(Uint16 category, Uint16 slot)
{
    if (slot > garden_object_capacities[category] - 1) {
        return NULL;
    }
    return CATEGORY_ENTRIES(category)[slot].task;
}

struct task *garden_active_object(Uint16 category, Uint16 index)
{
#ifdef NONMATCHING
    /* this side repairs the bug tagged in the matching side. it takes the entry
     * from garden_active_entry, which scans every slot */
    struct garden_entry *entry = garden_active_entry(category, index);
    return entry != NULL ? entry->task : NULL;
#else
    int slot;
    int active = 0;

    if (index > garden_object_capacities[category] - 1) {
        return NULL;
    }
    /* @bug logic: misses an object above a slot that emptied, since the scan
     * ends at the row's count (0x0c51df4a) */
    for (slot = 0; slot < garden_object_counts[category]; slot++) {
        if (CATEGORY_ENTRIES(category)[slot].task != NULL) {
            if (index == active) {
                return CATEGORY_ENTRIES(category)[slot].task;
            }
            active++;
        }
    }
    return NULL;
#endif
}

struct garden_entry *garden_find_entry(Uint16 category, Uint16 slot)
{
    struct garden_entry *entry;
    if (slot >= garden_object_capacities[category]) {
        return NULL;
    }
    entry = &CATEGORY_ENTRIES(category)[slot];
    if (entry->task != NULL) {
        return entry;
    }
    return NULL;
}

struct garden_entry *garden_active_entry(Uint16 category, Uint16 index)
{
    Uint16 count;
    int slot;
    int active = 0;

    if (index >= garden_object_capacities[category]) {
        return NULL;
    }
#ifdef NONMATCHING
    /* this side repairs the bug tagged in the matching side: the scan takes
     * every slot of the row */
    count = garden_object_capacities[category];
#else
    /* @bug logic: misses an object above a slot that emptied, since the scan
     * ends at the row's count (0x0c51e018) */
    count = garden_object_counts[category];
#endif
    for (slot = 0; slot < count; slot++) {
#ifdef NONMATCHING
        /* the seed row's entries hold toy slots 16 to 31 too */
        if (CATEGORY_ENTRIES(category)[slot].category != category) {
            continue;
        }
#endif
        if (CATEGORY_ENTRIES(category)[slot].task != NULL) {
            if (index == active) {
                return &CATEGORY_ENTRIES(category)[slot];
            }
            active++;
        }
    }
    return NULL;
}

Sint32 garden_object_count(Uint16 category)
{
    return garden_object_counts[category];
}

int garden_lock_entry(struct garden_entry *entry)
{
    if (entry != NULL) {
        entry->flags |= GARDEN_ENTRY_LOCKED;
        return TRUE;
    }
    return FALSE;
}

int garden_unlock_entry(struct garden_entry *entry)
{
    if (entry != NULL) {
        entry->flags &= ~GARDEN_ENTRY_LOCKED;
        return TRUE;
    }
    return FALSE;
}

int garden_object_locked(struct task *task)
{
    struct garden_entry *entry = task->force.p;
    if (entry && (entry->flags & GARDEN_ENTRY_LOCKED)) {
        return TRUE;
    }
    return FALSE;
}

int garden_set_carry_height(struct task *task, Float value)
{
    struct garden_entry *entry = task->force.p;
    if (entry != NULL) {
        entry->carry_height = value;
        return TRUE;
    }
    return FALSE;
}

int garden_set_carry_radius(struct task *task, Float value)
{
    struct garden_entry *entry = task->force.p;
    if (entry != NULL) {
        entry->carry_radius = value;
        return TRUE;
    }
    return FALSE;
}

int garden_set_target(struct task *task, struct task *target)
{
    struct garden_entry *entry = task->force.p;
    struct garden_entry *other = target->force.p;
    if (entry && other) {
        entry->target = other;
        return TRUE;
    }
    return FALSE;
}

int garden_object_targeted(struct task *object)
{
    int i;
    for (i = 0; i < CHAO_PER_GARDEN; i++) {
        if (CATEGORY_ENTRIES(GARDEN_OBJECT_CHAO)[i].task) {
            if (CATEGORY_ENTRIES(GARDEN_OBJECT_CHAO)[i].target) {
                if (CATEGORY_ENTRIES(GARDEN_OBJECT_CHAO)[i].target->task ==
                    object) {
                    return TRUE;
                }
            }
        }
    }
    return FALSE;
}

int garden_turn_to_target(struct task *task, int step)
{
    struct task *target = garden_target_object(task);
    return garden_turn_to_object(task, target, step);
}

struct task *garden_target_object(struct task *task)
{
    struct garden_entry *entry = task->force.p;
    if (entry && entry->target && entry->target->task) {
        return entry->target->task;
    }
    /* @bug uninitialized: returns no value without a target. R0 then holds
     * 64, target's offset in the entry, or for a task with no entry the
     * caller's R0 (0x0c51e174) */
#ifdef NONMATCHING
    /* this side repairs the bug tagged above: without a target, it returns
     * null */
    return NULL;
#endif
} /* NOLINT(clang-diagnostic-return-type) */

Float garden_target_distance(struct task *task)
{
    return garden_object_distance(task, garden_target_object(task));
}

struct garden_entry *garden_target_entry(struct task *task)
{
    struct garden_entry *entry = task->force.p;
    if (entry && entry->target && entry->target->task) {
        return entry->target;
    }
    /* @bug uninitialized: returns no value without a target. R0 then holds
     * 64, target's offset in the entry, or for a task with no entry the
     * caller's R0 (0x0c51e1a6) */
#ifdef NONMATCHING
    /* this side repairs the bug tagged above: without a target, it returns
     * null */
    return NULL;
#endif
} /* NOLINT(clang-diagnostic-return-type) */

void garden_hold_object(struct task *task)
{
    struct garden_entry *entry = task->force.p;
    if (entry != NULL) {
        entry->flags |= GARDEN_ENTRY_HELD;
    }
}

void garden_release_object(struct task *task)
{
    struct garden_entry *entry = task->force.p;
    if (entry != NULL) {
        entry->flags &= ~GARDEN_ENTRY_HELD;
    }
}

int garden_object_held(struct task *task)
{
    struct garden_entry *entry = task->force.p;
    if (entry && (entry->flags & GARDEN_ENTRY_HELD)) {
        return TRUE;
    }
    return FALSE;
}

int garden_set_partner(struct task *task, struct task *partner)
{
    struct garden_entry *entry = task->force.p;
    struct garden_entry *other = partner->force.p;
    if (entry && other) {
        entry->partner = other;
        entry->pair_id = next_pair_id;
        next_pair_id++;
        if (next_pair_id >= PAIR_IDS) {
            next_pair_id = 0;
        }
        return TRUE;
    }
    return FALSE;
}

void garden_clear_partner(struct task *task)
{
    struct garden_entry *entry = task->force.p;
    if (entry != NULL) {
        entry->flags = 0;
        entry->partner = NULL;
        entry->pair_id = NO_PAIR;
    }
}

struct garden_entry *garden_partner(struct task *task)
{
    struct garden_entry *entry = task->force.p;
    if (entry != NULL) {
        return entry->partner;
    }
    return NULL;
}

int garden_paired_elsewhere(struct task *task, struct task *other)
{
    struct garden_entry *entry = task->force.p;
    struct garden_entry *candidate = other->force.p;
    if (candidate && candidate->partner && candidate->partner != entry) {
        return TRUE;
    }
    return FALSE;
}

int garden_pair_objects(struct task *task, struct task *partner)
{
    struct garden_entry *entry = task->force.p;
    struct garden_entry *other = partner->force.p;
    if (!entry || !other) {
        return FALSE;
    }
    entry->partner = other;
    other->partner = entry;
    entry->pair_id = next_pair_id;
    other->pair_id = next_pair_id;
    next_pair_id++;
    if (next_pair_id >= PAIR_IDS) {
        next_pair_id = 0;
    }
    return TRUE;
}

int garden_separate_partners(struct task *task)
{
    struct garden_entry *entry = task->force.p;
    struct garden_entry *partner;

    if (entry != NULL) {
        partner = entry->partner;
        entry->flags = 0;
        entry->partner = NULL;
        entry->pair_id = NO_PAIR;
        if (partner != NULL) {
            if (partner->partner == entry) {
                partner->flags = 0;
                partner->partner = NULL;
            }
            partner->pair_id = NO_PAIR;
        }
    }
    return TRUE;
}

struct garden_entry *garden_mutual_partner(struct task *task)
{
    struct garden_entry *entry = task->force.p;
    struct garden_entry *partner;
    if (entry != NULL) {
        partner = entry->partner;
        if (partner && partner->partner == entry) {
            return partner;
        }
    }
    return NULL;
}

struct garden_entry *garden_partner_in_category(struct task *task,
                                                Uint16 category)
{
    struct garden_entry *entry = task->force.p;
    struct garden_entry *partner;
    if (entry != NULL) {
        partner = entry->partner;
        if (partner && partner->partner == entry &&
            partner->category == category) {
            return partner;
        }
    }
    return NULL;
}

Sint16 garden_take_command(struct task *task)
{
    struct garden_entry *entry = task->force.p;
    Sint16 command = 0;
    if (entry != NULL) {
        command = entry->command;
        entry->command = 0;
    }
    return command;
}

int garden_command_partner(struct task *task, Sint16 command)
{
    struct garden_entry *partner = garden_mutual_partner(task);
    if (partner != NULL) {
        partner->command = command;
        return TRUE;
    }
    return FALSE;
}

int garden_command_entry(struct garden_entry *entry, Sint16 command)
{
    if (entry != NULL) {
        entry->command = command;
        return TRUE;
    }
    return FALSE;
}

Sint16 garden_take_state(struct task *task)
{
    struct garden_entry *entry = task->force.p;
    Sint16 state = NO_STATE;
    if (entry != NULL) {
        state = entry->state;
        entry->state = NO_STATE;
    }
    return state;
}

int garden_send_partner_state(struct task *task, Sint16 command, Sint16 state)
{
    struct garden_entry *partner = garden_mutual_partner(task);
    if (partner != NULL) {
        partner->command = command;
        partner->state = state;
        return TRUE;
    }
    return FALSE;
}

int garden_send_entry_state(struct garden_entry *entry, Sint16 command,
                            Sint16 state)
{
    if (entry != NULL) {
        entry->command = command;
        entry->state = state;
        return TRUE;
    }
    return FALSE;
}

#ifdef NONMATCHING
static Bool slot_available(Uint16 category, Uint32 slot)
{
    return CATEGORY_ENTRIES(category)[slot].task == NULL &&
           pickup_reservations[category * GARDEN_ROW_SLOTS + slot] == NULL;
}

/* release pickup's reservation, unless clear_entries already has. extra
 * points into pickup_reservations, so it is cleared before task_destroy can
 * free it */
static void cancel_pickup(struct task *pickup)
{
    struct task **reservation = pickup->extra.p;
    if (*reservation == pickup) {
        *reservation = NULL;
    }
    pickup->extra.p = NULL;
}

struct task *garden_prepare_pickup(Uint16 category, int level,
                                   void (*init)(struct task *),
                                   const char *name,
                                   const struct collision_shape *shapes)
{
    Uint32 slot;
    struct task *pickup;
    struct task **reservation;

    for (slot = 0; slot < garden_object_capacities[category]; slot++) {
        if (slot_available(category, slot)) {
            break;
        }
    }
    if (slot == garden_object_capacities[category]) {
        return NULL;
    }
    pickup = task_create(TASK_WORK, level, init, name);
    if (pickup == NULL) {
        return NULL;
    }
    if (mover_alloc(pickup) == NULL ||
        !collision_init(pickup, shapes, PICKUP_SHAPES,
                        COLLISION_OBJECT_MUTUAL)) {
        task_destroy(pickup);
        return NULL;
    }
    reservation = &pickup_reservations[category * GARDEN_ROW_SLOTS + slot];
    *reservation = pickup;
    pickup->extra.p = reservation;
    pickup->destructor = cancel_pickup;
    return pickup;
}

Bool garden_take_pickup_reservation(struct task *pickup)
{
    struct task **reservation = pickup->extra.p;
    if (*reservation != pickup) {
        /* clear_entries emptied the registry after the pickup was prepared */
        return FALSE;
    }
    cancel_pickup(pickup);
    /* the pickup's init registers it before another task runs, so a slot is
     * still free for it */
    return TRUE;
}
#endif

int garden_register_object(Uint16 category, struct task *task, Uint16 kind)
{
    Uint32 slot = 0;
    do {
#ifdef NONMATCHING
        /* this side refuses too a slot that a pickup has reserved */
        if (slot_available(category, slot)) {
#else
        if (CATEGORY_ENTRIES(category)[slot].task == NULL) {
#endif
#ifdef NONMATCHING
            /* a seed row entry can hold a toy. it takes the category and slot
             * of whatever registers in it */
            CATEGORY_ENTRIES(category)[slot].category = category;
            CATEGORY_ENTRIES(category)[slot].slot = (Uint16)slot;
#endif
            CATEGORY_ENTRIES(category)[slot].task = task;
            CATEGORY_ENTRIES(category)[slot].kind = kind;
            garden_object_counts[category]++;
            task->force.p = &CATEGORY_ENTRIES(category)[slot];
            switch (category) {
            case GARDEN_OBJECT_CHAO:
            case GARDEN_OBJECT_EGG:
                task->work->tick = CATEGORY_ENTRIES(category)[slot].slot;
                break;
            }
            return TRUE;
        }
        slot++;
    } while (slot <= garden_object_capacities[category] - 1);
    return FALSE;
}

int garden_unregister_object(struct task *task)
{
    struct garden_entry *entry = task->force.p;
    garden_separate_partners(task);
    if (entry != NULL) {
        if (entry->task == task) {
            clear_entry(entry);
            garden_object_counts[entry->category]--;
        }
        task->force.p = NULL;
    }
    return TRUE;
}

int garden_object_category(struct task *task)
{
    struct garden_entry *entry = task->force.p;
    if (entry) {
        return entry->category;
    }
    return -1;
}

/* a debug page of the registry, which nothing calls. each category's row
 * shows its count and a mark for each slot. a mutual pair's mark is =, and a
 * one-way pair's is !, each in the pair's color. an object without a
 * partner shows -, and a free slot shows a period. the last row marks each
 * save in its garden's color. it shows a period for an empty save, E for an
 * egg and C for a Chao */
/* NOLINTNEXTLINE(misc-use-internal-linkage) */
void garden_debug_objects(void)
{
    int row = FIRST_ROW;
    const char *names[GARDEN_OBJECT_CATEGORIES] = {
        "CHAO",  "EGG", "MINIMAL", "FRUIT", "TREE",
        "GTREE", "TOY", "SEED",    "SOUND", "HEAD"};
    Uint32 colors[PAIR_COLORS] = {0xffff0000, 0xff00ff00, 0xff0000ff,
                                  0xffffff00, 0xffff00ff, 0xff00ffff,
                                  0xffff7f7f, 0xff7f7fff};
    struct task *object;
    struct chao_parameters *resident;
    Uint32 slot;
    int i;
    int location;
    Uint32 color;
    /* @hack spelling: mark holds the letter, where passing "E" and "C"
     * straight to njPrintC would do */
    const char *mark;
    struct task_work
        *work; /* NOLINT(clang-diagnostic-unused-but-set-variable) */

    /* @hack spelling: i = i + 1, where i++ would do */
    for (i = 0; i < GARDEN_OBJECT_CATEGORIES; row++, i = i + 1) {
        njPrintC(NJM_LOCATION(NAME_COLUMN, row), names[i]);
        njPrintD(NJM_LOCATION(COUNT_COLUMN, row), garden_object_count(i), 2);
        for (slot = 0; slot < garden_object_capacities[i]; slot++) {
            object = garden_find_object(i, slot);
            location = NJM_LOCATION(slot + SLOT_COLUMN, row);
            if (object != NULL) {
                if (garden_mutual_partner(object)) {
                    if (((struct garden_entry *)object->force.p)->pair_id >=
                        0) {
                        color = colors[((struct garden_entry *)object->force.p)
                                           ->pair_id %
                                       PAIR_COLORS];
                    } else {
                        color = TEXT_WHITE;
                    }
                    njPrintColor(color);
                    njPrintC(location, "=");
                } else if (garden_partner(object)) {
                    if (((struct garden_entry *)object->force.p)->pair_id >=
                        0) {
                        color = colors[((struct garden_entry *)object->force.p)
                                           ->pair_id %
                                       PAIR_COLORS];
                    } else {
                        color = TEXT_WHITE;
                    }
                    njPrintColor(color);
                    njPrintC(location, "!");
                } else {
                    njPrintColor(TEXT_WHITE);
                    njPrintC(location, "-");
                }
            } else {
                njPrintColor(TEXT_GREY);
                njPrintC(location, ".");
            }
            njPrintColor(TEXT_WHITE);
        }
    }
    for (i = 0; i < CHAO_SAVE_SLOTS; i++) {
        resident = garden_saved_chao[i].record.head;
        switch (resident->garden) {
        case CHAO_AREA_NEUTRAL:
            njPrintColor(NEUTRAL_COLOR);
            break;
        case CHAO_AREA_HERO:
            njPrintColor(HERO_COLOR);
            break;
        case CHAO_AREA_DARK:
            njPrintColor(DARK_COLOR);
            break;
        default:
            njPrintColor(TEXT_WHITE);
            break;
        }
        switch (resident->type) {
        case CHAO_NONE:
            njPrintC(NJM_LOCATION(i + SAVE_COLUMN, SAVE_ROW), ".");
            /* @hack redundant: the store to work, which nothing reads. the
             * save's column shifted left 16 is never 0, so it never reads
             * garden_controller */
            work = (((i + SAVE_COLUMN) << 16) != 0) ? NULL
                                                    : garden_controller->work;
            break;
        case CHAO_EGG:
            mark = "E";
            njPrintC(NJM_LOCATION(i + SAVE_COLUMN, SAVE_ROW), mark);
            break;
        default:
            mark = "C";
            njPrintC(NJM_LOCATION(i + SAVE_COLUMN, SAVE_ROW), mark);
            break;
        }
    }
    njPrintColor(TEXT_WHITE);
}

static void garden_control_step(struct task *task)
{
    struct task_work *work = task->work;
    NJS_POINT3 position;
    NJS_VECTOR velocity;
    Float rarity;
    int area;
    int count;
    int kind;

    switch (work->mode) {
    case CONTROL_START:
        work->mode++;
        last_player_mode = -1;
        break;
    case CONTROL_READ_AREA:
        area = chao_current_area();
        if (area >= CHAO_AREA_NEUTRAL && area <= CHAO_AREA_DARK) {
            work->mode = CONTROL_GARDEN;
        } else {
            work->mode = CONTROL_ELSEWHERE;
        }
        break;
    case CONTROL_GARDEN:
        garden_age_step();
        switch (player_character(0)) {
        case PLAYER_KNUCKLES:
        case PLAYER_ROUGE:
            count = garden_object_count(GARDEN_OBJECT_HEADGEAR);
            if (count < HEADGEAR_LIMIT) {
                if (last_player_mode == PLAYER_MODE_HUNTER_DIG &&
                    player_work[0]->mode == PLAYER_MODE_HUNTER_DIG_OUT) {
                    rarity = count + 1;
                    rarity = rarity * rarity;
                    if (RANDOM_UNIT() < DIG_CHANCE / rarity) {
                        if (chao_current_area() == CHAO_AREA_DARK) {
                            kind = GARDEN_HEADGEAR_SKULL;
                        } else {
                            kind = GARDEN_HEADGEAR_PUMPKIN;
                        }
                        position.x = player_work[0]->pos.x;
                        position.y = player_work[0]->pos.y + 2.0F;
                        position.z = player_work[0]->pos.z;
                        velocity.x = 0.0F;
                        velocity.y = 1.5F;
                        velocity.z = 0.0F;
                        garden_drop_headgear(kind, &position, RANDOM_TURN(),
                                             &velocity);
                    }
                }
                last_player_mode = player_work[0]->mode;
            }
            break;
        }
        break;
    }
    garden_advance_fruit_counts();
}

static void garden_control_draw(struct task *task)
{
    njGetMatrix(&garden_view);
    njGetMatrix(&garden_view_inverse);
    njInvertMatrix(&garden_view_inverse);
    /* a paused stage draws its tasks without stepping them. the controller
     * gathers the save here, once each pause */
    if (stage_get_mode() == STAGE_PAUSE) {
        if (!gathered_on_pause) {
            garden_file_gather();
            gathered_on_pause = TRUE;
        }
    } else {
        gathered_on_pause = FALSE;
    }
}

static void garden_control_destroy(struct task *task)
{
    garden_controller = NULL;
    clear_entries();
}

int garden_control_start(void)
{
    if (garden_controller != NULL) {
        task_destroy(garden_controller);
        garden_controller = NULL;
    }
    garden_controller =
        task_create(TASK_WORK, 1, garden_control_step, "ALW_Control");
    garden_controller->draw = garden_control_draw;
    garden_controller->destructor = garden_control_destroy;
    clear_entries();
    garden_age_reset();
    garden_enable_objects();
    return TRUE;
}

int fun_0c51e994(void) /* NOLINT(misc-use-internal-linkage) */
{
    return 0;
}
