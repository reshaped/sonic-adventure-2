/* the kindergarten's doors, the principal and the doctor, as the rooms place
 * them */
#ifndef CHAOSTGKINDER_OBJECTS_H
#define CHAOSTGKINDER_OBJECTS_H
#include <ninja.h>

struct task;

extern NJS_TEXLIST kinder_object_texlist;

void kinder_door_init(struct task *task);
/* the door in slot, 0 to 7, which its placement's ang.x names. open and
 * close only a door the current room has placed */
struct task *kinder_door(int slot);
void kinder_door_open(struct task *task);
void kinder_door_close(struct task *task);

void kinder_principal_prop_init(struct task *task);
void kinder_principal_init(struct task *task);
void kinder_doctor_init(struct task *task);

#endif
