/* the race camera editor's keys and views. it keeps the camera script's
 * path keys and position keys in two lists. each list holds its keys in the
 * reverse of the script's order */
#ifndef CHAOSTGRACE_EDITOR_H
#define CHAOSTGRACE_EDITOR_H
#include <sg_xpt.h>
#include "chao/course.h"
#include "chaostgrace/camera_key.h"
#include "system/pad.h"

struct race_path_cell {
    struct race_path_key key;
    struct race_path_cell *prev, *next;
};
struct race_position_cell {
    struct race_position_key key;
    struct race_position_cell *prev, *next;
};

/* the camera the third pad drives as it edits the keys. EDIT_EYE_CAMERA,
 * camera 0, edits their eye. EDIT_TARGET_CAMERA, camera 1, edits their target.
 * EDIT_NO_CAMERA is none */
enum { EDIT_NO_CAMERA = -1, EDIT_EYE_CAMERA, EDIT_TARGET_CAMERA };
extern Sint32 race_edit_camera;
/* the path key and the position key selected, or NULL */
extern struct race_path_cell *race_edit_path;
extern struct race_position_cell *race_edit_position;

/* the pad that drives the editor, the third */
#define EDIT_PAD_INDEX 2
#define EDIT_PAD (player_pads[EDIT_PAD_INDEX])

/* move a path key's progress by EDIT_PAD's up and down, PROGRESS_STEP a frame.
 * while A is held, each press moves it a whole segment instead. the progress
 * then truncates to a whole segment each frame. it keeps from 0 to the track's
 * last point */
#define PROGRESS_STEP 0.025F
#define MOVE_PROGRESS(progress)                                                \
    do {                                                                       \
        if (EDIT_PAD->on & PDD_DGT_TA) {                                       \
            if (EDIT_PAD->press & PDD_DGT_KU) {                                \
                *(progress) += 1.0F;                                           \
            }                                                                  \
            if (EDIT_PAD->press & PDD_DGT_KD) {                                \
                *(progress) -= 1.0F;                                           \
            }                                                                  \
            *(progress) = (Uint32)(*(progress));                               \
        } else {                                                               \
            if (EDIT_PAD->on & PDD_DGT_KU) {                                   \
                *(progress) += PROGRESS_STEP;                                  \
            }                                                                  \
            if (EDIT_PAD->on & PDD_DGT_KD) {                                   \
                *(progress) -= PROGRESS_STEP;                                  \
            }                                                                  \
        }                                                                      \
        if (*(progress) < 0.0F) {                                              \
            *(progress) = 0.0F;                                                \
        }                                                                      \
        if (*(progress) > (int)garden_track_point_count() - 1) {               \
            *(progress) = (int)garden_track_point_count() - 1;                 \
        }                                                                      \
    } while (0)

/* the editor's camera steps, which the third pad drives. race_camera_free
 * moves its camera free of any key */
void race_camera_free(Uint16 index);
/* on the camera race_edit_camera names, move the eye and store it as the
 * selected keys' position. the selected path key's target, or else the
 * position key's, keeps ahead of the eye at its distance. while A is held, the
 * eye circles that target instead. any other camera takes the keys' position */
void race_camera_edit_eye(Uint16 index);
/* on the camera race_edit_camera names, move the target and store it as the
 * selected keys' target. any other camera takes the keys' target */
void race_camera_edit_target(Uint16 index);
/* on the camera race_edit_camera names, move the selected path key's progress
 * and the view. store the view's target as the selected position key's.
 * race_open_editor gives it camera 2 alone, which race_edit_camera never
 * names, so it returns at once */
void race_camera_edit_progress(Uint16 index);

#endif
