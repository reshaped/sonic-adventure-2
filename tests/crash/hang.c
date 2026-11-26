/* engine/dc/crash.c's capture of a hang, on a host. a test sets the stand-ins
 * below for what the capture reads on the console: the control registers,
 * what Shinobi's master keeps, main memory and the game's state. the capture
 * draws into crash_report.c's crash_screen */
#include <machine.h>
#include <sg_syfbr.h>
#include <shinobi.h>
#include <sy_int.h>

#include "engine/dc/crash_report.h"
#include "stage/stage.h"
#include "system/game_state.h"
#include "system/loader.h"

/* bank 0's R0 to R7, where the stopped code's lie while a handler runs */
Uint32 crash_r0_bank;
Uint32 crash_r1_bank;
Uint32 crash_r2_bank;
Uint32 crash_r3_bank;
Uint32 crash_r4_bank;
Uint32 crash_r5_bank;
Uint32 crash_r6_bank;
Uint32 crash_r7_bank;
/* SGR, SPC and SSR. the CPU writes each as it takes an interrupt, so a test
 * sets them to what the latest interrupt left */
Uint32 crash_sgr;
Uint32 crash_spc;
Uint32 crash_ssr;

/* syIntMasterISR_2's own */
Sint32 syIntNestCount;
void *syIntIntStack;
void *syIntLastStack;

/* the fiber switch's. a frame's PC names the function by the low 32 bits of
 * its address here, which is all a Uint32 keeps of a host's */
Uint32 isr_pc;
Uint32 isr_sr;

void syFbrSwitchToFiber_ISR_START(void)
{
}

/* the game's state, which the tests leave at 0 with no overlay's name */
struct game_state game_state;
static struct overlay_head head;
#undef OVERLAY_HEAD
#define OVERLAY_HEAD (&head)

int stage_current_id(void)
{
    return 0;
}

/* a handler's request for a switch of fibers while the capture reads the
 * frame. a test sets hang_switch to SWITCH, with READ_PC and READ_SR for each
 * of the frame's PC and SR that crash_read_frame had read by then */
#define SWITCH 1
#define READ_PC 2
#define READ_SR 4
Sint32 hang_switch;
/* SR.BL, which the request sets in the frame's SR */
#define SR_BL 0x10000000

/* crash_read_frame as the capture calls it, around the request that
 * hang_switch describes. the request does to the frame what sg_syf2.src's
 * patch_interrupt_return does. the record takes each word that hang_switch
 * says was read by then as the request found it, and the rest as it left
 * them */
static void read_frame_amid_switch(struct crash_stop *stop, const Uint32 *frame,
                                   const Uint32 *bank)
{
    Uint32 *patched = (Uint32 *)frame;
    Uint32 pc = frame[FRAME_SPC];
    Uint32 sr = frame[FRAME_SSR];

    if ((hang_switch & SWITCH) && pc != (Uint32)syFbrSwitchToFiber_ISR_START) {
        isr_sr = sr;
        patched[FRAME_SSR] = sr | SR_BL;
        isr_pc = pc;
        patched[FRAME_SPC] = (Uint32)syFbrSwitchToFiber_ISR_START;
    }
    crash_read_frame(stop, frame, bank);
    if (hang_switch & READ_PC) {
        stop->pc = pc;
    }
    if (hang_switch & READ_SR) {
        stop->sr = sr;
    }
}
#define crash_read_frame read_frame_amid_switch

#include "engine/dc/crash.c"

Uint32 crash_memory[MEMORY_SIZE / sizeof(Uint32)];

/* the screen's glyph for c, as the console draws it: 0 is a blank cell */
Uint32 hang_glyph(char c)
{
    return glyph(c);
}
