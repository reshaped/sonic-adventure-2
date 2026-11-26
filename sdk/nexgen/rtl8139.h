/* the broadband adapter's registers on the G2 bus: GAPS, its PCI bridge,
 * and the RTL8139 behind it. the adapter's packet memory holds the 16 KiB
 * receive ring from RTL_RX_RING. the four 2 KiB transmit buffers start at
 * RTL_TX_FIRST */
#ifndef NEXGEN_RTL8139_H
#define NEXGEN_RTL8139_H
#include <sg_xpt.h>

#define GAPS_ID 0xa1001400
#define GAPS_RESET 0xa1001418
#define GAPS_SIGNATURE 0xa100141c
#define GAPS_DMA_ADDRESS 0xa100142c
#define GAPS_DMA_WINDOW 0xa1848000
#define GAPS_ENABLE 0x5a14a501
#define GAPS_DISABLE 0x5a14a500
#define GAPS_RUNNING 1 /* what GAPS_RESET reads once GAPS_ENABLE takes */
#define G2_UNCACHED 0xa0000000

#define RTL_IDR0 0xa1001700
#define RTL_IDR4 0xa1001704
#define RTL_MAR0 0xa1001708
#define RTL_MAR4 0xa100170c
#define RTL_TSD0 0xa1001710
#define RTL_TSAD0 0xa1001720
/* transmit slot n's status and its buffer's address, 32-bit registers in a
 * row from slot 0's */
#define RTL_TSD(n) (RTL_TSD0 + ((n) * 4))
#define RTL_TSAD(n) (RTL_TSAD0 + ((n) * 4))
#define RTL_RBSTART 0xa1001730
#define RTL_CR 0xa1001737
#define RTL_CAPR 0xa1001738
#define RTL_IMR 0xa100173c
#define RTL_ISR 0xa100173e
#define RTL_TCR 0xa1001740
#define RTL_RCR 0xa1001744
#define RTL_MPC 0xa100174c
#define RTL_CFG9346 0xa1001750
#define RTL_CONFIG1 0xa1001752
#define RTL_MSR 0xa1001758
#define RTL_BMCR 0xa1001762
#define RTL_CONFIG5 0xa10017d8

#define RTL_CR_RESET 0x10
#define RTL_RESET_POLLS 1000 /* the reads of CR that wait out RTL_CR_RESET */
#define RTL_CR_RE 8
#define RTL_CR_TE 4
#define RTL_CR_EMPTY 1
#define RTL_ISR_ROK 1
#define RTL_ISR_RER 2
#define RTL_ISR_TOK 4
#define RTL_ISR_TER 8
#define RTL_ISR_RXOVW 0x10
#define RTL_ISR_LINK 0x20
#define RTL_ISR_FOVW 0x40
#define RTL_ISR_SERR 0x8000
#define RTL_ISR_ALL 0x807f
#define RTL_ISR_ONES 0xffff /* every bit of ISR */
#define RTL_TSD_OK 0x8000
#define RTL_TSD_COPIED 0x2000 /* the card has the frame in its FIFO */
#define RTL_TSD_UNDERRUN 0x4000
/* start sending once the FIFO holds 64 bytes */
#define RTL_TSD_THRESHOLD_64 0x20000
#define RTL_TSD_COLLISIONS 0x0f000000 /* how many collisions the send met */
#define RTL_TSD_COLLISIONS_SHIFT 24
#define RTL_TSD_NO_HEARTBEAT 0x10000000
#define RTL_TSD_OUT_OF_WINDOW 0x20000000 /* a collision late in the frame */
#define RTL_TSD_ABORTED 0x40000000
#define RTL_TSD_CARRIER_LOST 0x80000000
#define RTL_TCR_RETRANSMIT 1 /* send the aborted frame again */
#define RTL_TCR_DMA_32 0x100 /* DMA bursts of 32 bytes */
/* TCR's loopback field, set to mode, 0 to 3 */
#define RTL_TCR_LOOPBACK(mode) ((mode) << 17)
#define RTL_TCR_IFG_STANDARD 0x03000000 /* the standard gap between frames */
/* RCR's bits */
#define RTL_RCR_ALL 1 /* take frames to any address */
#define RTL_RCR_OWN 2 /* take frames to the adapter's address */
#define RTL_RCR_MULTICAST 4
#define RTL_RCR_BROADCAST 8
#define RTL_RCR_ERRORS 0x20 /* take frames with errors */
#define RTL_RCR_EEPROM_WIDE 0x40 /* the EEPROM is a 93C56 */
/* a frame that reaches the ring's end runs on past it, rather than wrapping
 * to the ring's start */
