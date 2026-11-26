/* the race camera editor's views: the free camera, and those that edit the
 * keys' eye, target and progress */
#include <ninja.h>
#include <sg_xpt.h>
#include <shinobi.h>
#include "camera/camera.h"
#include "chao/movement.h"
#include "chaostgrace/camera.h"
#include "chaostgrace/camera_key.h"
#include "chaostgrace/editor.h"
#include "system/angle.h"

/* a stick reading moves the view EDIT_STEP units a frame. that is 1 either way
 * past STICK_PER_UNIT, and 0 within it. it turns the view by the reading over
 * STICK_PER_ANGLE */
#define STICK_PER_UNIT 20000
#define STICK_PER_ANGLE 200
#define EDIT_STEP(axis) ((int)((axis) / STICK_PER_UNIT))
/* a reading within DEAD_ZONE of 0 is taken for 0 */
#define DEAD_ZONE 1000
/* @hack spelling: the body a bare if, where do { } while (0) would do */
#define EDIT_DEAD_ZONE(axis)                                                   \
    if ((axis) > -DEAD_ZONE && (axis) < DEAD_ZONE) {                           \
        (axis) = 0;                                                            \
    }
/* L and R lower and raise the view RISE_STEP a frame. they move the eye's view
 * SLOW_RISE_STEP while neither X nor A is held */
#define RISE_STEP 0.5F
#define SLOW_RISE_STEP 0.2F
/* the eye's view keeps from NEAREST_FOCUS to FARTHEST_FOCUS of its focus.
 * under X or A, L and R move it DOLLY_STEP a frame to or from the focus */
#define NEAREST_FOCUS 3.0F
#define FARTHEST_FOCUS 1000.0F
#define DOLLY_STEP 0.5F
/* the target's view keeps from NEAREST_TARGET to FARTHEST_TARGET of it */
#define NEAREST_TARGET 20.0F
#define FARTHEST_TARGET 100.0F
/* the free view prints its position and angles, a line each from PRINT_ROW */
#define PRINT_COLUMN 5
#define PRINT_ROW 15

void race_camera_free(Uint16 index)
{
    struct race_camera *camera = &race_cameras[index];
    Angle yaw = camera->view.ang.y;
    int x = CAMERA_STICK_READING(EDIT_PAD->x1);
    int y = CAMERA_STICK_READING(EDIT_PAD->y1);
    EDIT_DEAD_ZONE(x);
    EDIT_DEAD_ZONE(y);
    if (EDIT_PAD->on & PDD_DGT_TL) {
        camera->view.pos.y -= RISE_STEP;
    }
    if (EDIT_PAD->on & PDD_DGT_TR) {
        camera->view.pos.y += RISE_STEP;
    }
    if (EDIT_PAD->on & PDD_DGT_TX) {
        camera->view.ang.y -= x / STICK_PER_ANGLE;
        camera->view.ang.x += y / STICK_PER_ANGLE;
    } else if ((EDIT_PAD->on & PDD_DGT_TY) || (EDIT_PAD->on & PDD_DGT_TA)) {
        camera->view.pos.y -= EDIT_STEP(y);
        camera->view.pos.x += (Float)EDIT_STEP(x) * njCos(yaw);
        camera->view.pos.z -= (Float)EDIT_STEP(x) * njSin(yaw);
    } else {
        camera->view.pos.x += (Float)EDIT_STEP(y) * njSin(yaw);
        camera->view.pos.z += (Float)EDIT_STEP(y) * njCos(yaw);
        camera->view.ang.y -= x / STICK_PER_ANGLE;
    }
    camera->view.type = CAMERA_POS_ANG_DIST;
    njPrint(NJM_LOCATION(PRINT_COLUMN, PRINT_ROW), "%4.2f", camera->view.pos.x);
    njPrint(NJM_LOCATION(PRINT_COLUMN, PRINT_ROW + 1), "%4.2f",
            camera->view.pos.y);
    njPrint(NJM_LOCATION(PRINT_COLUMN, PRINT_ROW + 2), "%4.2f",
            camera->view.pos.z);
    njPrint(NJM_LOCATION(PRINT_COLUMN, PRINT_ROW + 3), "%3h",
            camera->view.ang.x);
    njPrint(NJM_LOCATION(PRINT_COLUMN, PRINT_ROW + 4), "%3h",
            camera->view.ang.y);
    njPrint(NJM_LOCATION(PRINT_COLUMN, PRINT_ROW + 5), "%3h",
            camera->view.ang.z);
}

