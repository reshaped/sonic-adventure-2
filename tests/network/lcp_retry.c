/* Exercise LCP retry accounting, option bounds and negotiated authentication
 * roles. */
#include <assert.h>
#include <string.h>
#include "ppp_lcp.c"

static Uint8 reply_bytes[PPP_FRAME_DATA_BYTES];
static struct ppp_buffer reply;
static short send_result;
static int sends;

struct ppp_buffer *PP_alloc_mbuf_work(Uint16 index)
{
    (void)index;
    reply.data = reply_bytes;
    reply.length = 0;
    return &reply;
}

short PP_fsm_send(struct ppp_fsm *fsm, Uint8 code, Uint8 id,
                  struct ppp_buffer *packet)
{
    static const Uint8 proposed_mru[] = {LCP_MRU, 4, 0x05, 0x78};
    (void)fsm;
    assert(code == PPP_CONFIG_NAK && id == 19);
    assert(packet->length == sizeof(proposed_mru));
    assert(!memcmp(packet->data, proposed_mru, sizeof(proposed_mru)));
    sends++;
    return send_result;
}

long PP_Clock(void)
{
    return 0;
}

static void check_option_capacity(void)
{
    static const Uint8 expected[5] = {LCP_AUTHENTICATION, 5, 0xc2, 0x23, 5};
    struct ppp_lcp_options settings = {0};
    struct ppp_buffer *packet = PP_alloc_mbuf_work(0);
    lcp_option_lengths[LCP_AUTHENTICATION] = sizeof(expected) - 1;
    settings.authentication = PPP_CHAP_PROTOCOL;
    settings.chap_algorithm = CHAP_MD5;
    packet->length = sizeof(reply_bytes) - sizeof(expected);
    lcp_write_options(&packet, &settings, LCP_MASK(LCP_AUTHENTICATION));
    assert(packet == &reply && packet->length == sizeof(reply_bytes));
    assert(!memcmp(reply_bytes + sizeof(reply_bytes) - sizeof(expected),
                   expected, sizeof(expected)));
    packet->length = sizeof(reply_bytes) - sizeof(expected) + 1;
    lcp_write_options(&packet, &settings, LCP_MASK(LCP_AUTHENTICATION));
    assert(!packet &&
           reply.length == sizeof(reply_bytes) - sizeof(expected) + 1);
}

enum link_action {
    NO_ACTION,
    PAP_LOCAL,
    PAP_REMOTE,
    CHAP_LOCAL,
    CHAP_REMOTE,
    READ_CLOCK,
    START_IPCP,
    CLOSE_LCP
};

static const enum link_action *next_action;
static struct ppp_session *opening_session;

static void expect_action(enum link_action action)
{
    assert(*next_action == action);
    ++next_action;
}

static void authenticate(struct ppp_session *session, enum link_action action,
                         Uint8 flag)
{
    assert(session == opening_session);
    assert(session->phase == PPP_AUTH);
    assert(session->peer_name == NULL);
    expect_action(action);
    session->flags |= flag;
}

void PP_pap_local(struct ppp_session *session)
{
    authenticate(session, PAP_LOCAL, PPP_AUTH_LOCAL);
}

short PP_pap_remote(struct ppp_session *session)
{
    authenticate(session, PAP_REMOTE, PPP_AUTH_REMOTE);
    return 0;
}

short PP_chap_local(struct ppp_session *session)
{
    authenticate(session, CHAP_LOCAL, PPP_AUTH_LOCAL);
    return 0;
}

short PP_chap_remote(struct ppp_session *session)
{
    authenticate(session, CHAP_REMOTE, PPP_AUTH_REMOTE);
    return 0;
}

long PP_secclock(void)
{
    expect_action(READ_CLOCK);
    return 17;
}

void PP_fsm_start(struct ppp_fsm *fsm)
{
    assert(fsm == &opening_session->fsm[PPP_IPCP]);
    assert(opening_session->phase == PPP_READY);
    assert(opening_session->up_since == 17);
    expect_action(START_IPCP);
}

