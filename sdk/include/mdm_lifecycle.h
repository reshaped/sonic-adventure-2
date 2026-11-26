/* the modem members' entries in DSG_INIT_ and DSG_END_. the Chao area calls the
 * first as it starts, and the second as it ends */
#ifndef SDK_MDM_LIFECYCLE_H
#define SDK_MDM_LIFECYCLE_H

extern void (*init_dteapicc)(void);
extern void (*init_dspdbcc)(void);
extern void (*init_pacingcc)(void);
extern void (*init_targetcc)(void);
extern void (*init_RamDownlcc)(void);
extern void (*end_dteapicc)(void);
extern void (*end_pacingcc)(void);

#endif
