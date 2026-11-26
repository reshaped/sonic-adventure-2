/* a color for each kind of animal, which no code reads */
#include <ninja.h>

#include "object/animal.h"

/* the words have no braces of their own: SHC takes them with braces around
 * the array only */
/* NOLINTBEGIN(clang-diagnostic-missing-braces) */
static NJS_COLOR colors[ANIMAL_KINDS] = {
    0xff0000c0U, 0xff00ffffU, 0xff000000U, 0xff000000U, 0xffc0a000U,
    0xffc04000U, 0xff000000U, 0xff000000U, 0xff000040U, 0xff000000U,
    0xff000000U, 0xff000000U, 0xff000000U, 0xff000000U, 0xff0040ffU,
    0xff000000U, 0xff000000U, 0xff000000U, 0xff000000U, 0xff000000U,
    0xff000000U,
};
/* NOLINTEND(clang-diagnostic-missing-braces) */
