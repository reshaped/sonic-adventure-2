/* The host supplies a card and a model owner. The pack reader and the PRS
 * decoder are the same functions the kart stage calls. */
#include <string.h>

#include "cart/course.h"
#include "cart/stage.h"
#include "cart/track.h"
#include "save/save.h"

Uint8 kart_work[DOWNLOAD_SIZE + 0x19000];
struct download_entry download_chosen;
struct object_list cart_objects = {13, NULL};
struct cart_gate_list cart_piece_gates[CART_PIECES];

static const void *card;
static Uint32 card_size;
static Bool can_load_model;

void kart_test_card(const void *input, Uint32 size, Bool accept_model)
{
    static struct cart_gate_point gates[1][4];
    card = input;
    card_size = size;
    can_load_model = accept_model;
    cart_piece_gates[3].gates = gates;
    cart_piece_gates[3].count = 1;
}

void *save_download_load_sized(struct download_entry *entry,
                               struct download_file *file, Uint32 *size)
{
    (void)entry;
    *size = card_size;
    return card ? memcpy(file, card, card_size) : NULL;
}

Bool cart_replace_download_model(const void *archive, Uint32 size)
{
    return can_load_model && archive && size == 24;
}
