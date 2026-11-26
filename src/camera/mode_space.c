/* the camera's SPACE mode, which follows the player along the camlin nodes
 * of the camera's table. a step that follows a segment sets the zone's
 * camera_pos and point. FIX then stands the camera by them */
#include <ninja.h>
#include <sg_xpt.h>

#include "camera/camera.h"
#include "camera/camlin.h"
#include "camera/knukles.h"
#include "camera/modes.h"
#include "camera/stack.h"
#include "camera/zones.h"
#include "player/player.h"
#include "system/task.h"
#include "system/vector.h"

#define SPACE_START 0 /* the slot's first step */
#define SPACE_FIND 1 /* look for a segment that holds the player */
#define SPACE_FOLLOW 2 /* follow the segment the work names */
#define SPACE_UNK_3 3 /* no step sets it */

/* the zone's view, read as two flags. a camera that follows no segment runs
 * KNUKLES while view[HOLD] is 0. otherwise it looks at the player's target from
 * where it stands. a trigger can push a mode only while the camera follows a
 * segment and view[HOLD] is 0. while view[NO_COLLIDE] isn't 0,
 * camera_state.unk_04 is 0, so the camera task runs no camera_collide */
#define HOLD 0
#define NO_COLLIDE 1

/* a segment runs between the player_pos of two linked nodes. it lies on a
 * path where one node is the other's next. it holds the player where the
 * player lies between its ends, within its reach of its line. the reach runs
 * from one node's size to the other's. it is LEAST_REACH all along where
 * either size is below LEAST_REACH */
#define LEAST_REACH 50.0F
/* a player within AT_NODE of its nearest node counts as at the node. where no
 * segment holds that player, the step follows the segment to the node's last
 * link. the node must have two links or more */
#define AT_NODE 50.0F
#define POINT_NEAR 50.0F
/* the nearest distance the search for a segment starts from */
#define FAR_AWAY 30000.0F
/* while the camera follows no segment, it stands at least STAND_OFF from the
 * player in x and z */
#define STAND_OFF 30.0F
/* the camera and the point it looks at each move EASE of the way in a step */
#define EASE 0.3F
/* on a path, the camera looks at the point LOOK_AHEAD entries on from where
 * it stands. once that point would reach the path's last node, the camera
 * looks at the player's target. the path's last two entries are that node */
#define LOOK_AHEAD 1.0F
/* the pull is the player's offset from its point on the segment. on a path, the
 * pull moves the camera and the point it looks at. it does so only while the
 * segment's first node has its highlight set. they take PULL_UP of it in y. in
 * x and z they take PULL_ACROSS of it where the path runs on from that node.
 * they take all of it where the path runs into the node */
#define PULL_UP 0.5F
#define PULL_ACROSS 0.8F
/* the step's path has room for PATH_ROOM entries, which camlin_trace doesn't
 * check */
#define PATH_ROOM 52

Uint32 camlin_camera_mask;

struct space_work {
    /* first, since KNUKLES takes the slot's work for its own */
    struct knukles_work knukles;
    Sint32 state; /* a SPACE_ state */
    /* the segment the camera follows, as indexes of its nodes in the
     * camera's table. segment[0] is the node nearest the player, and
     * segment[1] the one linked to it */
    Sint32 segment[2];
    Sint32 unk_24; /* no step reads it or writes it */
    /* where the last step that followed a segment put the player on it */
    NJS_POINT3 point;
};

static void space_stand_off(void)
{
    NJS_VECTOR away;

    away.x = camera_state.pos.x - player_work[camera_current_index]->pos.x;
    away.z = camera_state.pos.z - player_work[camera_current_index]->pos.z;
    if ((away.x * away.x) + (away.z * away.z) < STAND_OFF * STAND_OFF) {
        away.y = 0.0F;
        if (away.x == 0.0F && away.z == 0.0F) {
            away.x += 0.01F;
        }
        njUnitVector(&away);
        camera_state.pos.x =
            player_work[camera_current_index]->pos.x + (away.x * STAND_OFF);
        camera_state.pos.z =
            player_work[camera_current_index]->pos.z + (away.z * STAND_OFF);
    }
}

static void space_off_path(struct camera_stack *stack, struct camera_slot *slot)
{
    struct camera_zone *zone = slot->zone;

    if (zone->view[HOLD] != 0) {
        camera_state.target = player_work[camera_current_index]->pos;
        camera_state.target.y +=
            player_records[camera_current_index]->consts.target_height;
        camera_state.trigger_push = 0;
    } else {
        camera_mode_knukles(stack, slot);
    }
}

