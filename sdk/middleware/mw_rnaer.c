/* the error function of the middleware's sound output, and the call that
 * passes it an error's text */
#include <mw_rna.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>

/* the member exports these names, though no header of the SDK declares
 * them */
/* NOLINTBEGIN(misc-use-internal-linkage) */
/* the error function and its obj, as mwRnaErrEntryFunc entered them */
MW_RNA_ERR_FUNC mwg_rna_err_func = NULL;
void *mwg_rna_err_obj = NULL;
/* the text of the last error */
char mwg_rna_err_msg[MW_RNA_ERR_MSG_LEN];
/* NOLINTEND(misc-use-internal-linkage) */

/* have func take each error, with obj, from now on. a null func takes
 * none. the call also clears the last error's text */
void mwRnaErrEntryFunc(MW_RNA_ERR_FUNC func, void *obj)
{
    if (func == NULL) {
        mwg_rna_err_func = NULL;
        mwg_rna_err_obj = NULL;
    } else {
        mwg_rna_err_func = func;
        mwg_rna_err_obj = obj;
    }
    memset(mwg_rna_err_msg, 0, sizeof(mwg_rna_err_msg));
}

void mwRnaErrCallFunc(const char *format, ...)
{
    va_list args;

    va_start(args, format);
    vsprintf(mwg_rna_err_msg, format, args);
    va_end(args);
    if (mwg_rna_err_func != NULL) {
        mwg_rna_err_func(mwg_rna_err_obj, mwg_rna_err_msg);
    }
}
