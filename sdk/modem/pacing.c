/* the pacing: two queues that move bytes between the host and the port, at
 * the port's speed. MainPacingFunc fills the port's receive buffers with
 * the host's bytes. SecondPacingFunc gives the host the port's transmit
 * buffers */
#include <sg_xpt.h>
#include <string.h>

#include "cmi.h"
#include "cmm.h"
#include "dte_vxd.h"
#include "dteapi.h"
#include "pacing.h"

/* each pass of the pacing moves the bytes of PACE_MS milliseconds at the
 * port's rate */
#define PACE_MS 9
#define MS_PER_SECOND 1000
/* the bytes PacingInQ_MoveInQ moves at most in one pass while the port
 * enters command mode */
#define COMMAND_BYTES 0x80
/* what PacingOutQ_MoveOutQ returns */
#define OUT_DONE 0 /* every byte went into the receive buffers */
#define OUT_HELD 1 /* none moved: the port's mode holds the host's bytes */
#define OUT_WAITING 4 /* some wait in the queue for a later pass */

/* the bytes of a queue, and of the buffer PacingInQ copies a transmit
 * buffer into */
#define PACING_QUEUE_SIZE 0x400
#define PACING_BUFFER_SIZE 0x100

/* the queue of the port's bytes for the host. buffer holds the transmit
 * buffer taken last, with left bytes from next on still to queue. queue
 * holds the bytes from read to write, which the host has yet to take.
 * remainder holds the thousandths of a byte the passes so far have earned
 * and not moved */
struct PacingInQ {
    Uint8 buffer[PACING_BUFFER_SIZE];
    Uint8 queue[PACING_QUEUE_SIZE];
    Uint16 left;
    Uint16 next;
    Uint8 *read;
    Uint8 *write;
    Uint32 remainder;
};

/* the queue of the host's bytes for the port. queue holds the held bytes
 * the port had no room for, from head on. the host's next bytes land at
 * tail */
struct PacingOutQ {
    Uint8 queue[PACING_QUEUE_SIZE];
    Uint8 *head;
    Uint8 *tail;
    Uint32 held;
};

/* the member exports these names, though no other member uses them */
/* NOLINTBEGIN(misc-use-internal-linkage) */
/* forget the two queues */
void ClosePacing(void);
/* set up in empty, and return it. PacingInQ_dt does nothing */
struct PacingInQ *PacingInQ_ct(struct PacingInQ *in);
void PacingInQ_dt(struct PacingInQ *in);
/* move a pass's worth of the port's bytes to the host, and return 1. while
 * the mode's hold_in is set, it moves none and returns 0. cmi's flush_in
 * first empties the queue */
Uint8 PacingInQ_Pace(struct PacingInQ *in);
/* copy the next filled transmit buffer into in's buffer, and mark the
 * transmit buffer empty again. it returns 1, or 0 where none is filled */
Uint8 PacingInQ_MoveSccBuffer(struct PacingInQ *in);
/* queue a pass's worth of bytes from the transmit buffers. the call then gives
 * the host every byte queued, through dteSetRxBuffer. the bytes the host
 * doesn't take stay queued. where the mode's hold_in stops the queuing, the
 * call gives the host nothing and returns 1. otherwise it returns 0 */
Uint8 PacingInQ_MoveInQ(struct PacingInQ *in);
/* set up out empty, and return it. PacingOutQ_dt does nothing */
struct PacingOutQ *PacingOutQ_ct(struct PacingOutQ *out);
void PacingOutQ_dt(struct PacingOutQ *out);
/* move a pass's worth of the host's bytes into the port's receive buffers,
 * and return 1. while the mode's hold_out is set, it moves none and returns
 * 0. cmi's flush_out first drops the held bytes */
Uint8 PacingOutQ_Pace(struct PacingOutQ *out);
/* fill the receive buffers with a pass's worth of the host's bytes, which
 * dteGetTxBuffer gives. the bytes stop at the room the buffers have. it
 * returns an OUT_ value. once every byte is in, it closes the receive
 * buffer and marks the port idle */
