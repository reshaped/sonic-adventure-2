#include <access/aveppp.h>
#include <sg_xpt.h>
#include <string.h>
#include "net_debug.h"
#include "ppp_driver.h"
#include "ppp_framing.h"
#include "ppp_fsm.h"
#include "ppp_ipcp.h"
#include "ppp_lcp.h"
#include "ppp_link.h"
#include "ppp_modem.h"
#include "ppp_packet.h"
#include "ppp_protocols.h"
#include "ppp_serial.h"
#include "ppp_system.h"

enum {
    HDLC_ADDRESS = 255,
    HDLC_CONTROL = 3,
    HDLC_FLAG = 0x7e,
    HDLC_ESCAPE = 0x7d,
    HDLC_ESCAPE_BIT = 0x20,
    HDLC_HEADER = 4, /* the address, the control and a two-byte protocol */
    HDLC_ADDRESS_CONTROL_BYTES = 2,
    HDLC_CONTROL_CHARACTERS = 32, /* the bytes an ACCM can ask to escape */
    LCP_ACCM_BIT = 1 << LCP_ACCM,
    LCP_PROTOCOL_COMPRESSION_BIT = 1 << LCP_PROTOCOL_COMPRESSION,
    LCP_ADDRESS_COMPRESSION_BIT = 1 << LCP_ADDRESS_COMPRESSION,
    FCS_INITIAL = 0xffff,
    FCS_GOOD = 0xf0b8, /* the FCS over a frame that ends in its own FCS */
    FCS_BYTES = 2,
    TRACE_BYTES = 8192,
    FRAME_BYTES = PPP_FRAME_DATA_BYTES,
    DUMP_BYTES = 64,
    DUMP_ROW_BYTES = 16,
    ASCII_SPACE = 32,
    ASCII_DELETE = 127
};
/* the escape map of LCP's frames, and of the others until LCP negotiates
 * one: every control character */
#define ACCM_ALL 0xffffffff
struct hdlc_header {
    Uint8 address, control;
    Uint16 protocol;
};
struct encoded_frame {
    struct ppp_buffer packet;
    Uint8 headroom[16];
#ifdef NONMATCHING
    /* each byte of the data and the two-byte FCS can take an escape. a flag
     * opens and closes the frame */
    Uint8 bytes[2 * FRAME_BYTES + 6];
#else
    Uint8 bytes[4096];
#endif
};
struct input_frame {
    struct ppp_buffer packet;
    Uint8 headroom[PPP_HEADROOM];
    Uint8 bytes[FRAME_BYTES];
};
#pragma section ATA
static char packet_format[16] = "--- %s(%d):\n";
static char offset_format[12] = "  %04x: ";
static char missing_octet[4] = "   ";
static char byte_format[8] = "%02x ";
static char open_bracket[4] = "[";
static char missing_character[4] = "  ";
static char character_format[4] = "%c ";
static char close_bracket[] = "]\n";
#pragma section
static Uint8 pull_word[4];
static Uint8 pull_byte;
/* the frames sent and the bytes received, each appended while it fits in
 * TRACE_BYTES */
static Uint8 send_trace[TRACE_BYTES], receive_trace[TRACE_BYTES];
static Uint8 *send_trace_at, *receive_trace_at;
static struct encoded_frame encoded_frame;
static struct input_frame input_frame;
static Uint8 serial_input[256];
static int trace_initialized = 0;
static struct ppp_buffer *receiving = NULL;
static Uint8 *serial_at = serial_input;
static Uint8 *serial_end = serial_input;
static Uint16 receive_fcs = FCS_INITIAL;
static int escape_pending = 0;

#ifdef NONMATCHING
/* this side pulls every octet through PP_pullup */
static int pull_octet(struct ppp_buffer **packet)
{
    if (PP_pullup(packet, &pull_byte, 1) != 1) {
        return -1;
    }
    return pull_byte;
}
/* the plain form the hack tag below gives */
#define ACCM_ONE 1U
/* this side repairs the bug tagged below: the test is >= */
#define PAST_PACKET(index, length) ((index) >= (length))
#else
/* @hack type: a signed 1, whose shift by 31 leaves int's range, where 1U
 * would do */
#define ACCM_ONE 1
/* @bug memory: dumps data[length], the byte past the packet, where the
 * packet's last row has room for it. the test is > where >= is meant
 * (0x0c9e13e0) */
#define PAST_PACKET(index, length) ((index) > (length))
#endif
/* whether byte goes escaped. a flag and an escape always do. a control
 * character does where its bit is set in mask, an ACCM */
