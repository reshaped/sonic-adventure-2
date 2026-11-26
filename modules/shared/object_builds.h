/* the builds of the objects the stages share, which one call runs in turn */
#ifndef SHARED_OBJECT_BUILDS_H
#define SHARED_OBJECT_BUILDS_H
#include <sg_xpt.h>

/* while objects_fog_off isn't 0, the objects that read it turn the fog off
 * around some builds and draws. it starts FALSE, and a stage's setup may set
 * it, as Lost Colony's does */
extern Bool objects_fog_off;

/* object_builds_init and object_builds_exit do nothing */
void object_builds_init(void);
void object_builds_exit(void);
/* run the objects' builds in turn. the first starts at buf, and each other
 * where the one before ended. it returns where the final build ended */
Uint8 *object_builds_run(Uint8 *buf);

#endif
