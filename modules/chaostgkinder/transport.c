#include <sg_xpt.h>
#ifdef NONMATCHING
/* memcpy, with which this side's receive copies a frame */
#include <string.h>
#endif
#include "chaostgkinder/network.h"
#include "chaostgkinder/packet.h"
#include "chaostgkinder/packet_cipher.h"
#include "chaostgkinder/transport.h"

/* a frame's length field, which starts it, low byte first */
#define LENGTH_BYTES 2

/* @hack type: external linkage for transport_pong and the handlers, which
 * no other source calls, where static would do */
/* NOLINTBEGIN(misc-use-internal-linkage) */
void transport_pong(struct packet_transport *transport, Uint32 cookie)
{
    struct ping_packet pong;
    pong_packet_init(&pong);
    pong.cookie = cookie;
    transport_send(transport, &pong.header);
}
void transport_handle_empty(struct packet_transport *transport,
                            struct kinder_packet *packet)
{
}
void transport_handle_disconnect(struct packet_transport *transport,
                                 struct kinder_packet *packet)
{
    transport_close(transport);
}
void transport_handle_notice(struct packet_transport *transport,
                             struct kinder_packet *packet)
{
}
void transport_handle_ping(struct packet_transport *transport,
                           struct kinder_packet *packet)
{
    transport_pong(transport, ((struct ping_packet *)packet)->cookie);
}
void transport_handle_pong(struct packet_transport *transport,
                           struct kinder_packet *packet)
{
}
void transport_handle_greeting(struct packet_transport *transport,
                               struct kinder_packet *packet)
{
}

/* NOLINTEND(misc-use-internal-linkage) */

void transport_dispatch(struct packet_transport *transport,
                        struct kinder_packet *packet)
{
    switch (packet->kind) {
    case PACKET_EMPTY:
        transport_handle_empty(transport, packet);
        break;
    case PACKET_DISCONNECT:
        transport_handle_disconnect(transport, packet);
        break;
    case PACKET_NOTICE:
        transport_handle_notice(transport, packet);
        break;
    case PACKET_PING:
        transport_handle_ping(transport, packet);
        break;
    case PACKET_PONG:
        transport_handle_pong(transport, packet);
        break;
    case PACKET_GREETING:
        transport_handle_greeting(transport, packet);
        break;
    }
}

void transport_open(struct packet_transport *transport)
{
    struct kinder_tcp_status status;
    switch (transport->state) {
    case TRANSPORT_IDLE:
        /* @hack spelling: the assignment inside the test, where a statement
         * of its own would do */
        if ((transport->handle =
                 kinder_tcp_open(transport->address, transport->port)) < 0) {
            transport->state = TRANSPORT_FAILED;
        } else {
            transport->state = TRANSPORT_OPENING;
        }
        break;
    case TRANSPORT_OPENING:
        kinder_tcp_status(transport->handle, &status);
        switch (status.state) {
        case TCP_CLOSED:
        case TCP_CONNECTING:
            break;
        case TCP_CONNECTED:
            transport->state = TRANSPORT_OPEN;
            break;
        case TCP_CLOSING:
        case TCP_FAILED:
        default:
            transport->state = TRANSPORT_FAILED;
            break;
        }
        break;
    }
}

void transport_close(struct packet_transport *transport)
{
    switch (transport->state) {
    case TRANSPORT_OPENING:
    case TRANSPORT_OPEN:
    case TRANSPORT_FAILED:
#ifdef NONMATCHING
        /* this side closes no handle it never opened */
        if (transport->handle < 0) {
            break;
        }
#endif
        kinder_tcp_close(transport->handle);
    }
    transport->state = TRANSPORT_IDLE;
#ifdef NONMATCHING
    /* this side leaves the transport as new: no handle, no bytes held, no
     * cipher */
    transport->handle = -1;
    transport->used = 0;
    transport->encrypted = FALSE;
#endif
}

