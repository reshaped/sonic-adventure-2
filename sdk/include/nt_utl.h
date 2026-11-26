/* Network SDK utilities, as far as the sources use them.
 *
 * the ntUtl calls keep a flag, a jump number and three buffers of 64 bytes
 * in a record. the record lies at a fixed address below the program. the
 * record survives a jump to another program and back. every ntUtl call but
 * ntUtlExit first starts the record where it isn't started, as ntUtlInit
 * does. a call that returns a result returns NTD_OK unless its comment says
 * otherwise.
 *
 * the ntInf getters read the network information that ntInfInit read. a
 * getter's isp names a connection: 1 or 2 for an ISP, 3 for the LAN, and 0
 * for the one the information's flag selects. the flag selects the LAN
 * where it has both VERSION_2 and USE_LAN, if the getter takes isp 3.
 * otherwise it selects ISP 2 where it has ISP_USE2, and ISP 1 where it
 * doesn't.
 *
 * a getter returns NTD_OK unless its comment says otherwise. before
 * ntInfInit it returns NTD_ERR_NOTINIT. an isp it doesn't take gives
 * NTD_ERR_INVALIDPARAM. where the information lacks what the getter asks
 * for, it returns NTD_ERR_NOINFO. a getter writes its outputs only where it
 * returns NTD_OK. a string getter copies a count of bytes as strncpy does.
 * where the string fills the count, the copy has no null */
#ifndef NT_UTL_H
#define NT_UTL_H
#include <sg_xpt.h>

/* the utilities' results. NTD_ERR_INVALIDPARAM means an argument is out of
 * range. ntInfInit also returns it where an allocation fails. NTD_ERR_NOINFO
 * means the record a call reads lacks what the call asks for */
#define NTD_OK 0
#define NTD_ERR_NOTINIT (-1)
#define NTD_ERR_INVALIDPARAM (-2)
#define NTD_ERR_NOINFO (-3)

/* the magic of an access, extra, LAN or common record: the bytes "SEGA",
 * read as a little-endian word. a record is valid where it holds the magic.
 * a valid extra or LAN record also needs VERSION_2 in the information's
 * flag. a valid extra record also needs USE_EXTRA_INFO in its ISP's access
 * record */
#define NETINFO_MAGIC 0x41474553

/* an IPv4 address */
struct in_addr {
    union {
        struct {
            unsigned char s_b1, s_b2, s_b3, s_b4;
        } S_un_b;
        struct {
            unsigned short s_w1, s_w2;
        } S_un_w;
        unsigned long S_addr;
    } S_un;
#define s_addr S_un.S_addr
};

/* an ISP's access record */
typedef struct _NetworkAccessInfo {
    Uint32 magic;
    char loginId[28];
    char loginPasswd[16];
    char accessPointNumber[3][40];
    struct in_addr dnsServerAddress1;
    struct in_addr dnsServerAddress2;
    char mailAddress[48];
    char mailServer[32];
    char popServer[32];
    char popId[16];
    char popPasswd[16];
    char proxyServer[32];
    Uint16 proxyPortNum;
    Uint16 flag;
} NetworkAccessInfo;

/* a mail account. an ISP numbers its accounts 0 to 5. account 1 is the one
 * in its access record. accounts 2 to 5 are the four in its extra record.
 * account 0 stands for the first of 2 to 5 in use, or for 1 where none is.
 * where the ISP has no valid extra record, every number stands for 1.
 * another number gives NTD_ERR_INVALIDPARAM */
typedef struct _MailAccount {
    char mailAddress[48];
    char mailServer[32];
    char popServer[32];
    char popId[16];
    char popPasswd[16];
} MailAccount;

/* an ISP's extra record: its IP address, the parental password and four
 * more mail accounts */
typedef struct _NetworkAccessInfo2 {
    Uint32 magic;
    Uint32 flag;
    struct in_addr ipAddr;
    char parentalPasswd[12];
    MailAccount mailAccount[4];
} NetworkAccessInfo2;

/* the LAN's record */
typedef struct _TCPIPInfo {
    Uint32 magic;
    Uint32 flag;
    struct in_addr ipAddr;
    struct in_addr subnetMask;
    struct in_addr broadcast;
    struct in_addr dnsServerAddress1;
    struct in_addr dnsServerAddress2;
    struct in_addr gateway1;
    struct in_addr gateway2;
    char host[32];
    char domain[64];
    char mailAddress[140];
    char mailServer[64];
    char popServer[64];
    char popId[32];
    char popPasswd[32];
    char proxyServer[64];
    Uint16 proxyPortNum;
    Uint16 reserved_short;
    char unk_214[8];
    char loginId[40];
    char loginPasswd[16];
    Uint32 reserved;
} TCPIPInfo;