/* @hack spelling: SELECT_FOCUS, LIMIT_DISTANCE, DOLLY and ROTATE_OFFSET bare
 * blocks, where do { } while (0) would do */
#define SELECT_FOCUS()                                                         \
    {                                                                          \
        eye = &camera->view.pos;                                               \
        if (race_edit_path) {                                                  \
            focus = &race_edit_path->key.camera.target;                        \
        } else if (race_edit_position) {                                       \
            focus = &race_edit_position->key.camera.target;                    \
        } else {                                                               \
            focus = eye;                                                       \
        }                                                                      \
    }
#define LIMIT_DISTANCE()                                                       \
    {                                                                          \
        if (distance > FARTHEST_FOCUS) {                                       \
            distance = FARTHEST_FOCUS;                                         \
        }                                                                      \
        if (distance < NEAREST_FOCUS) {                                        \
            distance = NEAREST_FOCUS;                                          \
        }                                                                      \
    }
#define DOLLY()                                                                \
    {                                                                          \
        if (EDIT_PAD->on & PDD_DGT_TL) {                                       \
            distance -= DOLLY_STEP;                                            \
        }                                                                      \
        if (EDIT_PAD->on & PDD_DGT_TR) {                                       \
            distance += DOLLY_STEP;                                            \
        }                                                                      \
    }
#define ROTATE_OFFSET(offset)                                                  \
    {                                                                          \
        Angle pitch = camera->view.ang.x;                                      \
        Angle yaw = camera->view.ang.y;                                        \
        (offset).x = 0.0F;                                                     \
        (offset).y = 0.0F;                                                     \
        (offset).z = -distance;                                                \
        njPushMatrix(&_nj_unit_matrix_);                                       \
        njRotateY(NULL, yaw);                                                  \
        njRotateX(NULL, pitch);                                                \
        njCalcVector(NULL, &(offset), &(offset));                              \
        njPopMatrixEx();                                                       \
    }

/* dst takes src where mode, a race_position_mode or race_target_mode, is
 * one of its enum's three */
/* @hack spelling: a switch whose three cases copy alike, where a test of
 * mode below 3 would do */
#define COPY_FOR_MODE(mode, dst, src)                                          \
    switch (mode) {                                                            \
    case 0:                                                                    \
        (dst) = (src);                                                         \
        break;                                                                 \
    case 1:                                                                    \
        (dst) = (src);                                                         \
        break;                                                                 \
    case 2:                                                                    \
        (dst) = (src);                                                         \
        break;                                                                 \
    }

