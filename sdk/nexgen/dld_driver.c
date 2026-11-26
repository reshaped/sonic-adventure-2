#include <sg_exint.h>
#include <sg_syg2.h>
#include <sg_xpt.h>
#include <shinobi.h>
#include "../network/g2_io.h"
#include "../network/packet_driver.h"
#include "../nexgen/dld_driver.h"
#include "../nexgen/dld_state.h"
#include "../nexgen/rtl8139.h"

#define PACKET_RAM_BYTES                                                       \
    (RTL_TX_FIRST + (RTL_TX_SLOTS * RTL_TX_BYTES) - RTL_RX_RING)
#define GAPS_ID_VALUE 0x53504147 /* "GAPS", low byte first */
#define GAPS_ID_HIGH (GAPS_ID + 2)
#define ETHERTYPE_IP 0x0800
#define ETHERTYPE_ARP 0x0806
#define ETHERNET_MIN_FRAME_BYTES 60 /* without its CRC */
/* syG2DmaSetPrm cuts its addresses and its size down to whole cache lines */
#define CACHE_LINE_BYTES 32
#define ITEM_BYTES 4 /* the bytes of a SYE_G2_DATA_WIDTH_32BIT item */
#define RX_HEADER_BYTES ((int)sizeof(struct rtl_rx_header))
#define MPC_COUNT 0xffffff /* the bits of MPC that count missed frames */
/* the 16th collision aborts a send, and RTL_TSD_COLLISIONS holds 15 at most */
#define ABORT_COLLISIONS 16

struct ethernet_header {
    Uint8 destination[ETHERNET_ADDRESS_BYTES];
    Uint8 source[ETHERNET_ADDRESS_BYTES];
    Uint16 type; /* the EtherType, high byte first */
};
/* ethertype as the Uint16 type field reads it */
#define FRAME_TYPE(ethertype) ((((ethertype) & 0xff) << 8) | ((ethertype) >> 8))

static void card_interrupt(void *argument);
static void receive_interrupt(void);
static void deliver_frame(void);
static void restart_receiver(void);
static void transmit_interrupt(void);
static void copy_from_ring(Uint32 bus, void *buffer, Sint32 length);

Sint32 DLD_probe(void)
{
    /* @hack spelling: each read's result held in low, high or id, where each
     * test could take its read */
    Uint16 low;
    Uint16 high;
    Uint32 id;
    low = ETD_G2Read16(GAPS_ID);
    if (low != (GAPS_ID_VALUE & 0xffff)) {
        return -1;
    }
    high = ETD_G2Read16(GAPS_ID_HIGH);
    if (high != (GAPS_ID_VALUE >> 16)) {
        return -1;
    }
    id = ETD_G2Read32(GAPS_ID);
    if (id != GAPS_ID_VALUE) {
        return -1;
    }
    return 0;
}

Sint32 DLD_init(Sint32 mode)
{
    /* @hack spelling: each call's result held in result, where each test
     * could take its call */
    Sint32 result;
    Uint32 address;
    /* @hack spelling: offset, which counts beside address, where the loop's
     * test could read address */
    Sint32 offset;
    dl_adapter = &dl_adapter_state;
    dl_config = &dl_adapter_config;
    result = ETD_MadisonInit();
    if (result != 0) {
        return -2;
    }
    ETD_PciInit();
    dl_interrupt =
        syExtIntAddHandler(SYD_EXTINT_EVENT_IML4_LANPCI, card_interrupt, 16,
                           dl_interrupt_argument);
    offset = 0;
    /* @hack spelling: address = address + ITEM_BYTES, where
     * address += ITEM_BYTES would do */
    for (address = RTL_RX_RING | G2_UNCACHED; offset < PACKET_RAM_BYTES;
         address = address + ITEM_BYTES, offset += ITEM_BYTES) {
        ETD_G2Write32(address, 0);
    }
    dl_config->media = RTL_MEDIA_10_HALF;
    dl_config->loopback = RTL_LOOPBACK_OFF;
    dl_config->receive_mode = RTL_RECEIVE_BROADCAST;
    ETD_DriverInit();
    result = ETD_CardInit();
    if (result != 0) {
        return -3;
    }
    result = ETD_DiagMain(RTL_DIAG_MEMORY);
    if (result != 0) {
        return -1;
    }
    return 0;
}

