#include <mathf.h>
#include <ninja.h>
#include <nt_utl.h>
#include <sg_xpt.h>
#include <shinobi.h>

#include "chao/garden_save.h"
#include "chaostgkinder/labels.h"
#include "chaostgkinder/objects.h"
#include "chaostgkinder/rooms.h"
#include "chaostgkinder/widget.h"
#include "player/player.h"
#include "sound/adx.h"
#include "sound/sound.h"
#include "stage/stage.h"
#include "system/angle.h"
#include "system/pad.h"
#include "system/task.h"

#define CROSS_RED 0xffff0000
#define CROSS_INSET 3
#define BULLETIN_BOARD 7
#define BOARD_X 0.0F
#define BOARD_Z 88.3F
#define BROWSER_JUMP 10
#define REFUSED_SOUND 0x800a
#define DOOR_REACH 20.0F
#define ENTER_DELAY 30
#define LABEL_X 48
#define LABEL_Y 48
#define CONNECT_ID 1
#define BACK_ID 2

enum corridor_mode {
    CORRIDOR_DONE = -1,
    CORRIDOR_START = 0,
    CORRIDOR_WALK = 1,
    CORRIDOR_ENTER = 4,
    BOARD_OPEN = 5,
    BOARD_WAIT = 6,
    BOARD_SAVE = 7,
    BOARD_LAUNCH = 8,
    BOARD_CLOSE = 9
};

/* a slot's destination: the room and entrance its door leads to, and the
 * caption its label shows. slots 0 to 6 are the doors, and BULLETIN_BOARD the
 * board */
struct door_destination {
    Sint32 room, entrance, caption;
};

static const struct door_destination destinations[] = {
    {KINDER_HOUSE, HOUSE_FROM_CORRIDOR, KINDER_CAPTION_EXIT},
    {KINDER_BLACK_MARKET, 0, KINDER_CAPTION_BLACK_MARKET},
    {KINDER_CLASSROOM, 0, KINDER_CAPTION_CLASSROOM},
    {KINDER_HEALTH_CENTER, 0, KINDER_CAPTION_HEALTH_CENTER},
    {KINDER_LIBRARY, 0, KINDER_CAPTION_LIBRARY},
    {KINDER_PRINCIPAL, 0, KINDER_CAPTION_PRINCIPAL},
    {KINDER_PLAYGROUND, 0, KINDER_CAPTION_PLAYGROUND},
    {0, 0, KINDER_CAPTION_BULLETIN_BOARD},
};

static const struct kinder_rect focused[6] = {
#include "chaostgkinder/corridor/focused.inc"
};
static const struct kinder_rect unfocused[3] = {
#include "chaostgkinder/corridor/unfocused.inc"
};
static const struct kinder_rect disabled_focused[6] = {
#include "chaostgkinder/corridor/disabled_focused.inc"
};
static const struct kinder_rect disabled_unfocused[3] = {
#include "chaostgkinder/corridor/disabled_unfocused.inc"
};

static struct kinder_widget *
create_cross(const struct kinder_widget_desc *desc);
static int on_connect(void *owner, struct kinder_widget *widget, int event);
static int on_back(void *owner, struct kinder_widget *widget, int event);
static int on_board(void *owner, struct kinder_widget *widget, int event);
static void close_board(void);

static const struct kinder_label_desc board_message = {
#include "chaostgkinder/corridor/board_message.inc"
};
static const struct kinder_widget_desc board_text = {
#include "chaostgkinder/corridor/board_text.inc"
};
static const struct kinder_custom_desc cross_factory = {
#include "chaostgkinder/corridor/cross_factory.inc"
};
static const struct kinder_widget_desc disabled_cross = {
#include "chaostgkinder/corridor/disabled_cross.inc"
};
static const struct kinder_button_desc connect_button = {
#include "chaostgkinder/corridor/connect_button.inc"
};
static const struct kinder_widget_desc connect_widget = {
#include "chaostgkinder/corridor/connect_widget.inc"
};
static const struct kinder_button_desc back_button = {
#include "chaostgkinder/corridor/back_button.inc"
};
static const struct kinder_widget_desc back_widget = {
#include "chaostgkinder/corridor/back_widget.inc"
};
static const struct kinder_texture_desc panel_texture = {
#include "chaostgkinder/corridor/panel_texture.inc"
};
static const struct kinder_widget_desc board_panel = {
#include "chaostgkinder/corridor/board_panel.inc"
};
static const struct kinder_rect board_border[8] = {
#include "chaostgkinder/corridor/board_border.inc"
};

static void cross_destroy(struct kinder_widget *widget);
static void cross_step(struct kinder_widget *widget);
static void cross_draw(struct kinder_widget *widget, Sint32 x, Sint32 y,
                       Sint32 z);
