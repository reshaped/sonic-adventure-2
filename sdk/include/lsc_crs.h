/* LSC's lock around its handles, which masks every interrupt */
#ifndef LSC_CRS_H
#define LSC_CRS_H
#include <sg_xpt.h>

/* save the interrupt mask in *mask, and mask every interrupt */
void LSC_LockCrs(Sint32 *mask);
/* set the interrupt mask to *mask again */
void LSC_UnlockCrs(const Sint32 *mask);

#endif
