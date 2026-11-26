/* the hunt for the three emerald pieces a stage hides, the task named
 * "SearchEmeraldsGameManager". the stage's objects tell it where a piece can
 * lie, and it places the pieces there. it draws the radar and the icons of
 * the pieces taken, and gives the hints. player/knuckles.c holds it */
#ifndef STAGE_HUNT_H
#define STAGE_HUNT_H
#include <ninja.h>
#include <sg_xpt.h>

/* the kinds of place a piece can lie in. each is named as an emerald's debug
 * view prints it, "B NORMAL" to "? INENEMY". a kind's letter names the piece
 * its places can hold. an EMERALD_KIND_INENEMY place, an enemy group's, can
 * hold C or B. in MISSION_HD, an EMERALD_KIND_FINAL place holds the piece of
 * the slot its id names */
#define EMERALD_KINDS 11
#define EMERALD_KIND_B_NORMAL 0
#define EMERALD_KIND_C_NORMAL 1
#define EMERALD_KIND_B_HIDDEN 2
#define EMERALD_KIND_C_HIDDEN 3
#define EMERALD_KIND_A_UNDERGND 4
#define EMERALD_KIND_B_UNDERGND 5
#define EMERALD_KIND_A_2P_UNDGND 6
#define EMERALD_KIND_A_PATHMOVE 7
#define EMERALD_KIND_A_1P_TECH 8
#define EMERALD_KIND_FINAL 9
#define EMERALD_KIND_INENEMY 10

/* the hunt's slots, one for each piece, by its letter. the current piece is
 * the first whose slot holds no taken piece */
#define HUNT_SLOT_C 0
#define HUNT_SLOT_B 1
#define HUNT_SLOT_A 2
#define HUNT_PIECES 3

/* a piece's kind before the hunt places it, and once a player takes it. a
 * taken piece's id is the number of the player who took it, 0 or 1 */
#define HUNT_PIECE_EMPTY (-1)
#define HUNT_PIECE_TAKEN (-2)

/* a place a piece can lie in, or the piece the hunt placed there */
struct hunt_piece {
    Sint8 kind; /* an EMERALD_KIND_, or a HUNT_PIECE_ */
    Sint8 id; /* among the places of its kind */
    Uint8 unk_02[2];
    NJS_POINT3 pos;
};

/* the hunt's modes, each by what the hunt's step does in it */
enum hunt_mode {
    HUNT_START = 0, /* allocates the lists of places */
    /* lists the places the stage's objects tell of, until set_passed */
    HUNT_WAIT = 1,
    HUNT_SET_DONE = 2, /* goes on to HUNT_PLACE in the same step */
    /* places the pieces, or in MISSION_HD keeps those the stage placed */
    HUNT_PLACE = 3,
    /* beeps and says the voices until every piece is taken. in a two-player
     * game it ends once a player holds two */
    HUNT_PLAY = 4,
    HUNT_WON = 5, /* clears the stage for its winner */
    /* empties the hunt once play starts again, and goes back to HUNT_WAIT */
    HUNT_FINISHED = 6,
    HUNT_RESTARTED = 7 /* as HUNT_FINISHED, but keeps the pieces taken */
};

/* the hunt's record, its task's extra */
struct hunt {
    Sint8 mode; /* an enum hunt_mode */
    /* requests the hunt's next step reads and clears. a finish of 1 sends the
     * hunt to HUNT_FINISHED. without it, a restart of 1 sends the hunt to
     * HUNT_RESTARTED outside MISSION_HD. a hunt in HUNT_START or HUNT_WAIT
     * stays where it is */
    Sint8 finish;
    Sint8 restart;
    /* how many hints hunt_next_hint has given for the current piece, up to
     * HUNT_HINTS. a taking of the current piece and each emptying of the hunt
     * set it to 0 */
    Sint8 hints;
    /* bit n is set once player n comes within 100 of the current piece. a
     * Knuckles or a Rouge then says its voice. a taking and each emptying of
     * the hunt clear the bits */
    Sint8 voices;
    Sint8 placed; /* the pieces placed and not yet taken */
    /* how many places c_places, b_places, a_places and enemy_places hold */
    Sint8 c_count;
    Sint8 b_count;
    Sint8 a_count;
    Sint8 enemy_count;
    /* the last place in error. one of a pair listed twice is an error. so is
     * an EMERALD_KIND_FINAL place whose id names no slot, or whose slot holds
     * a piece already. error_kind is HUNT_PIECE_EMPTY for none */
    Sint8 error_kind;
    Sint8 error_id;
    NJS_POINT3 error_pos;
    /* the other place of the last pair listed twice. its kind is
     * HUNT_PIECE_EMPTY for none */
    struct hunt_piece twin;
    struct hunt_piece pieces[HUNT_PIECES]; /* by slot */
    /* the places the stage's objects told of, by the piece they can hold */
    struct hunt_piece *c_places;
    struct hunt_piece *b_places;
    struct hunt_piece *a_places;
    struct hunt_piece *enemy_places;
    /* the steps since the hunt started or emptied, or a piece was taken. it
     * stops at HUNT_TIME_MAX */
    Uint32 time;
    /* the radar's and the icons' textures: KGAMETEX, or KGAMETEXSH on
     * STAGE_SECURITY_HALL, or KGAMETEXDZ on STAGE_DEATH_CHAMBER and
     * STAGE_EGG_QUARTERS */
    NJS_TEXLIST *texlist;
};
#define HUNT_TIME_MAX 0x7fffffff