Sint32 DLD_terminate(Sint32 mode)
{
    if (DLD_pd_rcv806 != NULL) {
        return -2;
    }
    if (DLD_pd_rcv800 != NULL) {
        return -2;
    }
    syExtIntDeleteHandler(dl_interrupt);
    ETD_G2Write32(GAPS_RESET, GAPS_DISABLE);
    return 0;
}

Sint32 DLD_driver_info(Sint32 handle, struct pd_info *info)
{
    info->version = 1;
    info->if_class = 1;
    info->if_type = 52;
    info->if_number = 0;
    info->name = dl_driver_build;
    info->functionality = 2;
    return 0;
}

Sint32 DLD_access_type(Sint32 if_class, Sint32 if_type, Sint32 if_number,
                       const char *type, Uint32 type_length,
                       pd_receiver receiver)
{
#ifdef NONMATCHING
    /* this side refuses a missing type, a length too short for the type's 2
     * bytes, and a missing receiver */
    if (!type || type_length < 2 || !receiver) {
        return -2;
    }
#endif
    if (type[0] == (ETHERTYPE_ARP >> 8) && type[1] == (ETHERTYPE_ARP & 0xff)) {
        if (DLD_pd_rcv806 != NULL) {
            return -1;
        }
        DLD_pd_rcv806 = receiver;
        return ETHERTYPE_ARP;
    }
    if (type[0] == (ETHERTYPE_IP >> 8) && type[1] == (ETHERTYPE_IP & 0xff)) {
        if (DLD_pd_rcv800 != NULL) {
            return -1;
        }
        DLD_pd_rcv800 = receiver;
        return ETHERTYPE_IP;
    }
    return -2;
}

Sint32 DLD_release_type(Sint32 handle)
{
    if (handle == ETHERTYPE_ARP) {
        DLD_pd_rcv806 = NULL;
        return 0;
    }
    if (handle == ETHERTYPE_IP) {
        DLD_pd_rcv800 = NULL;
        return 0;
    }
    return -1;
}

#define WRITE_BUS(source, destination, width, count)                           \
    syG2Write(source, (void *)(destination), width, count,                     \
              SYE_G2_ADR_MODE_ADD, SYE_G2_ADR_MODE_ADD)
#define READ_BUS(source, destination, width, count)                            \
    syG2Read((void *)(source), destination, width, count, SYE_G2_ADR_MODE_ADD, \
             SYE_G2_ADR_MODE_ADD)

