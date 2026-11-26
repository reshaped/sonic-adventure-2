/* the kindergarten's message font. a string holds a character to each
 * Uint16. a character above 0xff is a Shift JIS code. any other is a byte
 * of the Latin font, whose bytes below ' ' are spaces. sizes, spacing,
 * leading and positions are in screen pixels. kinder_font_start loads the
 * font and starts its task, and kinder_font_stop ends them */
#ifndef CHAOSTGKINDER_FONT_H
#define CHAOSTGKINDER_FONT_H
#include <sg_xpt.h>

/* a string and the box it fills. a negative length, such as
 * KINDER_WHOLE_STRING, runs to the terminating zero. spacing follows each
 * character, and leading each line. x and y are the cursor in the box. a draw
 * or a measure leaves it past the last character it takes */
struct kinder_text_layout {
    const Uint16 *string;
    int length;
    Float width, height;
    Float spacing, leading;
    Float x, y;
};
#define KINDER_WHOLE_STRING (-1)
/* the spacing and leading the kindergarten's widgets and forms give text */
#define KINDER_TEXT_GAP 2

/* a layout, with its box's top left corner on the screen and its depth.
 * color is its glyphs' ARGB 8888 color */
struct kinder_text {
    struct kinder_text_layout layout;
    Float left, top, z;
    Uint32 color;
};

void kinder_font_start(void);
void kinder_font_stop(void);
/* each takes characters until the string ends, length runs out, or a new
 * line would pass height. a character past width starts a new line. a draw
 * starts one at '\n' too, where a measure takes '\n' as a space. a draw
 * returns the number of characters it took, and a measure that number
 * negated. the font keeps the glyphs of the 800 characters it took most
 * recently. a new character draws as nothing, or as the one it replaced,
 * until the font's task next runs. the scaled forms draw a glyph height
 * pixels tall. they draw it scale pixels wide for each GLYPH_SIZE of its
 * width */
int kinder_text_draw(struct kinder_text *text);
int kinder_text_draw_scaled(struct kinder_text *text, Float scale,
                            Float height);
int kinder_text_measure(struct kinder_text_layout *layout);
int kinder_text_measure_scaled(struct kinder_text_layout *layout, Float scale,
                               Float height);
#endif
