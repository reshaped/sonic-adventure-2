/* what the race camera's playback and its editor's preview share: taking a
 * key, and moving the view */
#ifndef CHAOSTGRACE_FOLLOW_VIEW_H
#define CHAOSTGRACE_FOLLOW_VIEW_H
#include <ninja.h>
#include "camera/camera.h"
#include "chao/work.h"
#include "chaostgrace/camera.h"
#include "chaostgrace/camera_key.h"

#define READ_CAMERA_KEY(cam, cell)                                             \
    do {                                                                       \
        (cam)->key++;                                                          \
        (cam)->target_mode = (cell)->camera.target_mode;                       \
        (cam)->target = (cell)->camera.target;                                 \
        (cam)->position_mode = (cell)->camera.position_mode;                   \
        (cam)->position = (cell)->camera.position;                             \
    } while (0)

#define ADVANCE_VIEW(camera, work, view)                                       \
    {                                                                          \
        switch ((camera)->target_mode) {                                       \
        case RACE_TARGET_FIX:                                                  \
            break;                                                             \
        case RACE_TARGET_CHASE:                                                \
            (camera)->target = (work)->base.pos;                               \
            (camera)->target.y += 3.0F;                                        \
            break;                                                             \
        case RACE_TARGET_MOVE:                                                 \
            if ((camera)->target_frames > 0) {                                 \
                njAddVector(&(camera)->target, &(camera)->target_step);        \
                (camera)->target_frames--;                                     \
            }                                                                  \
            break;                                                             \
        }                                                                      \
        switch ((camera)->position_mode) {                                     \
        case RACE_POS_FIX:                                                     \
            break;                                                             \
        case RACE_POS_MOVE:                                                    \
            if ((camera)->position_frames > 0) {                               \
                njAddVector(&(camera)->position, &(camera)->position_step);    \
                (camera)->position_frames--;                                   \
            }                                                                  \
            break;                                                             \
        case RACE_POS_PARALLEL:                                                \
            (camera)->position.x =                                             \
                (camera)->target.x + (camera)->position_offset.x;              \
            (camera)->position.y =                                             \
                (camera)->target.y + (camera)->position_offset.y;              \
            (camera)->position.z =                                             \
                (camera)->target.z + (camera)->position_offset.z;              \
            break;                                                             \
        }                                                                      \
        (view)->target = (camera)->target;                                     \
        (view)->pos = (camera)->position;                                      \
        (view)->type = CAMERA_POS_TARGET;                                      \
    }

#endif