Uint8 PacingOutQ_MoveOutQ(struct PacingOutQ *out);
/* the bytes the empty receive buffers have room for, counted from the one
 * the host's side fills. that one's room counts a byte short */
Uint32 PacingOutQ_GetSCCFreeCount(struct PacingOutQ *out);
/* SetPacingEvent keeps event, and GetPacingEvent returns it.
 * StartMainPacingEvent does nothing */
void SetPacingEvent(Uint8 event);
Uint8 GetPacingEvent(void);
void StartMainPacingEvent(void);
void init_pacing_cc_f(void);
void end_pacing_cc_f(void);

/* the pacing's two queues, which init_pacing_cc_f sets up */
struct PacingInQ Static_PacingInQ;
struct PacingOutQ Static_PacingOutQ;
/* NOLINTEND(misc-use-internal-linkage) */

/* the bytes a second the port moves at each speed, as vxd_dte_set_interface
 * numbers them */
static const Uint32 pacing_rates[] = {
    100,   100,   144,   288,   576,   864,   1152,  1440,   1728,   2016,
    1920,  2592,  2880,  3168,  3360,  3456,  3520,  3680,   3740,   3840,
    4000,  4032,  4080,  4160,  4320,  4480,  4560,  4608,   4640,   4800,
    4960,  5040,  5120,  5280,  5440,  5520,  5600,  5760,   5920,   6000,
    6080,  6240,  6400,  6480,  6560,  6720,  6912,  7680,   8640,   9216,
    11520, 13824, 15360, 27648, 55296, 82944, 82944, 110592, 110592, 110592};

/* a byte of the member's data that no code reads */
static Uint8 dat_0cb05bd0 = 0;
/* the event SetPacingEvent keeps */
static volatile Uint8 pacing_event = 0;
/* bytes of the member's data that no code reads */
static Uint8 dat_0cb05bd2[14] = {0};

/* the milliseconds a pass moves bytes for: 0 until MainPacingFunc first
 * runs, and PACE_MS from then on */
static Uint32 pacing_ms;
/* the two queues, from OpenPacing until ClosePacing */
static struct PacingInQ *pacing_in;
static struct PacingOutQ *pacing_out;

void OpenPacing(void)
{
    pacing_in = &Static_PacingInQ;
    pacing_out = &Static_PacingOutQ;
}

void ClosePacing(void)
{
    pacing_in = NULL;
    pacing_out = NULL;
}

void MainPacingFunc(void)
{
    pacing_ms = PACE_MS;
    if (glbScc_Pc->cmi->mode.value.bits.command == 0) {
        PacingOutQ_Pace(pacing_out);
    } else {
        SCC_PC_RxInCMDMode(glbScc_Pc);
    }
}

void SecondPacingFunc(void)
{
    if (glbScc_Pc->cmi->mode.value.bits.command == 0) {
        PacingInQ_Pace(pacing_in);
    } else {
        SCC_PC_TxPrioCharBuff(glbScc_Pc);
    }
}

struct PacingInQ *PacingInQ_ct(struct PacingInQ *in)
{
    in->left = 0;
    in->next = 0;
    in->remainder = 0;
    in->read = in->write = in->queue;
    return in;
}

void PacingInQ_dt(struct PacingInQ *in)
{
}

Uint8 PacingInQ_Pace(struct PacingInQ *in)
{
    if (glbScc_Pc->cmi->mode.value.bits.hold_in) {
        return 0;
    }
    if (glbScc_Pc->cmi->flush_in) {
        in->read = in->write = in->queue;
        in->next = in->left = 0;
        in->remainder = 0;
    }
    glbScc_Pc->cmi->flush_in = 0;
    PacingInQ_MoveInQ(in);
    return 1;
}

