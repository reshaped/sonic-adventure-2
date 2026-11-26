/* the middleware's player calls, as far as the sources use them */
#ifndef SG_MWPLY_H
#define SG_MWPLY_H
#include <sg_xpt.h>
#include <sj.h>

typedef enum {
    MWE_PLY_STAT_STOP,
    MWE_PLY_STAT_PREP,
    MWE_PLY_STAT_PLAYING,
    MWE_PLY_STAT_PLAYEND,
    MWE_PLY_STAT_ERROR
} MWE_PLY_STAT;

/* each middleware player begins with this table. its first three slots
 * are reserved for COM, and the Sofdec player implements none of them */
typedef struct {
    struct _mwply_if *vtbl;
} MW_PLY_OBJ;
typedef MW_PLY_OBJ *MWPLY;

typedef struct _mwply_if {
    void (*QueryInterface)(void);
    void (*AddRef)(void);
    void (*Release)(void);
    void (*VsyncHndl)(MWPLY);
    void (*ExecSvrHndl)(MWPLY);
    void (*Destroy)(MWPLY);
    void (*StartFname)(MWPLY, Sint8 *filename);
    void (*Stop)(MWPLY);
    MWE_PLY_STAT (*GetStat)(MWPLY);
    void (*GetTime)(MWPLY, Sint32 *count, Sint32 *scale);
    void (*Pause)(MWPLY, Sint32 paused);
    void (*SetOutVol)(MWPLY, Sint32 volume);
    Sint32 (*GetOutVol)(MWPLY);
    void (*SetOutPan)(MWPLY, Sint32 channel, Sint32 pan);
    Sint32 (*GetOutPan)(MWPLY, Sint32 channel);
    void (*StartSj)(MWPLY, SJ);
    void (*StartMem)(MWPLY, void *data, Sint32 bytes);
} MWD_IF;

/* an error function, as sg_mw.h's MW_ERRFN is */
typedef void (*MW_PLY_ERRFN)(void *obj, char *msg);
/* the same as mwMngEntryErrFn */
void mwPlyEntryErrFunc(MW_PLY_ERRFN errfn, void *obj);

/* the vertical blank chain priority of the manager's handler */
#define MWD_PLY_PRI_VBLNK 0x70

/* mwPlyStartFrame calls mwMngStartFrame, and mwPlyExecServer
 * mwMngExecServer */
void mwPlyStartFrame(void);
void mwPlyExecServer(void);

/* the switches between the main fiber and the idle fiber, which mwMngInit
 * makes after mwMngPreInit. mwExecIdleSvr switches to the idle fiber. that
 * fiber runs the functions entered for the idle time until the next
 * vertical blank switches back. mwReturnToMain switches to the main fiber
 * at once. mwReturnToMainFromIntr, called from an interrupt, switches to it
 * once the interrupt returns */
void mwExecIdleSvr(void);
void mwReturnToMain(void);
void mwReturnToMainFromIntr(void);
/* with a nonzero flag, the default, mwMngInit has Kamui's wait-vsync
 * callback switch to the idle fiber. with 0, mwMngInit and mwMngFinish
 * leave the callback as it is */
void mwMngConfigUsingKmWaitVsyncCb(Sint32 flag);

#endif
