#include <machine.h>
#include <sh4scif.h>
#include <stdarg.h>
#include <stdio.h>
#include "net_debug.h"

static char receive_ring[256];
static char transmit_ring[256];
int dbDebug = 0;

void dbInit(void)
{
    scif_init(receive_ring, sizeof(receive_ring), transmit_ring,
              sizeof(transmit_ring));
    scif_open(BPS_57600);
}

#ifdef NONMATCHING
/* the GCC side writes each character as the formatter makes it, so no buffer
 * limits a message */
static int write_character(int character, FILE *stream)
{
    (void)stream;
    if (character == '\n') {
        scif_putq('\r');
    }
    scif_putq((char)character);
    return (unsigned char)character;
}
#else
static char message[DB_MESSAGE_BYTES];
#endif

void dbPrint(int category, const char *format, ...)
{
    int saved_mask;
    va_list arguments;
#ifdef NONMATCHING
    /* the stream this side's _fmtout below writes through write_character */
    FILE stream = {NULL, 0, NULL, 0, _IOWRITE};
#else
    char *cursor;
#endif
    if (dbDebug & category) {
        saved_mask = get_imask();
        set_imask(IMASK_ALL);
        va_start(arguments, format);
#ifdef NONMATCHING
        /* this side writes each character as _fmtout makes it, so no buffer
         * limits the message. the matching side's vsprintf fills message's
         * DB_MESSAGE_BYTES unchecked */
        _fmtout(PRINT_TO_STREAM, write_character, &stream, format, arguments);
#else
        vsprintf(message, format, arguments);
        for (cursor = message; *cursor != '\0'; cursor++) {
            if (*cursor == '\n') {
                scif_putq('\r');
            }
            scif_putq(*cursor);
        }
#endif
        va_end(arguments);
        set_imask(saved_mask);
    }
}
