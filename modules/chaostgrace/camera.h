/* the race's cameras, one for each entrant. a camera's step moves its view
 * each frame, and each screen shows the camera its viewport names */
#ifndef CHAOSTGRACE_CAMERA_H
#define CHAOSTGRACE_CAMERA_H
#include <ninja.h>
#include <sg_xpt.h>
#include "camera/camera.h"
#include "chao/race_record.h"

struct camera_stack;
struct camera_slot;

/* a camera's step, which race_step_camera calls with the camera's index */
typedef void race_camera_step(Uint16 index);

/* a race camera. frame, mode and timer are its step's own, which
 * race_set_camera_step clears. target_mode is a race_target_mode and
 * position_mode a race_position_mode, the ways a key's target and position
 * move the view */
struct race_camera {
    struct camera_state view;
    race_camera_step *step;
    Sint8 entrant; /* the camera's own index */
    Uint8 unk_45;
    /* the index of the path key it took last, or -1 before the first */
    Sint16 key;
    Float frame;
    Uint16 mode, timer;
    Uint16 target_mode, target_frames;
    NJS_POINT3 target;
    NJS_VECTOR target_step;
    Uint16 position_mode, position_frames;
    NJS_POINT3 position;
    /* position_offset is position less target, which RACE_POS_PARALLEL
     * keeps */
    NJS_VECTOR position_step, position_offset;
    Uint8 unk_94[20];
};

/* the camera a screen shows */
struct race_viewport {
    Uint16 camera;
    Uint8 unk_02[8];
};

extern struct race_viewport race_viewports[CAMERAS];
extern struct race_camera race_cameras[RACE_ENTRANTS];
/* the numbers of the entrants racing, from index 0 */
extern int race_racing_entrants[RACE_ENTRANTS];

/* the user mode race_camera_create pushes on each screen's camera stack. it
 * sets camera_state to the view of the camera the screen's viewport names */
void race_camera_to_screen(struct camera_stack *stack,
                           struct camera_slot *slot);
/* unless the camera manager exists, make it and load track's camera keys. the
 * manager puts one screen in use, and screen n shows camera 2n. camera 0 runs
 * the opening, or the preview if the first pad has just pressed Y. the other
 * cameras run nothing. with the manager there, it does nothing */
#ifdef NONMATCHING
/* this side returns FALSE where it can't make the task or load the keys */
Bool race_camera_create(int track);
#else
void race_camera_create(int track);
#endif
void race_set_camera_step(int index, race_camera_step *step);
race_camera_step *race_get_camera_step(Uint16 index);
void race_set_viewport(Uint16 screen, Uint16 camera);
/* set every camera following its entrant, and in a multiplayer race show a
 * random racer on screen 0 */
void race_start_tracking(void);
void race_step_camera(Uint16 index);
/* the camera manager's step for screen 0 in a multiplayer race, each frame.
 * now and then it cuts screen 0 to the racer cheered last. after a longer
 * while without a cut, it cuts to a random racer. from the podium on, it shows
 * camera 0. screen is unused */
void race_select_spectator_view(int screen);
void race_camera_preview(Uint16 index);
/* move the entrant's camera by the path keys its progress has reached */
void race_camera_follow(Uint16 entrant);
void race_camera_opening(Uint16 index);
void race_camera_podium(Uint16 index);
/* free the editor's lists of camera keys */
void race_clear_camera_keys(void);
/* read track's camera script into race_camera_script, and make the editor's
 * lists of its keys */
#ifdef NONMATCHING
/* this side returns FALSE where it can't read the script or make a list. the
 * lists then stay as they were */
Bool race_load_camera_keys(int track);
#else
void race_load_camera_keys(int track);
#endif

#endif
