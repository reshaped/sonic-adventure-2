/* the 512-byte Chao record of Sonic Adventure's memory card file, and the
 * editor's page that makes one. the garden takes Chao in from such records */
#ifndef CHAO_LEGACY_H
#define CHAO_LEGACY_H
#include <sg_xpt.h>

struct task;

/* finish_flags' bits */
enum legacy_finish {
    LEGACY_BLACK = 0x02,
    LEGACY_GOLD = 0x04,
    LEGACY_SILVER = 0x08,
    LEGACY_DOWNLOAD = 0x20,
    LEGACY_JEWEL = 0x40
};

/* mark_c, mark_h, mark_a and mark_o hold CHAO's four letters. each of parts
 * holds the kind of the animal that gave it, or -1 for none. skill holds
 * swim, fly, run and power, in that order */
struct legacy_chao {
    Uint8 group, garden;
    Sint8 affection;
    Uint8 mark_c;
    Sint8 name[8];
    Uint16 skill[4];
    Uint16 health, max_health;
    Sint8 fruit[8];
    Float run_power, fly_swim, evolution;
    Uint16 like_changes, life, old, minimal;
    Uint8 race_wins, mark_h;
    Uint16 finish_flags;
    Uint8 unk_38[16];
    Uint32 id;
    Sint8 parts[7];
    Uint8 mark_a;
    Uint16 swim_points, fly_points, run_points, power_points;
    Sint8 kindness, aggression, curiosity;
    Uint8 unk_5f[0x178 - 0x5f];
    Uint8 generation, mark_o, unk_17a;
    Uint8 unk_17b[0x192 - 0x17b];
    Uint8 jewel;
    Uint8 unk_193[0x200 - 0x193];
};

/* the record chao_debug_legacy's page sets up when it opens and then edits.
 * editing it changes no Chao's save */
extern struct legacy_chao chao_legacy_preview;
void chao_debug_legacy(struct task *page);

#endif
