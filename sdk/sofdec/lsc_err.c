/* LSC's error function, and the call that passes it an error's text */
#include <cri_lsc.h>
#include <lsc_err.h>
#include <stdarg.h>
#include <stdio.h>

LSC_ERRFN lsc_err_func = NULL;
void *lsc_err_obj = NULL;
char lsc_err_msg[LSC_ERR_MSG_LEN];

void LSC_EntryErrFunc(LSC_ERRFN func, void *obj)
{
    if (func == NULL) {
        lsc_err_func = NULL;
        lsc_err_obj = NULL;
        return;
    }
    lsc_err_func = func;
    lsc_err_obj = obj;
}

void LSC_CallErrFunc(const char *format, ...)
{
    va_list args;

    va_start(args, format);
    vsprintf(lsc_err_msg, format, args);
    va_end(args);
    if (lsc_err_func != NULL) {
        lsc_err_func(lsc_err_obj, lsc_err_msg);
    }
}
