#include <ninja.h>
#include <sg_syrtc.h>
#include <sg_xpt.h>
#include <shinobi.h>
#include <stdlib.h>
#include <string.h>

#ifdef NONMATCHING
/* GARDEN_SEED_TYPES, by which this side's check of a garden bounds a tree's
 * kind */
#include "chao/orchard_art.h"
#endif
#include "chao/tree.h"
#include "chao/world_card.h"
#include "chao/garden.h"
#include "chao/garden_save.h"
#include "chao/identity.h"
#include "chao/race.h"
#include "chao/race_record.h"
#include "chao/race_state.h"
#include "chao/residents.h"
#include "chao/save_buffers.h"
#include "chao/work.h"
#include "chao/world.h"
#include "system/task.h"
#include "system/util.h"

#define RANDOM_ID_BYTE() ((Uint32)(Uint8)(rand() * (100.0F * RAND_SCALE)))
#define STARTING_TREES 2
#define TREE_PLACES 10

static struct garden_tree_save initial_tree = {
#include "chao/garden_save/initial_tree.inc"
};
static struct garden_tree_save empty_tree = {
#include "chao/garden_save/empty_tree.inc"
};
/* each garden's places for trees. garden_load_trees plants the first
 * GARDEN_TREES */
/* clang-format off */
static NJS_POINT3 tree_positions[CHAO_GARDENS][TREE_PLACES] = {
#include "chao/garden_save/tree_positions.inc"
};
/* clang-format on */

static void garden_make_origin(void)
{
    struct garden_origin *origin = &garden_saved_world.origin;
    Uint32 first;
    Uint32 second;
    Uint32 stamp;
    Sint32 result;

    {
        Sint8 id[SYD_CFG_IID_SIZE];

        result = syCfgGetIndividualID(id);
        if (result == SYD_CFG_IID_OK) {
            first = 0;
            second = 0;
            /* @bug arithmetic: id's bytes are signed, so a term that comes
             * out negative sets every bit above its own byte. ~id[4] is
             * negative for any byte from 0 to 127, and the OR at 0x0c50b3d8
             * keeps those bits */
            first |= (Uint32)~id[4];
            first |= (Uint32)(id[0] + 50) << 8;
            first |= (Uint32)(~id[3] + 17) << 16;
            first |= RANDOM_ID_BYTE() << 24;
            second |= (Uint32)(~id[1] + 163);
            second |= (Uint32)(~id[5] + 50) << 8;
            second |= (Uint32)(~id[2] + 17) << 16;
            second |= RANDOM_ID_BYTE() << 24;
            origin->console[0] = first;
            origin->console[1] = second;
        } else {
            first = 0;
            second = 0;
            first |= RANDOM_ID_BYTE();
            first |= RANDOM_ID_BYTE() << 8;
            first |= RANDOM_ID_BYTE() << 16;
            first |= RANDOM_ID_BYTE() << 24;
            second |= RANDOM_ID_BYTE();
            second |= RANDOM_ID_BYTE() << 8;
            second |= RANDOM_ID_BYTE() << 16;
            second |= RANDOM_ID_BYTE() << 24;
            origin->console[0] = first;
            origin->console[1] = second;
        }
    }
    {
        SYS_RTC_DATE date;

        result = syRtcGetDate(&date);
        if (result == SYD_RTC_ERR_OK) {
            stamp = 0;
            stamp |= date.second & 63;
            stamp |= (date.minute & 63U) << 6;
            stamp |= (date.hour & 31U) << 12;
            stamp |= (date.day & 31U) << 17;
            stamp |= (date.month & 15) << 22;
            stamp |= ((date.year - SYD_RTC_FIRSTYEAR) & 63U) << 26;
            origin->timestamp = stamp;
        } else {
            stamp = 0;
            stamp |= RANDOM_ID_BYTE();
            stamp |= RANDOM_ID_BYTE() << 8;
            stamp |= RANDOM_ID_BYTE() << 16;
            stamp |= RANDOM_ID_BYTE() << 24;
            origin->timestamp = stamp;
        }
    }
}

