/* the Dreamcast's half of the crash screen that crash.h describes. it draws
 * 3 by 5 glyphs into the frame buffer the screen shows, each dot SCALE pixels
 * square. it calls nothing of the renderer's.
 *
 * a caller is a word of the stopped code's stack that addresses the instruction
 * after a call's delay slot. the callers are what the stack holds, not an
 * unwinding. so a word that a function left there before it returned shows
 * too */
#include "crash.h"

#include <machine.h>
#include <sg_syfbr.h>
#include <sg_xpt.h>
#include <shinobi.h>
#include <sy_int.h>

#include "crash_report.h"
#include "stage/stage.h"
#include "system/game_state.h"
#include "system/loader.h"

/* the PowerVR's registers that name the buffers the screen reads, and give its
 * pixels' depth. a buffer lies at VIDEO_MEMORY plus its register's VIDEO_OFFSET
 * bits. VIDEO_MEMORY is video memory's 32-bit view, which takes no 8-bit write.
 * so the screen is written a word at a time */
#define REG(address) (*(volatile Uint32 *)(address))
#define FB_R_CTRL REG(0xa05f8044)
#define FB_R_SOF1 REG(0xa05f8050)
#define FB_R_SOF2 REG(0xa05f8054)
#define FB_DEPTH_SHIFT 2
#define FB_DEPTH_MASK 3
#define VIDEO_MEMORY 0xa5000000
#define VIDEO_SIZE 0x00800000
#define VIDEO_OFFSET (VIDEO_SIZE - 4)
/* the CPU's registers that hold the address an address error asked for, and
 * TRAPA's number. TRA holds the number shifted up TRA_SHIFT bits */
#define TEA REG(0xff00000c)
#define TRA REG(0xff000020)
#define TRA_SHIFT 2

/* the picture is WIDTH pixels across. the text starts at LEFT and TOP, inside
 * the action-safe area, the picture's middle 90 percent. a line of
 * CRASH_COLUMNS cells is wider than the title-safe area, its middle 80
 * percent. each character takes a cell: its glyph, and a dot's gap to the
 * right and below */
#define WIDTH 640
#define LEFT 40
#define TOP 44
#define SCALE 3
#define GLYPH_W 3
#define GLYPH_H 5
#define CELL_W ((GLYPH_W + 1) * SCALE)
#define CELL_H ((GLYPH_H + 1) * SCALE)
/* the bytes of a pixel at the deepest depth FB_R_CTRL gives */
#define PIXEL_BYTES 4
/* the first pixel past the last cell's bottom row, from the scanout base */
#define REPORT_END                                                             \
    ((TOP + CRASH_LINES * CELL_H - 1) * WIDTH + LEFT + CRASH_COLUMNS * CELL_W)

/* main memory is MEMORY_SIZE bytes from MEMORY. below VIEWS_END the CPU drops
 * an address's bits from VIEW_SHIFT up. so main memory appears again in each 1
 * << VIEW_SHIFT bytes. each appearance is a view. the game's code runs in two
 * views: P1's, from P1_BASE, and the lowest. their addresses' top bytes are
 * CODE_VIEW and CODE_VIEW_LOW. CHAO.PRS and the Chao areas link in the lower
 * one */
#define MEMORY 0x0c000000
#define MEMORY_SIZE 0x01000000
#define VIEW_SHIFT 29
#define VIEWS_END 0xc0000000
#define P1_BASE 0x80000000
#define CODE_VIEW_SHIFT 24
#define CODE_VIEW ((P1_BASE | MEMORY) >> CODE_VIEW_SHIFT)
#define CODE_VIEW_LOW (MEMORY >> CODE_VIEW_SHIFT)
/* the most words of the stopped code's stack the screen reads */
#define STACK_WORDS 512
/* a call returns CALL_RETURN bytes past its instruction, after the delay slot.
 * the calls are JSR @Rn, BSRF Rn and BSR */
#define CALL_RETURN 4
#define IS_JSR(op) (((op) & 0xf0ff) == 0x400b)
#define IS_BSRF(op) (((op) & 0xf0ff) == 0x0003)
#define IS_BSR(op) (((op) & 0xf000) == 0xb000)

/* the words of the stack a report runs on */
#define OWN_STACK_WORDS 1024