Sint32 DLD_send_pkt(const void *buffer, Uint32 length)
{
    SYG2DMA channel;
    Bool busy;
    void *source;
    Uint32 bytes;
    Uint32 destination;
    Uint32 size;
    busy = TRUE;
    if (dl_adapter->link_up != TRUE) {
        return -2;
    }
    if (dl_adapter->tx_pending >= RTL_TX_SLOTS) {
        dl_adapter->stats.count.tx_full++;
        return -1;
    }
#ifdef NONMATCHING
    /* this side repairs the bug tagged below for a frame that isn't whole cache
     * lines: it writes the frame a byte at a time, and pads one shorter than
     * ETHERNET_MIN_FRAME_BYTES with zeros. it refuses a missing or empty frame
     * and one past ETHERNET_FRAME_BYTES */
    if (!buffer || !length || length > ETHERNET_FRAME_BYTES) {
        return -1;
    }
    if (((Uint32)buffer & (CACHE_LINE_BYTES - 1)) ||
        (length & (CACHE_LINE_BYTES - 1)) ||
        length < ETHERNET_MIN_FRAME_BYTES) {
        static const Uint8 padding[ETHERNET_MIN_FRAME_BYTES] = {0};
        channel = NULL;
        destination = dl_adapter->tx_buffer[dl_adapter->tx_head] | G2_UNCACHED;
        WRITE_BUS((void *)buffer, destination, SYE_G2_DATA_WIDTH_8BIT, length);
        if (length < ETHERNET_MIN_FRAME_BYTES) {
            WRITE_BUS((void *)padding, destination + length,
                      SYE_G2_DATA_WIDTH_8BIT,
                      ETHERNET_MIN_FRAME_BYTES - length);
        }
        goto submit;
    }
#endif
    /* @bug memory: reads up to 31 bytes before the frame and 31 past it, the
     * rest of the cache lines that hold it, by DMA or by WRITE_BUS. the
     * source is rounded down to a line at 0x0cadf168, and the size up at
     * 0x0cadf166 */
    syG2DmaOpen(&channel, SYE_G2_DMA_TYPE_EXT0);
    if (channel == NULL) {
        syG2DmaOpen(&channel, SYE_G2_DMA_TYPE_EXT1);
    }
    source = (void *)((Uint32)buffer & -CACHE_LINE_BYTES);
    destination = dl_adapter->tx_buffer[dl_adapter->tx_head] -
                  ((Uint32)buffer & (CACHE_LINE_BYTES - 1));
    bytes = (((Uint32)buffer & (CACHE_LINE_BYTES - 1)) + length +
             CACHE_LINE_BYTES - 1) &
            -CACHE_LINE_BYTES;
    if (channel != NULL) {
        ETD_G2Write32(GAPS_DMA_ADDRESS, destination);
        syCacheOCBP(source, bytes);
        syG2DmaSetPrm(channel, (void *)GAPS_DMA_WINDOW, source, bytes,
                      SYE_G2_DMA_DIR_ROOT_TO_G2, FALSE, TRUE, NULL, NULL);
        syG2DmaTrigger(channel, SYE_G2_DMA_TRIGGER_CPU);
    } else {
        WRITE_BUS(source, destination | G2_UNCACHED, SYE_G2_DATA_WIDTH_32BIT,
                  bytes / ITEM_BYTES);
    }
#ifdef NONMATCHING
/* where this side's byte copy above joins the send */
submit:
#endif
    if (length < ETHERNET_MIN_FRAME_BYTES) {
        size = ETHERNET_MIN_FRAME_BYTES;
    } else {
        size = length;
    }
    dl_adapter->stats.count.tx_packets++;
    dl_adapter->stats.count.tx_bytes += length;
    if (channel != NULL) {
        while (busy) {
            syG2DmaIsBusy(channel, &busy);
        }
        syG2DmaClose(channel);
    }
    ETD_G2Write32(RTL_TSD(dl_adapter->tx_head), size | RTL_TSD_THRESHOLD_64);
    dl_adapter->tx_pending++;
    dl_adapter->tx_head = (dl_adapter->tx_head + 1) & (RTL_TX_SLOTS - 1);
    return 0;
}

Sint32 DLD_get_address(Sint32 handle, Uint8 *address, Sint32 *length)
{
    Uint32 id;
    id = ETD_G2Read32(RTL_IDR0);
    address[0] = id;
    address[1] = id >> 8;
    address[2] = id >> 16;
    address[3] = id >> 24;
    id = ETD_G2Read32(RTL_IDR4);
    address[4] = id;
    address[5] = id >> 8;
    *length = ETHERNET_ADDRESS_BYTES;
    return 0;
}

Sint32 DLD_set_address(Uint8 *address, Sint32 length)
{
    Sint32 result;
    if (length != ETHERNET_ADDRESS_BYTES) {
        return -2;
    }
    /* @hack spelling: the call's result held in result, where the test could
     * take the call */
    result = DLD_SetEtdAddr((struct rtl_address *)address);
    if (result < 0) {
        return -1;
    }
    return 0;
}

