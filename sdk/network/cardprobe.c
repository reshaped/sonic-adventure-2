#include <access/cardprobe.h>
#include <sg_xpt.h>
#include <shinobi.h>

int AT_CardType = AT_CARD_TYPE_PPP;

int AT_CardProbe(void)
{
    Sint32 result = syExtChkConnect(SYD_EXT_CHK_TYPE_LAN);
    if (result == 0) {
        AT_CardType = AT_CARD_TYPE_LAN;
    } else {
        AT_CardType = AT_CARD_TYPE_PPP;
    }
    return AT_CardType;
}
int AT_GetCardType(void)
{
    return AT_CardType;
}
