/* the builds of the objects the stages share, which one call runs in turn */
#include <sg_xpt.h>

#include "shared/cage.h"
#include "shared/chao_box.h"
#include "shared/dash_panel.h"
#include "shared/dynamite.h"
#include "shared/iron_ball.h"
#include "shared/iron_box.h"
#include "shared/item_box.h"
#include "shared/messenger.h"
#include "shared/object_builds.h"
#include "shared/reel.h"
#include "shared/ring.h"
#include "shared/search_box.h"
#include "shared/spring.h"
#include "shared/weight.h"
#include "shared/wood_box.h"

Bool objects_fog_off = FALSE;

void object_builds_init(void)
{
}

void object_builds_exit(void)
{
}

Uint8 *object_builds_run(Uint8 *buf)
{
    buf = ring_build(buf);
    buf = messenger_build_models(buf);
    buf = cage_build(buf);
    buf = wood_box_build(buf);
    buf = chao_box_build(buf);
    buf = iron_box_build(buf);
    buf = iron_ball_build(buf);
    buf = item_box_build(buf);
    buf = reel_build(buf);
    buf = spring_build(buf);
    buf = dash_panel_build(buf);
    buf = weight_build(buf);
    buf = search_box_build(buf);
    buf = dynamite_build(buf);
    return buf;
}
