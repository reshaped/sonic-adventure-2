/* the middleware's manager. beside sg_mw.h's three tables of functions, it
 * keeps a fourth for the idle time. with fibers, the idle fiber runs that
 * table's functions while the main fiber waits for a vertical blank */
#include <kamui.h>
#include <machine.h>
#include <sg_chain.h>
#include <sg_mw.h>
#include <sg_mwply.h>
#include <sg_syfbr.h>
#include <sg_xpt.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>

/* the entries in each table */
#define FUNC_MAX 16
/* the fibers syFbrInit makes room for */
#define FIBER_MAX 5
/* the words of the interrupts' stack, and the bytes of the idle fiber's */
#define INT_STACK_WORDS 0x1000
#define IDLE_STACK_SIZE 0x4000
/* the room for an error's text, with its terminator */
#define ERR_MSG_LEN 256

/* a function a table holds, and the obj it is called with. an empty entry
 * has a null func */
struct mng_func {
    void (*func)(void *obj);
    void *obj;
};

/* the member exports these names, though no header of the SDK declares
 * them */
/* NOLINTBEGIN(misc-use-internal-linkage) */
void mwlMngLockCs(Sint32 *mask);
void mwlMngUnlockCs(const Sint32 *mask);
void mwMngVsync(void *obj);
void mwMngIdleThread(void *obj);
void mwMngSwitchToIdle(void);
void mwMngDelayedSwitchToMain(void *obj);
void mwStartFrame(void);

char *mwg_mng_build = "\nmwMng Ver.2.05 Build:Jan 19 2001 18:50:59\n";
/* the handler mwMngInit added to the vertical blank's chain, or 0 */
SYCHAIN mwg_mng_chain = 0;
/* the mwMngInit calls that no mwMngFinish has matched */
Sint32 mwg_mng_init_level = 0;
/* set once mwMngPreInit has run */
Bool mwg_mng_preinit_flag = FALSE;
/* while set, mwMngInit makes the switch to the idle fiber Kamui's wait-vsync
 * callback, and mwMngFinish takes it off. mwMngConfigUsingKmWaitVsyncCb sets
 * the flag */
Bool mwg_mng_kmwaitcb_flag = TRUE;
/* while set, mwMngPreInit sets up the fibers */
Bool mwg_mng_mthrd_master_flag = TRUE;
/* set once mwMngPreInit has set up the fibers */
Bool mwg_mng_mthrd = FALSE;
/* set while the idle fiber runs the idle functions */
/* @hack type: volatile on mwg_mng_thread_exec_flag, where a plain Bool would
 * do */
volatile Bool mwg_mng_thread_exec_flag = FALSE;
/* set while mwMngSleepIdle waits for the idle functions to end */
/* @hack type: volatile on mwg_mng_thread_sleep_flag, where a plain Bool
 * would do */
volatile Bool mwg_mng_thread_sleep_flag = FALSE;
/* NOLINTEND(misc-use-internal-linkage) */

static struct mng_func mng_server[FUNC_MAX];
static struct mng_func mng_vsync[FUNC_MAX];
static struct mng_func mng_start_frame[FUNC_MAX];
static struct mng_func mng_idle[FUNC_MAX];
/* the error function, and the obj it is called with */
struct mng_err_func {
    MW_ERRFN func;
    void *obj;
};
static struct mng_err_func mng_err;

/* the member exports these names too */
/* NOLINTBEGIN(misc-use-internal-linkage) */
/* the stack interrupts run on, from its end */
Sint32 mwg_mng_intStack[INT_STACK_WORDS];
SYS_FBR_INFO mwg_mng_fbrWork[FIBER_MAX];
/* the idle fiber, and the main one */
SYFBR mwg_mng_fbrSub;
SYFBR mwg_mng_fbrMain;
/* the idle fiber's stack */
char mwg_mng_subStack[IDLE_STACK_SIZE];
/* NOLINTEND(misc-use-internal-linkage) */
/* the last error's text */
static char mng_err_msg[ERR_MSG_LEN];
/* mwExecMainServer stores mwg_lib_build here, so that the link keeps it */
char *mwg_dummy; /* NOLINT(misc-use-internal-linkage) */

