/* the GCC build's start of the Hero, Dark and Neutral Gardens' SET layout and
 * land */
#ifndef SHARED_GARDEN_LAYOUT_H
#define SHARED_GARDEN_LAYOUT_H

#include "chao/garden_scene.h"
#include "collision/world.h"
#include "object/set.h"
#include "system/loader.h"

/* load name's SET layout, start the set manager on objects with it, start
 * land, and return the layout. where any of them fails, it returns null with
 * nothing started or held. the caller's step can then return and run again */
static struct set_head *garden_start_layout(const char *name,
                                            struct object_list *objects,
                                            struct land_table *land)
{
    struct set_head *layout = garden_file_load(name);

    if (layout == NULL) {
        return NULL;
    }
    if (!set_start(objects, layout)) {
        loader_free(layout);
        return NULL;
    }
    if (!world_land_start(land)) {
        set_end();
        loader_free(layout);
        return NULL;
    }
    return layout;
}

#endif
