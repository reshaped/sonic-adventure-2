#include "chao/animal_training.h"
#include "hud/item_stock.h"

/* the points each item adds to a Chao's skills, by the item's kind. the
 * kinds run ANIMAL_PEN to ANIMAL_PHEN, then the yellow, green, red and
 * purple Chaos Drives */
struct animal_training_points animal_training_rewards[ITEMS] = {
    {18, 6, 4, -12}, {20, -8, 0, 4},  {22, -2, 4, -8}, {0, 4, 20, -8},
    {-4, -4, 20, 4}, {-2, -6, 16, 8}, {4, -8, 2, 18},  {-4, -8, 10, 18},
    {-2, -4, 2, 20}, {6, 24, -8, -6}, {0, 24, -10, 2}, {10, 30, -12, 8},
    {4, 6, 6, 4},    {4, 6, 10, 10},  {10, 6, 2, 4},   {16, 0, 4, 12},
    {4, 4, 16, 8},   {4, 20, 4, 4},   {10, 2, 4, 16},  {8, 6, 18, 0},
    {6, 16, 2, 8},   {8, 0, 0, 0},    {0, 0, 8, 0},    {0, 0, 0, 8},
    {0, 8, 0, 0}};
