#include <sg_xpt.h>
#include <sy_g2_work.h>

struct sy_g2_work *sygG2WrkPtr = NULL;
Uint8 sygG2WrkImg[sizeof(struct sy_g2_work) + SY_G2_WORK_ALIGN];
Bool sygG2DmaSuspendAll;
