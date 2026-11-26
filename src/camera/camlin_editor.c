/* the debug editor of the camlin nodes, which a player's pad drives */
#include <ninja.h>
#include <sg_xpt.h>
#include <shinobi.h>

#include "camera/camedit.h"
#include "camera/camera.h"
#include "camera/camlin.h"
#include "camera/editor.h"
#include "camera/helpers.h"
#include "camera/stack.h"
#include "draw/lines.h"
#include "player/player.h"
#include "system/angle.h"
#include "system/pad.h"
#include "system/task.h"

/* the editor's pages, in the order Y steps through them, each named by the
 * title it prints. they move the held node's player_pos, link_cursor and
 * the held node's camera_pos */
#define PAGE_PLAYER_POINT 0
#define PAGE_FRIEND 1
#define PAGE_CAMERA_POINT 2
#define PAGES 3

/* the speeds X with B steps through in turn, how far a step moves a page's
 * point. the step and the draw tell the speeds apart by the bounds between
 * them. a speed below HIGH_FROM is NORMAL, below SUPER_FROM HIGH, below
 * MADNESS_FROM SUPER, and any other MADNESS */
#define SPEED_NORMAL 1.0F
#define SPEED_HIGH 5.0F
#define SPEED_SUPER 10.0F
#define SPEED_MADNESS 30.0F
#define HIGH_FROM 3.0F
#define SUPER_FROM 7.5F
#define MADNESS_FROM 20.0F

/* on the first two pages, right and left step the held node's size by
 * SIZE_STEP, down to SIZE_MIN. every page prints SIZE IGNORE while the size
 * is below SIZE_IGNORE_BELOW */
#define SIZE_STEP 1.0F
#define SIZE_MIN 1.0F
#define SIZE_IGNORE_BELOW 50.0F

/* how far above the held node's player_pos camlin_editor_open puts
 * link_cursor and the node's camera_pos. Start with X on their pages puts
 * them back at those heights */
#define LINK_CURSOR_HEIGHT 20.0F
#define CAMERA_HEIGHT 50.0F

/* the size of link_cursor's mark */
#define MARK_LINK_CURSOR 5.0F

/* the slot of the player's camera stack that holds the editor's mode */
#define EDITOR_SLOT (CAMERA_STACK_DEPTH - 1)

/* the held node's model draws unturned */
static struct angles camlin_editor_model_ang = {0, 0, 0};
/* the end colors of the held node's lines to the nodes it links, by player_pos
 * and by camera_pos. SHC refuses braces around each color, with error 2129 */
/* NOLINTBEGIN(clang-diagnostic-missing-braces) */
static NJS_COLOR camlin_editor_player_colors[2] = {RED, RED};
static NJS_COLOR camlin_editor_camera_colors[2] = {YELLOW, YELLOW};
/* NOLINTEND(clang-diagnostic-missing-braces) */

static int camlin_editor_page;
static Float camlin_editor_speed;

/* nothing calls these. none is static, since SHC leaves out a static
 * function that nothing calls.
 *
 * the editor edits camera_editor's held node for camedit_player, in that
 * player's table, CAMLIN_PLAYER_TABLE(camedit_player) */
/* NOLINTBEGIN(misc-use-internal-linkage) */
void camlin_editor_close(void);
/* put CAMERA_MODE_EDITOR in the player's EDITOR_SLOT. start the held node at
 * the player's pos, with no links and no next. its camera_pos starts
 * CAMERA_HEIGHT above that, and link_cursor LINK_CURSOR_HEIGHT above. the
 * node's other fields keep their values. the editor starts on PAGE_PLAYER_POINT
 * at SPEED_NORMAL, and forgets the next camlin_pick_up kept */
void camlin_editor_open(void);
/* a step of the editor by camedit_player's pad. Y turns the page, and the
 * pad moves the page's point. Start with A picks up a node, and Start with B
 * puts the held node down. the step sets camlin_camera_mask to the player's bit
 * alone */
void camlin_editor_step(void);
/* draw the held node where the current camera is camedit_player's. then
 * draw the current camera's table with every flag of camlin_draw. the held
 * node's sphere takes its whole size, where camlin_draw's take half */
