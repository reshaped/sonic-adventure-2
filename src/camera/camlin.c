/* a stage's camlin nodes: the paths SPACE follows, the camlin editor's
 * edits to them, and their draw */
#include <ninja.h>
#include <sg_xpt.h>

/* a next that camlin_pick_up emptied, and the value it held, for
 * camlin_put_held to put back */
struct camlin_cut {
    Sint16 value;
    Sint16 *next; /* null when there is none to put back */
};

/* camlin_parents[n] is the node from which the last camlin_search reached
 * node n. the start holds CAMLIN_START, which is no node's index, and a
 * node not reached holds CAMLIN_NONE */
#define CAMLIN_SEARCH_NODES 512
#define CAMLIN_START 999
/* @hack spelling: camlin_parents and camlin_cut defined ahead of the camera
 * headers, not after them */
static Sint16 camlin_parents[CAMLIN_SEARCH_NODES];
static struct camlin_cut camlin_cut;

#include "camera/camedit.h"
#include "camera/camera.h"
#include "camera/camlin.h"
#include "camera/editor.h"
#include "draw/lines.h"
#include "system/angle.h"
#include "system/areas.h"
#include "system/curve.h"
#include "system/game_state.h"
#include "system/vector.h"

/* the most nodes a frontier of camlin_search holds, before its CAMLIN_NONE */
#define FRONTIER_NODES 31

/* the fewest entries of a path camlin_trace takes: three nodes, with the
 * first and last twice */
#define TRACE_LEAST 5

/* the size of camlin_draw's moving mark */
#define MARK_MOVING 8.0F
/* the room DRAW_MOVING_MARK gives camlin_trace for a path */
#define MARK_PATH_ROOM 64

#define BLUE 0xff0000ff
#define MAGENTA 0xffff00ff

/* camlin_draw's spheres draw unturned */
static const struct angles camlin_model_ang = {0, 0, 0};

/* @bug memory: walks of tables 1 and 3 read a 342nd entry, and
 * camlin_put_held can write it. a table's 16 KB hold 341 entries of 48
 * bytes, with 16 bytes over, so a 342nd entry runs 32 bytes past the table.
 * a walk takes every entry that starts below the table's bound (CMP/HS
 * R1,R2 at 0x8c0aed26). its read of that entry's used lands 30 bytes past
 * the table (MOV.B @(R0,R3),R1 at 0x8c0aece4). for table 1 that byte lies in
 * table 3's first node, and for table 3 past CAMLIN_AREA_END. where the byte
 * isn't 0, the walks take the entry for a node. where it is 0 and the other
 * 341 entries are used, camlin_find_free gives it to camlin_put_held (TST
 * R1,R1 at 0x8c0aec4a). the copy of the held node then writes all 32 bytes
 * past the table (MOV.L R1,@(16,R3) at 0x8c0aef1a, and the stores after
 * it) */
struct camlin_node *camlin_tables[CAMLIN_TABLES + 1] = {
    NULL,
    (struct camlin_node *)CAMLIN_AREA_0,
    (struct camlin_node *)CAMLIN_AREA_1,
    (struct camlin_node *)CAMLIN_AREA_1,
    (struct camlin_node *)CAMLIN_AREA_END,
    (struct camlin_node *)CAMLIN_AREA_END,
};

/* the colors at the ends of camlin_draw's lines: player path links, camera
 * path links, and other camera links. SHC refuses braces around each color,
 * with error 2129 */
/* NOLINTBEGIN(clang-diagnostic-missing-braces) */
static NJS_COLOR camlin_player_path_colors[2] = {GREEN, GREEN};
static NJS_COLOR camlin_camera_path_colors[2] = {YELLOW, YELLOW};
static NJS_COLOR camlin_camera_link_colors[2] = {BLUE, BLUE};
/* NOLINTEND(clang-diagnostic-missing-braces) */

int camlin_found;
/* camlin_search's two frontiers, each ended by CAMLIN_NONE. camlin_frontier
 * picks the one the last step filled */
static Sint16 camlin_frontiers[2][FRONTIER_NODES + 1];
static int camlin_frontier;

/* the first free entry of table, or null for a full table. camlin_found
 * gets its index */
static struct camlin_node *camlin_find_free(int table)
{
    struct camlin_node *entries;
    int count;
    int i;

