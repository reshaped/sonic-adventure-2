#include <assert.h>
#include <stddef.h>

#include "vms.h"

struct bu_fbr_server {
    Sint32 live;
};

static struct bu_fbr_server drives;
static int enabled;
static int reject_after;
static int registered;
static int destroyed;
static int observed_drive;
static int selected;

void observe_drive(long drive)
{
    observed_drive = (int)drive;
}

void buFbrInit(void)
{
    assert(!drives.live);
    registered = 0;
    selected = 0;
}

BU_FBR_SERVER *buFbrCreateServer(Sint32 count)
{
    assert(count == VMS_DRIVES);
    drives.live = 1;
    return &drives;
}

Sint32 buFbrCreateFiber(BU_FBR_SERVER *server, void (*entry)(void),
                        Uint32 *stack, Uint32 size)
{
    const unsigned char *bytes = (const unsigned char *)stack;
    unsigned i;

    assert(server == &drives && server->live);
    assert(stack != NULL && (unsigned long)stack % sizeof(*stack) == 0);
    assert(size == 1024);
    while (selected < VMS_DRIVES && (enabled & (1 << selected)) == 0) {
        selected++;
    }
    assert(selected < VMS_DRIVES);
    for (i = 0; i < size; i++) {
        assert(bytes[i] == '0' + selected);
    }
    observed_drive = -1;
    entry();
    assert(observed_drive == selected);
    selected++;
    if (registered == reject_after) {
        return -1;
    }
    return registered++;
}

void buFbrDestroyServer(BU_FBR_SERVER *server)
{
    assert(server == &drives && server->live);
    server->live = 0;
    destroyed++;
}

void buFbrExecFiberServer(BU_FBR_SERVER *server)
{
    (void)server;
}

void buFbrSleepVFiber(void)
{
}

void buFbrEndFiber(void)
{
}

int main(void)
{
    Uint32 stack[2];
    int i;

    enabled = (1 << VMS_DRIVES) - 1;
    for (i = 0; i < VMS_DRIVES; i++) {
        reject_after = i;
        destroyed = 0;
        assert(vmsfs_c_init(enabled) == -1);
        assert(!drives.live && registered == i && destroyed == 1);
        reject_after = -1;
        assert(vmsfs_c_init(enabled) == 0);
        assert(drives.live && registered == VMS_DRIVES);
        vmsfb_exit();
    }
    enabled = (1 << 1) | (1 << 4) | (1 << 7);
    assert(vmsfs_c_init(enabled) == 0);
    assert(vmsfb_create(buFbrEndFiber, stack, 0) == -1);
    assert(vmsfb_create(buFbrEndFiber, stack, -1) == -1);
    assert(drives.live && registered == 3);
    vmsfb_exit();
    return 0;
}