/* what the screen reads of the CPU, and of main memory by address. STC stores
 * a control register in to, by the register's name. AT gives the bytes at an
 * address, which must lie in main memory. off the console, a test stands in
 * for both. it defines each register STC names as a variable, crash_ before
 * the register's name, and then includes this file's text. it defines
 * crash_memory, main memory's words by memory_offset's offsets */
#ifdef DREAMCAST
#define STC(name, to) __asm__("stc " #name ",%0" : "=r"(to))
#define AT(address) ((const void *)(address))
#else
extern Uint32 crash_memory[MEMORY_SIZE / sizeof(Uint32)];
#define STC(name, to) ((to) = crash_##name)
#define AT(address)                                                            \
    ((const void *)((const Uint8 *)crash_memory + memory_offset(address)))
#endif

/* where the game stopped. a crash's is the first exception's, and nothing
 * writes it again */
static struct crash_stop stopped;

/* each glyph's rows from the top, three bits a row, the left dot the high
 * bit */
#define GLYPH(a, b, c, d, e)                                                   \
    (Uint16)((a) << 12 | (b) << 9 | (c) << 6 | (d) << 3 | (e))
static const Uint16 digits[10] = {
    GLYPH(7, 5, 5, 5, 7), GLYPH(2, 6, 2, 2, 7), GLYPH(7, 1, 7, 4, 7),
    GLYPH(7, 1, 7, 1, 7), GLYPH(5, 5, 7, 1, 1), GLYPH(7, 4, 7, 1, 7),
    GLYPH(7, 4, 7, 5, 7), GLYPH(7, 1, 1, 1, 1), GLYPH(7, 5, 7, 5, 7),
    GLYPH(7, 5, 7, 1, 7),
};
static const Uint16 letters[26] = {
    GLYPH(7, 5, 7, 5, 5), GLYPH(6, 5, 6, 5, 6), GLYPH(7, 4, 4, 4, 7),
    GLYPH(6, 5, 5, 5, 6), GLYPH(7, 4, 7, 4, 7), GLYPH(7, 4, 7, 4, 4),
    GLYPH(7, 4, 5, 5, 7), GLYPH(5, 5, 7, 5, 5), GLYPH(7, 2, 2, 2, 7),
    GLYPH(1, 1, 1, 5, 7), GLYPH(5, 5, 6, 5, 5), GLYPH(4, 4, 4, 4, 7),
    GLYPH(5, 7, 7, 5, 5), GLYPH(7, 5, 5, 5, 5), GLYPH(7, 5, 5, 5, 7),
    GLYPH(7, 5, 7, 4, 4), GLYPH(7, 5, 5, 7, 1), GLYPH(7, 5, 6, 5, 5),
    GLYPH(7, 4, 7, 1, 7), GLYPH(7, 2, 2, 2, 2), GLYPH(5, 5, 5, 5, 7),
    GLYPH(5, 5, 5, 5, 2), GLYPH(5, 5, 7, 7, 5), GLYPH(5, 5, 2, 5, 5),
    GLYPH(5, 5, 2, 2, 2), GLYPH(7, 1, 2, 4, 7),
};
/* the small letters a to f, which only hex digits use */
static const Uint16 hex_letters[6] = {
    GLYPH(0, 3, 5, 5, 3), GLYPH(4, 4, 6, 5, 6), GLYPH(0, 3, 4, 4, 3),
    GLYPH(1, 1, 3, 5, 3), GLYPH(2, 5, 7, 4, 3), GLYPH(3, 4, 6, 4, 4),
};
#define GLYPH_DOT GLYPH(0, 0, 0, 0, 2)
#define GLYPH_UNDERSCORE GLYPH(0, 0, 0, 0, 7)
#define GLYPH_HYPHEN GLYPH(0, 0, 7, 0, 0)
#define GLYPH_QUESTION GLYPH(7, 1, 2, 0, 2)

/* c's glyph. 0 is a blank cell: a space's, and that of a character with no
 * glyph here. off the console, a test reads each glyph through this function,
 * and holds that every other character of the report's lines has one */
static Uint32 glyph(char c)
{
    if (c >= '0' && c <= '9') {
        return digits[c - '0'];
    }
    if (c >= 'a' && c <= 'f') {
        return hex_letters[c - 'a'];
    }
    if (c >= 'A' && c <= 'Z') {
        return letters[c - 'A'];
    }
    switch (c) {
    case '.':
        return GLYPH_DOT;
    case '_':
        return GLYPH_UNDERSCORE;
    case '-':
        return GLYPH_HYPHEN;
    case '?':
        return GLYPH_QUESTION;
    default:
        return 0;
    }
}

