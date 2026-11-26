/* the input method's text, in five spans of 16-bit codes. a text entry draws
 * text, kana and roman before the cursor, and remaining_kana and
 * remaining_text after it */
#ifndef CHAOSTGKINDER_COMPOSITION_H
#define CHAOSTGKINDER_COMPOSITION_H
#include <sg_xpt.h>

enum kinder_composition_result {
    COMPOSE_EDITING,
    COMPOSE_ACCEPTED,
    COMPOSE_CANCELED
};

/* the most codes an edit's five spans hold together, unless
 * kinder_composition_set_limit sets another */
#define KINDER_COMPOSITION_LIMIT 127

struct kinder_composition {
    /* @hack type: result a Sint32 holding a kinder_composition_result, where
     * the enum would do */
    Sint32 result;
    Bool converting;
    Sint32 text_length, kana_length, roman_length;
    Sint32 remaining_kana_length, remaining_text_length;
    /* the codes committed before the cursor */
    Uint16 *text;
    /* the reading still to convert before the cursor. while converting, the
     * phrases chosen before the current one */
    Uint16 *kana;
    /* the letters not yet composed into kana. while converting, the current
     * phrase's candidate */
    Uint16 *roman;
    /* the reading still to convert after the cursor. while converting, the
     * reading after the current phrase */
    Uint16 *remaining_kana;
    /* the codes committed after the cursor */
    Uint16 *remaining_text;
};

/* start the conversion library, load the soft keyboard and point the spans
 * at their buffers */
void kinder_kana_start(void);
/* join remaining_text to text, and release the soft keyboard */
void kinder_kana_stop(void);
struct kinder_composition *kinder_composition(void);
/* begin an edit with empty spans and a limit of KINDER_COMPOSITION_LIMIT
 * codes, unless one runs */
void kinder_composition_start(void);
void kinder_composition_stop(void);
/* text takes initial's codes, up to the limit */
void kinder_composition_set_text(const Uint16 *initial);
/* open the soft keyboard, which the pad then types on */
void kinder_composition_use_pad(void);
/* set the most codes the five spans hold together to count */
void kinder_composition_set_limit(Sint32 count);
/* take key, a USB HID usage below WIDGET_PAD_UP, or a pad key. a usage
 * closes the soft keyboard. a pad key opens it, or types on it once open.
 * with no reading, Enter or Tab ends the edit as COMPOSE_ACCEPTED, and
 * Escape as COMPOSE_CANCELED */
void kinder_composition_input(Uint16 key);
#endif