void race_camera_edit_eye(Uint16 index)
{
    struct race_camera *camera = &race_cameras[index];
    Angle old_yaw = camera->view.ang.y;
    struct race_camera_source *source;

    if (index != race_edit_camera) {
        if (race_edit_path) {
            source = &race_edit_path->key.camera;
            COPY_FOR_MODE(source->position_mode, camera->view.pos,
                          source->position);
        }
        if (race_edit_position) {
            source = &race_edit_position->key.camera;
            COPY_FOR_MODE(source->position_mode, camera->view.pos,
                          source->position);
        }
        return;
    }
    {
        /* @hack spelling: the stick read times 256, where
         * CAMERA_STICK_READING would do */
        int x = EDIT_PAD->x1 * 256;
        int y = EDIT_PAD->y1 * 256;
        NJS_POINT3 *focus;
        NJS_POINT3 *eye;
        Float distance;
        NJS_VECTOR direction;
        EDIT_DEAD_ZONE(x);
        EDIT_DEAD_ZONE(y)
        if (EDIT_PAD->on & PDD_DGT_TX) {
            SELECT_FOCUS();
            distance = njDistanceP2P(focus, eye);
            DOLLY();
            LIMIT_DISTANCE()
            camera->view.ang.y = camera->view.ang.y - (x / STICK_PER_ANGLE);
            camera->view.ang.x += y / STICK_PER_ANGLE;
            ROTATE_OFFSET(direction);
            focus->x = eye->x + direction.x;
            focus->y = eye->y + direction.y;
            focus->z = eye->z + direction.z;
        } else if (EDIT_PAD->on & PDD_DGT_TA) {
            struct angles angles;
            NJS_VECTOR offset;
            SELECT_FOCUS();
            direction.x = focus->x - eye->x;
            direction.y = focus->y - eye->y;
            direction.z = focus->z - eye->z;
            garden_aim_angles(&direction, &angles);
            camera->view.ang.x = -angles.x;
            camera->view.ang.y = angles.y + HALF_TURN;
            camera->view.ang.y = camera->view.ang.y - (x / STICK_PER_ANGLE);
            camera->view.ang.x += y / STICK_PER_ANGLE;
            distance = njDistanceP2P(focus, eye);
            DOLLY();
            LIMIT_DISTANCE()
            ROTATE_OFFSET(offset);
            eye->x = focus->x - offset.x;
            eye->y = focus->y - offset.y;
            eye->z = focus->z - offset.z;
        } else {
            SELECT_FOCUS();
            distance = njDistanceP2P(focus, eye);
            camera->view.pos.x += EDIT_STEP(y) * njSin(old_yaw);
            camera->view.pos.z += EDIT_STEP(y) * njCos(old_yaw);
            camera->view.ang.y = camera->view.ang.y - (x / STICK_PER_ANGLE);
            if (EDIT_PAD->on & PDD_DGT_TL) {
                camera->view.pos.y -= SLOW_RISE_STEP;
            }
            if (EDIT_PAD->on & PDD_DGT_TR) {
                camera->view.pos.y += SLOW_RISE_STEP;
            }
            LIMIT_DISTANCE();
            ROTATE_OFFSET(direction)
            focus->x = eye->x + direction.x;
            focus->y = eye->y + direction.y;
            focus->z = eye->z + direction.z;
        }
        camera->view.type = CAMERA_POS_ANG_DIST;
        if (race_edit_path) {
            source = &race_edit_path->key.camera;
            switch (source->position_mode) {
            case RACE_POS_FIX:
                source->position = camera->view.pos;
                break;
            case RACE_POS_MOVE:
                source->position = camera->view.pos;
                break;
            case RACE_POS_PARALLEL:
                source->position = camera->view.pos;
                source->position_step = camera->view.pos;
                njSubVector(&source->position_step,
                            &race_edit_path->key.camera.target);
                break;
            }
        }
        if (race_edit_position) {
            source = &race_edit_position->key.camera;
            /* @hack redundant: a threefold test of source under if (0),
             * which never runs */
            if (0) {
                /* NOLINTNEXTLINE(misc-redundant-expression) */
                if (source && source && source) {
                }
            }
            switch (source->position_mode) {
            case RACE_POS_FIX:
                source->position = camera->view.pos;
                break;
            case RACE_POS_MOVE:
                source->position = camera->view.pos;
                break;
            case RACE_POS_PARALLEL:
                source->position = camera->view.pos;
                source->position_step = camera->view.pos;
#ifdef NONMATCHING
                /* this side repairs the bug tagged in the matching side: the
                 * step takes the source's own target */
                njSubVector(&source->position_step, &source->target);
#else
                /* @bug memory: reads the target through race_edit_path, which
                 * is NULL where only a position key is selected, at
                 * 0x0c9c9b4a */
                njSubVector(&source->position_step,
                            &race_edit_path->key.camera.target);
#endif
                break;
            }
        }
    }
}

