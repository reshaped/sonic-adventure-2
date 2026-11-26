/* the middleware's manager, as far as the sources use it. the middleware's
 * libraries enter functions in its tables. it runs them at the server, at the
 * vertical blank and at the frame's start */
#ifndef SG_MW_H
#define SG_MW_H
#include <sg_xpt.h>

/* an error function. it is passed the obj entered with it, and emsg, the
 * error's text, which the next error overwrites */
typedef void (*MW_ERRFN)(void *obj, char *emsg);

/* pairs of mwMngInit and mwMngFinish nest. the outermost mwMngInit empties
 * the tables and the error function. it adds the manager's handler to the
 * vertical blank's chain at MWD_PLY_PRI_VBLNK. its mwMngFinish takes the
 * handler off.
 *
 * after mwMngPreInit, the outermost mwMngInit also makes the main fiber and
 * the idle fiber. it has Kamui's wait-vsync callback switch to the idle
 * fiber, unless mwMngConfigUsingKmWaitVsyncCb turned that off. it enters a
 * vertical blank function that switches back to the main fiber. its
 * mwMngFinish clears Kamui's callback, unless mwMngConfigUsingKmWaitVsyncCb
 * has turned the switch off by then */
void mwMngInit(void);
void mwMngFinish(void);
/* at the first call, have exceptions and interrupts run on a stack of the
 * manager's, through syFbrPreInit. mwMngInit then makes fibers. later calls do
 * nothing */
void mwMngPreInit(void);
/* have errfn take each error, with obj, from now on. a null errfn takes
 * none. every interrupt is masked while it runs */
void mwMngEntryErrFn(MW_ERRFN errfn, void *obj);
/* format an error in the manager's 256-byte buffer and pass it to the
 * current error function. the text and its terminator must fit */
void mwMngError(const char *format, ...);
/* enter a callback with its object, or remove the callback from its table */
void mwMngEntryServerFunc(void (*func)(void *obj), void *obj);
void mwMngDeleteServerFunc(void (*func)(void *obj));
void mwMngEntryVsyncFunc(void (*func)(void *obj), void *obj);
void mwMngDeleteVsyncFunc(void (*func)(void *obj));
void mwMngEntryStartFrameFunc(void (*func)(void *obj), void *obj);
void mwMngDeleteStartFrameFunc(void (*func)(void *obj));
void mwMngSleepIdle(void);
void mwMngEntryIdleFunc(void (*func)(void *obj), void *obj);
void mwMngDeleteIdleFunc(void (*func)(void *obj));
void mwMngIdle(void);
/* call each function entered for the server, or for the frame's start. each is
 * passed the obj it was entered with */
void mwMngExecServer(void);
void mwMngStartFrame(void);

/* the middleware library's version and build time, as text */
extern char *mwg_lib_build;
/* mwExecMainServer calls mwMngExecServer, mwEntryErrFunc mwMngEntryErrFn,
 * mwInitSystem mwMngInit, and mwExitSystem mwMngFinish */
void mwExecMainServer(void);
void mwEntryErrFunc(MW_ERRFN errfn, void *obj);
void mwInitSystem(void);
void mwExitSystem(void);

#endif
