/* The game registers its own row draw. Record every rendering boundary so a
 * rejected icon cannot submit a quad with the previously selected texture. */
#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include <shinobi.h>

#include "camera/camera.h"
#include "player/player.h"
#include "shared/item_box.h"
#include "shared/item_icon.h"
#include "system/game_state.h"
#include "system/task.h"

struct game_state game_state;
int camera_current_index;
Sint32 camera_active_count = 1;

static struct task row;
static struct task_work work;
static int character;
static Uint32 wanted;
static char calls[32];
static unsigned int call_count;

static void record(char call)
{
    assert(call_count + 1 < sizeof(calls));
    calls[call_count++] = call;
    calls[call_count] = '\0';
}

struct task *task_create(Uint16 flags, int level, void (*step)(struct task *),
                         const char *name)
{
    assert(flags == TASK_WORK && level == 3 && step != NULL);
    assert(row.motion.p == NULL);
    row.work = &work;
    row.step = step;
    return &row;
}

void task_destroy(struct task *task)
{
    abort();
}
void *syCalloc(Uint32 count, Uint32 size)
{
    return calloc(count, size);
}
void syFree(void *ptr)
{
    free(ptr);
}
int player_character(int slot)
{
    assert(slot == camera_current_index);
    return character;
}

void njColorBlendingMode(Int target, Int mode)
{
    assert(
        (target == NJD_SOURCE_COLOR && mode == NJD_COLOR_BLENDING_SRCALPHA) ||
        (target == NJD_DESTINATION_COLOR &&
         mode == NJD_COLOR_BLENDING_INVSRCALPHA));
    record(target == NJD_SOURCE_COLOR ? 'S' : 'D');
}

Sint32 njSetTexture(NJS_TEXLIST *texlist)
{
    assert(texlist == &item_icon_texlist);
    record('T');
    return 1;
}

Sint32 njSetTextureNum(Uint32 number)
{
    assert(number == wanted && number < item_icon_texlist.nbTexture);
    assert(item_icon_texlist.textures[number].texaddr != 0);
    record('N');
    return 1;
}

Sint32 njGetTextureNumG(void)
{
    record('G');
    return 90;
}
Bool njPushMatrix(NJS_MATRIX *m)
{
    record('P');
    return TRUE;
}
Bool njPopMatrix(Uint32 count)
{
    assert(count == 1);
    record('p');
    return TRUE;
}
void njTranslateV(NJS_MATRIX *m, NJS_VECTOR *v)
{
    record('t');
}
void njRotateY(NJS_MATRIX *m, Angle angle)
{
    record('r');
}
void njScale(NJS_MATRIX *m, Float x, Float y, Float z)
{
    record('s');
}
void njDrawTexture3DEx(NJS_TEXTURE_VTX *corners, Int count, Int trans)
{
    assert(count == 4 && trans == TRUE);
    record('W');
}

void njDrawTexture(NJS_TEXTURE_VTX *corners, Int count, Int texture, Int trans)
{
    assert(count == 4 && texture == 90 && trans == TRUE);
    record('H');
}

static void both(int kind, int draws)
{
    NJS_POINT3 point = {1.0F, 2.0F, 3.0F};

    calls[0] = '\0';
    call_count = 0;
    assert(item_icon_draw(kind, &point, 0x4000, 2.0F) == draws);
    assert(strcmp(calls, draws ? "SDTNPtrsWp" : "") == 0);
    calls[0] = '\0';
    call_count = 0;
    item_icon_row_add(0, kind);
    assert(row.draw != NULL);
    row.draw(&row);
    assert(strcmp(calls, draws ? "SDTNGH" : "") == 0);
    row.destructor(&row);
}

static void loaded_slots(void)
{
    int slot;

    for (slot = 0; slot < (int)item_icon_texlist.nbTexture; slot++) {
        item_icon_texlist.textures[slot].texaddr = (Uint32)(slot + 100);
    }
    wanted = ITEM_10_RINGS;
    both(ITEM_10_RINGS, 1);
    wanted = ITEM_1UP;
    character = -1;
    both(ITEM_1UP, 1);
    for (slot = 0; slot <= PLAYER_EGGMAN_MECH; slot++) {
        character = slot;
        wanted = ITEM_1UP + slot;
        both(ITEM_1UP, 1);
    }
}

int main(void)
{
    static NJS_TEXNAME slots[19];
    game_state.unk_0b = 1;
#ifdef OPTICAL
    assert(item_icon_texlist.nbTexture == 19 &&
           item_icon_texlist.textures != NULL);
#else
    assert(item_icon_texlist.nbTexture == 0);
    both(ITEM_10_RINGS, 0);
    item_icon_texlist.textures = slots;
    item_icon_texlist.nbTexture = 19;
#endif
    loaded_slots();
#ifdef NONMATCHING
    both(-1, 0);
    both(ITEM_1UP + 1, 0);
    character = PLAYER_SUPER_SONIC;
    both(ITEM_1UP, 0);
    item_icon_texlist.textures[ITEM_10_RINGS].texaddr = 0;
    both(ITEM_10_RINGS, 0);
    item_icon_texlist.textures = NULL;
    both(ITEM_10_RINGS, 0);
    item_icon_texlist.nbTexture = 0;
    both(ITEM_10_RINGS, 0);
#endif
    return 0;
}
