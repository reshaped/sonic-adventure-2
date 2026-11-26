#include <assert.h>
#include <stdlib.h>

#include "modules/advertise/kart_race_panel.c"

struct menu_works menu_works;
struct kart_race_work kart_race_work;
NJS_CNK_OBJECT cart_sonic_model, cart_tails_model, cart_knuckles_model;
NJS_CNK_OBJECT cart_shadow_model, cart_eggman_model, cart_rouge_model;

Bool menu_approach(Float *value, Float target, Float step)
{
    return FALSE;
}

void menu_scroll_step(const struct menu_scroll *scroll)
{
}

static void check_stats(const struct kart_race_pick *pick, const Uint8 *stats)
{
    assert(pick->stat_0 == stats[0]);
    assert(pick->stat_1 == stats[1]);
    assert(pick->stat_2 == stats[2]);
    assert(pick->stat_3 == stats[3]);
}

static void check_drivers(void)
{
    Int driver;
    const Uint8 *unselected =
        (const Uint8 *)&drivers_by_character[PLAYER_EGGMAN_MECH];

    for (driver = 0; driver < KART_RACE_DRIVERS; ++driver) {
        kart_race_work.picks[0].driver = (Sint8)driver;
        kart_race_work.picks[1].driver = -1;
        kart_race_panel_step(NULL);
        check_stats(&kart_race_work.picks[0], previews[driver].stats);
        check_stats(&kart_race_work.picks[1], unselected);

        kart_race_work.picks[0].driver =
            (Sint8)(KART_RACE_DRIVERS - 1 - driver);
        kart_race_work.picks[1].driver = (Sint8)driver;
        kart_race_panel_step(NULL);
        check_stats(&kart_race_work.picks[0],
                    previews[kart_race_work.picks[0].driver].stats);
        check_stats(&kart_race_work.picks[1], previews[driver].stats);
    }
}

int main(int argc, char **argv)
{
    Int driver, stat;

    assert(argc == 2);
    assert(drivers_by_character[PLAYER_EGGMAN_MECH] == atoi(argv[1]));
    menu_works.work_13 = &kart_race_work;
    kart_race_work.shown = TRUE;
    kart_race_work.drivers.shown = TRUE;
    check_drivers();

    /* A disc-free build's previews begin with identical zeros, and the
     * map's last word holds three zero bytes. Give each owner and each of
     * its bytes a distinct value before testing their identities. */
    drivers_by_character[PLAYER_EGGMAN_MECH] = 0x12345678;
    for (driver = 0; driver < KART_RACE_DRIVERS; ++driver) {
        for (stat = 0; stat < STATS; ++stat) {
            previews[driver].stats[stat] = (Uint8)(1 + driver * STATS + stat);
        }
    }
    check_drivers();
    return 0;
}
