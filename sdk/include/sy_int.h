/* the CPU's exception and interrupt entry, which syIntIsr.src holds.
 * syIntInit copies its stubs to VBR. a stub jumps to a master, which saves
 * every register and calls the event's handler */
#ifndef SY_INT_H
#define SY_INT_H
#include <sg_xpt.h>

/* the bounds of the three pieces of code syIntInit copies. syIntISRRTE to
 * syIntISRRTE_Bottom goes to VBR. the exception stub, syIntExpISRTop to
 * syIntExpISRBottom, goes to VBR + 0x100 and VBR + 0x400. the interrupt stub,
 * syIntISRTop to syIntISRBottom, goes to VBR + 0x600. each bottom is the first
 * word of the code after its piece. the copy takes that word too */
/* @hack type: each bound declared as one word, where an array of words would
 * do */
extern Uint32 syIntISRRTE;
extern Uint32 syIntISRRTE_Bottom;
extern Uint32 syIntExpISRTop;
extern Uint32 syIntExpISRBottom;
extern Uint32 syIntISRTop;
extern Uint32 syIntISRBottom;
/* the words the exception stub and the interrupt stub jump through.
 * syIntInit points both at one master. syIntMasterISR runs a handler on the
 * interrupted stack, and syIntMasterISR_2 on syIntIntStack */
extern void (*syIntExpMastISR)(void);
extern void (*syIntMastISR)(void);
void syIntMasterISR(void);
void syIntMasterISR_2(void);
/* syIntNestCount counts the runs of syIntMasterISR_2 under way, one inside
 * another. syIntIntStack is the top of the stack it runs a handler on, where
 * that stack's area ends. syIntLastStack is the stack pointer of the code the
 * outermost run stopped. that run stores it as it starts, and a run inside
 * it doesn't */
extern Sint32 syIntNestCount;
extern void *syIntIntStack;
extern void *syIntLastStack;

#endif
