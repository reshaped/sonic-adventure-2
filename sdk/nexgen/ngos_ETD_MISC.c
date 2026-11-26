/* the adapter's setup and settings, and the G2 reads and writes of its
 * registers */
#include <sg_syg2.h>
#include <sg_xpt.h>
#include "../network/g2_io.h"
#include "../nexgen/ngos_ETD_DLC.h"
#include "../nexgen/rtl8139.h"
#include "../nexgen/dld_state.h"

#define PCI_COMMAND 0xa1001604
#define PCI_STATUS 0xa1001606
#define PCI_LATENCY 0xa100160d
#define PCI_MEMORY_BASE 0xa1001614
#define PCI_ROM_BASE 0xa1001630
#define PCI_INTERRUPT_LINE 0xa100163c
#define PCI_PM_CAPABILITY 0xa1001650
#define PCI_PM_CONTROL 0xa1001654
/* PCI_COMMAND's bits */
#define PCI_MEMORY_SPACE 2
#define PCI_BUS_MASTER 4
/* PCI_STATUS's error bits, which writing 1 clears */
#define PCI_STATUS_ERRORS 0xf900
#define PM_ID 1 /* the power management capability's ID */
/* the bits of PCI_PM_CONTROL that hold the power state, which is 0 in D0 */
#define PM_STATE 3
/* PCI_PM_CONTROL's event bit, which writing 1 clears */
#define PME_STATUS 0x8000
#define GAPS_START_WAIT 2000 /* microseconds */
#define EEPROM_RELOAD_POLLS 3000
#define RELOAD_POLL_WAIT 1 /* microseconds */

#define TX_CONFIG (RTL_TCR_IFG_STANDARD | RTL_TCR_DMA_32)
#define RX_CONFIG                                                              \
    (RTL_RCR_FIFO_64 | RTL_RCR_RING_16K | RTL_RCR_DMA_32 | RTL_RCR_NO_WRAP)
#define BMCR_FULL_DUPLEX 0x0100
#define BMCR_AUTONEGOTIATE 0x1000
#define BMCR_100 0x2000 /* 100 Mb/s rather than 10 */
#define MSR_LINK_FAIL 4
#define CONFIG1_LED0 0x40
#define CONFIG1_LED1 0x80
/* set, the card keeps its PHY powered while the link is down */
#define CONFIG5_LDPS 4
#define MULTICAST_ALL 0xffffffff /* every bit of MAR's hash */

Sint32 ETD_MadisonInit(void)
{
    Uint32 enable;
    ETD_G2Write32(GAPS_RESET, GAPS_ENABLE);
    ETD_DlcWait(GAPS_START_WAIT);
    /* @hack spelling: the call's result held in enable, where the test could
     * take the call */
    enable = ETD_G2Read32(GAPS_RESET);
    if (enable != GAPS_RUNNING) {
        return -1;
    }
    ETD_G2Write32(0xa1001420, 0x01000000);
    ETD_G2Write32(0xa1001424, 0x01000000);
    ETD_G2Write32(0xa1001428, RTL_RX_RING);
    ETD_G2Write32(0xa1001414, 1);
    ETD_G2Write32(0xa1001434, 1);
    return 0;
}

void ETD_PciInit(void)
{
    Uint8 capability;
    Uint16 command;
    Uint16 power_state;
    ETD_G2Write16(PCI_STATUS, PCI_STATUS_ERRORS);
    ETD_G2Write32(PCI_ROM_BASE, 0);
    ETD_G2Write8(PCI_INTERRUPT_LINE, 0);
    ETD_G2Write8(PCI_LATENCY, 0xf0);
    /* @hack spelling: the read held in command and changed there, where the
     * write could take the changed read */
    command = ETD_G2Read16(PCI_COMMAND);
    command |= PCI_MEMORY_SPACE | PCI_BUS_MASTER;
    ETD_G2Write16(PCI_COMMAND, command);
    ETD_G2Write32(PCI_MEMORY_BASE, 0x01000000);
    /* @hack spelling: the call's result held in capability, where the test
     * could take the call */
    capability = ETD_G2Read8(PCI_PM_CAPABILITY);
    if (capability & PM_ID) {
        /* @hack spelling: the read held in power_state and changed there,
         * where the write could take the changed read */
        power_state = ETD_G2Read16(PCI_PM_CONTROL);
        power_state &= ~PM_STATE;
        power_state |= PME_STATUS;
        ETD_G2Write16(PCI_PM_CONTROL, power_state);
    }
}

