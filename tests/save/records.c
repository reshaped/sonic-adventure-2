/* Exercise the same VMU field layout in the portable and original C routes. */
#include <assert.h>
#include <string.h>

#include <ninja.h>
#include "cart/course.h"
#include "cart/race.h"
#include "player/player.h"
#include "save/save.h"
#include "stage/stage.h"
#include "system/flags.h"
#include "system/game_state.h"
#include "system/task.h"

/* the port's two calls from make dc's side of save.c, which keep its 16:9 and
 * 60 fps in the save. both stay off here */
void port_store(Uint8 *flags);
void port_load(Uint8 flags);

void port_store(Uint8 *flags)
{
    *flags &= (Uint8)~0x30;
}

void port_load(Uint8 flags)
{
    (void)flags;
}


/* The retail image exports this reader without a public header. */
Sint32 save_best_score(void);

Uint8 game_flags[GAME_FLAGS];
Uint8 game_bits[GAME_BITS];
struct game_state game_state;
struct cart_race cart_race;
struct cart_setup cart_setup;
Sint32 cart_result_done;

int stage_character(int slot)
{
    return PLAYER_SONIC;
}

int stage_current_id(void)
{
    return STAGE_CITY_ESCAPE;
}

Float njRoundUp(Float number)
{
    assert(number == 0.0F);
    return number;
}

Uint8 flag_get(int flag)
{
    assert(0);
    return 0;
}

void flag_set(int flag)
{
    assert(0);
}

struct task *task_create(Uint16 flags, int level, void (*step)(struct task *),
                         const char *name)
{
    static struct task task;
    task.step = step;
    return &task;
}

static struct save_record *scores(void)
{
    return save_data.stages[STAGE_CITY_ESCAPE].records[0];
}

static void existing(Sint32 first, Sint32 second, Sint32 third)
{
    Sint32 values[SAVE_BEST];
    int rank;

    values[0] = first;
    values[1] = second;
    values[2] = third;
    for (rank = 0; rank < SAVE_BEST; rank++) {
        scores()[rank].rings = 10 + rank;
        scores()[rank].time.minute = 1 + rank;
        scores()[rank].score_low = (Uint16)values[rank];
        scores()[rank].score_high =
            (Uint8)(values[rank] >> SAVE_SCORE_LOW_BITS);
    }
}

static void expect(Sint32 a, Sint32 b, Sint32 c)
{
    int rank;

    for (rank = 0; rank < SAVE_BEST; rank++) {
        assert(scores()[rank].rings == 10 + rank);
        assert(scores()[rank].time.minute == 1 + rank);
    }
    assert(save_record_score(scores()) == a);
    assert(save_record_score(scores() + 1) == b);
    assert(save_record_score(scores() + 2) == c);
    assert(save_best_score() == a);
}

static void stage_scores(void)
{
    int i;
    static const Sint32 boundary[] = {65535,    65536,     0xfeffff, 0xff0000,
                                      0xffffff, 0x1000000, 99999999};
#ifdef NONMATCHING
    static const Sint32 result[] = {65535,    65536,    0xfeffff, 0xff0000,
                                    0xffffff, 0xffffff, 0xffffff};
#else
    static const Sint32 result[] = {65535, 65536,   0xfeffff, 0,
                                    65535, 0x10000, 0xfae0ff};
#endif

    for (i = 0; i < (int)(sizeof(boundary) / sizeof(boundary[0])); i++) {
        existing(0, 0, 0);
        assert(save_record_stage(boundary[i]));
        expect(result[i], 0, 0);
    }
    existing(100000, 90000, 80000);
    assert(save_record_stage(30000));
#ifdef NONMATCHING
    expect(100000, 90000, 80000);
    assert(save_record_stage(85000));
    expect(100000, 90000, 85000);
    assert(save_record_stage(95000));
    expect(100000, 95000, 90000);
    assert(save_record_stage(110000));
    expect(110000, 100000, 95000);
    assert(save_record_stage(-1));
    expect(110000, 100000, 95000);
    /* Ties keep their existing place but may precede a lower score. */
    assert(save_record_stage(100000));
    expect(110000, 100000, 100000);
    existing(0xffffff, 0xfffffe, 0xfffffd);
    assert(save_record_stage(99999999));
    expect(0xffffff, 0xffffff, 0xfffffe);
#else
    expect(100000, 30000, 90000);
#endif
}

static void kart_times(void)
{
    struct save_data before;
    struct save_kart_record *record;
    int i;
    int course;

    memset(&save_data, 0x5a, sizeof(save_data));
    for (course = 0; course < SAVE_KART_RECORDS; course++) {
        record = &save_data.kart_records[course];
        for (i = 0; i < SAVE_BEST; i++) {
            record->best[i].time.minute = 10 * (i + 1);
        }
        game_state.kart_course = course;
        save_record_kart_time(0, 25, 16, 12);
        assert(record->best[2].time.minute == 25);
        save_record_kart_time(0, 15, 14, 21);
        assert(record->best[1].time.minute == 15);
        assert(record->best[2].time.minute == 20);
        save_record_kart_time(0, 5, 12, 34);
        assert(record->best[1].time.minute == 10);
        assert(record->best[2].time.minute == 15);
        assert(record->best[0].character == PLAYER_SONIC);
        assert(record->best[0].time.minute == 5);
        cart_start_hud();
        assert(cart_race.best.minute == 5 && cart_race.best.second == 12 &&
               cart_race.best.hundredths == 34);
    }
#ifdef NONMATCHING
    before = save_data;
    for (i = -1; i <= SAVE_KART_RECORDS + 1; i++) {
        if (i >= 0 && i < SAVE_KART_RECORDS) {
            continue;
        }
        game_state.kart_course = i;
        assert(!cart_time_record_available());
        save_record_kart_time(0, 0, 0, 1);
        assert(!memcmp(&save_data, &before, sizeof(save_data)));
        cart_start_hud();
        assert(cart_race.best.minute == -1);
    }
    cart_setup.course_mode = CART_DOWNLOAD;
    download_chosen.slot = DOWNLOAD_SLOTS - 1;
    save_data.download_best_times[DOWNLOAD_SLOTS - 1].minute = 7;
    cart_start_hud();
    assert(cart_race.best.minute == 7);
    download_chosen.slot = DOWNLOAD_SLOTS;
    assert(!cart_time_record_available());
    cart_start_hud();
    assert(cart_race.best.minute == -1);
    download_chosen.slot = -1;
    assert(!cart_time_record_available());
#endif
}

int main(void)
{
    game_state.unlock_pending = 1;
    game_state.minute = 99;
    cart_setup.course_mode = CART_BEGINNER;
    stage_scores();
    kart_times();
    return 0;
}
