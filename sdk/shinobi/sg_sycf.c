/* the console's settings. the sound mode and the language come from the
 * copy in low memory that syBtCfgSet reads and writes. the rest come from
 * kbCfgGetId's record */
#include <sg_xpt.h>
#include <shinobi.h>
#include <string.h>

/* the language and the sound mode in the settings' copy in low memory */
/* @hack type: the bytes as volatile, where plain char would do */
#define CFG_LANGUAGE (*(volatile char *)0x8c00007d)
#define CFG_SOUND_MODE (*(volatile char *)0x8c00007e)
/* CFG_LANGUAGE's value while the console has no language set */
#define CFG_LANGUAGE_UNSET (-1)
/* the default language syCfgGetLanguage takes for none, and replaces with
 * SYD_CFG_ENGLISH */
#define DEFAULT_LANGUAGE_NONE (-1)

/* bytes of kbCfgGetId's record. the individual ID's check bytes come first,
 * then the ID. the country, the default language and the broadcast standard
 * are each an ASCII digit */
#define ID_CHECK_NOT 0
#define ID_CHECK_SUM 1
#define ID_INDIVIDUAL 2
#define ID_COUNTRY 10
#define ID_LANGUAGE 11
#define ID_BROADCAST 12

/* the member exports these names, though no header of the SDK declares
 * them */
/* NOLINTBEGIN(misc-use-internal-linkage) */
/* the work syCfgInit took, or null before it and after syCfgExit */
void *sygCfgBufAddr = NULL;
/* the library's version and build time */
char *sygCfgBuild = "\nsyCfg Ver 1.15 Build:Apr 26 1999 14:38:59\n";
/* syCfgGetIDAddress calls kbCfgIni, and returns kbCfgGetId's record. it
 * returns null where kbCfgIni fails */
Sint8 *syCfgGetIDAddress(void);
/* syCfgIIDCheckSum checks kbCfgGetId's record. its ID_CHECK_SUM byte must
 * be the low byte of the individual ID's sum. its ID_CHECK_NOT byte must be
 * that byte's complement. it returns TRUE where both hold, and FALSE
 * otherwise */
Bool syCfgIIDCheckSum(const Sint8 *record);
/* NOLINTEND(misc-use-internal-linkage) */

Sint32 syCfgInit(void *work)
{
    if (work == NULL) {
        return SYD_CFG_NG;
    }
    sygCfgBufAddr = work;
    return SYD_CFG_OK;
}

Sint32 syCfgExit(void)
{
    sygCfgBufAddr = NULL;
    return SYD_CFG_OK;
}

Sint32 syCfgSetSoundMode(Sint32 mode)
{
    Sint32 result;

    if (sygCfgBufAddr == NULL) {
        return SYD_CFG_NG;
    }
    if (mode != SYD_CFG_STEREO && mode != SYD_CFG_MONO) {
        return SYD_CFG_NG;
    }
    CFG_SOUND_MODE = mode;
    /* @hack spelling: result, where the call in the test would do */
    result = syBtCfgSet(SYD_BT_CFGWRT_SM, sygCfgBufAddr);
    if (result == SYD_BT_CFG_ERR) {
        return SYD_CFG_NG;
    }
    return SYD_CFG_OK;
}

Sint32 syCfgGetSoundMode(Sint32 *mode)
{
    Sint32 result;
    char setting;

    if (mode == NULL) {
        return SYD_CFG_NG;
    }
    *mode = SYD_CFG_STEREO;
    if (sygCfgBufAddr == NULL) {
        return SYD_CFG_NG;
    }
    /* @hack spelling: result, where the call in the test would do */
    result = syBtCfgSet(SYD_BT_CFGINI, sygCfgBufAddr);
    if (result == SYD_BT_CFG_ERR) {
        return SYD_CFG_NG;
    }
    setting = CFG_SOUND_MODE;
    if (setting == SYD_CFG_STEREO) {
        *mode = SYD_CFG_STEREO;
    } else if (setting == SYD_CFG_MONO) {
        *mode = SYD_CFG_MONO;
    } else {
        return SYD_CFG_NG;
    }
    return SYD_CFG_OK;
}

Sint32 syCfgGetCountryCode(Sint32 *country)
{
    Sint8 *record;

    if (country == NULL) {
        return SYD_CFG_NG;
    }
    *country = SYD_CFG_JAPAN;
    record = syCfgGetIDAddress();
    if (record == NULL) {
        return SYD_CFG_NG;
    }
    *country = record[ID_COUNTRY];
    return SYD_CFG_OK;
}

