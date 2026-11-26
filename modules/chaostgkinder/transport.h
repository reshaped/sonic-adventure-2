/* the connection each of the REGISTER, ADDRESS and ACCOUNT services opens to
 * its server. it carries SA2DTP, the protocol the services' greetings name.
 * its stream holds frames as packet.h lays them out, each one packet. a
 * service's session holds its transport first, so the session's dispatch
 * can cast the transport to the session */
#ifndef CHAOSTGKINDER_TRANSPORT_H
#define CHAOSTGKINDER_TRANSPORT_H
#include <sg_xpt.h>
#include "chaostgkinder/packet_cipher.h"

struct kinder_packet;
enum transport_state {
    TRANSPORT_IDLE,
    TRANSPORT_OPENING,
    TRANSPORT_OPEN,
    TRANSPORT_FAILED
};

/* a service sets address, port, decode and dispatch, and reads state. decode
 * makes the packet of a whole frame, or returns NULL to drop the frame.
 * dispatch takes each packet, which the transport frees once dispatch
 * returns. used counts the bytes of frame gathered so far */
struct packet_transport {
    int state; /* a transport_state */
    Uint32 address;
    Uint16 port;
    Sint16 handle; /* negative where kinder_tcp_open refused */
    Uint8 send_buffer[1024], receive_buffer[1024];
    Uint8 frame[4096];
    int used;
    Bool encrypted;
    struct packet_cipher send_cipher, receive_cipher;
    struct kinder_packet *(*decode)(Uint8 *);
    void (*dispatch)(struct packet_transport *, struct kinder_packet *);
};

/* answer a ping with a pong, and close the transport on a disconnect. every
 * other packet changes nothing */
void transport_dispatch(struct packet_transport *transport,
                        struct kinder_packet *packet);
/* take one step toward TRANSPORT_OPEN. from TRANSPORT_IDLE it starts a TCP
 * connection to address and port, and moves to TRANSPORT_OPENING. from there
 * it moves to TRANSPORT_OPEN once the connection is TCP_CONNECTED. it moves
 * to TRANSPORT_FAILED where kinder_tcp_open refuses, or once the connection
 * is TCP_CLOSING or TCP_FAILED. it does nothing in the other states */
void transport_open(struct packet_transport *transport);
/* close the connection, unless the transport is TRANSPORT_IDLE, and leave it
 * TRANSPORT_IDLE. it keeps used, encrypted and the ciphers, so a transport
 * opened again starts with them.
 *
 * NONMATCHING closes no negative handle, and leaves handle -1 and used and
 * encrypted clear */
void transport_close(struct packet_transport *transport);
/* read what the connection holds, and dispatch the packet of each frame it
 * completes. it returns FALSE when the connection isn't TCP_CONNECTED, and
 * TRUE otherwise. nothing tests a frame's length, which must lie from
 * PACKET_HEADER_SIZE to the size of frame. past its two length bytes, a
 * frame must arrive in one read. once a dispatch starts the cipher, the rest
 * of the read is deciphered too.
 *
 * NONMATCHING lets a frame span reads, and closes the transport on a wrong
 * length. it also returns FALSE where kinder_tcp_receive gives no bytes, or
 * once the transport isn't open */
Bool transport_receive(struct packet_transport *transport);
/* send packet, enciphered once the cipher has started. nothing tests that
 * its encoding fits send_buffer. NONMATCHING sends no packet whose size
 * passes send_buffer's */
void transport_send(struct packet_transport *transport,
                    struct kinder_packet *packet);
/* seed the receive cipher with receive_seed and the send cipher with
 * send_seed. from then on the transport enciphers what it sends and
 * deciphers what it reads */
void transport_start_cipher(struct packet_transport *transport,
                            Uint32 receive_seed, Uint32 send_seed);
#endif
