/* a Chao race's course: the track garden_select_track chooses, whose left
 * and right edges are lists of keys. key k of one edge pairs with key k of
 * the other. segment k runs from pair k to pair k + 1 */
#ifndef CHAO_COURSE_H
#define CHAO_COURSE_H
#include <ninja.h>
#include <sg_xpt.h>

struct task;
/* a segment's corners, by their index in struct garden_track_quad's points:
 * its pair of keys, then the next pair */
enum garden_track_corner {
    TRACK_LEFT,
    TRACK_RIGHT,
    TRACK_NEXT_LEFT,
    TRACK_NEXT_RIGHT
};
struct garden_track_quad {
    NJS_POINT3 points[4];
};

/* make track, an index in track_edges, the course the other calls read */
void garden_select_track(Sint32 track);
/* draw the edges, and a line across each pair of keys but the last */
void garden_draw_track(void);
/* keep the Chao on the course, seeking its segment from the one before
 * race_progress's. race_progress becomes the segment plus the Chao's way along
 * it. that runs from 0 at the segment's first pair of keys to 1 at its next.
 * race_floor becomes the plane under the Chao. FALSE where the Chao is past the
 * track's last keys, which changes nothing. under CHAO_COURSE_CONSTRAIN the
 * Chao stays within the edges and above the floor. under CHAO_COURSE_AIM the
 * mover's target lies on the next segment's far pair of keys. under
 * CHAO_COURSE_AIM_NEAR it lies on this segment's. lane gives the target's share
 * of the way from the left key to the right */
Bool chao_follow_course_floor(struct task *chao);
/* whether the last chao_follow_course_floor moved the Chao into segment */
Bool chao_entered_course_segment(struct task *chao, Uint16 segment);
/* how many keys each of the track's edges holds */
Uint32 garden_track_point_count(void);
/* segment's corners. a segment from garden_track_point_count() - 1 on is
 * taken as that one. its next corners lie past the track's end */
void garden_track_quad(Uint16 segment, struct garden_track_quad *quad);
/* the segment whose first pair of keys has its midpoint nearest position.
 * -1 where none lies within 1000000 */
int garden_nearest_track_segment(NJS_POINT3 *position);

#endif