static void garden_seed_trees(void)
{
    int garden;
    int trees;
    struct garden_tree_save *tree;

    for (garden = 0; garden < CHAO_GARDENS; garden++) {
        tree = garden_saved_world.trees[garden];
        for (trees = STARTING_TREES; trees != 0; trees--) {
            *tree = initial_tree;
            tree->fruit[0] = (Uint32)(rand() * (49.9999F * RAND_SCALE));
            tree->fruit[1] = (Uint32)(rand() * (49.9999F * RAND_SCALE));
            tree->fruit[2] = (Uint32)(rand() * (49.9999F * RAND_SCALE));
            tree++;
        }
        for (trees = GARDEN_TREES - STARTING_TREES; trees != 0; trees--) {
            *tree = empty_tree;
            tree++;
        }
    }
}

static void garden_seed_races(void)
{
    struct garden_race_save *race = &garden_saved_world.race;
    int event;

    race->group_status[RACE_BEGINNER] = RACE_GROUP_OPEN;
    race->group_status[RACE_JEWEL] = RACE_GROUP_HIDDEN;
    race->group_status[RACE_CHALLENGE] = RACE_GROUP_HIDDEN;
    race->group_status[RACE_DARK_COURSES] = RACE_GROUP_HIDDEN;
    race->group_status[RACE_HERO_COURSES] = RACE_GROUP_HIDDEN;
    race->group_status[RACE_MULTI] = RACE_GROUP_OPEN;
    for (event = 0; event < RACE_EVENTS; event++) {
        race->unlocked[event] = -1;
    }
    for (event = 0; event < RACE_EVENTS; event++) {
        race->best[event] = -1;
    }
}

Bool garden_request_save(void) /* NOLINT(misc-use-internal-linkage) */
{
    garden_file_write();
    return TRUE;
}

#ifdef NONMATCHING
/* the last read failed with the card's file unknown, so garden_file_write
 * mustn't replace it */
static Bool garden_file_read_only;

Bool garden_file_stage(struct garden_card_request *request, const void *payload,
                       Uint32 size)
{
    struct garden_save world;
    struct garden_tree_save *tree;
    int garden, slot, lesson;

    if (size != sizeof(garden_file_image.garden) + sizeof(garden_saved_chao) ||
        request->size < 0 || size > (Uint32)request->size) {
        return FALSE;
    }
    memcpy(&world, payload, sizeof(world));
    if (world.chao_slots != CHAO_SAVE_SLOTS || world.classroom.count < 0 ||
        world.classroom.count > (int)sizeof(world.classroom.lesson)) {
        return FALSE;
    }
    for (slot = 0; slot < world.classroom.count; slot++) {
        /* a lesson is -1 for none, or a dance or an instrument from 0 to
         * 14. a song is 20 and a drawing 30, each plus its level, 0 to 4 */
        lesson = world.classroom.lesson[slot];
        if (lesson < -1 || (lesson > 14 && lesson < 20) ||
            (lesson > 24 && lesson < 30) || lesson > 34) {
            return FALSE;
        }
    }
    for (garden = 0; garden < CHAO_GARDENS; garden++) {
        for (slot = 0; slot < GARDEN_TREES; slot++) {
            tree = &world.trees[garden][slot];
            /* planting reads kind 0 as empty and TREE_KIND_PERMANENT as the
             * garden's own tree, whatever their stage. any other seed,
             * sapling or mature tree picks its model by kind and stage */
            if (tree->kind == 0 || tree->kind == TREE_KIND_PERMANENT ||
                tree->stage == TREE_STAGE_EMPTY ||
                tree->stage == TREE_STAGE_DEAD ||
                tree->stage == TREE_STAGE_PERMANENT) {
                continue;
            }
            if (tree->kind > GARDEN_SEED_TYPES ||
                tree->stage > TREE_STAGE_MATURE) {
                return FALSE;
            }
        }
    }
    memcpy(request->data, payload, size);
    memset((Uint8 *)request->data + size, 0, request->size - size);
    garden_file_read_only = FALSE;
    return TRUE;
}

