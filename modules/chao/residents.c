#include <ninja.h>
#include <sg_syrtc.h>
#include <sg_xpt.h>
#include <stdlib.h>
#include <string.h>

#include "chao/actor.h"
#include "chao/behavior.h"
#include "chao/egg.h"
#include "chao/emotion.h"
#include "chao/garden.h"
#include "chao/garden_save.h"
#include "chao/genes.h"
#include "chao/identity.h"
#include "chao/life.h"
#include "chao/residents.h"
#include "chao/sleep.h"
#include "chao/work.h"
#include "object/mover.h"
#include "player/player.h"
#include "system/angle.h"
#include "system/task.h"
#include "system/util.h"

#define SPAWN_PLACES 16
#define NO_GARDEN 0
#define NO_CLASSROOM (-1)
/* garden_seed_eggs's records: the Neutral Garden's eggs, then the Hero
 * Garden's, then the Dark Garden's, up to SEEDED_EGGS */
#define FIRST_HERO_EGG 2
#define FIRST_DARK_EGG 3
#define SEEDED_EGGS 4
#define RESTORE_AHEAD 3.0F
#define RANDOM_ID_BYTE() ((Uint32)(Uint8)(rand() * (100.0F * RAND_SCALE)))

static struct chao_save *carried_chao = NULL;
static Bool carried_faced_player = FALSE;

/* clang-format off */
static NJS_POINT3 spawn_positions[CHAO_GARDENS][SPAWN_PLACES] = {
#include "chao/residents/spawn_positions.inc"
};
/* clang-format on */

static Uint32 chao_birth_time(void)
{
    SYS_RTC_DATE date;
    Sint32 result;
    Uint32 stamp;

    result = syRtcGetDate(&date);
    if (result == SYD_RTC_ERR_OK) {
        stamp = 0;
        stamp |= date.second & 63;
        stamp |= (date.minute & 63U) << 6;
        stamp |= (date.hour & 31U) << 12;
        stamp |= (date.day & 31U) << 17;
        stamp |= (date.month & 15) << 22;
        stamp |= ((date.year - SYD_RTC_FIRSTYEAR) & 63U) << 26;
        return stamp;
    }
    stamp = 0;
    stamp |= RANDOM_ID_BYTE();
    stamp |= RANDOM_ID_BYTE() << 8;
    stamp |= RANDOM_ID_BYTE() << 16;
    stamp |= RANDOM_ID_BYTE() << 24;
    return stamp;
}

void chao_stamp_birth(struct task *chao)
{
    struct chao_gene *gene = &((struct chao_work *)chao->work)->gene;
    struct garden_origin *origin;

    origin = garden_save_origin();
    gene->origin = *origin;
    gene->birth = chao_birth_time();
}

void garden_seed_eggs(void)
{
    struct chao_parameters *parameters;
    struct chao_gene *gene;
    int slot;
    int index;

    memset(garden_saved_chao, 0, sizeof(garden_saved_chao));
    /* @hack spelling: index, a copy of slot, where slot would do */
    for (slot = 0; slot < FIRST_HERO_EGG; slot++) {
        index = slot;
        parameters = &garden_saved_chao[index].record.saved.parameters;
        parameters->type = CHAO_EGG;
        parameters->garden = CHAO_AREA_NEUTRAL;
        gene = &garden_saved_chao[index].record.saved.gene;
        chao_new_gene(gene);
    }
    /* @hack redundant: a loop of one pass for the Hero Garden's egg */
    for (slot = FIRST_HERO_EGG; slot < FIRST_DARK_EGG; slot++) {
        index = slot;
        parameters = &garden_saved_chao[index].record.saved.parameters;
        parameters->type = CHAO_EGG;
        parameters->garden = CHAO_AREA_HERO;
        gene = &garden_saved_chao[index].record.saved.gene;
        chao_new_hero_gene(gene);
    }
    /* @hack redundant: a loop of one pass for the Dark Garden's egg */
    for (slot = FIRST_DARK_EGG; slot < SEEDED_EGGS; slot++) {
        index = slot;
        parameters = &garden_saved_chao[index].record.saved.parameters;
        parameters->type = CHAO_EGG;
        parameters->garden = CHAO_AREA_DARK;
        gene = &garden_saved_chao[index].record.saved.gene;
        chao_new_dark_gene(gene);
    }
}

