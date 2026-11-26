/* Shinobi's fibers, as far as the sources use them. a fiber runs on a stack
 * of its own. a switch saves the running fiber's registers in a frame on
 * that stack, then runs another fiber */
#ifndef SG_SYFBR_H
#define SG_SYFBR_H
#include <sg_xpt.h>

/* a fiber's record: its saved stack pointer, and its data. a record whose
 * sp is null is free */
typedef struct {
    void *sp;
    void *data;
} SYS_FBR_INFO;
/* a fiber, by its record */
typedef SYS_FBR_INFO *SYFBR;
/* what a fiber runs. it is passed the arg its fiber was made with */
typedef void (*SyFbrYfunc)(void *arg);

/* keep the fibers' records in work, a table of n records. the call clears
 * every record and returns 0, and no fiber is current afterward. a null
 * work or an n below 2 returns -1 and changes nothing */
Sint32 syFbrInit(void *work, Sint32 n);
/* have exceptions and interrupts run on the nword words at stack, from its
 * end, and return 0. the call sets sygIntMode to SYD_INT_MODE_INTSTACK and
 * sygIntStack to the stack's end, which syHwInit reads.
 * syFbrDelayedSwitchToFiber finds an interrupt's return PC and SR just below
 * that end. a null stack returns -1 and changes nothing */
Sint32 syFbrPreInit(Sint32 *stack, int nword);
/* make a fiber that runs func with arg on the size bytes at stack, and
 * return it. a null stack, a size of 0 or a null func returns NULL. where no
 * record is free, the call returns NULL too. it has built the fiber's first
 * frame on the stack all the same */
SYFBR syFbrCreateFiber(void *stack, Uint32 size, SyFbrYfunc func, void *arg);
/* make the running code a fiber with data, make it the current fiber, and
 * return it. it returns NULL while a fiber is current, or when no record is
 * free */
SYFBR syFbrConvertThreadToFiber(void *data);
/* lay the initial register frame on the stack and return its saved sp */
void *syFbrCreateFiberMain(void *stack, Sint32 size, SyFbrYfunc entry,
                           void *arg);
/* switch to fbr now */
void syFbrSwitchToFiber(SYFBR fbr);
/* called from an interrupt: switch to fbr once the interrupt returns. a
 * later call in the same interrupt changes only the fiber to switch to */
void syFbrDelayedSwitchToFiber(SYFBR fbr);
/* where an interrupt that asked for a switch returns, in place of the
 * code it stopped. syFbrDelayedSwitchToFiber saves the interrupt's return
 * PC and SR in isr_pc and isr_sr. it puts this address in the return PC,
 * and sets SR.BL in the return SR. the switch then saves the running fiber
 * as though the interrupt had returned to isr_pc with SR isr_sr */
void syFbrSwitchToFiber_ISR_START(void);
extern Uint32 isr_pc;
extern Uint32 isr_sr;

#endif
