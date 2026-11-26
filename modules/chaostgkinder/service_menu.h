/* the boxes, panels, buttons, frames and pages the kindergarten's menus
 * draw from kinder_service_textures. a create function makes a task, a
 * child of parent, that draws copies of the records it takes. where parent
 * is NULL or an allocation fails it makes none. one that returns the task
 * then returns NULL */
#ifndef CHAOSTGKINDER_SERVICE_MENU_H
#define CHAOSTGKINDER_SERVICE_MENU_H
#include <ninja.h>

struct task;
extern NJS_TEXLIST kinder_service_textures;

enum service_palette { SERVICE_PINK, SERVICE_BLUE, SERVICE_GOLD };

/* the sounds two or more of the lesson, the checkup and the advice play. a
 * menu plays SERVICE_ACCEPT_SOUND as it takes a choice, and
 * SERVICE_BACK_SOUND as it steps back. the advice's list plays
 * SERVICE_ACCEPT_SOUND again as its rows show. SERVICE_RESULT_SOUND marks a
 * checkup's result, and a Chao a lesson enrolls or gives back.
 * SERVICE_LEAVE_SOUND plays as a service closes */
#define SERVICE_CURSOR_SOUND 0x8000
#define SERVICE_BACK_SOUND 0x100a
#define SERVICE_ACCEPT_SOUND 0x1007
#define SERVICE_RESULT_SOUND 0x1005
#define SERVICE_LEAVE_SOUND 0x102a

/* a panel's decoration: four copies of one cell of the service atlas, drawn
 * at scale. a translucent cell draws in color. the copies stand at the box's
 * corners under SERVICE_DECORATE_CORNERS. otherwise each copy's top left
 * corner stands at left or right, and at top or bottom */
struct service_decoration {
    Sint32 cell;
    Uint32 color;
    Float scale;
    Float left, top, right, bottom;
};

/* a button's color while selected, and selection, which points at the id of
 * the button to select. a button under SERVICE_TRACK_SELECTION follows it. a
 * NULL selection turns the tracking off. the id it points at is signed, so a
 * button whose id passes 127 never matches it */
struct service_choice {
    Uint32 selected_color;
    const Sint8 *selection;
};

/* a box of width by height screen pixels, its top left at pos and its depth
 * pos.z. a panel or a button draws it in color. a panel reads
 * detail.decoration, and a button detail.choice. storage only sizes detail,
 * which makes a box 108 bytes */
struct service_box {
    NJS_POINT3 pos;
    Uint32 color;
    Float width, height;
    Uint32 flags;
    union {
        struct service_decoration decoration;
        struct service_choice choice;
        Uint8 storage[80];
    } detail;
};
/* service_box.flags. a panel reads SERVICE_DECORATED and
 * SERVICE_DECORATE_CORNERS, and a button SERVICE_TRACK_SELECTION */
#define SERVICE_DECORATED 1
#define SERVICE_DECORATE_CORNERS 2
#define SERVICE_TRACK_SELECTION 1

/* draw box in the atlas's box cells, border pixels wider on each side. source
 * gives the scale, about the box's center. a NULL tlist or tanim in source
 * takes the service atlas's. a NULL source draws at scale 1 */
void service_box_draw(const NJS_SPRITE *source, const struct service_box *box,
                      int border, Float depth);
/* set the constant material to argb, whose top byte is alpha */
void service_set_color(Uint32 argb);
/* a panel: box, with decoration where box has SERVICE_DECORATED. decoration
 * must not be NULL, whatever box's flags */
void service_panel_create(struct task *parent, const struct service_box *box,
                          const struct service_decoration *decoration);
/* a button with work->id id. while selected it draws in
 * choice->selected_color with a white border. it pulses where
 * SERVICE_TRACK_SELECTION selects it. a NULL choice leaves box's detail as
 * it is */
struct task *service_button_create(struct task *parent, int id,
                                   const struct service_box *box,
                                   const struct service_choice *choice);
/* in the lesson, the checkup and the advice, a column's buttons stand
 * SERVICE_ROW_GAP apart. each one's label stands SERVICE_LABEL_DEPTH in front
 * of it, and one sized to its button is SERVICE_LABEL_INSET narrower. the
 * lesson stands the labels of its title's panel and its badge
 * SERVICE_LABEL_DEPTH in front of them too */
#define SERVICE_LABEL_DEPTH 0.5F
#define SERVICE_LABEL_INSET 2.0F
#define SERVICE_ROW_GAP 10.0F
/* select a button, or deselect it and end its pulse */
void service_button_select(struct task *task);
void service_button_deselect(struct task *task);
void service_arrow_draw(Float x, Float y, Float depth);
/* a button with text centered on it. choice must not be NULL */
void service_text_button(struct task *parent, int id,
                         const struct service_box *box,
                         const struct service_choice *choice, const char *text);
/* a frame over box's rectangle, in palette's colors and tiles. its edges lie
 * on box's edges, and its tiles fill it */
struct task *service_frame_create(struct task *parent, Uint8 palette,
                                  const struct service_box *box);
/* box as a page. its white panel and its tab stand at the same place on
 * screen, whatever box says */
struct task *service_page_create(struct task *parent,
                                 const struct service_box *box);

#endif