/* the manager's lock is the interrupt mask. mwlMngLockCs saves the
 * interrupt mask in *mask, and masks every interrupt. mwlMngUnlockCs sets the
 * interrupt mask to *mask again */
void mwlMngLockCs(Sint32 *mask)
{
    *mask = get_imask();
    set_imask(IMASK_ALL);
}

void mwlMngUnlockCs(const Sint32 *mask)
{
    set_imask(*mask);
}

void mwMngEntryErrFn(MW_ERRFN errfn, void *obj)
{
    Sint32 mask;

    mwlMngLockCs(&mask);
    mng_err.func = errfn;
    mng_err.obj = obj;
    mwlMngUnlockCs(&mask);
}

/* format an error's text as sprintf does, into the manager's text of
 * ERR_MSG_LEN bytes. pass it to the error function, if one is set. the text
 * and its terminator must fit: nothing tests that they do */
void mwMngError(const char *format, ...)
{
    va_list args;
    /* @hack spelling: err, a pointer to mng_err, where mng_err read in place
     * would do */
    struct mng_err_func *err;

    memset(mng_err_msg, 0, sizeof(mng_err_msg));
    va_start(args, format);
    vsprintf(mng_err_msg, format, args);
    va_end(args);
    err = &mng_err;
    if (err->func != NULL) {
        err->func(err->obj, mng_err_msg);
    }
}

/* each mwMngEntry call enters func, with obj, in the first empty entry of
 * its table. with no empty entry, it reports an error through mwMngError.
 * each mwMngDelete call empties every entry of its table that holds func.
 * both run under the manager's lock */
void mwMngEntryServerFunc(void (*func)(void *obj), void *obj)
{
    Sint32 mask;
    Sint32 i;
    struct mng_func *f;

    mwlMngLockCs(&mask);
    for (i = 0; i < FUNC_MAX; i++) {
        f = &mng_server[i];
        /* @hack spelling: a continue past a full entry, where the entry
         * filled under a test that it is empty would do */
        if (f->func != NULL) {
            continue;
        }
        f->func = func;
        f->obj = obj;
        break;
    }
    if (i == FUNC_MAX) {
        mwMngError("mwMng:too many libarary");
    }
    mwlMngUnlockCs(&mask);
}

void mwMngDeleteServerFunc(void (*func)(void *obj))
{
    Sint32 mask;
    Sint32 i;
    struct mng_func *f;

    mwlMngLockCs(&mask);
    for (i = 0; i < FUNC_MAX; i++) {
        f = &mng_server[i];
        /* @hack spelling: a continue past another function's entry, where
         * the entry emptied under a test that it holds func would do */
        if (f->func != func) {
            continue;
        }
        f->func = NULL;
        f->obj = NULL;
    }
    mwlMngUnlockCs(&mask);
}

void mwMngExecServer(void)
{
    Sint32 i;
    struct mng_func *f;

    for (i = 0; i < FUNC_MAX; i++) {
        f = &mng_server[i];
        /* @hack spelling: a continue past an empty entry, where the call
         * under a test that the entry is full would do */
        if (f->func == NULL) {
            continue;
        }
        f->func(f->obj);
    }
}

void mwMngEntryVsyncFunc(void (*func)(void *obj), void *obj)
{
    Sint32 mask;
    Sint32 i;
    struct mng_func *f;

    mwlMngLockCs(&mask);
    for (i = 0; i < FUNC_MAX; i++) {
        f = &mng_vsync[i];
        /* @hack spelling: a continue past a full entry, where the entry
         * filled under a test that it is empty would do */
        if (f->func != NULL) {
            continue;
        }
        f->func = func;
        f->obj = obj;
        break;
    }
    if (i == FUNC_MAX) {
        mwMngError("mwMng:too many libarary");
    }
    mwlMngUnlockCs(&mask);
}

void mwMngDeleteVsyncFunc(void (*func)(void *obj))
{
    Sint32 mask;
    Sint32 i;
    struct mng_func *f;

    mwlMngLockCs(&mask);
    for (i = 0; i < FUNC_MAX; i++) {
        f = &mng_vsync[i];
        /* @hack spelling: a continue past another function's entry, where
         * the entry emptied under a test that it holds func would do */
        if (f->func != func) {
            continue;
        }
        f->func = NULL;
        f->obj = NULL;
    }
    mwlMngUnlockCs(&mask);
}

