#include "ppp_system.h"

int PP_main(int argc, char **argv)
{
    if (PP_TimerDriverInitialize()) {
        return 1;
    }
    if (ppp_init(0)) {
        return 1;
    }
    return 0;
}
int ppp_restart(void)
{
    if (PP_TimerDriverInitialize()) {
        return 1;
    }
    if (ppp_init(0)) {
        return 1;
    }
    return 0;
}
