/* a track's camera script, which race_read_camera_script reads from
 * ChaoRaceCam<track>.bin. it holds a bank of path keys, by course progress,
 * and a bank of position keys. a key whose camera.active is 0 ends its
 * bank */
#ifndef CHAOSTGRACE_CAMERA_KEY_H
#define CHAOSTGRACE_CAMERA_KEY_H
#include <ninja.h>
#include <sg_xpt.h>

enum race_target_mode { RACE_TARGET_FIX, RACE_TARGET_MOVE, RACE_TARGET_CHASE };
enum race_position_mode { RACE_POS_FIX, RACE_POS_MOVE, RACE_POS_PARALLEL };

struct race_camera_source {
    Sint8 active;
    Uint8 target_mode, position_mode;
    Uint8 unk_03[5];
    NJS_POINT3 target;
    Uint8 unk_14[20];
    NJS_POINT3 position;
    NJS_VECTOR position_step;
};

struct race_path_key {
    struct race_camera_source camera;
    Uint16 position_frames;
    Uint8 unk_42[26];
    Float progress;
};
struct race_position_key {
    struct race_camera_source camera;
    Uint8 unk_40[52];
};

#define RACE_CAMERA_KEYS 128
struct race_camera_script {
    union {
        struct race_path_key keys[RACE_CAMERA_KEYS];
        /* the bank's first 464 words, which race_store_camera_keys zeroes
         * before it stores the keys */
        Uint32 clear_words[464];
    } path;
    struct race_position_key position[RACE_CAMERA_KEYS];
};
extern struct race_camera_script race_camera_script;

#ifdef NONMATCHING
/* Scratch storage is valid until the next disc read. */
const struct race_camera_script *race_read_camera_script(int track);
#else
void race_read_camera_script(int track);
#endif

#endif
