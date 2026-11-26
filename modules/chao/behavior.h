/* the queue of behaviors a garden Chao runs, one step a frame */
#ifndef CHAO_BEHAVIOR_H
#define CHAO_BEHAVIOR_H
#include <ninja.h>
#include <sg_xpt.h>

struct task;

/* a behavior: the Chao's step for one frame. it returns a
 * chao_behavior_result */
typedef int (*chao_behavior_fn)(struct task *chao);

enum chao_behavior_result {
    BEHAVIOR_CONTINUE, /* run it again the next frame */
    BEHAVIOR_FINISH, /* run the queue's next behavior, or idle at its end */
    BEHAVIOR_CHOOSE, /* run the Chao's intention, choosing one if it has none */
    BEHAVIOR_BREAK /* drop the rest of the queue and idle */
};

/* the queue's slots. it holds one behavior fewer, so a null slot ends it */
#define CHAO_BEHAVIORS 16
/* the intentions a Chao can hold, each with its cooldown */
#define CHAO_INTENTIONS 18
/* the frames of a behavior with no time limit */
#define BEHAVIOR_UNTIMED (-1)
/* the frames a behavior's limit starts at */
#define BEHAVIOR_LIMIT 3600

/* a Chao's queue of behaviors, and the state they keep */
struct chao_behavior {
    Uint16 flags; /* bits the behaviors keep, which a set leaves as they are */
    /* the running behavior's own state, with timer and count below. a set,
     * an advance or a resume makes each 0 */
    Uint16 mode, phase;
    Uint8 unk_06[2];
    /* TRUE from chao_interrupt_behavior to chao_resume_behavior */
    Bool interrupted;
    Sint32 timer, count;
    /* frames a behavior may count down for itself. a set, an advance or a
     * resume makes it BEHAVIOR_LIMIT */
    Sint32 limit;
    Sint16 intention, intention_age;
    Uint8 unk_1c[4];
    Uint32 cooldown[CHAO_INTENTIONS];
    Uint8 unk_68[512];
    /* chao_set_behavior_value's, which a set or a resume clears */
    Sint32 value;
    Float range;
    NJS_POINT3 base;
    /* the behavior queue[current] held before the last set, advance or
     * resume */
    chao_behavior_fn previous;
    /* queue[current] is the running behavior, and queued counts the
     * behaviors from it on */
    Sint32 queued, current;
    chao_behavior_fn queue[CHAO_BEHAVIORS];
    /* each slot's frames left. only a positive count runs down */
    Sint32 frames[CHAO_BEHAVIORS];
};

/* run the next behavior in the queue. FALSE where none is left, which
 * leaves no behavior running */
Bool chao_advance_behavior(struct task *chao);
/* empty the queue and run action alone, until it finishes. the set tells the
 * Chao's partner to cancel. it undoes what the replaced behavior left on the
 * Chao. the props fade, the field and book go, and the mark chao_show_mark set
 * clears. the eyes and mouth return to their defaults, unless action is the
 * behavior it replaces. the Chao collides, moves and pushes again, and yields
 * to pushes */
void chao_set_behavior(struct task *chao, chao_behavior_fn action);
/* chao_set_behavior, where action finishes after frames frames at most. under
 * BEHAVIOR_UNTIMED it finishes only by its own result */
void chao_set_timed_behavior(struct task *chao, chao_behavior_fn action,
                             int frames);
void chao_set_behavior_with_value(struct task *chao, chao_behavior_fn action,
                                  int value);
/* append action to the queue, to run once the behaviors before it finish. a
 * full queue drops it. call it only between a set and the next advance. past an
 * advance it writes over a behavior still queued, or the running one */
void chao_queue_behavior(struct task *chao, chao_behavior_fn action);
/* chao_queue_behavior, where action finishes after frames frames at most */
void chao_queue_timed_behavior(struct task *chao, chao_behavior_fn action,
                               int frames);
chao_behavior_fn chao_current_behavior(struct task *chao);
/* set action, as chao_set_behavior does, part the Chao from its partners and
 * set interrupted */
void chao_interrupt_behavior(struct task *chao, chao_behavior_fn action);
/* run previous again in the running behavior's slot, from its start, and keep
 * the running one as previous. it cancels and resets as a set does, but keeps
 * the queue */
void chao_resume_behavior(struct task *chao);
void chao_set_behavior_value(struct task *chao, int value);
int chao_behavior_value(struct task *chao);
/* start the Chao idle, with every intention's cooldown at 0 */
void chao_behavior_init(struct task *chao);
/* run the Chao's behavior for this frame, while CHAO_RUNS_BEHAVIOR is set.
 * the chao_try_ calls that may interrupt it run first, unless the Chao
 * evolves or dies. the last that applies sets its behavior, and any that
 * applies parts the Chao from its partners. a timed behavior finishes once its
 * frames run out, whatever it returns. under CHAO_NO_INTENTIONS, no frames or
 * cooldowns count down, and BEHAVIOR_CHOOSE acts as BEHAVIOR_CONTINUE */
void chao_behavior_step(struct task *chao);
/* stand without colliding for 61 frames, then idle */
int chao_wait_behavior(struct task *chao);
/* stand without colliding until another behavior is set */
int chao_wait_held_behavior(struct task *chao);

/* the behavior's part of a Chao's destructor. chao_actor_destroy and
 * racer_destroy call it after chao_free_body. it does nothing, but on the GCC
 * side it lets go of the Chao's book by chao_release_book */
void chao_behavior_destroy(struct task *chao);
void fun_0c501218(struct task *chao);

void fun_0c501440(void);
void fun_0c501444(void);
#endif