void garden_clear_chao(struct chao_save *save)
{
    if (save != NULL) {
        memset(save, 0, sizeof(*save));
    }
}

/* @hack spelling: garden_pack_chao inlined in garden_pack_egg, where a call
 * would do */
#pragma inline(garden_pack_chao)
void garden_pack_chao(struct task *chao, struct chao_save *save)
{
    struct chao_work *work;
    struct chao_profile *profile;
    struct chao_gene *gene;

    if (chao != NULL && save != NULL) {
        work = (struct chao_work *)chao->work;
        memset(save, 0, sizeof(*save));
        profile = &save->record.live;
        gene = &save->record.saved.gene;
        *profile = work->profile;
        *gene = work->gene;
    }
}

void garden_pack_egg(struct task *egg, struct chao_save *save)
{
    garden_pack_chao(egg, save);
}

int garden_chao_count(int area)
{
    struct chao_parameters *parameters;
    int slots = garden_save_slots();
    int slot;
    int count = 0;

    for (slot = 0; slot < slots; slot++) {
        parameters = &garden_saved_chao[slot].record.saved.parameters;
        if (parameters->type != CHAO_NONE && parameters->garden == area) {
            count++;
        }
    }
    return count;
}

int garden_resident_count(void)
{
    return garden_chao_count(chao_current_area());
}

int garden_total_chao(void)
{
    struct chao_parameters *parameters;
    int slots = garden_save_slots();
    int slot;
    int count = 0;

    for (slot = 0; slot < slots; slot++) {
        parameters = &garden_saved_chao[slot].record.saved.parameters;
        if (parameters->type != CHAO_NONE) {
            count++;
        }
    }
    return count;
}

struct chao_save *garden_unused_chao(void)
{
    struct chao_parameters *parameters;
    int slots = garden_save_slots();
    int slot;

    for (slot = 0; slot < slots; slot++) {
        parameters = &garden_saved_chao[slot].record.saved.parameters;
        if (parameters->type == CHAO_NONE) {
            return &garden_saved_chao[slot];
        }
    }
    return NULL;
}

void garden_load_residents(void)
{
    struct chao_save *save;
    struct chao_save *end;
    struct chao_save *held;
    struct chao_parameters *parameters;
    NJS_POINT3 *position;
    struct task *chao;
    int area;
    int slots;
    Bool full;
    int displaced;
    Uint32 place;
    int count;
    int first;
    int second;
    int destination;
    Sint8 classroom_slot;

    area = chao_current_area();
    slots = garden_save_slots();
    save = garden_saved_chao;
    end = &garden_saved_chao[slots];
    full = FALSE;
    displaced = 0;
    destination = NO_GARDEN;
    count = 0;
    if (area < CHAO_AREA_NEUTRAL || area > CHAO_AREA_DARK) {
        return;
    }
    held = carried_chao;
    if (held != NULL && held->record.saved.parameters.garden != area &&
        garden_chao_count(area) >= CHAO_PER_GARDEN) {
        full = TRUE;
        switch (area) {
        case CHAO_AREA_NEUTRAL:
            first = CHAO_AREA_HERO;
            second = CHAO_AREA_DARK;
            break;
        case CHAO_AREA_HERO:
            first = CHAO_AREA_NEUTRAL;
            second = CHAO_AREA_DARK;
            break;
        case CHAO_AREA_DARK:
            first = CHAO_AREA_NEUTRAL;
            second = CHAO_AREA_HERO;
            break;
        }
        displaced = (Uint32)(rand() * (7.9999F * RAND_SCALE));
        slots = garden_chao_count(first);
        if (slots < CHAO_PER_GARDEN) {
            destination = first;
        }
        slots = garden_chao_count(second);
        if (slots < CHAO_PER_GARDEN) {
            if (destination != NO_GARDEN) {
                if (rand() * RAND_SCALE < 0.5F) {
                    destination = second;
                }
            } else {
                destination = second;
            }
        }
        if (destination == NO_GARDEN) {
            destination = held->record.saved.parameters.garden;
        }
    }
    /* @hack spelling: skip the held and displaced records by jumping to
     * the increment, where a for loop and continue would do */
    while (save < end) {
        parameters = &save->record.saved.parameters;
        if (parameters->garden == area) {
            /* @hack redundant: held read again for each record, where the
             * copy read before the walk would do */
            held = carried_chao;
            if (save == held) {
                goto next_slot;
            }
            classroom_slot = save->record.saved.parameters.classroom_slot;
            place = rand() * (15.9F * RAND_SCALE);
            position = &spawn_positions[area - CHAO_AREA_NEUTRAL][place];
            chao = NULL;
            if (full && count == displaced) {
                parameters->garden = destination;
                count++;
                goto next_slot;
            }
            count++;
            switch (parameters->type) {
            case CHAO_NONE:
                break;
            case CHAO_EGG:
                garden_spawn_chao(save, position, RANDOM_TURN());
                break;
            default:
                if (classroom_slot == NO_CLASSROOM) {
                    chao = garden_spawn_chao(save, position, RANDOM_TURN());
                }
                break;
            }
            /* @hack spelling: two nested tests, where one condition with &&
             * would do */
            if (chao != NULL) {
                if (chao_emotion_get(chao, CHAO_SLEEP_DEPTH) > 0) {
                    chao_set_behavior(chao, chao_sleep_behavior);
                }
            }
        }
    next_slot:
        save++;
    }
}

