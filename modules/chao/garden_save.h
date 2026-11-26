/* the garden's record in the world file, and the calls that read, write and
 * fill the file */
#ifndef CHAO_GARDEN_SAVE_H
#define CHAO_GARDEN_SAVE_H
#include <ninja.h>
#include <sg_xpt.h>

#include "chao/identity.h"
#include "chao/race_state.h"

#define CHAO_GARDENS 3
/* bits of garden_save.flags. the lobby places the gate to each place whose
 * bit is set */
#define GARDEN_KINDER_OPEN 0x4
#define GARDEN_HERO_OPEN 0x10
#define GARDEN_DARK_OPEN 0x40
/* bits of garden_save.flags the garden editor names NEUT_PLAY, HERO_PLAY
 * and DARK_PLAY. the race entrance counts the Hero or Dark Garden open while
 * its OPEN or PLAY bit is set */
#define GARDEN_NEUTRAL_PLAY 0x2
#define GARDEN_HERO_PLAY 0x20
#define GARDEN_DARK_PLAY 0x80

#define CHAO_SAVE_SLOTS 24
#define GARDEN_TREES 6
#define GARDEN_TREE_SLOTS 8

extern NJS_POINT3 garden_tree_layouts[CHAO_GARDENS][GARDEN_TREES];

struct garden_tree_save {
    Uint8 kind, stage, growth, water;
    Sint8 fruit[3];
    Uint8 angle;
};

/* a bit of garden_class.flags: the seat's lesson has run its time */
#define GARDEN_CLASS_FINISHED 1
struct garden_class {
    Sint8 lesson[4];
    Sint32 start_time[4];
    Sint8 count;
    Uint8 flags[4];
};

struct garden_save {
    struct garden_origin origin;
    Sint32 chao_slots; /* CHAO_SAVE_SLOTS once a garden is loaded or made */
    Uint32 flags, awards;
    Sint32 frames;
    /* a garden's trees are the first GARDEN_TREES of its row */
    struct garden_tree_save trees[CHAO_GARDENS][GARDEN_TREE_SLOTS];
    struct garden_race_save race;
    struct garden_class classroom;
    Uint8 unk_320[24];
};

/* where a read or a write of the card's world file stands. GARDEN_FILE_INVALID
 * is a file the read found damaged */
enum garden_file_state {
    GARDEN_FILE_BUSY = 1,
    GARDEN_FILE_OK,
    /* the card holds no world file. only the GCC side's read sets it */
    GARDEN_FILE_ABSENT,
    GARDEN_FILE_ERROR,
    GARDEN_FILE_INVALID
};

#ifdef NONMATCHING
struct garden_card_request;
/* copy a world file's payload of size bytes to request->data, and zero the
 * rest of request->size. a payload must hold the garden's bytes and every
 * saved Chao. each lesson, tree kind and tree stage must be in range. any
 * other payload returns FALSE, and copies nothing */
Bool garden_file_stage(struct garden_card_request *request, const void *payload,
                       Uint32 size);
/* settle a read that ended in a state other than GARDEN_FILE_OK. returns
 * whether the garden must be written to the card. where the card holds no
 * file, a new garden starts, and the result is TRUE. after any other failure,
 * what the card holds is unknown. garden_file_write then writes nothing until
 * a read succeeds. a new garden starts only where none is loaded */
Bool garden_file_recover(int state);
#endif
/* start reading the card's world file */
void garden_file_read(void);
/* once garden_file_state gives GARDEN_FILE_OK, copy the file read into the
 * saved garden and Chao. the result carries no meaning */
int garden_file_unpack(void);
/* gather the garden, and start writing it to the card */
void garden_file_write(void);
int garden_file_state(void);
/* pack every live Chao, egg and tree, and the race state, into the saved
 * records */
void garden_file_gather(void);
void garden_gather_race(void);
/* make a new garden in the saved records: a new origin, and the first trees,
 * races and eggs */
void garden_file_init(void);
/* plant the current area's trees from its saved records. the area must be a
 * garden */
void garden_load_trees(void);
struct garden_save *garden_save_record(void);
struct garden_origin *garden_save_origin(void);
int garden_save_slots(void);
struct garden_tree_save *garden_file_trees(void);
const struct garden_race_save *garden_file_race_save(void);
struct garden_class *garden_file_class(void);
Bool garden_flag_test(Uint32 mask);
void garden_flag_set(Uint32 mask);
void garden_flag_clear(Uint32 mask);
/* an award is a bit of garden_save.awards, numbered as a race course gives
 * it */
Bool garden_has_award(int bit);
void garden_add_award(int bit);

#endif
