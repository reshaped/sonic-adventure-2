/* the Chao the garden file keeps, and the one the player carries between
 * areas */
#ifndef CHAO_RESIDENTS_H
#define CHAO_RESIDENTS_H
#include <ninja.h>
#include <sg_xpt.h>

#include "chao/garden_save.h"
#include "chao/identity.h"
#include "chao/work.h"

struct task;

struct saved_chao_data {
    struct chao_parameters parameters;
    struct chao_emotions emotions;
    Uint8 unk_f8[0x368 - 0xf8];
    struct chao_gene gene;
    Uint8 unk_3b4[0x3e8 - 0x3b4];
};

/* a Chao's record in the garden file. saved and live overlap: saved's gene
 * starts 24 bytes before live ends. garden_pack_chao writes live first, so
 * the gene it writes next overwrites live's last 24 bytes */
struct chao_save {
    union {
        /* @hack type: head, an array of one parameter record for
         * chao_make_saved_egg, where &saved.parameters would do */
        struct chao_parameters head[1];
        struct chao_profile live;
        struct saved_chao_data saved;
    } record;
};

/* a garden is full with CHAO_PER_GARDEN records */
#define CHAO_PER_GARDEN 8

/* each record holds a Chao or an egg, or is empty: type CHAO_NONE */
extern struct chao_save garden_saved_chao[CHAO_SAVE_SLOTS];
/* empty every record, then lay four eggs. two go in the Neutral Garden, one
 * in the Hero Garden and one in the Dark Garden */
void garden_seed_eggs(void);
/* empty save, which may be null */
void garden_clear_chao(struct chao_save *save);
/* write chao's profile and gene to save. nothing happens where either is
 * null */
void garden_pack_chao(struct task *chao, struct chao_save *save);
void garden_pack_egg(struct task *egg, struct chao_save *save);
/* count the records that hold a Chao or an egg. garden_chao_count counts
 * area's, garden_resident_count the current area's, and garden_total_chao
 * every area's */
int garden_chao_count(int area);
int garden_resident_count(void);
int garden_total_chao(void);
/* the first empty record, or NULL where none is */
struct chao_save *garden_unused_chao(void);
/* spawn the current garden's records, but for the carried Chao's. outside
 * the three gardens nothing spawns. an egg always spawns, and a Chao only
 * outside the classroom. each takes one of the garden's spawn places at
 * random. a Chao whose sleep depth is above 0 starts asleep.
 *
 * where the carried Chao comes from another garden into a full one, a
 * random resident moves out unspawned. it moves to another garden with
 * room. where neither has room, it moves to the carried Chao's garden */
void garden_load_residents(void);
/* spawn save's egg, or its Chao facing yaw, at position, and return the
 * task. an empty record spawns nothing and returns NULL. the GCC side also
 * returns NULL where it can't make the task */
struct task *garden_spawn_chao(struct chao_save *save, NJS_POINT3 *position,
                               Angle yaw);
/* empty player 0's hands, even where no Chao is carried. then spawn the
 * carried Chao 3 units ahead of the player, hand it over and return it.
 * return NULL where none is carried. a hatched Chao spawned in the lobby or
 * the kindergarten neither ages nor forms intentions */
struct task *garden_restore_carried(void);
/* make chao's record the carried one, and note whether chao faces player
 * 0. a null chao carries none. garden_carry_egg's egg must not be null */
void garden_carry_chao(struct task *chao);
void garden_carry_egg(struct task *egg);
/* stamp chao's gene with the save's origin and the clock's time. a clock
 * that fails gives a random stamp */
void chao_stamp_birth(struct task *chao);

#endif
