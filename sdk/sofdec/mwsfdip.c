/* Sofdec's setup before syHwInit */
#include <sg_mw.h>
#include <sg_xpt.h>
#include <sofdec.h>

#define INT_STACK_WORDS 0x800

/* the member exports this name, though no header of the SDK declares it */
/* NOLINTBEGIN(misc-use-internal-linkage) */
/* a stack of INT_STACK_WORDS words for interrupts. mwMngPreInit gives them
 * the manager's stack instead, so nothing uses this one */
Sint32 mwsfdg_intStack[INT_STACK_WORDS];
/* NOLINTEND(misc-use-internal-linkage) */

void mwPlyPreInitSofdec(void)
{
    mwMngPreInit();
}
