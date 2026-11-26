#include <ninja.h>
#include <sg_xpt.h>

#include "event/equipment.h"
#include "event/scene.h"
#include "player/player.h"
#include "system/flags.h"

#define HIDE_ALWAYS (-1)
#define SHOW_ALWAYS (-2)

/* the upgrade whose flag shows each attachment, or SHOW_ALWAYS */
static Sint32 attachment_flags[EVENT_ATTACHMENTS] = {
    UPGRADE_SONIC_SHOES,       UPGRADE_SONIC_FLAMERING,
    UPGRADE_SONIC_RUBBERUNIT,  UPGRADE_SONIC_MAGICWRIST,
    UPGRADE_SHADOW_SHOES,      UPGRADE_SHADOW_FLAMERING,
    UPGRADE_KNUCKLES_CLAWS,    UPGRADE_KNUCKLES_CLAWS,
    UPGRADE_KNUCKLES_GROVES,   UPGRADE_KNUCKLES_GROVES,
    UPGRADE_KNUCKLES_SUNGLASS, UPGRADE_KNUCKLES_AIR,
    UPGRADE_ROUGE_NAILS,       UPGRADE_ROUGE_SCOPE,
    UPGRADE_ROUGE_BOOTS,       SHOW_ALWAYS};
/* the matrix capture_node took at each part's node, and whether it took one
 * since event_attachments_reset */
static NJS_MATRIX matrices[EVENT_ATTACHMENTS][EVENT_ATTACHMENT_PARTS];
static Sint8 captured[EVENT_ATTACHMENTS][EVENT_ATTACHMENT_PARTS];

static void capture_node(NJS_CNK_OBJECT *node);
static Bool part_visible(int index);

void event_attachments_reset(void)
{
    Sint8 *flag = &captured[0][0];
    int left;

    /* @hack spelling: the flags cleared a byte at a time, counting down,
     * where memset would do */
    for (left = sizeof(captured); left != 0; left--) {
        *flag++ = 0;
    }
    /* @hack redundant: the store of 0 to left, which nothing reads */
    left = 0, njCnkSetMotionCallback(NULL);
}

void event_attachments_watch(NJS_CNK_OBJECT *owner)
{
    int i;

    for (i = 0; i < EVENT_ATTACHMENTS; i++) {
        if (event_scene_file->attachments[i].owner == owner) {
            njCnkSetMotionCallback(capture_node);
        }
    }
}

void event_attachments_unwatch(void)
{
    njCnkSetMotionCallback(NULL);
}

#define PART(index, side)                                                      \
    (event_scene_file->attachments[(index)].parts[(side)].model)

/* @hack spelling: a macro expanded once for each side, where a loop over the
 * sides would do */
#define DRAW_PART(index, side)                                                 \
    {                                                                          \
        if (captured[(index)][(side)] && part_visible((index))) {              \
            if (PART(index, side)) {                                           \
                njPushMatrixEx();                                              \
                njSetMatrix(NULL, &matrices[(index)][(side)]);                 \
                PART(index, side)->pos[0] = 0.0F;                              \
                PART(index, side)->pos[1] = 0.0F;                              \
                PART(index, side)->pos[2] = 0.0F;                              \
                PART(index, side)->ang[0] = 0;                                 \
                PART(index, side)->ang[1] = 0;                                 \
                PART(index, side)->ang[2] = 0;                                 \
                njCnkSimpleDrawObject(PART(index, side));                      \
                njPopMatrix(1);                                                \
            }                                                                  \
        }                                                                      \
    }

void event_attachments_draw(void)
{
    int i;

    for (i = 0; i < EVENT_ATTACHMENTS; i++) {
        DRAW_PART(i, 0);
        DRAW_PART(i, 1);
    }
}

static void capture_node(NJS_CNK_OBJECT *node)
{
    int i;

    for (i = 0; i < EVENT_ATTACHMENTS; i++) {
        if (node == event_scene_file->attachments[i].parts[0].node) {
            njGetMatrix(&matrices[i][0]);
            captured[i][0] = TRUE;
        }
        if (node == event_scene_file->attachments[i].parts[1].node) {
            njGetMatrix(&matrices[i][1]);
            captured[i][1] = TRUE;
        }
    }
}

static Bool part_visible(int index)
{
    if (attachment_flags[index] == HIDE_ALWAYS) {
        return FALSE;
    }
    if (attachment_flags[index] == SHOW_ALWAYS) {
        return TRUE;
    }
    /* @hack type: flag_get_int, declared to return an int, where flag_get
     * would do */
    return flag_get_int(attachment_flags[index]);
}
