/* the address checker's serial console */
#include <sg_xpt.h>
#include <sy_int_chk.h>

#define STANDBY_CONTROL (*(volatile Uint8 *)0xffc00004)
#define SCI_CLOCK_STOP 1
#define CLOCK_SETTLE 0x50000
#define BAUD_SETTLE 0x80000
#define INIT_BAUD 40
/* SCI_PORT's bit that reads 1 while the SCK input is high */
#define SCI_SCK_HIGH 4
#define SCI_TX_EMPTY 0x80
#define SCI_TX_END 4
#define SCIF_TX_EMPTY 0x20
#define SCIF_TX_END 0x40
#define SCIF_RESET_FIFOS 6
#define SCIF_FIFO_TRIGGER 0x308
/* FIFO_COUNT's count of bytes received */
#define SCIF_RX_COUNT 0xff
/* BAUD's value at reset */
#define BAUD_RESET 0xff
#define SERIAL_TX_RX 0x30

static const Uint32 sci_base = 0xffe00000;
static const Uint32 scif_base = 0xffe80000;
static enum serial_channel console_channel;

/* a register of the SCI or the SCIF, by its width in bits and its offset */
#define SCI8(offset) (*(volatile Uint8 *)(sci_base + (offset)))
#define SCIF8(offset) (*(volatile Uint8 *)(scif_base + (offset)))
#define SCIF16(offset) (*(volatile Uint16 *)(scif_base + (offset)))
#define MODE 0
#define BAUD 4
#define CONTROL 8
#define TX 12
#define STATUS 16
#define RX 20
#define FIFO_CONTROL 24
#define FIFO_COUNT 28
#define SCI_PORT 28
#define SCIF_PORT 32
#define SCIF_LINE_STATUS 36

enum serial_channel syIntAdrChk_init(void)
{
    volatile int clock_wait;

    STANDBY_CONTROL &= ~SCI_CLOCK_STOP;
    /* a dummy read, which completes the store to STANDBY_CONTROL before the
     * wait */
    clock_wait = STANDBY_CONTROL;
    for (clock_wait = 0; clock_wait < CLOCK_SETTLE; clock_wait++) {
    }
    if (SCI8(SCI_PORT) & SCI_SCK_HIGH) {
        console_channel = CHANNEL_SCIF;
    } else {
        console_channel = CHANNEL_SCI;
    }
    switch (console_channel) {
    case CHANNEL_SCIF: {
        volatile int baud_wait;

        SCIF16(CONTROL) = 0;
        SCIF16(FIFO_CONTROL) = SCIF_RESET_FIFOS;
        SCIF16(CONTROL) = 0;
        SCIF16(MODE) = 0;
        SCIF8(BAUD) = INIT_BAUD;
        for (baud_wait = 0; baud_wait < BAUD_SETTLE; baud_wait++) {
        }
        SCIF16(FIFO_CONTROL) = SCIF_FIFO_TRIGGER;
        SCIF16(CONTROL) = SERIAL_TX_RX;
        break;
    }
    case CHANNEL_SCI:
    default: {
        volatile int baud_wait;

        SCI8(CONTROL) = 0;
        SCI8(CONTROL) = 0;
        SCI8(MODE) = 0;
        SCI8(BAUD) = INIT_BAUD;
        for (baud_wait = 0; baud_wait < BAUD_SETTLE; baud_wait++) {
        }
        SCI8(CONTROL) = SERIAL_TX_RX;
        break;
    }
    }
    return console_channel;
}

void syIntAdrChk_putchar(char character)
{
    /* @hack spelling: character through byte, a Uint8 copy, where character
     * would do */
    Uint8 byte = character;

    switch (console_channel) {
    case CHANNEL_SCIF:
        while ((SCIF16(STATUS) & SCIF_TX_EMPTY) != SCIF_TX_EMPTY) {
        }
        SCIF8(TX) = byte;
        SCIF16(STATUS) &= ~SCIF_TX_EMPTY;
        SCIF16(STATUS) &= ~SCIF_TX_END;
        while ((SCIF16(STATUS) & SCIF_TX_END) != SCIF_TX_END) {
        }
        break;
    case CHANNEL_SCI:
        while ((SCI8(STATUS) & SCI_TX_EMPTY) != SCI_TX_EMPTY) {
        }
        SCI8(TX) = byte;
        SCI8(STATUS) &= ~SCI_TX_EMPTY;
        while ((SCI8(STATUS) & SCI_TX_END) != SCI_TX_END) {
        }
        break;
    }
}

char syIntAdrChk_getchar(void)
{
#ifdef NONMATCHING
    /* this side repairs the bug tagged below: getchar returns 0 while SCK reads
     * low on the SCI */
    char character = 0;
#else
    char character;
#endif

    /* @bug uninitialized: returns an unset character while SCK reads low on
     * the SCI. the BF at 0x8c1707e4 skips the only store to it, and the
     * slot at 0x8c1707ea returns R5 as the caller left it */
    switch (console_channel) {
    case CHANNEL_SCIF:
        while ((SCIF16(FIFO_COUNT) & SCIF_RX_COUNT) == 0) {
        }
        character = SCIF8(RX);
        break;
    case CHANNEL_SCI:
        /* getchar reads no byte from the SCI. it gives '\r' while SCK reads
         * high */
        /* @hack spelling: the bit compared with SCI_SCK_HIGH, where testing
         * it alone, as syIntAdrChk_init does, would do */
        /* NOLINTNEXTLINE(clang-diagnostic-sometimes-uninitialized) */
        if ((SCI8(SCI_PORT) & SCI_SCK_HIGH) == SCI_SCK_HIGH) {
            character = '\r';
        }
        break;
    }
    return character;
}

void syIntAdrChk_finish(void)
{
    switch (console_channel) {
    case CHANNEL_SCIF:
        SCIF16(CONTROL) = 0;
        SCIF16(FIFO_CONTROL) = SCIF_RESET_FIFOS;
        SCIF16(MODE) = 0;
        SCIF8(BAUD) = BAUD_RESET;
        SCIF16(CONTROL) = 0;
        SCIF16(STATUS) = 0;
        SCIF16(FIFO_CONTROL) = 0;
        SCIF16(SCIF_PORT) = 0;
        SCIF16(SCIF_LINE_STATUS) = 0;
        break;
    case CHANNEL_SCI:
    default:
        break;
    }
    STANDBY_CONTROL |= SCI_CLOCK_STOP;
}

void syIntAdrChk_prints(const char *text)
{
    while (*text) {
        syIntAdrChk_putchar(*text++);
    }
}

void syIntAdrChk_printh(Uint32 number)
{
    int shift;
    Uint8 digit;

    for (shift = 28; shift >= 0; shift -= 4) {
        digit = (number >> shift) & 15;
        if (digit >= 10) {
            digit += 'A' - 10;
        } else {
            digit += '0';
        }
        syIntAdrChk_putchar(digit);
    }
}