void ETD_DriverInit(void)
{
    int i;
    dl_adapter->rx_end = RTL_RX_RING_END;
    dl_adapter->tx_buffer[0] = RTL_TX_FIRST;
    dl_adapter->tx_buffer[1] = RTL_TX_FIRST + RTL_TX_BYTES;
    dl_adapter->tx_buffer[2] = RTL_TX_FIRST + (2 * RTL_TX_BYTES);
    dl_adapter->tx_buffer[3] = RTL_TX_FIRST + (3 * RTL_TX_BYTES);
    dl_receive_test = 0;
    for (i = 0; (unsigned)i < sizeof(union rtl_statistics) / sizeof(Uint32);
         i++) {
#ifdef NONMATCHING
        /* this side writes the plain form the hack tag below gives */
        dl_adapter->stats.words[i] = 0;
#else
        /* @hack type: the statistics written through a Uint32 pointer,
         * where stats.words[i] would do */
        ((Uint32 *)&dl_adapter->stats)[i] = 0;
#endif
    }
    dl_adapter->tx_config = TX_CONFIG;
    switch (dl_config->loopback) {
    case 1:
        dl_adapter->tx_config |= RTL_TCR_LOOPBACK(1);
        break;
    case 2:
        dl_adapter->tx_config |= RTL_TCR_LOOPBACK(2);
        break;
    case RTL_LOOPBACK_ON:
        dl_adapter->tx_config |= RTL_TCR_LOOPBACK(RTL_LOOPBACK_ON);
        break;
    case RTL_LOOPBACK_OFF:
        break;
    /* @hack redundant: an empty default beside RTL_LOOPBACK_OFF's case */
    default:
        break;
    }
    dl_adapter->rx_config = RX_CONFIG;
    switch (dl_config->receive_mode) {
    case RTL_RECEIVE_ERRORS:
        dl_adapter->rx_config |= RTL_RCR_ERRORS;
        /* fall through */
    case RTL_RECEIVE_ALL:
        dl_adapter->rx_config |= RTL_RCR_ALL;
        /* fall through */
    case RTL_RECEIVE_MULTICAST:
        dl_adapter->rx_config |= RTL_RCR_MULTICAST;
        /* fall through */
    case RTL_RECEIVE_BROADCAST:
        dl_adapter->rx_config |= RTL_RCR_BROADCAST | RTL_RCR_OWN;
        /* fall through */
    case RTL_RECEIVE_NONE:
        break;
    }
    switch (dl_config->media) {
    case RTL_MEDIA_AUTO:
        dl_adapter->bmcr = BMCR_AUTONEGOTIATE;
        break;
    case RTL_MEDIA_100_FULL:
        dl_adapter->bmcr = BMCR_100 | BMCR_FULL_DUPLEX;
        break;
    case RTL_MEDIA_100_HALF:
        dl_adapter->bmcr = BMCR_100;
        break;
    case RTL_MEDIA_10_FULL:
        dl_adapter->bmcr = BMCR_FULL_DUPLEX;
        break;
    case RTL_MEDIA_10_HALF:
    default:
        dl_adapter->bmcr = 0;
        break;
    }
}

Sint32 ETD_CardInit(void)
{
    int i;
    /* @hack spelling: CONFIG5's and CONFIG1's reads held in value, where
     * each write could take its read */
    Uint8 value;
    Uint8 reset;
    Uint8 command;
    ETD_G2Write8(RTL_CR, RTL_CR_RESET);
    dl_adapter->rx_position = RTL_RX_RING;
    dl_adapter->rx_read = RTL_RX_INITIAL_CAPR;
    dl_adapter->tx_pending = 0;
    dl_adapter->tx_head = 0;
    dl_adapter->tx_tail = 0;
    for (i = 0; i < RTL_RESET_POLLS; i++) {
        /* @hack spelling: the call's result held in reset, where the test
         * could take the call */
        reset = ETD_G2Read8(RTL_CR);
        if (!(reset & RTL_CR_RESET)) {
            break;
        }
    }
    value = ETD_G2Read8(RTL_CONFIG5);
    ETD_G2Write8(RTL_CONFIG5, value | CONFIG5_LDPS);
    ETD_G2Write8(RTL_CFG9346, RTL_EEPROM_CONFIG);
    value = ETD_G2Read8(RTL_CONFIG1);
    ETD_G2Write8(RTL_CONFIG1, (value | CONFIG1_LED1) & ~CONFIG1_LED0);
    ETD_G2Write8(RTL_CFG9346, RTL_EEPROM_NORMAL);
    if (DLD_IsLinkUp() == TRUE) {
        dl_adapter->link_up = TRUE;
    } else {
        dl_adapter->link_up = FALSE;
    }
    ETD_G2Write32(RTL_RBSTART, dl_adapter->rx_position);
    ETD_G2Write16(RTL_CAPR, dl_adapter->rx_read);
    ETD_G2Write32(RTL_TSAD(0), dl_adapter->tx_buffer[0]);
    ETD_G2Write32(RTL_TSAD(1), dl_adapter->tx_buffer[1]);
    ETD_G2Write32(RTL_TSAD(2), dl_adapter->tx_buffer[2]);
    ETD_G2Write32(RTL_TSAD(3), dl_adapter->tx_buffer[3]);
    /* @hack spelling: the read held in command and changed there, where the
     * write could take the changed read */
    command = ETD_G2Read8(RTL_CR);
    command |= RTL_CR_TE | RTL_CR_RE;
    ETD_G2Write8(RTL_CR, command);
    ETD_G2Write32(RTL_MAR0, MULTICAST_ALL);
    ETD_G2Write32(RTL_MAR4, MULTICAST_ALL);
    ETD_G2Write8(RTL_CFG9346, RTL_EEPROM_CONFIG);
    ETD_G2Write16(RTL_BMCR, dl_adapter->bmcr);
    ETD_G2Write8(RTL_CFG9346, RTL_EEPROM_NORMAL);
    ETD_G2Write32(RTL_TCR, dl_adapter->tx_config);
    ETD_G2Write32(RTL_RCR, dl_adapter->rx_config);
    ETD_G2Write16(RTL_ISR, RTL_ISR_ONES);
    ETD_G2Write16(RTL_IMR, RTL_ISR_ALL);
    return 0;
}

