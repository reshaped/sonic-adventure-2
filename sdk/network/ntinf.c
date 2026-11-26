/* the Network SDK's network information, read from flash */
#include <nt_utl.h>
#include <sg_fm.h>
#include <sg_xpt.h>
#include <stdlib.h>
#include <string.h>

/* the bits of ntgInfFlag. ntInfExit frees the workspace and the record
 * unless ntInfInit's caller gave them */
#define INF_FLAG_USER_WORK 0x00000001
#define INF_FLAG_USER_INFO 0x00000002
/* the record is a NetworkInfo3. ntInfInit sets the bit whenever it sets a
 * record. ntInfExit clears every bit as it drops the record */
#define INF_FLAG_INFO3 0x00000004

#define INF_FM_BANK 2
#define INF_FM_WORK_BYTES 0x4000

/* the information's first offset in the bank. each offset holds
 * FLASH_DATA_BYTES bytes of it */
#define INF_OFFSET_FIRST 0xc0
/* the bytes of a NetworkInfo3 that come before its pw_info. ntInfInit clears
 * and reads that many */
#define INF_INFO3_BYTES 0xac8
/* the last offset a NetworkInfo takes, and the last a NetworkInfo3's first
 * INF_INFO3_BYTES take */
#define INF_OFFSET_LAST_INFO                                                   \
    (INF_OFFSET_FIRST + (sizeof(NetworkInfo) / FLASH_DATA_BYTES) - 1)
#define INF_OFFSET_LAST_INFO3                                                  \
    (INF_OFFSET_FIRST + (INF_INFO3_BYTES / FLASH_DATA_BYTES) - 1)

/* the values of isp. INF_CURRENT takes the connection the information
 * selects */
#define INF_CURRENT 0
#define INF_ISP1 1
#define INF_ISP2 2
#define INF_LAN 3

/* the values of account, as MailAccount numbers the accounts */
#define INF_ACCOUNT_IN_USE 0
#define INF_ACCOUNT_OWN 1
#define INF_ACCOUNT_MAIL1 2
#define INF_ACCOUNT_MAIL2 3
#define INF_ACCOUNT_MAIL3 4
#define INF_ACCOUNT_MAIL4 5

/* the most bytes ntInfGetMailAddress copies from the LAN's mailAddress. the
 * field holds 4 bytes fewer */
#define INF_LAN_MAIL_COPY_BYTES 144

/* the most bytes of pintel that ntInfSplitTelNumber reads */
#define INF_TEL_BYTES 40

/* the smaller of size and field's size, compared as Sint32s */
#define INF_BOUND(size, field)                                                 \
    ((size) > (Sint32)sizeof(field) ? (Sint32)sizeof(field) : (size))

/* the flash manager's workspace, or null before ntInfInit sets one */
static void *inf_fm_work = NULL;

/* the member exports these names, though no header of the SDK declares
 * them */
/* NOLINTBEGIN(misc-use-internal-linkage) */
/* run the flash manager's request to its end, and return FMD_STAT_COMPLETE
 * or FMD_STAT_ERR. with no request since fmMngInit, or no bank open, it
 * never returns */
Sint32 _ntInfWaitDone(void);
/* where *isp is INF_CURRENT, set it to the ISP that the information's
 * ISP_USE2 selects. ntgInfNetworkInfo must be set */
void _ntInfSetCurrentIspNumber(Sint32 *isp);
/* as _ntInfSetCurrentIspNumber, but INF_CURRENT becomes INF_LAN in a
 * NetworkInfo3 whose flag has both VERSION_2 and USE_LAN */
void _ntInfSetCurrentIspNumber2(Sint32 *isp);
/* resolve *account, an account of ISP isp, to the account it stands for, as
 * MailAccount says. the call returns NTD_OK. an isp other than INF_ISP1 and
 * INF_ISP2, or an account outside the INF_ACCOUNT_ values, gives
 * NTD_ERR_INVALIDPARAM */
Sint32 _ntInfSetCurrentAccountNumber(Sint32 isp, Sint32 *account);
/* the information ntInfInit read, or null before ntInfInit sets a record */
NetworkInfo3 *ntgInfNetworkInfo = NULL;
/* the INF_FLAG_ bits */
Uint32 ntgInfFlag = 0;
/* NOLINTEND(misc-use-internal-linkage) */

static const char *const inf_build =
    "\nntInf Ver 4.05 Build:Jan 15 2001 11:13:11\n";

Sint32 _ntInfWaitDone(void)
{
    Sint32 status;

    do {
        status = fmMngExecServer();
    } while (status != FMD_STAT_COMPLETE && status != FMD_STAT_ERR);
    return status;
}

/* read the information's offsets into info. the call returns FMD_OK, or at
 * the first failure fmMngRead's error or FMD_STAT_ERR */
static Sint32 inf_read_flash(NetworkInfo3 *info)
{
    /* @hack spelling: buffer, a byte pointer set from info, where info cast
     * at its use would do */
    Uint8 *buffer = (Uint8 *)info;
    Sint32 last;
    Sint32 offset;
    Sint32 result;

    last = INF_OFFSET_LAST_INFO;
    if (ntgInfFlag & INF_FLAG_INFO3) {
        last = INF_OFFSET_LAST_INFO3;
    }
    for (offset = INF_OFFSET_FIRST; offset <= last; offset++) {
        result = fmMngRead(
            offset, buffer + ((offset - INF_OFFSET_FIRST) * FLASH_DATA_BYTES));
        if (result != FMD_OK) {
            return result;
        }
        result = _ntInfWaitDone();
        if (result != FMD_STAT_COMPLETE) {
            return result;
        }
    }
    return FMD_OK;
}

/* return NTD_OK where isp's access record holds its magic, and
 * NTD_ERR_NOINFO where it doesn't. INF_CURRENT tests ISP 1's record. an isp
 * outside INF_CURRENT to INF_ISP2 gives NTD_ERR_INVALIDPARAM. before
 * ntInfInit sets a record, the call returns NTD_ERR_NOTINIT */
static Sint32 inf_check_isp(Sint32 isp)
{
    NetworkInfo3 *info = ntgInfNetworkInfo;

    /* @hack spelling: the test for no information around the others, which
     * puts its return last, where an early return would do */
    if (info != NULL) {
        /* @bug logic: INF_CURRENT tests ISP 1's magic even where ISP_USE2
         * makes ISP 2 current (MOV.L @(4,R5),R3 at 0x8c0bdc38). it then
         * fails where ISP 1's record alone is missing. where ISP 2's record
         * alone is missing, ntInfGetNetworkAccessInfo returns it untested
         * (0x8c0be0ca). no word in any image holds the address of any of
         * this function's callers */
        if (isp == INF_CURRENT || isp == INF_ISP1) {
            if (info->isp_info1.magic == NETINFO_MAGIC) {
                return NTD_OK;
            }
            return NTD_ERR_NOINFO;
        }
        if (isp == INF_ISP2) {
            if (info->isp_info2.magic == NETINFO_MAGIC) {
                return NTD_OK;
            }
            return NTD_ERR_NOINFO;
        }
        return NTD_ERR_INVALIDPARAM;
    }
    return NTD_ERR_NOTINIT;
}

