/* the shouko, E SHOUKO in a stage's object list: an enemy that flies a path of
 * its own. it is a gunner or a bomber. E SW SHOUKO, its switch, wakes it */
#ifndef SHARED_SHOUKO_H
#define SHARED_SHOUKO_H

struct task;

/* the kinds a shouko's ang.x picks. a bomber drops as many bombs as its
 * name's number, at the drops its comment names */
enum shouko_kind {
    SHOUKO_GUNNER, /* fires as it flies to the target, and as it hovers */
    SHOUKO_BOMBER_3, /* at the first, the second and the third drop */
    SHOUKO_BOMBER_2, /* at the first and the third drop */
    SHOUKO_BOMBER_1, /* at the second drop */
    SHOUKO_KINDS
};

/* the object list's step for E SHOUKO. its first call sets the shouko up, and
 * each later call runs it. task needs a work and a placement. the heap must
 * have room for the shouko's enemy_work, collision record and route. it needs
 * room for each bullet's and each bomb's enemy_work and collision record too.
 * where no task holds the shouko's textures yet, it needs room for one too.
 * no call tests for that room. the work holds the shouko's entry in the set
 * file:
 *
 * - pos: the path's origin;
 * - scl: the target;
 * - ang.x: the kind in bits 0 to 3, where a kind past SHOUKO_BOMBER_1 gives a
 *   gunner. bits 4 to 7 pick one of the two paths, where a value past 1
 *   gives path 0. bits 8 to 15 are the point of the path where a bomber's
 *   first drop lies. a point past the path's count stands for the path's
 *   start;
 * - ang.y: the group in its low byte, as enemy_work_alloc_grouped takes it.
 *   the group's low 4 bits are the number of the switch that wakes the
 *   shouko. the rest of ang.y turns the path about y;
 * - ang.z: the spread of a bomber's bombs. the first drop's bomb lands short
 *   of the target by 0.75 times ang.z. the third drop's lands as far past
 *   it, and the second drop's on it.
 *
 * the shouko waits unseen until enemy_switch_bits has its switch number's bit.
 * it then rides the path, until a hit or the path's last segment. from there
 * a gunner flies to the target and hovers. a bomber flies on in a straight
 * line. a gunner fires at the player nearest it, as player_nearest finds it.
 * that player's work and collision record must exist, unchecked. a bomber
 * drops only during the ride. its second and third drops lie 140 and 280 past
 * the first along the path. in a one-player game, player_work[0] must exist
 * whenever a bomb bursts, unchecked.
 *
 * a hit breaks the shouko into its pieces and scores. the next call ends the
 * shouko through placement_kill, unless it has bullets or bombs. the call
 * after that ends it either way. its bullets and bombs end with it.
 *
 * with no bullets or bombs out, a call outside the first and the ride asks
 * placement_try_leave(task) first. it does nothing more where that returns
 * TRUE */
void shouko_step(struct task *task);

/* the object list's step for E SW SHOUKO. it sets the switch up, and gives
 * task the switch's own step. that step, and this call, do nothing more where
 * placement_try_leave(task) returns TRUE. task needs a work and a placement.
 * the heap must have room for the switch's collision record, unchecked. the
 * work holds the switch's entry in the set file:
 *
 * - pos: the switch's center;
 * - scl: a box's half sizes, or in scl.x a sphere's radius;
 * - ang.x: a box where bit 0 is clear, and a sphere where it is set;
 * - ang.y: the switch's number in bits 0 to 3. less its low byte, ang.y turns
 *   a box about y;
 * - ang.z: less its low byte, a box's turn about z, as SHAPE_BOX2 takes it.
 *
 * the setup clears the number's bit of enemy_switch_bits. a player's touch
 * sets the bit, and ends the switch through placement_kill. no touch counts
 * while game_state.time_stopped is set */
void shouko_switch_init(struct task *task);

#endif
