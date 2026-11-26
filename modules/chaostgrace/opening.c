/* the race's opening camera. it pulls back from the starting grid, cuts to
 * one end and pans along the entrants. then it makes the race's displays,
 * sets RACE_READY and starts every camera following its entrant */
#include <ninja.h>
#include <sg_xpt.h>
#include "camera/camera.h"
#include "chao/race.h"
#include "chao/race_record.h"
#include "chao/race_state.h"
#include "chaostgrace/camera.h"
#include "chaostgrace/grid.h"
#include "chaostgrace/map.h"
#include "chaostgrace/race.h"
#include "system/angle.h"

/* the frames of a beat of the race group's music */
static Float beat_frames;
static NJS_VECTOR eye_near = {0.0F, 7.5F, 21.0F};
static NJS_VECTOR eye_far = {0.0F, 4.5F, 60.0F};
static NJS_VECTOR target_near = {0.0F, 20.0F, 0.0F};
static NJS_VECTOR target_far = {0.0F, 20.0F, 0.0F};
static NJS_VECTOR near_grid_eye = {-12.85F, 3.5F, 15.0F};
static NJS_VECTOR near_grid_target = {-12.85F, 3.5F, 0.0F};
static NJS_VECTOR far_grid_eye = {-25.7F, 3.5F, 15.0F};
static NJS_VECTOR far_grid_target = {-25.7F, 3.5F, 0.0F};

enum opening_mode {
    OPENING_START,
    OPENING_PULL,
    OPENING_HOLD,
    OPENING_PAN,
    OPENING_WAIT,
    OPENING_DONE
};
/* the pull lasts PULL_BEATS beats. it ends in a cut to near_grid_eye on the
 * beginner swim, fly and power tracks. it cuts to far_grid_eye on the rest.
 * the camera then holds a beat, and pans NEAR_PAN or FAR_PAN a beat for
 * PAN_BEATS beats. the displays come WAIT_FRAMES frames after the pan */
#define PULL_BEATS 5.5F
#define PAN_BEATS 7.027027F
#define NEAR_PAN 3.7F
#define FAR_PAN 7.4F
#define WAIT_FRAMES 25.0F

#define PAN_GRID(speed)                                                        \
    do {                                                                       \
        view->pos.x += ((speed) / beat_frames) * njSin(yaw + QUARTER_TURN);    \
        view->pos.z += ((speed) / beat_frames) * njCos(yaw + QUARTER_TURN);    \
        view->target.x += ((speed) / beat_frames) * njSin(yaw + QUARTER_TURN); \
        view->target.z += ((speed) / beat_frames) * njCos(yaw + QUARTER_TURN); \
    } while (0)

#define GRID_LERP(axis, from, to)                                              \
    (race_grids[track].pos.axis + (from)->axis +                               \
     (phase * ((to)->axis - (from)->axis)))