/* as inf_check_isp, but isp may also be INF_LAN, which needs a valid LAN
 * record. INF_CURRENT passes on ISP 1's magic, or on the LAN's where the
 * information's flag has VERSION_2 or USE_LAN */
static Sint32 inf_check_connection(Sint32 isp)
{
    NetworkInfo3 *info = ntgInfNetworkInfo;

    /* @hack spelling: the gotos to the returns at the end, where a return at
     * each test would do */
    /* @hack redundant: the else after the goto */
    if (info == NULL) {
        goto no_info;
    } else if (isp == INF_CURRENT && (ntgInfFlag & INF_FLAG_INFO3)) {
        /* @bug logic: INF_CURRENT tests ISP 1's magic or the LAN's, not the
         * magic of the record its caller reads (0x8c0bdc96 and 0x8c0bdca6).
         * where ISP_USE2 makes ISP 2 current, ntInfGetLoginId reads ISP 2's
         * login id untested (0x8c0be14c). of this function's callers, words
         * in the images hold the addresses of ntInfIsNetworkInfo and
         * ntInfGetNetworkInfo3 alone. CHAOSTGKINDER reads both ISPs' records
         * through ntInfGetNetworkInfo3, and tests neither magic (0x0c9d3be6) */
        if (info->isp_info1.magic != NETINFO_MAGIC) {
            /* @hack redundant: the else after the goto */
            if (!(info->flag & (VERSION_2 | USE_LAN))) {
                goto missing;
            } else if (info->lan_info.magic != NETINFO_MAGIC) {
                goto missing;
            }
        }
        goto valid;
    }
    if (isp == INF_CURRENT || isp == INF_ISP1) {
        if (info->isp_info1.magic != NETINFO_MAGIC) {
            goto missing;
        }
        goto valid;
    }
    if (isp == INF_ISP2) {
        if (info->isp_info2.magic != NETINFO_MAGIC) {
            goto missing;
        }
        goto valid;
    }
    if (isp != INF_LAN) {
        goto invalid;
    }
    if (!(ntgInfFlag & INF_FLAG_INFO3)) {
        goto invalid;
    }
    if (!(info->flag & VERSION_2) || info->lan_info.magic != NETINFO_MAGIC) {
        goto missing;
    }
valid:
    return NTD_OK;
missing:
    return NTD_ERR_NOINFO;
invalid:
    return NTD_ERR_INVALIDPARAM;
no_info:
    return NTD_ERR_NOTINIT;
}

/* return NTD_OK where the common record holds its magic, and NTD_ERR_NOINFO
 * where it doesn't. ntgInfNetworkInfo must be set */
static Sint32 inf_check_common(void)
{
    if (ntgInfNetworkInfo->comm_info.magic == NETINFO_MAGIC) {
        return NTD_OK;
    }
    return NTD_ERR_NOINFO;
}

/* return NTD_OK where ISP isp has a valid extra record, and NTD_ERR_NOINFO
 * where it hasn't. NETINFO_MAGIC says what makes the record valid. before
 * ntInfInit sets a record, the call returns NTD_ERR_NOINFO too. an isp other
 * than INF_ISP1 and INF_ISP2 gives NTD_ERR_INVALIDPARAM */
static Sint32 inf_check_ext(Sint32 isp)
{
    NetworkAccessInfo *access;
    NetworkAccessInfo2 *ext;

    if (isp != INF_ISP1 && isp != INF_ISP2) {
        return NTD_ERR_INVALIDPARAM;
    }
    if (!(ntgInfFlag & INF_FLAG_INFO3)) {
        return NTD_ERR_NOINFO;
    }
    access = isp == INF_ISP1 ? &ntgInfNetworkInfo->isp_info1
                             : &ntgInfNetworkInfo->isp_info2;
    ext = isp == INF_ISP1 ? &ntgInfNetworkInfo->ext_info1
                          : &ntgInfNetworkInfo->ext_info2;
    if (!(ntgInfNetworkInfo->flag & VERSION_2)) {
        return NTD_ERR_NOINFO;
    }
    /* @hack redundant: the else after the return */
    if (!(access->flag & USE_EXTRA_INFO)) {
        return NTD_ERR_NOINFO;
    } else if (ext->magic != NETINFO_MAGIC) {
        return NTD_ERR_NOINFO;
    }
    return NTD_OK;
}

/* open INF_FM_BANK in work, and format it where fmMngInit finds its header
 * wrong. the call returns fmMngFormat's result where it formats, and
 * fmMngInit's where it doesn't */
static Sint32 inf_init_flash(void *work)
{
    Sint32 result;

    result = fmMngInit(INF_FM_BANK, work);
    if (result == FLASH_BAD_HEADER) {
        result = fmMngFormat(FLASH_TYPE_LOG);
    }
    return result;
}

void _ntInfSetCurrentIspNumber(Sint32 *isp)
{
    if (*isp == INF_CURRENT) {
        *isp = (ntgInfNetworkInfo->flag & ISP_USE2) ? INF_ISP2 : INF_ISP1;
    }
}

void _ntInfSetCurrentIspNumber2(Sint32 *isp)
{
    if (!(ntgInfFlag & INF_FLAG_INFO3)) {
        _ntInfSetCurrentIspNumber(isp);
    } else if (*isp == INF_CURRENT) {
        /* NOLINTBEGIN(readability-avoid-nested-conditional-operator) */
        /* @hack spelling: a chain of conditional operators, where an if
         * chain would do */
        *isp = (ntgInfNetworkInfo->flag & VERSION_2) &&
                       (ntgInfNetworkInfo->flag & USE_LAN)
                   ? INF_LAN
               : (ntgInfNetworkInfo->flag & ISP_USE2) ? INF_ISP2
                                                      : INF_ISP1;
        /* NOLINTEND(readability-avoid-nested-conditional-operator) */
    }
}

/* where *isp is INF_CURRENT, set it to the connection for mail that the
 * common record names. the common record must hold its magic */
static void inf_set_current_mail_isp(Sint32 *isp)
{
    if (!(ntgInfFlag & INF_FLAG_INFO3)) {
        _ntInfSetCurrentIspNumber(isp);
    } else if (*isp == INF_CURRENT) {
        switch (ntgInfNetworkInfo->comm_info.flag & NTINF_COMM_MAIL_MASK) {
        case NTINF_COMM_MAIL_ISP1:
            *isp = INF_ISP1;
            break;
        case NTINF_COMM_MAIL_ISP2:
            *isp = INF_ISP2;
            break;
        case NTINF_COMM_MAIL_LAN:
            *isp = INF_LAN;
            break;
        case NTINF_COMM_MAIL_AUTO:
        default:
            _ntInfSetCurrentIspNumber2(isp);
            break;
        }
    }
}