/* when summer time starts or ends: the month, the week of it, the day of
 * the week, and the hour */
typedef struct _NetworkSummerTime {
    Uint8 mon;
    Uint8 mweek;
    Uint8 dweek;
    Uint8 hour;
} NetworkSummerTime;

/* the common record: the settings every connection shares */
typedef struct _NetworkCommonInfo {
    Uint32 magic;
    Uint32 flag;
    char url[52];
    NetworkSummerTime stStart;
    NetworkSummerTime stEnd;
    Sint32 stDiff;
    char initAT[48];
} NetworkCommonInfo;

/* a call id: its communication id and password */
typedef struct _NetworkCIDStruct {
    char communicationID[16];
    char password[8];
} NetworkCIDStruct;

/* ISP 1's call ids, one for each of its accounts */
typedef struct _NetworkCIDInfo {
    NetworkCIDStruct isp1[5];
} NetworkCIDInfo;

/* NetworkInfo3's first fields alone: the information as ntInfGetNetworkInfo
 * gives it */
typedef struct _NetworkInfo {
    Uint32 flag;
    NetworkAccessInfo isp_info1;
    NetworkAccessInfo isp_info2;
    Uint32 reserve;
} NetworkInfo;

/* the web browser's settings */
typedef struct _PlanetWebInfo {
    char signature[10];
    Uint8 version;
    Sint8 dialArea;
    char outLine[16];
    char mailAddress2[16];
    char callWait[16];
    char mailAddress3[16];
    char realName[32];
    char pcntrl[42];
    char modemInit[32];
    char areaCode[32];
    char longDst[32];
    Sint8 useProxy;
    Sint8 mailHdr;
    Sint8 titlebar;
    Sint8 toolbar;
    Sint8 style;
    Sint8 qwertykeyb;
    Sint8 sticky;
    Sint8 quote;
    Sint8 sndEffects;
    Sint8 japanese;
    Sint8 showTime;
    Sint8 timezone;
    Sint8 daylightSavings;
    Sint8 hideKeyb;
    Sint8 ssvroff;
    Sint8 showmodem;
    Sint8 ccself;
    Sint8 anchorDisplay;
    Sint8 urlbar;
    Sint8 padding[25];
} PlanetWebInfo;

/* the network information, as the flash holds it */
typedef struct _NetworkInfo3 {
    Uint32 flag;
    NetworkAccessInfo isp_info1;
    NetworkAccessInfo isp_info2;
    Uint32 reserve;
    NetworkAccessInfo2 ext_info1;
    NetworkAccessInfo2 ext_info2;
    TCPIPInfo lan_info;
    NetworkCommonInfo comm_info;
    NetworkCIDInfo cid_info;
    PlanetWebInfo pw_info;
} NetworkInfo3;

/* in the information's flag: the bits that select the connection, and the
 * information's version */
#define ISP_USE2 0x00000001
#define USE_LAN 0x00000004
#define VERSION_2 0x01000000
/* in an access record's flag: the ISP has an extra record */
#define USE_EXTRA_INFO 0x0001
/* in an extra record's flag: accounts 2 to 5 are in use */
#define NA2_USE_MAIL1 0x00000001
#define NA2_USE_MAIL2 0x00000002
#define NA2_USE_MAIL3 0x00000004
#define NA2_USE_MAIL4 0x00000008
/* in ISP 1's extra record's flag: the account has a call id. NA2_CID_M0 is
 * for account 1, and NA2_CID_M1 to NA2_CID_M4 are for accounts 2 to 5 */
#define NA2_CID_M0 0x00000100
#define NA2_CID_M1 0x00000200
#define NA2_CID_M2 0x00000400
#define NA2_CID_M3 0x00000800
#define NA2_CID_M4 0x00001000
/* in the common record's flag: summer time is on, and the modem is an
 * external one */
#define NTINF_COMM_SUMMERTIME 0x00000001
#define NTINF_COMM_EXTERNAL_MODEM 0x00000002
/* in the common record's flag. without it, the getters of summer time, of
 * the modem and of call ids return NTD_ERR_NOINFO */
#define NTINF_COMM_DP3 0x80000000
/* the common record's flag names the connection for mail under
 * NTINF_COMM_MAIL_MASK. NTINF_COMM_MAIL_AUTO, or a value no constant here
 * names, takes the connection the information selects */