Uint8 PacingInQ_MoveSccBuffer(struct PacingInQ *in)
{
    /* @hack spelling: moved, a result set in each arm, where returns of 1 and
     * 0 would do */
    Uint8 moved;

    if (!glbScc_Pc->mem->tx[glbScc_Pc->cmi->bd->tx_index].status.bit.empty) {
        in->left = SCC_PC_SwapByte(
            glbScc_Pc, glbScc_Pc->mem->tx[glbScc_Pc->cmi->bd->tx_index].length);
        memcpy(in->buffer,
               (Uint8 *)glbScc_Pc->mem->tx[glbScc_Pc->cmi->bd->tx_index].data,
               in->left);
        in->next = 0;
        SCC_PC_Close_TxBuffer(glbScc_Pc, 1);
        moved = 1;
    } else {
        moved = 0;
    }
    return moved;
}

Uint8 PacingInQ_MoveInQ(struct PacingInQ *in)
{
    struct ddalDataTransfer transfer;
    struct ddal_result *result;
    Uint32 queued = 0;
    Uint32 budget;
    Uint32 scaled;
    Uint32 i;
    Uint8 *to;
    Uint16 from;

    if (in->read != in->write) {
        queued = in->write - in->read;
    }
    scaled = pacing_ms * pacing_rates[glbScc_Pc->speed];
    budget = scaled / MS_PER_SECOND;
    in->remainder += scaled % MS_PER_SECOND;
    if (in->remainder >= MS_PER_SECOND) {
        budget++;
        in->remainder -= MS_PER_SECOND;
    }
    if (glbScc_Pc->entering_command) {
        budget = COMMAND_BYTES;
    }
    /* @bug logic: where the bytes left from the last pass already exceed
     * this pass's budget, the loop queues on until the transmit buffers run
     * dry or the queue fills. its tests end it only where queued equals
     * budget (CMP/EQ at 0x0cae3eb6 and 0x0cae3e82). it happens where the
     * host takes no byte of a pass whose budget the remainder raised by one.
     * the pass after it then finds one byte more queued than its budget */
    while (queued != budget) {
        if (in->left == 0) {
            if (glbScc_Pc->entering_command) {
                break;
            }
            if (!PacingInQ_MoveSccBuffer(in)) {
                break;
            }
        }
        from = in->next;
        while (in->left != 0) {
            if (in->write - in->queue == PACING_QUEUE_SIZE - 1) {
                break;
            }
            if (glbScc_Pc->cmi->mode.value.bits.hold_in) {
                return 1;
            }
            *in->write++ = in->buffer[from];
            queued++;
            in->next++;
            in->left--;
            if (queued == budget) {
                break;
            }
            from++;
        }
        if (in->write - in->queue == PACING_QUEUE_SIZE - 1) {
            break;
        }
    }
    if (queued > 0) {
        transfer.data = in->read;
        transfer.length = queued;
        result = dteSetRxBuffer(&transfer);
        if (result->length != queued) {
            to = in->queue;
            in->read += result->length;
            i = result->length;
            while (i < queued) {
                *to++ = *in->read++;
                i++;
            }
            in->read = in->queue;
            in->write = in->queue + queued - result->length;
        } else {
            in->read = in->write = in->queue;
        }
    }
    return 0;
}

struct PacingOutQ *PacingOutQ_ct(struct PacingOutQ *out)
{
    out->head = out->tail = out->queue;
    out->held = 0;
    return out;
}

void PacingOutQ_dt(struct PacingOutQ *out)
{
}

Uint8 PacingOutQ_Pace(struct PacingOutQ *out)
{
    if (glbScc_Pc->cmi->mode.value.bits.hold_out) {
        return 0;
    }
    if (glbScc_Pc->cmi->flush_out) {
        out->head = out->tail = out->queue;
        out->held = 0;
    }
    PacingOutQ_MoveOutQ(out);
    glbScc_Pc->cmi->flush_out = 0;
    return 1;
}

