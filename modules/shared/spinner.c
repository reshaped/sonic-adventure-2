/* the spinner of shared/spinner.h: shared/shaft.c compiled a second time */
#include "shared/spinner.h"

/* NOLINTBEGIN(readability-identifier-naming) */
#define shaft_init spinner_init
#define shaft_texlist spinner_texlist
#define shaft_texlists spinner_texlists
#define shaft_object spinner_object
#define shaft_motion spinner_motion
/* NOLINTEND(readability-identifier-naming) */

#include "shared/shaft.c" /* NOLINT(bugprone-suspicious-include) */
