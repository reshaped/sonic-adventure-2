/* SOLIDBOX variant with its own texture archive and stage registration. */
#include "shared/solid_box_e.h"
#include <ninja.h>

#include "shared/solid_box_e/text.inc"
#include "shared/solid_box_art.h"

NJS_TEXLIST *solid_box_e_texlists[2] = {
#include "shared/solid_box_e/solid_box_e_texlists.inc"
};

/* NOLINTNEXTLINE(readability-identifier-naming) */
#define solid_box_init solid_box_e_init
#include "shared/solid_box_task.h" /* IWYU pragma: keep -- the code it compiles here */