#define NTINF_COMM_MAIL_AUTO 0x00000000
#define NTINF_COMM_MAIL_ISP1 0x00010000
#define NTINF_COMM_MAIL_ISP2 0x00020000
#define NTINF_COMM_MAIL_LAN 0x00030000
#define NTINF_COMM_MAIL_MASK 0x00070000

/* the bits of the flag the utilities keep. NTUTL_JUMP, NTUTL_FIX, NTUTL_UP
 * and NTUTL_DOWN each mean that their data is kept. an _OK bit allows an
 * upload or a download, and a _DO bit wants one. NTUTL_VIRTUALVM means the
 * memory card is a virtual one */
#define NTUTL_JUMP 0x00000001
#define NTUTL_FIX 0x00000002
#define NTUTL_UP 0x00000004
#define NTUTL_UP_OK 0x00000008
#define NTUTL_UP_DO 0x00000010
#define NTUTL_DOWN 0x00000020
#define NTUTL_DOWN_OK 0x00000040
#define NTUTL_DOWN_DO 0x00000080
#define NTUTL_VIRTUALVM 0x00000100
/* the flag's version, and the flag a record starts with */
#define NTUTL_VER1 0x00000000
#define NTUTL_FLAG_INIT (NTUTL_VER1 | NTUTL_DOWN_OK)

/* start the record where it isn't started */
void ntUtlInit(void);
/* do nothing: the record keeps its contents */
void ntUtlExit(void);
/* set the record's flag to flag, or store the record's flag in *flag */
Sint16 ntUtlSetFlag(Uint32 flag);
Sint16 ntUtlGetFlag(Uint32 *flag);
/* keep number as the jump number */
Sint16 ntUtlSetJumpNumber(Sint32 number);
/* store the kept jump number in *number. it returns NTD_ERR_NOINFO where none
 * is kept */
Sint16 ntUtlGetJumpNumber(Sint32 *number);
/* keep the size bytes at fixdata as the fixed data. size isn't checked, and
 * must be 64 or less */
Sint16 ntUtlSetFix(Uint8 *fixdata, Sint32 size);
/* copy the fixed data into fixdata, no more than *size bytes, and set *size
 * to what it copied. where none is kept it sets *size to 0 and returns
 * NTD_ERR_NOINFO */
Sint16 ntUtlGetFix(Uint8 *fixdata, Sint32 *size);
/* keep the size bytes at updata as the data to upload, and allow an upload.
 * size is bound as ntUtlSetFix's is */
Sint16 ntUtlSetUpload(Uint8 *updata, Sint32 size);
/* copy the data to upload into updata, as ntUtlGetFix copies the fixed
 * data */
Sint16 ntUtlGetUpload(Uint8 *updata, Sint32 *size);
/* keep the size bytes at downdata as the data to download. size is bound as
 * ntUtlSetFix's is */
Sint16 ntUtlSetDownload(Uint8 *downdata, Sint32 size);
/* copy the data to download into downdata, as ntUtlGetFix copies the fixed
 * data */
Sint16 ntUtlGetDownload(Uint8 *downdata, Sint32 *size);
/* NTD_OK where NTUTL_DOWN_DO wants a download, and NTD_ERR_NOINFO where it
 * doesn't */
Sint32 ntUtlIsDownload(void);
/* NTD_OK where NTUTL_UP_DO wants an upload, and NTD_ERR_NOINFO where it
 * doesn't */
Sint32 ntUtlIsUpload(void);
/* NTD_OK where NTUTL_VIRTUALVM is set, and NTD_ERR_NOINFO where it isn't */
Sint32 ntUtlIsVirtualVM(void);
/* set NTUTL_DOWN_OK where flag isn't 0, and clear it where it is */
Sint16 ntUtlSetDownFlag(Bool flag);
/* set NTUTL_UP_OK where flag isn't 0, and clear it where it is */
Sint16 ntUtlSetUpFlag(Bool flag);
/* set NTUTL_VIRTUALVM where flag isn't 0, and clear it where it is */
Sint16 ntUtlSetVirtualVM(Bool flag);

/* start the flash manager on bank 2, and read the network information into
 * pNetInfo. the read fills every field before pw_info. a null pNetInfo has
 * the call allocate the record, without room for pw_info. pWork is the
 * flash manager's workspace. a null pWork has the call allocate 0x4000
 * bytes for it.
 *
 * the call returns NTD_OK, and returns it at once where it has a record
 * already. it returns NTD_ERR_INVALIDPARAM where an allocation fails, and
 * NTD_ERR_NOTINIT where the flash won't start or read.
 *
 * what the call sets stays set until ntInfExit, after a failure too. a
 * failed read leaves the record set, so the next call returns NTD_OK over
 * what was read. after a failure before the record is set, the next call
 * sets a new workspace. a workspace the first call allocated is then lost */