/* the handler mwMngInit adds to the vertical blank's chain. it calls each
 * function of the vertical blank's table, and doesn't read obj */
void mwMngVsync(void *obj)
{
    Sint32 i;
    struct mng_func *f;

    for (i = 0; i < FUNC_MAX; i++) {
        f = &mng_vsync[i];
        /* @hack spelling: a continue past an empty entry, where the call
         * under a test that the entry is full would do */
        if (f->func == NULL) {
            continue;
        }
        f->func(f->obj);
    }
}

void mwMngEntryStartFrameFunc(void (*func)(void *obj), void *obj)
{
    Sint32 mask;
    Sint32 i;
    struct mng_func *f;

    mwlMngLockCs(&mask);
    for (i = 0; i < FUNC_MAX; i++) {
        f = &mng_start_frame[i];
        /* @hack spelling: a continue past a full entry, where the entry
         * filled under a test that it is empty would do */
        if (f->func != NULL) {
            continue;
        }
        f->func = func;
        f->obj = obj;
        break;
    }
    if (i == FUNC_MAX) {
        mwMngError("mwMng:too many libarary");
    }
    mwlMngUnlockCs(&mask);
}

void mwMngDeleteStartFrameFunc(void (*func)(void *obj))
{
    Sint32 mask;
    Sint32 i;
    struct mng_func *f;

    mwlMngLockCs(&mask);
    for (i = 0; i < FUNC_MAX; i++) {
        f = &mng_start_frame[i];
        /* @hack spelling: a continue past another function's entry, where
         * the entry emptied under a test that it holds func would do */
        if (f->func != func) {
            continue;
        }
        f->func = NULL;
        f->obj = NULL;
    }
    mwlMngUnlockCs(&mask);
}

void mwMngStartFrame(void)
{
    Sint32 i;
    struct mng_func *f;

    for (i = 0; i < FUNC_MAX; i++) {
        f = &mng_start_frame[i];
        /* @hack spelling: a continue past an empty entry, where the call
         * under a test that the entry is full would do */
        if (f->func == NULL) {
            continue;
        }
        f->func(f->obj);
    }
}

/* with fibers, let the idle fiber end the pass over the idle functions it
 * is in. the idle fiber begins no pass while this call waits */
void mwMngSleepIdle(void)
{
    if (mwg_mng_mthrd == TRUE) {
        mwg_mng_thread_sleep_flag = TRUE;
        while (mwg_mng_thread_exec_flag == TRUE) {
            syFbrSwitchToFiber(mwg_mng_fbrSub);
        }
        mwg_mng_thread_sleep_flag = FALSE;
    }
}

void mwMngEntryIdleFunc(void (*func)(void *obj), void *obj)
{
    Sint32 mask;
    Sint32 i;
    struct mng_func *f;

    mwlMngLockCs(&mask);
    for (i = 0; i < FUNC_MAX; i++) {
        f = &mng_idle[i];
        /* @hack spelling: a continue past a full entry, where the entry
         * filled under a test that it is empty would do */
        if (f->func != NULL) {
            continue;
        }
        f->func = func;
        f->obj = obj;
        break;
    }
    if (i == FUNC_MAX) {
        mwMngError("mwMng:too many libarary");
    }
    mwlMngUnlockCs(&mask);
}

void mwMngDeleteIdleFunc(void (*func)(void *obj))
{
    Sint32 mask;
    Sint32 i;
    struct mng_func *f;

    mwlMngLockCs(&mask);
    for (i = 0; i < FUNC_MAX; i++) {
        f = &mng_idle[i];
        /* @hack spelling: a continue past another function's entry, where
         * the entry emptied under a test that it holds func would do */
        if (f->func != func) {
            continue;
        }
        f->func = NULL;
        f->obj = NULL;
    }
    mwlMngUnlockCs(&mask);
}

void mwMngIdle(void)
{
    Sint32 i;
    struct mng_func *f;

    for (i = 0; i < FUNC_MAX; i++) {
        f = &mng_idle[i];
        /* @hack spelling: a continue past an empty entry, where the call
         * under a test that the entry is full would do */
        if (f->func == NULL) {
            continue;
        }
        f->func(f->obj);
    }
}

/* the idle fiber's body, which never returns. it runs the idle functions
 * pass after pass. while mwMngSleepIdle waits, it switches to the main fiber
 * instead. it doesn't read obj */