/* the console's own exception entry and drawing. off the console,
 * crash_report.c keeps each line instead, and no exception comes */
#ifdef DREAMCAST
/* a pixel's bytes at each depth FB_R_CTRL gives: 555 RGB, 565 RGB, packed 888
 * and 0888 */
static const Uint8 depth_bytes[FB_DEPTH_MASK + 1] = {2, 2, 3, 4};

/* the exceptions crash_exception has taken. the first is the crash, and a
 * second is a fault of its report. any later one draws nothing */
static Uint32 exceptions;
static Uint32 own_stack[OWN_STACK_WORDS];

static void crash_exception(Uint32 code, const Uint32 *frame)
    __attribute__((noreturn, used));

/* the handler crash_install gives each code it takes. crash_report.h describes
 * a master and its frame. a master calls the handler with the code in R4, SR.BL
 * clear and every interrupt masked. R15 then points at the floats
 * CRASH_FLOAT_WORDS describes. the handler takes the frame's address from the
 * word above them, and jumps to crash_exception(code, frame) on own_stack. so
 * the report needs no room on the master's stack */
/* a macro's value as the assembler reads it */
#define QUOTE(value) #value
#define QUOTE_EXPANDED(value) QUOTE(value)
/* clang-format off */
__asm__("\t.pushsection\t.text.crash_entry,\"ax\",@progbits\n"
        "\t.align\t2\n"
        "\t.global\t_crash_entry\n"
        "_crash_entry:\n"
        "\tmov.w\t1f,r0\n"
        "\tmov.l\t@(r0,r15),r5\n"
        "\tmov.l\t2f,r15\n"
        "\tmov.l\t3f,r0\n"
        "\tjmp\t@r0\n"
        "\tnop\n"
        "1:\t.word\t" QUOTE_EXPANDED(CRASH_FLOAT_WORDS) " * 4\n"
        "\t.align\t2\n"
        "2:\t.long\t%a0\n"
        "3:\t.long\t%a1\n"
        "\t.popsection\n"
        :
        : "i"(&own_stack[OWN_STACK_WORDS]), "i"(crash_exception));
/* clang-format on */

/* draw c white on black in the cell at column and line, in the buffer the
 * screen shows. an interlaced picture keeps its two fields' lines by turns in
 * one buffer. the buffer starts at the lower of FB_R_SOF1 and FB_R_SOF2, where
 * FB_R_SOF2 is set. so a line of the picture is a line of the buffer,
 * interlaced or not. LEFT, CELL_W and WIDTH are multiples of 4 pixels. so a
 * cell's rows start and end on a word at every depth */
static void draw_char(int column, int line, char c)
{
    Uint32 depth = (FB_R_CTRL >> FB_DEPTH_SHIFT) & FB_DEPTH_MASK;
    Uint32 bytes = depth_bytes[depth];
    Uint32 first = FB_R_SOF1 & VIDEO_OFFSET;
    Uint32 second = FB_R_SOF2 & VIDEO_OFFSET;
    Uint32 words = (CELL_W * bytes) / sizeof(Uint32);
    Uint32 bits = glyph(c);
    /* one row of the cell's pixels, as the buffer holds them. a lit pixel is
     * all 0xff bytes at every depth, so its byte order doesn't matter */
    Uint32 row[CELL_W * PIXEL_BYTES / sizeof(Uint32)];
    volatile Uint32 *to;
    Uint8 *byte;
    Uint32 i;
    int glyph_row;
    int dot;
    int y;

    if (second != 0 && second < first) {
        first = second;
    }
    if (first > VIDEO_SIZE - REPORT_END * bytes) {
        return;
    }
    to = (volatile Uint32 *)(VIDEO_MEMORY + first +
                             ((((TOP + (line * CELL_H)) * WIDTH) + LEFT +
                               (column * CELL_W)) *
                              bytes));
    for (glyph_row = 0; glyph_row <= GLYPH_H; glyph_row++) {
        byte = (Uint8 *)row;
        for (dot = 0; dot <= GLYPH_W; dot++) {
            int lit = glyph_row < GLYPH_H && dot < GLYPH_W &&
                      ((bits >> (((GLYPH_H - 1 - glyph_row) * GLYPH_W) +
                                 (GLYPH_W - 1 - dot))) &
                       1);

            for (i = 0; i < SCALE * bytes; i++) {
                *byte++ = lit ? 0xff : 0x00;
            }
        }
        for (y = 0; y < SCALE; y++) {
            for (i = 0; i < words; i++) {
                to[i] = row[i];
            }
            to += (WIDTH * bytes) / sizeof(Uint32);
        }
    }
}

