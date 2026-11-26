#include <assert.h>
#include <limits.h>
#include <string.h>

#include <sg_syrtc.h>
#include <shinobi.h>

#include "chao/garden.h"
#include "chao/garden_save.h"
#include "chao/race_state.h"
#include "chao/residents.h"
#include "chao/save_buffers.h"
#include "chao/tree.h"
#include "chao/world.h"
#include "chao/world_card.h"

struct garden_card_request garden_card_request;
static struct garden_file_image card, previous;
static int writes, seeded;

/* There are no live tasks to gather; the race record stays as loaded. */
Sint32 garden_object_count(Uint16 category)
{
    return 0;
}
struct task *garden_active_object(Uint16 category, Uint16 index)
{
    return NULL;
}
void garden_pack_chao(struct task *chao, struct chao_save *save)
{
    assert(0);
}
void garden_pack_egg(struct task *egg, struct chao_save *save)
{
    assert(0);
}
void garden_pack_tree(struct task *tree)
{
    assert(0);
}
int chao_current_area(void)
{
    return CHAO_AREA_NEUTRAL;
}
struct garden_race_save *garden_race_save(void)
{
    return &garden_saved_world.race;
}

void njMemCopy(void *dst, const void *src, Sint32 size)
{
    memcpy(dst, src, size);
}
int garden_card_read(struct garden_card_request *request)
{
    return 0;
}
int garden_card_write(struct garden_card_request *request)
{
    assert(request->size == 24896);
    memcpy(&card, request->data, request->size);
    writes++;
    return 0;
}
Sint32 syCfgGetIndividualID(Sint8 *id)
{
    return -1;
}
Sint32 syRtcGetDate(SYS_RTC_DATE *date)
{
    return -1;
}
void garden_seed_eggs(void)
{
    seeded++;
}

static int stage(const struct garden_save *candidate, Uint32 size)
{
    memcpy(card.garden, candidate, sizeof(*candidate));
    return garden_file_stage(&garden_card_request, &card, size);
}

int main(void)
{
    struct garden_save world;
    int bad_slots[] = {-1, CHAO_SAVE_SLOTS - 1, CHAO_SAVE_SLOTS + 1, INT_MIN,
                       INT_MAX};
    unsigned int i;
    size_t size = sizeof(card.garden) + sizeof(garden_saved_chao);

    assert(sizeof(garden_saved_world) == 0x338);
    assert(sizeof(garden_saved_chao[0]) == 1000);
    assert(size == 24896);
    garden_saved_world.chao_slots = CHAO_SAVE_SLOTS;
    garden_saved_world.frames = 1357;
    garden_saved_world.classroom.count = 2;
    garden_saved_world.classroom.lesson[0] = 24;
    garden_saved_world.classroom.lesson[1] = 34;
    garden_saved_world.trees[0][0].kind = 9;
    garden_saved_world.trees[0][0].stage = TREE_STAGE_MATURE;
    garden_saved_world.trees[0][1].kind = 10;
    garden_saved_world.trees[0][1].stage = TREE_STAGE_PERMANENT;
    /* The garden plants six of the eight saved trees per garden. */
    garden_saved_world.trees[0][7].kind = 254;
    garden_saved_chao[23].record.saved.parameters.type = CHAO_CHILD;
    garden_saved_chao[23].record.saved.parameters.garden = CHAO_AREA_DARK;
    garden_saved_chao[23].record.saved.parameters.medal = 26;
    garden_file_write();
    assert(writes == 1);
    previous = card;
    world = garden_saved_world;
    garden_file_read();
    memset(&garden_file_image, 0x54, sizeof(garden_file_image));
    assert(stage(&world, size));
    assert(memcmp(&garden_file_image, &previous, sizeof(previous)) == 0);
    memset(&garden_saved_world, 0, sizeof(garden_saved_world));
    memset(garden_saved_chao, 0, sizeof(garden_saved_chao));
    garden_file_unpack();
    assert(garden_save_slots() == CHAO_SAVE_SLOTS);
    assert(garden_saved_world.frames == 1357);
    assert(garden_saved_chao[23].record.saved.parameters.garden ==
           CHAO_AREA_DARK);
    assert(garden_saved_chao[23].record.saved.parameters.medal == 26);
    garden_file_write();
    assert(writes == 2 && memcmp(&card, &previous, sizeof(card)) == 0);

    garden_file_read();
    assert(!garden_file_recover(GARDEN_FILE_ERROR));
    garden_file_write();
    assert(writes == 2 && seeded == 0 && garden_saved_world.frames == 1357);
    garden_file_read();
    assert(stage(&world, size));
    for (i = 0; i < sizeof(bad_slots) / sizeof(bad_slots[0]); i++) {
        world.chao_slots = bad_slots[i];
        assert(!stage(&world, size));
    }
    world.chao_slots = CHAO_SAVE_SLOTS;
    assert(!stage(&world, size - 1));
    assert(!stage(&world, size + 1));
    world.classroom.count = 5;
    assert(!stage(&world, size));
    world.classroom.count = 2;
    world.classroom.lesson[1] = 19;
    assert(!stage(&world, size));
    world.classroom.lesson[1] = 34;
    world.trees[0][0].kind = 11;
    assert(!stage(&world, size));
    world.trees[0][0].kind = 9;
    world.trees[0][0].stage = 99;
    assert(!stage(&world, size));
    assert(memcmp(&garden_file_image, &previous, sizeof(previous)) == 0);
    assert(!garden_file_recover(GARDEN_FILE_INVALID));
    garden_file_write();
    garden_file_write();
    assert(garden_file_state() == GARDEN_FILE_ERROR && writes == 2);
    assert(seeded == 0 && garden_saved_world.frames == 1357);
    assert(garden_saved_chao[23].record.saved.parameters.type == CHAO_CHILD);

    /* A transient read error cannot unlock the same known-bad file. */
    garden_file_read();
    assert(!garden_file_recover(GARDEN_FILE_ERROR));
    garden_file_write();
    assert(writes == 2 && seeded == 0);

    /* First use can start a playable, unsaved world without replacing the card.
     */
    memset(&garden_saved_world, 0, sizeof(garden_saved_world));
    assert(!garden_file_recover(GARDEN_FILE_INVALID));
    assert(seeded == 1 && garden_save_slots() == CHAO_SAVE_SLOTS);
    garden_file_write();
    assert(writes == 2);
    assert(garden_file_recover(GARDEN_FILE_ABSENT));
    garden_file_write();
    assert(writes == 3 && seeded == 2);
    return 0;
}