struct task *garden_spawn_chao(struct chao_save *save, NJS_POINT3 *position,
                               Angle yaw)
{
    struct chao_profile *profile = &save->record.live;
    struct chao_gene *gene = &save->record.saved.gene;
    struct task *chao;

    switch (profile->parameters.type) {
    case CHAO_NONE:
        chao = NULL;
        break;
    case CHAO_EGG:
        if (profile->parameters.reborn_egg) {
            chao = chao_create_egg_with_profile(gene, profile, position,
                                                EGG_RESIDENT, save);
        } else {
            chao = chao_create_egg(gene, position, EGG_RESIDENT, save);
        }
        break;
    default:
        chao = chao_create(gene, profile, NULL, position, yaw, save);
        break;
    }
    return chao;
}

struct task *garden_restore_carried(void)
{
    NJS_POINT3 position;
    Angle yaw;
    struct chao_save *held;
    struct task *chao;

    yaw = QUARTER_TURN - player_work[0]->ang.y;
    player_let_go(0);
    player_records[0]->held = NULL;
    held = carried_chao;
    /* @hack spelling: a goto to the null return at the end, where return
     * NULL would do */
    if (held == NULL) {
        goto no_resident;
    }
    position = player_work[0]->pos;
    position.x += njSin(yaw) * RESTORE_AHEAD;
    position.z += njCos(yaw) * RESTORE_AHEAD;
    chao = garden_spawn_chao(held, &position, 0);
    /* @bug memory: reads the spawned Chao's work untested (0x0c502b00,
     * 0x0c502b04). garden_spawn_chao returns NULL for an empty record, and
     * garden_load_residents tests its result (0x0c5029ac) */
#ifdef NONMATCHING
    /* this side repairs the bug tagged above: without a Chao, it goes on as for
     * no resident */
    if (chao == NULL) {
        goto no_resident;
    }
#endif
    if (carried_faced_player) {
        chao->work->ang.y = yaw;
    } else {
        chao->work->ang.y = yaw + HALF_TURN;
    }
    switch (chao_current_area()) {
    case CHAO_AREA_LOBBY:
    case CHAO_AREA_KINDERGARTEN:
        if (held->record.saved.parameters.type != CHAO_EGG) {
            ((struct chao_work *)chao->work)->flags |= CHAO_NO_INTENTIONS;
            ((struct chao_work *)chao->work)->flags &= ~CHAO_AGES;
        }
        break;
    }
    carried_chao = NULL;
    player_hand_task(0, chao);
    return chao;
no_resident:
    return NULL;
}

void garden_carry_chao(struct task *chao)
{
    if (chao != NULL) {
        carried_chao = ((struct chao_work *)chao->work)->save;
        if (angle_distance(mover_player_heading(chao, 0), chao->work->ang.y) <
            QUARTER_TURN) {
            carried_faced_player = TRUE;
        } else {
            carried_faced_player = FALSE;
        }
    } else {
        carried_chao = NULL;
    }
}

void garden_carry_egg(struct task *egg)
{
    carried_chao = ((struct chao_work *)egg->work)->save;
}
