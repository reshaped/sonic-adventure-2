/* the left and right edges of each race track, as position keys. key k of
 * one edge pairs with key k of the other. the left edge's motion counts the
 * keys. the two swimming tracks, 0 and 4, share their edges */
#ifndef CHAO_COURSE_KEYS_H
#define CHAO_COURSE_KEYS_H
#include <ninja.h>

#include "chao/race_record.h"

struct track_edges {
    NJS_MKEY_F *left, *right;
};

static NJS_MKEY_F short_fly_left_keys[17] = {
#include "chao/course_keys/short_fly_left_keys.inc"
};
static NJS_MDATA2 short_fly_left_data = {
#include "chao/course_keys/short_fly_left_data.inc"
};
static NJS_MOTION short_fly_left_motion = {
#include "chao/course_keys/short_fly_left_motion.inc"
};

static NJS_MKEY_F short_fly_right_keys[17] = {
#include "chao/course_keys/short_fly_right_keys.inc"
};
static NJS_MDATA2 short_fly_right_data = {
#include "chao/course_keys/short_fly_right_data.inc"
};
static NJS_MOTION short_fly_right_motion = {
#include "chao/course_keys/short_fly_right_motion.inc"
};

static NJS_MKEY_F short_power_left_keys[5] = {
#include "chao/course_keys/short_power_left_keys.inc"
};
static NJS_MDATA2 short_power_left_data = {
#include "chao/course_keys/short_power_left_data.inc"
};
static NJS_MOTION short_power_left_motion = {
#include "chao/course_keys/short_power_left_motion.inc"
};

static NJS_MKEY_F short_power_right_keys[5] = {
#include "chao/course_keys/short_power_right_keys.inc"
};
static NJS_MDATA2 short_power_right_data = {
#include "chao/course_keys/short_power_right_data.inc"
};
static NJS_MOTION short_power_right_motion = {
#include "chao/course_keys/short_power_right_motion.inc"
};

static NJS_MKEY_F short_run_left_keys[24] = {
#include "chao/course_keys/short_run_left_keys.inc"
};
static NJS_MDATA2 short_run_left_data = {
#include "chao/course_keys/short_run_left_data.inc"
};
static NJS_MOTION short_run_left_motion = {
#include "chao/course_keys/short_run_left_motion.inc"
};

static NJS_MKEY_F short_run_right_keys[24] = {
#include "chao/course_keys/short_run_right_keys.inc"
};
static NJS_MDATA2 short_run_right_data = {
#include "chao/course_keys/short_run_right_data.inc"
};
static NJS_MOTION short_run_right_motion = {
#include "chao/course_keys/short_run_right_motion.inc"
};

static NJS_MKEY_F fly_left_keys[44] = {
#include "chao/course_keys/fly_left_keys.inc"
};
static NJS_MDATA2 fly_left_data = {
#include "chao/course_keys/fly_left_data.inc"
};
static NJS_MOTION fly_left_motion = {
#include "chao/course_keys/fly_left_motion.inc"
};

static NJS_MKEY_F fly_right_keys[44] = {
#include "chao/course_keys/fly_right_keys.inc"
};
static NJS_MDATA2 fly_right_data = {
#include "chao/course_keys/fly_right_data.inc"
};
static NJS_MOTION fly_right_motion = {
#include "chao/course_keys/fly_right_motion.inc"
};

static NJS_MKEY_F intelligence_left_keys[63] = {
#include "chao/course_keys/intelligence_left_keys.inc"
};
static NJS_MDATA2 intelligence_left_data = {
#include "chao/course_keys/intelligence_left_data.inc"
};
static NJS_MOTION intelligence_left_motion = {
#include "chao/course_keys/intelligence_left_motion.inc"
};

static NJS_MKEY_F intelligence_right_keys[63] = {
#include "chao/course_keys/intelligence_right_keys.inc"
};
static NJS_MDATA2 intelligence_right_data = {
#include "chao/course_keys/intelligence_right_data.inc"
};
static NJS_MOTION intelligence_right_motion = {
#include "chao/course_keys/intelligence_right_motion.inc"
};

static NJS_MKEY_F power_left_keys[36] = {
#include "chao/course_keys/power_left_keys.inc"
};
static NJS_MDATA2 power_left_data = {
#include "chao/course_keys/power_left_data.inc"
};
static NJS_MOTION power_left_motion = {
#include "chao/course_keys/power_left_motion.inc"
};

static NJS_MKEY_F power_right_keys[36] = {
#include "chao/course_keys/power_right_keys.inc"
};
static NJS_MDATA2 power_right_data = {
#include "chao/course_keys/power_right_data.inc"
};
static NJS_MOTION power_right_motion = {
#include "chao/course_keys/power_right_motion.inc"
};

static NJS_MKEY_F run_left_keys[73] = {
#include "chao/course_keys/run_left_keys.inc"
};
static NJS_MDATA2 run_left_data = {
#include "chao/course_keys/run_left_data.inc"
};
static NJS_MOTION run_left_motion = {
#include "chao/course_keys/run_left_motion.inc"
};

static NJS_MKEY_F run_right_keys[73] = {
#include "chao/course_keys/run_right_keys.inc"
};
static NJS_MDATA2 run_right_data = {
#include "chao/course_keys/run_right_data.inc"
};
static NJS_MOTION run_right_motion = {
#include "chao/course_keys/run_right_motion.inc"
};

static NJS_MKEY_F swim_left_keys[31] = {
#include "chao/course_keys/swim_left_keys.inc"
};
static NJS_MDATA2 swim_left_data = {
#include "chao/course_keys/swim_left_data.inc"
};
static NJS_MOTION swim_left_motion = {
#include "chao/course_keys/swim_left_motion.inc"
};

static NJS_MKEY_F swim_right_keys[31] = {
#include "chao/course_keys/swim_right_keys.inc"
};
static NJS_MDATA2 swim_right_data = {
#include "chao/course_keys/swim_right_data.inc"
};
static NJS_MOTION swim_right_motion = {
#include "chao/course_keys/swim_right_motion.inc"
};

static NJS_MKEY_F long_course_left_keys[83] = {
#include "chao/course_keys/long_course_left_keys.inc"
};
static NJS_MDATA2 long_course_left_data = {
#include "chao/course_keys/long_course_left_data.inc"
};
static NJS_MOTION long_course_left_motion = {
#include "chao/course_keys/long_course_left_motion.inc"
};

static NJS_MKEY_F long_course_right_keys[83] = {
#include "chao/course_keys/long_course_right_keys.inc"
};
static NJS_MDATA2 long_course_right_data = {
#include "chao/course_keys/long_course_right_data.inc"
};
static NJS_MOTION long_course_right_motion = {
#include "chao/course_keys/long_course_right_motion.inc"
};

static NJS_MOTION *track_lengths[RACE_RECORD_TRACKS] = {
#include "chao/course_keys/track_lengths.inc"
};
static struct track_edges track_edges[RACE_RECORD_TRACKS] = {
#include "chao/course_keys/track_edges.inc"
};

#endif
