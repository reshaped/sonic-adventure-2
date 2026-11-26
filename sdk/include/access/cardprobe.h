/* ACCESS's network card. AT_CardProbe asks syExtChkConnect for a LAN adapter.
 * it sets AT_CardType to AT_CARD_TYPE_LAN where it finds one, or else
 * AT_CARD_TYPE_PPP, and returns it. AT_GetCardType returns AT_CardType, which
 * is AT_CARD_TYPE_PPP before any probe */
#ifndef ACCESS_CARDPROBE_H
#define ACCESS_CARDPROBE_H
#define AT_CARD_TYPE_PPP 1
#define AT_CARD_TYPE_LAN 2
#define AT_CARD_TYPE_UNKNOWN (-1)

extern int AT_CardType;
int AT_CardProbe(void);
int AT_GetCardType(void);
#endif