    entries = camlin_tables[table];
    /* @hack spelling: the entries counted by a loop of their own before the
     * search, where one loop that tests the bound as it searches would do */
    for (count = 0; &entries[count] < camlin_tables[table + 1]; count++) {
    }
    for (i = 0; i < count; i++) {
        if (!entries[i].used) {
            camlin_found = i;
            return &entries[i];
        }
    }
    return NULL;
}

static void camlin_cut_next(Sint16 *next)
{
    camlin_cut.next = next;
    if (next != NULL) {
        camlin_cut.value = *next;
        *next = CAMLIN_NONE;
    }
}

static void camlin_restore_cut(void)
{
    if (camlin_cut.next != NULL) {
        *camlin_cut.next = camlin_cut.value;
    }
    camlin_cut.next = NULL;
}

void camlin_forget_cut(void)
{
    camlin_cut.next = NULL;
}

struct camlin_node *camlin_nearest(int table, NJS_POINT3 *pos)
{
    struct camlin_node *nodes;
    struct camlin_node *nearest;
    Float least;
    Float distance;
    int i;

    nodes = camlin_tables[table];
    least = CAMERA_NEAREST_REACH;
    nearest = NULL;
#ifdef NONMATCHING
    /* this side repairs the tag on camlin_tables: the walk takes only the
     * entries that end by the table's bound */
    for (i = 0; &nodes[i + 1] <= camlin_tables[table + 1]; i++) {
#else
    for (i = 0; &nodes[i] < camlin_tables[table + 1]; i++) {
#endif
        if (nodes[i].used) {
            distance = njDistanceP2P(pos, &nodes[i].player_pos);
            if (distance < least) {
                least = distance;
                nearest = &nodes[i];
                camlin_found = i;
            }
        }
    }
    return nearest;
}

struct camlin_node *camlin_nearest_camera(int table, NJS_POINT3 *pos)
{
    struct camlin_node *nodes;
    struct camlin_node *nearest;
    Float least;
    Float distance;
    int i;

