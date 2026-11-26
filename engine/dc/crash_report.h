/* the crash screen's record of where the game stopped, and its lines.
 * crash_report.c reads nothing of the machine. on the Dreamcast crash.c fills
 * the record and draws each line. on a host crash_draw keeps each line in
 * crash_screen, for the tests to read.
 *
 * the lines, from the top:
 *
 * - what and number. a crash's are CRASH and the exception's code. a hang's are
 *   HANG and the vblanks since the last new frame. port_stop gives its own.
 *   name follows on a line of its own, where it isn't null. a hang's name is
 *   CODE UNDER A HANDLER WHOSE PC IS NOT KNOWN, where the vblank stopped
 *   another event's handler. its other lines are then of the code under the
 *   handlers, which the outermost event stopped.
 * - PC, then PR where the record holds registers, then SR. a hang whose
 *   stopped code crash.c can't find has PC SR AND SP NOT KNOWN in this line's
 *   place, and after it MODE's line alone.
 * - SP, then ADDR for an address error, or TRAPA and its number.
 * - MODE and STAGE in decimal, then SLOT and the name of the overlay in the
 *   stage slot. the slot holds other files too, as a mission's messages. SLOT
 *   shows ? where its first bytes are no name.
 * - R0 to R14, where the record holds registers. they go four to a line, behind
 *   R and the first one's number in decimal.
 * - the callers, three to a line. each is its offset from SP in bytes, in three
 *   hex digits, then the word. where SP addresses no stack, the line is NO
 *   STACK AT SP.
 *
 * every other number is hex, in eight digits. a hex digit past 9 is a small
 * letter. no b or d then reads as an 8 or a 0. every other letter is a
 * capital */
#ifndef DC_CRASH_REPORT_H
#define DC_CRASH_REPORT_H

#include <sg_xpt.h>
#include <stdint.h>

/* the codes EXPEVT gives the exceptions the screen takes */
#define CRASH_ADDRESS_READ 0x0e0
#define CRASH_ADDRESS_WRITE 0x100
#define CRASH_TRAPA 0x160
#define CRASH_ILLEGAL 0x180
#define CRASH_ILLEGAL_SLOT 0x1a0

/* the general registers a bank holds, R0 to R7 */
#define CRASH_BANK_REGISTERS 8
/* the general registers the record and a master's frame hold, R0 to R14 */
#define CRASH_REGISTERS 15

/* a master is the routine of sdk/shinobi/syIntIsr.src that takes an exception
 * or an interrupt. it saves every register in a frame and calls the event's
 * handler.
 *
 * the frame's words, from its lowest address. a master pushes the frame below
 * the stopped code's stack pointer. under syIntInit's mode
 * SYD_INT_MODE_INTSTACK, the outermost event's frame ends at syIntIntStack
 * instead. FRAME_SPC and FRAME_SSR hold the stopped code's PC and SR. FRAME_SR
 * holds the SR the master itself runs under. the frame's R0 to R7 are bank 1's,
 * since taking the event sets SR.RB */
enum crash_frame {
    FRAME_FPSCR,
    FRAME_FPUL,
    FRAME_SR,
    FRAME_R0,
    FRAME_MACL = FRAME_R0 + CRASH_REGISTERS,
    FRAME_MACH,
    FRAME_VBR,
    FRAME_GBR,
    FRAME_PR,
    FRAME_SPC,
    FRAME_SSR,
    FRAME_WORDS
};
/* the words of both banks of floats, which a master pushes below the frame. it
 * calls the handler with R15 at the lowest of them. the word just above the
 * highest holds the frame's address */
#define CRASH_FLOAT_WORDS 32

/* the characters on a line, the most callers a record holds, and the lines of a
 * screen */
#define CRASH_COLUMNS 44
#define CRASH_CALLERS 24
#define CRASH_LINES 17

/* the bytes of an overlay's name, which opens its head */
#define CRASH_NAME_BYTES 8

/* crash_stop.callers where sp addresses no word of main memory */
#define CRASH_NO_STACK (-1)

/* where the game stopped, and why */
struct crash_stop {
    /* the first line's words and number, and the name on the line after it, or
     * null */
    const char *what;
    uint32_t number;
    const char *name;
    /* the exception's code, or 0 for a hang or port_stop */
    uint32_t code;
    /* for an address error, the address its access asked for, from TEA. for
     * TRAPA, its number. the screen shows it for those codes alone */
    uint32_t address;
    /* the stopped code's PC, SR and R15. pc is the instruction that faulted,
     * the branch whose delay slot faulted, or the one after TRAPA. a hang's pc
     * is the instruction the vblank stopped. where the vblank stopped another
     * event's handler, it is the instruction the outermost event stopped.
     * port_stop's is where port_stop returns to */
    uint32_t pc;
    uint32_t sr;
    uint32_t sp;
    /* TRUE where pr and r hold the stopped code's PR and R0 to R14 */
    Bool registers;
    uint32_t pr;
    uint32_t r[CRASH_REGISTERS];
    /* mode is game_state.mode, and stage is what stage_current_id returns.
     * slot holds the stage slot's first bytes, where an overlay's head keeps
     * its name. the screen shows them as a name where they are a capital, then
     * capitals, digits, underscores and spaces */
    int32_t mode;
    int32_t stage;
    char slot[CRASH_NAME_BYTES];
    /* the words of the stopped code's stack that crash.c takes for return
     * addresses, in order from sp up. caller_offset gives each one's offset
     * from sp, in bytes. callers counts them, or is CRASH_NO_STACK */
    int callers;
    uint32_t caller_offset[CRASH_CALLERS];
    uint32_t caller[CRASH_CALLERS];
};

/* fill stop's pc, sr, pr and r from a master's frame, and set its registers to
 * TRUE. bank is bank 0's R0 to R7, as a handler reads them by STC Rn_BANK. r
 * takes them where the stopped code ran in bank 0, with SR.RB clear */
void crash_read_frame(struct crash_stop *stop,
                      const uint32_t frame[FRAME_WORDS],
                      const uint32_t bank[CRASH_BANK_REGISTERS]);

/* draw stop's lines, and blank the rest up to CRASH_LINES. no line of an
 * earlier screen stays */
void crash_show(const struct crash_stop *stop);
/* draw stop's first line and its name, then PC SR AND SP NOT KNOWN and stop's
 * MODE line, and blank the rest. it reads nothing else of stop */
void crash_show_unknown(const struct crash_stop *stop);
/* redraw stop's first line without its name, and its PC and SP lines, after its
 * report faulted. then draw REPORT FAULTED with code and pc: the report's own
 * exception, and the instruction that took it */
void crash_show_fault(const struct crash_stop *stop, uint32_t code,
                      uint32_t pc);

/* The console supplies the vector entry, the scanout address sampled on
 * each vblank and the capture of the interrupted context when a report is due.
 */
void crash_entry(void);
uint32_t crash_scanout(void);
void crash_capture_hang(uint32_t elapsed);

/* draw text on line, from 0 at the top. a line holds CRASH_COLUMNS characters:
 * text is cut there, and the line is blank past its end. crash.c defines it for
 * the Dreamcast, and crash_report.c for a host */
void crash_draw(int line, const char *text);

#ifdef DREAMCAST
#else
/* a host's screen: each line crash_draw last drew, as text */
extern char crash_screen[CRASH_LINES][CRASH_COLUMNS + 1];
/* FRAME_WORDS, for the tests to hold to the masters' pushes */
extern const int crash_frame_words;
#endif

#endif
