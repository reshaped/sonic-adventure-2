/* the fiber server that the drives' fibers run on */
#include <sg_xpt.h>

#include "vms.h"

/* the member exports these functions, though no header of the SDK declares
 * them */
/* NOLINTBEGIN(misc-use-internal-linkage) */
#ifdef NONMATCHING
#else
void vmsfb_exit(void);
#endif
void vmsfb_end(void);
/* NOLINTEND(misc-use-internal-linkage) */

static BU_FBR_SERVER *server;

BU_FBR_SERVER *vmsfb_init(void)
{
    buFbrInit();
    server = buFbrCreateServer(VMS_DRIVES);
    return server;
}

Sint32 vmsfb_create(void (*entry)(void), void *stack, Sint32 size)
{
#ifdef NONMATCHING
    /* this side refuses a stack of no bytes */
    if (size <= 0) {
        return -1;
    }
#endif
    return buFbrCreateFiber(server, entry, stack, size);
}

void vmsfb_exec(void)
{
    buFbrExecFiberServer(server);
}

void vmsfb_exit(void)
{
#ifdef NONMATCHING
    /* this side does nothing once the server is gone */
    if (server == NULL) {
        return;
    }
#endif
    buFbrDestroyServer(server);
#ifdef NONMATCHING
    /* this side forgets the server it destroyed, so a second vmsfb_exit does
     * nothing */
    server = NULL;
#endif
}

void vmsfb_sleep(void)
{
    buFbrSleepVFiber();
}

void vmsfb_end(void)
{
    buFbrEndFiber();
}
