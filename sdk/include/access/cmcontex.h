/* a context of ACCESS's cooperative tasks: the registers a switch saves.
 * cmSwitchContext saves the running task's into previous and resumes next.
 * cmInitContext readies a context to run start on stack */
#ifndef ACCESS_CMCONTEX_H
#define ACCESS_CMCONTEX_H
#include <sg_xpt.h>

typedef struct {
    Uint32 id;
    Uint32 r0, r1, r2, r3, r4, r5, r6, r7;
    Uint32 r8, r9, r10, r11, r12, r13, r14, r15;
    Uint32 pr, sr, dummy;
} CM_CONTEXT;

void cmInitContext(CM_CONTEXT *context, void (*start)(void), Uint32 *stack);
void cmSwitchContext(CM_CONTEXT *next, CM_CONTEXT *previous);
Uint32 *cmGetSP(void);
#endif
