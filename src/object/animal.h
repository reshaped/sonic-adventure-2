/* the stages' small animals, which a player takes into the item stock. a
 * counter turns player 0's animals into lives */
#ifndef OBJECT_ANIMAL_H
#define OBJECT_ANIMAL_H
#include <sg_xpt.h>

struct task;

/* the kinds, each named for its file, MINIMAL_<name>.PRS. a taken animal's
 * item in the stock is its kind */
enum animal_kind {
    ANIMAL_PEN = 0, /* the penguin */
    ANIMAL_GOMA = 1, /* the seal */
    ANIMAL_RAKO = 2, /* the otter */
    ANIMAL_USA = 3, /* the rabbit */
    ANIMAL_CHIT = 4, /* the cheetah */
    ANIMAL_INO = 5, /* the boar */
    ANIMAL_KUMA = 6, /* the bear */
    ANIMAL_TORA = 7, /* the tiger */
    ANIMAL_GORI = 8, /* the gorilla */
    ANIMAL_KUJA = 9, /* the peacock */
    ANIMAL_OUM = 10, /* the parrot */
    ANIMAL_KON = 11, /* the condor */
    ANIMAL_SKA = 12, /* the skunk */
    ANIMAL_SHEP = 13, /* the sheep */
    ANIMAL_ARA = 14, /* the raccoon */
    ANIMAL_HAN = 15, /* the half fish */
    ANIMAL_BONE = 16, /* the skeleton dog */
    ANIMAL_BATMAN = 17, /* the bat */
    ANIMAL_DRA = 18, /* the dragon */
    ANIMAL_UNI = 19, /* the unicorn */
    ANIMAL_PHEN = 20, /* the phoenix */
    ANIMAL_KINDS = 21
};

/* each stage names five kinds, one to a slot */
#define STAGE_ANIMALS 5

/* the release kinds: the slots of the stage's five that animal_release
 * takes its animal from. a kind of two slots takes one at random. any other
 * value takes the fifth slot, as ANIMAL_FIFTH does */
#define ANIMAL_FIFTH 0
#define ANIMAL_THIRD_OR_FOURTH 1
#define ANIMAL_FIRST_OR_SECOND 2
#define ANIMAL_RELEASE_KINDS 3

/* the animal byte: the low byte of ang.z, where an object's placement chooses
 * the animal the object releases. its ANIMAL_KIND_BITS, modulo
 * ANIMAL_RELEASE_KINDS, give animal_release's kind. its ANIMAL_CHANCE_BITS
 * hold a count from 0 to 15, which each object turns into the odds of a
 * release by its own rule. a count of 0 releases none */
#define ANIMAL_KIND_BITS 0xf
#define ANIMAL_CHANCE_BITS 0xf0
#define ANIMAL_CHANCE_SHIFT 4

/* set the kept count to 0. a counter destroyed while game_state.cleared is 1
 * keeps its count. the next counter starts from the kept count */
void kept_animals_clear(void);
/* start player 0's counter, and load the stage's models into the slots that
 * have none. a two-player game and STAGE_GREEN_HILL start no counter. a
 * stage past STAGE_STG_END loads no models. each model, motion and texture
 * list takes a block of the Shinobi heap. the GCC game publishes a slot
 * once all three are ready. an unavailable slot skips its model draw and
 * can be loaded on a later call.
 *
 * the counter counts the animals player 0 takes, up to a limit. Sonic and
 * Shadow count to 20, Knuckles and Rouge to 10, and a mech to 15. any other
 * character's limit is 0. the animal that brings the count to its limit
 * gives a life. so does each animal after it. the count stays at the limit,
 * in red. the life comes as the count's number ends its shrink. an
 * animal taken during that shrink starts it over, and the two give one
 * life */
void animals_start(void);
/* free the slots' models, motions and texture lists, and empty the slots */
void animals_free(void);

/* release one of the stage's animals at (x, y, z). kind, a release kind,
 * picks its slot. it returns the animal's task, or null where the heap
 * can't give the task or its record. the release's sound plays at (x, y, z)
 * either way.
 *
 * the animal leaps along y, up under COLLISION_UP_Y and down under the other
 * up axes. as the leap turns back, it runs to the nearest player and falls to
 * the floor. a peacock, a parrot or a condor flies to a point above the
 * player instead. a player near enough takes the animal, but not in its first
 * 59 steps, while it blinks. a time stop holds the animal where it is, but a
 * player can still take it. the animal counts on the taker's counter */
struct task *animal_release(Float x, Float y, Float z, int kind);
/* animal_release, but the animal counts on no counter. for a kind of two
 * slots it takes one more number from rand than animal_release does */
struct task *animal_release_uncounted(Float x, Float y, Float z, int kind);
/* release an animal in a bubble at (x, y, z). the animal is of one of the
 * stage's first four slots, at random. it plays the sound and returns as
 * animal_release does, and the animal counts on no counter.
 *
 * the bubble sinks slowly, bobbing, and drifts round in a circle. after 600
 * steps the bubble is gone, and the animal runs as animal_release's does. no
 * player can take the animal in its first 10 steps */
struct task *animal_bubble_release(Float x, Float y, Float z);

/* the object list's step for MINIMAL, which runs once. it makes task one of
 * the stage's animals. where the heap can't give the animal's record, it
 * destroys task. the animal has no collision record.
 *
 * the work's ang.x holds three fields. bits 8 to 11 hold the mode, where 1
 * stands and 2 hides. any other value circles the work's pos, scl.x from it,
 * and turns by ang.z a step. a hidden animal draws nothing until a player
 * within scl.x takes the action it offers. the animal then hops out and runs
 * as animal_release's does.
 *
 * bits 4 to 7 hold a release kind. any other value there takes one of the
 * first four slots at random. bits 0 to 3 pick one of a kind's two slots. 0
 * picks the first, 1 the second, and any other value either at random */
void animal_init(struct task *task);

#endif