Bool garden_file_recover(int state)
{
    garden_file_read_only = state != GARDEN_FILE_ABSENT;
    if (garden_file_read_only) {
        if (garden_saved_world.chao_slots != CHAO_SAVE_SLOTS) {
            garden_file_init();
        }
        return FALSE;
    }
    garden_file_init();
    return TRUE;
}
#endif

void garden_file_read(void)
{
    /* @hack redundant: the store of garden_card_read's result, which nothing
     * reads */
    int result; /* NOLINT(clang-diagnostic-unused-but-set-variable) */

    garden_card_request.data = &garden_file_image;
    garden_card_request.size = sizeof(garden_file_image);
    result = garden_card_read(&garden_card_request);
}

int garden_file_unpack(void)
{
#ifdef NONMATCHING
    /* this side returns 0, where the matching side returns the local the hack
     * tag names, which nothing sets */
    int result = 0;
#else
    /* @hack implicit: a result read from a local nothing sets, where a void
     * function would do, since the caller discards it */
    int result;
#endif

    njMemCopy(&garden_saved_world, garden_file_image.garden,
              sizeof(garden_saved_world));
    njMemCopy(garden_saved_chao, garden_file_image.chao,
              sizeof(garden_saved_chao));
    return result; /* NOLINT(clang-diagnostic-uninitialized) */
}

void garden_file_write(void)
{
    /* @hack spelling: the Chao's size in a local, where sizeof at its use
     * would do */
    int chao_size = sizeof(garden_saved_chao);
    /* @hack redundant: the store of garden_card_write's result, which
     * nothing reads */
    int result; /* NOLINT(clang-diagnostic-unused-but-set-variable) */

#ifdef NONMATCHING
    /* this side writes nothing to a card whose read failed, as
     * garden_file_recover says, and reports GARDEN_FILE_ERROR */
    if (garden_file_read_only) {
        garden_card_request.state = GARDEN_FILE_ERROR;
        return;
    }
#endif
    garden_file_gather();
    memset(garden_file_image.garden, 0, sizeof(garden_file_image.garden));
    memset(garden_file_image.chao, 0, sizeof(garden_file_image.chao));
    njMemCopy(garden_file_image.garden, &garden_saved_world,
              sizeof(garden_saved_world));
    njMemCopy(garden_file_image.chao, garden_saved_chao, chao_size);
    garden_card_request.data = &garden_file_image;
    garden_card_request.size =
        sizeof(garden_file_image.garden) + sizeof(garden_saved_chao);
    result = garden_card_write(&garden_card_request);
}

int garden_file_state(void)
{
    return garden_card_request.state;
}

void fun_0c50b818(void) /* NOLINT(misc-use-internal-linkage) */
{
}
void fun_0c50b81c(void) /* NOLINT(misc-use-internal-linkage) */
{
}
void fun_0c50b820(void) /* NOLINT(misc-use-internal-linkage) */
{
}

void garden_load_trees(void)
{
    int garden = chao_current_area();
    int slot;

    /* @bug logic: the range test at 0x0c50b83c never holds, since no area is
     * both below CHAO_AREA_NEUTRAL and above CHAO_AREA_DARK. an area outside
     * the three gardens indexes outside tree_positions and
     * garden_saved_world.trees */
    /* NOLINTBEGIN(clang-diagnostic-tautological-overlap-compare) */
    /* NOLINTNEXTLINE(misc-redundant-expression) */
    if (garden < CHAO_AREA_NEUTRAL && garden > CHAO_AREA_DARK) {
        return;
    }
    /* NOLINTEND(clang-diagnostic-tautological-overlap-compare) */
    garden--;
    for (slot = 0; slot < GARDEN_TREES; slot++) {
        garden_plant_tree(&tree_positions[garden][slot],
                          &garden_saved_world.trees[garden][slot]);
    }
}

