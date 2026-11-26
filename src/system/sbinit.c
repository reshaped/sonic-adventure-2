/* the system's start and end: the hardware, the heap, Ninja, the pads, the
 * clock and the GD file system */
#include <machine.h>
#include <ninja.h>
#include <sg_syhw.h>
#include <sg_syrtc.h>
#include <sg_sysrt.h>
#include <sg_xpt.h>
#include <shinobi.h>

#include "system/rom_system.h"
#include "system/sbinit.h"

#ifdef NONMATCHING
/* the crash screen of PORTS.md, which the shipped game lacks */
#include "engine/dc/crash.h"
#endif

/* the heap takes main memory's last 4 MB */
#define HEAP_SIZE 0x400000
/* REGION_BITS are an address's top three bits, which pick its region of the
 * address space. MAIN_MEMORY_END is main memory's end without them, and
 * MEMORY_END that end in _BSG_END's region */
#define REGION_BITS 0xe0000000
#define MAIN_MEMORY_END 0x0d000000
#define MEMORY_END ((((Uint32)_BSG_END) & REGION_BITS) | MAIN_MEMORY_END)

/* the bytes kdInitSystem's recvbuf takes: PAD_DEVICE_BYTES for each of
 * PAD_DEVICES devices in each of PAD_FRAMES frames. the send buffer has as
 * many */
#define PAD_DEVICE_BYTES 1024
#define PAD_DEVICES 24
#define PAD_FRAMES 2
#define PAD_BUFFER_SIZE (PAD_DEVICE_BYTES * PAD_DEVICES * PAD_FRAMES)

/* the GD file system's limits: open files, directory entries and tries at
 * its start */
#define GD_FILES 8
#define GD_ENTRIES 1800
#define GD_TRIES 8

/* the boundary each buffer below starts on. each holds BUFFER_ALIGN bytes past
 * its size, so that its start can move up to one. Kaede, the peripheral driver,
 * uses pad_recv_buffer and pad_send_buffer from their first boundary */
#define BUFFER_ALIGN 32
/* the first boundary past buffer's start, BUFFER_ALIGN on where the start is
 * one already. sbInitSystem moves gd_work and gd_directory to it */
#define PAST_BOUNDARY(buffer)                                                  \
    ((void *)(((Uint32)(buffer) & ~(BUFFER_ALIGN - 1)) + BUFFER_ALIGN))

static Uint8 pad_recv_buffer[PAD_BUFFER_SIZE + BUFFER_ALIGN];
static Uint8 pad_send_buffer[PAD_BUFFER_SIZE + BUFFER_ALIGN];
static Uint8 gd_work[GDFS_WORK_SIZE(GD_FILES) + BUFFER_ALIGN];
static Uint8 gd_directory[GDFS_DIRREC_SIZE(GD_ENTRIES) + BUFFER_ALIGN];
static Uint8 gd_toc[GDFS_TOC_SIZE];

void sbInitSystem(Int mode, Int frame, Int count)
{
    void *work;
    void *directory;
    Sint32 err;
    int tries;

    set_imask(IMASK_ALL);
    rom_system_reload();
    syHwInit();
    syMallocInit((void *)(MEMORY_END - HEAP_SIZE), HEAP_SIZE);
    njInitSystem(mode, frame, count);
    syHwInit2();
#ifdef NONMATCHING
    /* the crash screen takes its exception codes once the system is up */
    crash_install();
#endif
    pdInitPeripheral(PDD_PLOGIC_ACTIVE, pad_recv_buffer, pad_send_buffer);
    syRtcInit();
    set_imask(0);
    work = PAST_BOUNDARY(gd_work);
    directory = PAST_BOUNDARY(gd_directory);
    for (tries = GD_TRIES; tries > 0; tries--) {
        err = gdFsInit(GD_FILES, work, GD_ENTRIES, directory);
        if (err == GDD_ERR_TRAYOPEND || err == GDD_ERR_UNITATTENT) {
            sbExitSystem();
            syBtExit();
        } else if (err == GDD_ERR_OK) {
            break;
        }
    }
    if (tries == 0) {
        sbExitSystem();
        syBtExit();
    }
    gdFsGetToc(GDFS_TOC_HD_AREA, gd_toc);
    gdFsChangeDir("SONIC2");
}

void sbExitSystem(void)
{
#ifdef NONMATCHING
    /* the crash screen gives back the exception codes it took */
    crash_remove();
#endif
    gdFsFinish();
    syRtcFinish();
    pdExitPeripheral();
    njExitSystem();
    syMallocFinish();
    syHwFinish();
    set_imask(IMASK_ALL);
}
