/* crash_report.h's lines, and its reading of a master's frame. nothing here
 * divides or calls a library. so an exception or the vblank's interrupt can run
 * it, whatever the game was doing */
#include "crash_report.h"

#include <sg_xpt.h>
#include <stddef.h>
#include <stdint.h>

#ifdef DREAMCAST
#else
char crash_screen[CRASH_LINES][CRASH_COLUMNS + 1];
const int crash_frame_words = FRAME_WORDS;

void crash_draw(int line, const char *text)
{
    int column;

    for (column = 0; column < CRASH_COLUMNS && text[column]; column++) {
        crash_screen[line][column] = text[column];
    }
    crash_screen[line][column] = 0;
}
#endif

/* SR.RB, set while the CPU runs in register bank 1 */
#define SR_RB 0x20000000

#define REGISTERS_A_LINE 4
#define CALLERS_A_LINE 3
/* the hex digits of a word, and of a caller's offset from SP */
#define WORD_DIGITS 8
#define OFFSET_DIGITS 3

/* the powers of ten a decimal number's digits stand for, the highest first */
static const uint32_t tens[] = {
    1000000000, 100000000, 10000000, 1000000, 100000, 10000, 1000, 100, 10, 1};

void crash_read_frame(struct crash_stop *stop,
                      const uint32_t frame[FRAME_WORDS],
                      const uint32_t bank[CRASH_BANK_REGISTERS])
{
    int i;

    stop->pc = frame[FRAME_SPC];
    stop->sr = frame[FRAME_SSR];
    stop->pr = frame[FRAME_PR];
    for (i = 0; i < CRASH_REGISTERS; i++) {
        stop->r[i] = i < CRASH_BANK_REGISTERS && !(stop->sr & SR_RB)
                         ? bank[i]
                         : frame[FRAME_R0 + i];
    }
    stop->registers = TRUE;
}

/* each writer below writes at to, no further than end, and returns where the
 * next character goes */

/* from, in capitals */
static char *capitals(char *to, const char *from, const char *end)
{
    while (*from && to < end) {
        char c = *from++;

        *to++ = (char)(c >= 'a' && c <= 'z' ? c - 'a' + 'A' : c);
    }
    return to;
}

/* value's low hex digits, as many as digits says, the highest first */
static char *hex(char *to, uint32_t value, int digits, const char *end)
{
    int shift;

    for (shift = (digits - 1) * 4; shift >= 0 && to < end; shift -= 4) {
        uint32_t digit = (value >> shift) & 15;

        *to++ = (char)(digit < 10 ? '0' + digit : 'a' + digit - 10);
    }
    return to;
}

/* value in decimal, behind a hyphen where it is negative. each digit counts the
 * subtractions of its power of ten, since nothing here divides */
static char *decimal(char *to, int32_t value, const char *end)
{
    uint32_t left = value < 0 ? 0 - (uint32_t)value : (uint32_t)value;
    size_t i = 0;

    if (value < 0) {
        to = capitals(to, "-", end);
    }
    while (i < sizeof tens / sizeof tens[0] - 1 && left < tens[i]) {
        i++;
    }
    for (; i < sizeof tens / sizeof tens[0] && to < end; i++) {
        char digit = '0';

        while (left >= tens[i]) {
            left -= tens[i];
            digit++;
        }
        *to++ = digit;
    }
    return to;
}

/* name and a space after it. a space goes before name too, unless to is at
 * start */
static char *label(char *to, const char *start, const char *name,
                   const char *end)
{
    if (to != start) {
        to = capitals(to, " ", end);
    }
    to = capitals(to, name, end);
    return capitals(to, " ", end);
}

/* label's text, then value in eight hex digits */
static char *field(char *to, const char *start, const char *name,
                   uint32_t value, const char *end)
{
    return hex(label(to, start, name, end), value, WORD_DIGITS, end);
}

/* draw stop's what and number on line 0. where with_name is nonzero and stop's
 * name isn't null, draw stop's name below them. return the next line */
static int show_what(const struct crash_stop *stop, int with_name)
{
    char text[CRASH_COLUMNS + 1];
    const char *end = text + CRASH_COLUMNS;
    char *to;
    int line = 0;

    to = capitals(capitals(text, stop->what, end), " ", end);
    to = hex(to, stop->number, WORD_DIGITS, end);
    *to = 0;
    crash_draw(line++, text);
    if (with_name && stop->name != NULL) {
        to = capitals(text, stop->name, end);
        *to = 0;
        crash_draw(line++, text);
    }
    return line;
}

