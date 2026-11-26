/* the race course's props, which the race's SET files place, and the art
 * they draw with */
#ifndef CHAOSTGRACE_PROPS_H
#define CHAOSTGRACE_PROPS_H
#include <ninja.h>
#include <sg_xpt.h>
struct task;

enum race_field {
    RACE_FIELD_CHAO = 129,
    RACE_FIELD_SURPRISE = 162,
    RACE_FIELD_BOX = 163,
    RACE_FIELD_FRUIT_DROP = 170, /* a shaken tree's, as its fruit drops */
    RACE_FIELD_ENTRANT = 175,
    RACE_FIELD_PENALTY = 195, /* a falling Omochao's, for a wrong answer */
    RACE_FIELD_RIGHT_ANSWER = 200 /* a lane's, for a right answer */
};

extern NJS_TEXLIST race_neutral_texlist;
extern NJS_TEXLIST race_hero_texlist;
extern NJS_TEXLIST race_dark_texlist;
extern NJS_CNK_OBJECT race_fruit_model[1];
extern NJS_CNK_OBJECT race_omo_model;
extern NJS_TEXLIST race_fruit_texlist;
enum { RACE_NEUTRAL_PROPS = 14, RACE_HERO_PROPS = 11, RACE_DARK_PROPS = 7 };
extern NJS_CNK_OBJECT *race_neutral_props[RACE_NEUTRAL_PROPS];
extern NJS_CNK_OBJECT *race_hero_props[RACE_HERO_PROPS];
extern NJS_CNK_OBJECT *race_dark_props[RACE_DARK_PROPS];
extern const Sint32 race_neutral_prop_count, race_hero_prop_count,
    race_dark_prop_count;
extern NJS_TEXANIM race_prop_glow_cells[2];

void race_fruit_bounce(struct task *task);
struct task *race_fruit_create(NJS_POINT3 *position, Angle yaw,
                               NJS_VECTOR *velocity);
int race_choose_pitfall(void);
void race_pitfalls_create(void);

void race_scenery_init(struct task *task);
void race_box_init(struct task *task);
void race_register_tree(struct task *tree);
void race_trees_reset(void);
#ifdef NONMATCHING
/* this side clears a dying fruit's link from its tree */
void race_forget_tree_fruit(struct task *fruit);
#endif
/* a tree chosen at random from those not yet claimed, which it claims. with
 * all eight claimed it never returns, so each racer calls it once at most */
struct task *race_claim_tree(void);
struct task *race_tree_fruit(struct task *chao);
void race_tree_init(struct task *tree);
void race_log_init(struct task *task);
void race_crab_init(struct task *task);
void race_omo_attach(NJS_CNK_OBJECT *root, Uint16 index, NJS_CNK_MODEL *model);
void race_omo_detach(NJS_CNK_OBJECT *root, Uint16 index);
void race_omochao_init(struct task *task);

#endif