void race_camera_edit_target(Uint16 index)
{
    struct race_camera *camera = &race_cameras[index];
    Angle old_yaw = camera->view.ang.y;
    struct race_camera_source *source;
    if (index != race_edit_camera) {
        if (race_edit_path) {
            source = &race_edit_path->key.camera;
            COPY_FOR_MODE(source->target_mode, camera->view.target,
                          source->target);
        }
        if (race_edit_position) {
            source = &race_edit_position->key.camera;
            COPY_FOR_MODE(source->target_mode, camera->view.target,
                          source->target);
        }
        return;
    }
    {
        int x;
        int y;
        if (race_edit_path) {
            source = &race_edit_path->key.camera;
            COPY_FOR_MODE(source->target_mode, source->target,
                          camera->view.target);
        }
        if (race_edit_position) {
            source = &race_edit_position->key.camera;
            COPY_FOR_MODE(source->target_mode, source->target,
                          camera->view.target);
        }
        x = CAMERA_STICK_READING(EDIT_PAD->x1);
        y = CAMERA_STICK_READING(EDIT_PAD->y1);
        EDIT_DEAD_ZONE(x);
        EDIT_DEAD_ZONE(y);
        if (EDIT_PAD->on & PDD_DGT_TL) {
            camera->view.target.y -= RISE_STEP;
        }
        if (EDIT_PAD->on & PDD_DGT_TR) {
            camera->view.target.y += RISE_STEP;
        }
        if (EDIT_PAD->on & PDD_DGT_TX) {
            camera->view.ang.y += x / STICK_PER_ANGLE;
            camera->view.ang.x += y / STICK_PER_ANGLE;
        } else if (EDIT_PAD->on & PDD_DGT_TY) {
            camera->view.target.y -= EDIT_STEP(y);
            camera->view.target.x += EDIT_STEP(x) * njCos(old_yaw);
            camera->view.target.z -= EDIT_STEP(x) * njSin(old_yaw);
        } else {
            camera->view.target.x += EDIT_STEP(y) * njSin(old_yaw);
            camera->view.target.z += EDIT_STEP(y) * njCos(old_yaw);
            camera->view.target.x += EDIT_STEP(x) * njCos(old_yaw);
            camera->view.target.z -= EDIT_STEP(x) * njSin(old_yaw);
        }
        if (EDIT_PAD->on & PDD_DGT_TA) {
            camera->view.dist -= EDIT_STEP(y);
        }
        if (camera->view.dist < NEAREST_TARGET) {
            camera->view.dist = NEAREST_TARGET;
        }
        if (camera->view.dist > FARTHEST_TARGET) {
            camera->view.dist = FARTHEST_TARGET;
        }
        camera->view.type = CAMERA_TARGET_ANG_DIST;
    }
}

void race_camera_edit_progress(Uint16 index)
{
    struct race_camera *camera = &race_cameras[index];
    Angle old_yaw = camera->view.ang.y;
    struct race_camera_source *source;
    if (index != race_edit_camera) {
        return;
    }
    {
        int x;
        int y;
        if (race_edit_path) {
            Float *progress = &race_edit_path->key.progress;
            MOVE_PROGRESS(progress);
        }
        if (race_edit_position) {
            source = &race_edit_position->key.camera;
            COPY_FOR_MODE(source->target_mode, source->target,
                          camera->view.target);
        }
        x = CAMERA_STICK_READING(EDIT_PAD->x1);
        y = CAMERA_STICK_READING(EDIT_PAD->y1);
        EDIT_DEAD_ZONE(x);
        EDIT_DEAD_ZONE(y);
        if (EDIT_PAD->on & PDD_DGT_TL) {
            camera->view.pos.y -= RISE_STEP;
        }
        if (EDIT_PAD->on & PDD_DGT_TR) {
            camera->view.pos.y += RISE_STEP;
        }
        if (EDIT_PAD->on & PDD_DGT_TX) {
            camera->view.ang.y -= x / STICK_PER_ANGLE;
            camera->view.ang.x += y / STICK_PER_ANGLE;
        } else {
            Float speed = EDIT_STEP(y);
            if (EDIT_PAD->on & PDD_DGT_TY) {
                camera->view.pos.y -= speed;
                camera->view.pos.x += (speed = EDIT_STEP(x)) * njCos(old_yaw);
                camera->view.pos.z -= speed * njSin(old_yaw);
            } else {
                camera->view.pos.x += speed * njSin(old_yaw);
                camera->view.pos.z += speed * njCos(old_yaw);
                camera->view.ang.y -= x / STICK_PER_ANGLE;
            }
        }
        camera->view.type = CAMERA_POS_ANG_DIST;
    }
}
