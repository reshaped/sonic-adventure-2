#include <sg_syg2.h>
#include <sg_xpt.h>
#include "../network/g2_io.h"
#include "../nexgen/dld_state.h"
#include "../nexgen/rtl8139.h"

#define GAPS_SIGNATURE_VALUE 0x41474553 /* "SEGA", low byte first */
#define TEST_PATTERN 0x55aaff00U
#define DIAG_POLLS 0x400000
#define DIAG_ERROR(code) (0xffff0000U | (code))
/* where each test frame's payload starts. its bytes count up from there */
#define FIRST_PAYLOAD 0x55
#define SECOND_PAYLOAD 0x5a
#define BROADCAST_BYTE 0xff /* each byte of the broadcast address */
#define TEST_TX_DESCRIPTOR (RTL_TSD_THRESHOLD_64 | ETHERNET_FRAME_BYTES)
#define RECEIVE_ERRORS (RTL_ISR_RER | RTL_ISR_RXOVW | RTL_ISR_FOVW)
#define TX_ERRORS                                                              \
    (RTL_TSD_CARRIER_LOST | RTL_TSD_ABORTED | RTL_TSD_OUT_OF_WINDOW |          \
     RTL_TSD_NO_HEARTBEAT | RTL_TSD_UNDERRUN)
/* @hack spelling: | between the two tests, where || would do */
#define TSD_FAILED(status)                                                     \
    ((((status) & TX_ERRORS) != 0) |                                           \
     (((status) & (RTL_TSD_OK | RTL_TSD_COPIED)) == 0))

#define SET_LOOPBACK_OR_FAIL(config, result, receive)                          \
    {                                                                          \
        (config).media = RTL_MEDIA_10_HALF;                                    \
        (config).loopback = RTL_LOOPBACK_ON;                                   \
        (config).receive_mode = (receive);                                     \
        (result) = DLD_SetCtlPara(&(config));                                  \
        if ((result) != 0)                                                     \
            return DIAG_ERROR(0x16);                                           \
    }
/* write a broadcast test frame from source, the adapter's address, into the
 * first transmit buffer. its type field holds the frame's length, low byte
 * first. its payload counts up from first */
#define WRITE_TEST_FRAME(cursor, i, source, first)                             \
    {                                                                          \
        (cursor) = dl_adapter->tx_buffer[0] | G2_UNCACHED;                     \
        for ((i) = 0; (i) < ETHERNET_ADDRESS_BYTES; (i)++)                     \
            ETD_G2Write8((cursor)++, BROADCAST_BYTE);                          \
        for ((i) = 0; (i) < ETHERNET_ADDRESS_BYTES; (i)++)                     \
            ETD_G2Write8((cursor)++, (source)[(i)]);                           \
        ETD_G2Write8((cursor)++, ETHERNET_FRAME_BYTES & 0xff);                 \
        ETD_G2Write8((cursor)++, ETHERNET_FRAME_BYTES >> 8);                   \
        for ((i) = 0; (i) < ETHERNET_MTU; (i)++)                               \
            ETD_G2Write8((cursor)++, (i) + (first));                           \
    }
union diagnostic_address {
    struct rtl_address mac;
    struct {
        Uint32 low;
        Uint16 high;
    } words;
};

