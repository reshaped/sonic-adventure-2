/* ACCESS's PPP: the records AvepppOpen takes, the connection's phases and
 * errors, and the session's calls */
#ifndef ACCESS_AVEPPP_H
#define ACCESS_AVEPPP_H
typedef struct AvepppSerialParam {
    short speed, stop, parity, databit, flow;
    long reserved_1, reserved_2;
} AvepppSerialParam;
typedef struct AvepppTelephoneParam {
    short dialtype, outside_line, timeout, dial_retry, dial_interval;
    char *outside_number;
    long reserved_1, reserved_2;
} AvepppTelephoneParam;
typedef struct AvepppConnectionParam {
    unsigned long his_ip, my_ip;
    short recognize, mru;
    long magic_number;
    short acfcomp, protocomp, vjcomp;
    long reserved_1, reserved_2;
} AvepppConnectionParam;
typedef struct AvepppOpenParam {
    AvepppSerialParam *serial;
    AvepppTelephoneParam *telephone;
    AvepppConnectionParam *pppconection;
    char *login_script1, *login_script2, *telephone_number, *login, *password,
        *signup;
    long reserved_1, reserved_2;
} AvepppOpenParam;
enum {
    AvepppConnectUnused,
    AvepppConnectOpenStandby,
    AvepppConnectDialing,
    AvepppConnectAuthen,
    AvepppConnectEstablished,
    AvepppConnectDisconnected,
    AvepppConnectCloseStandby,
    AvepppConnectOnhooking,
    AvepppConnectFail
};
enum {
    AvepppErrorNon,
    AvepppErrorModem,
    AvepppErrorBusy,
    AvepppErrorNoDialTone,
    AvepppErrorScript,
    AvepppErrorLcp,
    AvepppErrorAuth,
    AvepppErrorIpcp,
    AvepppErrorTcp
};
typedef struct AvepppStatus {
    unsigned long his_ip, my_ip;
    short phase;
    long baud_rate;
    short port_inuse, last_error;
} AvepppStatus;
short AvepppInitialize2(char *tx, char *rx, int tx_size, int rx_size);
short AvepppInitialize(void);
void AvepppSetDisconnectScript(char *script);
void AvepppTerminate(void);
short AvepppOpen(const AvepppOpenParam *param);
short AvepppClose(short handle);
short AvepppGetStatus(AvepppStatus *status);
void AvepppSetDns(long myip, long hisip, long mydns1, long mydns2, long hisdns1,
                  long hisdns2);
void AvepppGetDns(long *myip, long *hisip, long *mydns1, long *mydns2,
                  long *hisdns1, long *hisdns2);
#endif
