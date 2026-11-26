/* each decoder's errors and the library's: the last code, and the function
 * that takes each nonzero one */
#include <sg_xpt.h>
#include <sofdec.h>

/* the member exports these functions and mpverrinf, though no header of the
 * SDK declares them */
/* NOLINTBEGIN(misc-use-internal-linkage) */
/* copy mpv's errors to *errinf, and return 0. for an mpv that MPVLIB_CheckHn
 * refuses, *errinf keeps what it held. the call then reports
 * MPV_ERR_GET_ERR_INF as the library's error instead, and returns that */
Sint32 MPV_GetErrInf(struct mpv_obj *mpv, struct mpv_errinf *errinf);
/* make ecode errinf's last error. a nonzero ecode also goes to errinf's
 * function, unless that is null */
void mpverr_SetCodeSub(struct mpv_errinf *errinf, Sint32 ecode);
/* the library's own errors, which MPVERR_SetCode takes for a null mpv */
struct mpv_errinf mpverrinf;
/* NOLINTEND(misc-use-internal-linkage) */

void MPVERR_Init(void)
{
    MPVERR_InitErrInf(&mpverrinf);
}

void MPVERR_InitErrInf(struct mpv_errinf *errinf)
{
    errinf->func = NULL;
    errinf->obj = NULL;
    errinf->ecode = 0;
    errinf->delimiter_misses = 0;
    errinf->other_errors = 0;
}

Sint32 MPV_SetErrFunc(struct mpv_obj *mpv, MPV_ERRFN func, void *obj)
{
    /* @hack spelling: mpv copied to a local of its own, where the parameter
     * would do */
    struct mpv_obj *dec = mpv;
    /* @hack spelling: the record's address in a local of its own, where
     * dec->err would do */
    struct mpv_errinf *errinf;

    if (MPVLIB_CheckHn(dec) != 0) {
        return MPVERR_SetCode(NULL, MPV_ERR_SET_ERR_FUNC);
    }
    errinf = &dec->err;
    errinf->func = func;
    errinf->obj = obj;
    return 0;
}

Sint32 MPV_GetErrInf(struct mpv_obj *mpv, struct mpv_errinf *errinf)
{
    /* @hack spelling: mpv copied to a local of its own, where the parameter
     * would do */
    struct mpv_obj *dec = mpv;

    if (MPVLIB_CheckHn(dec) != 0) {
        return MPVERR_SetCode(NULL, MPV_ERR_GET_ERR_INF);
    }
    *errinf = dec->err;
    return 0;
}

Sint32 MPVERR_SetCode(struct mpv_obj *mpv, Sint32 ecode)
{
    if (mpv == NULL) {
        mpverr_SetCodeSub(&mpverrinf, ecode);
    } else {
        mpverr_SetCodeSub(&mpv->err, ecode);
    }
    return ecode;
}

void mpverr_SetCodeSub(struct mpv_errinf *errinf, Sint32 ecode)
{
    /* @hack spelling: the function read into a local of its own, where
     * errinf->func would do */
    MPV_ERRFN func;

    errinf->ecode = ecode;
    if (ecode != 0) {
        func = errinf->func;
        if (func != NULL) {
            func(errinf->obj, ecode);
        }
    }
}