static Sint32 cross_input(struct kinder_widget *widget, Uint32 key);
static void cross_focus(struct kinder_widget *widget, Sint32 focus);

static struct kinder_widget_ops cross_ops = {
    cross_destroy, cross_step, cross_draw, cross_input, cross_focus};
static struct kinder_container_desc board_container = {
#include "chaostgkinder/corridor/board_container.inc"
};
static struct kinder_widget_desc board_desc = {
#include "chaostgkinder/corridor/board_desc.inc"
};
static struct kinder_widget *board;

static void cross_destroy(struct kinder_widget *widget)
{
    kinder_widget_free(widget);
}

static void cross_step(struct kinder_widget *widget)
{
}

static void cross_draw(struct kinder_widget *widget, Sint32 x, Sint32 y,
                       Sint32 z)
{
    NJS_POLYGON_VTX vertices[4];
    Float depth;

    if (!widget->visible) {
        return;
    }
    x += widget->x;
    y += widget->y;
    z += widget->z;
    depth = KINDER_WIDGET_DEPTH(z);

#define CROSS_VERTEX(i, px, py)                                                \
    vertices[i].x = (px);                                                      \
    vertices[i].y = (py);                                                      \
    vertices[i].z = depth;                                                     \
    vertices[i].col = CROSS_RED
    CROSS_VERTEX(0, x + CROSS_INSET, y);
    CROSS_VERTEX(1, x, y + CROSS_INSET);
    CROSS_VERTEX(2, x + widget->width, y + widget->height - CROSS_INSET);
    CROSS_VERTEX(3, x + widget->width - CROSS_INSET, y + widget->height);
    njDrawPolygon(vertices, 4, FALSE);
    CROSS_VERTEX(0, x, y + widget->height - CROSS_INSET);
    CROSS_VERTEX(1, x + CROSS_INSET, y + widget->height);
    CROSS_VERTEX(2, x + widget->width - CROSS_INSET, y);
    CROSS_VERTEX(3, x + widget->width, y + CROSS_INSET);
#undef CROSS_VERTEX
    njDrawPolygon(vertices, 4, FALSE);
}

static Sint32 cross_input(struct kinder_widget *widget, Uint32 key)
{
    return FALSE;
}

static void cross_focus(struct kinder_widget *widget, Sint32 focus)
{
}

static struct kinder_widget *create_cross(const struct kinder_widget_desc *desc)
{
    struct kinder_widget *widget;

    /* @hack spelling: the assignment inside the test, where a statement of
     * its own would do */
    if ((widget = kinder_widget_alloc(desc)) != NULL) {
        widget->ops = &cross_ops;
    }
    return widget;
}

static int on_connect(void *owner, struct kinder_widget *widget, int event)
{
    int handled = FALSE;
    struct task *task = owner;

    switch (event) {
    case WIDGET_RIGHT:
        kinder_widget_select(widget->parent, BACK_ID);
        handled = TRUE;
        break;
    case WIDGET_ACCEPT:
        if (!kinder_network_info) {
            close_board();
            task->work->mode = BOARD_SAVE;
            pause_disable();
        } else {
            sound_play(REFUSED_SOUND, NULL, 0, 0);
        }
        handled = TRUE;
        break;
    }
    return handled;
}

static int on_back(void *owner, struct kinder_widget *widget, int event)
{
    int handled = FALSE;
    struct task *task = owner;

    switch (event) {
    case WIDGET_LEFT:
        kinder_widget_select(widget->parent, CONNECT_ID);
        handled = TRUE;
        break;
    case WIDGET_ACCEPT:
        close_board();
        task->work->mode = BOARD_CLOSE;
        handled = TRUE;
        break;
    }
    return handled;
}

static int on_board(void *owner, struct kinder_widget *widget, int event)
{
    int handled = FALSE;
    struct task *task = owner;

    /* @hack spelling: a switch of one case, where an if would do */
    switch (event) {
    case WIDGET_CANCEL:
        close_board();
        task->work->mode = BOARD_CLOSE;
        handled = TRUE;
        break;
    }
    return handled;
}

static void open_board(struct task *task)
{
    struct kinder_widget *widget;
    struct kinder_button *button;

    kinder_widget_textures_load();
    board = kinder_widget_create(&board_desc);
    board->owner = task;
    kinder_widget_add(board, &board_text);
    widget = kinder_widget_add(board, &connect_widget);
    widget->owner = task;
    /* @hack spelling: the empty arm before the else, where testing
     * kinder_network_info alone would do */
    if (!kinder_network_info) {
    } else {
        button = widget->state;
        button->focused = disabled_focused;
        button->unfocused = disabled_unfocused;
        kinder_widget_add(board, &disabled_cross);
    }
    widget = kinder_widget_add(board, &back_widget);
    widget->owner = task;
    kinder_widget_add(board, &board_panel);
    if (!kinder_network_info) {
        kinder_widget_select(board, CONNECT_ID);
    } else {
        kinder_widget_select(board, BACK_ID);
    }
    kinder_widget_push(board);
}