Sint32 _ntInfSetCurrentAccountNumber(Sint32 isp, Sint32 *account)
{
    NetworkAccessInfo2 *ext;
    Sint32 result;

    if (isp != INF_ISP1 && isp != INF_ISP2) {
        return NTD_ERR_INVALIDPARAM;
    }
    if (*account < INF_ACCOUNT_IN_USE || *account > INF_ACCOUNT_MAIL4) {
        return NTD_ERR_INVALIDPARAM;
    }
    result = inf_check_ext(isp);
    if (result != NTD_OK) {
        *account = INF_ACCOUNT_OWN;
        return NTD_OK;
    }
    if (*account != INF_ACCOUNT_IN_USE) {
        return NTD_OK;
    }
    ext = isp == INF_ISP1 ? &ntgInfNetworkInfo->ext_info1
                          : &ntgInfNetworkInfo->ext_info2;
    /* NOLINTBEGIN(readability-avoid-nested-conditional-operator) */
    /* @hack spelling: a chain of conditional operators, where an if chain
     * would do */
    *account = (ext->flag & NA2_USE_MAIL1)   ? INF_ACCOUNT_MAIL1
               : (ext->flag & NA2_USE_MAIL2) ? INF_ACCOUNT_MAIL2
               : (ext->flag & NA2_USE_MAIL3) ? INF_ACCOUNT_MAIL3
               : (ext->flag & NA2_USE_MAIL4) ? INF_ACCOUNT_MAIL4
                                             : INF_ACCOUNT_OWN;
    /* NOLINTEND(readability-avoid-nested-conditional-operator) */
    return NTD_OK;
}

/* ntInfInit by another name. no word in any image holds its address */
/* @hack redundant: external linkage, which no call needs, where static would
 * do */
/* NOLINTNEXTLINE(misc-use-internal-linkage) */
Sint32 inf_init_alias(NetworkInfo3 *pNetInfo, void *pWork)
{
    return ntInfInit(pNetInfo, pWork);
}

Sint32 ntInfInit(NetworkInfo3 *pNetInfo, void *pWork)
{
    Sint32 result;

    /* @hack redundant: the else after the return */
    if (ntgInfNetworkInfo != NULL) {
        return NTD_OK;
    } else if (pWork == NULL) {
        inf_fm_work = malloc(INF_FM_WORK_BYTES);
        if (inf_fm_work == NULL) {
            return NTD_ERR_INVALIDPARAM;
        }
    } else {
        inf_fm_work = pWork;
        ntgInfFlag |= INF_FLAG_USER_WORK;
    }
    if (inf_init_flash(inf_fm_work) != FMD_OK) {
        return NTD_ERR_NOTINIT;
    }
    if (pNetInfo == NULL) {
        ntgInfNetworkInfo = malloc(INF_INFO3_BYTES);
        if (ntgInfNetworkInfo == NULL) {
            return NTD_ERR_INVALIDPARAM;
        }
    } else {
        ntgInfNetworkInfo = pNetInfo;
        ntgInfFlag |= INF_FLAG_USER_INFO;
    }
    ntgInfFlag |= INF_FLAG_INFO3;
    memset(ntgInfNetworkInfo, 0, INF_INFO3_BYTES);
    result = inf_read_flash(ntgInfNetworkInfo);
    if (result != FMD_OK) {
        return NTD_ERR_NOTINIT;
    }
    return NTD_OK;
}

Sint32 ntInfExit(void)
{
    if (inf_fm_work != NULL) {
        fmMngFinish();
        if (!(ntgInfFlag & INF_FLAG_USER_WORK)) {
            free(inf_fm_work);
        }
        inf_fm_work = NULL;
    }
    if (ntgInfNetworkInfo != NULL) {
        if (!(ntgInfFlag & INF_FLAG_USER_INFO)) {
            free(ntgInfNetworkInfo);
        }
        ntgInfNetworkInfo = NULL;
    }
    ntgInfFlag = 0;
    return NTD_OK;
}

Sint32 ntInfIsNetworkInfo(void)
{
    Sint32 result;

    ntInfInit(NULL, NULL);
    result = inf_check_connection(INF_CURRENT);
    ntInfExit();
    return result;
}

Sint32 ntInfGetNetworkInfo(NetworkInfo **ppNetInfo)
{
    Sint32 result;

    result = inf_check_isp(INF_CURRENT);
    /* @hack spelling: the test of result around the rest, which returns
     * result last, where an early return would do */
    if (result == NTD_OK) {
        *ppNetInfo = (NetworkInfo *)ntgInfNetworkInfo;
    }
    return result;
}

Sint32 ntInfGetNetworkInfo3(NetworkInfo3 **ppNetInfo)
{
    Sint32 result;

    result = inf_check_connection(INF_CURRENT);
    /* @hack spelling: the test of result around the rest, which returns
     * result last, where an early return would do */
    if (result == NTD_OK) {
        *ppNetInfo = ntgInfNetworkInfo;
    }
    return result;
}

Sint32 ntInfGetNetworkAccessInfo(Sint32 isp, NetworkAccessInfo **ppNetAccInfo)
{
    Sint32 result;
    Sint32 current;

    /* @hack spelling: current, a copy of isp that a call rewrites, where
     * &isp would do */
    current = isp;
    result = inf_check_isp(isp);
    /* @hack spelling: the test of result around the rest, which returns
     * result last, where an early return would do */
    if (result == NTD_OK) {
        _ntInfSetCurrentIspNumber(&current);
        if (current == INF_ISP1) {
            *ppNetAccInfo = &ntgInfNetworkInfo->isp_info1;
        } else if (current == INF_ISP2) {
            *ppNetAccInfo = &ntgInfNetworkInfo->isp_info2;
        } else {
            result = NTD_ERR_NOINFO;
        }
    }
    return result;
}

Sint32 ntInfGetFlag(Uint32 *fFlag)
{
    Sint32 result;

    result = inf_check_connection(INF_CURRENT);
    /* @hack spelling: the test of result around the rest, which returns
     * result last, where an early return would do */
    if (result == NTD_OK) {
        *fFlag = ntgInfNetworkInfo->flag;
    }
    return result;
}

Sint32 ntInfGetLoginId(Sint32 isp, char *pLoginId)
{
    Sint32 result;
    Sint32 current;

    /* @hack spelling: current, a copy of isp that a call rewrites, where
     * &isp would do */
    current = isp;
    result = inf_check_connection(isp);
    /* @hack spelling: the test of result around the rest, which returns
     * result last, where an early return would do */
    if (result == NTD_OK) {
        _ntInfSetCurrentIspNumber2(&current);
        if (current == INF_ISP1) {
            strncpy(pLoginId, ntgInfNetworkInfo->isp_info1.loginId,
                    sizeof(ntgInfNetworkInfo->isp_info1.loginId));
        } else if (current == INF_ISP2) {
            strncpy(pLoginId, ntgInfNetworkInfo->isp_info2.loginId,
                    sizeof(ntgInfNetworkInfo->isp_info2.loginId));
        } else if (current == INF_LAN) {
            strncpy(pLoginId, ntgInfNetworkInfo->lan_info.loginId,
                    sizeof(ntgInfNetworkInfo->lan_info.loginId));
        } else {
            result = NTD_ERR_NOINFO;
        }
    }
    return result;
}

