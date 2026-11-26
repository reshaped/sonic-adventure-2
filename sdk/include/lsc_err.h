/* LSC's error function, and the call that passes it an error's text */
#ifndef LSC_ERR_H
#define LSC_ERR_H
#include <cri_lsc.h>

/* the error function, or null, and its obj, as LSC_EntryErrFunc entered
 * them */
extern LSC_ERRFN lsc_err_func;
extern void *lsc_err_obj;
/* the room for an error's text, with its terminator */
#define LSC_ERR_MSG_LEN 256
/* the text of the last error */
extern char lsc_err_msg[LSC_ERR_MSG_LEN];

/* format an error's text as sprintf does, into lsc_err_msg, and pass it to
 * the function LSC_EntryErrFunc was given, if any. it doesn't test that the
 * text fits */
void LSC_CallErrFunc(const char *format, ...);

#endif
