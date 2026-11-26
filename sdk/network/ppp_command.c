#include "ppp_fsm.h"

short PP_doppp_close(struct ppp_fsm *fsm)
{
    fsm->flags &= ~(PPP_ACTIVE | PPP_PASSIVE);
    PP_fsm_close(fsm);
    return 0;
}
short PP_doppp_active(struct ppp_fsm *fsm)
{
    fsm->flags &= ~PPP_PASSIVE;
    fsm->flags |= PPP_ACTIVE;
    if (fsm->state < PPP_LISTEN) {
        fsm->state = PPP_LISTEN;
    }
    return 0;
}