uint32_t crash_scanout(void)
{
    return FB_R_SOF1;
}

void crash_draw(int line, const char *text)
{
    int column;

    if (line < 0 || line >= CRASH_LINES) {
        return;
    }
    for (column = 0; column < CRASH_COLUMNS; column++) {
        draw_char(column, line, *text ? *text++ : ' ');
    }
}
#endif

/* address's offset from the start of main memory in its view. below
 * VIEWS_END, an address outside main memory gives MEMORY_SIZE or more */
static Uint32 memory_offset(Uint32 address)
{
    return (address & ((1U << VIEW_SHIFT) - 1)) - MEMORY;
}

/* whether the size bytes from address lie in one view of main memory, at a
 * multiple of size. size must be a power of two */
static int in_memory(Uint32 address, Uint32 size)
{
    Uint32 offset = memory_offset(address);

    return address < VIEWS_END && (address & (size - 1)) == 0 &&
           offset < MEMORY_SIZE && size <= MEMORY_SIZE - offset;
}

/* whether word could be one of the game's return addresses. such a word lies in
 * a view the game's code runs in, CALL_RETURN bytes past a call */
static int returns_from_call(Uint32 word)
{
    Uint32 view = word >> CODE_VIEW_SHIFT;
    Uint16 op;

    if ((view != CODE_VIEW && view != CODE_VIEW_LOW) ||
        !in_memory(word - CALL_RETURN, sizeof(Uint16))) {
        return FALSE;
    }
    op = *(const Uint16 *)AT(word - CALL_RETURN);
    return IS_JSR(op) || IS_BSRF(op) || IS_BSR(op);
}

static void read_game(struct crash_stop *stop)
{
    Uint32 i;

    stop->mode = game_state.mode;
    stop->stage = stage_current_id();
    for (i = 0; i < CRASH_NAME_BYTES; i++) {
        stop->slot[i] = OVERLAY_HEAD->name[i];
    }
}

/* fill stop's callers from the stack at its sp. it reads up to STACK_WORDS
 * words, and none past main memory's end in sp's view */
static void read_callers(struct crash_stop *stop)
{
    const Uint32 *stack;
    Uint32 words;
    Uint32 i;

    if (!in_memory(stop->sp, sizeof(Uint32))) {
        stop->callers = CRASH_NO_STACK;
        return;
    }
    stack = AT(stop->sp);
    words = (MEMORY_SIZE - memory_offset(stop->sp)) / sizeof(Uint32);
    if (words > STACK_WORDS) {
        words = STACK_WORDS;
    }
    stop->callers = 0;
    for (i = 0; i < words && stop->callers < CRASH_CALLERS; i++) {
        if (returns_from_call(stack[i])) {
            stop->caller_offset[stop->callers] = i * sizeof(Uint32);
            stop->caller[stop->callers++] = stack[i];
        }
    }
}

/* bank 0's R0 to R7. a handler runs in bank 1, so STC Rn_BANK reads bank 0 */
static void read_bank(Uint32 bank[CRASH_BANK_REGISTERS])
{
    STC(r0_bank, bank[0]);
    STC(r1_bank, bank[1]);
    STC(r2_bank, bank[2]);
    STC(r3_bank, bank[3]);
    STC(r4_bank, bank[4]);
    STC(r5_bank, bank[5]);
    STC(r6_bank, bank[6]);
    STC(r7_bank, bank[7]);
}