static void card_interrupt(void *argument)
{
    Uint16 status;
    Uint32 missed;
    ETD_G2Write16(RTL_IMR, 0);
    for (;;) {
        status = ETD_G2Read16(RTL_ISR);
        if (!(status & RTL_ISR_ALL)) {
            break;
        }
        if (status == RTL_ISR_ONES) {
            ETD_CardInit();
            break;
        }
        ETD_G2Write16(RTL_ISR, status);
        if (status & (RTL_ISR_TOK | RTL_ISR_TER)) {
            transmit_interrupt();
        }
        if (status &
            (RTL_ISR_ROK | RTL_ISR_RER | RTL_ISR_RXOVW | RTL_ISR_FOVW)) {
            if (status & RTL_ISR_RXOVW) {
                dl_adapter->stats.count.rx_overflows++;
            }
            if (status & RTL_ISR_FOVW) {
                dl_adapter->stats.count.fifo_overflows++;
                /* @hack spelling: the masked read held in missed, where the
                 * sum could take it */
                missed = ETD_G2Read32(RTL_MPC) & MPC_COUNT;
                dl_adapter->stats.count.rx_dropped += missed;
                ETD_G2Write32(RTL_MPC, 0);
            }
            receive_interrupt();
        }
        if (status & RTL_ISR_LINK) {
            if (DLD_IsLinkUp() == TRUE) {
                dl_adapter->link_up = TRUE;
            } else {
                dl_adapter->link_up = FALSE;
            }
        }
        if (status & RTL_ISR_SERR) {
            dl_adapter->stats.count.system_errors++;
        }
    }
    ETD_G2Write16(RTL_IMR, RTL_ISR_ALL);
}

static void receive_interrupt(void)
{
    struct rtl_rx_header header;
    Uint16 status;
    Uint8 command;
    for (;;) {
        /* @hack spelling: the call's result held in command, where the test
         * could take the call */
        command = ETD_G2Read8(RTL_CR);
        if (command & RTL_CR_EMPTY) {
            break;
        }
        READ_BUS(dl_adapter->rx_position | G2_UNCACHED, &header,
                 SYE_G2_DATA_WIDTH_32BIT, 1);
        dl_adapter->rx_length = header.length;
        status = header.status;
#ifdef NONMATCHING
        /* this side drops a frame too short to hold an Ethernet header and its
         * CRC */
        if (header.length < ETHERNET_HEADER_BYTES + ETHERNET_CRC_BYTES) {
            status &= ~RTL_RX_OK;
        }
#endif
        dl_adapter->rx_position += sizeof(header);
        if (status & RTL_RX_OK) {
            dl_adapter->stats.count.rx_packets++;
            dl_adapter->stats.count.rx_bytes +=
                header.length - ETHERNET_CRC_BYTES;
            if (status & RTL_RX_PHYSICAL) {
                dl_adapter->stats.count.rx_unicast++;
            } else if (status & RTL_RX_BROADCAST) {
                dl_adapter->stats.count.rx_broadcast++;
            } else if (status & RTL_RX_MULTICAST) {
                dl_adapter->stats.count.rx_multicast++;
            }
            deliver_frame();
        } else {
            dl_adapter->stats.count.rx_errors++;
            restart_receiver();
        }
    }
}