    nodes = camlin_tables[table];
    least = CAMERA_NEAREST_REACH;
    nearest = NULL;
#ifdef NONMATCHING
    /* this side repairs the tag on camlin_tables, as camlin_nearest does */
    for (i = 0; &nodes[i + 1] <= camlin_tables[table + 1]; i++) {
#else
    for (i = 0; &nodes[i] < camlin_tables[table + 1]; i++) {
#endif
        if (nodes[i].used) {
            distance = njDistanceP2P(pos, &nodes[i].camera_pos);
            if (distance < least) {
                least = distance;
                nearest = &nodes[i];
                camlin_found = i;
            }
        }
    }
    return nearest;
}

Bool camlin_put_held(int table)
{
    struct camlin_node *nodes;
    struct camlin_node *entry;
    int i;
    int linked;
    int k;
    Bool full;

    nodes = camlin_tables[table];
    entry = camlin_find_free(table);
    /* @hack spelling: the null return last, after the placing, where an
     * early return would do */
    if (entry != NULL) {
        for (i = 0; i < CAMLIN_LINKS; i++) {
            linked = camera_editor.held.links[i];
            if (linked >= 0) {
                /* @hack spelling: == TRUE, where a truth test would do: the
                 * two are equal while used is TRUE or FALSE */
                if (nodes[linked].used == TRUE) {
                    full = TRUE;
                    for (k = 0; k < CAMLIN_LINKS; k++) {
                        /* @hack redundant: the else after the break */
                        if (nodes[linked].links[k] == CAMLIN_NONE) {
                            nodes[linked].links[k] = camlin_found;
                            full = FALSE;
                            break;
                        } else if (nodes[linked].links[k] == camlin_found) {
                            full = FALSE;
                            break;
                        }
                    }
                    /* @hack spelling: == TRUE, where a truth test would
                     * do */
                    if (full == TRUE) {
                        camera_editor.held.links[i] = CAMLIN_NONE;
                    }
                } else {
                    camera_editor.held.links[i] = CAMLIN_NONE;
                }
            }
        }
        *entry = camera_editor.held;
        entry->used = TRUE;
        /* @bug logic: puts back the next field camlin_pick_up cut with the
         * index the node had then, though camlin_find_free (BSR at
         * 0x8c0aee20) gives the node the table's first free entry, which
         * can lie below that index. the field then names an entry that no
         * longer holds the node (BSR at 0x8c0aef3e) */
        camlin_restore_cut();
        for (i = 0; i < CAMLIN_LINKS; i++) {
            camera_editor.held.links[i] = CAMLIN_NONE;
        }
        camera_editor.held.next = CAMLIN_NONE;
        return TRUE;
    }
    return FALSE;
}

Bool camlin_pick_up(int table)
{
    struct camlin_node *nodes;
    struct camlin_node *node;
    int index;
    int i;
    int k;

    nodes = camlin_tables[table];
    node = camlin_nearest(table, &camera_editor.cursor);
    /* @hack spelling: camlin_found held in index, which nothing here
     * changes, not read in place in the loop */
    /* @hack spelling: index read before the null test, where a read after it
     * would do */
    index = camlin_found;
    /* @hack spelling: the null return in an else, where an early return
     * would do */
    if (node != NULL) {
        camera_editor.held = *node;
        node->used = FALSE;
    } else {
        return FALSE;
    }
    camlin_forget_cut();
    for (i = 0; &nodes[i] < camlin_tables[table + 1]; i++) {
        if (nodes[i].used) {
            for (k = 0; k < CAMLIN_LINKS; k++) {
                if (nodes[i].links[k] == index) {
                    nodes[i].links[k] = CAMLIN_NONE;
                }
            }
            if (nodes[i].next == index) {
                camlin_cut_next(&nodes[i].next);
            }
        }
    }
    return TRUE;
}

Bool camlin_link_nearest(int table)
{
    struct camlin_node *node;
    Bool full;
    int i;
    Bool refused;

    node = camlin_nearest(table, &camera_editor.cursor);
    if (node == NULL) {
        return FALSE;
    }
    camera_editor.cursor = node->player_pos;
    camera_editor.link_cursor = node->player_pos;
    full = TRUE;
    for (i = 0; i < CAMLIN_LINKS; i++) {
        if (node->links[i] == CAMLIN_NONE) {
            full = FALSE;
        }
    }
    /* @hack spelling: == TRUE, where a truth test would do */
    if (full == TRUE) {
        return FALSE;
    }
    /* @hack spelling: the outcome kept in refused until after the loop, not
     * returned from inside the loop */
    refused = TRUE;
    for (i = 0; i < CAMLIN_LINKS; i++) {
        /* @bug logic: adds a node the held node links already, where one of
         * its links before that one is empty: CMP/EQ #-1,R0 at 0x8c0af1a2
         * ends the search at the first empty link. camlin_unlink_nearest
         * leaves such a gap, and so can a node camlin_pick_up takes */
        /* @hack redundant: the else after this arm's break */
        if (camera_editor.held.links[i] == CAMLIN_NONE) {
            /* @hack redundant: a test that the empty link isn't
             * camlin_found, which always holds */
            if (camera_editor.held.links[i] != camlin_found) {
                camera_editor.held.links[i] = camlin_found;
            }
            refused = FALSE;
            break;
        } else if (camera_editor.held.links[i] == camlin_found) {
            /* @hack redundant: refused set TRUE again, which it holds
             * already */
            refused = TRUE;
            break;
        }
    }
    /* @hack spelling: == TRUE, where a truth test would do */
    if (refused == TRUE) {
        return FALSE;
    }
    return TRUE;
}

Bool camlin_unlink_nearest(int table)
{
    struct camlin_node *nodes;
    struct camlin_node *nearest;
    Float least;
    Float distance;
    int i;
    int linked;

    nodes = camlin_tables[table];
    /* @hack spelling: least set ahead of nearest, not in their declared
     * order */
    least = CAMERA_NEAREST_REACH;
    nearest = NULL;
    for (i = 0; i < CAMLIN_LINKS; i++) {
        /* @hack spelling: the link tested in place, then read into linked,
         * not read first and tested through linked */
        if (camera_editor.held.links[i] >= 0) {
            linked = camera_editor.held.links[i];
            distance =
                njDistanceP2P(&camera_editor.cursor, &nodes[linked].player_pos);
            if (distance < least) {
                least = distance;
                nearest = &nodes[linked];
                camlin_found = i;
            }
        }
    }
    if (nearest == NULL) {
        return FALSE;
    }
    camera_editor.cursor = nearest->player_pos;
    camera_editor.link_cursor = nearest->player_pos;
    camera_editor.held.links[camlin_found] = CAMLIN_NONE;
    camlin_forget_cut();
    return TRUE;
}

/* reach each node not yet reached that a node of camlin_frontier's
 * frontier links. each gets its parent in camlin_parents and a place in the
 * other frontier. it returns whether it reached any */
static Bool camlin_search_step(struct camlin_node *nodes)
{
    Sint16 *from;
    Sint16 *to;
    int count;
    int i;
    int k;
    int linked;

    /* @hack spelling: i cleared ahead of from and to, not after count */
    i = 0;
    from = camlin_frontiers[camlin_frontier];
    to = camlin_frontiers[!camlin_frontier];
    count = 0;
    while (from[i] != CAMLIN_NONE) {
        for (k = 0; k < CAMLIN_LINKS; k++) {
            linked = nodes[from[i]].links[k];
            /* @hack spelling: two ifs, where one test joined by && would
             * do */
            if (linked >= 0) {
                if (camlin_parents[linked] == CAMLIN_NONE) {
                    camlin_parents[linked] = from[i];
                    /* @bug memory: a step that reaches more than
                     * FRONTIER_NODES nodes writes past the 32 entries of its
                     * frontier: frontier 0 into frontier 1, which the step
                     * reads, and frontier 1 into camlin_frontier and the
                     * next object's bss. MOV.W R12,@(R0,R13) at 0x8c0af50a
                     * stores with no bound on count. no code in the game's
                     * images calls camlin_search */
                    to[count++] = linked;
                }
            }
        }
        i++;
    }
    to[count] = CAMLIN_NONE;
    /* @hack spelling: the result as two returns, where return count != 0
     * would do */
    if (count == 0) {
        return FALSE;
    }
    return TRUE;
}

/* nothing calls camlin_search. it isn't static, since SHC leaves out a
 * static function that nothing calls.
 *
 * search table breadth first from start along the links, keeping in
 * camlin_parents how each node was reached. it returns 1 once it reaches
 * goal, or -1 where it runs out of nodes first. more than FRONTIER_NODES
 * nodes at one count of links from start overrun a frontier, unchecked */
/* NOLINTBEGIN(misc-use-internal-linkage) */
int camlin_search(int table, int start, int goal)
{
    struct camlin_node *nodes;
    char *byte;
    int i;
    int node;

    nodes = camlin_tables[table];
    /* @hack spelling: the parents filled with 0xff bytes, one at a time down
     * a count, where a loop that sets each to CAMLIN_NONE would do */
    byte = (char *)camlin_parents;
    for (i = sizeof(camlin_parents); i != 0; i--) {
        *byte++ = -1;
    }
    camlin_frontier = 0;
    camlin_frontiers[0][0] = start;
    /* @hack spelling: the frontier indexed through camlin_frontier, which is
     * 0, where camlin_frontiers[0][1] would do */
    camlin_frontiers[camlin_frontier][1] = CAMLIN_NONE;
    camlin_parents[start] = CAMLIN_START;
    if (start == goal) {
        return 1;
    }
    for (;;) {
        if (!camlin_search_step(nodes)) {
            return -1;
        }
        i = 0;
        camlin_frontier = !camlin_frontier;
        /* @hack spelling: each node read into node inside the test, where a
         * for loop that reads the frontier in place would do */
        while ((node = camlin_frontiers[camlin_frontier][i]) != CAMLIN_NONE) {
            if (node == goal) {
                return 1;
            }
            i++;
        }
    }
}
/* NOLINTEND(misc-use-internal-linkage) */

/* draw a mark that moves along the path through node. the low 5 bits of
 * game_state.total_steps place it, in 32nds of the way from node to its
 * next */
/* @hack spelling: a macro, where a static function called at both of its
 * uses would do */
/* @bug logic: the mark follows a path of table camedit_player * 2 + 1 (MOV.L
 * @R11,R7 at 0x8c0afd18), found by node's index in the table drawn. the
 * editor draws table camera_current_index * 2 + 1 (ADD #1,R4 at 0x8c0aebbc),
 * so the mark follows another table's path whenever camera_current_index
 * isn't camedit_player */
#define DRAW_MOVING_MARK(node)                                                 \
    {                                                                          \
        Sint16 path[MARK_PATH_ROOM];                                           \
        NJS_POINT3 pos;                                                        \
        int index;                                                             \
                                                                               \
        if (camlin_trace(node, path, &index,                                   \
                         CAMLIN_PLAYER_TABLE(camedit_player)) > 0) {           \
            camlin_path_point(path,                                            \
                              index + (((game_state.total_steps * 8) & 0xff) * \
                                       (1.0F / 256.0F)),                       \
                              &pos, CAMLIN_PLAYER_TABLE(camedit_player));      \
            debug_draw_mark(&pos, MARK_MOVING);                                \
        }                                                                      \
    }

void camlin_draw(int table, int flags)
{
    struct camlin_node *nodes;
    NJS_VECTOR from_camera;
    NJS_VECTOR scl;
    int idx;
    int i;
    int j;

    nodes = camlin_tables[table];
    for (idx = 0; &nodes[idx] < camlin_tables[table + 1]; idx++) {
        if (nodes[idx].used) {
            if (flags & CAMLIN_DRAW_PLAYER) {
                scl.x = nodes[idx].size * 0.5F;
                scl.y = nodes[idx].size * 0.5F;
                scl.z = nodes[idx].size * 0.5F;
                vector_subtract(&nodes[idx].player_pos,
                                &camera_views[camera_current_index]->pos,
                                &from_camera);
                if (njScalor(&from_camera) < CAMERA_MODEL_RANGE) {
                    debug_draw_model(&nodes[idx].player_pos, &camlin_model_ang,
                                     &scl, DEBUG_MODEL_SPHERE);
                }
                if (nodes[idx].next >= 0) {
                    debug_draw_mark(&nodes[idx].player_pos,
                                    CAMLIN_MARK_WITH_NEXT);
                } else {
                    debug_draw_mark(&nodes[idx].player_pos,
                                    CAMLIN_MARK_WITHOUT_NEXT);
                }
            }
            if (flags & CAMLIN_DRAW_PLAYER_LINKS) {
                for (i = 0; i < CAMLIN_LINKS; i++) {
                    j = nodes[idx].links[i];
                    if (j >= 0 && j < idx) {
                        /* @bug logic: takes a link for one along a path
                         * where either node's next is the other's index or
                         * above it (CMP/GE R2,R3 at 0x8c0af8a0, CMP/GE
                         * R13,R3 at 0x8c0af8b6). the camera links' test asks
                         * for the index itself */
                        if (nodes[idx].next >= j || nodes[j].next >= idx) {
                            /* @bug logic: no code sets the colors back, so
                             * every path link drawn after the node's, in
                             * this call or a later one, is magenta too
                             * (MOV.L R3,@R9 at 0x8c0af8c4) */
                            if (nodes[idx].highlight) {
                                camlin_player_path_colors[0].color = MAGENTA;
                                /* @hack spelling: the second color copied
                                 * from the first, where MAGENTA would do */
                                camlin_player_path_colors[1].color =
                                    camlin_player_path_colors[0].color;
                            }
                            line_draw_colored(nodes[idx].player_pos.x,
                                              nodes[idx].player_pos.y,
                                              nodes[idx].player_pos.z,
                                              &camlin_player_path_colors[0],
                                              nodes[j].player_pos.x,
                                              nodes[j].player_pos.y,
                                              nodes[j].player_pos.z,
                                              &camlin_player_path_colors[1]);
                        } else {
                            line_draw_points(&nodes[idx].player_pos,
                                             &nodes[j].player_pos);
                        }
                    }
                }
            }
            if (flags & CAMLIN_DRAW_CAMERA) {
                /* @hack redundant: a scale and an empty distance test, for a
                 * model this branch never draws */
                scl.x = nodes[idx].unk_1c;
                scl.y = nodes[idx].unk_1c;
                scl.z = nodes[idx].unk_1c;
                vector_subtract(&nodes[idx].camera_pos,
                                &camera_views[camera_current_index]->pos,
                                &from_camera);
                if (njScalor(&from_camera) < CAMERA_MODEL_RANGE) {
                }
                if (nodes[idx].next >= 0) {
                    debug_draw_mark(&nodes[idx].camera_pos,
                                    CAMLIN_MARK_WITH_NEXT);
                }
            }
            if (flags & CAMLIN_DRAW_CAMERA_LINKS) {
                /* @hack spelling: j counts the links and i holds each, where
                 * i counting and j holding, as in the player links' loop,
                 * would do */
                for (j = 0; j < CAMLIN_LINKS; j++) {
                    i = nodes[idx].links[j];
                    if (i >= 0 && i < idx) {
                        if (nodes[idx].next == i || nodes[i].next == idx) {
                            /* @bug logic: no code sets the colors back, so
                             * every path link drawn after the node's, in
                             * this call or a later one, is red too (MOV.L
                             * R3,@R8 at 0x8c0afce2) */
                            if (nodes[idx].highlight) {
                                camlin_camera_path_colors[0].color = RED;
                                /* @hack spelling: the second color copied
                                 * from the first, where RED would do */
                                camlin_camera_path_colors[1].color =
                                    camlin_camera_path_colors[0].color;
                            }
                            line_draw_colored(nodes[idx].camera_pos.x,
                                              nodes[idx].camera_pos.y,
                                              nodes[idx].camera_pos.z,
                                              &camlin_camera_path_colors[0],
                                              nodes[i].camera_pos.x,
                                              nodes[i].camera_pos.y,
                                              nodes[i].camera_pos.z,
                                              &camlin_camera_path_colors[1]);
                            DRAW_MOVING_MARK(idx);
                        } else {
                            line_draw_colored(nodes[idx].camera_pos.x,
                                              nodes[idx].camera_pos.y,
                                              nodes[idx].camera_pos.z,
                                              &camlin_camera_link_colors[0],
                                              nodes[i].camera_pos.x,
                                              nodes[i].camera_pos.y,
                                              nodes[i].camera_pos.z,
                                              &camlin_camera_link_colors[1]);
                        }
                    } else if (i >= 0) {
                        if (nodes[idx].next == i || nodes[i].next == idx) {
                            DRAW_MOVING_MARK(idx);
                        }
                    }
                }
            }
        }
    }
}

int camlin_trace(int node, Sint16 *path, int *index, int table)
{
    struct camlin_node *nodes;
    int at;
    int linked;
    Bool found;
    int count;
    int i;

    nodes = camlin_tables[table];
    if (nodes[node].next < 0) {
        return -1;
    }
    at = node;
    do {
        found = FALSE;
        for (i = 0; i < CAMLIN_LINKS; i++) {
            linked = nodes[at].links[i];
            if (linked >= 0 && nodes[linked].next == at) {
                found = TRUE;
                at = linked;
                break;
            }
        }
    } while (found);
    count = 0;
    path[count++] = at;
    do {
        if (at == node) {
            *index = count;
        }
        path[count++] = at;
        at = nodes[at].next;
    } while (nodes[at].next >= 0);
    path[count++] = at;
    path[count++] = at;
    if (count < TRACE_LEAST) {
        return -1;
    }
    path[count] = CAMLIN_NONE;
    return count;
}

void camlin_path_point(const Sint16 *path, Float place, NJS_POINT3 *out,
                       int table)
{
    struct camlin_node *nodes;
    struct curve curve;
    /* @bug memory: a path of more than 30 nodes holds more than
     * CAMLIN_PATH_POINTS entries. the copy then writes past points, over
     * curve and the return address (MOV.L R0,@R2 at 0x8c0b0742, with no
     * bound on i). DRAW_MOVING_MARK gives camlin_trace room for such a path,
     * of up to MARK_PATH_ROOM entries */
#ifdef NONMATCHING
    /* this side repairs the tag above: points has room for MARK_PATH_ROOM
     * entries. SPACE's path has room for PATH_ROOM, and DRAW_MOVING_MARK's
     * for MARK_PATH_ROOM, each with CAMLIN_NONE last */
    NJS_POINT3 points[MARK_PATH_ROOM];
#else
    NJS_POINT3 points[CAMLIN_PATH_POINTS];
#endif
    int i;

    nodes = camlin_tables[table];
    for (i = 0; path[i] != CAMLIN_NONE; i++) {
        points[i] = nodes[path[i]].camera_pos;
    }
    curve.keys = points;
    curve.count = i;
    curve.format = "FFF";
    curve.out = out;
    curve.kind = CURVE_B_SPLINE;
    curve_evaluate(&curve, place);
}

Bool camlin_has_prev(int table, int node)
{
    struct camlin_node *entries;
    int count;
    int i;

    entries = camlin_tables[table];
    /* @hack spelling: the entries counted by a loop of their own before the
     * search, where one loop that tests the bound as it searches would do */
    for (count = 0; &entries[count] < camlin_tables[table + 1]; count++) {
    }
    for (i = 0; i < count; i++) {
        /* @hack spelling: two ifs, where one test joined by && would do */
        /* @hack spelling: != FALSE, where a truth test would do */
        if (entries[i].used != FALSE) {
            if (entries[i].next == node) {
                return TRUE;
            }
        }
    }
    return FALSE;
}
