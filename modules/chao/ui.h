/* the names and rectangles CHAO.PRS draws on the screen for its areas */
#ifndef CHAO_UI_H
#define CHAO_UI_H
#include <ninja.h>
#include <sg_xpt.h>

struct task;
void garden_create_name_label(struct task *chao);
void garden_draw_world_name(NJS_POINT3 *position, const char *name,
                            Uint32 color, Float scale);
void garden_draw_name(NJS_POINT2 *position, const char *name, Uint32 color,
                      Float scale, int center);
void garden_draw_plain_rect(int left, int top, int right, int bottom,
                            Uint32 color);
void garden_draw_rect(int left, int top, int right, int bottom, Float depth,
                      Uint32 color);

#endif