Bool transport_receive(struct packet_transport *transport)
{
    struct kinder_tcp_status status;
    Uint8 *input;
    Uint8 *frame;
    Uint8 *destination;
    int count;
    int offset;
    int used;
    int needed;
    int remaining;
    int frame_length;
    struct kinder_packet *packet;
    Bool was_encrypted;
    /* @hack spelling: chunk, which each copy assigns among its arguments,
     * where the copies could pass needed and remaining */
    int chunk;
#ifdef NONMATCHING
#else
    /* memcpy unprototyped, so the copy of a frame that runs past the read
     * can pass it two arguments. that copy is a bug, tagged where it
     * stands */
    /* NOLINTNEXTLINE(clang-diagnostic-strict-prototypes) */
    extern void *memcpy();
#endif
    kinder_tcp_status(transport->handle, &status);
    if (status.state != TCP_CONNECTED) {
        return FALSE;
    }
    if (status.receive_size > 0) {
        input = transport->receive_buffer;
        frame = transport->frame;
        count = kinder_tcp_receive(transport->handle, input,
                                   sizeof(transport->receive_buffer));
#ifdef NONMATCHING
        /* this side fails a read of nothing, or of more than the buffer
         * holds */
        if (count <= 0 ||
            (unsigned int)count > sizeof(transport->receive_buffer)) {
            return FALSE;
        }
#endif
        if (transport->encrypted) {
            packet_cipher_apply(&transport->receive_cipher, input, count);
        }
        used = transport->used;
        offset = 0;
        while (offset != count) {
            if (used < LENGTH_BYTES) {
                if (used == 0) {
                    frame[0] = input[offset];
                    if (++offset == count) {
                        used = 1;
                        break;
                    }
                }
                frame[1] = input[offset];
                used = LENGTH_BYTES;
                if (++offset == count) {
                    break;
                }
            }
            frame_length = PACKET_FRAME_SIZE(frame);
#ifdef NONMATCHING
            /* this side closes the transport on a frame shorter than its header
             * or longer than frame. the copy below would pass such a frame's
             * bounds */
            if (frame_length < PACKET_HEADER_SIZE ||
                (unsigned int)frame_length > sizeof(transport->frame)) {
                transport_close(transport);
                return FALSE;
            }
#endif
            remaining = count - offset;
            /* @hack spelling: the two assignments joined by a comma, where
             * two statements would do */
            needed = frame_length - used, destination = frame + used;
            if (remaining >= needed) {
                /* NOLINTNEXTLINE(clang-diagnostic-deprecated-non-prototype) */
                memcpy(destination, input + offset, chunk = needed);
                offset += chunk;
                /* @hack spelling: the assignment inside the test, where a
                 * statement of its own would do */
                if ((packet = transport->decode(frame)) != 0) {
                    was_encrypted = transport->encrypted;
                    transport->dispatch(transport, packet);
                    packet_free(packet);
                    if (!was_encrypted && transport->encrypted) {
                        packet_cipher_apply(&transport->receive_cipher,
                                            input + offset, count - offset);
                    }
#ifdef NONMATCHING
                    /* this side stops where the dispatch closed or failed the
                     * transport. the loop would go on reading its buffers */
                    if (transport->state != TRANSPORT_OPEN) {
                        return FALSE;
                    }
#endif
                }
                used = 0;
            } else {
                /* @bug memory: copies a frame that runs past the read from
                 * address remaining, which is a count, for a length no
                 * argument gives. the call passes memcpy two of its three
                 * arguments (0x0c9d4df4) */
#ifdef NONMATCHING
                /* this side repairs the bug tagged above: memcpy takes the
                 * source and the count */
                chunk = remaining;
                memcpy(destination, input + offset, (Uint32)chunk);
#else
                /* NOLINTNEXTLINE(clang-diagnostic-deprecated-non-prototype) */
                memcpy(destination, chunk = remaining);
#endif
                used += chunk;
                break;
            }
        }
        transport->used = used;
    }
    return TRUE;
}

void transport_send(struct packet_transport *transport,
                    struct kinder_packet *packet)
{
    Uint8 *buffer = transport->send_buffer;
    int count;
#ifdef NONMATCHING
    /* this side sends nothing larger than the send buffer */
    if (packet->size > sizeof(transport->send_buffer)) {
        return;
    }
#endif
    count = packet->codec->write(packet, buffer) - buffer;
    if (transport->encrypted) {
        packet_cipher_apply(&transport->send_cipher, buffer, count);
    }
    kinder_tcp_send(transport->handle, buffer, count);
}

void transport_start_cipher(struct packet_transport *transport,
                            Uint32 receive_seed, Uint32 send_seed)
{
    packet_cipher_seed(&transport->receive_cipher, receive_seed);
    packet_cipher_seed(&transport->send_cipher, send_seed);
    transport->encrypted = TRUE;
}
