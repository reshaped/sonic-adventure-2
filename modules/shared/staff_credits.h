/* the credits' lines, which the staff roll scrolls in ENDING.PRS and
 * EVENT.PRS */
#ifndef SHARED_STAFF_CREDITS_H
#define SHARED_STAFF_CREDITS_H
#include <ninja.h>

#include "shared/staff_roll.h"

struct staff_line {
    enum staff_line_kind kind;
    NJS_ARGB color;
    const char *text;
};
extern struct staff_line staff_lines[];
extern int staff_line_count;

#endif
