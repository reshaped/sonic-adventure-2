/* the preview of the follow camera. it takes each path key its entrant's
 * progress reaches, as race_camera_follow does. it keeps its own position
 * step, frames and offset rather than the key's */
#include <sg_xpt.h>
#include "camera/camera.h"
#include "chao/work.h"
#include "chaostgrace/camera.h"
#include "chaostgrace/camera_key.h"
#include "chaostgrace/follow_view.h"
#include "chaostgrace/race.h"
#include "system/task.h"

void race_camera_preview(Uint16 index)
{
    struct race_camera *camera = &race_cameras[index];
    struct camera_state *view = &camera->view;
    struct chao_work *work;
    struct race_path_key *key;
    struct task *entrant = race_entrant(index);
    if (entrant) {
        /* @hack redundant: a second call of race_entrant, where entrant->work
         * would do */
        work = CHAO_WORK(race_entrant(index));
        /* the key after the one it took last */
        key = &race_camera_script.path.keys[camera->key + 1];
        if (key->camera.active) {
            if (key->progress <= work->race_progress) {
                READ_CAMERA_KEY(camera, key);
            }
        }
        ADVANCE_VIEW(camera, work, view);
    }
}
