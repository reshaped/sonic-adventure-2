/* Crazy Gadget's KAZ3, a marker whose model also lines the transport rails */
#ifndef STG22_RAIL_MARKER_H
#define STG22_RAIL_MARKER_H
#include <ninja.h>
#include <sg_xpt.h>

struct task;

/* the marker's texture list, object and build, which the rails' markers
 * draw. *build is null until gadget_rail_marker_build builds the model */
void gadget_rail_marker_art(NJS_TEXLIST **textures, NJS_CNK_OBJECT **object,
                            NJS_DIRECT_HEAD **build);
/* build the marker's first model at buffer, unless it is built, and return
 * the build's end. where the build would pass DEMO_AREA, it
 * builds nothing and returns buffer */
Uint8 *gadget_rail_marker_build(Uint8 *buffer);
/* the object list's init for KAZ3. it does nothing where task leaves at its
 * placement's range. otherwise it gives task the marker's step, draw and
 * destructor. pos places the marker, and ang turns it */
void gadget_rail_marker_init(struct task *task);
extern NJS_TEXLIST *gadget_rail_marker_texlists[];

#endif
