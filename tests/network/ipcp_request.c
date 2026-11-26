/* The SDK's IPCP compression option occupies six bytes for either method. */
#include <assert.h>
#include <string.h>
#include "ppp_ipcp.h"
#include "ppp_link.h"
#include "ppp_packet.h"

static struct ppp_buffer frame;
static Uint8 payload[PPP_FRAME_DATA_BYTES];
static int allocations;
static int available = 1;

struct ppp_buffer *PP_alloc_mbuf_work(Uint16 index)
{
    assert(index == 0);
    allocations++;
    if (!available) {
        return NULL;
    }
    frame.data = payload;
    frame.length = 0;
    return &frame;
}

int main(void)
{
    static const Uint8 van_jacobson[6] = {2, 6, 0, 0x2d, 15, 1};
    static const Uint8 other_method[6] = {2, 6, 0, 0x37, 0xff, 0xff};
    struct ppp_ipcp state = {0};
    struct ppp_fsm fsm = {0};
    Uint8 method_bytes[2] = {0x12, 0x34};
    struct ppp_buffer method = {method_bytes, sizeof(method_bytes), 0};
    struct ppp_buffer *method_cursor = &method;
    struct ppp_buffer *packet;

    ipcp_option_lengths[IPCP_COMPRESS] = sizeof(van_jacobson);
    fsm.context = &state;
    state.local.work.negotiate = 1 << 2;
    state.local.work.slots = 16;
    state.local.work.slot_compression = 1;

    state.local.work.compression = 0x2d;
    packet = ipcp_make_request(&fsm);
    assert(packet == &frame && packet->length == sizeof(van_jacobson));
    assert(!memcmp(packet->data, van_jacobson, sizeof(van_jacobson)));

    state.local.work.compression = 0x37;
    packet = ipcp_make_request(&fsm);
    assert(packet == &frame && packet->length == sizeof(other_method));
    assert(!memcmp(packet->data, other_method, sizeof(other_method)));

    frame.length = sizeof(payload) - 6;
    ipcp_emit_option(&packet, &state.local.work, IPCP_COMPRESS, 6,
                     &method_cursor);
    assert(packet == &frame && frame.length == sizeof(payload));
    assert(!memcmp(payload + sizeof(payload) - 2, method_bytes, 2));
    assert(method.length == 0 && method.data == method_bytes + 2);

    frame.length = sizeof(payload) - 5;
    ipcp_write_options(&packet, &state.local.work, state.local.work.negotiate);
    assert(!packet && frame.length == sizeof(payload) - 5);

    available = 0;
    assert(!ipcp_make_request(&fsm));
    assert(allocations == 3);
    return 0;
}
