/* the path enemies, E PATH in a stage's object list. each flies along one
 * of the stage's paths and fires as it goes */
#ifndef SHARED_PATH_ENEMY_H
#define SHARED_PATH_ENEMY_H

struct task;

/* the kinds, named as in the stage's strings, such as
 * < ENEMY PATH GUNKUMI >. the gunkumi and the bombkumi are the kumis, which
 * share the textures E_PATHKUMITEX. the kyoko and the backkyoko are the
 * kyokos, which share E_PATHKYOKOTEX. on the path, each kind faces as its
 * comment says */
enum path_enemy_kind {
    /* faces the nearest player, and fires bullets in a firing action */
    PATH_ENEMY_GUNKUMI,
    /* faces the way it flies, never tilts with the path, and drops bombs */
    PATH_ENEMY_BOMBKUMI,
    /* faces the way it flies, fires bullets, and sends out two jets */
    PATH_ENEMY_KYOKO,
    PATH_ENEMY_BACKKYOKO, /* a kyoko that faces away from the way it flies */
    PATH_ENEMY_KINDS
};

/* the object list's step for E PATH. its first call sets the enemy up, and
 * each later call runs it. task needs a work and a placement. the heap must
 * have room for the enemy's enemy_work and enemy_route. where no task holds
 * the textures of the enemy's family yet, it needs room for one too. no call
 * tests for that room. the work holds the enemy's entry in the set file:
 *
 * - pos: the path's origin;
 * - scl: the wake point less pos. each part's fraction, taken without its
 *   sign, also gives a value of its own. the wake range is 10 plus 400 times
 *   scl.x's fraction. the enemy starts from point n of the path, n 100 times
 *   scl.y's fraction. 100 times scl.z's fraction is the steps the enemy adds
 *   to each wait between shots;
 * - ang.x: the kind in bits 0 to 3, where a kind past PATH_ENEMY_BACKKYOKO
 *   gives a gunkumi. bits 4 to 11 hold the PATH_ENEMY_ flags. bits 12 to 15
 *   are the index of the path, one of the stage's 12. an index of 12 or more
 *   gives path 0;
 * - ang.y: the path's turn about y, with only its high byte kept;
 * - ang.z: in its high byte, the point of the path where the enemy starts
 *   firing. in its low byte, the point where it stops.
 *
 * a point past the path's count stands for the path's start. the enemy
 * waits unseen until player 0 or 1 comes within the wake range of the wake
 * point. it then rides the path. at the path's last segment it flies on in a
 * straight line. under PATH_ENEMY_STOP it hovers there instead, facing the
 * nearest player. a hit breaks the enemy into its pieces and scores, and the
 * next call kills its placement. a later call, while the enemy isn't riding,
 * does nothing more where placement_try_leave returns TRUE */
void path_enemy_step(struct task *task);

/* the flags of a path enemy's work's tick, bits 4 to 11 of its ang.x */
#define PATH_ENEMY_STOP 0x1 /* hover at the path's last segment, not fly on */
#define PATH_ENEMY_SHADOW 0x2 /* draw a shadow under the enemy */
/* fire ENEMY_BULLET_PASSES bullets, which fly through the stage, in place of
 * ENEMY_BULLET_STOPS */
#define PATH_ENEMY_BULLETS_PASS 0x4
#define PATH_ENEMY_MIRROR 0x10 /* fly the path with its x negated */

#endif
