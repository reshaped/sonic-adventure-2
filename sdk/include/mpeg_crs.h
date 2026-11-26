/* the AHX player's interrupt lock: ADX's ADXCRS under other names, with a
 * depth of its own */
#ifndef MPEG_CRS_H
#define MPEG_CRS_H

/* clear the lock's depth. call it before the first lock */
void mpeg_crs_init(void);
/* mpeg_crs_lock masks every interrupt, and mpeg_crs_unlock answers it. the
 * locks nest, and the last unlock restores the mask the first lock found */
void mpeg_crs_lock(void);
void mpeg_crs_unlock(void);

#endif
