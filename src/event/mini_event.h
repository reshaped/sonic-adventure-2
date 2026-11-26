/* the mini events: scenes a stage plays from an event's files */
#ifndef EVENT_MINI_EVENT_H
#define EVENT_MINI_EVENT_H
#include <sg_xpt.h>

/* the running event's frame, which its script and its players' motions
 * follow. mini_event_start sets it to 0, and each step adds 1, up to the
 * event's last frame */
extern Float mini_event_frame;

/* load event's files from the directory EVENT, then change to the directory
 * above it. a successful load stops the music. the files are me<event>.prs, the
 * script me<event>_<language>.scr and a messages file. the messages file is
 * evmesH<language>.prs below event 100, evmesD<language>.prs below 200, and
 * evmesL<language>.prs from 200 on. the me names write event in four digits.
 * language is game_state.text_language, in one digit.
 *
 * The GCC game admits the scene and camera before allowing playback. A
 * missing script supplies the zero script. Missing or invalid subtitles
 * select the no-message warning. A failed scene cannot be started.
 *
 * On the original compiler path the load stops the music before reading, and
 * checks no read. me<event>.prs must open, since the load reads the
 * length of its camera motion. a script that won't open leaves a zeroed one,
 * which opens no window and plays no cue. a messages file that won't open
 * leaves MESSAGE_AREA as it was. where the messages hold no list for event, a
 * window says so, and the event shows no messages */
void mini_event_load(int event);
/* start the event mini_event_load loaded last, under a task named
 * miniEventExec. it returns 0, or -1 on failure. the GCC game also requires
 * an admitted scene and a free camera motion. a failed task allocation
 * changes no player, camera or game_state. on the original path no event may be
 * running, unchecked: nothing would destroy the first one's task.
 *
 * player_start_event gives each character the event's file names its entry.
 * every player then moves to the script's pos and turns to its ang. camera 0
 * loses its modes above CAMERA_MOTION_SLOT, and plays the event's camera
 * motion about the script's pos. the start drops the sound, music, jingle and
 * voice that REQUEST_AREA's record asks for.
 *
 * each step opens the windows and plays the cues of the frame it reaches. the
 * camera motion's end destroys the task, then puts each player back where the
 * start found it. the start sets game_state.mini_event_running to 1 and
 * idle_dim to 0. the task's destruction sets them to 0 and 1. the
 * destruction also ends each character's event with player_end_event, and
 * stops the vibration.
 *
 * in the GCC game, Start on the first pad skips the event: its next step is
 * its last. the stage's music still starts, where a cue the skip passes
 * starts it */
int mini_event_start(void);

#endif