Uint8 PacingOutQ_MoveOutQ(struct PacingOutQ *out)
{
    struct ddal_result none;
    struct ddalDataTransfer transfer;
    struct ddal_result *result;
    Uint32 written;
    Uint32 budget;
    Uint32 room;
    Uint32 total;
    Uint32 scaled;
    struct SCC_PC *scc;

    /* @hack spelling: written and scc set in one statement by a comma, where
     * two statements would do */
    written = 0, scc = glbScc_Pc;
    if (*scc->cmi->mode.ref && !scc->cmi->mode.value.bits.unk_bit3) {
        return OUT_HELD;
    }
    scaled = pacing_ms * pacing_rates[scc->speed];
    budget = scaled / MS_PER_SECOND;
    room = PacingOutQ_GetSCCFreeCount(out);
    if (out->held + budget > room) {
        if (out->held > room) {
            budget = 0;
        } else {
            budget = room - out->held;
        }
    }
    if (budget != 0) {
        transfer.data = out->tail;
        if (out->held + budget >= PACING_QUEUE_SIZE) {
            budget = PACING_QUEUE_SIZE - out->held;
        }
        transfer.length = budget;
        result = dteGetTxBuffer(&transfer);
    } else {
        /* @hack spelling: none's fields stored through result, where
         * stores to none itself would do */
        result = &none;
        result->remaining = 1;
        result->length = 0;
    }
    total = result->length + out->held;
    out->held = 0;
    SCC_PC_FillSccBuffer(glbScc_Pc, out->head, total, &written);
    out->head += written;
    if (written != total) {
        out->held = total - written;
        out->tail = out->queue + out->held;
        memcpy(out->queue, out->head, out->held);
        out->head = out->queue;
        return OUT_WAITING;
    }
    out->head = out->tail = out->queue;
    SCC_PC_CloseBuffTimeOut(glbScc_Pc);
    return OUT_DONE;
}

Uint32 PacingOutQ_GetSCCFreeCount(struct PacingOutQ *out)
{
    Uint8 i;
    Uint32 total = 0;

    i = glbScc_Pc->cmi->bd->rx_index;

    while (glbScc_Pc->mem->rx[i].status.bit.empty) {
        if (i == glbScc_Pc->cmi->bd->rx_index) {
            total += glbScc_Pc->cmi->mrblr - glbScc_Pc->cmi->bd->rx_count - 1;
        } else {
            total += glbScc_Pc->cmi->mrblr;
        }
        i++;
        if (i == COM_BUFFERS) {
            i = 0;
        }
        if (i == glbScc_Pc->cmi->bd->rx_index) {
            break;
        }
    }
    return total;
}

void SetPacingEvent(Uint8 event)
{
    pacing_event = event;
}

Uint8 GetPacingEvent(void)
{
    return pacing_event;
}

void StartMainPacingEvent(void)
{
    /* @hack redundant: a test of pacing_event, which does nothing either
     * way */
    if (pacing_event == 1) {
    }
}

void init_pacing_cc_f(void)
{
    PacingInQ_ct(&Static_PacingInQ);
    PacingOutQ_ct(&Static_PacingOutQ);
}

void end_pacing_cc_f(void)
{
    PacingOutQ_dt(&Static_PacingOutQ);
    PacingInQ_dt(&Static_PacingInQ);
}

/* the member's entries in DSG_INIT_ and DSG_END_. the Chao area calls one as
 * it starts, and the other as it ends */
#pragma section _INIT_
/* NOLINTBEGIN(misc-use-internal-linkage) */
void (*init_pacingcc)(void) = init_pacing_cc_f;
/* NOLINTEND(misc-use-internal-linkage) */
#pragma section _END_
/* NOLINTBEGIN(misc-use-internal-linkage) */
void (*end_pacingcc)(void) = end_pacing_cc_f;
/* NOLINTEND(misc-use-internal-linkage) */
#pragma section
