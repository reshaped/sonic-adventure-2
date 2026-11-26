/* the check of the expansion devices on the G2 bus */
#include <machine.h>
#include <sg_syg2.h>
#include <sg_xpt.h>
#include <shinobi.h>

#define PRESENT (-1)
#define INITIALIZED (-1)
#define UNAVAILABLE (-2)
#define UNKNOWN (-1)
/* the modem slot's info and type bytes */
#define MODEM_INFO ((void *)0xa0600000)
#define MODEM_TYPE ((void *)0xa0600004)
/* the type byte's codes for a modem and for device 5 */
#define TYPE_BYTE_MODEM 0x10
#define TYPE_BYTE_5 0x12
/* a modem's speed code, in the info byte's MODEM_SPEED bits */
#define MODEM_SPEED 0xc0
#define SPEED_33600 0x00
#define SPEED_56K 0x40
/* a modem's country code, in the info byte's MODEM_COUNTRY bits */
#define MODEM_COUNTRY 0x0f
#define COUNTRY_USA 0
#define COUNTRY_EU 2
#define COUNTRY_JAPAN 6
#define COUNTRY_AUSTRALIA 13
/* syExtChkConnect's device codes 5 and 6, which shinobi.h leaves unnamed */
#define TYPE_5 5
#define TYPE_6 6
/* the registers of the device in slot n, as sg_exint.h numbers the slots */
#define DEVICE_STRIDE 0x400UL
#define DEVICE_REGISTERS(n) (0xa1000000UL + ((n) * DEVICE_STRIDE))
#define ZIP_BASE DEVICE_REGISTERS(1)
#define USB_BASE DEVICE_REGISTERS(2)
#define LAN_BASE DEVICE_REGISTERS(5)
#define TYPE6_BASE DEVICE_REGISTERS(6)
/* "GAPS", the first word of a device that ext_find_gaps accepts */
#define GAPS_ID_VALUE 0x53504147UL
/* the offset of a device's interrupt word. a write there sets the
 * interrupt's mask, and INTERRUPT_DISABLED masks it */
#define DEVICE_INTERRUPT 0x14
#define INTERRUPT_DISABLED 0
/* the offset of the word ext_gaps_write18 writes, and the word's fixed
 * bits. its other bits are SLOT_BYTE's bits of the device's address, moved
 * up 8 */
#define DEVICE_WORD18 0x18
#define GAPS_WORD18 0x5a00a500UL
#define SLOT_BYTE 0xff00UL
/* the system bus's error status. writing 1 to a bit clears it */
#ifdef NONMATCHING
/* this side reads the register as volatile, the plain form the hack tag below
 * gives, so GCC reads it at each use */
#define INT_ERROR_STATUS (*(volatile Uint32 *)0xa05f6908)
#else
/* @hack type: the register as a plain Uint32, where a volatile Uint32
 * would do */
#define INT_ERROR_STATUS (*(Uint32 *)0xa05f6908)
#endif
/* the error status bit an access to an empty slot sets */
#define G2_ERROR 0x08000000UL

/* the member exports its build string, though no header of the SDK declares
 * it */
/* NOLINTNEXTLINE(misc-use-internal-linkage) */
const char *sygExtChkBuild = "\nsyExtChk Ver 2.04 Build:Dec 14 2000 15:49:52\n";

static Sint32 ext_initialized = 0;
static Sint32 ext_modem_speed = UNAVAILABLE;
static Sint32 ext_modem_country = UNAVAILABLE;
static Sint32 ext_modem = 0;
static Sint32 ext_type5 = 0;
static Sint32 ext_mmb_zip = 0;
static Sint32 ext_mmb_usb = 0;
static Sint32 ext_lan = 0;
static Sint32 ext_type6 = 0;

static void ext_clear_errors(void);
static Bool ext_find_gaps(Uint32 base);
static Bool ext_disable_interrupt(Uint32 base);
static Bool ext_gaps_write18(Uint32 base);

#define WRITE_DEVICE_WORDS(base)                                               \
    do {                                                                       \
        ext_disable_interrupt(base);                                           \
        ext_gaps_write18(base);                                                \
    } while (0)