/* the watch's report, of the code the outermost event stopped: the vblank,
 * unless the vblank stopped another event's handler.
 *
 * it reads none of SGR, SPC and SSR. the CPU writes all three as it takes an
 * interrupt, and a return puts none of them back. the vblank chain runs under
 * mask 6, and the watch last in it. so an interrupt of a higher level may come
 * and go first, and leave in them the place in a handler that it stopped.
 *
 * it reads what syIntMasterISR_2 keeps of the code its outermost run stopped:
 *
 * - syIntLastStack, that code's stack pointer.
 * - the outermost run's frame, which ends at syIntIntStack. with bank 0's R0
 *   to R7, it gives the PC, SR, PR and R0 to R14.
 * - isr_pc and isr_sr, where the frame's PC is syFbrSwitchToFiber_ISR_START.
 *   a handler then asked for a switch of fibers, as a vblank function of the
 *   middleware's does before this handler runs. syFbrDelayedSwitchToFiber
 *   wrote that address over the frame's PC, set SR.BL in the frame's SR, and
 *   kept the PC and SR it found in isr_pc and isr_sr.
 *
 * the capture masks no interrupt. on a run inside the outermost one, the
 * master's own code writes none of these. a handler on any run may ask for a
 * switch, during the capture too. so the capture reads the frame's PC a second
 * time, after crash_read_frame has read the frame's PC and SR. a switch asked
 * for before that second read shows in it, and one asked for after it changes
 * nothing the capture has yet to read.
 *
 * where syIntNestCount is above 1, the vblank stopped another event's handler.
 * that handler's PC lies in the vblank's own frame, at an address nothing
 * keeps. so the report's name line says that it is not known.
 *
 * where syIntIntStack is null, the masters push each frame on the stopped
 * code's stack, at no address the screen can find. where syIntNestCount is
 * below 1, no run is under way. the report then says that its PC, SR and SP
 * are not known */
void crash_capture_hang(uint32_t elapsed)
{
    Uint32 bank[CRASH_BANK_REGISTERS];
    const Uint32 *frame;

    stopped.what = "HANG";
    stopped.number = elapsed;
    stopped.name = NULL;
    stopped.code = 0;
    read_game(&stopped);
    if (syIntIntStack == NULL || syIntNestCount < 1) {
        crash_show_unknown(&stopped);
        return;
    }
    if (syIntNestCount > 1) {
        stopped.name = "CODE UNDER A HANDLER WHOSE PC IS NOT KNOWN";
    }
    frame = AT((Uint32)syIntIntStack - (FRAME_WORDS * sizeof(Uint32)));
    read_bank(bank);
    crash_read_frame(&stopped, frame, bank);
    if (frame[FRAME_SPC] == (Uint32)syFbrSwitchToFiber_ISR_START) {
        stopped.pc = isr_pc;
        stopped.sr = isr_sr;
    }
    stopped.sp = (Uint32)syIntLastStack;
    read_callers(&stopped);
    crash_show(&stopped);
}

/* the two stops that end the game. neither returns, so only the console runs
 * them */
#ifdef DREAMCAST
/* crash_entry's jump, on own_stack. the first exception fills stopped and draws
 * it. a second is a fault of that report. it redraws what it can of the first,
 * with the second's code and PC. any later one draws nothing. SGR, TEA and TRA
 * hold what the latest exception left. so they are read before anything that
 * could fault. the game can't go on, so this never returns */
static void crash_exception(Uint32 code, const Uint32 *frame)
{
    Uint32 bank[CRASH_BANK_REGISTERS];
    Uint32 sp;
    Uint32 address = code == CRASH_TRAPA ? TRA >> TRA_SHIFT : TEA;

    STC(sgr, sp);
    read_bank(bank);
    if (exceptions++ == 0) {
        crash_read_frame(&stopped, frame, bank);
        stopped.what = "CRASH";
        stopped.number = code;
        stopped.name = NULL;
        stopped.code = code;
        stopped.address = address;
        stopped.sp = sp;
        read_game(&stopped);
        read_callers(&stopped);
        crash_show(&stopped);
    } else if (exceptions == 2) {
        crash_show_fault(&stopped, code, frame[FRAME_SPC]);
    }
    for (;;) {
    }
}

void port_stop(const char *what, const char *name, Uint32 number)
{
    /* no handler of the game's runs again */
    set_imask(IMASK_ALL);
    stopped.what = what;
    stopped.number = number;
    stopped.name = name;
    stopped.code = 0;
    stopped.pc = (Uint32)__builtin_return_address(0);
    stopped.sp = (Uint32)__builtin_frame_address(0);
    STC(sr, stopped.sr);
    stopped.registers = FALSE;
    read_game(&stopped);
    read_callers(&stopped);
    crash_show(&stopped);
    for (;;) {
    }
}
#endif
