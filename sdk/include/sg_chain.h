/* Shinobi interrupt chains, as far as the sources use them */
#ifndef SG_CHAIN_H
#define SG_CHAIN_H
#include <sg_xpt.h>

/* chain events. an event below 0x0900 is a CPU exception or interrupt code.
 * the vertical and horizontal blanks follow, which the Kamui graphics
 * library's sync callbacks deliver at interrupt level 6. then come the
 * system bus's interrupt sources, which arrive at level 4, 0x20 apart. its
 * normal interrupts start at 0x0a00, and its external ones at 0x0b00 */
#define SYD_CHAIN_EVENT_IML6_VBLANK 0x0900
#define SYD_CHAIN_EVENT_IML6_HBLANK 0x0920
#define SYD_CHAIN_EVENT_IML4_NRM 0x0a00
/* the first two normal interrupts: a Maple DMA's end, and a vertical blank
 * during a Maple transfer */
#define SYD_CHAIN_EVENT_IML4_MPDMAE 0x0a00
#define SYD_CHAIN_EVENT_IML4_MPVOVR 0x0a20
/* the GD-ROM drive's DMA end */
#define SYD_CHAIN_EVENT_IML4_CDDMAE 0x0a40
/* the ends of the G2 bus's DMA channels, in SYE_G2_DMA_TYPE's order */
#define SYD_CHAIN_EVENT_IML4_AICADM 0x0a60
#define SYD_CHAIN_EVENT_IML4_EX0DMA 0x0a80
#define SYD_CHAIN_EVENT_IML4_EX1DMA 0x0aa0
#define SYD_CHAIN_EVENT_IML4_DADMAE 0x0ac0
#define SYD_CHAIN_EVENT_IML4_EXT 0x0b00
/* the modem's external interrupt */
#define SYD_CHAIN_EVENT_IML4_MODEM 0x0b40
typedef Sint16 SY_CHAIN_EVENT;

/* a handler on an event's chain. the chain calls its handlers by ascending
 * priority, and a handler added at a priority already there goes first.
 * priority has bit 31 set while the handler is in use */
typedef struct SYS_CHAIN_ISR_ {
    void (*usrIsr)(void *);
    struct SYS_CHAIN_ISR_ *next;
    void *arg;
    Uint32 priority;
} SYS_CHAIN_ISR;
/* a handler's number in the handler pool, from 1, as syChainAddHandler
 * returns it; 0 when the pool is full */
typedef Uint32 SYCHAIN;

/* empty every chain and mark every handler unused. each event stays hooked
 * as syChainAddHandler hooked it */
void syChainInit(void);
/* add usrIsr, with arg, to event's chain at priority, whose bit 31 is
 * ignored. it returns the new handler's number. with the pool full it
 * returns 0 and changes nothing. event must be from 0 to 0x0bff, unchecked.
 *
 * an event's first handler hooks the event. a CPU event's vector, the word
 * that holds its handler, gets syChainMasterHandler, which runs the chain. the
 * blanks get Kamui's sync callbacks, which run their chains with the interrupt
 * mask at 6. every other event from 0x0900 up is taken for a bus event. the
 * level 4 interrupt's vector gets syChainClx1Handler. a normal or external
 * event's source is enabled at level 4.
 *
 * syChainClx1Handler serves the normal events to 0x0ac0 and the external
 * events to 0x0b60. it runs a served event's chain when the event's source
 * is pending and enabled at level 4. it runs no external event's chain while
 * a normal source is pending and enabled */
SYCHAIN syChainAddHandler(SY_CHAIN_EVENT event, void (*usrIsr)(void *),
                          Uint32 priority, void *arg);
/* take the handler numbered chain off its event's chain, and mark it
 * unused. a number that names no handler in use changes nothing. chain must
 * be below 256, unchecked.
 *
 * emptying a chain unhooks its event. the blanks lose Kamui's sync
 * callbacks. any other event's vector gets syIntBrkRteRte, and a normal or
 * external event's source is disabled at level 4. the level 4 interrupt's
 * vector keeps syChainClx1Handler.
 *
 * as shipped, a handler that heads the chain of events 0 to 15 is never
 * taken off. for an event below 0x200, syIntBrkRteRte goes to a word
 * 0x20000000 bytes above the event's vector */
void syChainDeleteHandler(SYCHAIN chain);

#endif