void camlin_editor_draw(void);
/* NOLINTEND(misc-use-internal-linkage) */

void camlin_editor_close(void)
{
    camera_editor.camlin_open = FALSE;
}

void camlin_editor_open(void)
{
    int i;

    camera_set_mode(camedit_player, EDITOR_SLOT, CAMERA_MODE_EDITOR);
    camera_editor.camlin_open = TRUE;
    camera_editor.held.next = CAMLIN_NONE;
    camera_editor.held.player_pos = player_work[camedit_player]->pos;
    camera_editor.link_cursor = player_work[camedit_player]->pos;
    camera_editor.link_cursor.y += LINK_CURSOR_HEIGHT;
    camera_editor.held.camera_pos = player_work[camedit_player]->pos;
    camera_editor.held.camera_pos.y += CAMERA_HEIGHT;
    for (i = 0; i < CAMLIN_LINKS; i++) {
        camera_editor.held.links[i] = CAMLIN_NONE;
    }
    camlin_editor_page = PAGE_PLAYER_POINT;
    camlin_editor_speed = SPEED_NORMAL;
    camlin_forget_cut();
}

void camlin_editor_step(void)
{
    if (player_pads[camedit_player]->press & PDD_DGT_TY) {
        /* @hack implicit: the increment of camlin_editor_page and the store
         * of the remainder unsequenced, where camlin_editor_page++ and then
         * camlin_editor_page %= PAGES would do */
#ifdef NONMATCHING
        /* the other side's increment and store are unsequenced, which C
         * leaves undefined. this side steps the page and then takes the
         * remainder, as SHC's code does */
        camlin_editor_page = (camlin_editor_page + 1) % PAGES;
#else
        /* NOLINTBEGIN(clang-diagnostic-unsequenced) */
        camlin_editor_page = ++camlin_editor_page % PAGES;
        /* NOLINTEND(clang-diagnostic-unsequenced) */
#endif
        camera_set_adjust(camedit_player, EDITOR_SLOT, CAMERA_ADJUST_RELATIVE1);
    }
    camlin_camera_mask = 1 << camedit_player;
    switch (camlin_editor_page) {
    case PAGE_PLAYER_POINT:
        if ((player_pads[camedit_player]->on & PDD_DGT_ST) &&
            (player_pads[camedit_player]->press & PDD_DGT_TX)) {
            /* @bug logic: puts the held node at player 0's position, where
             * camlin_editor_open puts it at the position of the player who
             * edits (MOV.L @R0,R3 at 0x8c0ae2a8 reads player_work[0]). no
             * code in the game's images calls camlin_editor_step */
            camera_editor.held.player_pos = player_work[0]->pos;
        }
        point_move_by_pad(&camera_editor.held.player_pos, camlin_editor_speed,
                          camedit_player, TRUE);
        camera_editor.cursor = camera_editor.held.player_pos;
        njPrintC(NJM_LOCATION(2, 3), "PLAYER POINT SET");
        if (player_pads[camedit_player]->on & PDD_DGT_KR) {
            camera_editor.held.size += SIZE_STEP;
        } else if (player_pads[camedit_player]->on & PDD_DGT_KL) {
            camera_editor.held.size -= SIZE_STEP;
        }
        if (camera_editor.held.size < SIZE_MIN) {
            camera_editor.held.size = SIZE_MIN;
        }
        break;
    case PAGE_FRIEND:
        if ((player_pads[camedit_player]->on & PDD_DGT_ST) &&
            (player_pads[camedit_player]->press & PDD_DGT_TX)) {
            camera_editor.link_cursor = camera_editor.held.player_pos;
            camera_editor.link_cursor.y += LINK_CURSOR_HEIGHT;
        }
        point_move_by_pad(&camera_editor.link_cursor, camlin_editor_speed,
                          camedit_player, TRUE);
        camera_editor.cursor = camera_editor.link_cursor;
        if (!(player_pads[camedit_player]->on & PDD_DGT_ST) &&
            (player_pads[camedit_player]->press & PDD_DGT_TA)) {
            camlin_unlink_nearest(CAMLIN_PLAYER_TABLE(camedit_player));
            camera_set_adjust(camedit_player, EDITOR_SLOT,
                              CAMERA_ADJUST_RELATIVE1);
            camera_editor.held.next = CAMLIN_NONE;
        }
        if (!(player_pads[camedit_player]->on & PDD_DGT_ST) &&
            (player_pads[camedit_player]->press & PDD_DGT_TB)) {
            /* @bug logic: takes camlin_found for the held node's next,
             * whatever camlin_link_nearest returns. the JSR to it at
             * 0x8c0ae440 leaves R0, its result, unread, and MOV.L @R2,R5 at
             * 0x8c0ae468 reads camlin_found. where no node lies in reach,
             * camlin_found holds what an earlier search left. where
             * camlin_link_nearest refuses the node it finds, next can name a
             * node the held node doesn't link. no code in the game's images
             * calls camlin_editor_step */
            camlin_link_nearest(CAMLIN_PLAYER_TABLE(camedit_player));
            camera_set_adjust(camedit_player, EDITOR_SLOT,
                              CAMERA_ADJUST_RELATIVE1);
            if ((player_pads[camedit_player]->on & PDD_DGT_KD) ||
                (player_pads[camedit_player]->on & PDD_DGT_KU)) {
                if (!camlin_has_prev(CAMLIN_PLAYER_TABLE(camedit_player),
                                     camlin_found)) {
                    camera_editor.held.next = camlin_found;
                } else {
                    njPrintC(NJM_LOCATION(10, 10), "NO!!!!!!!!!");
                    camera_editor.held.next = CAMLIN_NONE;
                }
                if (player_pads[camedit_player]->on & PDD_DGT_KD) {
                    camera_editor.held.highlight = 0;
                } else {
                    camera_editor.held.highlight = 1;
                }
            } else {
                camera_editor.held.next = CAMLIN_NONE;
            }
        }
        njPrintC(NJM_LOCATION(2, 3), "FRIEND SET");
        if (player_pads[camedit_player]->on & PDD_DGT_KR) {
            camera_editor.held.size += SIZE_STEP;
        } else if (player_pads[camedit_player]->on & PDD_DGT_KL) {
            camera_editor.held.size -= SIZE_STEP;
        }
        if (camera_editor.held.size < SIZE_MIN) {
            camera_editor.held.size = SIZE_MIN;
        }
        break;
    case PAGE_CAMERA_POINT:
        if ((player_pads[camedit_player]->on & PDD_DGT_ST) &&
            (player_pads[camedit_player]->press & PDD_DGT_TX)) {
            camera_editor.held.camera_pos = camera_editor.held.player_pos;
            camera_editor.held.camera_pos.y += CAMERA_HEIGHT;
        }
        point_move_by_pad(&camera_editor.held.camera_pos, camlin_editor_speed,
                          camedit_player, TRUE);
        camera_editor.cursor = camera_editor.held.camera_pos;
        njPrintC(NJM_LOCATION(2, 3), "CAMERA POINT SET");
        break;
    }
    if (camera_editor.held.size < SIZE_IGNORE_BELOW) {
        njPrintC(NJM_LOCATION(2, 5), "SIZE IGNORE");
    }
    if ((player_pads[camedit_player]->on & PDD_DGT_ST) &&
        (player_pads[camedit_player]->press & PDD_DGT_TA)) {
        camlin_pick_up(CAMLIN_PLAYER_TABLE(camedit_player));
        camlin_editor_page = PAGE_PLAYER_POINT;
        camera_set_adjust(camedit_player, EDITOR_SLOT, CAMERA_ADJUST_RELATIVE1);
    }
    if ((player_pads[camedit_player]->on & PDD_DGT_ST) &&
        (player_pads[camedit_player]->press & PDD_DGT_TB)) {
        camlin_put_held(CAMLIN_PLAYER_TABLE(camedit_player));
        camlin_editor_page = PAGE_PLAYER_POINT;
        camera_set_adjust(camedit_player, EDITOR_SLOT, CAMERA_ADJUST_RELATIVE1);
    }
    if ((player_pads[camedit_player]->on & PDD_DGT_TX) &&
        (player_pads[camedit_player]->press & PDD_DGT_TB)) {
        if (camlin_editor_speed < HIGH_FROM) {
            camlin_editor_speed = SPEED_HIGH;
        } else if (camlin_editor_speed < SUPER_FROM) {
            camlin_editor_speed = SPEED_SUPER;
        } else if (camlin_editor_speed < MADNESS_FROM) {
            camlin_editor_speed = SPEED_MADNESS;
        } else {
            camlin_editor_speed = SPEED_NORMAL;
        }
    }
}

