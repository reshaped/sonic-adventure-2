/* the colored torch, LIGHT in Pyramid Cave's object list: shared/color_torch.c
 * compiled again. its orange light is yellower than Death Chamber's */
#ifndef STG28_CAVE_COLOR_TORCH_H
#define STG28_CAVE_COLOR_TORCH_H

struct task;

/* the object list's step for LIGHT. it is color_torch_init of
 * shared/color_torch.h, with this copy's presets */
void cave_color_torch_init(struct task *task);

#endif
