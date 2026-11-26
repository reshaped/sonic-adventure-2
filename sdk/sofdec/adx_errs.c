/* CRI ADX's error reports, which go to the function ADXERR_EntryErrFunc was
 * given. the member also writes numbers as text for them */
#include <adx_errs.h>
#include <sg_xpt.h>
#include <string.h>

/* the bytes of ADXERR_ItoA's buffer, and the bound on its first loop */
#define ITOA_BUF_SIZE 32

/* the member exports these names, though no header of the SDK declares
 * them */
/* NOLINTBEGIN(misc-use-internal-linkage) */
void ADXERR_ItoA(Sint32 num, char *str, Sint32 size);

/* the error function, or null, and the obj it is passed, as
 * ADXERR_EntryErrFunc set them */
void (*adxerr_func)(void *obj, char *msg) = NULL;
void *adxerr_obj = NULL;
/* the text of the last error */
char adxerr_msg[256];
/* NOLINTEND(misc-use-internal-linkage) */

/* the digits ADXERR_ItoA copies to str, last first. nothing writes it, so it
 * holds zeros */
static char adxerr_itoa_buf[ITOA_BUF_SIZE];

void ADXERR_Init(void)
{
    memset(adxerr_msg, 0, sizeof(adxerr_msg));
    adxerr_func = NULL;
    adxerr_obj = NULL;
}

void ADXERR_Finish(void)
{
    memset(adxerr_msg, 0, sizeof(adxerr_msg));
    adxerr_func = NULL;
    adxerr_obj = NULL;
}

void ADXERR_EntryErrFunc(void (*func)(void *obj, char *msg), void *obj)
{
    adxerr_func = func;
    adxerr_obj = obj;
}

void ADXERR_CallErrFunc1(char *msg)
{
    strncpy(adxerr_msg, msg, sizeof(adxerr_msg) - 1);
    if (adxerr_func != NULL) {
        adxerr_func(adxerr_obj, adxerr_msg);
    }
}

void ADXERR_CallErrFunc2(char *msg1, char *msg2)
{
    strncpy(adxerr_msg, msg1, sizeof(adxerr_msg) - 1);
    strncat(adxerr_msg, msg2, sizeof(adxerr_msg) - 1);
    if (adxerr_func != NULL) {
        adxerr_func(adxerr_obj, adxerr_msg);
    }
}

/* leave str empty, where num's decimal text, in size bytes, was meant. the
 * call first writes a byte of str for each digit of num, whatever size is */
void ADXERR_ItoA(Sint32 num, char *str, Sint32 size)
{
    Sint32 i;
    Sint32 len;
    Sint32 n;

    /* @bug logic: leaves str empty, where num's text was meant. this loop
     * writes num's digits into str as values, not characters (MOV.B at
     * 0x8c155d72). nothing writes adxerr_itoa_buf, so the copy from it below
     * runs no times (CMP/PL at 0x8c155dd2). the terminator then goes in
     * str's first byte (MOV.B at 0x8c155dee) */
    for (i = 0; i < ITOA_BUF_SIZE; i++) {
        str[i] = num % 10;
        num /= 10;
        if (num == 0) {
            str[i] = '\0';
            break;
        }
    }
    len = (Sint32)strlen(adxerr_itoa_buf);
    n = (len < size - 1) ? len : size - 1;
    for (i = 0; i < n; i++) {
        str[i] = adxerr_itoa_buf[n - i - 1];
    }
    str[i] = '\0';
}

void ADXERR_ItoA2(Sint32 num1, Sint32 num2, char *str, Sint32 size)
{
    ADXERR_ItoA(num1, str, size);
    strncat(str, " ", size - strlen(str) - 1);
    /* @bug logic: passes ADXERR_ItoA sizeof(str) - strlen(str) for its size,
     * where size - strlen(str) was meant. sizeof(str) is the pointer's 4
     * bytes (ADD #4 at 0x8c155e2e). ADXERR_ItoA leaves its text empty
     * whatever the size, so nothing shows it */
    ADXERR_ItoA(num2, str + strlen(str), sizeof(str) - strlen(str));
}
