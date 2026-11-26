/* the models of the garden's fruit, seeds and trees. a fruit's or a seed's
 * object keeps its kind, its model's index, in its work's ang.x. a planted
 * tree of kind k blends between the shapes of garden_tree_models[k - 1]:
 * sapling, grown and dead. the three shapes' nodes match. a garden's own
 * tree is garden_tree_neutral, _hero or _dark, and the dark one has a crown */
#ifndef CHAO_ORCHARD_ART_H
#define CHAO_ORCHARD_ART_H
#include <ninja.h>

#define GARDEN_FRUIT_TYPES 22
#define GARDEN_SEED_TYPES 9
extern NJS_CNK_OBJECT *garden_fruit_models[GARDEN_FRUIT_TYPES];
extern NJS_CNK_OBJECT *garden_seed_models[GARDEN_SEED_TYPES];
extern NJS_CNK_OBJECT *garden_tree_models[GARDEN_SEED_TYPES][3];
extern NJS_CNK_OBJECT garden_tree_neutral, garden_tree_hero, garden_tree_dark;
extern NJS_CNK_OBJECT garden_tree_dark_crown;

#endif
