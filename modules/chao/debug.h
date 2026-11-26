/* the garden editor's pages of a Chao's parameters and of what it knows, and
 * the gauge they draw */
#ifndef CHAO_DEBUG_H
#define CHAO_DEBUG_H
#include <sg_xpt.h>

struct task;
/* the controller the debug pages and garden_free_camera read: controller
 * three */
#define CHAO_DEBUG_PAD 2
void chao_debug_gauge(int column, int row, Uint32 width, Uint32 left_color,
                      Uint32 right_color);
void chao_debug_parameters(struct task *page);
void chao_debug_knowledge(struct task *page);

#endif
