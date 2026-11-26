/* a stage's camlin nodes: the paths SPACE follows, the camlin editor's
 * edits to them, and their draw */
#ifndef CAMERA_CAMLIN_H
#define CAMERA_CAMLIN_H
#include <ninja.h>
#include <sg_xpt.h>

/* the links a node has room for */
#define CAMLIN_LINKS 6
/* an empty link, the next of a node that ends its path, and the end of a
 * list of nodes */
#define CAMLIN_NONE (-1)

/* a node of a camlin file. links and next hold indexes in the node's table.
 * links name the node's neighbors, and next the node after it on its path. a
 * next below 0 ends the path */
struct camlin_node {
    NJS_POINT3 player_pos;
    Float size; /* camlin_draw scales the node's model by half of it */
    NJS_POINT3 camera_pos;
    Float unk_1c;
    Sint16 links[CAMLIN_LINKS];
    Sint16 next;
    Sint8 used; /* not 0 where the entry holds a node */
    /* while it isn't 0, camlin_draw draws the node's path links in magenta
     * and red, not green and yellow */
    Sint8 highlight;
};

/* table n's entries run from camlin_tables[n] up to camlin_tables[n + 1].
 * table 1 lies in CAMLIN_AREA_0 and table 3 in CAMLIN_AREA_1. tables 2 and
 * 4 are empty. camlin_tables[0] is null, and no code walks table 0 */
#define CAMLIN_TABLES 5
/* @hack type: the array declared without its size, where CAMLIN_TABLES + 1
 * would do */
extern struct camlin_node *camlin_tables[];
/* camera n's table, which its SPACE mode follows and player n's editor
 * edits */
#define CAMLIN_PLAYER_TABLE(n) (((n) * 2) + 1)

/* an index that camlin_nearest, camlin_nearest_camera and the editor's
 * edits below set, as each one's comment says. a call that finds no node or
 * entry leaves it as it was */
extern int camlin_found;

/* a bit for each camera, 1 << the camera. SPACE's step sets its camera's
 * bit, and camlin_editor_step sets it to its player's bit alone. no code
 * tests one */
extern Uint32 camlin_camera_mask;

/* the used node of table whose player_pos lies nearest pos, closer than
 * CAMERA_NEAREST_REACH, or null. camlin_nearest_camera measures to
 * camera_pos instead. of nodes at one distance, the first is taken.
 * camlin_found gets the node's index */
struct camlin_node *camlin_nearest(int table, NJS_POINT3 *pos);
struct camlin_node *camlin_nearest_camera(int table, NJS_POINT3 *pos);

/* the camlin editor's edits to table, through camera_editor's held node and
 * cursor. a node in reach lies closer to the cursor than
 * CAMERA_NEAREST_REACH */

/* pick up the node nearest the cursor: copy it to the held node, and free
 * its entry. camlin_found gets the entry's index. every used node drops its
 * links to the entry, and each next that names it becomes CAMLIN_NONE. the
 * last of those nexts, or none, is kept for camlin_put_held to put back. it
 * returns TRUE, or FALSE where no node lies in reach, changing nothing */
Bool camlin_pick_up(int table);
/* put the held node in table's first free entry, and return TRUE.
 * camlin_found gets the entry's index. each node the held node links gets a
 * link back, in its first empty link. a node that holds a link back already,
 * ahead of that empty link, gets none. the node put down keeps no link to a
 * free entry. nor does it keep one to a node that has neither an empty link nor
 * a link back. the next camlin_pick_up kept then gets its old value back. last,
 * the held node's links and next are emptied. for a full table it returns
 * FALSE, changing nothing */
Bool camlin_put_held(int table);
/* link the held node to the node nearest the cursor, in its first empty
 * link, and return TRUE. that node gains no link back until
 * camlin_put_held. camlin_found gets its index, and the cursor and
 * link_cursor move to its player_pos. where no node lies in reach, it
 * returns FALSE, changing nothing. it also returns FALSE, with the cursors
 * moved, where either node has no empty link. it does the same where the
 * held node links the node already, ahead of its first empty link */
Bool camlin_link_nearest(int table);
/* empty the held node's link to the linked node nearest the cursor, and
 * return TRUE. camlin_found gets the link's place in the held node's links. the
 * cursor and link_cursor move to the node's player_pos. the next camlin_pick_up
 * kept is forgotten. where no linked node lies in reach, it returns FALSE,
 * changing nothing */
Bool camlin_unlink_nearest(int table);
/* forget the next camlin_pick_up kept, so that camlin_put_held puts nothing
 * back */
void camlin_forget_cut(void);

/* camlin_draw's flags. CAMLIN_DRAW_PLAYER draws a sphere and a mark at each
 * node's player_pos. CAMLIN_DRAW_CAMERA draws a mark at the camera_pos of each
 * node with a next. the LINKS flags draw the links between those points */
#define CAMLIN_DRAW_PLAYER_LINKS 0x1
#define CAMLIN_DRAW_CAMERA_LINKS 0x2
#define CAMLIN_DRAW_PLAYER 0x4
#define CAMLIN_DRAW_CAMERA 0x8
/* the size of the mark at a node's point, by whether the node has a next */
#define CAMLIN_MARK_WITH_NEXT 13.0F
#define CAMLIN_MARK_WITHOUT_NEXT 3.0F

/* draw table's used nodes by flags. a sphere draws only within
 * CAMERA_MODEL_RANGE of the current camera. a node draws only its links to
 * nodes of lower indexes. so a link both nodes hold draws once.
 *
 * a camera link is a path link where one node is the other's next. a player
 * link is a path link where either node's next is at least the other's
 * index. camera path links draw in yellow, and other camera links in blue.
 * player path links draw in green, and other player links in
 * line_draw_points' color. a node whose highlight isn't 0 draws its camera path
 * links red and its player path links magenta. the colors then stay. each later
 * path link of that kind takes them too, in this call and in later ones.
 *
 * under CAMLIN_DRAW_CAMERA_LINKS, a node draws a moving mark for each camera
 * path link it holds. the mark moves along the node's path in table
 * CAMLIN_PLAYER_TABLE(camedit_player), and doesn't draw where camlin_trace
 * fails */
void camlin_draw(int table, int flags);

/* fill path with the indexes of the nodes on node's path, then CAMLIN_NONE.
 * return the count of indexes before CAMLIN_NONE. the path runs forward from
 * node along next. it runs back from node through each node's prior node. that
 * is the first of its linked nodes whose next it is. the path's first and last
 * nodes each come twice. *index gets node's place in path, or for the first
 * node its second place.
 *
 * it returns -1 where node has no next, or where its path has fewer than
 * three nodes. path is then left unended. path needs room for the path's
 * nodes, the ends' repeats and CAMLIN_NONE, and the path must not loop.
 * neither is checked */
int camlin_trace(int node, Sint16 *path, int *index, int table);
/* the most entries camlin_path_point takes from a path */
#define CAMLIN_PATH_POINTS 32
/* set *out to the point at place along the CURVE_B_SPLINE that the
 * camera_pos of path's nodes shape. place counts path's entries from 0, and
 * a fraction lies between two. path ends with CAMLIN_NONE, after at most
 * CAMLIN_PATH_POINTS entries, unchecked */
void camlin_path_point(const Sint16 *path, Float place, NJS_POINT3 *out,
                       int table);
/* whether a used node of table has node for its next */
Bool camlin_has_prev(int table, int node);

#endif