Sint32 ntInfGetLoginPasswd(Sint32 isp, char *pLoginPasswd)
{
    Sint32 result;
    Sint32 current;

    /* @hack spelling: current, a copy of isp that a call rewrites, where
     * &isp would do */
    current = isp;
    result = inf_check_connection(isp);
    /* @hack spelling: the test of result around the rest, which returns
     * result last, where an early return would do */
    if (result == NTD_OK) {
        _ntInfSetCurrentIspNumber2(&current);
        if (current == INF_ISP1) {
            strncpy(pLoginPasswd, ntgInfNetworkInfo->isp_info1.loginPasswd,
                    sizeof(ntgInfNetworkInfo->isp_info1.loginPasswd));
        } else if (current == INF_ISP2) {
            strncpy(pLoginPasswd, ntgInfNetworkInfo->isp_info2.loginPasswd,
                    sizeof(ntgInfNetworkInfo->isp_info2.loginPasswd));
        } else if (current == INF_LAN) {
            strncpy(pLoginPasswd, ntgInfNetworkInfo->lan_info.loginPasswd,
                    sizeof(ntgInfNetworkInfo->lan_info.loginPasswd));
        } else {
            result = NTD_ERR_NOINFO;
        }
    }
    return result;
}

Sint32 ntInfGetAccessPointNumber(Sint32 isp, char *pAccessPointNumber)
{
    Sint32 result;
    Sint32 current;

    /* @hack spelling: current, a copy of isp that a call rewrites, where
     * &isp would do */
    current = isp;
    result = inf_check_isp(isp);
    /* @hack spelling: the test of result around the rest, which returns
     * result last, where an early return would do */
    if (result == NTD_OK) {
        _ntInfSetCurrentIspNumber(&current);
        if (current == INF_ISP1) {
            memcpy(pAccessPointNumber,
                   ntgInfNetworkInfo->isp_info1.accessPointNumber,
                   sizeof(ntgInfNetworkInfo->isp_info1.accessPointNumber));
        } else if (current == INF_ISP2) {
            memcpy(pAccessPointNumber,
                   ntgInfNetworkInfo->isp_info2.accessPointNumber,
                   sizeof(ntgInfNetworkInfo->isp_info2.accessPointNumber));
        } else {
            result = NTD_ERR_NOINFO;
        }
    }
    return result;
}

Sint32 ntInfGetDnsServerAddress(Sint32 isp, struct in_addr *pDns1,
                                struct in_addr *pDns2)
{
    Sint32 result;
    Sint32 current;

    /* @hack spelling: current, a copy of isp that a call rewrites, where
     * &isp would do */
    current = isp;
    result = inf_check_connection(isp);
    if (result != NTD_OK) {
        return result;
    }
    _ntInfSetCurrentIspNumber2(&current);
    if (current == INF_ISP1) {
        memcpy(pDns1, &ntgInfNetworkInfo->isp_info1.dnsServerAddress1,
               sizeof(struct in_addr));
        memcpy(pDns2, &ntgInfNetworkInfo->isp_info1.dnsServerAddress2,
               sizeof(struct in_addr));
    } else if (current == INF_ISP2) {
        memcpy(pDns1, &ntgInfNetworkInfo->isp_info2.dnsServerAddress1,
               sizeof(struct in_addr));
        memcpy(pDns2, &ntgInfNetworkInfo->isp_info2.dnsServerAddress2,
               sizeof(struct in_addr));
    } else if (current == INF_LAN) {
        memcpy(pDns1, &ntgInfNetworkInfo->lan_info.dnsServerAddress1,
               sizeof(struct in_addr));
        memcpy(pDns2, &ntgInfNetworkInfo->lan_info.dnsServerAddress2,
               sizeof(struct in_addr));
    } else {
        result = NTD_ERR_NOINFO;
    }
    return result;
}

Sint32 ntInfGetMailAddress(Sint32 isp, Sint32 account, char *pMailAddress,
                           Sint32 size)
{
    Sint32 result;
    NetworkAccessInfo *access;
    NetworkAccessInfo2 *ext;
    char *source;
    Sint32 current;
    Sint32 number;

    /* @hack spelling: current, a copy of isp that a call rewrites, where
     * &isp would do */
    current = isp;
    /* @hack spelling: number, a copy of account that a call rewrites, where
     * &account would do */
    number = account;
    result = inf_check_connection(current);
    if (result != NTD_OK) {
        return result;
    }
    result = inf_check_common();
    if (result != NTD_OK) {
        _ntInfSetCurrentIspNumber2(&current);
    } else {
        inf_set_current_mail_isp(&current);
    }
    if (current == INF_LAN) {
        /* @bug memory: reads past the LAN's mailAddress into its mailServer
         * where the address fills the field with no null and size is larger
         * than the field. the copy's bound is INF_LAN_MAIL_COPY_BYTES, not
         * the field's size (MOV.W at 0x8c0be31c) */
        strncpy(pMailAddress, ntgInfNetworkInfo->lan_info.mailAddress,
                size > INF_LAN_MAIL_COPY_BYTES ? INF_LAN_MAIL_COPY_BYTES
                                               : size);
    } else {
        access = current == INF_ISP1 ? &ntgInfNetworkInfo->isp_info1
                                     : &ntgInfNetworkInfo->isp_info2;
        result = _ntInfSetCurrentAccountNumber(current, &number);
        if (result != NTD_OK) {
            return result;
        }
        if (number == INF_ACCOUNT_OWN) {
            source = access->mailAddress;
        } else {
            ext = current == INF_ISP1 ? &ntgInfNetworkInfo->ext_info1
                                      : &ntgInfNetworkInfo->ext_info2;
            source = ext->mailAccount[number - INF_ACCOUNT_MAIL1].mailAddress;
        }
        strncpy(pMailAddress, source, INF_BOUND(size, access->mailAddress));
    }
    return NTD_OK;
}

Sint32 ntInfGetMailServer(Sint32 isp, Sint32 account, char *pMailServer,
                          Sint32 size)
{
    Sint32 result;
    NetworkAccessInfo *access;
    NetworkAccessInfo2 *ext;
    char *source;
    Sint32 current;
    Sint32 number;

    /* @hack spelling: current, a copy of isp that a call rewrites, where
     * &isp would do */
    current = isp;
    /* @hack spelling: number, a copy of account that a call rewrites, where
     * &account would do */
    number = account;
    result = inf_check_connection(current);
    if (result != NTD_OK) {
        return result;
    }
    result = inf_check_common();
    if (result != NTD_OK) {
        _ntInfSetCurrentIspNumber2(&current);
    } else {
        inf_set_current_mail_isp(&current);
    }
    if (current == INF_LAN) {
        strncpy(pMailServer, ntgInfNetworkInfo->lan_info.mailServer,
                INF_BOUND(size, ntgInfNetworkInfo->lan_info.mailServer));
    } else {
        access = current == INF_ISP1 ? &ntgInfNetworkInfo->isp_info1
                                     : &ntgInfNetworkInfo->isp_info2;
        result = _ntInfSetCurrentAccountNumber(current, &number);
        if (result != NTD_OK) {
            return result;
        }
        if (number == INF_ACCOUNT_OWN) {
            source = access->mailServer;
        } else {
            ext = current == INF_ISP1 ? &ntgInfNetworkInfo->ext_info1
                                      : &ntgInfNetworkInfo->ext_info2;
            source = ext->mailAccount[number - INF_ACCOUNT_MAIL1].mailServer;
        }
        strncpy(pMailServer, source, INF_BOUND(size, access->mailServer));
    }
    return NTD_OK;
}