void race_camera_opening(Uint16 index)
{
    struct race_camera *camera = &race_cameras[index];
    struct camera_state *view = &camera->view;
    const struct garden_race_course *course = garden_race_course();
    int track = course->track;
    Angle yaw = race_grids[track].yaw;
    Float phase;
    int entrant;

    switch (camera->mode) {
    case OPENING_START:
        camera->mode = OPENING_PULL;
        camera->frame = 0.0F;
        switch (garden_race_group()) {
        case RACE_BEGINNER:
            beat_frames = RACE_BEGINNER_BEAT;
            break;
        case RACE_JEWEL:
            beat_frames = RACE_JEWEL_BEAT;
            break;
        case RACE_CHALLENGE:
            beat_frames = RACE_CHALLENGE_BEAT;
            break;
        case RACE_DARK_COURSES:
            beat_frames = RACE_DARK_BEAT;
            break;
        case RACE_HERO_COURSES:
            beat_frames = RACE_HERO_BEAT;
            break;
        case RACE_MULTI:
            beat_frames = RACE_MULTI_BEAT;
            break;
        }
        /* fall through */
    case OPENING_PULL:
        camera->frame += 1.0F / (PULL_BEATS * beat_frames);
        if (camera->frame > 1.0F) {
            NJS_VECTOR eye;
            NJS_VECTOR target;
            camera->frame = 1.0F;
            switch (track) {
            case TRACK_BEG_SWIM:
            case TRACK_BEG_FLY:
            case TRACK_BEG_POWER:
                njPushMatrix(&_nj_unit_matrix_);
                njRotateY(NULL, yaw);
                njCalcVector(NULL, &near_grid_eye, &eye);
                njCalcVector(NULL, &near_grid_target, &target);
                njPopMatrixEx();
                view->pos.x = race_grids[track].pos.x + eye.x;
                view->pos.y = race_grids[track].pos.y + eye.y;
                view->pos.z = race_grids[track].pos.z + eye.z;
                view->target.x = race_grids[track].pos.x + target.x;
                view->target.y = race_grids[track].pos.y + target.y;
                view->target.z = race_grids[track].pos.z + target.z;
                break;
            case TRACK_BEG_RUN:
            case TRACK_JWL_SWIM:
            case TRACK_JWL_FLY:
            case TRACK_JWL_RUN:
            case TRACK_JWL_POWER:
            case TRACK_JWL_IL:
            case TRACK_JWL_LONG:
                njPushMatrix(&_nj_unit_matrix_);
                njRotateY(NULL, yaw);
                njCalcVector(NULL, &far_grid_eye, &eye);
                njCalcVector(NULL, &far_grid_target, &target);
                njPopMatrixEx();
                /* @hack redundant: two reads of race_grids[track].yaw, which
                 * nothing uses */
                view->pos.x = race_grids[track].pos.x + eye.x;
                (void)race_grids[track].yaw;
                view->pos.y = race_grids[track].pos.y + eye.y;
                (void)race_grids[track].yaw;
                view->pos.z = race_grids[track].pos.z + eye.z;
                view->target.x = race_grids[track].pos.x + target.x;
                view->target.y = race_grids[track].pos.y + target.y;
                view->target.z = race_grids[track].pos.z + target.z;
                break;
            }
            camera->mode = OPENING_HOLD;
        } else {
            /* @hack type: to_eye, from_eye, to_target and from_target, each an
             * array of one vector, where a vector would do */
            NJS_VECTOR to_eye[1];
            NJS_VECTOR from_eye[1];
            NJS_VECTOR to_target[1];
            NJS_VECTOR from_target[1];
            phase = camera->frame;
            njPushMatrix(&_nj_unit_matrix_);
            njRotateY(NULL, yaw);
            njCalcVector(NULL, &eye_near, from_eye);
            njCalcVector(NULL, &eye_far, to_eye);
            njCalcVector(NULL, &target_near, from_target);
            njCalcVector(NULL, &target_far, to_target);
            njPopMatrixEx();
            /* @hack redundant: two reads of from_eye, which nothing uses */
            view->pos.x = GRID_LERP(x, from_eye, to_eye);
            (void)from_eye->x;
            view->pos.y = GRID_LERP(y, from_eye, to_eye);
            (void)from_eye->y;
            view->pos.z = GRID_LERP(z, from_eye, to_eye);
            view->target.x = GRID_LERP(x, from_target, to_target);
            view->target.y = GRID_LERP(y, from_target, to_target);
            view->target.z = GRID_LERP(z, from_target, to_target);
        }
        break;
    case OPENING_HOLD:
        camera->frame += 1.0F;
        if (camera->frame > beat_frames) {
            camera->mode = OPENING_PAN;
            camera->frame = 0.0F;
        }
        break;
    case OPENING_PAN:
        switch (track) {
        case TRACK_BEG_SWIM:
        case TRACK_BEG_FLY:
        case TRACK_BEG_POWER:
            PAN_GRID(NEAR_PAN);
            break;
        case TRACK_BEG_RUN:
        case TRACK_JWL_SWIM:
        case TRACK_JWL_FLY:
        case TRACK_JWL_RUN:
        case TRACK_JWL_POWER:
        case TRACK_JWL_IL:
        case TRACK_JWL_LONG:
            PAN_GRID(FAR_PAN);
            break;
        }
        camera->frame += 1.0F;
        if (camera->frame > beat_frames * PAN_BEATS) {
            camera->mode = OPENING_WAIT;
            camera->frame = 0.0F;
        }
        break;
    case OPENING_WAIT:
        camera->frame += 1.0F;
        if (camera->frame > WAIT_FRAMES) {
            camera->mode = OPENING_DONE;
            race_clock_create();
            race_map_create();
            race_set_mode(RACE_READY);
            if (garden_race_group() != RACE_MULTI) {
                race_rank_create(0);
                race_stamina_create(0);
            } else {
                for (entrant = 0; entrant < RACE_ENTRANTS; ++entrant) {
                    if (garden_race_entrants[entrant]) {
                        race_multiplayer_rank_create(entrant);
                        race_multiplayer_stamina_create(entrant);
                    }
                }
            }
        }
        break;
    case OPENING_DONE:
        race_start_tracking();
        break;
    }
    view->type = CAMERA_POS_TARGET;
}
