/* the course and karts of a cart stage or a kart race. the calls below load
 * and build them */
#ifndef CART_COURSE_H
#define CART_COURSE_H
#include <ninja.h>
#include <sg_xpt.h>

#include "object/set.h"

enum cart_course_mode {
    CART_TAILS,
    CART_ROUGE,
    CART_BEGINNER,
    CART_STANDARD,
    CART_EXPERT,
    CART_DOWNLOAD
};
enum cart_model_id {
    CART_MODEL_TAILS_STORY,
    CART_MODEL_ROUGE_STORY,
    CART_MODEL_SONIC,
    CART_MODEL_KNUCKLES,
    CART_MODEL_TAILS,
    CART_MODEL_EGGMAN,
    CART_MODEL_SHADOW,
    CART_MODEL_ROUGE,
    CART_MODEL_DOWNLOAD,
    CART_MODELS
};

struct cart_model {
    Sint32 id;
    NJS_CNK_OBJECT *object, *second;
    NJS_TEXLIST *textures;
    void *model_buffer, *texture_buffer;
};

/* a set file as cart_set holds it: its head and up to 1023 entries, 32 KiB in
 * all */
struct cart_set_file {
    struct set_head head;
    struct set_entry entries[1023];
};
union cart_set_buffer {
    struct cart_set_file file;
    Uint32 words[8192];
};
extern union cart_set_buffer cart_set;

/* cart_setup.play_mode: a kart race of CART_RACERS karts, or a story race of
 * one, on STAGE_CART_1 */
enum cart_play_mode { CART_PLAY_RACE, CART_PLAY_STORY };
struct cart_setup {
    Sint32 frames, state;
    Sint8 *pieces;
    Sint32 piece_count;
    struct cart_model models[CART_MODELS];
    Uint8 unk_0e8[0x78];
    Sint32 play_mode;
    Sint32 course_mode;
};
extern struct cart_setup cart_setup;

struct cart_tuning {
    Float acceleration, braking, coasting, cruise_speed, gravity;
    Float unk_14, drift_grip, drift_min_speed, unk_20, max_speed;
};
struct cart_course {
    struct cart_tuning tuning;
    struct {
        Float scale, per_level, above_one;
    } speed[2];
    Sint32 special;
    /* the first byte of the music's name, which runs on past the record to its
     * terminator */
    char music[1];
};

struct cart_archive {
    Sint32 model_offset, model_size;
    Sint32 texlist_offset, texlist_size;
    Sint32 pvm_offset;
};

struct cart_model_request {
    NJS_CNK_OBJECT **object;
    NJS_TEXLIST **textures;
    struct cart_archive *archive;
    void **model_buffer, **texture_buffer;
#ifdef NONMATCHING
    /* the archive's size, by which this side's cart_model_unpack bounds its
     * reads */
    Uint32 archive_size;
#endif
};
extern NJS_TEXLIST cart_tails_textures, cart_rouge_textures, cart_race_textures;
int cart_load_download_model(void);
void cart_init_models(void);
void cart_release_models(void);
void cart_load_model_textures(void);
void cart_release_model_textures(void);
/* Unpack the archive into SH heap storage owned by the caller. The port
 * takes the declared archive size, initializes both texture outputs for a
 * textureless model and leaves every output untouched on failure. The
 * original uses these output slots as its allocation scratch space. */
int cart_model_unpack(struct cart_model_request *request);
int cart_download_load(void);
#ifdef NONMATCHING
/* this side replaces the download model from an archive of the given size. it
 * bounds each copy by its destination's capacity */
Bool cart_replace_download_model(const void *archive, Uint32 size);
Uint32 cart_download_model_size(void);
void cart_download_copy_set(void *to, Uint32 capacity);
void cart_download_copy_layout(void *to, Uint32 capacity);
#else
void cart_download_copy_set(void *to);
void cart_download_copy_layout(void *to);
#endif
struct cart_archive *cart_download_model(void);
struct cart_course *cart_download_course(void);
int cart_build_course(const Sint8 *pieces, int count);
void cart_start_course_draw(void);
void cart_start_course_collision(void);
void cart_select_layout(int mode);
void cart_set_tuning(void);
void cart_camera_start(int screen);
void cart_background_start(void);
/* end the background's task at its next step. no code calls it */
void cart_background_stop(void);
extern Float cart_course_lowest;
extern Float cart_speed_scale, cart_speed_per_level, cart_speed_above_one;
extern struct cart_tuning cart_tunings[CART_MODELS];
#endif