Sint32 ntInfGetPopServer(Sint32 isp, Sint32 account, char *pPopServer,
                         Sint32 size)
{
    Sint32 result;
    NetworkAccessInfo *access;
    NetworkAccessInfo2 *ext;
    char *source;
    Sint32 current;
    Sint32 number;

    /* @hack spelling: current, a copy of isp that a call rewrites, where
     * &isp would do */
    current = isp;
    /* @hack spelling: number, a copy of account that a call rewrites, where
     * &account would do */
    number = account;
    result = inf_check_connection(current);
    if (result != NTD_OK) {
        return result;
    }
    result = inf_check_common();
    if (result != NTD_OK) {
        _ntInfSetCurrentIspNumber2(&current);
    } else {
        inf_set_current_mail_isp(&current);
    }
    if (current == INF_LAN) {
        strncpy(pPopServer, ntgInfNetworkInfo->lan_info.popServer,
                INF_BOUND(size, ntgInfNetworkInfo->lan_info.popServer));
    } else {
        access = current == INF_ISP1 ? &ntgInfNetworkInfo->isp_info1
                                     : &ntgInfNetworkInfo->isp_info2;
        result = _ntInfSetCurrentAccountNumber(current, &number);
        if (result != NTD_OK) {
            return result;
        }
        if (number == INF_ACCOUNT_OWN) {
            source = access->popServer;
        } else {
            ext = current == INF_ISP1 ? &ntgInfNetworkInfo->ext_info1
                                      : &ntgInfNetworkInfo->ext_info2;
            source = ext->mailAccount[number - INF_ACCOUNT_MAIL1].popServer;
        }
        strncpy(pPopServer, source, INF_BOUND(size, access->popServer));
    }
    return NTD_OK;
}

Sint32 ntInfGetPopId(Sint32 isp, Sint32 account, char *pPopId, Sint32 size)
{
    Sint32 result;
    NetworkAccessInfo *access;
    NetworkAccessInfo2 *ext;
    char *source;
    Sint32 current;
    Sint32 number;

    /* @hack spelling: current, a copy of isp that a call rewrites, where
     * &isp would do */
    current = isp;
    /* @hack spelling: number, a copy of account that a call rewrites, where
     * &account would do */
    number = account;
    result = inf_check_connection(current);
    if (result != NTD_OK) {
        return result;
    }
    result = inf_check_common();
    if (result != NTD_OK) {
        _ntInfSetCurrentIspNumber2(&current);
    } else {
        inf_set_current_mail_isp(&current);
    }
    if (current == INF_LAN) {
        strncpy(pPopId, ntgInfNetworkInfo->lan_info.popId,
                INF_BOUND(size, ntgInfNetworkInfo->lan_info.popId));
    } else {
        access = current == INF_ISP1 ? &ntgInfNetworkInfo->isp_info1
                                     : &ntgInfNetworkInfo->isp_info2;
        result = _ntInfSetCurrentAccountNumber(current, &number);
        if (result != NTD_OK) {
            return result;
        }
        if (number == INF_ACCOUNT_OWN) {
            source = access->popId;
        } else {
            ext = current == INF_ISP1 ? &ntgInfNetworkInfo->ext_info1
                                      : &ntgInfNetworkInfo->ext_info2;
            source = ext->mailAccount[number - INF_ACCOUNT_MAIL1].popId;
        }
        strncpy(pPopId, source, INF_BOUND(size, access->popId));
    }
    return NTD_OK;
}

Sint32 ntInfGetPopPasswd(Sint32 isp, Sint32 account, char *pPopPasswd,
                         Sint32 size)
{
    Sint32 result;
    NetworkAccessInfo *access;
    NetworkAccessInfo2 *ext;
    char *source;
    Sint32 current;
    Sint32 number;

    /* @hack spelling: current, a copy of isp that a call rewrites, where
     * &isp would do */
    current = isp;
    /* @hack spelling: number, a copy of account that a call rewrites, where
     * &account would do */
    number = account;
    result = inf_check_connection(current);
    if (result != NTD_OK) {
        return result;
    }
    result = inf_check_common();
    if (result != NTD_OK) {
        _ntInfSetCurrentIspNumber2(&current);
    } else {
        inf_set_current_mail_isp(&current);
    }
    if (current == INF_LAN) {
        strncpy(pPopPasswd, ntgInfNetworkInfo->lan_info.popPasswd,
                INF_BOUND(size, ntgInfNetworkInfo->lan_info.popPasswd));
    } else {
        access = current == INF_ISP1 ? &ntgInfNetworkInfo->isp_info1
                                     : &ntgInfNetworkInfo->isp_info2;
        result = _ntInfSetCurrentAccountNumber(current, &number);
        if (result != NTD_OK) {
            return result;
        }
        if (number == INF_ACCOUNT_OWN) {
            source = access->popPasswd;
        } else {
            ext = current == INF_ISP1 ? &ntgInfNetworkInfo->ext_info1
                                      : &ntgInfNetworkInfo->ext_info2;
            source = ext->mailAccount[number - INF_ACCOUNT_MAIL1].popPasswd;
        }
        strncpy(pPopPasswd, source, INF_BOUND(size, access->popPasswd));
    }
    return NTD_OK;
}

Sint32 ntInfGetProxyServer(Sint32 isp, char *pProxyServer, Sint32 size)
{
    Sint32 result;
    NetworkAccessInfo *access;
    Sint32 current;

    /* @hack spelling: current, a copy of isp that a call rewrites, where
     * &isp would do */
    current = isp;
    result = inf_check_connection(isp);
    if (result != NTD_OK) {
        return result;
    }
    _ntInfSetCurrentIspNumber2(&current);
    if (current == INF_LAN) {
        strncpy(pProxyServer, ntgInfNetworkInfo->lan_info.proxyServer,
                INF_BOUND(size, ntgInfNetworkInfo->lan_info.proxyServer));
    } else {
        access = current == INF_ISP1 ? &ntgInfNetworkInfo->isp_info1
                                     : &ntgInfNetworkInfo->isp_info2;
        strncpy(pProxyServer, access->proxyServer,
                INF_BOUND(size, access->proxyServer));
    }
    return NTD_OK;
}

Sint32 ntInfGetProxyPortNum(Sint32 isp, Uint16 *pPort)
{
    Sint32 result;
    Sint32 current;

    /* @hack spelling: current, a copy of isp that a call rewrites, where
     * &isp would do */
    current = isp;
    result = inf_check_connection(isp);
    if (result != NTD_OK) {
        return result;
    }
    _ntInfSetCurrentIspNumber2(&current);
    if (current == INF_LAN) {
        *pPort = ntgInfNetworkInfo->lan_info.proxyPortNum;
    } else {
        *pPort = current == INF_ISP1
                     ? ntgInfNetworkInfo->isp_info1.proxyPortNum
                     : ntgInfNetworkInfo->isp_info2.proxyPortNum;
    }
    return NTD_OK;
}