#define RTL_RCR_NO_WRAP 0x80
#define RTL_RCR_DMA_32 0x100 /* DMA bursts of 32 bytes */
#define RTL_RCR_RING_16K 0x800 /* the 16 KiB ring */
/* start moving a frame once the FIFO holds 64 bytes */
#define RTL_RCR_FIFO_64 0x4000
/* Cfg9346's pins to the EEPROM. data in goes to the EEPROM, and data out
 * comes from it */
#define RTL_EEPROM_DATA_OUT 1
#define RTL_EEPROM_DATA_IN 2
#define RTL_EEPROM_CLOCK 4
#define RTL_EEPROM_CS 8
/* Cfg9346's mode field, and its modes. the card returns to
 * RTL_EEPROM_NORMAL when a reload ends */
#define RTL_EEPROM_MODE 0xc0
#define RTL_EEPROM_NORMAL 0
#define RTL_EEPROM_RELOAD 0x40
#define RTL_EEPROM_PROGRAM 0x80
#define RTL_EEPROM_CONFIG 0xc0
#define RTL_RX_RING 0x01840000
#define RTL_RX_RING_END 0x01844000
#define RTL_TX_FIRST 0x01846000
#define RTL_TX_BYTES 0x800
#define RTL_TX_SLOTS 4
#define RTL_RX_INITIAL_CAPR 0xfff0
#define ETHERNET_ADDRESS_BYTES 6
#define ETHERNET_HEADER_BYTES 14
#define ETHERNET_MTU 1500
#define ETHERNET_FRAME_BYTES (ETHERNET_HEADER_BYTES + ETHERNET_MTU)
#define ETHERNET_CRC_BYTES 4

struct rtl_address {
    Uint8 bytes[ETHERNET_ADDRESS_BYTES];
};
struct rtl_rx_header {
    Uint16 status;
    Uint16 length;
};
/* rtl_rx_header.status's bits */
#define RTL_RX_OK 1
#define RTL_RX_BROADCAST 0x2000
#define RTL_RX_PHYSICAL 0x4000 /* to the adapter's own address */
#define RTL_RX_MULTICAST 0x8000
struct rtl_config {
    Sint32 media;
    Sint32 loopback;
    Sint32 receive_mode;
};
/* rtl_config.media's values */
#define RTL_MEDIA_10_HALF 0
#define RTL_MEDIA_10_FULL 1
#define RTL_MEDIA_100_HALF 2
#define RTL_MEDIA_100_FULL 3
#define RTL_MEDIA_AUTO 4 /* speed and duplex by autonegotiation */
/* rtl_config.loopback, which ETD_DriverInit puts in TCR's loopback field as
 * it is */
#define RTL_LOOPBACK_OFF 0
#define RTL_LOOPBACK_ON 3 /* each frame sent comes back in */
/* rtl_config.receive_mode's values. RTL_RECEIVE_BROADCAST takes broadcasts
 * and frames to the adapter's address. each mode above it takes more:
 * multicasts, then frames to any address, then frames with errors */
