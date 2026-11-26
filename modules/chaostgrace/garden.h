/* the Chao race overlay's entry point and SET list */
#ifndef CHAOSTGRACE_GARDEN_H
#define CHAOSTGRACE_GARDEN_H
#include "object/set.h"
struct task;

extern struct object_kind chaostgrace_kinds[12];
extern struct object_list chaostgrace_objects;
void chaostgrace_step(struct task *task);

#endif
