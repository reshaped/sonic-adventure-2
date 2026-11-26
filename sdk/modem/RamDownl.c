/* a download talks to the data pump through its registers. it writes a
 * module's RAM address to r16 and r17, low byte first. it sends the module's
 * bytes through tx_data, then writes 0 to abcode. the pump answers with
 * CHECKSUM_READY in r17 and the module's checksum in r16 */
#include <sg_xpt.h>
#include "RamDownl.h"
#include "dspdb.h"
#include "ram_image.h"
#include "ecdcutil.h"
#include "roc_k56.h"
#include "target.h"

/* in setup_timer's 10 ms ticks: the deadline of each wait on the pump, and
 * a fixed pause */
#define REPLY_TICKS 50
#define PAUSE_TICKS 2

/* the abcode of a request for a download into the controller's RAM, and
 * into the signal processor's. the pump echoes it in dtmf */
#define CONTROL_REQUEST 0x4f
#define SIGNAL_REQUEST 0x5f

#define CHECKSUM_READY 0xff

/* the codes the downloads report to data_pump_error, one for each wait,
 * test, search or open that fails */
#define DOWNLOAD_ERR_V90_ABCODE 20120
#define DOWNLOAD_ERR_NO_MODULE 20140
#define DOWNLOAD_ERR_ECHO 20160
#define DOWNLOAD_ERR_ABCODE 20180
#define DOWNLOAD_ERR_READY 20200
#define DOWNLOAD_ERR_CHECKSUM 20220
#define DOWNLOAD_ERR_OPEN 20240
#define DOWNLOAD_ERR_OPEN_V34 20260
#define DOWNLOAD_ERR_OPEN_V90 20300

Uint8 K56SavedConf;
Uint8 K56SavedOrg;
Uint8 K56SavedEarc;
Uint8 K56SavedArc;
Uint8 K56SavedData;
Uint8 K56SavedAutomode;
Uint8 K56SavedTxSpeed;
/* the open database, or null. RamDownload opens one of its own where none
 * is, and closes it before it returns */
static struct ram_database *ram_database;

static char download_v90(void);
#define CLOSE_DATABASE() (DspDbRelease(), ram_database = 0)

char InitialDownloadRamModule(void)
{
    ram_database = DspDbInitDataBase();
    if (!ram_database) {
        data_pump_error(DOWNLOAD_ERR_OPEN);
        return 0;
    }
#define STARTUP(bank, id, wait)                                                \
    if (!RamDownload((bank), (id), (wait), 0, 1)) {                            \
        CLOSE_DATABASE();                                                      \
        return 0;                                                              \
    }
    STARTUP(RAM_BANK_CONTROLLER, 0xeb, 0);
    STARTUP(RAM_BANK_CONTROLLER, 0xee, 0);
    STARTUP(RAM_BANK_CONTROLLER, 0xec, 0);
    STARTUP(RAM_BANK_CONTROLLER, 0x41, 0);
    STARTUP(RAM_BANK_CONTROLLER, 0xea, 0);
    STARTUP(RAM_BANK_SIGNAL, 0xf2, 0);
    STARTUP(RAM_BANK_SIGNAL, 0xfd, 0);
    STARTUP(RAM_BANK_SIGNAL, 0xf7, 0);
    STARTUP(RAM_BANK_SIGNAL, 0xf8, 0);
    STARTUP(RAM_BANK_SIGNAL, 0x01, 1);
    STARTUP(RAM_BANK_SIGNAL, 0xf0, 1);
#undef STARTUP
    CLOSE_DATABASE();
    return 1;
}

int PostV8DownloadRamModule(void)
{
    if (k56flags.v90_download && rcdpi->abcode.byte == V34_MODULE) {
        k56flags.v90_download = 0;
        if (!download_v90()) {
            return RAM_DOWNLOAD_FAILED;
        } else {
            write_dp_ram(RAM_MAX_RX_RATE, MaxV90ReceiveSpeed, 1);
            return RAM_DOWNLOAD_DONE;
        }
    } else if (k56flags.v34_download && rcdpi->abcode.byte == V34_MODULE) {
        k56flags.v34_download = 0;
        ram_database = DspDbInitDataBase();
        if (!ram_database) {
            data_pump_error(DOWNLOAD_ERR_OPEN_V34);
            return RAM_DOWNLOAD_FAILED;
        }
        if (!RamDownload(RAM_BANK_CONTROLLER, V34_MODULE, 1, 0, 0)) {
            CLOSE_DATABASE();
            return RAM_DOWNLOAD_FAILED;
        } else {
            CLOSE_DATABASE();
            write_dp_ram(RAM_MAX_RX_RATE, MaxV34ReceiveSpeed, 1);
            write_dp_ram(RAM_MAX_TX_RATE, MaxV34TransmitSpeed, 1);
            return RAM_DOWNLOAD_DONE;
        }
    }
    return RAM_DOWNLOAD_WAITING;
}

static char download_v90(void)
{
    Uint32 deadline;

    ram_database = DspDbInitDataBase();
    if (!ram_database) {
        data_pump_error(DOWNLOAD_ERR_OPEN_V90);
        return 0;
    }
    if (!RamDownload(RAM_BANK_CONTROLLER, V34_MODULE, 1, 0, 0)) {
        CLOSE_DATABASE();
        return 0;
    }
    setup_timer(&deadline, REPLY_TICKS);
    while (rcdpi->abcode.byte != V90_MODULE) {
        if (timer_expired(deadline)) {
            /* @bug memory: a timeout leaves the database open. the branch at
             * 0x0caeaf04 returns past DspDbRelease's call at 0x0caeaf2c.
             * DspDbInitDataBase then returns null until the Chao area starts
             * again */
#ifdef NONMATCHING
            /* this side repairs the bug tagged above: it closes the database */
            CLOSE_DATABASE();
#endif
            data_pump_error(DOWNLOAD_ERR_V90_ABCODE);
            return 0;
        }
    }
    if (!RamDownload(RAM_BANK_CONTROLLER, V90_MODULE, 1, 0, 0)) {
        CLOSE_DATABASE();
        return 0;
    }
    CLOSE_DATABASE();
    return 1;
}

