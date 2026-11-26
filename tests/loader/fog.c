/* Optional stage and event fog files share a sector-sized transfer buffer. */
#include <assert.h>
#include <string.h>

#include "shared/fog.h"
#include "system/areas.h"

static Uint8 transfer[2048];
#undef WORK_AREA
#undef WORK_AREA_END
#define WORK_AREA transfer
#define WORK_AREA_END (transfer + sizeof(transfer))

#include "shared/fog.c"
#include "event/scene_fog.c"

static Sint32 bytes;
static int reads;

Sint32 loader_read_file_checked(const char *name, void *dest, Uint32 capacity)
{
    assert(name && dest == transfer && capacity == sizeof(transfer));
    reads++;
    memset(dest, 0xa5, capacity);
    return bytes;
}

void njPrint(Int location, const char *format, ...)
{
    (void)location;
    (void)format;
}

int main(void)
{
    struct fog_setting setting;
    struct fog_setting saved;

    fog_clear(&setting);
    saved = setting;
    bytes = -1;
    assert(!fog_read("missing.fog", &setting));
    assert(memcmp(&setting, &saved, sizeof(setting)) == 0);
    bytes = sizeof(setting) - 1;
    assert(!fog_read("short.fog", &setting));
    assert(memcmp(&setting, &saved, sizeof(setting)) == 0);
    bytes = sizeof(setting);
    assert(fog_read("scene.fog", &setting));
    assert(memcmp(&setting, transfer, sizeof(setting)) == 0);

    reads = 0;
    event_fog_load(2, EVENT_SCENE_FOGS + 1);
    assert(reads == EVENT_SCENE_FOGS);
    event_fog_select(EVENT_SCENE_FOGS - 1);
    assert(fog_from == &scene_fogs[EVENT_SCENE_FOGS - 1]);
    event_fog_select(EVENT_SCENE_FOGS);
    event_fog_select(-1);
    assert(fog_from == &scene_fogs[EVENT_SCENE_FOGS - 1]);
    event_fog_select(0);
    assert(fog_from == &scene_fogs[0]);
    return 0;
}
