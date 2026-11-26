#include <sg_xpt.h>
#include <shinobi.h>
#include <string.h>

#include "chao/messages.h"
#include "draw/text_sprite.h"
#include "system/game_state.h"
#include "system/loader.h"
#include "system/text.h"

#define PATH_SIZE 256

static Sint32 *message_offsets = NULL;
static const char *language_suffixes[TEXT_LANGUAGES] = {"_j", "_e", "_f", "_s",
                                                        "_g"};

const Sint32 *garden_messages_load(const char *name)
{
    char path[PATH_SIZE];
    Sint32 *offsets;

    strcpy(path, name);
    strcat(path, language_suffixes[game_state.text_language]);
    strcat(path, ".prs");
    offsets = loader_read_prs_alloc(path);
    if (offsets == NULL) {
        return NULL;
    }
    message_offsets = offsets;
    /* @hack spelling: message_offsets returned, where offsets would do */
    return message_offsets;
}

void garden_messages_free(void)
{
    if (message_offsets != NULL) {
        syFree(message_offsets);
        message_offsets = NULL;
    }
}

const char *garden_message(int id)
{
    if (message_offsets == NULL) {
        return NULL;
    }
    return (const char *)message_offsets + message_offsets[id];
}

struct text_sprite *garden_message_sprite(const char *text, Uint16 *width,
                                          Uint16 *height)
{
    struct text_sprite *sprite;
    Uint16 measured_width;
    Uint16 measured_height;

    sprite = text_sprite_create(text, game_state.text_language);
    if (width == NULL || height == NULL) {
        text_sprite_measure(sprite, text, game_state.text_language,
                            &measured_width, &measured_height);
    } else {
        text_sprite_measure(sprite, text, game_state.text_language, width,
                            height);
    }
    return sprite;
}