static void garden_gather_chao(void)
{
    int count;
    struct task *object;
    struct chao_save *save;

    count = garden_object_count(GARDEN_OBJECT_CHAO);
    while (count) {
        count--;
        object = garden_active_object(GARDEN_OBJECT_CHAO, count);
        if (object != NULL) {
            save = ((struct chao_work *)object->work)->save;
            garden_pack_chao(object, save);
        }
    }
    count = garden_object_count(GARDEN_OBJECT_EGG);
    while (count) {
        count--;
        object = garden_active_object(GARDEN_OBJECT_EGG, count);
        if (object != NULL) {
            save = ((struct chao_work *)object->work)->save;
            garden_pack_egg(object, save);
        }
    }
}

static void garden_gather_trees(void)
{
    int garden = chao_current_area();
    int count;
    struct task *tree;

    /* @bug logic: the range test at 0x0c50b972 and 0x0c50b978 never holds, as
     * garden_load_trees's doesn't. outside the three gardens the gather packs
     * whatever trees are registered */
    /* NOLINTBEGIN(clang-diagnostic-tautological-overlap-compare) */
    /* NOLINTNEXTLINE(misc-redundant-expression) */
    if (garden < CHAO_AREA_NEUTRAL && garden > CHAO_AREA_DARK) {
        return;
    }
    /* NOLINTEND(clang-diagnostic-tautological-overlap-compare) */
    count = garden_object_count(GARDEN_OBJECT_TREE);
    while (count) {
        count--;
        tree = garden_active_object(GARDEN_OBJECT_TREE, count);
        if (tree != NULL) {
            garden_pack_tree(tree);
        }
    }
    count = garden_object_count(GARDEN_OBJECT_GARDEN_TREE);
    while (count) {
        count--;
        tree = garden_active_object(GARDEN_OBJECT_GARDEN_TREE, count);
        if (tree != NULL) {
            garden_pack_tree(tree);
        }
    }
}

void garden_gather_race(void)
{
    garden_saved_world.race = *garden_race_save();
}

void garden_file_gather(void)
{
    garden_gather_chao();
    garden_gather_trees();
    garden_gather_race();
}

void garden_file_init(void)
{
    memset(&garden_saved_world, 0, sizeof(garden_saved_world));
    garden_saved_world.chao_slots = CHAO_SAVE_SLOTS;
    garden_make_origin();
    garden_seed_trees();
    garden_seed_races();
    garden_seed_eggs();
}

struct garden_save *garden_save_record(void)
{
    return &garden_saved_world;
}

struct garden_tree_save *garden_file_trees(void)
{
    return garden_saved_world.trees[0];
}

const struct garden_race_save *garden_file_race_save(void)
{
    return &garden_saved_world.race;
}

struct garden_class *garden_file_class(void)
{
    return &garden_saved_world.classroom;
}

struct garden_origin *garden_save_origin(void)
{
    return &garden_saved_world.origin;
}

int garden_save_slots(void)
{
    return garden_saved_world.chao_slots;
}

Bool garden_flag_test(Uint32 mask)
{
    if (garden_saved_world.flags & mask) {
        return TRUE;
    }
    return FALSE;
}

void garden_flag_set(Uint32 mask)
{
    garden_saved_world.flags |= mask;
}

void garden_flag_clear(Uint32 mask)
{
    garden_saved_world.flags &= ~mask;
}

Bool garden_has_award(int bit)
{
    int mask = 1 << bit;
    if (garden_saved_world.awards & mask) {
        return TRUE;
    }
    return FALSE;
}

void garden_add_award(int bit)
{
    int mask = 1 << bit;
    garden_saved_world.awards |= mask;
}
