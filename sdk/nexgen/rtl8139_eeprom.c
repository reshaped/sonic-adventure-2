#include <sg_xpt.h>
#include "../network/g2_io.h"
#include "../nexgen/ngos_ETD_DLC.h"
#include "../nexgen/rtl8139.h"
#include "../nexgen/rtl8139_eeprom.h"

/* commands go out from the top bit of a Uint32 */
#define EE_SHIFT_BITS 32
/* each command's start bit and 2-bit opcode, from the top bit. EE_ERASE_ALL,
 * EE_ENABLE and EE_DISABLE share opcode 00, and the address's top 2 bits tell
 * them apart */
#define EE_READ 0xc0000000U
#define EE_WRITE 0xa0000000U
#define EE_ERASE 0xe0000000U
#define EE_ERASE_ALL 0x90000000U
#define EE_ENABLE 0x98000000U
#define EE_DISABLE 0x80000000U
/* a command's bits with a 6-bit address and with an 8-bit one. a write's
 * data bits follow its address */
#define EE_NARROW_BITS 9
#define EE_WIDE_BITS 11
#define EE_DATA_BITS 16
#define EE_READY_POLLS 0x400000U
#define EE_POLL_WAIT 1 /* microseconds */

static Uint16 eeprom_receive(void);
static void eeprom_send(Sint32 bits, Uint32 command);
static Sint32 eeprom_wait_ready(void);

Uint16 rtl8139_eeprom_read(Bool wide, Uint8 address)
{
    Uint8 saved;
    Uint32 command;
    Uint16 value;
    saved = ETD_G2Read8(RTL_CFG9346);
    ETD_G2Write8(RTL_CFG9346, saved | RTL_EEPROM_CS);
    /* @hack spelling: the command built in command, where each call could
     * take its expression */
    command = EE_READ;
    if (!wide) {
        command |= address << (EE_SHIFT_BITS - EE_NARROW_BITS);
        eeprom_send(EE_NARROW_BITS, command);
    } else {
        command |= address << (EE_SHIFT_BITS - EE_WIDE_BITS);
        eeprom_send(EE_WIDE_BITS, command);
    }
    value = eeprom_receive();
    ETD_G2Write8(RTL_CFG9346, saved);
    return value;
}

Sint32 rtl8139_eeprom_write(Bool wide, Uint8 address, Uint16 value)
{
    Uint8 saved;
    Uint32 command;
    Sint32 result;
    saved = ETD_G2Read8(RTL_CFG9346);
    ETD_G2Write8(RTL_CFG9346, saved | RTL_EEPROM_CS);
    /* @hack spelling: the command built in command, where each call could
     * take its expression */
    command = EE_WRITE;
    if (!wide) {
        command |= address << (EE_SHIFT_BITS - EE_NARROW_BITS);
        command |= value << (EE_SHIFT_BITS - EE_NARROW_BITS - EE_DATA_BITS);
        eeprom_send(EE_NARROW_BITS + EE_DATA_BITS, command);
    } else {
        command |= address << (EE_SHIFT_BITS - EE_WIDE_BITS);
        command |= value << (EE_SHIFT_BITS - EE_WIDE_BITS - EE_DATA_BITS);
        eeprom_send(EE_WIDE_BITS + EE_DATA_BITS, command);
    }
    ETD_G2Write8(RTL_CFG9346, saved);
    /* @hack spelling: the call's result held in result, where the return
     * could take the call */
    result = eeprom_wait_ready();
    return result;
}

Sint32 rtl8139_eeprom_erase(Bool wide, Uint8 address)
{
    Uint8 saved;
    Uint32 command;
    Sint32 result;
    saved = ETD_G2Read8(RTL_CFG9346);
    ETD_G2Write8(RTL_CFG9346, saved | RTL_EEPROM_CS);
    /* @hack spelling: the command built in command, where each call could
     * take its expression */
    command = EE_ERASE;
    if (!wide) {
        command |= address << (EE_SHIFT_BITS - EE_NARROW_BITS);
        eeprom_send(EE_NARROW_BITS, command);
    } else {
        command |= address << (EE_SHIFT_BITS - EE_WIDE_BITS);
        eeprom_send(EE_WIDE_BITS, command);
    }
    ETD_G2Write8(RTL_CFG9346, saved);
    /* @hack spelling: the call's result held in result, where the return
     * could take the call */
    result = eeprom_wait_ready();
    return result;
}