Sint32 ntInfGetNetworkAccessInfoFlag(Sint32 isp, Uint16 *pFlag)
{
    Sint32 result;
    Sint32 current;

    /* @hack spelling: current, a copy of isp that a call rewrites, where
     * &isp would do */
    current = isp;
    result = inf_check_isp(isp);
    if (result != NTD_OK) {
        return result;
    }
    _ntInfSetCurrentIspNumber(&current);
    *pFlag = current == INF_ISP1 ? ntgInfNetworkInfo->isp_info1.flag
                                 : ntgInfNetworkInfo->isp_info2.flag;
    return NTD_OK;
}

Sint32 ntInfGetNetworkAccessInfo2(Sint32 isp,
                                  NetworkAccessInfo2 **ppNetAccInfo2)
{
    Sint32 result;
    Sint32 current;

    /* @hack spelling: current, a copy of isp that a call rewrites, where
     * &isp would do */
    current = isp;
    result = inf_check_isp(isp);
    if (result != NTD_OK) {
        return result;
    }
    _ntInfSetCurrentIspNumber(&current);
    result = inf_check_ext(current);
    if (result != NTD_OK) {
        return result;
    }
    *ppNetAccInfo2 = current == INF_ISP1 ? &ntgInfNetworkInfo->ext_info1
                                         : &ntgInfNetworkInfo->ext_info2;
    return NTD_OK;
}

Sint32 ntInfGetTCPIPInfo(TCPIPInfo **ppTCPIPInfo)
{
    Sint32 result;

    result = inf_check_connection(INF_LAN);
    if (result != NTD_OK) {
        return NTD_ERR_NOINFO;
    }
    *ppTCPIPInfo = &ntgInfNetworkInfo->lan_info;
    return NTD_OK;
}

Sint32 ntInfGetNetworkAccessInfo2Flag(Sint32 isp, Uint32 *pFlag)
{
    Sint32 result;
    Sint32 current;

    /* @hack spelling: current, a copy of isp that a call rewrites, where
     * &isp would do */
    current = isp;
    result = inf_check_isp(isp);
    if (result != NTD_OK) {
        return result;
    }
    _ntInfSetCurrentIspNumber(&current);
    result = inf_check_ext(current);
    if (result != NTD_OK) {
        return result;
    }
    *pFlag = current == INF_ISP1 ? ntgInfNetworkInfo->ext_info1.flag
                                 : ntgInfNetworkInfo->ext_info2.flag;
    return NTD_OK;
}

Sint32 ntInfGetIPAddr(Sint32 isp, struct in_addr *IPAddr)
{
    Sint32 result;
    NetworkAccessInfo2 *ext;
    Sint32 current;

    /* @hack spelling: current, a copy of isp that a call rewrites, where
     * &isp would do */
    current = isp;
    result = inf_check_connection(isp);
    if (result != NTD_OK) {
        return result;
    }
    _ntInfSetCurrentIspNumber2(&current);
    if (current == INF_LAN) {
        memcpy(IPAddr, &ntgInfNetworkInfo->lan_info.ipAddr,
               sizeof(struct in_addr));
    } else {
        result = inf_check_ext(current);
        if (result != NTD_OK) {
            return result;
        }
        ext = current == INF_ISP1 ? &ntgInfNetworkInfo->ext_info1
                                  : &ntgInfNetworkInfo->ext_info2;
        memcpy(IPAddr, &ext->ipAddr, sizeof(struct in_addr));
    }
    return NTD_OK;
}

Sint32 ntInfGetParentalPasswd(Sint32 isp, char *pPasswd)
{
    Sint32 result;
    NetworkAccessInfo2 *ext;
    Sint32 current;

    /* @hack spelling: current, a copy of isp that a call rewrites, where
     * &isp would do */
    current = isp;
    result = inf_check_isp(isp);
    if (result != NTD_OK) {
        return result;
    }
    _ntInfSetCurrentIspNumber(&current);
    result = inf_check_ext(current);
    if (result != NTD_OK) {
        return result;
    }
    ext = current == INF_ISP1 ? &ntgInfNetworkInfo->ext_info1
                              : &ntgInfNetworkInfo->ext_info2;
    strncpy(pPasswd, ext->parentalPasswd, sizeof(ext->parentalPasswd));
    return NTD_OK;
}

Sint32 ntInfGetMailAccount(Sint32 isp, Uint32 account, MailAccount **ppMailAcct)
{
    Sint32 result;
    NetworkAccessInfo *access;
    NetworkAccessInfo2 *ext;
    Sint32 current;
    Sint32 number;

    /* @hack spelling: current, a copy of isp that a call rewrites, where
     * &isp would do */
    current = isp;
    number = account;
    if (account > INF_ACCOUNT_MAIL4) {
        return NTD_ERR_INVALIDPARAM;
    }
    result = inf_check_isp(isp);
    if (result != NTD_OK) {
        return result;
    }
    _ntInfSetCurrentIspNumber(&current);
    result = _ntInfSetCurrentAccountNumber(current, &number);
    if (result != NTD_OK) {
        return result;
    }
    access = current == INF_ISP1 ? &ntgInfNetworkInfo->isp_info1
                                 : &ntgInfNetworkInfo->isp_info2;
    ext = current == INF_ISP1 ? &ntgInfNetworkInfo->ext_info1
                              : &ntgInfNetworkInfo->ext_info2;
    /* the access record's fields from mailAddress to popPasswd have a
     * MailAccount's layout */
    *ppMailAcct = number == INF_ACCOUNT_OWN
                      ? (MailAccount *)access->mailAddress
                      : &ext->mailAccount[number - INF_ACCOUNT_MAIL1];
    return NTD_OK;
}

Sint32 ntInfGetTCPIPInfoFlag(Uint32 *pFlag)
{
    Sint32 result;

    result = inf_check_connection(INF_LAN);
    if (result != NTD_OK) {
        return NTD_ERR_NOINFO;
    }
    *pFlag = ntgInfNetworkInfo->lan_info.flag;
    return NTD_OK;
}

Sint32 ntInfGetSubnetMask(struct in_addr *pSubnetMask)
{
    Sint32 result;

    result = inf_check_connection(INF_LAN);
    if (result != NTD_OK) {
        return NTD_ERR_NOINFO;
    }
    memcpy(pSubnetMask, &ntgInfNetworkInfo->lan_info.subnetMask,
           sizeof(struct in_addr));
    return NTD_OK;
}

Sint32 ntInfGetBroadcast(struct in_addr *pBroadcast)
{
    Sint32 result;

    result = inf_check_connection(INF_LAN);
    if (result != NTD_OK) {
        return NTD_ERR_NOINFO;
    }
    memcpy(pBroadcast, &ntgInfNetworkInfo->lan_info.broadcast,
           sizeof(struct in_addr));
    return NTD_OK;
}