/* draw stop's PC line and SP line from line on, and return the next line */
static int show_where(int line, const struct crash_stop *stop)
{
    char text[CRASH_COLUMNS + 1];
    const char *end = text + CRASH_COLUMNS;
    char *to;

    to = field(text, text, "PC", stop->pc, end);
    if (stop->registers) {
        to = field(to, text, "PR", stop->pr, end);
    }
    to = field(to, text, "SR", stop->sr, end);
    *to = 0;
    crash_draw(line++, text);
    to = field(text, text, "SP", stop->sp, end);
    if (stop->code == CRASH_ADDRESS_READ || stop->code == CRASH_ADDRESS_WRITE) {
        to = field(to, text, "ADDR", stop->address, end);
    } else if (stop->code == CRASH_TRAPA) {
        to = field(to, text, "TRAPA", stop->address, end);
    }
    *to = 0;
    crash_draw(line++, text);
    return line;
}

/* whether slot reads as an overlay's name, as crash_stop.slot describes one */
static int is_name(const char slot[CRASH_NAME_BYTES])
{
    size_t i;

    if (slot[0] < 'A' || slot[0] > 'Z') {
        return 0;
    }
    for (i = 1; i < CRASH_NAME_BYTES; i++) {
        char c = slot[i];

        if (!((c >= 'A' && c <= 'Z') || (c >= '0' && c <= '9') || c == '_' ||
              c == ' ')) {
            return 0;
        }
    }
    return 1;
}

static int show_game(int line, const struct crash_stop *stop)
{
    char text[CRASH_COLUMNS + 1];
    const char *end = text + CRASH_COLUMNS;
    char *to;
    char slot[CRASH_NAME_BYTES + 1] = "?";
    size_t i;

    if (is_name(stop->slot)) {
        for (i = 0; i < CRASH_NAME_BYTES; i++) {
            slot[i] = stop->slot[i];
        }
        slot[i] = 0;
    }
    to = decimal(label(text, text, "MODE", end), stop->mode, end);
    to = decimal(label(to, text, "STAGE", end), stop->stage, end);
    to = capitals(label(to, text, "SLOT", end), slot, end);
    *to = 0;
    crash_draw(line++, text);
    return line;
}

/* draw stop's R0 to R14 from line on, REGISTERS_A_LINE to a line behind the
 * first one's number */
static int show_registers(int line, const struct crash_stop *stop)
{
    char text[CRASH_COLUMNS + 1];
    const char *end = text + CRASH_COLUMNS;
    char *to = text;
    int i;

    for (i = 0; i < CRASH_REGISTERS; i++) {
        if (i % REGISTERS_A_LINE == 0) {
            to = decimal(capitals(text, "R", end), i, end);
        }
        to = hex(capitals(to, " ", end), stop->r[i], WORD_DIGITS, end);
        if (i % REGISTERS_A_LINE == REGISTERS_A_LINE - 1 ||
            i == CRASH_REGISTERS - 1) {
            *to = 0;
            crash_draw(line++, text);
        }
    }
    return line;
}

static int show_callers(int line, const struct crash_stop *stop)
{
    char text[CRASH_COLUMNS + 1];
    const char *end = text + CRASH_COLUMNS;
    char *to = text;
    int i;

    if (stop->callers == CRASH_NO_STACK) {
        crash_draw(line++, "NO STACK AT SP");
        return line;
    }
    for (i = 0; i < stop->callers; i++) {
        if (i != 0 && i % CALLERS_A_LINE == 0) {
            *to = 0;
            crash_draw(line++, text);
            to = text;
        }
        if (to != text) {
            to = capitals(to, "  ", end);
        }
        to = hex(to, stop->caller_offset[i], OFFSET_DIGITS, end);
        to = hex(capitals(to, " ", end), stop->caller[i], WORD_DIGITS, end);
    }
    if (to != text) {
        *to = 0;
        crash_draw(line++, text);
    }
    return line;
}

/* blank each line from line down */
static void blank_from(int line)
{
    while (line < CRASH_LINES) {
        crash_draw(line++, "");
    }
}

void crash_show(const struct crash_stop *stop)
{
    int line = show_what(stop, 1);

    line = show_where(line, stop);
    line = show_game(line, stop);
    if (stop->registers) {
        line = show_registers(line, stop);
    }
    blank_from(show_callers(line, stop));
}

void crash_show_unknown(const struct crash_stop *stop)
{
    int line = show_what(stop, 1);

    crash_draw(line++, "PC SR AND SP NOT KNOWN");
    blank_from(show_game(line, stop));
}

void crash_show_fault(const struct crash_stop *stop, uint32_t code, uint32_t pc)
{
    char text[CRASH_COLUMNS + 1];
    const char *end = text + CRASH_COLUMNS;
    char *to;
    int line = show_where(show_what(stop, 0), stop);

    to = field(text, text, "REPORT FAULTED", code, end);
    to = field(to, text, "PC", pc, end);
    *to = 0;
    crash_draw(line, text);
}