#define MUST_ESCAPE(byte, mask)                                                \
    (((byte) < HDLC_CONTROL_CHARACTERS && ((ACCM_ONE << (byte)) & (mask))) ||  \
     (byte) == HDLC_ESCAPE || (byte) == HDLC_FLAG)
#define FRAME_FCS(crc, byte) (((crc) >> 8) ^ PP_fcstab[((crc) ^ (byte)) & 255])

void DumpPacket(const char *name, Uint16 length, const Uint8 *data)
{
    int i;
    int j;
    int c;
    if (!(dbDebug & DB_DEBUG_DUMP)) {
        return;
    }
    dbPrint(DB_DEBUG_PPP, packet_format, name, length);
    for (i = 0; i < (length > DUMP_BYTES ? DUMP_BYTES : length);
         i += DUMP_ROW_BYTES) {
        dbPrint(DB_DEBUG_PPP, offset_format, i);
        for (j = 0; j < DUMP_ROW_BYTES; j++) {
            if (PAST_PACKET(i + j, length)) {
                dbPrint(DB_DEBUG_PPP, missing_octet);
            } else {
                dbPrint(DB_DEBUG_PPP, byte_format, data[i + j]);
            }
        }
        dbPrint(DB_DEBUG_PPP, open_bracket, i);
        for (j = 0; j < DUMP_ROW_BYTES; j++) {
            if (PAST_PACKET(i + j, length)) {
                dbPrint(DB_DEBUG_PPP, missing_character);
            } else {
                c = data[i + j];
                dbPrint(DB_DEBUG_PPP, character_format,
                        c > ASCII_SPACE && c < ASCII_DELETE ? c : '.');
            }
        }
        dbPrint(DB_DEBUG_PPP, close_bracket);
    }
}
static struct ppp_buffer *prepend_header(const struct hdlc_header *header,
                                         struct ppp_buffer *packet)
{
    Uint8 *data;
    packet = PP_pushdown(packet, HDLC_HEADER);
#ifdef NONMATCHING
    /* this side repairs the bug tagged below: it returns the NULL, which
     * ppp_output tests for */
    if (!packet) {
        return NULL;
    }
#endif
    /* @bug memory: reads through the NULL PP_pushdown returns where packet
     * has no room. ppp_output tests for that NULL (0x0c9e149c, 0x0c9e1896) */
    data = packet->data;
    *data++ = header->address;
    *data++ = header->control;
    PP_put16(data, header->protocol);
    return packet;
}
static int write_frame(struct ppp_buffer *packet)
{
    struct ppp_session *session;
    struct ppp_lcp *lcp;
    struct hdlc_header header;
    unsigned long escape;
    Uint16 crc;
    int header_size;
    int is_lcp;
    int full_protocol;
    int full_address;
    int c;
    Uint8 *out;
    struct ppp_buffer *encoded;
    struct encoded_frame *frame;
    Uint8 *bytes;

    session = &PP_ppp_str;
    lcp = session->fsm[PPP_LCP].context;
    crc = FCS_INITIAL;
    escape = ACCM_ALL;
    header_size = HDLC_HEADER;
#ifdef NONMATCHING
    /* this side counts a send error for a missing packet, a missing LCP
     * context, and a packet past FRAME_BYTES */
    if (!packet || !lcp || !packet->data || packet->length > FRAME_BYTES) {
        session->send_status++;
        return -1;
    }
#endif
    session->octets_sent += PP_len_p(packet) + 2;
    if (!packet || packet->length < HDLC_HEADER) {
        session->send_status++;
        return -1;
    }
    header.address = packet->data[0];
    header.control = packet->data[1];
    header.protocol = PP_get16(packet->data + 2);
    is_lcp = header.protocol == PPP_LCP_PROTOCOL;
    if (!is_lcp && (lcp->remote.work.negotiate & LCP_ACCM_BIT)) {
        escape = lcp->remote.work.accm;
    }
    full_protocol =
        !(!is_lcp &&
          (lcp->remote.work.negotiate & LCP_PROTOCOL_COMPRESSION_BIT) &&
          header.protocol < 255);
    if (!full_protocol) {
        header_size--;
    }
    full_address = !(
        !is_lcp && (lcp->remote.work.negotiate & LCP_ADDRESS_COMPRESSION_BIT));
    if (!full_address) {
        header_size -= HDLC_ADDRESS_CONTROL_BYTES;
    }
    packet->length -= HDLC_HEADER - header_size;
    packet->data += HDLC_HEADER - header_size;
    out = packet->data;
    if (full_address) {
        *out++ = header.address;
        /* @hack spelling: out stepped by statements of their own, here and
         * below, where *out++ would do */
        *out = header.control;
        out = out + 1;
    }
    if (full_protocol) {
        *out = header.protocol >> 8;
        out = out + 1;
    }
    *out = header.protocol;
    /* @hack spelling: the assignment to frame inside encoded's, where a
     * statement of its own would do */
    encoded = &(frame = &encoded_frame)->packet;
    bytes = frame->bytes;
    encoded->data = bytes;
    out = bytes;
    *out++ = HDLC_FLAG;
    /* @hack spelling: the count stepped by an add of 1, where ++ would do */
    session->packets_sent = session->packets_sent + 1;
    while ((c = pull_octet(&packet)) != -1) {
        crc = FRAME_FCS(crc, c);
        if (MUST_ESCAPE(c, escape)) {
            *out++ = HDLC_ESCAPE;
            *out++ = c ^ HDLC_ESCAPE_BIT;
        } else {
            *out++ = c;
        }
    }
    crc ^= FCS_INITIAL;
    c = crc & 255;
    if (MUST_ESCAPE(c, escape)) {
        *out++ = HDLC_ESCAPE;
        *out++ = c ^ HDLC_ESCAPE_BIT;
    } else {
        *out++ = c;
    }
    c = crc >> 8;
    if (MUST_ESCAPE(c, escape)) {
        *out++ = HDLC_ESCAPE;
        *out++ = c ^ HDLC_ESCAPE_BIT;
    } else {
        *out++ = c;
    }
    *out++ = HDLC_FLAG;
    encoded->length = out - encoded->data;
    if (PPP_ConnectionStatus == AvepppConnectOnhooking ||
        PPP_ConnectionStatus == AvepppConnectUnused) {
        return 0;
    }
    if (!trace_initialized) {
        trace_initialized = 1;
        memset(send_trace, 0, TRACE_BYTES);
        memset(receive_trace, 0, TRACE_BYTES);
        send_trace_at = send_trace;
        receive_trace_at = receive_trace;
    }
#ifdef NONMATCHING
    /* this side compares the room left in the trace, where the matching side
     * forms a pointer that can lie past its end */
    if (encoded->length < send_trace + TRACE_BYTES - send_trace_at) {
#else
    if (send_trace_at + encoded->length < send_trace + TRACE_BYTES) {
#endif
        memcpy(send_trace_at, encoded->data, encoded->length);
        send_trace_at += encoded->length;
    }
    return PP_ComWrite(encoded->length, (char *)encoded->data);
}
short ppp_send(struct ppp_buffer *packet)
{
    struct ppp_session *session;
    struct ppp_ipcp *ipcp;
    int protocol = PPP_IPV4;
    /* @hack spelling: the assignment inside the test, where a statement of
     * its own would do */
    if ((session = &PP_ppp_str)->fsm[PPP_IPCP].state != PPP_OPENED) {
        session->send_status++;
        return -1;
    }
    ipcp = session->fsm[PPP_IPCP].context;
    if (ipcp->remote.work.negotiate & IPCP_COMPRESS_BIT) {
        switch (PP_slhc_compress(ipcp->compression, &packet,
                                 ipcp->remote.work.slot_compression)) {
        case VJ_PLAIN_IP:
            protocol = PPP_IPV4;
            break;
        case VJ_DELTA_HEADER:
            protocol = PPP_VJ_COMPRESSED;
            break;
        case VJ_FULL_HEADER:
            protocol = PPP_VJ_UNCOMPRESSED;
            break;
        default:
            session->send_status++;
            return -1;
        }
    }
    return ppp_output(NULL, NULL, protocol, packet);
}
short ppp_output(void *interface, void *destination, Uint16 protocol,
                 struct ppp_buffer *packet)
{
    struct ppp_session *session = &PP_ppp_str;
    struct hdlc_header header;
    struct ppp_buffer *output;
    if (!session->phase) {
        session->send_status++;
        return -1;
    }
    header.address = HDLC_ADDRESS;
    header.control = HDLC_CONTROL;
    header.protocol = protocol;
    output = prepend_header(&header, packet);
    if (!output) {
        session->receive_status++;
        return -1;
    }
    return write_frame(output);
}
void ppp_ini_recv_buff(void)
{
    serial_at = serial_input;
    serial_end = serial_input;
}
int ppp_recv(void)
{
    Uint16 count;
    Uint8 c;
    Uint8 *trace_end = receive_trace + TRACE_BYTES;
    Uint8 *data;
    /* @hack spelling: data set in the loop's first clause, where a statement
     * before the loop would do */
    for (data = input_frame.bytes;;) {
        if (serial_at == serial_end) {
            if (PP_ComRead(sizeof(serial_input), (char *)serial_input,
                           &count) ||
                count < 1) {
                break;
            }
            serial_at = serial_input;
            serial_end = serial_input + count;
            if (!trace_initialized) {
                trace_initialized = 1;
                memset(send_trace, 0, TRACE_BYTES);
                memset(receive_trace, 0, TRACE_BYTES);
                send_trace_at = send_trace;
                receive_trace_at = receive_trace;
            }
#ifdef NONMATCHING
            /* this side compares the room left in the trace, where the matching
             * side forms a pointer that can lie past its end */
            if (count < trace_end - receive_trace_at) {
#else
            if (receive_trace_at + count < trace_end) {
#endif
                memcpy(receive_trace_at, serial_input, count);
                receive_trace_at += count;
            }
        }
        if ((c = *serial_at++) == HDLC_FLAG) {
            if (receiving && receiving->length < FRAME_BYTES &&
                receive_fcs == FCS_GOOD) {
                PP_trim_mbuf(&receiving, PP_len_p(receiving) - FCS_BYTES);
                ppp_proc(receiving);
                receiving = NULL;
                return 1;
            } else if (receiving && receiving->length > 0) {
                /* @hack redundant: the test for a refused frame, whose branch
                 * is empty */
            }
            receiving = NULL;
#ifdef NONMATCHING
            /* this side clears an escape left pending at a frame's end, which
             * the repair below would carry into the next frame */
            escape_pending = 0;
#endif
            continue;
        }
#ifdef NONMATCHING
        /* this side repairs the bug tagged below: the frame starts before an
         * escape that opens it, so the escape applies to the first byte */
        if (!receiving) {
            receiving = &input_frame.packet;
            receiving->data = data;
            receiving->length = 0;
            receive_fcs = FCS_INITIAL;
        }
        if (c == HDLC_ESCAPE && !escape_pending) {
            escape_pending = 1;
            continue;
        }
#else
        if (c == HDLC_ESCAPE) {
            escape_pending |= 1;
            continue;
        }
        if (!receiving) {
            receiving = &input_frame.packet;
            receiving->data = data;
            receiving->length = 0;
            /* @bug logic: a frame whose first byte is escaped takes that byte
             * unescaped. the escape comes before the frame starts, and the
             * start clears it (0x0c9e1a3c, 0x0c9e1a62) */
            escape_pending = 0;
            receive_fcs = FCS_INITIAL;
        }
#endif
        if (escape_pending & 1) {
            escape_pending &= ~1;
            c ^= HDLC_ESCAPE_BIT;
        }
        if (receiving->length < FRAME_BYTES) {
            receiving->data[receiving->length++] = c;
            receive_fcs = FRAME_FCS(receive_fcs, c);
        }
    }
    if (PPP_ConnectionStatus == AvepppConnectAuthen ||
        PPP_ConnectionStatus == AvepppConnectEstablished) {
        if (!PP_ComLineSence()) {
            PPP_ConnectionStatus = AvepppConnectDisconnected;
        }
    }
    return 0;
}
void ppp_proc(struct ppp_buffer *packet)
{
    struct ppp_session *session;
    struct ppp_ipcp *ipcp;
    struct hdlc_header header;
    struct ppp_buffer *rejected;
    if (!packet) {
        return;
    }
#ifdef NONMATCHING
    /* this side drops a packet with no bytes */
    if (!packet->length || !packet->data) {
        return;
    }
#endif
    session = &PP_ppp_str;
    if (packet->data[0] == HDLC_ADDRESS &&
#ifdef NONMATCHING
        /* this side reads the control byte only where the packet holds it */
        packet->length >= HDLC_ADDRESS_CONTROL_BYTES &&
#endif
        packet->data[1] == HDLC_CONTROL) {
        if (PP_pullup(&packet, pull_word, HDLC_ADDRESS_CONTROL_BYTES) ==
            HDLC_ADDRESS_CONTROL_BYTES) {
            PP_get16(pull_word);
        }
    }
    header.address = HDLC_ADDRESS;
    header.control = HDLC_CONTROL;
    header.protocol = pull_octet(&packet);
    /* an odd first byte is the whole protocol, compressed to one byte */
    if (!(header.protocol & 1)) {
        header.protocol = (header.protocol << 8) | pull_octet(&packet);
    }
    switch (header.protocol) {
    case PPP_IPV4:
        if (session->fsm[PPP_IPCP].state != PPP_OPENED) {
            session->receive_errors++;
            break;
        }
        PP_ip_route(packet, 0);
        break;
    case PPP_VJ_COMPRESSED:
        if (session->fsm[PPP_IPCP].state != PPP_OPENED) {
            session->receive_errors++;
            break;
        }
        ipcp = session->fsm[PPP_IPCP].context;
        if (!(ipcp->local.work.negotiate & IPCP_COMPRESS_BIT)) {
            session->receive_errors++;
            break;
        }
        if (PP_slhc_uncompress(ipcp->compression, &packet) <= 0) {
            session->receive_errors++;
            break;
        }
        PP_ip_route(packet, 0);
        break;
    case PPP_VJ_UNCOMPRESSED:
        if (session->fsm[PPP_IPCP].state != PPP_OPENED) {
            session->receive_errors++;
            break;
        }
        ipcp = session->fsm[PPP_IPCP].context;
        if (!(ipcp->local.work.negotiate & IPCP_COMPRESS_BIT)) {
            session->receive_errors++;
            break;
        }
        if (PP_slhc_remember(ipcp->compression, &packet) <= 0) {
            session->receive_errors++;
            break;
        }
        PP_ip_route(packet, 0);
        break;
    case PPP_LCP_PROTOCOL:
        session->received.protocol.lcp++;
        PP_fsm_proc(&session->fsm[PPP_LCP], packet);
        break;
    case PPP_PAP_PROTOCOL:
        if (session->phase != PPP_AUTH && session->phase != PPP_READY) {
            session->receive_errors++;
            if (session->phase == PPP_DEAD) {
                lcp_opening(&session->fsm[PPP_LCP]);
            }
            break;
        }
        session->received.protocol.pap++;
        PP_pap_proc(&session->fsm[PPP_PAP], packet);
        break;
    case PPP_CHAP_PROTOCOL:
        if (session->phase != PPP_AUTH && session->phase != PPP_READY) {
            session->receive_errors++;
            if (session->phase == PPP_DEAD) {
                lcp_opening(&session->fsm[PPP_LCP]);
            }
            break;
        }
        session->received.protocol.chap++;
        PP_chap_proc(&session->fsm[PPP_CHAP], packet);
        break;
    case PPP_IPCP_PROTOCOL:
        if (session->phase != PPP_READY) {
            session->receive_errors++;
            break;
        }
        /* @hack spelling: the count stepped by an add of 1, where ++ would
         * do */
        session->received.protocol.ipcp = session->received.protocol.ipcp + 1;
        PP_fsm_proc(&session->fsm[PPP_IPCP], packet);
        break;
    default:
        session->unknown_protocol++;
        rejected = prepend_header(&header, packet);
        if (!rejected) {
            session->allocation_errors++;
            return;
        }
        if (PP_pullup(&rejected, pull_word, HDLC_ADDRESS_CONTROL_BYTES) ==
            HDLC_ADDRESS_CONTROL_BYTES) {
            PP_get16(pull_word);
        }
        PP_fsm_send(&session->fsm[PPP_LCP], PPP_PROTOCOL_REJ, 0, rejected);
        break;
    }
}
short ppp_iostatus(short status)
{
    struct ppp_session *session = &PP_ppp_str;
    switch (status) {
    case PPP_SERIAL_CONNECT:
        if (session->phase == PPP_DEAD) {
            session->phase = PPP_LINK;
        }
        PP_fsm_start(&session->fsm[PPP_LCP]);
        return 0;
    case PPP_SERIAL_HANG_UP:
        PP_fsm_down(&session->fsm[PPP_LCP]);
        session->phase = PPP_DEAD;
        return 0;
    }
    return -1;
}
short ppp_init(int port)
{
    struct ppp_session *session = &PP_ppp_str;
    memset(session, 0, sizeof(*session));
    session->phase = PPP_DEAD;
    PP_lcp_init(session);
    PP_pap_init(session);
    PP_chap_init(session);
    PP_ipcp_init(session);
    ppp_iostatus(PPP_SERIAL_CONNECT);
    return 0;
}