void PP_fsm_close(struct ppp_fsm *fsm)
{
    assert(fsm == &opening_session->fsm[PPP_LCP]);
    expect_action(CLOSE_LCP);
}

static void check_authentication_roles(void)
{
    static const struct {
        Uint16 local, remote;
        enum link_action actions[3];
    } cases[] = {
        {0, 0, {READ_CLOCK, START_IPCP}},
        {0, PPP_PAP_PROTOCOL, {PAP_REMOTE}},
        {0, PPP_CHAP_PROTOCOL, {CHAP_REMOTE}},
        {PPP_PAP_PROTOCOL, 0, {PAP_LOCAL}},
        {PPP_PAP_PROTOCOL, PPP_PAP_PROTOCOL, {PAP_LOCAL, PAP_REMOTE}},
        {PPP_PAP_PROTOCOL, PPP_CHAP_PROTOCOL, {PAP_LOCAL, CHAP_REMOTE}},
        {PPP_CHAP_PROTOCOL, 0, {CHAP_LOCAL}},
        {PPP_CHAP_PROTOCOL, PPP_PAP_PROTOCOL, {CHAP_LOCAL, PAP_REMOTE}},
        {PPP_CHAP_PROTOCOL, PPP_CHAP_PROTOCOL, {CHAP_LOCAL, CHAP_REMOTE}},
        {PPP_LCP_PROTOCOL, PPP_PAP_PROTOCOL, {CLOSE_LCP}},
        {PPP_PAP_PROTOCOL, PPP_LCP_PROTOCOL, {PAP_LOCAL, CLOSE_LCP}}};
    size_t i;

    for (i = 0; i < sizeof(cases) / sizeof(cases[0]); i++) {
        struct ppp_session session = {0};
        struct ppp_lcp lcp = {0};
        struct ppp_fsm *fsm = &session.fsm[PPP_LCP];
        char old_peer[] = "old peer";

        opening_session = &session;
        next_action = cases[i].actions;
        session.phase = PPP_LINK;
        session.flags = PPP_ACTIVE | PPP_AUTH_LOCAL | PPP_AUTH_REMOTE;
        session.peer_name = old_peer;
        fsm->session = &session;
        fsm->context = &lcp;
        if (cases[i].local) {
            lcp.local.work.negotiate = LCP_MASK(LCP_AUTHENTICATION);
            lcp.local.work.authentication = cases[i].local;
        }
        if (cases[i].remote) {
            lcp.remote.work.negotiate = LCP_MASK(LCP_AUTHENTICATION);
            lcp.remote.work.authentication = cases[i].remote;
        }
        lcp_opening(fsm);
        assert(*next_action == NO_ACTION);
        assert(session.peer_name == NULL);
        assert(session.flags & PPP_ACTIVE);
    }
    opening_session = NULL;
}

int main(void)
{
    static Uint8 requested_mru[] = {LCP_MRU, 4, 0x05, 0xdc};
    struct ppp_control_header header = {PPP_CONFIG_REQ, 19, 4};
    struct ppp_buffer request = {requested_mru, 4, 0};
    struct ppp_lcp lcp = {0};
    struct ppp_lcp original;
    struct ppp_fsm fsm = {0};

    lcp_option_lengths[LCP_MRU] = sizeof(requested_mru);
    lcp.remote.will_negotiate = LCP_MASK(LCP_MRU);
    lcp.remote.want.negotiate = LCP_MASK(LCP_MRU);
    lcp.remote.want.mru = 1400;
    memcpy(&original, &lcp, sizeof(original));
    fsm.context = &lcp;
    fsm.nak_retries = 2;

    send_result = -1;
    assert(lcp_request(&fsm, &header, &request) == -1);
    assert(sends == 1);
    assert(fsm.nak_retries == 2);
    assert(!memcmp(&original, &lcp, sizeof(original)));

    send_result = 0;
    assert(lcp_request(&fsm, &header, &request) == 1);
    assert(sends == 2);
    assert(fsm.nak_retries == 1);
    assert(lcp.remote.work.mru == 1400);
    assert(request.data == requested_mru && request.length == 4);
    check_option_capacity();
    check_authentication_roles();
    return 0;
}