Sint32 syExtChkInit(void)
{
    Uint8 info;
    Uint8 type;
    Uint8 speed;
    Uint8 country;
    SYE_G2_ERR status;

    if (ext_initialized) {
        return -1;
    }
    ext_clear_errors();
    status = syG2Read(MODEM_TYPE, &type, SYE_G2_DATA_WIDTH_8BIT, 1,
                      SYE_G2_ADR_MODE_FIX, SYE_G2_ADR_MODE_FIX);
    if (status != SYE_G2_ERR_NOTHING) {
        return -1;
    }
    status = syG2Read(MODEM_INFO, &info, SYE_G2_DATA_WIDTH_8BIT, 1,
                      SYE_G2_ADR_MODE_FIX, SYE_G2_ADR_MODE_FIX);
    if (status != SYE_G2_ERR_NOTHING) {
        return -1;
    }
    if (type == TYPE_BYTE_MODEM) {
        ext_modem = PRESENT;
        speed = info & MODEM_SPEED;
        if (speed == SPEED_33600) {
            ext_modem_speed = SYD_EXT_CHK_MODEM_SPEED_33600;
        } else if (speed == SPEED_56K) {
            ext_modem_speed = SYD_EXT_CHK_MODEM_SPEED_56K;
        } else {
            ext_modem_speed = UNKNOWN;
        }
        country = info & MODEM_COUNTRY;
        if (country == COUNTRY_USA) {
            ext_modem_country = SYD_EXT_CHK_MODEM_COUNTRY_USA;
        } else if (country == COUNTRY_EU) {
            ext_modem_country = SYD_EXT_CHK_MODEM_COUNTRY_EU;
        } else if (country == COUNTRY_JAPAN) {
            ext_modem_country = SYD_EXT_CHK_MODEM_COUNTRY_JAPAN;
        } else if (country == COUNTRY_AUSTRALIA) {
            ext_modem_country = SYD_EXT_CHK_MODEM_COUNTRY_AUSTRALIA;
        } else {
            ext_modem_country = UNKNOWN;
        }
    }
    if (type == TYPE_BYTE_5) {
        ext_type5 = PRESENT;
    }
    if (ext_find_gaps(ZIP_BASE) == TRUE) {
        WRITE_DEVICE_WORDS(ZIP_BASE);
        ext_mmb_zip = PRESENT;
    }
    if (ext_find_gaps(USB_BASE) == TRUE) {
        WRITE_DEVICE_WORDS(USB_BASE);
        ext_mmb_usb = PRESENT;
    }
    if (ext_find_gaps(LAN_BASE) == TRUE) {
        WRITE_DEVICE_WORDS(LAN_BASE);
        ext_lan = PRESENT;
    }
    if (ext_find_gaps(TYPE6_BASE) == TRUE) {
        WRITE_DEVICE_WORDS(TYPE6_BASE);
        ext_type6 = PRESENT;
    }
    ext_clear_errors();
    ext_initialized = INITIALIZED;
    return 0;
}

Sint32 syExtChkExit(void)
{
    if (ext_initialized != INITIALIZED) {
        return -1;
    }
    if (ext_mmb_zip == PRESENT) {
        WRITE_DEVICE_WORDS(ZIP_BASE);
    }
    if (ext_mmb_usb == PRESENT) {
        WRITE_DEVICE_WORDS(USB_BASE);
    }
    if (ext_lan == PRESENT) {
        WRITE_DEVICE_WORDS(LAN_BASE);
    }
    if (ext_type6 == PRESENT) {
        WRITE_DEVICE_WORDS(TYPE6_BASE);
    }
    ext_clear_errors();
    ext_initialized = 0;
    ext_modem_speed = UNAVAILABLE;
    ext_modem_country = UNAVAILABLE;
    ext_modem = 0;
    ext_type5 = 0;
    ext_mmb_zip = 0;
    ext_mmb_usb = 0;
    ext_lan = 0;
    ext_type6 = 0;
    return 0;
}