char RamDownload(Uint8 bank, Uint8 id, char wait, Uint8 custom, char request)
{
    /* @hack spelling: most tests compare opened with 1, where a test of it
     * would do */
    Uint8 opened = 0;
    char success;
    Uint16 count;
    Uint16 index;
    Uint32 sent;
    Uint32 deadline;
    struct ram_module *table;
    struct ram_module *module;
    Uint16 address;
    Uint8 request_code;

    if (!ram_database) {
        opened = 1;
        ram_database = DspDbInitDataBase();
        if (!ram_database) {
            data_pump_error(DOWNLOAD_ERR_OPEN);
            return 0;
        }
    }
    success = 1;
    /* @bug memory: where custom is set, the search runs on an unset table
     * and count. the branches at 0x0caeafb4 and 0x0caeafe4 skip setting
     * both, and the search reads an id through table at 0x0caeb00e */
#ifdef NONMATCHING
    /* this side repairs the bug tagged above: a custom download fails with
     * DOWNLOAD_ERR_NO_MODULE, and closes the database it opened */
    if (custom) {
        if (opened) {
            CLOSE_DATABASE();
        }
        data_pump_error(DOWNLOAD_ERR_NO_MODULE);
        return 0;
    }
#endif
    if (bank == RAM_BANK_CONTROLLER) {
        request_code = CONTROL_REQUEST;
        /* NOLINTNEXTLINE(clang-diagnostic-sometimes-uninitialized) */
        if (!custom) {
            count = CONTROL_MODULES;
            table = &ram_database->modules[0];
        }
    } else {
        request_code = SIGNAL_REQUEST;
        /* NOLINTNEXTLINE(clang-diagnostic-sometimes-uninitialized) */
        if (!custom) {
            count = SIGNAL_MODULES;
            table = &ram_database->modules[CONTROL_MODULES];
        }
    }
    for (index = 0; index < count; index++) {
        if (id == table[index].id) {
            module = &table[index];
            break;
        }
    }
    if (index == count) {
        if (opened == 1) {
            CLOSE_DATABASE();
            opened = 0;
        }
        data_pump_error(DOWNLOAD_ERR_NO_MODULE);
        success = 0;
    } else {
        if (request) {
            rcdpi->abcode.byte = request_code;
            setup_timer(&deadline, REPLY_TICKS);
            while (request_code != rcdpi->dtmf.byte) {
                if (timer_expired(deadline)) {
                    if (opened == 1) {
                        CLOSE_DATABASE();
                    }
                    data_pump_error(DOWNLOAD_ERR_ECHO);
                    return 0;
                }
            }
        } else {
            setup_timer(&deadline, PAUSE_TICKS);
            while (!timer_expired(deadline)) {
            }
            if (rcdpi->abcode.byte != id) {
                if (opened == 1) {
                    CLOSE_DATABASE();
                }
                data_pump_error(DOWNLOAD_ERR_ABCODE);
                return 0;
            }
        }
        address = module->address;
        rcdpi->r16.byte = address;
        rcdpi->r17.byte = address >> 8;
        for (sent = 0; sent < module->size;) {
            if (rcdpi->r0d.bit.tx_room) {
                rcdpi->tx_data.byte =
                    ram_database->payload[module->offset + sent];
                sent++;
            }
        }
        setup_timer(&deadline, PAUSE_TICKS);
        while (!timer_expired(deadline)) {
        }
        /* @hack redundant: the test of wait, whose arms both write 0 to
         * abcode, where one write would do */
        if (wait) {
            rcdpi->abcode.byte = 0;
        } else {
            rcdpi->abcode.byte = 0;
        }
        setup_timer(&deadline, REPLY_TICKS);
        /* @bug logic: a pump that never answers hangs the download. the loop
         * at 0x0caeb144 tests r17 alone, and timer_expired runs once it ends,
         * at 0x0caeb150 */
        while (rcdpi->r17.byte != CHECKSUM_READY) {
#ifdef NONMATCHING
            /* this side repairs the bug tagged above: the wait ends at the
             * deadline */
            if (timer_expired(deadline)) {
                break;
            }
#endif
        }
        if (timer_expired(deadline)) {
            if (opened == 1) {
                CLOSE_DATABASE();
            }
            data_pump_error(DOWNLOAD_ERR_READY);
            return 0;
        }
        if (rcdpi->r16.byte != module->checksum) {
            data_pump_error(DOWNLOAD_ERR_CHECKSUM);
            success = 0;
        }
    }
    if (opened == 1) {
        CLOSE_DATABASE();
    }
    return success;
}

/* NOLINTNEXTLINE(misc-use-internal-linkage) */
void init_RamDownl_cc_f(void)
{
    ram_database = 0;
}

/* the member's entry in DSG_INIT_, the table of functions the Chao area
 * calls as it starts */
#pragma section _INIT_
/* NOLINTNEXTLINE(misc-use-internal-linkage) */
void (*init_RamDownlcc)(void) = init_RamDownl_cc_f;
#pragma section