Sint32 DLD_SetCtlPara(struct rtl_config *config)
{
    dl_config->media = config->media;
    dl_config->loopback = config->loopback;
    dl_config->receive_mode = config->receive_mode;
    ETD_DriverInit();
    return ETD_CardInit();
}

void DLD_GetCtlPara(struct rtl_config *config)
{
    config->media = dl_config->media;
    config->loopback = dl_config->loopback;
    config->receive_mode = dl_config->receive_mode;
}

void DLD_CntrStatus(union rtl_statistics *stats)
{
    *stats = dl_adapter->stats;
}

Sint32 DLD_IsLinkUp(void)
{
    /* @hack spelling: the call's result held in status, where the test could
     * take the call */
    Uint8 status = ETD_G2Read8(RTL_MSR);
    if (status & MSR_LINK_FAIL) {
        return FALSE;
    }
    return TRUE;
}

Sint32 rtl8139_reload_eeprom(void)
{
    Uint32 i;
    Uint8 value;
    ETD_G2Write8(RTL_CFG9346, RTL_EEPROM_RELOAD);
    for (i = 0; i < EEPROM_RELOAD_POLLS; i++) {
        /* @hack spelling: the call's result held in value, where the test
         * could take the call */
        value = ETD_G2Read8(RTL_CFG9346);
        if (!(value & RTL_EEPROM_MODE)) {
            break;
        }
        ETD_DlcWait(RELOAD_POLL_WAIT);
    }
    if (i == EEPROM_RELOAD_POLLS) {
        return -1;
    }
    return 0;
}

#define READ_REGISTER(address, value, width)                                   \
    syG2Read((void *)(address), &(value), width, 1, SYE_G2_ADR_MODE_ADD,       \
             SYE_G2_ADR_MODE_ADD)
/* @hack spelling: value copied into copy, where value's own address would
 * do */
#define WRITE_REGISTER(type, address, value, width)                            \
    {                                                                          \
        type copy = (value);                                                   \
        syG2Write(&copy, (void *)(address), width, 1, SYE_G2_ADR_MODE_ADD,     \
                  SYE_G2_ADR_MODE_ADD);                                        \
    }

Uint8 ETD_G2Read8(Uint32 address)
{
    Uint8 value;
    READ_REGISTER(address, value, SYE_G2_DATA_WIDTH_8BIT);
    return value;
}
Uint16 ETD_G2Read16(Uint32 address)
{
    Uint16 value;
    READ_REGISTER(address, value, SYE_G2_DATA_WIDTH_16BIT);
    return value;
}
Uint32 ETD_G2Read32(Uint32 address)
{
    Uint32 value;
    READ_REGISTER(address, value, SYE_G2_DATA_WIDTH_32BIT);
    return value;
}
void ETD_G2Write8(Uint32 address, Uint8 value)
{
    WRITE_REGISTER(Uint8, address, value, SYE_G2_DATA_WIDTH_8BIT);
}
void ETD_G2Write16(Uint32 address, Uint16 value)
{
    WRITE_REGISTER(Uint16, address, value, SYE_G2_DATA_WIDTH_16BIT);
}
void ETD_G2Write32(Uint32 address, Uint32 value)
{
    WRITE_REGISTER(Uint32, address, value, SYE_G2_DATA_WIDTH_32BIT);
}