Sint32 syExtChkConnect(Uint32 type)
{
    if (ext_initialized != INITIALIZED) {
        return UNAVAILABLE;
    }
    /* @hack spelling: an if and two returns for each device, where one
     * conditional return would do */
    if (type == SYD_EXT_CHK_TYPE_MODEM) {
        if (ext_modem == PRESENT) {
            return 0;
        }
        return -1;
    }
    if (type == TYPE_5) {
        if (ext_type5 == PRESENT) {
            return 0;
        }
        return -1;
    }
    if (type == SYD_EXT_CHK_TYPE_MMBZIP) {
        if (ext_mmb_zip == PRESENT) {
            return 0;
        }
        return -1;
    }
    if (type == SYD_EXT_CHK_TYPE_MMBUSB) {
        if (ext_mmb_usb == PRESENT) {
            return 0;
        }
        return -1;
    }
    if (type == SYD_EXT_CHK_TYPE_LAN) {
        if (ext_lan == PRESENT) {
            return 0;
        }
        return -1;
    }
    if (type == TYPE_6) {
        if (ext_type6 == PRESENT) {
            return 0;
        }
        return -1;
    }
    return UNAVAILABLE;
}

Sint32 syExtChkModemCountry(void)
{
    return ext_modem_country;
}

Sint32 syExtChkModemSpeed(void)
{
    return ext_modem_speed;
}

static void ext_clear_errors(void)
{
    Sint32 mask;
    Uint32 status;

    mask = get_imask();
    set_imask(IMASK_ALL);
    status = INT_ERROR_STATUS;
    INT_ERROR_STATUS = status | G2_ERROR;
    set_imask(mask);
}

static Bool ext_find_gaps(Uint32 base)
{
    Uint32 signature = 0;
    Sint32 mask;
    Uint32 error;
    SYE_G2_ERR status;
    /* @hack spelling: bus, a local copy of base as a pointer, where
     * (void *)base in the call would do */
    void *bus;

    ext_clear_errors();
    bus = (void *)base;
    status = syG2Read(bus, &signature, SYE_G2_DATA_WIDTH_32BIT, 1,
                      SYE_G2_ADR_MODE_ADD, SYE_G2_ADR_MODE_ADD);
    /* syG2Read has no result for a bus error, so its read succeeds at an
     * empty slot too. the slot is empty where the read set G2_ERROR */
    if (status == SYE_G2_ERR_NOTHING) {
        mask = get_imask();
        set_imask(IMASK_ALL);
        error = INT_ERROR_STATUS;
        set_imask(mask);
        if (error & G2_ERROR) {
            ext_clear_errors();
            return FALSE;
        }
        if (signature == GAPS_ID_VALUE) {
            return TRUE;
        }
    }
    return FALSE;
}

static Bool ext_disable_interrupt(Uint32 base)
{
    Uint32 value = INTERRUPT_DISABLED;
    Sint32 mask;
    SYE_G2_ERR status;

    mask = get_imask();
    set_imask(IMASK_ALL);
    status = syG2Write(&value, (void *)(base + DEVICE_INTERRUPT),
                       SYE_G2_DATA_WIDTH_32BIT, 1, SYE_G2_ADR_MODE_ADD,
                       SYE_G2_ADR_MODE_ADD);
    set_imask(mask);
    /* @hack spelling: an if and two returns, where returning
     * status == SYE_G2_ERR_NOTHING would do */
    if (status != SYE_G2_ERR_NOTHING) {
        return FALSE;
    }
    return TRUE;
}

static Bool ext_gaps_write18(Uint32 base)
{
    Uint32 value = ((base & SLOT_BYTE) << 8) | GAPS_WORD18;
    Sint32 mask;
    SYE_G2_ERR status;

    mask = get_imask();
    set_imask(IMASK_ALL);
    status = syG2Write(&value, (void *)(base + DEVICE_WORD18),
                       SYE_G2_DATA_WIDTH_32BIT, 1, SYE_G2_ADR_MODE_ADD,
                       SYE_G2_ADR_MODE_ADD);
    set_imask(mask);
    /* @hack spelling: an if and two returns, where returning
     * status == SYE_G2_ERR_NOTHING would do */
    if (status != SYE_G2_ERR_NOTHING) {
        return FALSE;
    }
    return TRUE;
}