static void close_board(void)
{
    kinder_widget_remove(board);
    board->ops->destroy(board);
    kinder_widget_textures_release();
}

static void corridor_destroy(struct task *task)
{
}

static void corridor_draw(struct task *task)
{
}

/* @hack type: the label task kept in scl.x's word through a cast, where a
 * work record with a pointer field would do */
#define CORRIDOR_LABEL(work) (*(struct task **)&(work)->scl.x)

/* ang.y is the slot the player faces, or -1. ang.z is the slot whose caption
 * shows, and CORRIDOR_LABEL the task that shows it, or NULL. ang.x counts
 * the frames left before the corridor requests a door's room */
void kinder_corridor_init(struct task *task)
{
    struct task_work *work = task->work;
    struct task_work *player;
    struct task *door;
    struct task *label;
    struct task_work *door_work;
    Sint32 slot;
    Float dx;
    Float dz;
    Sint16 angle;
    Angle heading;
    const struct door_destination *destination;

    switch (work->mode) {
    case CORRIDOR_START:
        task->draw = corridor_draw;
        task->destructor = corridor_destroy;
        work->ang.y = -1;
        work->ang.z = -1;
        work->mode = CORRIDOR_WALK;
        break;
    case CORRIDOR_WALK:
        player = player_work[0];
        work->ang.y = -1;
        for (slot = 0; slot < BULLETIN_BOARD; ++slot) {
            door = kinder_door(slot);
            if (door) {
                door_work = door->work;
                dx = player->pos.x - door_work->pos.x;
                dz = player->pos.z - door_work->pos.z;
                if ((dx * dx) + (dz * dz) < DOOR_REACH * DOOR_REACH) {
                    heading = (3 * QUARTER_TURN) - player->ang.y;
                    angle = (Sint32)(atan2f(dx, dz) * (Float)ANGLE_PER_RADIAN);
                    angle = heading - angle;
                    if (angle > -QUARTER_TURN && angle < QUARTER_TURN) {
                        work->ang.y = slot;
                        if (player_pads[0]->press & (PDD_DGT_TA | PDD_DGT_TB)) {
                            pad_disable();
                            kinder_door_open(door);
                            work->ang.x = ENTER_DELAY;
                            work->mode = CORRIDOR_ENTER;
                        }
                    }
                    break;
                }
            }
        }
        if (work->ang.y < 0) {
            dx = player->pos.x - BOARD_X;
            dz = player->pos.z - BOARD_Z;
            if ((dx * dx) + (dz * dz) < DOOR_REACH * DOOR_REACH) {
                angle = QUARTER_TURN - player->ang.y;
                /* @hack spelling: & of the two bounds' tests, where && would
                 * do */
                if ((angle > -QUARTER_TURN) & (angle < QUARTER_TURN)) {
                    work->ang.y = BULLETIN_BOARD;
                    if (player_pads[0]->press & (PDD_DGT_TA | PDD_DGT_TB)) {
                        pad_disable();
                        work->mode = BOARD_OPEN;
                    }
                }
            }
        }
        if (work->ang.y != work->ang.z) {
            label = CORRIDOR_LABEL(work);
            if (label != NULL) {
                task_kill(label);
            }
            if (work->ang.y >= 0) {
                /* slot is the door the loop stopped at, or the board's. no
                 * door stands within twice DOOR_REACH of the board, so slot
                 * is the one ang.y holds */
                CORRIDOR_LABEL(work) = kinder_label_create(
                    task, destinations[slot].caption, LABEL_X, LABEL_Y);
            }
            work->ang.z = work->ang.y;
        }
        break;
    case CORRIDOR_ENTER:
        if (--work->ang.x == 0) {
            work->mode = CORRIDOR_DONE;
            destination = &destinations[work->ang.y];
            kinder_request_room(destination->room, destination->entrance);
        }
        break;
    case BOARD_OPEN:
        open_board(task);
        work->mode = BOARD_WAIT;
        break;
    case BOARD_WAIT:
        break;
    case BOARD_SAVE:
        garden_file_write();
        music_fade_out();
        work->mode = BOARD_LAUNCH;
        break;
    case BOARD_LAUNCH:
        switch (garden_file_state()) {
        case GARDEN_FILE_OK:
        case GARDEN_FILE_ABSENT:
        case GARDEN_FILE_ERROR:
            gdFsChangeDir("..");
            ntUtlSetJumpNumber(BROWSER_JUMP);
            ntLDBrowser();
        }
        break;
    case BOARD_CLOSE:
        pad_enable();
        work->mode = CORRIDOR_WALK;
        return;
    case CORRIDOR_DONE:
        break;
    }
}