static void deliver_frame(void)
{
    struct ethernet_header header;
    Sint32 length;
    Uint32 bus;
    void *buffer;
    void *returned;

    length = dl_adapter->rx_length - ETHERNET_CRC_BYTES;
    if (length > ETHERNET_FRAME_BYTES) {
        dl_adapter->stats.count.rx_dropped++;
        dl_adapter->stats.count.rx_too_long++;
    } else {
        bus = dl_adapter->rx_position | G2_UNCACHED;
        READ_BUS(bus, &header, SYE_G2_DATA_WIDTH_8BIT, ETHERNET_HEADER_BYTES);
        if (dl_receive_test != -1) {
            if (header.type == FRAME_TYPE(ETHERTYPE_ARP)) {
                if (DLD_pd_rcv806 == NULL) {
                    returned = NULL;
                } else {
                    returned = DLD_pd_rcv806((void *)DLD_pd_rcv806,
                                             PD_GET_BUFFER, length, NULL);
                }
            } else if (header.type == FRAME_TYPE(ETHERTYPE_IP)) {
                if (DLD_pd_rcv800 == NULL) {
                    returned = NULL;
                } else {
                    returned = DLD_pd_rcv800((void *)DLD_pd_rcv800,
                                             PD_GET_BUFFER, length, NULL);
                }
            } else {
                returned = NULL;
            }
            if (returned != NULL) {
                /* @hack spelling: returned copied into buffer, where
                 * copy_from_ring could take returned */
                buffer = returned;
                copy_from_ring(bus, buffer, length);
                if (header.type == FRAME_TYPE(ETHERTYPE_ARP)) {
                    DLD_pd_rcv806((void *)DLD_pd_rcv806, PD_COPIED, length,
                                  NULL);
                } else if (header.type == FRAME_TYPE(ETHERTYPE_IP)) {
                    DLD_pd_rcv800((void *)DLD_pd_rcv800, PD_COPIED, length,
                                  NULL);
                }
            } else {
                /* @hack redundant: the goto to the label right below it */
                goto dropped;
            dropped:
                dl_adapter->stats.count.rx_dropped++;
            }
        } else {
            dl_receive_test = 0;
        }
    }
    /* move rx_read and rx_position to the next frame's header, at a multiple
     * of 4. CAPR, which takes rx_read, holds 16 bits */
    dl_adapter->rx_read =
        (dl_adapter->rx_read + dl_adapter->rx_length + (RX_HEADER_BYTES + 3)) &
        0xfffc;
    dl_adapter->rx_position =
        (dl_adapter->rx_position + dl_adapter->rx_length + 3) & ~3;
    if (dl_adapter->rx_position >= dl_adapter->rx_end) {
        dl_adapter->rx_position =
            dl_adapter->rx_position - dl_adapter->rx_end + RTL_RX_RING;
    }
    ETD_G2Write32(RTL_CAPR, dl_adapter->rx_read);
}

static void restart_receiver(void)
{
    /* @hack spelling: each read held in command and changed there, where
     * each write could take its changed read */
    Uint8 command;
    command = ETD_G2Read8(RTL_CR);
    command &= ~RTL_CR_RE;
    ETD_G2Write8(RTL_CR, command);
    command = ETD_G2Read8(RTL_CR);
    command |= RTL_CR_RE;
    ETD_G2Write8(RTL_CR, command);
    ETD_G2Write32(RTL_RCR, dl_adapter->rx_config);
    dl_adapter->rx_position = RTL_RX_RING;
    dl_adapter->rx_read = RTL_RX_INITIAL_CAPR;
}

static void transmit_interrupt(void)
{
    Uint32 status;
    while (dl_adapter->tx_pending > 0) {
        status = ETD_G2Read32(RTL_TSD(dl_adapter->tx_tail));
        if (status & RTL_TSD_OK) {
            dl_adapter->stats.count.collisions +=
                (status & RTL_TSD_COLLISIONS) >> RTL_TSD_COLLISIONS_SHIFT;
            if (status & RTL_TSD_CARRIER_LOST) {
                dl_adapter->stats.count.tx_carrier_lost++;
            }
            if (status & RTL_TSD_OUT_OF_WINDOW) {
                dl_adapter->stats.count.tx_out_of_window++;
            }
            dl_adapter->tx_pending--;
            dl_adapter->tx_tail =
                (dl_adapter->tx_tail + 1) & (RTL_TX_SLOTS - 1);
        } else if (status & RTL_TSD_ABORTED) {
#ifdef NONMATCHING
            /* this side repairs the bug tagged below: it sets TCR from
             * tx_config */
            ETD_G2Write32(RTL_TCR, dl_adapter->tx_config | RTL_TCR_RETRANSMIT);
#else
            /* @bug logic: retries an aborted send with TCR set from
             * rx_config, where tx_config is meant, at 0x0cadf78a */
            ETD_G2Write32(RTL_TCR, dl_adapter->rx_config | RTL_TCR_RETRANSMIT);
#endif
            dl_adapter->stats.count.collisions += ABORT_COLLISIONS;
            dl_adapter->stats.count.tx_aborted++;
        } else {
            break;
        }
    }
}

