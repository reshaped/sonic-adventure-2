/* the model objects, SARROW and TRBACK in City Escape's object list. each
 * draws the model of its kind, from a list of shared/model_kind.h */
#ifndef STG13_MODEL_OBJECTS_H
#define STG13_MODEL_OBJECTS_H
#include <ninja.h>
#include <sg_xpt.h>

struct task;

/* the TRBACK's texture list for texture_load_shared, ended by null */
extern NJS_TEXLIST *model_objects_texlists[];

/* the object list's steps for SARROW and TRBACK. unless
 * placement_try_leave(task) returns TRUE, each gives task its own step and
 * destructor, and its kind's draw. each step makes the same test.
 *
 * task needs a work, whose pos places the model and whose ang.y turns it. the
 * low byte of ang.x picks the kind, held to the list's last. each list holds
 * one kind, so the init sets ang.x to 0. a placed task's work takes its
 * placement's state as its submode, and the destructor gives the state back.
 *
 * a SARROW draws its model and three lights, lit in turn by game_state.steps.
 * it draws only after a step that found player 0 in a box about it. the box
 * reaches 60 along x, 30 along y and 130 along z each way from pos. its axes
 * are those ang.y turns. a TRBACK draws its model scaled by scl.y along x and
 * scl.z along y */
void sarrow_init(struct task *task);
void trback_init(struct task *task);

/* compile at buf the model of each kind with KIND_BUILD that isn't built
 * already, for njDirectDrawObject. a kind without KIND_TREE compiles its first
 * model alone. a build that would reach DEMO_AREA of system/areas.h isn't
 * made, and that kind draws its chunk model. returns buf past the builds it
 * made */
Uint8 *model_objects_build(Uint8 *buf);

extern NJS_CNK_OBJECT sarrow_object;
extern NJS_TEXLIST trback_texlist;
extern NJS_CNK_OBJECT trback_object;

#endif
