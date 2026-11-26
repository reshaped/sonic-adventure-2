/* the pictures a Chao draws on the garden floor, from its art lessons or
 * with a crayon */
#ifndef CHAO_DRAWING_H
#define CHAO_DRAWING_H
#include <sg_xpt.h>
struct task;
/* set a behavior that draws a picture ahead of the Chao, and return TRUE.
 * half the time, a Chao that knows an art lesson draws a lesson's picture.
 * failing that, a Chao with a crayon draws. four times in five it draws a
 * player at 50 affinity or more, where one is. otherwise it draws a picture
 * for its form. the crayon's choice lowers each such player's affinity to
 * 50. FALSE where 100 pictures stand or no picture is chosen, which sets
 * nothing */
Bool chao_try_drawing(struct task *chao);
#endif