Sint32 ntInfGetGateway(struct in_addr *pGw1, struct in_addr *pGw2)
{
    Sint32 result;

    result = inf_check_connection(INF_LAN);
    if (result != NTD_OK) {
        return NTD_ERR_NOINFO;
    }
    memcpy(pGw1, &ntgInfNetworkInfo->lan_info.gateway1, sizeof(struct in_addr));
    memcpy(pGw2, &ntgInfNetworkInfo->lan_info.gateway2, sizeof(struct in_addr));
    return NTD_OK;
}

Sint32 ntInfGetHost(char *pHost)
{
    Sint32 result;

    result = inf_check_connection(INF_LAN);
    if (result != NTD_OK) {
        return NTD_ERR_NOINFO;
    }
    strncpy(pHost, ntgInfNetworkInfo->lan_info.host,
            sizeof(ntgInfNetworkInfo->lan_info.host));
    return NTD_OK;
}

Sint32 ntInfGetDomain(char *pDomain)
{
    Sint32 result;

    result = inf_check_connection(INF_LAN);
    if (result != NTD_OK) {
        return NTD_ERR_NOINFO;
    }
    strncpy(pDomain, ntgInfNetworkInfo->lan_info.domain,
            sizeof(ntgInfNetworkInfo->lan_info.domain));
    return NTD_OK;
}

Sint32 ntInfGetISP(Sint32 *isp)
{
    Sint32 result;

    result = inf_check_connection(INF_CURRENT);
    if (result != NTD_OK) {
        return result;
    }
    *isp = INF_CURRENT;
    _ntInfSetCurrentIspNumber2(isp);
    return NTD_OK;
}

/* store in *isp the connection for mail that ntInfGetMailAddress takes for
 * isp 0. the call returns NTD_OK, or inf_check_connection's failure for
 * INF_CURRENT. no word in any image holds its address */
/* @hack redundant: external linkage, which no call needs, where static would
 * do */
/* NOLINTNEXTLINE(misc-use-internal-linkage) */
Sint32 inf_get_mail_isp(Sint32 *isp)
{
    Sint32 result;

    result = inf_check_connection(INF_CURRENT);
    if (result != NTD_OK) {
        return result;
    }
    *isp = INF_CURRENT;
    result = inf_check_common();
    if (result != NTD_OK) {
        _ntInfSetCurrentIspNumber2(isp);
    } else {
        inf_set_current_mail_isp(isp);
    }
    return NTD_OK;
}

Sint32 ntInfGetURL(char *pURL)
{
    Sint32 result;

    result = inf_check_connection(INF_CURRENT);
    if (result != NTD_OK) {
        return result;
    }
    result = inf_check_common();
    if (result != NTD_OK) {
        return result;
    }
    strncpy(pURL, ntgInfNetworkInfo->comm_info.url,
            sizeof(ntgInfNetworkInfo->comm_info.url));
    return NTD_OK;
}

Sint32 ntInfGetSummerTimeStart(NetworkSummerTime *pstStart)
{
    Sint32 result;

    result = inf_check_connection(INF_CURRENT);
    if (result != NTD_OK) {
        return result;
    }
    result = inf_check_common();
    if (result != NTD_OK) {
        return result;
    }
    if (!(ntgInfNetworkInfo->comm_info.flag & NTINF_COMM_DP3)) {
        return NTD_ERR_NOINFO;
    }
    memcpy(pstStart, &ntgInfNetworkInfo->comm_info.stStart,
           sizeof(NetworkSummerTime));
    return NTD_OK;
}

Sint32 ntInfGetSummerTimeEnd(NetworkSummerTime *pstEnd)
{
    Sint32 result;

    result = inf_check_connection(INF_CURRENT);
    if (result != NTD_OK) {
        return result;
    }
    result = inf_check_common();
    if (result != NTD_OK) {
        return result;
    }
    if (!(ntgInfNetworkInfo->comm_info.flag & NTINF_COMM_DP3)) {
        return NTD_ERR_NOINFO;
    }
    memcpy(pstEnd, &ntgInfNetworkInfo->comm_info.stEnd,
           sizeof(NetworkSummerTime));
    return NTD_OK;
}

Sint32 ntInfGetSummerTimeDiff(Sint32 *pdiff)
{
    Sint32 result;

    result = inf_check_connection(INF_CURRENT);
    if (result != NTD_OK) {
        return result;
    }
    result = inf_check_common();
    if (result != NTD_OK) {
        return result;
    }
    if (!(ntgInfNetworkInfo->comm_info.flag & NTINF_COMM_DP3)) {
        return NTD_ERR_NOINFO;
    }
    *pdiff = ntgInfNetworkInfo->comm_info.stDiff;
    return NTD_OK;
}

Sint32 ntInfSummerTime(void)
{
    Sint32 result;

    result = inf_check_connection(INF_CURRENT);
    if (result != NTD_OK) {
        return result;
    }
    result = inf_check_common();
    if (result != NTD_OK) {
        return result;
    }
    /* @hack spelling: the gotos to the return at the end, where a return at
     * each test would do */
    if (!(ntgInfNetworkInfo->comm_info.flag & NTINF_COMM_DP3)) {
        goto none;
    }
    if (!(ntgInfNetworkInfo->comm_info.flag & NTINF_COMM_SUMMERTIME)) {
        goto none;
    }
    return NTD_OK;
none:
    return NTD_ERR_NOINFO;
}

Sint32 ntInfExternalModem(void)
{
    Sint32 result;

    result = inf_check_connection(INF_CURRENT);
    if (result != NTD_OK) {
        return result;
    }
    result = inf_check_common();
    if (result != NTD_OK) {
        return result;
    }
    /* @hack spelling: the gotos to the return at the end, where a return at
     * each test would do */
    if (!(ntgInfNetworkInfo->comm_info.flag & NTINF_COMM_DP3)) {
        goto none;
    }
    if (!(ntgInfNetworkInfo->comm_info.flag & NTINF_COMM_EXTERNAL_MODEM)) {
        goto none;
    }
    return NTD_OK;
none:
    return NTD_ERR_NOINFO;
}

Sint32 ntInfGetModemInit(char *pinit)
{
    Sint32 result;

    result = inf_check_connection(INF_CURRENT);
    if (result != NTD_OK) {
        return result;
    }
    result = inf_check_common();
    if (result != NTD_OK) {
        return result;
    }
    if (!(ntgInfNetworkInfo->comm_info.flag & NTINF_COMM_DP3)) {
        return NTD_ERR_NOINFO;
    }
    strncpy(pinit, ntgInfNetworkInfo->comm_info.initAT,
            sizeof(ntgInfNetworkInfo->comm_info.initAT));
    return NTD_OK;
}