/* copy the frame at bus into buffer, length bytes rounded up to a multiple of
 * 4. DMA copies the cache lines that lie wholly in buffer, and 4-byte reads
 * copy the rest. without a channel or a whole cache line in buffer, 4-byte
 * reads copy it all */
static void copy_from_ring(Uint32 bus, void *buffer, Sint32 length)
{
    SYG2DMA channel;
    Bool busy;
    Uint32 aligned;
    Sint32 head;
    Sint32 total;
    Sint32 rest;
    Sint32 body;
    Uint32 source;
    busy = TRUE;
#ifdef NONMATCHING
    /* this side repairs the tag below: it copies a length that isn't a
     * multiple of 4 a byte at a time, so it writes nothing past length bytes.
     * it copies a buffer off a 4-byte boundary a byte at a time too, where
     * 4-byte stores would be misaligned. a length of 0 or less copies
     * nothing */
    if (length <= 0) {
        return;
    }
    if (((Uint32)buffer & 3) || (length & 3)) {
        READ_BUS(bus, buffer, SYE_G2_DATA_WIDTH_8BIT, length);
        return;
    }
#endif
    /* @bug memory: writes up to 3 bytes past the length bytes the receiver's
     * buffer holds. the size is rounded up to a multiple of 4 at 0x0cadf800.
     * the reads at 0x0cadf864 and 0x0cadf8d0, or the DMA, write it whole */
    syG2DmaOpen(&channel, SYE_G2_DMA_TYPE_EXT0);
    if (channel == NULL) {
        syG2DmaOpen(&channel, SYE_G2_DMA_TYPE_EXT1);
    }
    total = (length + 3) & ~3;
    aligned = ((Uint32)buffer + CACHE_LINE_BYTES - 1) & -CACHE_LINE_BYTES;
    head = aligned - (Uint32)buffer;
    source = bus + head;
    body = (rest = total - head) & -CACHE_LINE_BYTES;
    if (channel != NULL && body > 0) {
        /* @hack redundant: an unreachable test of bus and buffer */
        if (0) {
            if (bus && buffer) {
            }
        }
        if (head != 0) {
            READ_BUS(bus, buffer, SYE_G2_DATA_WIDTH_32BIT, head / ITEM_BYTES);
        }
        if (rest - body != 0) {
            rest -= body;
            READ_BUS(source + body, (void *)(aligned + body),
                     SYE_G2_DATA_WIDTH_32BIT, rest / ITEM_BYTES);
        }
        ETD_G2Write32(GAPS_DMA_ADDRESS, source);
        syCacheOCBI((void *)aligned, body);
        syG2DmaSetPrm(channel, (void *)GAPS_DMA_WINDOW, (void *)aligned, body,
                      SYE_G2_DMA_DIR_G2_TO_ROOT, FALSE, TRUE, NULL, NULL);
        syG2DmaTrigger(channel, SYE_G2_DMA_TRIGGER_CPU);
        while (busy) {
            syG2DmaIsBusy(channel, &busy);
        }
        syG2DmaClose(channel);
    } else {
        /* @bug logic: leaves the DMA channel open where buffer holds no whole
         * cache line. the branch at 0x0cadf81e goes to the 4-byte reads at
         * 0x0cadf8d0, and nothing closes the channel after them */
        READ_BUS(bus, buffer, SYE_G2_DATA_WIDTH_32BIT, total / ITEM_BYTES);
#ifdef NONMATCHING
        /* this side repairs the bug tagged above: it closes the channel */
        if (channel != NULL) {
            syG2DmaClose(channel);
        }
#endif
    }
}
