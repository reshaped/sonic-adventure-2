/* the Katana modem library, one face over the G2 modem, the SCIF and a
 * UART. mdmOpen returns the port's handle, which the other calls take */
#ifndef MDM_API_H
#define MDM_API_H
#include <sg_xpt.h>

typedef Uint32 MDM;
typedef Uint32 MD_DEVICE;
typedef struct {
    char Dtr, Rts, CharLen, Parity, StopBit, SetBreak, LineSpeed;
    char Cts, Dsr, Ri, Dcd, Brk;
} MDS_DCB;

enum {
    MDD_MCR_DTR_OFF = 0,
    MDD_MCR_DTR_ON = 1,
    MDD_MCR_RTS_OFF = 0,
    MDD_MCR_RTS_ON = 2,
    MDD_LCR_CHARLEN_7 = 2,
    MDD_LCR_CHARLEN_8 = 3,
    MDD_LCR_PARITY_NONE = 0,
    MDD_LCR_PARITY_ODD = 8,
    MDD_LCR_PARITY_EVEN = 24,
    MDD_LCR_STOPBIT_1B = 0,
    MDD_LCR_STOPBIT_2B = 4
};
enum {
    MDD_LINESPEED_300 = 1,
    MDD_LINESPEED_1200 = 2,
    MDD_LINESPEED_2400 = 3,
    MDD_LINESPEED_4800 = 4,
    MDD_LINESPEED_9600 = 6,
    MDD_LINESPEED_14400 = 8,
    MDD_LINESPEED_19200 = 10,
    MDD_LINESPEED_38400 = 27,
    MDD_LINESPEED_57600 = 46,
    MDD_LINESPEED_115200 = 51,
    MDD_LINESPEED_230400 = 53,
    MDD_LINESPEED_460800 = 54
};
enum { MDD_DEVICE_SCIF, MDD_DEVICE_G2_MODEM, MDD_DEVICE_UART };
extern const Uint16 mdgMdmAPIVer[2];
extern const char *mdgMdmAPIBuild;
void mdmDropDtr(MDM handle);
void mdmEnableDtr(MDM handle);
int mdmRxAvailSize(MDM handle);
int mdmRxFreeSize(MDM handle);
int mdmTxAvailSize(MDM handle);
/* the free bytes of handle's transmit ring, under the name the SDK spells
 * Szie */
int mdmTxFreeSzie(MDM handle);
void mdmInit(char *tx, char *rx, int tx_size, int rx_size);
MDM mdmOpen(MD_DEVICE device);
int mdmRead(MDM handle, char *buffer, int size);
int mdmWrite(MDM handle, char *buffer, int size);
void mdmSetCommState(MDS_DCB state);
void mdmSetCommState2(MDM handle, MDS_DCB state);
void mdmGetCommState(MDS_DCB *state);
void mdmGetCommState2(MDM handle, MDS_DCB *state);
void mdmPurgeTxBuffer(MDM handle);
void mdmPurgeRxBuffer(MDM handle);
void mdmClose(MDM handle);
void mdmFinish(void);
void mdmServicePort(MDM handle);
void mdmSelectDevice(MD_DEVICE device);
#endif