/* the language a default language's digit stands for */
static Sint32 language_of_digit(Sint32 digit)
{
    return digit - SYD_CFG_D_JAPANESE;
}

Sint32 syCfgGetLanguage(Sint32 *language)
{
    Sint32 result;
    char setting;
    Sint32 digit;

    if (language == NULL) {
        return SYD_CFG_NG;
    }
    *language = SYD_CFG_JAPANESE;
    if (sygCfgBufAddr == NULL) {
        return SYD_CFG_NG;
    }
    /* @hack spelling: result, where the call in the test would do */
    result = syBtCfgSet(SYD_BT_CFGINI, sygCfgBufAddr);
    if (result == SYD_BT_CFG_ERR) {
        return SYD_CFG_NG;
    }
    setting = CFG_LANGUAGE;
    if (setting == CFG_LANGUAGE_UNSET) {
        /* @bug logic: stores -48 as the language where
         * syCfgGetDefaultLanguage fails. the call's result is dropped (BSR at
         * 0x8c11167e). the test reads digit instead (CMP/EQ #-1,R0 at
         * 0x8c111684), which the failure leaves SYD_CFG_JAPANESE, not a digit
         * (MOV #0,R3 at 0x8c1116ac) */
        syCfgGetDefaultLanguage(&digit);
        if (digit == DEFAULT_LANGUAGE_NONE) {
            *language = SYD_CFG_ENGLISH;
        } else {
            *language = language_of_digit(digit);
        }
    } else {
        *language = setting;
    }
    return SYD_CFG_OK;
}

Sint32 syCfgGetDefaultLanguage(Sint32 *language)
{
    Sint8 *record;

    if (language == NULL) {
        return SYD_CFG_NG;
    }
    *language = SYD_CFG_JAPANESE;
    record = syCfgGetIDAddress();
    if (record == NULL) {
        return SYD_CFG_NG;
    }
    *language = record[ID_LANGUAGE];
    return SYD_CFG_OK;
}

Sint32 syCfgGetBroadcastType(Sint32 *broadcast)
{
    Sint8 *record;

    if (broadcast == NULL) {
        return SYD_CFG_NG;
    }
    *broadcast = SYD_CFG_NTSC;
    record = syCfgGetIDAddress();
    if (record == NULL) {
        return SYD_CFG_NG;
    }
    *broadcast = record[ID_BROADCAST];
    return SYD_CFG_OK;
}

Sint32 syCfgGetIndividualID(Sint8 *id)
{
    Sint8 *record;
    Sint8 *src;
    Sint32 result;

    if (id == NULL) {
        return SYD_CFG_IID_NG;
    }
    kbCfgIni(KBD_CFGINI);
    /* @hack spelling: src, a copy of record stepped to the ID, where
     * record + ID_INDIVIDUAL in the call would do */
    src = kbCfgGetId();
    record = src;
    src += ID_INDIVIDUAL;
    memcpy(id, src, SYD_CFG_IID_SIZE);
    if (syCfgIIDCheckSum(record)) {
        result = SYD_CFG_IID_OK;
    } else {
        result = SYD_CFG_IID_CHKERR;
    }
    return result;
}

Sint8 *syCfgGetIDAddress(void)
{
    Sint32 result;

    /* @hack spelling: result, where the call in the test would do */
    result = kbCfgIni(KBD_CFGINI);
    if (result != 0) {
        return NULL;
    }
    return kbCfgGetId();
}

Bool syCfgIIDCheckSum(const Sint8 *record)
{
    Uint8 check_not;
    Uint8 check_sum;
    Uint8 sum;
    Sint32 total;
    Sint32 i;

    /* @hack spelling: check_not and check_sum, read ahead of the sum, where
     * the bytes in the test would do */
    check_not = record[ID_CHECK_NOT];
    check_sum = record[ID_CHECK_SUM];
    total = 0;
    /* @hack spelling: the byte as *(record + ID_INDIVIDUAL + i), added in a
     * sum of its own, where total += (Uint8)record[ID_INDIVIDUAL + i] would
     * do */
    for (i = 0; i < SYD_CFG_IID_SIZE; i++) {
        total = total + (Uint8)(*(record + ID_INDIVIDUAL + i));
    }
    /* @hack redundant: total cut to its low byte, which sum keeps alone */
    total &= 0xff;
    sum = total;
    if (sum == check_sum && (~sum & 0xff) == check_not) {
        return TRUE;
    }
    return FALSE;
}
