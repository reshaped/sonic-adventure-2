/* the solid box, SOLIDBOX in a stage's object list: a box no player breaks.
 * it falls when a box under it breaks */
#include "shared/solid_box.h"
#include <ninja.h>

#include "shared/solid_box/text.inc"
#include "shared/solid_box_art.h"

NJS_TEXLIST *solid_box_texlists[2] = {
#include "shared/solid_box/solid_box_texlists.inc"
};

#include "shared/solid_box_task.h" /* IWYU pragma: keep -- the code it compiles here */