Sint32 ETD_DiagMain(Sint32 mode)
{
    union diagnostic_address address;
    struct rtl_config saved;
    struct rtl_config test;
    struct rtl_rx_header header;
    Uint32 signature;
    Uint32 status;
    Uint32 cursor;
    unsigned int wait;
    int i;
    /* @hack spelling: one Uint16 for ISR's value, a receive status, a length
     * and a byte pair, where one local for each would do */
    Uint16 word;
    int result;
    Uint8 expected;
    DLD_GetCtlPara(&saved);
    signature = ETD_G2Read32(GAPS_SIGNATURE);
    if (signature != GAPS_SIGNATURE_VALUE) {
        return DIAG_ERROR(0x01);
    }
    ETD_G2Write32(GAPS_SIGNATURE, TEST_PATTERN);
    if (ETD_G2Read32(GAPS_SIGNATURE) != TEST_PATTERN) {
        return DIAG_ERROR(0x02);
    }
    ETD_G2Write32(GAPS_SIGNATURE, ~TEST_PATTERN);
    if (ETD_G2Read32(GAPS_SIGNATURE) != ~TEST_PATTERN) {
        return DIAG_ERROR(0x03);
    }
    ETD_G2Write32(GAPS_SIGNATURE, signature);
    ETD_G2Write8(RTL_CR, RTL_CR_RESET);
    for (i = 0; i < RTL_RESET_POLLS; i++) {
        if (!(ETD_G2Read8(RTL_CR) & RTL_CR_RESET)) {
            break;
        }
    }
    if (ETD_G2Read8(RTL_CR) & (RTL_CR_RE | RTL_CR_TE)) {
        return DIAG_ERROR(0x04);
    }
    ETD_G2Write8(RTL_CR, RTL_CR_RE);
    if (!(ETD_G2Read8(RTL_CR) & RTL_CR_RE)) {
        return DIAG_ERROR(0x11);
    }
    ETD_G2Write8(RTL_CR, RTL_CR_TE);
    if (!(ETD_G2Read8(RTL_CR) & RTL_CR_TE)) {
        return DIAG_ERROR(0x12);
    }
    ETD_G2Write8(RTL_CR, 0);
    ETD_G2Write32(RTL_MAR0, TEST_PATTERN);
    ETD_G2Write32(RTL_MAR4, ~TEST_PATTERN);
    if (ETD_G2Read32(RTL_MAR0) != TEST_PATTERN ||
        ETD_G2Read32(RTL_MAR4) != ~TEST_PATTERN) {
        return DIAG_ERROR(0x13);
    }
    SET_LOOPBACK_OR_FAIL(test, result, RTL_RECEIVE_NONE);
    if (!(ETD_G2Read8(RTL_CR) & RTL_CR_EMPTY)) {
        return DIAG_ERROR(0x14);
    }
    /* @hack spelling: the call's result held in result, where the test could
     * take the call */
    result = DLD_GetEtdAddr(&address.mac);
    if (result < 0) {
        return DIAG_ERROR(0x15);
    }
    if (address.words.low != ETD_G2Read32(RTL_IDR0) ||
        address.words.high != (Uint16)ETD_G2Read32(RTL_IDR4)) {
        return DIAG_ERROR(0x15);
    }
    SET_LOOPBACK_OR_FAIL(test, result, RTL_RECEIVE_ERRORS);
    ETD_G2Write16(RTL_IMR, 0);
    WRITE_TEST_FRAME(cursor, i, address.mac.bytes, FIRST_PAYLOAD);
    if (mode == RTL_DIAG_LOOPBACK) {
        ETD_G2Write32(RTL_TSD0, TEST_TX_DESCRIPTOR);
        for (wait = 0; wait < DIAG_POLLS; wait++) {
            if (!(ETD_G2Read8(RTL_CR) & RTL_CR_EMPTY)) {
                break;
            }
        }
        if (wait == DIAG_POLLS) {
            return DIAG_ERROR(0x21);
        }
        for (wait = 0; wait < DIAG_POLLS; wait++) {
            if (ETD_G2Read32(RTL_TSD0) & RTL_TSD_OK) {
                break;
            }
        }
        if (wait == DIAG_POLLS) {
            return DIAG_ERROR(0x22);
        }
        status = ETD_G2Read32(RTL_TSD0);
        if (TSD_FAILED(status)) {
            return DIAG_ERROR(0x23);
        }
        if (ETD_G2Read8(RTL_CR) & RTL_CR_EMPTY) {
            return DIAG_ERROR(0x25);
        }
        word = ETD_G2Read16(RTL_ISR);
        if (word & RTL_ISR_SERR) {
            return DIAG_ERROR(0x24);
        }
        if (word & RECEIVE_ERRORS) {
            return DIAG_ERROR(0x25);
        }
        syG2Read((void *)(dl_adapter->rx_position | G2_UNCACHED), &header,
                 SYE_G2_DATA_WIDTH_32BIT, 1, SYE_G2_ADR_MODE_ADD,
                 SYE_G2_ADR_MODE_ADD);
        word = header.status;
        dl_adapter->rx_position += sizeof(header);
        if (!(word & RTL_RX_OK)) {
            if (!(word & 0x1fc0)) {
                return DIAG_ERROR(0x26);
            }
            return DIAG_ERROR(0x27);
        }
        word = header.length - ETHERNET_CRC_BYTES;
        if (word != ETHERNET_FRAME_BYTES) {
            return DIAG_ERROR(0x28);
        }
    }
    if (mode == RTL_DIAG_LOOPBACK) {
        cursor = dl_adapter->rx_position | G2_UNCACHED;
    } else {
        cursor = dl_adapter->tx_buffer[0] | G2_UNCACHED;
    }
    cursor += ETHERNET_HEADER_BYTES;
    for (i = 0; i < ETHERNET_MTU; i++) {
        /* @hack redundant: an unreachable test of mode */
        if (0) {
            if (mode) {
            }
        }
        if (ETD_G2Read8(cursor++) != (Uint8)(i + FIRST_PAYLOAD)) {
            return DIAG_ERROR(0x29);
        }
    }
    SET_LOOPBACK_OR_FAIL(test, result, RTL_RECEIVE_ERRORS);
    dl_receive_test = -1;
    ETD_G2Write16(RTL_IMR, RTL_ISR_ROK | RECEIVE_ERRORS);
    WRITE_TEST_FRAME(cursor, i, address.mac.bytes, SECOND_PAYLOAD);
    if (dl_receive_test != -1) {
        return DIAG_ERROR(0x31);
    }
    if (mode == RTL_DIAG_LOOPBACK) {
        ETD_G2Write32(RTL_TSD0, TEST_TX_DESCRIPTOR);
        for (wait = 0; wait < DIAG_POLLS; wait++) {
            if (dl_receive_test == 0) {
                break;
            }
        }
        if (wait == DIAG_POLLS) {
            return DIAG_ERROR(0x34);
        }
        if (!(ETD_G2Read32(RTL_TSD0) & RTL_TSD_OK)) {
            return DIAG_ERROR(0x32);
        }
        status = ETD_G2Read32(RTL_TSD0);
        if (TSD_FAILED(status)) {
            return DIAG_ERROR(0x33);
        }
        word = ETD_G2Read16(RTL_ISR);
        if (word & RTL_ISR_SERR) {
            return DIAG_ERROR(0x36);
        }
        if (word & RTL_ISR_ROK) {
            return DIAG_ERROR(0x35);
        }
        if (word & RECEIVE_ERRORS) {
            return DIAG_ERROR(0x37);
        }
        if (dl_adapter->stats.count.tx_full ||
            dl_adapter->stats.count.rx_dropped ||
            dl_adapter->stats.count.tx_aborted ||
            dl_adapter->stats.count.rx_too_long ||
            dl_adapter->stats.count.reserved ||
            dl_adapter->stats.count.rx_errors ||
            dl_adapter->stats.count.fifo_overflows ||
            dl_adapter->stats.count.rx_overflows) {
            return DIAG_ERROR(0x51);
        }
        if (dl_adapter->stats.count.tx_packets ||
            dl_adapter->stats.count.rx_packets != 1 ||
            dl_adapter->stats.count.rx_broadcast != 1 ||
            dl_adapter->stats.count.tx_bytes ||
            dl_adapter->stats.count.rx_bytes != ETHERNET_FRAME_BYTES) {
            return DIAG_ERROR(0x52);
        }
    } else {
        cursor = dl_adapter->tx_buffer[0] | G2_UNCACHED;
        cursor += ETHERNET_HEADER_BYTES;
        for (i = 0; i < ETHERNET_MTU; i += 2) {
            word = ETD_G2Read16(cursor);
            expected = i + SECOND_PAYLOAD;
            if ((Uint8)word != expected ||
                (Uint8)(word >> 8) != (Uint8)(expected + 1)) {
                return DIAG_ERROR(0x38);
            }
            cursor += 2;
        }
    }
    result = DLD_SetCtlPara(&saved);
    if (result != 0) {
        return DIAG_ERROR(0x16);
    }
    return 0;
}
