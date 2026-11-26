/* Shinobi's address-check handler, which dumps the registers it saved over
 * the SH-4's SCIF or SCI. syIntAdrChk_init picks the channel by the SCK
 * input, and returns it */
#ifndef SY_INT_CHK_H
#define SY_INT_CHK_H
#include <sg_xpt.h>

/* the serial module the dump goes over */
enum serial_channel { CHANNEL_SCIF, CHANNEL_SCI };

enum serial_channel syIntAdrChk_init(void);
void syIntAdrChk_putchar(char character);
char syIntAdrChk_getchar(void);
void syIntAdrChk_finish(void);
void syIntAdrChk_prints(const char *text);
void syIntAdrChk_printh(Uint32 number);
void syIntChkDump(Uint16 event, Uint32 *frame);

#endif