#define RTL_RECEIVE_NONE 0
#define RTL_RECEIVE_BROADCAST 1
#define RTL_RECEIVE_MULTICAST 2
#define RTL_RECEIVE_ALL 3
#define RTL_RECEIVE_ERRORS 4
struct rtl_counters {
    Uint32 tx_packets, rx_packets, tx_bytes, rx_bytes;
    Uint32 tx_full, rx_dropped, collisions, tx_aborted;
    Uint32 rx_too_long, reserved, rx_errors, tx_carrier_lost;
    Uint32 tx_out_of_window, rx_multicast, rx_unicast, rx_broadcast;
    Uint32 fifo_overflows, rx_overflows, system_errors;
};
union rtl_statistics {
    struct rtl_counters count;
    Uint32 words[19];
};
struct rtl_state {
    Uint32 rx_config, tx_config;
    Uint32 tx_buffer[RTL_TX_SLOTS];
    Uint32 rx_end;
    Uint16 bmcr;
    Uint32 rx_position;
    Uint16 rx_read, rx_length;
    Uint8 tx_pending, tx_head, tx_tail, link_up;
    union rtl_statistics stats;
};
/* run the adapter's self-test. it checks GAPS's and the RTL8139's
 * registers, then that the EEPROM's MAC address is the IDR registers'. it
 * writes two test frames. RTL_DIAG_LOOPBACK sends each through the card's
 * loopback, and checks what comes back and the counters. RTL_DIAG_MEMORY
 * sends neither, and reads each back from its transmit buffer. any mode but
 * RTL_DIAG_LOOPBACK is RTL_DIAG_MEMORY.
 *
 * it returns 0 with dl_config's settings back on the card. otherwise it
 * returns 0xffff0000 | a code for the check that failed, which some checks
 * share. a failed check returns at once, and leaves the card as the test
 * left it */
#define RTL_DIAG_MEMORY 0
#define RTL_DIAG_LOOPBACK 1
Sint32 ETD_DiagMain(Sint32 mode);
/* enable GAPS and set up its bridge. it returns 0, or -1 if GAPS isn't
 * running 2 ms later */
Sint32 ETD_MadisonInit(void);
/* turn on the card's PCI memory space and bus mastering, and clear its PCI
 * errors. a card with power management goes to power state D0 */
void ETD_PciInit(void);
/* set up dl_adapter's transmit buffers and zero its counters. its TCR, RCR
 * and BMCR values, which ETD_CardInit writes, come from dl_config's
 * settings. it sets dl_receive_test to 0 */
void ETD_DriverInit(void);
/* reset the card and set it up from dl_adapter. it takes every multicast
 * address, and enables every interrupt RTL_ISR_ALL holds. it returns 0 */
Sint32 ETD_CardInit(void);
/* take config's settings into dl_config, then set dl_adapter and the card
 * up from them. it returns 0 */
Sint32 DLD_SetCtlPara(struct rtl_config *config);
/* copy dl_config's settings into config */
void DLD_GetCtlPara(struct rtl_config *config);
/* copy the driver's counters into stats */
void DLD_CntrStatus(union rtl_statistics *stats);
/* TRUE while MSR says the link is up, else FALSE */
Sint32 DLD_IsLinkUp(void);
/* have the card reload the EEPROM. it returns 0 once Cfg9346 is back at
 * RTL_EEPROM_NORMAL, or -1 if it isn't after 3000 polls */
Sint32 rtl8139_reload_eeprom(void);
/* write address into the EEPROM, then read it back. the IDR registers keep
 * the old address. it returns 0, and leaves Cfg9346 at RTL_EEPROM_NORMAL. it
 * returns -1 if an erase or a write times out, or if the readback differs. a
 * time-out leaves the EEPROM's writes enabled and Cfg9346 at
 * RTL_EEPROM_PROGRAM */
Sint32 DLD_SetEtdAddr(struct rtl_address *address);
/* read the MAC address from the EEPROM into address, and leave Cfg9346 at
 * RTL_EEPROM_NORMAL. it returns 0 */
Sint32 DLD_GetEtdAddr(struct rtl_address *address);
#endif