static void camlin_editor_draw_held(void)
{
    struct camlin_node *nodes;
    NJS_VECTOR scl;
    int i;
    int linked;

    nodes = camlin_tables[CAMLIN_PLAYER_TABLE(camedit_player)];
    scl.x = camera_editor.held.size;
    scl.y = camera_editor.held.size;
    scl.z = camera_editor.held.size;
    debug_draw_model(&camera_editor.held.player_pos, &camlin_editor_model_ang,
                     &scl, DEBUG_MODEL_SPHERE);
    if (camera_editor.held.next >= 0) {
        debug_draw_mark(&camera_editor.held.player_pos, CAMLIN_MARK_WITH_NEXT);
    } else {
        debug_draw_mark(&camera_editor.held.player_pos,
                        CAMLIN_MARK_WITHOUT_NEXT);
    }
    debug_draw_mark(&camera_editor.link_cursor, MARK_LINK_CURSOR);
    for (i = 0; i < CAMLIN_LINKS; i++) {
        if (camera_editor.held.links[i] >= 0) {
            linked = camera_editor.held.links[i];
            line_draw_colored(
                camera_editor.held.player_pos.x,
                camera_editor.held.player_pos.y,
                camera_editor.held.player_pos.z,
                &camlin_editor_player_colors[0], nodes[linked].player_pos.x,
                nodes[linked].player_pos.y, nodes[linked].player_pos.z,
                &camlin_editor_player_colors[1]);
        }
    }
    if (camera_editor.held.next >= 0) {
        debug_draw_mark(&camera_editor.held.camera_pos, CAMLIN_MARK_WITH_NEXT);
    } else {
        debug_draw_mark(&camera_editor.held.camera_pos,
                        CAMLIN_MARK_WITHOUT_NEXT);
    }
    for (i = 0; i < CAMLIN_LINKS; i++) {
        if (camera_editor.held.links[i] >= 0) {
            linked = camera_editor.held.links[i];
            line_draw_colored(
                camera_editor.held.camera_pos.x,
                camera_editor.held.camera_pos.y,
                camera_editor.held.camera_pos.z,
                &camlin_editor_camera_colors[0], nodes[linked].camera_pos.x,
                nodes[linked].camera_pos.y, nodes[linked].camera_pos.z,
                &camlin_editor_camera_colors[1]);
        }
    }
    if (camlin_editor_speed < HIGH_FROM) {
        njPrintC(NJM_LOCATION(25, 3), "NORMAL SPD");
    } else if (camlin_editor_speed < SUPER_FROM) {
        njPrintC(NJM_LOCATION(25, 3), "HIGH SPD");
    } else if (camlin_editor_speed < MADNESS_FROM) {
        njPrintC(NJM_LOCATION(25, 3), "SUPER SPD");
    } else {
        njPrintC(NJM_LOCATION(25, 3), "MADNESS SPD");
    }
}

void camlin_editor_draw(void)
{
    if (camedit_player == camera_current_index) {
        camlin_editor_draw_held();
    }
    camlin_draw(CAMLIN_PLAYER_TABLE(camera_current_index),
                CAMLIN_DRAW_PLAYER_LINKS | CAMLIN_DRAW_CAMERA_LINKS |
                    CAMLIN_DRAW_PLAYER | CAMLIN_DRAW_CAMERA);
}
