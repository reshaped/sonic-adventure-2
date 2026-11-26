/* the items' icons: the icon over an item box, and a row of icons on a
 * player's screen. the row shows the items the player has just got */
#include <ninja.h>
#include <sg_xpt.h>
#include <shinobi.h>

#include "camera/camera.h"
#include "player/player.h"
#include "shared/item_box.h"
#include "shared/item_icon.h"
#include "system/game_state.h"
#include "system/task.h"
#ifdef NONMATCHING
/* texture_slot_loaded, by which this side's draws test the icon's texture */
#include "system/texture.h"
#endif

#include "shared/item_icon/text.inc"

#define ITEM_KINDS (ITEM_1UP + 1)
/* the textures item_icon_texlist names, as item_icon.h lays them out. a last
 * copy of Sonic's 1-up follows at ITEM_1UP + 8, which no PLAYER_ character
 * selects */
#define ITEM_ICON_TEXTURES 19
#define QUAD_VERTICES 4
#define OPAQUE_WHITE 0xffffffff

#define ROW_PLAYERS 2
#define ROW_ICONS 20
#define ROW_LEVEL 3

/* the row's layout, in pixels on a screen 640 wide. a new icon enters at x
 * START_X, and slides right SLIDE_SPEED a step to its place. the icons lie
 * ICON_SPACING apart, centered on x ROW_X, with their centers at y ROW_Y */
#define START_X (-60.0F)
#define SLIDE_SPEED 30.0F
#define ICON_SPACING 80.0F
#define ROW_X 320.0F
#define ROW_Y 370.0F
/* the z of each icon's corners, as njDrawTexture takes it */
#define ICON_DEPTH 0.952381F
/* a new icon is ICON_SIZE across. where a new icon cuts a shrink short, the
 * shrunken icons grow back by GROWTH a step */
#define ICON_SIZE 64.0F
#define GROWTH 0.2F
/* a row whose icons are all in place holds until its timer passes
 * HOLD_STEPS. it then shrinks away over HOLD_STEPS steps */
#define HOLD_STEPS 60
/* with two cameras, each player's screen is SPLIT_WIDTH wide, side by side.
 * the draw scales a row by 1 / camera_active_count to fit */
#define SPLIT_WIDTH 320.0F

/* a row's modes, in its work's mode. a row in MODE_END destroys its task on
 * its next step */
enum item_icon_row_mode { MODE_END, MODE_SLIDE, MODE_HOLD, MODE_SHRINK };

/* an icon of a row */
struct item_icon {
    Sint32 kind;
    Float x; /* its center's x */
    Float size; /* how many pixels it is across */
};

/* a row's record, in its task's motion */
struct item_icon_row {
    Sint32 unk_00;
    Sint32 count;
    /* while the row holds, timer counts the steps it has held. while the row
     * shrinks, timer counts down the steps left */
    Uint32 timer;
    struct item_icon icons[ROW_ICONS];
};

#define ROW(task) ((struct item_icon_row *)(task)->motion.p)
/* the player whose screen the row lies on, in its work's tick */
#define ROW_PLAYER(task) ((task)->work->tick)

static void item_icon_row_destroy(struct task *task);
static void item_icon_row_step(struct task *task);
static void item_icon_row_slide(struct task *task);
static void item_icon_row_hold(struct task *task);
static void item_icon_row_shrink(struct task *task);
static void item_icon_row_draw(struct task *task);

static NJS_TEXNAME item_icon_texnames[ITEM_ICON_TEXTURES] = {
#include "shared/item_icon/item_icon_texnames.inc"
};
NJS_TEXLIST item_icon_texlist = {
#include "shared/item_icon/item_icon_texlist.inc"
};
/* each kind's name, which nothing reads */
static const char *item_icon_names[ITEM_KINDS] = {
#include "shared/item_icon/item_icon_names.inc"
};
/* a square 1 on a side, centered on the origin in the plane of x and y */
static NJS_TEXTURE_VTX item_icon_square[QUAD_VERTICES] = {
#include "shared/item_icon/item_icon_square.inc"
};
/* each player's row, or null while the player has none */
static struct task *item_icon_rows[ROW_PLAYERS] = {NULL, NULL};

