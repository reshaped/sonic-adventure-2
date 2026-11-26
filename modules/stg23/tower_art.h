#ifndef STG23_TOWER_ART_H
#define STG23_TOWER_ART_H

/* clang-format off */
static struct debris_part fragments[6] = {
    {&tower_shards_texlist, &tower_fragment_0, {26.0583F, 19.145401F, 16.4736F}, {0, 0, 0}},
    {&tower_shards_texlist, &tower_fragment_1, {12.28035F, 22.739601F, -22.464F}, {0, 0, 0}},
    {&tower_shards_texlist, &tower_fragment_2, {-12.579899F, 10.9585F, -3.2947502F}, {0, 0, 0}},
    {&tower_shards_texlist, &tower_fragment_3, {25.75875F, -10.4073F, -25.75875F}, {0, 0, 0}},
    {&tower_shards_texlist, &tower_fragment_4, {-21.865051F, 1.5734997F, -3.5942998F}, {0, 0, 0}},
    {&tower_shards_texlist, &tower_fragment_5, {14.67645F, 7.3642006F, 15.275549F}, {0, 0, 0}},
};
/* clang-format on */
NJS_TEXLIST *base_tower_texlists[] = {&tower_support_texlist,
                                      &tower_shards_texlist,
                                      &tower_platform_texlist, NULL};
/* clang-format off */
static struct collision_shape shape = {
    0, SHAPE_BOX, PUSH_GROUPS, SHAPE_GUARD_LEVEL(3), SHAPE_UNK_2000,
    {0.0F, 22.5F, 0.0F}, 15.0F, 22.5F, 15.0F, 0, {0, 0, 0},
};
/* clang-format on */

#endif
