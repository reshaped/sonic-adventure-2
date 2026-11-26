/* the depths the kart race panel draws a course's three best times at,
 * which the image takes from a stack slot that the rule's y fills first */
#include <assert.h>

#include "modules/advertise/kart_race_panel.c"

struct kart_race_work kart_race_work;
struct save_data save_data;
NJS_CNK_OBJECT cart_sonic_model, cart_tails_model, cart_knuckles_model;
NJS_CNK_OBJECT cart_shadow_model, cart_eggman_model, cart_rouge_model;

static int times_drawn;

void menu_fade_begin(Float level)
{
    (void)level;
}

void menu_sprite_row_draw(const struct menu_sprite_row *row)
{
    (void)row;
}

void menu_list_draw(const struct menu_list *list,
                    const struct menu_scroll *scroll, Float scale)
{
    (void)list;
    (void)scroll;
    (void)scale;
}

void njDrawSprite2D(NJS_SPRITE *sp, Int n, Float pri, Uint32 attr)
{
    (void)sp;
    (void)n;
    (void)pri;
    (void)attr;
}

void share_data_sprite_draw(Int cell, Float scale, const NJS_POINT3 *pos,
                            Int attr)
{
    (void)cell;
    (void)scale;
    (void)pos;
    (void)attr;
}

void share_data_time_draw(const NJS_POINT3 *pos, const struct save_time *time,
                          Float scale)
{
    static const Float depths[SAVE_BEST] = {140.0F, 145.0F, 150.0F};

    (void)time;
    (void)scale;
    assert(times_drawn < SAVE_BEST);
    assert(pos->z == depths[times_drawn]);
    times_drawn++;
}

int main(void)
{
    kart_race_work.course = 0;
    kart_race_courses_draw();
    assert(times_drawn == SAVE_BEST);
    return 0;
}
