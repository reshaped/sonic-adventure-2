/* the garden's registry: a row of slots for each category of object. a
 * registered task keeps its entry in force. through the entry it has a
 * target, a partner, and the commands and states other tasks send it */
#ifndef CHAO_WORLD_H
#define CHAO_WORLD_H
#include <ninja.h>
#include <sg_xpt.h>

#include "system/angle.h"

struct task;

enum garden_object_category {
    GARDEN_OBJECT_CHAO = 0,
    GARDEN_OBJECT_EGG = 1,
    GARDEN_OBJECT_MINIMAL = 2,
    GARDEN_OBJECT_FRUIT = 3,
    GARDEN_OBJECT_TREE = 4,
    GARDEN_OBJECT_GARDEN_TREE = 5,
    GARDEN_OBJECT_TOY = 6,
    GARDEN_OBJECT_SEED = 7,
    GARDEN_OBJECT_SOUND = 8,
    GARDEN_OBJECT_HEADGEAR = 9,
    GARDEN_OBJECT_CATEGORIES = 10
};

/* a registered object's entry. pos and ang are where a Chao carrying the
 * object holds it. carry_radius sets how far in front of the Chao's hands
 * the object sits. carry_height sets how far below them. distance is the
 * camera's from the object, as garden_visible_box last measured it */
struct garden_entry {
    Uint16 category, slot, kind, flags;
    Sint32 pair_id; /* the id an entry shares with its partner, or -1 */
    NJS_POINT3 pos;
    struct angles ang;
    Float carry_radius, carry_height, distance;
    Sint16 command, state, unk_34;
    Uint8 unk_36[2];
    struct task *task; /* null in a free slot */
    struct garden_entry *partner, *target;
};

/* the task or the entry in category's slot. a free slot gives null, and so
 * does a slot past the category's capacity */
struct task *garden_find_object(Uint16 category, Uint16 slot);
struct garden_entry *garden_find_entry(Uint16 category, Uint16 slot);
/* the index-th registered object of category, counting in slot order, or
 * null. the image's scan stops at slot garden_object_count(category), so it
 * misses an object above a slot that emptied. the non-matching build scans
 * every slot */
struct task *garden_active_object(Uint16 category, Uint16 index);
struct garden_entry *garden_active_entry(Uint16 category, Uint16 index);
Sint32 garden_object_count(Uint16 category);
/* the category of task's entry, or -1 for a task with none */
int garden_object_category(struct task *task);
/* give task the first free slot of category, and kind. a Chao's or an egg's
 * work->tick becomes its slot. returns FALSE where no slot is free */
int garden_register_object(Uint16 category, struct task *task, Uint16 kind);
/* separate task from its partner, free its slot and clear force, and return
 * TRUE. task_destroy frees a force left set, so a registered task's
 * destructor must call this */
int garden_unregister_object(struct task *task);

/* the id of a shape that shades an object touching it */
#define GARDEN_SHADE_HIT 150

/* a Chao carrying an object locks the object's entry each step it sets pos.
 * garden_follow_chao unlocks it as the object follows */
int garden_lock_entry(struct garden_entry *entry);
int garden_unlock_entry(struct garden_entry *entry);
int garden_object_locked(struct task *task);
int garden_set_carry_height(struct task *task, Float value);
int garden_set_carry_radius(struct task *task, Float value);
/* set, clear and test a held flag of task's entry, apart from the lock.
 * nothing calls them */
void garden_hold_object(struct task *task);
void garden_release_object(struct task *task);
int garden_object_held(struct task *task);

/* a target is one-way: the target's entry doesn't name the entry that targets
 * it. unregistering an object clears no target that names it. a target
 * whose task is null counts as none. without a target,
 * garden_target_object and garden_target_entry return null in the
 * non-matching build. the original returns no value */
int garden_set_target(struct task *task, struct task *target);
/* whether a Chao in one of the first 8 Chao slots targets object */
int garden_object_targeted(struct task *object);
int garden_turn_to_target(struct task *task, int step);
struct task *garden_target_object(struct task *task);
Float garden_target_distance(struct task *task);
struct garden_entry *garden_target_entry(struct task *task);

/* two entries are mutual partners while each names the other.
 * garden_set_partner names partner in task's entry alone, and
 * garden_pair_objects names each in the other's. each call takes a new pair
 * id. clearing or separating a partner clears the entry's flags too */
int garden_set_partner(struct task *task, struct task *partner);
void garden_clear_partner(struct task *task);
struct garden_entry *garden_partner(struct task *task);
/* whether other's partner is an entry other than task's */
int garden_paired_elsewhere(struct task *task, struct task *other);
int garden_pair_objects(struct task *task, struct task *partner);
int garden_separate_partners(struct task *task);
struct garden_entry *garden_mutual_partner(struct task *task);
struct garden_entry *garden_partner_in_category(struct task *task,
                                                Uint16 category);

/* an entry holds one command, 0 for none, and one state, -1 for none. a take
 * returns it and leaves none, and returns none for a task with no entry.
 * the partner functions send only to a mutual partner */
Sint16 garden_take_command(struct task *task);
int garden_command_partner(struct task *task, Sint16 command);
int garden_command_entry(struct garden_entry *entry, Sint16 command);
Sint16 garden_take_state(struct task *task);
int garden_send_partner_state(struct task *task, Sint16 command, Sint16 state);
int garden_send_entry_state(struct garden_entry *entry, Sint16 command,
                            Sint16 state);
/* the commands a Chao and its partner send each other. a Chao sends the
 * object it uses PARTNER_USE on each step. a Chao whose behavior is replaced
 * sends its partner PARTNER_CANCEL. the object sends PARTNER_USED_UP once it
 * is used up, and PARTNER_STOLEN as the player takes it */
enum garden_partner_command {
    PARTNER_USE = 2,
    PARTNER_CANCEL = 3,
    PARTNER_USED_UP = 4,
    PARTNER_STOLEN = 6
};
/* each category's slots. the toys' 32 run past their row of 16 into the
 * seeds' row */
extern const Uint32 garden_object_capacities[];

#endif