Sint32 ntInfInit(NetworkInfo3 *pNetInfo, void *pWork);
/* stop the flash manager and free what ntInfInit allocated. returns NTD_OK */
Sint32 ntInfExit(void);
/* NTD_OK where ISP 1's access record holds its magic. the LAN's magic also
 * counts where the information's flag has VERSION_2 or USE_LAN.
 * NTD_ERR_NOINFO where neither counts, and NTD_ERR_NOTINIT where ntInfInit
 * sets no record.
 * the call runs ntInfInit, then ntInfExit, which also ends an earlier
 * ntInfInit. a getter after this call needs ntInfInit again */
Sint32 ntInfIsNetworkInfo(void);

/* point *ppNetInfo at the information, read as a NetworkInfo. it needs a
 * valid access record for ISP 1 */
Sint32 ntInfGetNetworkInfo(NetworkInfo **ppNetInfo);
/* point *ppNetInfo at the information */
Sint32 ntInfGetNetworkInfo3(NetworkInfo3 **ppNetInfo);
/* point *ppNetAccInfo at the access record of isp, 0 to 2 */
Sint32 ntInfGetNetworkAccessInfo(Sint32 isp, NetworkAccessInfo **ppNetAccInfo);
/* store the information's flag in *fFlag */
Sint32 ntInfGetFlag(Uint32 *fFlag);
/* copy the login id of isp, 0 to 3, into pLoginId. the copy is 28 bytes for
 * an ISP, and 40 for the LAN */
Sint32 ntInfGetLoginId(Sint32 isp, char *pLoginId);
/* copy the login password of isp, 0 to 3, into pLoginPasswd: 16 bytes */
Sint32 ntInfGetLoginPasswd(Sint32 isp, char *pLoginPasswd);
/* copy the three access point numbers of isp, 0 to 2, into
 * pAccessPointNumber: 40 bytes each */
Sint32 ntInfGetAccessPointNumber(Sint32 isp, char *pAccessPointNumber);
/* store the two DNS server addresses of isp, 0 to 3, in *pDns1 and *pDns2 */
Sint32 ntInfGetDnsServerAddress(Sint32 isp, struct in_addr *pDns1,
                                struct in_addr *pDns2);
/* copy an account's mail address into pMailAddress, no more than size
 * bytes. isp is 0 to 3. isp 0 takes the connection for mail that a valid
 * common record names. without one, 0 takes the connection the information
 * selects. an ISP's account is numbered as MailAccount says. the LAN has
 * one account, and the call ignores account for it */
Sint32 ntInfGetMailAddress(Sint32 isp, Sint32 account, char *pMailAddress,
                           Sint32 size);
/* copy an account's mail server, POP server, POP id or POP password, as
 * ntInfGetMailAddress copies its mail address */
Sint32 ntInfGetMailServer(Sint32 isp, Sint32 account, char *pMailServer,
                          Sint32 size);
Sint32 ntInfGetPopServer(Sint32 isp, Sint32 account, char *pPopServer,
                         Sint32 size);
Sint32 ntInfGetPopId(Sint32 isp, Sint32 account, char *pPopId, Sint32 size);
Sint32 ntInfGetPopPasswd(Sint32 isp, Sint32 account, char *pPopPasswd,
                         Sint32 size);
/* copy the proxy server of isp, 0 to 3, into pProxyServer, no more than size
 * bytes */
Sint32 ntInfGetProxyServer(Sint32 isp, char *pProxyServer, Sint32 size);
/* store the proxy port of isp, 0 to 3, in *pPort */
Sint32 ntInfGetProxyPortNum(Sint32 isp, Uint16 *pPort);
/* store the flag of isp's access record, isp 0 to 2, in *pFlag */
Sint32 ntInfGetNetworkAccessInfoFlag(Sint32 isp, Uint16 *pFlag);
/* point *ppNetAccInfo2 at the extra record of isp, 0 to 2. it needs a valid
 * extra record */
Sint32 ntInfGetNetworkAccessInfo2(Sint32 isp,
                                  NetworkAccessInfo2 **ppNetAccInfo2);
/* point *ppTCPIPInfo at the LAN's record. any failure gives NTD_ERR_NOINFO */
Sint32 ntInfGetTCPIPInfo(TCPIPInfo **ppTCPIPInfo);
/* store the flag of isp's extra record, isp 0 to 2, in *pFlag. it needs a
 * valid extra record */
