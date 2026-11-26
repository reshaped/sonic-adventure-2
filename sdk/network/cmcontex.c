#include <access/cmcontex.h>
#include <sg_xpt.h>
#include <string.h>

/* SR's bits for privileged mode and register bank 1, and its interrupt mask.
 * a task starts with all of them set, every interrupt masked */
#define SR_MD 0x40000000
#define SR_RB 0x20000000
#define SR_IMASK 0xf0

#pragma section ATA
static char context_id[] = "$CTX";
#pragma section

/* the SDK declares stack without const, since the task runs on it */
/* NOLINTNEXTLINE(readability-non-const-parameter) */
void cmInitContext(CM_CONTEXT *context, void (*start)(void), Uint32 *stack)
{
    memset(context, 0, sizeof(*context));
    memcpy(&context->id, context_id, sizeof(context->id));
    context->r15 = (Uint32)stack;
    context->pr = (Uint32)start;
    context->sr = SR_MD | SR_RB | SR_IMASK;
}
