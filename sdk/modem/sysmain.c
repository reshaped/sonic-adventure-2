/* the modem's controller, driven a pass at a time. the modem library's own
 * loop calls scm_main_init and then scm_main_core forever (0x0caf3c44).
 * sysmainOpen makes the first call instead, and each sysmainCall makes one
 * pass of the loop */
#include "scm_main.h"
#include "sysmain.h"

void sysmainOpen(void)
{
    scm_main_init();
}

void sysmainClose(void)
{
}

void sysmainCall(void)
{
    scm_main_core();
}