/* the hunt that runs, or null where none does */
extern struct hunt *hunt_current;

/* the hints a search box gives for each place */
#define HUNT_HINTS 3

/* a place's hints. kind and id name the place, as a struct hunt_piece's do.
 * messages holds the index of each hint's message in the hint file. a kind
 * of HUNT_HINT_END ends a stage's table of them */
struct hunt_hint {
    Sint8 kind;
    Sint8 id;
    Sint16 messages[HUNT_HINTS];
};
#define HUNT_HINT_END (-1)

/* start the hunt, where none runs, with the radar for its draw. no hunt
 * starts past STAGE_STG_END, or on STAGE_VERSUS_KNUCKLES, STAGE_RELAY_ROUGE
 * or STAGE_RELAY_KNUCKLES. none starts in MISSION_RINGS or MISSION_CHAO.
 * nothing starts where task_create or syCalloc fails */
void hunt_start(void);
/* tell the hunt, where one runs, of a place of kind and id at pos. the hunt
 * lists a place only in HUNT_WAIT to HUNT_PLACE. it lists one of these kinds
 * only as follows:
 * - EMERALD_KIND_A_2P_UNDGND, in a two-player game
 * - EMERALD_KIND_A_1P_TECH, in a one-player game
 * - EMERALD_KIND_A_1P_TECH and EMERALD_KIND_A_PATHMOVE, in a one-player game
 *   on STAGE_PUMPKIN_HILL or STAGE_EGG_QUARTERS, once player 0 has
 *   UPGRADE_KNUCKLES_CLAWS or UPGRADE_ROUGE_NAILS
 * - EMERALD_KIND_FINAL, in MISSION_HD alone. there it places slot id's piece
 *   at once, where the slot is empty
 *
 * each list holds HUNT_PLACES places, or HUNT_ENEMY_PLACES for the enemy
 * groups'. B's choices, its places and the groups' that C's pick leaves, must
 * number HUNT_B_CHOICES or fewer. neither bound is checked */
void hunt_add_place(int kind, Sint8 id, const NJS_POINT3 *pos);
#define HUNT_PLACES 48
#define HUNT_ENEMY_PLACES 128
#define HUNT_B_CHOICES 93
/* tell the hunt that player took the piece of kind and id. for player 0 or 1,
 * the piece becomes HUNT_PIECE_TAKEN with player for its id. the hunt then
 * shows the piece taken, with its sound. it scores HUNT_PIECE_SCORE and a
 * bonus by its time and hints. outside MISSION_TIMED, the stage's start, where
 * it has one, becomes the player's restart point. any other player gives the
 * piece back to the world, by player_hooks.create_piece at pos. a null pos
 * gives it back where the piece lies. nothing happens without a hunt or a
 * piece of kind and id */
void hunt_take_piece(int kind, Sint8 id, int player, NJS_POINT3 *pos);
#define HUNT_PIECE_SCORE 500
/* hunt_find_piece's result for a piece no slot holds, and for one in slot */
#define HUNT_NOT_FOUND 0
#define HUNT_FOUND(slot) ((slot) + 1)
/* the slot that holds the piece of kind and id, plus 1, or 0 where none
 * does. it returns HUNT_NONE where no hunt runs. where pos isn't null, the
 * piece found takes *pos for its place */
#define HUNT_NONE 4
int hunt_find_piece(int kind, Sint8 id, const NJS_POINT3 *pos);

/* set or clear the table of hints hunt_next_hint reads */
void hunt_set_hints(struct hunt_hint *hints);
void hunt_clear_hints(void);
/* load the hint file name, with the text language's suffix and .prs. it
 * becomes the file whose messages hunt_next_hint gives. a hint file starts
 * with a Sint32 for each message, its offset from the file's start. it
 * returns the file, or null where loader_read_prs_alloc fails. hunt_next_hint
 * then keeps the file it had. the caller frees the file with syFree, but not
 * while hunt_next_hint may read it. name must take 10 characters or fewer,
 * unchecked, for the file's name to fit its 16 bytes */
Sint32 *hunt_load_hint_file(const char *name);
/* count a hint for the current piece in hints, up to HUNT_HINTS. it returns
 * the message of the hint it counted. once hints is HUNT_HINTS, each call
 * returns the last hint's message again. it counts nothing and returns null
 * without a hunt, a table or a file. where the table lists no hints for the
 * piece, it counts nothing. it then returns (char *)3, which is no message */
char *hunt_next_hint(void);

#endif