Sint32 ntInfGetCIDStruct(Sint32 isp, Sint32 account, NetworkCIDStruct *pcidstr)
{
    Sint32 result;
    Uint32 mask;
    Sint32 current;
    Sint32 number;
    NetworkAccessInfo2 *ext;

    /* @hack spelling: current, a copy of isp that a call rewrites, where
     * &isp would do */
    current = isp;
    /* @hack spelling: number, a copy of account that a call rewrites, where
     * &account would do */
    number = account;
    if (account < INF_ACCOUNT_IN_USE || account > INF_ACCOUNT_MAIL4) {
        return NTD_ERR_INVALIDPARAM;
    }
    result = inf_check_isp(isp);
    if (result != NTD_OK) {
        return result;
    }
    _ntInfSetCurrentIspNumber(&current);
    result = _ntInfSetCurrentAccountNumber(current, &number);
    if (result != NTD_OK) {
        return result;
    }
    /* @bug logic: tests NTINF_COMM_DP3 without first testing the common
     * record's magic, as the summer-time getters do (MOV.L at 0x8c0bef80).
     * flash that never held the record reads as 0xff bytes, which set the
     * bit. no word in any image holds this function's address */
    if (!(ntgInfNetworkInfo->comm_info.flag & NTINF_COMM_DP3) ||
        current != INF_ISP1) {
        return NTD_ERR_NOINFO;
    }
    switch (number) {
    case INF_ACCOUNT_OWN:
        mask = NA2_CID_M0;
        break;
    case INF_ACCOUNT_MAIL1:
        mask = NA2_CID_M1;
        break;
    case INF_ACCOUNT_MAIL2:
        mask = NA2_CID_M2;
        break;
    case INF_ACCOUNT_MAIL3:
        mask = NA2_CID_M3;
        break;
    case INF_ACCOUNT_MAIL4:
        mask = NA2_CID_M4;
        break;
    default:
        return NTD_ERR_NOINFO;
    }
    /* @hack spelling: ext, a local for ISP 1's extra record, where the
     * record in place would do */
    ext = &ntgInfNetworkInfo->ext_info1;
    /* @bug logic: tests the account's NA2_CID_M bit even where ISP 1's extra
     * record is invalid (MOV.L at 0x8c0beff0). flash that never held the
     * record reads as 0xff bytes, which set the bit */
    if (!(ext->flag & mask)) {
        return NTD_ERR_NOINFO;
    }
    memcpy(pcidstr, &ntgInfNetworkInfo->cid_info.isp1[number - INF_ACCOUNT_OWN],
           sizeof(NetworkCIDStruct));
    return NTD_OK;
}

Sint32 ntInfGetCommunicationID(Sint32 isp, Sint32 account, char *pId)
{
    Sint32 result;
    Uint32 mask;
    Sint32 current;
    Sint32 number;
    NetworkAccessInfo2 *ext;

    /* @hack spelling: current, a copy of isp that a call rewrites, where
     * &isp would do */
    current = isp;
    /* @hack spelling: number, a copy of account that a call rewrites, where
     * &account would do */
    number = account;
    if (account < INF_ACCOUNT_IN_USE || account > INF_ACCOUNT_MAIL4) {
        return NTD_ERR_INVALIDPARAM;
    }
    result = inf_check_isp(isp);
    if (result != NTD_OK) {
        return result;
    }
    _ntInfSetCurrentIspNumber(&current);
    result = _ntInfSetCurrentAccountNumber(current, &number);
    if (result != NTD_OK) {
        return result;
    }
    /* @bug logic: tests NTINF_COMM_DP3 without testing the common record's
     * magic, as ntInfGetCIDStruct does (MOV.L at 0x8c0bf07a). no word in any
     * image holds this function's address */
    if (!(ntgInfNetworkInfo->comm_info.flag & NTINF_COMM_DP3) ||
        current != INF_ISP1) {
        return NTD_ERR_NOINFO;
    }
    switch (number) {
    case INF_ACCOUNT_OWN:
        mask = NA2_CID_M0;
        break;
    case INF_ACCOUNT_MAIL1:
        mask = NA2_CID_M1;
        break;
    case INF_ACCOUNT_MAIL2:
        mask = NA2_CID_M2;
        break;
    case INF_ACCOUNT_MAIL3:
        mask = NA2_CID_M3;
        break;
    case INF_ACCOUNT_MAIL4:
        mask = NA2_CID_M4;
        break;
    default:
        return NTD_ERR_NOINFO;
    }
    /* @hack spelling: ext, a local for ISP 1's extra record, where the
     * record in place would do */
    ext = &ntgInfNetworkInfo->ext_info1;
    /* @bug logic: tests the account's NA2_CID_M bit even where ISP 1's extra
     * record is invalid, as ntInfGetCIDStruct does (MOV.L at 0x8c0bf0be) */
    if (!(ext->flag & mask)) {
        return NTD_ERR_NOINFO;
    }
    memcpy(pId,
           ntgInfNetworkInfo->cid_info.isp1[number - INF_ACCOUNT_OWN]
               .communicationID,
           sizeof(ntgInfNetworkInfo->cid_info.isp1[0].communicationID));
    return NTD_OK;
}

Sint32 ntInfGetCommonFlag(Uint32 *pFlag)
{
    Sint32 result;

    result = inf_check_connection(INF_CURRENT);
    if (result != NTD_OK) {
        return NTD_ERR_NOINFO;
    }
    result = inf_check_common();
    if (result != NTD_OK) {
        return NTD_ERR_NOINFO;
    }
    *pFlag = ntgInfNetworkInfo->comm_info.flag;
    return NTD_OK;
}

Sint32 ntInfGetNetworkCommonInfo(NetworkCommonInfo **ppCommInfo)
{
    Sint32 result;

    result = inf_check_connection(INF_CURRENT);
    if (result != NTD_OK) {
        return NTD_ERR_NOINFO;
    }
    *ppCommInfo = &ntgInfNetworkInfo->comm_info;
    return NTD_OK;
}

static Sint32 inf_strnlen(char *text, Sint32 limit)
{
    Sint32 length;
    Sint32 i;

    length = 0;
    /* @hack spelling: length, a counter stepped with i, where i alone would
     * do */
    for (i = 0; i < limit; i++) {
        if (*text++ == '\0') {
            break;
        }
        length++;
    }
    return length;
}

/* the first c in text's first limit characters, up to its null, or NULL
 * where none is */
static char *inf_strnchr(char *text, char c, Sint32 limit)
{
    Sint32 i;

    for (i = 0; i < limit; i++) {
        if (*text == '\0') {
            break;
        }
        /* @hack spelling: text stepped in its test and stepped back for the
         * result, where a step after the test would do */
        if (*text++ == c) {
            return text - 1;
        }
    }
    return NULL;
}

void ntInfSplitTelNumber(Uint8 *pintel, Uint8 *poutextension, Uint8 *poutprefix,
                         Uint8 *pouttel)
{
    char *tel;
    char *mark;
    Sint32 rest;

    tel = (char *)pintel;
    rest = inf_strnlen(tel, INF_TEL_BYTES);
    mark = inf_strnchr(tel, ',', rest);
    if (mark != NULL) {
        /* @hack type: the addresses subtracted as integers for the length,
         * where the pointers' difference would do */
        strncpy((char *)poutextension, tel, (Uint32)mark - (Uint32)tel);
        rest -= mark - tel + 1;
        tel = mark + 1;
    }
    mark = inf_strnchr(tel, ' ', rest);
    if (mark != NULL) {
        /* @hack type: the addresses subtracted as integers for the length,
         * where the pointers' difference would do */
        strncpy((char *)poutprefix, tel, (Uint32)mark - (Uint32)tel);
        rest -= mark - tel + 1;
        tel = mark + 1;
    }
    strncpy((char *)pouttel, tel, rest);
}