/* pos's distance from the segment between nodes from and to. it returns -1
 * where the segment doesn't hold pos, or has no length. where out isn't null,
 * *out gets the segment's point nearest pos, unless it returns -1 */
static Float space_segment_distance(int from, int to, NJS_POINT3 *pos,
                                    NJS_POINT3 *out)
{
    struct camlin_node *nodes;
    struct camlin_node *start;
    struct camlin_node *end;
    NJS_POINT3 nearest;
    NJS_LINE line;
    NJS_VECTOR off;
    NJS_VECTOR half;
    Float dist;
    Float reach;
    Float along;
    Float run;

    /* @hack spelling: camlin_tables[camera_current_index + 1], where
     * camlin_tables[CAMLIN_PLAYER_TABLE(camera_current_index)] would do for
     * cameras 0 and 1: table 2 is empty and starts where table 3 does */
    nodes = camlin_tables[camera_current_index + 1];
    start = &nodes[from];
    line.px = start->player_pos.x;
    line.py = start->player_pos.y;
    line.pz = start->player_pos.z;
    end = &nodes[to];
    line.vx = end->player_pos.x - start->player_pos.x;
    line.vy = end->player_pos.y - start->player_pos.y;
    line.vz = end->player_pos.z - start->player_pos.z;
    dist = njDistanceP2L(pos, &line, &nearest);
    half.x = line.vx * 0.5F;
    half.y = line.vy * 0.5F;
    half.z = line.vz * 0.5F;
    off.x = nearest.x - start->player_pos.x - half.x;
    off.y = nearest.y - start->player_pos.y - half.y;
    off.z = nearest.z - start->player_pos.z - half.z;
    if (njScalor2(&half) >= njScalor2(&off)) {
        reach = LEAST_REACH;
        /* @hack spelling: the test of dist written in each arm for an axis
         * the line runs along, where one test after the arms would do */
        /* @hack spelling: the sizes in the reach read as nodes[from] and
         * nodes[to], where start and end would do */
        if (start->size >= reach && end->size >= reach) {
            if (line.vx != 0.0F) {
                /* @hack spelling: the x arm's part divided through run,
                 * where along divided in place, as the arms after it
                 * divide, would do */
                run = nearest.x - start->player_pos.x;
                along = run / line.vx;
                reach = (along * (nodes[to].size - nodes[from].size)) +
                        nodes[from].size;
                if (dist > reach) {
                    return -1.0F;
                }
            } else if (line.vy != 0.0F) {
                along = nearest.y - start->player_pos.y;
                along /= line.vy;
                reach = (along * (nodes[to].size - nodes[from].size)) +
                        nodes[from].size;
                if (dist > reach) {
                    return -1.0F;
                }
            } else if (line.vz != 0.0F) {
                along = nearest.z - start->player_pos.z;
                along /= line.vz;
                reach = (along * (nodes[to].size - nodes[from].size)) +
                        nodes[from].size;
                if (dist > reach) {
                    return -1.0F;
                }
            } else {
                return -1.0F;
            }
        } else if (line.vx != 0.0F) {
            if (dist > reach) {
                return -1.0F;
            }
        } else if (line.vy != 0.0F) {
            if (dist > reach) {
                return -1.0F;
            }
        } else if (line.vz != 0.0F) {
            if (dist > reach) {
                return -1.0F;
            }
        } else {
            return -1.0F;
        }
        if (out) {
            *out = nearest;
        }
        return dist;
    }
    return -1.0F;
}

/* move pos from the segment between nodes from and to onto the line between
 * their camera_pos. pos keeps its part of the way from node from to node
 * to */
static void space_camera_point(int from, int to, NJS_POINT3 *pos)
{
    struct camlin_node *nodes;
    NJS_POINT3 start;
    NJS_POINT3 end;
    NJS_VECTOR span;
    NJS_VECTOR run;
    Float along;

    /* @hack spelling: camlin_tables[camera_current_index + 1], where
     * camlin_tables[CAMLIN_PLAYER_TABLE(camera_current_index)] would do for
     * cameras 0 and 1: table 2 is empty and starts where table 3 does */
    nodes = camlin_tables[camera_current_index + 1];
    start = nodes[from].player_pos;
    end = nodes[to].player_pos;
    vector_subtract(&end, &start, &span);
    vector_subtract(pos, &start, &run);
    along = njScalor(&run) / njScalor(&span);
    start = nodes[from].camera_pos;
    end = nodes[to].camera_pos;
    vector_subtract(&end, &start, &span);
    pos->x = start.x + (along * span.x);
    pos->y = start.y + (along * span.y);
    pos->z = start.z + (along * span.z);
}

