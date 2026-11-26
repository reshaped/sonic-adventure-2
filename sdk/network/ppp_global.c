/* the one PPP session the ACCESS library keeps, for its packet driver */
#include <sg_xpt.h>

#ifdef NONMATCHING
#else
static Uint8 pull_word[4], pull_byte;
#endif

#include "ppp_fsm.h"
#include "ppp_link.h"

struct ppp_session PP_ppp_str;