Sint32 ntInfGetNetworkAccessInfo2Flag(Sint32 isp, Uint32 *pFlag);
/* store the IP address of isp, 0 to 3, in *IPAddr. an ISP's is in its extra
 * record, which must be valid */
Sint32 ntInfGetIPAddr(Sint32 isp, struct in_addr *IPAddr);
/* copy the parental password of isp, 0 to 2, into pPasswd: 12 bytes. it
 * needs a valid extra record */
Sint32 ntInfGetParentalPasswd(Sint32 isp, char *pPasswd);
/* point *ppMailAcct at an account of isp, 0 to 2, numbered as MailAccount
 * says. account 1 points into the access record, whose mailAddress to
 * popPasswd lie as a MailAccount's fields do. an account above 5 gives
 * NTD_ERR_INVALIDPARAM even before ntInfInit */
Sint32 ntInfGetMailAccount(Sint32 isp, Uint32 account,
                           MailAccount **ppMailAcct);
/* store or copy what the call names from the LAN's record. ntInfGetHost
 * copies 32 bytes, and ntInfGetDomain copies 64. each call needs a valid
 * LAN record, and gives NTD_ERR_NOINFO for any failure */
Sint32 ntInfGetTCPIPInfoFlag(Uint32 *pFlag);
Sint32 ntInfGetSubnetMask(struct in_addr *pSubnetMask);
Sint32 ntInfGetBroadcast(struct in_addr *pBroadcast);
Sint32 ntInfGetGateway(struct in_addr *pGw1, struct in_addr *pGw2);
Sint32 ntInfGetHost(char *pHost);
Sint32 ntInfGetDomain(char *pDomain);
/* store in *isp the connection the information selects: 1, 2 or 3 */
Sint32 ntInfGetISP(Sint32 *isp);
/* copy the common record's URL into pURL: 52 bytes. it needs a valid
 * common record */
Sint32 ntInfGetURL(char *pURL);
/* store when summer time starts, when it ends, or its difference. each call
 * needs a valid common record with NTINF_COMM_DP3 */
Sint32 ntInfGetSummerTimeStart(NetworkSummerTime *pstStart);
Sint32 ntInfGetSummerTimeEnd(NetworkSummerTime *pstEnd);
Sint32 ntInfGetSummerTimeDiff(Sint32 *pdiff);
/* NTD_OK where a valid common record has NTINF_COMM_DP3 and
 * NTINF_COMM_SUMMERTIME. where it lacks either, NTD_ERR_NOINFO */
Sint32 ntInfSummerTime(void);
/* NTD_OK where a valid common record has NTINF_COMM_DP3 and
 * NTINF_COMM_EXTERNAL_MODEM. where it lacks either, NTD_ERR_NOINFO */
Sint32 ntInfExternalModem(void);
/* copy the modem's init string into pinit: 48 bytes. it needs a valid
 * common record with NTINF_COMM_DP3 */
Sint32 ntInfGetModemInit(char *pinit);
/* store in *pcidstr the call id of an account of ISP 1. isp is 0 to 2, and
 * one that doesn't select ISP 1 gives NTD_ERR_NOINFO. the call also needs
 * NTINF_COMM_DP3 and the account's NA2_CID_M bit. an account outside 0 to 5
 * gives NTD_ERR_INVALIDPARAM even before ntInfInit */
Sint32 ntInfGetCIDStruct(Sint32 isp, Sint32 account, NetworkCIDStruct *pcidstr);
/* copy into pId the 16-byte communication id of an account's call id. the
 * call finds the call id as ntInfGetCIDStruct does */
Sint32 ntInfGetCommunicationID(Sint32 isp, Sint32 account, char *pId);
/* store the common record's flag in *pFlag. any failure gives
 * NTD_ERR_NOINFO */
Sint32 ntInfGetCommonFlag(Uint32 *pFlag);
/* point *ppCommInfo at the common record, whether its magic is set or not.
 * any failure gives NTD_ERR_NOINFO */
Sint32 ntInfGetNetworkCommonInfo(NetworkCommonInfo **ppCommInfo);
/* split pintel, a phone number of up to 40 bytes, into its parts. the
 * extension is the text before the first comma. the prefix is the text
 * before the next space. the number is the rest. without the comma,
 * poutextension is left as it was. without the space, poutprefix is left as it
 * was. no part is copied with a null */
void ntInfSplitTelNumber(Uint8 *pintel, Uint8 *poutextension, Uint8 *poutprefix,
                         Uint8 *pouttel);

/* leave the program for the Dream Passport browser's loader on the disc. the
 * call doesn't return: where the loader won't open or read, it leaves through
 * syBtExit instead */
void ntLDBrowser(void);

#endif