int item_icon_draw(int kind, NJS_POINT3 *pos, Angle ang, Float scale)
{
    int character;

    /* @hack spelling: else if after the return, where if alone would do */
    if (kind >= ITEM_KINDS) {
        return 0;
    } else if (kind == ITEM_1UP) {
        character = player_character(camera_current_index);
        if (character >= 0) {
            kind += character;
        }
    }
#ifdef NONMATCHING
    /* this side draws no icon whose texture isn't loaded */
    if (!texture_slot_loaded(&item_icon_texlist, (Uint32)kind)) {
        return 0;
    }
#endif
    njColorBlendingMode(NJD_SOURCE_COLOR, NJD_COLOR_BLENDING_SRCALPHA);
    njColorBlendingMode(NJD_DESTINATION_COLOR, NJD_COLOR_BLENDING_INVSRCALPHA);
    njSetTexture(&item_icon_texlist);
    njSetTextureNum(kind);
    njPushMatrix(NULL);
    njTranslateV(NULL, pos);
    njRotateY(NULL, ang);
    njScale(NULL, scale, scale, scale);
    njDrawTexture3DEx(item_icon_square, QUAD_VERTICES, TRUE);
    njPopMatrix(1);
    return 1;
}

/* draw the icon of kind on the screen, as item_icon_draw does in the world.
 * the icon is a square whose side is size pixels. its center is pos's x and
 * y, at depth pos->z. return 1, or 0 for a kind above ITEM_1UP, which draws
 * nothing */
static int item_icon_draw_2d(int kind, NJS_POINT3 *pos, Float size)
{
    NJS_TEXTURE_VTX vtx[QUAD_VERTICES];
    int character;

    /* @hack spelling: the draw inside an else after the return, where the
     * statements after the if would do */
    if (kind >= ITEM_KINDS) {
        return 0;
    } else {
        /* @hack spelling: the first corner set in a do-while that runs once,
         * where its assignments alone would do */
        do {
            vtx[0].x = pos->x - (size * 0.5F);
            vtx[0].y = pos->y - (size * 0.5F);
            vtx[0].z = pos->z;
            vtx[0].u = 0.0F;
            vtx[0].v = 0.0F;
            vtx[0].col = OPAQUE_WHITE;
        } while (0);
        vtx[1].x = pos->x - (size * 0.5F);
        vtx[1].y = pos->y + (size * 0.5F);
        vtx[1].z = pos->z;
        vtx[1].u = 0.0F;
        vtx[1].v = 1.0F;
        vtx[1].col = OPAQUE_WHITE;
        vtx[2].x = pos->x + (size * 0.5F);
        vtx[2].y = pos->y - (size * 0.5F);
        vtx[2].z = pos->z;
        vtx[2].u = 1.0F;
        vtx[2].v = 0.0F;
        vtx[2].col = OPAQUE_WHITE;
        vtx[3].x = pos->x + (size * 0.5F);
        vtx[3].y = pos->y + (size * 0.5F);
        vtx[3].z = pos->z;
        vtx[3].u = 1.0F;
        vtx[3].v = 1.0F;
        vtx[3].col = OPAQUE_WHITE;
        if (kind == ITEM_1UP) {
            character = player_character(camera_current_index);
            if (character >= 0) {
                kind += character;
            }
        }
#ifdef NONMATCHING
        /* this side draws no icon whose texture isn't loaded */
        if (!texture_slot_loaded(&item_icon_texlist, (Uint32)kind)) {
            return 0;
        }
#endif
        njColorBlendingMode(NJD_SOURCE_COLOR, NJD_COLOR_BLENDING_SRCALPHA);
        njColorBlendingMode(NJD_DESTINATION_COLOR,
                            NJD_COLOR_BLENDING_INVSRCALPHA);
        njSetTexture(&item_icon_texlist);
        njSetTextureNum(kind);
        njDrawTexture(vtx, QUAD_VERTICES, njGetTextureNumG(), TRUE);
    }
    return 1;
}

/* create a row task with no icons, in MODE_END. return it, or null where the
 * heap can't give the task or its record */
static struct task *item_icon_row_create(void)
{
    struct task *task;

    task =
        task_create(TASK_WORK, ROW_LEVEL, item_icon_row_step, "itemBoxManager");
    if (task != NULL) {
        task->motion.p = syCalloc(1, sizeof(struct item_icon_row));
        if (task->motion.p != NULL) {
            task->draw = item_icon_row_draw;
            task->destructor = item_icon_row_destroy;
            ROW(task)->count = 0;
            task->work->mode = MODE_END;
        } else {
            task_destroy(task);
            task = NULL;
        }
    }
    return task;
}