void camera_mode_space(struct camera_stack *stack, struct camera_slot *slot)
{
    /* @hack redundant: kept, which nothing reads */
    Sint32 kept[2];
    NJS_POINT3 pos;
    /* NOLINTNEXTLINE(clang-diagnostic-unused-but-set-variable) */
    NJS_POINT3 nearest_pos;
    NJS_VECTOR away;
    Sint16 path[PATH_ROOM];
    NJS_VECTOR span;
    NJS_VECTOR pull;
    struct space_work *work;
    struct camera_zone *zone;
    struct camlin_node *nodes;
    struct camlin_node *node;
    Sint16 *link;
    int linked;
    int links;
    int none;
    int last;
    int count;
    int index;
    Float nearest;
    Float dist;
    Float along;

    work = (struct space_work *)slot->work;
    zone = slot->zone;
    /* @hack spelling: camlin_tables[camera_current_index + 1], where
     * camlin_tables[CAMLIN_PLAYER_TABLE(camera_current_index)] would do for
     * cameras 0 and 1: table 2 is empty and starts where table 3 does */
    nodes = camlin_tables[camera_current_index + 1];
    if (zone == NULL) {
        camera_set_mode(camera_current_index, camera_top(camera_current_index),
                        CAMERA_MODE_KNUKLES);
        return;
    }
    camera_state.type = CAMERA_POS_TARGET;
    if (slot->steps == 0) {
        camera_set_adjust(camera_current_index,
                          camera_top(camera_current_index),
                          CAMERA_ADJUST_RELATIVE3);
        work->state = SPACE_START;
        zone->camera_pos = player_work[camera_current_index]->pos;
    }
    camlin_camera_mask |= 1 << camera_current_index;
    switch (work->state) {
    case SPACE_START:
        work->state = SPACE_FIND;
        /* fall through */
    case SPACE_FIND:
    find:
        kept[0] = work->segment[0];
        kept[1] = work->segment[1];
        node = camlin_nearest(CAMLIN_PLAYER_TABLE(camera_current_index),
                              &player_work[camera_current_index]->pos);
        /* @hack spelling: the store of camlin_found in an else after the
         * break, where a statement after the if would do */
        if (node == NULL) {
            work->segment[0] = CAMLIN_NONE;
            space_off_path(stack, slot);
            break;
        } else {
            work->segment[0] = camlin_found;
        }
        nearest = FAR_AWAY;
        none = 1;
        work->segment[1] = CAMLIN_NONE;
        last = CAMLIN_NONE;
        links = 0;
        if (node->used) {
            /* @hack spelling: link stepped as it is read, from
             * &node->links[0], where a loop that steps it from node->links
             * would do */
            for (link = &node->links[0]; link < &node->links[CAMLIN_LINKS];) {
                linked = *link++;
                if (linked >= 0) {
                    last = linked;
                    links++;
                    dist = space_segment_distance(
                        work->segment[0], linked,
                        &player_work[camera_current_index]->pos, &pos);
                    if (dist >= 0.0F && dist < nearest) {
                        nearest = dist;
                        /* @bug logic: follows the nearest segment from the
                         * point of the last one that holds the player. the
                         * loop keeps the nearest one's point in nearest_pos
                         * (MOV.L R1,@R2 at 0x8c0b5892), which nothing reads,
                         * and the code at the label follow takes pos (MOV.L
                         * @R2,R3 at 0x8c0b59e0) */
                        nearest_pos = pos;
                        none = 0;
                        work->segment[1] = linked;
                    }
                }
            }
        }
        if (none) {
            vector_subtract(&player_work[camera_current_index]->pos,
                            &nodes[work->segment[0]].player_pos, &away);
            if (links >= 2 && njScalor(&away) < AT_NODE) {
                work->segment[1] = last;
                pos = nodes[work->segment[0]].player_pos;
                work->state = SPACE_FOLLOW;
                goto follow;
            } else {
                work->segment[0] = CAMLIN_NONE;
                space_off_path(stack, slot);
            }
        } else {
            if (camlin_nearest_camera(
                    CAMLIN_PLAYER_TABLE(camera_current_index),
                    &camera_views[camera_current_index]->pos) != NULL) {
                kept[0] = camlin_found;
            } else {
                space_off_path(stack, slot);
                break;
            }
            /* @hack redundant: away, from pos to the work's point, and the
             * test of its length, whose two arms do the same */
            vector_subtract(&work->point, &pos, &away);
            if (njScalor(&away) < POINT_NEAR) {
                work->state = SPACE_FOLLOW;
                goto follow;
            } else {
                work->state = SPACE_FOLLOW;
                goto follow;
            }
        }
        break;
    case SPACE_FOLLOW:
        /* @bug logic: a player on the segment's line, where
         * space_segment_distance returns 0, counts as off the segment, and
         * the step looks for one again. the test takes only a distance above
         * 0 (FCMP/GT FR3,FR0 at 0x8c0b59d0), where the find loop takes 0 too
         * (FCMP/GT FR4,FR3 at 0x8c0b5878), and off the segment is -1 */
        if (space_segment_distance(work->segment[0], work->segment[1],
                                   &player_work[camera_current_index]->pos,
                                   &pos) > 0.0F) {
        follow:
            work->point = pos;
            /* @hack redundant: zone read from the slot again, which it
             * holds already */
            zone = slot->zone;
            /* NOLINTBEGIN(clang-diagnostic-self-assign) */
            /* NOLINTBEGIN(clang-diagnostic-uninitialized) */
            /* NOLINTBEGIN(clang-diagnostic-sometimes-uninitialized) */
            if (nodes[work->segment[0]].next == work->segment[1] ||
                nodes[work->segment[1]].next == work->segment[0]) {
                if (nodes[work->segment[1]].next == work->segment[0]) {
                    count =
                        camlin_trace(work->segment[1], path, &index,
                                     CAMLIN_PLAYER_TABLE(camera_current_index));
                    if (count > 0) {
                        vector_subtract(&nodes[work->segment[0]].player_pos,
                                        &nodes[work->segment[1]].player_pos,
                                        &span);
                        along = njScalor(&span);
                        vector_subtract(
                            &pos, &nodes[work->segment[1]].player_pos, &span);
                        along = njScalor(&span) / along;
                    } else {
                        /* @hack redundant: along set to itself */
                        along = along;
                        space_camera_point(work->segment[0], work->segment[1],
                                           &pos);
                        zone->point = player_work[camera_current_index]->pos;
                        zone->point.y += player_records[camera_current_index]
                                             ->consts.target_height;
                    }
                    /* @bug uninitialized: where camlin_trace fails, as it
                     * does for a path of two nodes (CMP/GE R3,R14 at
                     * 0x8c0b06ae), the step still takes the place along the
                     * path, from an along the arm above never set. SHC keeps
                     * along in FR15 (FADD FR15,FR4 at 0x8c0b5b22 and at
                     * 0x8c0b5ce2, the same in the arm for the other way), so
                     * the place comes from what FR15 last held. it replaces
                     * the pos the arm worked out */
                    /* @bug memory: camlin_trace then leaves path unended,
                     * and camlin_path_point reads on through entries nothing
                     * wrote until one is CAMLIN_NONE (JSR @R3 at 0x8c0b5b24
                     * and at 0x8c0b5ce4). it takes each as a node's index,
                     * and past CAMLIN_PATH_POINTS entries it writes beyond
                     * its array of points */
#ifdef NONMATCHING
                    /* this side repairs the two tags above: where
                     * camlin_trace fails, it skips their reads, which C
                     * can't reproduce, and pos stays where the else above
                     * set it */
                    if (count > 0) {
                        camlin_path_point(
                            path, index + along, &pos,
                            CAMLIN_PLAYER_TABLE(camera_current_index));
                    }
#else
                    camlin_path_point(
                        path, index + along, &pos,
                        CAMLIN_PLAYER_TABLE(camera_current_index));
#endif
#ifdef NONMATCHING
                    /* this side repairs the two tags above here too: where
                     * camlin_trace fails, it looks at the player's target, not
                     * ahead along the path */
                    if (count > 0 && index + along < count - 3) {
#else
                    if (index + along < count - 3) {
#endif
                        camlin_path_point(
                            path, index + along + LOOK_AHEAD, &zone->point,
                            CAMLIN_PLAYER_TABLE(camera_current_index));
                    } else {
                        zone->point = player_work[camera_current_index]->pos;
                        zone->point.y += player_records[camera_current_index]
                                             ->consts.target_height;
                    }
                    if (nodes[work->segment[0]].highlight) {
                        vector_subtract(&player_work[camera_current_index]->pos,
                                        &work->point, &pull);
                        pull.y *= PULL_UP;
                        vector_add(&pos, &pull, &pos);
                        vector_add(&zone->point, &pull, &zone->point);
                    }
                } else {
                    count =
                        camlin_trace(work->segment[0], path, &index,
                                     CAMLIN_PLAYER_TABLE(camera_current_index));
                    if (count > 0) {
                        vector_subtract(&nodes[work->segment[1]].player_pos,
                                        &nodes[work->segment[0]].player_pos,
                                        &span);
                        along = njScalor(&span);
                        vector_subtract(
                            &pos, &nodes[work->segment[0]].player_pos, &span);
                        along = njScalor(&span) / along;
                    } else {
                        /* @hack redundant: along set to itself */
                        along = along;
                        space_camera_point(work->segment[0], work->segment[1],
                                           &pos);
                        zone->point = player_work[camera_current_index]->pos;
                        zone->point.y += player_records[camera_current_index]
                                             ->consts.target_height;
                    }
#ifdef NONMATCHING
                    /* this side repairs the two tags in the arm above, as
                     * that arm's guards do: where camlin_trace fails, pos
                     * stays where the else above set it, and the camera
                     * looks at the player's target */
                    if (count > 0) {
                        camlin_path_point(
                            path, index + along, &pos,
                            CAMLIN_PLAYER_TABLE(camera_current_index));
                    }
                    if (count > 0 && index + along < count - 3) {
#else
                    camlin_path_point(
                        path, index + along, &pos,
                        CAMLIN_PLAYER_TABLE(camera_current_index));
                    if (index + along < count - 3) {
#endif
                        camlin_path_point(
                            path, index + along + LOOK_AHEAD, &zone->point,
                            CAMLIN_PLAYER_TABLE(camera_current_index));
                    } else {
                        zone->point = player_work[camera_current_index]->pos;
                        zone->point.y += player_records[camera_current_index]
                                             ->consts.target_height;
                    }
                    if (nodes[work->segment[0]].highlight) {
                        vector_subtract(&player_work[camera_current_index]->pos,
                                        &work->point, &pull);
                        pull.x *= PULL_ACROSS;
                        pull.y *= PULL_UP;
                        pull.z *= PULL_ACROSS;
                        vector_add(&pos, &pull, &pos);
                        vector_add(&zone->point, &pull, &zone->point);
                    }
                }
            } else {
                space_camera_point(work->segment[0], work->segment[1], &pos);
                zone->point = player_work[camera_current_index]->pos;
                zone->point.y +=
                    player_records[camera_current_index]->consts.target_height;
            }
            /* NOLINTEND(clang-diagnostic-sometimes-uninitialized) */
            /* NOLINTEND(clang-diagnostic-uninitialized) */
            /* NOLINTEND(clang-diagnostic-self-assign) */
            pos.x =
                camera_views[camera_current_index]->pos.x +
                ((pos.x - camera_views[camera_current_index]->pos.x) * EASE);
            pos.y =
                camera_views[camera_current_index]->pos.y +
                ((pos.y - camera_views[camera_current_index]->pos.y) * EASE);
            pos.z =
                camera_views[camera_current_index]->pos.z +
                ((pos.z - camera_views[camera_current_index]->pos.z) * EASE);
            zone->point.x = camera_views[camera_current_index]->target.x +
                            ((zone->point.x -
                              camera_views[camera_current_index]->target.x) *
                             EASE);
            zone->point.y = camera_views[camera_current_index]->target.y +
                            ((zone->point.y -
                              camera_views[camera_current_index]->target.y) *
                             EASE);
            zone->point.z = camera_views[camera_current_index]->target.z +
                            ((zone->point.z -
                              camera_views[camera_current_index]->target.z) *
                             EASE);
            zone->camera_pos = pos;
            zone->camera_ang.z = 0;
            camera_mode_fix(stack, slot);
            if (zone->view[HOLD]) {
                camera_state.trigger_push = 0;
            } else {
                camera_state.trigger_push = 1;
            }
        } else {
            work->state = SPACE_FIND;
            goto find;
        }
        break;
    /* @hack redundant: the case for SPACE_UNK_3, which no step sets */
    case SPACE_UNK_3:
        break;
    }
    if (work->state != SPACE_FOLLOW) {
        space_stand_off();
    }
    if (zone->view[NO_COLLIDE]) {
        camera_state.unk_04 = 0;
    } else {
        camera_state.unk_04 = 1;
    }
}
