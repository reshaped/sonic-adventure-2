/* the race camera that follows its entrant by the track's path keys */
#include <sg_xpt.h>
#include "camera/camera.h"
#include "chao/work.h"
#include "chaostgrace/camera.h"
#include "chaostgrace/camera_key.h"
#include "chaostgrace/follow_view.h"
#include "chaostgrace/race.h"

void race_camera_follow(Uint16 entrant)
{
    /* @hack spelling: index and slot, two copies of entrant, where entrant
     * would do */
    Uint16 index = entrant;
    Uint16 slot = index;
    struct race_camera *camera = &race_cameras[index];
    struct camera_state *view = &camera->view;
    struct chao_work *work;
    struct race_path_key *key;

    if (race_entrant(slot)) {
        /* @hack redundant: a second call of race_entrant, where the first's
         * result would do */
        work = CHAO_WORK(race_entrant(index));
        /* the key after the one it took last */
        key = &race_camera_script.path.keys[camera->key + 1];
        if (key->camera.active) {
            if (key->progress <= work->race_progress) {
                READ_CAMERA_KEY(camera, key);
                switch (camera->position_mode) {
                case RACE_POS_FIX:
                    break;
                case RACE_POS_MOVE:
                    camera->position_step = key->camera.position_step;
                    camera->position_frames = key->position_frames;
                    break;
                case RACE_POS_PARALLEL:
                    camera->position_offset = key->camera.position_step;
                    break;
                }
            }
        }
        ADVANCE_VIEW(camera, work, view);
    }
}