static void item_icon_row_destroy(struct task *task)
{
    int i;

    for (i = 0; i < ROW_PLAYERS; i++) {
        if (task == item_icon_rows[i]) {
            item_icon_rows[i] = NULL;
        }
    }
    syFree(task->motion.p);
    task->motion.p = NULL;
}

/* item_icon_row_add for player 0. nothing calls item_icon_show_player0. it
 * isn't static, since SHC leaves out a static function that nothing calls */
/* NOLINTBEGIN(misc-use-internal-linkage) */
void item_icon_show_player0(int kind)
{
    item_icon_row_add(0, kind);
}
/* NOLINTEND(misc-use-internal-linkage) */

void item_icon_row_add(int player, int kind)
{
    struct task *task;

    if (kind != ITEM_EMPTY && player >= 0 && player < ROW_PLAYERS &&
        game_state.unk_0b) {
        if (item_icon_rows[player] == NULL) {
            item_icon_rows[player] = item_icon_row_create();
        }
        /* @bug memory: reads the row's work through null where
         * item_icon_row_create returns null. its result is stored untested
         * (at 0x2c8 of the object's code), and read for the work at once (at
         * 0x2d4 of the object's code) */
#ifdef NONMATCHING
        /* this side repairs the tag above: where the row can't be made, the
         * icon isn't shown */
        if (item_icon_rows[player] == NULL) {
            return;
        }
#endif
        task = item_icon_rows[player];
        ROW_PLAYER(task) = player;
        ROW(task)->icons[ROW(task)->count].kind = kind;
        ROW(task)->icons[ROW(task)->count].x = START_X;
        ROW(task)->icons[ROW(task)->count].size = ICON_SIZE;
        ROW(task)->count++;
        task->work->mode = MODE_SLIDE;
    }
}

static void item_icon_row_step(struct task *task)
{
    switch (task->work->mode) {
    case MODE_END:
        task_destroy(task);
        break;
    case MODE_SLIDE:
        item_icon_row_slide(task);
        break;
    case MODE_HOLD:
        item_icon_row_hold(task);
        break;
    case MODE_SHRINK:
        item_icon_row_shrink(task);
        break;
    }
}

static void item_icon_row_slide(struct task *task)
{
    int i;
    int arrived;
    Float place;

    arrived = 0;
    for (i = 0; i < ROW(task)->count; i++) {
        ROW(task)->icons[i].x += SLIDE_SPEED;
        place = ((ROW(task)->count - 1) * (ICON_SPACING / 2.0F)) -
                (i * ICON_SPACING) + ROW_X;
        if (ROW(task)->icons[i].x >= place) {
            ROW(task)->icons[i].x = place;
            arrived++;
        }
        ROW(task)->icons[i].size += GROWTH;
        if (ROW(task)->icons[i].size > ICON_SIZE) {
            ROW(task)->icons[i].size = ICON_SIZE;
        }
    }
    if (arrived == ROW(task)->count) {
        task->work->mode = MODE_HOLD;
        ROW(task)->timer = 0;
    }
}

static void item_icon_row_hold(struct task *task)
{
    int i;

    for (i = 0; i < ROW(task)->count; i++) {
        ROW(task)->icons[i].size += GROWTH;
        if (ROW(task)->icons[i].size > ICON_SIZE) {
            ROW(task)->icons[i].size = ICON_SIZE;
        }
    }
    ROW(task)->timer++;
    if (ROW(task)->timer > HOLD_STEPS) {
        ROW(task)->timer = HOLD_STEPS;
        task->work->mode = MODE_SHRINK;
    }
}

static void item_icon_row_shrink(struct task *task)
{
    int i;

    for (i = 0; i < ROW(task)->count; i++) {
        ROW(task)->icons[i].size = ROW(task)->timer * (ICON_SIZE / HOLD_STEPS);
    }
    ROW(task)->timer--;
    if (ROW(task)->timer == 0) {
        ROW(task)->count = 0;
        task->work->mode = MODE_END;
    }
}

static void item_icon_row_draw(struct task *task)
{
    int i;
    NJS_POINT3 pos;

    if (camera_active_count > 0 && ROW_PLAYER(task) == camera_current_index) {
        pos.y = ROW_Y;
        pos.z = ICON_DEPTH;
        for (i = 0; i < ROW(task)->count; i++) {
            pos.x = (ROW(task)->icons[i].x / camera_active_count) +
                    (SPLIT_WIDTH * camera_current_index);
            item_icon_draw_2d(ROW(task)->icons[i].kind, &pos,
                              ROW(task)->icons[i].size / camera_active_count);
        }
    }
}
