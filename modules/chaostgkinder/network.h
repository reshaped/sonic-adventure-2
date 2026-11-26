/* the kindergarten's network: a PPP connection through the modem, a resolver
 * and TCP sockets. the connection goes to the ISP the flash's network
 * information selects */
#ifndef CHAOSTGKINDER_NETWORK_H
#define CHAOSTGKINDER_NETWORK_H
#include <sg_xpt.h>

enum kinder_connection_state {
    CONNECTION_IDLE,
    CONNECTION_DIALING,
    CONNECTION_NEGOTIATING,
    CONNECTION_ONLINE,
    CONNECTION_DISCONNECTED,
    CONNECTION_CLOSING,
    CONNECTION_UNKNOWN
};
#define ISP_ACCOUNTS 2
#define ISP_PHONE_NUMBERS 3

/* an ISP's account as the flash holds it, each string ended by a NUL. the
 * DNS servers' addresses are in network byte order */
struct isp_account {
    char login[32], password[24], mail[64], phone[ISP_PHONE_NUMBERS][48];
    Uint32 primary_dns, secondary_dns;
};
/* kinder_network_start reads ISP 1's and ISP 2's accounts. kinder_isp_result
 * is then NTD_OK, or the error that stopped the read, which leaves the
 * accounts as they were. kinder_selected_isp is the index of the account the
 * information selects */
extern Sint32 kinder_isp_result;
extern struct isp_account kinder_isp_accounts[ISP_ACCOUNTS];
extern Sint32 kinder_selected_isp;

/* write the console's individual ID as eight characters and a NUL, into the
 * 9 bytes at text */
void kinder_console_id(char *text);

/* kinder_network_start clears the overlay's BSS, runs its constructors,
 * reads the ISP accounts and starts PPP and TCP. kinder_network_stop closes
 * the resolver, where it started, then stops TCP and PPP and runs the
 * overlay's destructors */
void kinder_network_start(void);
void kinder_network_stop(void);
/* dial the selected account's phone number at index number, below
 * ISP_PHONE_NUMBERS. kinder_connection_number returns that number */
void kinder_connection_dial(int number);
/* hang up a connection kinder_connection_dial opened. returns AvepppClose's
 * result, or 0 where no connection was dialed */
int kinder_connection_hang_up(void);
enum kinder_connection_state kinder_connection_state(void);
const char *kinder_connection_number(int number);
/* start the resolver, once the connection is online. it asks the DNS servers
 * PPP gave, or the selected account's where PPP gave none */
void kinder_resolver_start(void);
/* start looking up name's address, and return the lookup's ticket. a dotted
 * address takes a numeric ticket, which asks no server, while one is free. a
 * negative ticket means the lookup didn't start. the caller holds a ticket
 * until kinder_lookup_release */
int kinder_lookup_start(const char *name);
void kinder_lookup_release(Sint32 ticket);
/* kinder_lookup_poll's results. where it returns KINDER_LOOKUP_DONE, the poll
 * has written the address's four bytes at address */
enum kinder_lookup_status {
    KINDER_LOOKUP_DONE,
    KINDER_LOOKUP_WAITING,
    KINDER_LOOKUP_FAILED
};
enum kinder_lookup_status kinder_lookup_poll(Sint32 ticket, Uint32 *address);
/* a service session's step through a lookup: it starts one, then polls it */
enum kinder_lookup_step { LOOKUP_START, LOOKUP_POLL };
enum kinder_tcp_state {
    TCP_CLOSED,
    TCP_CONNECTING,
    TCP_CONNECTED,
    TCP_CLOSING,
    TCP_FAILED
};
struct kinder_tcp_status {
    int state;
    Uint16 send_size; /* the bytes the socket can take to send */
    Uint16 receive_size; /* the bytes received and not yet read */
};
/* open a connection to port at address, both in network byte order, and
 * return its handle. a negative handle means the stack refused it */
Sint16 kinder_tcp_open(Uint32 address, Uint16 port);
void kinder_tcp_close(Sint16 handle);
void kinder_tcp_status(Sint16 handle, struct kinder_tcp_status *status);
/* receive up to count bytes into buffer, or send count bytes from it. each
 * returns the stack's result: the bytes it took, or a negative error */
int kinder_tcp_receive(Sint16 handle, Uint8 *buffer, int count);
int kinder_tcp_send(Sint16 handle, Uint8 *buffer, int count);
/* what kinder_dialer_open's *result holds */
enum kinder_dialer_reply { DIALER_CONNECTED, DIALER_CANCELLED, DIALER_PENDING };
/* ask the player to connect, and dial. *result holds DIALER_PENDING until
 * the modem connects, then DIALER_CONNECTED, or until the player cancels,
 * then DIALER_CANCELLED. where the modem is dialing or connected already,
 * *result holds DIALER_CONNECTED at once. result must stay valid until it
 * holds one of those two */
void kinder_dialer_open(Sint32 *result);
#endif
