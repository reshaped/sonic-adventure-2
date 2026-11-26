/* the race's camera scripts. race_read_camera_script reads a track's file,
 * ChaoRaceCam and the track's name, into race_camera_script */
#include <string.h>
#include "chao/race_record.h"
#include "chaostgrace/camera_key.h"
#ifdef NONMATCHING
/* WORK_AREA and its end, which bound this side's read of a script */
#include "system/areas.h"
#endif
#include "system/loader.h"

struct race_camera_script race_camera_script;
static const char *track_names[RACE_RECORD_TRACKS];

#ifdef NONMATCHING
/* this side returns the script it read, or null for one it refuses */
const struct race_camera_script *race_read_camera_script(int track)
#else
void race_read_camera_script(int track)
#endif
{
    char name[256];
#ifdef NONMATCHING
    /* this side refuses a track outside track_names, which the name below
     * indexes */
    struct race_camera_script *script = (struct race_camera_script *)WORK_AREA;
    Sint32 size;
    unsigned int i;

    if ((unsigned int)track >= RACE_RECORD_TRACKS) {
        return NULL;
    }
#endif
    strcpy(name, "ChaoRaceCam");
    strcat(name, track_names[track]);
    strcat(name, ".bin");
#ifdef NONMATCHING
    size = loader_read_file_checked(name, script, WORK_AREA_END - WORK_AREA);
    /* the file holds script->path whole, then script->position up to its first
     * key whose camera.active is 0. path and position must each hold such a
     * key. position's keys up to it must start within the file. the script
     * past the file is zeroed */
    if (size <= (Sint32)sizeof(script->path) ||
        (Uint32)size > sizeof(*script)) {
        return NULL;
    }
    for (i = 0; i < RACE_CAMERA_KEYS; i++) {
        if (!script->path.keys[i].camera.active) {
            break;
        }
    }
    if (i == RACE_CAMERA_KEYS) {
        return NULL;
    }
    for (i = 0; i < RACE_CAMERA_KEYS; i++) {
        if (sizeof(script->path) + i * sizeof(script->position[0]) >=
            (Uint32)size) {
            return NULL;
        }
        if (!script->position[i].camera.active) {
            memset(WORK_AREA + size, 0, sizeof(*script) - (Uint32)size);
            return script;
        }
    }
    return NULL;
#else
    loader_read_file(name, &race_camera_script);
#endif
}

static const char *track_names[RACE_RECORD_TRACKS] = {
    "BegSwm", "BegFly", "BegRun", "BegPow", "JwlSwm",
    "JwlFly", "JwlRun", "JwlPow", "JwlIL",  "JwlLng"};