Sint32 rtl8139_eeprom_erase_all(Bool wide)
{
    Uint8 saved;
    Uint32 command;
    Sint32 result;
    saved = ETD_G2Read8(RTL_CFG9346);
    ETD_G2Write8(RTL_CFG9346, saved | RTL_EEPROM_CS);
    /* @hack spelling: command set in each branch, where one assignment above
     * the test would do */
    if (!wide) {
        command = EE_ERASE_ALL;
        eeprom_send(EE_NARROW_BITS, command);
    } else {
        command = EE_ERASE_ALL;
        eeprom_send(EE_WIDE_BITS, command);
    }
    ETD_G2Write8(RTL_CFG9346, saved);
    /* @hack spelling: the call's result held in result, where the return
     * could take the call */
    result = eeprom_wait_ready();
    return result;
}

void rtl8139_eeprom_enable_write(Bool wide)
{
    Uint8 saved;
    Uint32 command;
    saved = ETD_G2Read8(RTL_CFG9346);
    ETD_G2Write8(RTL_CFG9346, saved | RTL_EEPROM_CS);
    /* @hack spelling: command set in each branch, where one assignment above
     * the test would do */
    if (!wide) {
        command = EE_ENABLE;
        eeprom_send(EE_NARROW_BITS, command);
    } else {
        command = EE_ENABLE;
        eeprom_send(EE_WIDE_BITS, command);
    }
    ETD_G2Write8(RTL_CFG9346, saved);
}

void rtl8139_eeprom_disable_write(Bool wide)
{
    Uint8 saved;
    Uint32 command;
    saved = ETD_G2Read8(RTL_CFG9346);
    ETD_G2Write8(RTL_CFG9346, saved | RTL_EEPROM_CS);
    /* @hack spelling: EE_DISABLE held in command, where each call could take
     * EE_DISABLE */
    command = EE_DISABLE;
    if (!wide) {
        eeprom_send(EE_NARROW_BITS, command);
    } else {
        eeprom_send(EE_WIDE_BITS, command);
    }
    ETD_G2Write8(RTL_CFG9346, saved);
}

static Uint16 eeprom_receive(void)
{
    Uint16 value = 0;
    Uint8 saved = ETD_G2Read8(RTL_CFG9346);
    Sint32 bit;
    Uint8 sample;
    for (bit = EE_DATA_BITS - 1; bit >= 0; bit--) {
        ETD_G2Write8(RTL_CFG9346, saved | RTL_EEPROM_CLOCK);
        ETD_G2Write8(RTL_CFG9346, saved);
        /* @hack spelling: the call's result held in sample, where the test
         * could take the call */
        sample = ETD_G2Read8(RTL_CFG9346);
        if (sample & RTL_EEPROM_DATA_OUT) {
            value |= 1 << bit;
        }
    }
    return value;
}

static void eeprom_send(Sint32 bits, Uint32 command)
{
    Uint8 saved = ETD_G2Read8(RTL_CFG9346);
    /* @hack spelling: bit set in its declaration, where the for's start would
     * do */
    Sint32 bit = 0;
    Uint32 mask = (Uint32)1 << (EE_SHIFT_BITS - 1);
    for (; bit < bits; bit++) {
        if (command & mask) {
            ETD_G2Write8(RTL_CFG9346, saved | RTL_EEPROM_DATA_IN);
            ETD_G2Write8(RTL_CFG9346,
                         saved | RTL_EEPROM_DATA_IN | RTL_EEPROM_CLOCK);
        } else {
            ETD_G2Write8(RTL_CFG9346, saved);
            ETD_G2Write8(RTL_CFG9346, saved | RTL_EEPROM_CLOCK);
        }
        ETD_G2Write8(RTL_CFG9346, saved);
        mask >>= 1;
    }
}

static Sint32 eeprom_wait_ready(void)
{
    Uint8 saved;
    Uint8 sample;
    Uint32 attempts;
    saved = ETD_G2Read8(RTL_CFG9346);
    ETD_G2Write8(RTL_CFG9346, saved | RTL_EEPROM_CS);
    for (attempts = 0; attempts < EE_READY_POLLS; attempts++) {
        /* @hack spelling: the call's result held in sample, where the test
         * could take the call */
        sample = ETD_G2Read8(RTL_CFG9346);
        if (sample & RTL_EEPROM_DATA_OUT) {
            break;
        }
        ETD_DlcWait(EE_POLL_WAIT);
    }
    ETD_G2Write8(RTL_CFG9346, saved);
    if (attempts == EE_READY_POLLS) {
        return -1;
    }
    return 0;
}
