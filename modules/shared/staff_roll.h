/* the staff roll scrolls the credits up the screen and fades a story's
 * photographs in and out. ENDING.PRS and EVENT.PRS each link it.
 * OTHERPRINT.PRS links staff_roll_texlist and staff_roll_cells without it */
#ifndef SHARED_STAFF_ROLL_H
#define SHARED_STAFF_ROLL_H
#include <ninja.h>
#include <sg_xpt.h>

enum staff_line_kind {
    STAFF_LINE_BLANK,
    STAFF_LINE_TEXT, /* text in the font of capitals, digits and punctuation */
    STAFF_LINE_ALT_TEXT, /* text in the font of capitals and small letters */
    STAFF_LOGO_ADX,
    STAFF_LOGO_SUPER_78,
    STAFF_LOGO_SOFDEC,
    STAFF_LOGO_10TH_ANNIVERSARY,
    STAFF_LOGO_SOAP,
    STAFF_LOGO_SEGA,
    STAFF_LOGO_SONICTEAM,
    STAFF_LINE_KINDS
};

/* TRUE from staff_roll_start until the roll ends. the roll ends 60 frames
 * after the last line's y passes -240 */
extern Bool staff_roll_running;
/* load the credits' textures, and story's photographs where story is
 * SEQUENCE_HERO or SEQUENCE_DARK. story must be one of those, or SEQUENCE_NONE
 * for a roll without photographs */
void staff_roll_load(int story);
void staff_roll_release(void);
/* start the roll, after staff_roll_load. start it at most once per load of its
 * program. a second start takes up the lines and the photograph where the
 * first left them. the roll's duration is the frames from its start until the
 * last line's y reaches -40. it is the hero story's where
 * game_flags[FLAG_SEQUENCE] is SEQUENCE_HERO, and the dark story's otherwise */
void staff_roll_start(void);
/* set the roll's duration to frames, which must be positive. call it right
 * after staff_roll_start, which sets a duration of its own */
void staff_roll_set_duration(int frames);

/* staff_roll_cells holds the cells of the STAFF_GLYPHS glyphs, then the logos'
 * in the order of their kinds */
#define STAFF_GLYPHS 109
#define STAFF_LOGO_CELL(kind) ((kind) + (STAFF_GLYPHS - STAFF_LOGO_ADX))
#define STAFF_ROLL_CELLS (STAFF_GLYPHS + (STAFF_LINE_KINDS - STAFF_LOGO_ADX))
/* the glyphs' and logos' textures, and each story's photographs */
extern NJS_TEXLIST staff_roll_texlist;
extern NJS_TEXLIST staff_hero_texlist;
extern NJS_TEXLIST staff_dark_texlist;
extern NJS_TEXANIM staff_roll_cells[STAFF_ROLL_CELLS];
/* the story staff_roll_load was given */
extern int staff_roll_story;

void staff_glyphs_measure(void);
/* draw the logo of kind, a STAFF_LOGO_ kind, centered on the credits' column
 * with its middle at y */
void staff_logo_draw(enum staff_line_kind kind, Float y);
/* draw text centered on the credits' column, with the tops of its glyphs at y.
 * it takes kind's font, STAFF_LINE_TEXT's or STAFF_LINE_ALT_TEXT's. characters
 * past the 128th are dropped. a character missing from the font leaves a gap.
 * the call skips a line whose y lies outside -50 to 550. a call that doesn't
 * skip its line leaves the constant material at color */
void staff_text_draw(enum staff_line_kind kind, NJS_ARGB *color,
                     const char *text, Float y);
/* draw texture with its top left where pos->x and pos->y give, at priority
 * pos->z. attributes is njDrawSprite2D's attr. texture is one of
 * staff_hero_texlist's where staff_roll_story is SEQUENCE_HERO, and of
 * staff_dark_texlist's otherwise */
void staff_photo_draw(int texture, const NJS_POINT3 *pos, int attributes);

#endif