void mwMngIdleThread(void *obj)
{
    for (;;) {
        if (mwg_mng_thread_sleep_flag == TRUE) {
            syFbrSwitchToFiber(mwg_mng_fbrMain);
        } else {
            mwg_mng_thread_exec_flag = TRUE;
            mwMngIdle();
            mwg_mng_thread_exec_flag = FALSE;
        }
    }
}

void mwMngSwitchToIdle(void)
{
    syFbrSwitchToFiber(mwg_mng_fbrSub);
}

/* the vertical blank function mwMngInit enters. it switches to the main
 * fiber once the interrupt returns, and doesn't read obj */
void mwMngDelayedSwitchToMain(void *obj)
{
    syFbrDelayedSwitchToFiber(mwg_mng_fbrMain);
}

void mwMngConfigUsingKmWaitVsyncCb(Sint32 flag)
{
    mwg_mng_kmwaitcb_flag = flag;
}

void mwMngInit(void)
{
    if (mwg_mng_init_level == 0) {
        mwg_mng_thread_exec_flag = FALSE;
        mwg_mng_thread_sleep_flag = FALSE;
        memset(mng_server, 0, sizeof(mng_server));
        memset(mng_vsync, 0, sizeof(mng_vsync));
        memset(mng_start_frame, 0, sizeof(mng_start_frame));
        memset(mng_idle, 0, sizeof(mng_idle));
        memset(&mng_err, 0, sizeof(mng_err));
        mwg_mng_chain = syChainAddHandler(SYD_CHAIN_EVENT_IML6_VBLANK,
                                          mwMngVsync, MWD_PLY_PRI_VBLNK, NULL);
        if (mwg_mng_mthrd != FALSE) {
            syFbrInit(mwg_mng_fbrWork, FIBER_MAX);
            mwg_mng_fbrMain = syFbrConvertThreadToFiber(NULL);
            mwg_mng_fbrSub =
                syFbrCreateFiber(mwg_mng_subStack, sizeof(mwg_mng_subStack),
                                 mwMngIdleThread, NULL);
            if (mwg_mng_kmwaitcb_flag != FALSE) {
                /* mwMngSwitchToIdle takes no argument, so the cast gives it
                 * the type of Kamui's callback. it ignores the NULL Kamui
                 * passes */
                kmSetWaitVsyncCallback((PKMCALLBACKFUNC)mwMngSwitchToIdle,
                                       NULL);
            }
            mwMngEntryVsyncFunc(mwMngDelayedSwitchToMain, NULL);
        }
    }
    mwg_mng_init_level++;
}

void mwMngFinish(void)
{
    mwg_mng_init_level--;
    if (mwg_mng_init_level == 0) {
        if (mwg_mng_chain != 0) {
            syChainDeleteHandler(mwg_mng_chain);
            mwg_mng_chain = 0;
        }
        if (mwg_mng_mthrd != FALSE && mwg_mng_kmwaitcb_flag != FALSE) {
            kmSetWaitVsyncCallback(NULL, NULL);
        }
    }
}

void mwMngPreInit(void)
{
    if (mwg_mng_preinit_flag == FALSE) {
        mwg_mng_preinit_flag = TRUE;
        if (mwg_mng_mthrd_master_flag != FALSE) {
            syFbrPreInit(mwg_mng_intStack, INT_STACK_WORDS);
            mwg_mng_mthrd = TRUE;
        }
    }
}

void mwExecMainServer(void)
{
    mwMngExecServer();
    mwg_dummy = mwg_lib_build;
}

void mwStartFrame(void)
{
    mwMngStartFrame();
}

void mwEntryErrFunc(MW_ERRFN errfn, void *obj)
{
    mwMngEntryErrFn(errfn, obj);
}

void mwInitSystem(void)
{
    mwMngInit();
}

void mwExitSystem(void)
{
    mwMngFinish();
}

void mwExecIdleSvr(void)
{
    mwMngSwitchToIdle();
}

void mwReturnToMain(void)
{
    syFbrSwitchToFiber(mwg_mng_fbrMain);
}

void mwReturnToMainFromIntr(void)
{
    syFbrDelayedSwitchToFiber(mwg_mng_fbrMain);
}
