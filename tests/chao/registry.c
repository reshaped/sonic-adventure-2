/* Exercise the live task registry and its Chao and egg save consumers. */
#include <assert.h>

#include "chao/garden.h"
#include "chao/garden_save.h"
#include "chao/residents.h"
#include "chao/tree.h"
#include "chao/work.h"
#include "chao/world.h"
#include "chao/world_slots.h"
#include "system/task.h"

int chao_current_area(void)
{
    return CHAO_AREA_NEUTRAL;
}

void garden_pack_tree(struct task *tree)
{
    assert(0); /* This fixture registers no trees. */
}

static void check_residents(Uint16 category)
{
    struct task actors[7] = {{0}};
    struct chao_work work[7] = {{0}};
    struct chao_save saves[7] = {{0}};
    const int survivors[] = {1, 3, 4};
    struct garden_entry *partner;
    int i;

    for (i = 0; i < 7; i++) {
        actors[i].work = &work[i].base;
        work[i].save = &saves[i];
        work[i].profile.parameters.affection = 100 + i;
    }
    for (i = 0; i < 6; i++) {
        assert(garden_register_object(category, &actors[i], 0));
    }
    partner = actors[4].force.p;
    assert(garden_pair_objects(&actors[1], &actors[4]));
    assert(garden_set_target(&actors[1], &actors[4]));

    assert(garden_unregister_object(&actors[0]));
    assert(garden_unregister_object(&actors[2]));
    assert(garden_unregister_object(&actors[5]));
    assert(garden_object_count(category) == 3);
    garden_file_gather();
    for (i = 0; i < 3; i++) {
        int n = survivors[i];
        assert(garden_active_object(category, i) == &actors[n]);
        assert(garden_active_entry(category, i) == actors[n].force.p);
        assert(saves[n].record.saved.parameters.affection == 100 + n);
        work[n].profile.parameters.affection += 100;
    }
    assert(!garden_active_entry(category, 3));
    assert(!garden_active_object(category, 3));
    assert(!saves[0].record.saved.parameters.affection);
    assert(!saves[2].record.saved.parameters.affection);
    assert(!saves[5].record.saved.parameters.affection);

    assert(garden_register_object(category, &actors[6], 0));
    assert(garden_find_entry(category, 0) == actors[6].force.p);
    assert(garden_active_object(category, 0) == &actors[6]);
    assert(garden_mutual_partner(&actors[1]) == partner);
    assert(garden_target_entry(&actors[1]) == partner);
    garden_file_gather();
    assert(saves[6].record.saved.parameters.affection == 106);
    for (i = 0; i < 3; i++) {
        int n = survivors[i];
        assert(garden_active_object(category, i + 1) == &actors[n]);
        assert(saves[n].record.saved.parameters.affection == 200 + n);
    }
    for (i = 0; i < 3; i++) {
        assert(garden_unregister_object(&actors[survivors[i]]));
    }
    assert(garden_unregister_object(&actors[6]));
    assert(!garden_object_count(category));
}

static void check_shared_slots(void)
{
    struct task toys[17] = {{0}};
    struct task seeds[3] = {{0}};
    struct garden_entry *borrowed;
    int i;

    assert(garden_register_object(GARDEN_OBJECT_SEED, &seeds[0], 0));
    for (i = 0; i < 17; i++) {
        assert(garden_register_object(GARDEN_OBJECT_TOY, &toys[i], 0));
    }
    borrowed = toys[16].force.p;
    assert(borrowed->category == GARDEN_OBJECT_TOY && borrowed->slot == 17);
    assert(garden_find_entry(GARDEN_OBJECT_SEED, 1) == borrowed);
    assert(garden_find_entry(GARDEN_OBJECT_TOY, 17) == borrowed);
    assert(garden_register_object(GARDEN_OBJECT_SEED, &seeds[1], 0));
    assert(garden_active_entry(GARDEN_OBJECT_TOY, 16) == borrowed);
    assert(garden_active_object(GARDEN_OBJECT_SEED, 1) == &seeds[1]);
    assert(!garden_active_entry(GARDEN_OBJECT_SEED, 2));
    assert(!garden_active_entry(GARDEN_OBJECT_TOY, 17));

    assert(garden_unregister_object(&toys[16]));
    assert(garden_object_count(GARDEN_OBJECT_SEED) == 2);
    assert(garden_object_count(GARDEN_OBJECT_TOY) == 16);
    assert(garden_register_object(GARDEN_OBJECT_SEED, &seeds[2], 0));
    assert(seeds[2].force.p == borrowed && borrowed->slot == 1);
    assert(garden_active_entry(GARDEN_OBJECT_SEED, 1) == borrowed);
    assert(!garden_active_entry(GARDEN_OBJECT_TOY, 16));
}

int main(void)
{
    int category, slot;
    /* The garden controller assigns these physical coordinates at startup. */
    for (category = 0; category < GARDEN_OBJECT_CATEGORIES; category++) {
        for (slot = 0; slot < GARDEN_ROW_SLOTS; slot++) {
            struct garden_entry *entry =
                &garden_entries[category * GARDEN_ROW_SLOTS + slot];
            entry->category = (Uint16)category;
            entry->slot = (Uint16)slot;
        }
    }
    check_residents(GARDEN_OBJECT_CHAO);
    check_residents(GARDEN_OBJECT_EGG);
    check_shared_slots();
    return 0;
}
