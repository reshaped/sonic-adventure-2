/* speed lines: short lines that streak back from a running player */
#ifndef EFFECT_SPEED_LINES_H
#define EFFECT_SPEED_LINES_H
#include <ninja.h>

/* start count lines near pos, each a task named LineExecute in level 3. no
 * more lines start where the heap can't give a task or its record. the slot
 * must hold a player, unchecked.
 *
 * a line's near end lies at random within 3 of pos + dir along each axis.
 * its far end lies along that offset, sqrt(|dir| / sprint_speed) from the near
 * end. sprint_speed is the record's. the far end then moves back by 2 up to 6
 * times dir. a line draws added to what lies behind it, and its third step ends
 * it. a step that finds the slot's work null ends it at once */
void speed_lines_create(int player, int count, NJS_POINT3 *pos,
                        NJS_VECTOR *dir);

#endif
