/* the harbor's stopper, STOPPER in Metal Harbor's object list. it is the
 * stopper of shared/stopper.h, MHSLIDER in the list, with a model of its own */
#ifndef STG10_HARBOR_STOPPER_H
#define STG10_HARBOR_STOPPER_H
#include <ninja.h>

struct task;

/* the stopper's texture lists for texture_load_shared, ended by null */
extern NJS_TEXLIST *harbor_stopper_texlists[];

/* the object list's step for STOPPER. it sets task up as stopper_init does,
 * with this stopper's own collision node and model. its destructor also sets
 * task->force and task->motion to null */
void harbor_stopper_init(struct task *task);

#endif
