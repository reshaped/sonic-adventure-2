#include <assert.h>
#include <math.h>

#include "system/areas.h"
#include "system/pad.h"

static struct pad_demo_frame demo;
#undef DEMO_AREA
#define DEMO_AREA (&demo)
#include "src/system/pad.c"

struct game_state game_state;
struct save_data save_data;
struct task_work *camera_task_work;
struct camera_place *camera_views[CAMERAS];
Sint32 camera_active_count;
static PDS_PERIPHERAL physical[PAD_PORTS];
static PDS_PERIPHERALINFO socket;

Float njSqrt(Float value)
{
    return (Float)sqrt(value);
}

const PDS_PERIPHERAL *pdGetPeripheral(Uint32 port)
{
    int index;
    for (index = 0; index < PAD_PORTS; ++index) {
        if (port == scan_ports[index]) {
            return &physical[index];
        }
    }
    return NULL;
}

const PDS_PERIPHERALINFO *pdGetPeripheralInfo(Uint32 port)
{
    (void)port;
    return &socket;
}

int main(void)
{
    int i;
    int enabled;
    int mask;
    for (i = 0; i < PAD_PORTS; ++i) {
        physical[i].support = PAD_SUPPORT;
        physical[i].x1 = 127;
    }
    game_state.rank = 1;
    game_state.two_players = 1;
    for (enabled = 0; enabled < 2; ++enabled) {
        for (mask = 0; mask < 4; ++mask) {
            game_state.pads_on = enabled;
            game_state.player_pads_on[0] = mask & 1;
            game_state.player_pads_on[1] = (mask >> 1) & 1;
            pad_scan();
            for (i = 0; i < PAD_PLAYERS; ++i) {
                if (i < GAME_PLAYERS && enabled && (mask & (1 << i))) {
                    assert(pad_sticks[i].stroke > 0.0F);
                } else {
                    assert(pad_sticks[i].stroke == 0.0F);
                    assert(pad_sticks_read[i].stroke == 0.0F);
                    assert(pad_stick_pushed[i] == 0);
                }
            }
        }
    }
    game_state.demo = DEMO_RECORD;
    pad_scan();
    demo.stick[1].stroke = 0.375F;
    game_state.demo = DEMO_ON;
    pad_scan();
    assert(pad_sticks[1].stroke == 0.375F);
    for (i = GAME_PLAYERS; i < PAD_PLAYERS; ++i) {
        assert(pad_sticks[i].stroke == 0.0F);
    }
    return 0;
}
