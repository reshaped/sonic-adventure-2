/* the credits, top to bottom. each line takes 40 pixels of the screen, a
 * blank one too. a role is gold, and an organization green */
#include "shared/staff_credits.h"

#include "shared/staff_credits/text.inc"

struct staff_line staff_lines[500] = {
#include "shared/staff_credits/staff_lines.inc"
};

int staff_line_count = sizeof(staff_lines) / sizeof(staff_lines[0]);
