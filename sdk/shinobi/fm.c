/* the flash memory manager: a bank's packets, kept on flash as a log and
 * cached in the caller's workspace */
#include <boot_flash.h>
#include <sg_fm.h>
#include <sg_xpt.h>

#define PACKET_SHIFT 6 /* log2 of FLASH_PACKET_BYTES: bytes to packets */
/* log2 of FLASH_BYTES_PER_MAP_PACKET: bytes to map packets */
#define MAP_SHIFT 15
/* a map byte's first bit, its top */
#define MAP_TOP_BIT 0x80
/* the bytes of a packet its CRC covers: its offset and data */
#define PACKET_CHECKED (FLASH_PACKET_BYTES - FLASH_CRC_BYTES)

/* FM_WRK.exec_mode's values: the request fmMngExecServer runs */
#define REQUEST_NONE 0
#define REQUEST_READ 1
#define REQUEST_WRITE 2
/* FM_WRK.exec_flag's values: the phases of a write, in the order
 * flash_write_step runs them */
#define PHASE_APPEND 0
#define PHASE_ERASE 1
#define PHASE_STORE 2
/* two results within a write, which never reach the caller. LOG_FULL is
 * flash_append_packet's for a full log, and takes the value sg_fm.h's
 * results leave out. PHASE_REMAINS is flash_write_step's while a phase of the
 * write is still to run */
#define LOG_FULL (-10)
#define PHASE_REMAINS 1

/* the open bank's record, which is flash_storage, or NULL with no bank
 * open */
static FM_WRK *flash_active = NULL;
static FM_WRK flash_storage;
/* the member exports this name, though no header of the SDK declares it */
/* NOLINTBEGIN(misc-use-internal-linkage) */
const char *const fmgBuild = "\nfmMng Ver 1.04 Build:Apr 26 1999 14:34:41\n";
/* NOLINTEND(misc-use-internal-linkage) */

static Bool flash_bytes_equal(const char *left, const char *right, Sint32 count)
{
    while (count-- > 0) {
        if (*left++ != *right++) {
            return FALSE;
        }
    }
    return TRUE;
}

/* copy bytes / 4 Sint32 words from source to destination, which must both
 * be aligned to 4 */
static void flash_copy_words(const void *source, void *destination,
                             Sint32 bytes)
{
    const Sint32 *from = source;
    Sint32 *to = destination;

    /* @hack spelling: bytes divided in place to count the words, where
     * Sint32 words = bytes / 4 would do */
    bytes /= 4;
    while (bytes-- > 0) {
        *to++ = *from++;
    }
}

static void flash_copy_bytes(const Uint8 *from, Uint8 *to, Sint32 count)
{
    while (count-- > 0) {
        *to++ = *from++;
    }
}

/* @hack type: the cache as Sint8, and -1 for an erased byte, where Uint8 and
 * 0xff would do */
static void flash_erase_cache(Sint8 *cache, Sint32 bytes)
{
    for (; bytes > 0; bytes--) {
        *cache++ = -1;
    }
}

/* the number of packets the log holds: the clear bits before the map's first
 * set bit */
static Sint32 flash_count_used(const Uint8 *map, Sint32 map_packets)
{
    Sint32 count = 0;
    Sint32 bytes;

    for (bytes = map_packets * FLASH_PACKET_BYTES; bytes > 0; bytes--) {
        /* @hack type: bits as Sint8, where Uint8 would do */
        Sint8 bits = *map++;
        Sint32 bit;

        for (bit = 8; bit > 0; bit--) {
            if (bits & MAP_TOP_BIT) {
                return count;
            }
            bits <<= 1;
            count++;
        }
    }
    return count;
}

/* write map for a log that holds used packets: used clear bits, then set
 * bits to its end */
/* @hack type: the map as Sint8, and -1 for a byte of set bits, where Uint8
 * and 0xff would do */
static Sint32 flash_make_map(Sint32 used, Sint8 *map, Sint32 map_packets)
{
    Sint32 bytes;
    Sint32 bit;

    for (bytes = map_packets * FLASH_PACKET_BYTES; bytes > 0; bytes--) {
        if (used >= 8) {
            *map++ = 0;
            used -= 8;
        } else if (used > 0) {
            Uint8 bits;

#ifdef NONMATCHING
            /* this side keeps the result of the bug tagged below: the eight
             * shifts push out whatever bits held, so a start at 0 stores the
             * byte the original stores */
            bits = 0;
#endif
            /* @bug uninitialized: bits is shifted before anything sets it.
             * SHC keeps bits in R6, where map_packets arrives, so bits starts
             * as map_packets. MOV R6,R1 at 0x8c137460 copies map_packets out,
             * and nothing writes R6 before SHLL R6 at 0x8c137486. the byte
             * stored at 0x8c13749a is right all the same. after eight shifts
             * it holds only the bits the loop shifted in */
            for (bit = 0; bit < 8; bit++) {
                bits <<= 1; /* NOLINT(clang-diagnostic-uninitialized) */
                if (used > 0) {
                    used--;
                } else {
                    bits |= 1;
                }
            }
            *map++ = bits;
        } else {
            *map++ = -1;
        }
    }
    /* @hack redundant: a result, FMD_OK, that no caller reads, where a void
     * function would do */
    return FMD_OK;
}

/* where packet's data is all 0xff and it passes its CRC, erase all of it and
 * return TRUE. the new log needn't hold it: a read of an erased slot gives
 * the same data and FMD_OK */
/* @hack redundant: work, which the function never reads */
static Bool flash_erase_if_blank(FM_WRK *work, FM_PKTN *packet)
{
    Uint8 *at = packet->data;
    Sint32 count;
    /* @hack spelling: crc, where (Uint16)fmcalccrc(...) in the comparison
     * would do */
    Uint16 crc;

    for (count = FLASH_DATA_BYTES; count > 0; count--) {
        if (*at++ != 0xff) {
            return FALSE;
        }
    }
    crc = fmcalccrc(PACKET_CHECKED, (Uint8 *)packet);
    if (packet->chk != crc) {
        return FALSE;
    }
    at = (Uint8 *)packet;
    for (count = FLASH_PACKET_BYTES; count > 0; count--) {
        *at++ = 0xff;
    }
    return TRUE;
}

/* read the bank into the cache, which the caller has erased. each slot then
 * holds the latest packet at its offset that passes its CRC, or stays
 * erased. it returns FMD_OK, or FLASH_READ_FAILED when a read fails. for a
 * bank not FLASH_TYPE_LOG, it returns FLASH_BAD_TYPE with only the header and
 * the map read */
static Sint32 flash_load_cache(FM_WRK *work)
{
    Sint32 address = work->blk_adr;
    Uint8 *cache = work->blk_buf;
    /* @hack spelling: result, where each call in its test would do */
    Sint32 result;
    Sint32 map_offset;
    Sint32 remaining;
    Sint32 read_address;
    /* @hack spelling: crc, where (Uint16)fmcalccrc(...) in the comparison
     * would do */
    Uint16 crc;
    FM_PKTN *packet;

    result = syBtFlRead(address, cache, FLASH_PACKET_BYTES);
    if (result == BOOT_FLASH_FAILURE) {
        return FLASH_READ_FAILED;
    }
    map_offset = (work->blk_pat_adr + 1) * FLASH_PACKET_BYTES;
    result = syBtFlRead(address + map_offset, cache + map_offset,
                        work->blk_pat_size * FLASH_PACKET_BYTES);
    if (result == BOOT_FLASH_FAILURE) {
        return FLASH_READ_FAILED;
    }
    /* @hack spelling: work->blk_fre_adr and remaining set in one statement,
     * where remaining = flash_count_used(...) and then work->blk_fre_adr =
     * remaining would do */
    work->blk_fre_adr = remaining =
        flash_count_used(cache + map_offset, work->blk_pat_size);
    /* @hack spelling: read_address set before the type test, not by the for
     * that steps it */
    read_address = address + FLASH_PACKET_BYTES;
    /* @hack spelling: the FLASH_BAD_TYPE return last, where an early return
     * would do */
    if (work->fmtype == FLASH_TYPE_LOG) {
        packet = &work->pkt_buf.packet;
        for (; remaining > 0; remaining--, read_address += FLASH_PACKET_BYTES) {
            result = syBtFlRead(read_address, packet, FLASH_PACKET_BYTES);
            if (result == BOOT_FLASH_FAILURE) {
                return FLASH_READ_FAILED;
            }
            if (packet->offset == FLASH_OFFSET_ERASED) {
                break;
            }
            crc = fmcalccrc(PACKET_CHECKED, (Uint8 *)packet);
            if (packet->chk == crc && packet->offset < work->blk_pat_adr) {
                flash_copy_words(
                    packet, cache + ((packet->offset + 1) * FLASH_PACKET_BYTES),
                    FLASH_PACKET_BYTES);
            }
        }
        return FMD_OK;
    }
    return FLASH_BAD_TYPE;
}

/* write the cache to the bank, which the caller has erased. the bank's new
 * log holds each slot's packet, except erased slots and those
 * flash_erase_if_blank erases. it returns FMD_OK, or FLASH_WRITE_FAILED
 * when a write fails. for a bank not FLASH_TYPE_LOG, it returns
 * FLASH_BAD_TYPE and writes nothing */
static Sint32 flash_store_cache(FM_WRK *work)
{
    Sint32 address = work->blk_adr;
    Sint32 write_address;
    /* @hack spelling: map_offset, where its expression in the map's write
     * would do */
    Sint32 map_offset;
    /* @hack spelling: result, where each call in its test would do */
    Sint32 result;
    Sint32 used;
    Sint32 remaining;
    FM_PKTN *packet;

    /* @hack spelling: the FLASH_BAD_TYPE return last, where an early return
     * would do */
    if (work->fmtype == FLASH_TYPE_LOG) {
        packet = work->blk_buf;
        /* @hack spelling: write_address set in two steps, where
         * write_address = address + FLASH_PACKET_BYTES after the header's
         * write would do */
        write_address = address;
        /* @hack spelling: packet++ in the header write's argument, where
         * packet++ after the write's test would do */
        result = syBtFlWrite(address, packet++, FLASH_PACKET_BYTES);
        if (result != FLASH_PACKET_BYTES) {
            return FLASH_WRITE_FAILED;
        }
        write_address += FLASH_PACKET_BYTES;
        used = 0;
        for (remaining = work->blk_pat_adr; remaining > 0;
             remaining--, packet++) {
            /* @hack spelling: two tests, not one joined by && */
            if (packet->offset != FLASH_OFFSET_ERASED) {
                if (!flash_erase_if_blank(work, packet)) {
                    result =
                        syBtFlWrite(write_address, packet, FLASH_PACKET_BYTES);
                    if (result != FLASH_PACKET_BYTES) {
                        return FLASH_WRITE_FAILED;
                    }
                    write_address += FLASH_PACKET_BYTES;
                    used++;
                }
            }
        }
        map_offset = (work->blk_pat_adr + 1) * FLASH_PACKET_BYTES;
        /* the loop leaves packet past the last slot, at the map */
        flash_make_map(used, (Sint8 *)packet, work->blk_pat_size);
        result = syBtFlWrite(address + map_offset, packet,
                             work->blk_pat_size * FLASH_PACKET_BYTES);
        if (result != work->blk_pat_size * FLASH_PACKET_BYTES) {
            return FLASH_WRITE_FAILED;
        }
        work->blk_fre_adr = used;
        return FMD_OK;
    }
    return FLASH_BAD_TYPE;
}

/* store fmMngWrite's packet in its slot and at the log's end. it returns
 * FMD_OK, or FLASH_WRITE_FAILED when a write fails. a failed write of the
 * packet leaves the map and work->blk_fre_adr counting the packet. it returns
 * LOG_FULL for a full log, with the packet in its slot but not the log. it
 * returns FLASH_BAD_TYPE for a bank not FLASH_TYPE_LOG */
static Sint32 flash_append_packet(FM_WRK *work)
{
    FM_PKTN *packet;
    /* @hack spelling: result, where each call in its test would do */
    Sint32 result;
    /* @hack spelling: byte_offset, where each offset computed in its call
     * would do. the packet's write would take work->blk_fre_adr *
     * FLASH_PACKET_BYTES, after work->blk_fre_adr++ */
    Sint32 byte_offset;
    Sint32 map_offset;
    /* @hack spelling: crc, where packet->chk = fmcalccrc(...) would do */
    Uint16 crc;

    /* @hack spelling: the FLASH_BAD_TYPE return last, where an early return
     * would do */
    if (work->fmtype == FLASH_TYPE_LOG) {
        packet = &work->pkt_buf.packet;
        packet->offset = work->exec_ofs;
        flash_copy_bytes(work->exec_buf, packet->data, FLASH_DATA_BYTES);
        crc = fmcalccrc(PACKET_CHECKED, (Uint8 *)packet);
        packet->chk = crc;
        byte_offset = (work->exec_ofs + 1) * FLASH_PACKET_BYTES;
        flash_copy_words(packet, (Uint8 *)work->blk_buf + byte_offset,
                         FLASH_PACKET_BYTES);
        if (work->blk_fre_adr >= work->blk_pat_adr) {
            return LOG_FULL;
        }
        map_offset = (work->blk_pat_adr + 1) * FLASH_PACKET_BYTES;
        flash_make_map(work->blk_fre_adr + 1,
                       (Sint8 *)work->blk_buf + map_offset, work->blk_pat_size);
        result = syBtFlWrite(work->blk_adr + map_offset,
                             (Uint8 *)work->blk_buf + map_offset,
                             work->blk_pat_size * FLASH_PACKET_BYTES);
        if (result != work->blk_pat_size * FLASH_PACKET_BYTES) {
            return FLASH_WRITE_FAILED;
        }
        byte_offset = (work->blk_fre_adr + 1) * FLASH_PACKET_BYTES;
        work->blk_fre_adr++;
        result = syBtFlWrite(work->blk_adr + byte_offset, packet,
                             FLASH_PACKET_BYTES);
        if (result != FLASH_PACKET_BYTES) {
            return FLASH_WRITE_FAILED;
        }
        return FMD_OK;
    }
    return FLASH_BAD_TYPE;
}

/* copy the data in the slot for fmMngRead's offset to fmMngRead's buffer.
 * it returns FMD_OK, or FLASH_BAD_CRC when a written slot fails its CRC. it
 * returns FLASH_BAD_TYPE for a bank not FLASH_TYPE_LOG */
static Sint32 flash_read_packet(FM_WRK *work)
{
    FM_PKTN *slot;
    /* @hack spelling: crc, where (Uint16)fmcalccrc(...) in the comparison
     * would do */
    Uint16 crc;

    /* @hack spelling: the FLASH_BAD_TYPE return last, where an early return
     * would do */
    if (work->fmtype == FLASH_TYPE_LOG) {
        slot = (FM_PKTN *)work->blk_buf + (work->exec_ofs + 1);
        flash_copy_bytes(slot->data, work->exec_buf, FLASH_DATA_BYTES);
        if (slot->offset != FLASH_OFFSET_ERASED) {
            crc = fmcalccrc(PACKET_CHECKED, (Uint8 *)slot);
            if (slot->chk != crc) {
                return FLASH_BAD_CRC;
            }
        }
        return FMD_OK;
    }
    return FLASH_BAD_TYPE;
}

/* run a write's next phase. PHASE_APPEND appends the packet. when the log is
 * full, PHASE_ERASE erases the bank on the next call. PHASE_STORE writes the
 * cache back on the call after. it returns PHASE_REMAINS while a phase
 * remains, and then the write's result */
static Sint32 flash_write_step(FM_WRK *work)
{
    Sint32 result;
    /* @hack spelling: appended, where result would do */
    Sint32 appended;

    /* @hack spelling: one return, of result, after the switch, where a
     * return in each arm would do */
    switch (work->exec_flag) {
    case PHASE_APPEND:
        appended = flash_append_packet(work);
        if (appended == LOG_FULL) {
            work->exec_flag = PHASE_ERASE;
            result = PHASE_REMAINS;
        } else {
            result = appended;
        }
        break;
    case PHASE_ERASE:
        /* @hack spelling: the erase's result through result, where the call
         * in the test would do */
        result = syBtFlDeleteSct(work->blk_adr);
        if (result == BOOT_FLASH_FAILURE) {
            result = FLASH_ERASE_FAILED;
        } else {
            work->exec_flag = PHASE_STORE;
            result = PHASE_REMAINS;
        }
        break;
    case PHASE_STORE:
        result = flash_store_cache(work);
        break;
    /* @hack redundant: an arm for a phase no code sets */
    default:
        result = FLASH_WRITE_FAILED;
        break;
    }
    return result;
}

Sint32 fmMngInit(Sint32 bank, void *workspace)
{
    SY_BT_FL_BLKINFO info;
    /* @hack spelling: result, where syBtFlRead and flash_bytes_equal in
     * their tests would do */
    Sint32 result;
    FM_BLKHDR *header;

    if (bank < 1 || bank >= BOOT_FLASH_BLOCK_END) {
        return FLASH_BAD_BANK;
    }
    flash_active = &flash_storage;
    syBtFlGetInfo(bank, &info);
    flash_active->fmblock = bank;
    flash_active->blk_adr = info.fmaddr;
    flash_active->blk_size = info.size;
    flash_active->blk_pat_size =
        (flash_active->blk_size + (FLASH_BYTES_PER_MAP_PACKET - 1)) >>
        MAP_SHIFT;
    flash_active->blk_pat_adr = (flash_active->blk_size >> PACKET_SHIFT) -
                                flash_active->blk_pat_size - 1;
    flash_active->blk_buf = workspace;
    flash_active->exec_mode = REQUEST_NONE;
    flash_active->exec_stat = FLASH_STATE_NONE;
    flash_active->exec_err = FMD_OK;
    result = syBtFlRead(flash_active->blk_adr, &flash_active->pkt_buf,
                        FLASH_PACKET_BYTES);
    if (result == BOOT_FLASH_FAILURE) {
        return FLASH_READ_FAILED;
    }
    header = &flash_active->pkt_buf.header;
    result =
        flash_bytes_equal(header->format_id, FLASH_SIGNATURE,
                          FLASH_SIGNATURE_BYTES - FLASH_SIGNATURE_UNCHECKED);
    if (!result) {
        return FLASH_BAD_HEADER;
    }
    if (flash_active->fmblock != header->blockno) {
        return FLASH_BAD_HEADER;
    }
    flash_active->fmtype = header->type;
    if (flash_active->fmtype != FLASH_TYPE_LOG &&
        flash_active->fmtype != FLASH_TYPE_SEALED) {
        return FLASH_BAD_HEADER;
    }
    flash_erase_cache(flash_active->blk_buf, flash_active->blk_size);
    /* @hack redundant: the test of result, though the function returns
     * result either way. return flash_load_cache(flash_active) would do */
    result = flash_load_cache(flash_active);
    if (result != FMD_OK) {
        return result;
    }
    return FMD_OK;
}

Sint32 fmMngFormat(Sint32 format)
{
    FM_BLKHDR *header;
    /* @hack spelling: result, where syBtFlDeleteSct and syBtFlWrite in
     * their tests would do */
    Sint32 result;

    if (flash_active == NULL) {
        return FLASH_NO_BANK;
    }
    if (format != FLASH_TYPE_LOG && format != FLASH_TYPE_SEALED) {
        return FLASH_BAD_TYPE;
    }
    result = syBtFlDeleteSct(flash_active->blk_adr);
    if (result != BOOT_FLASH_SUCCESS) {
        return FLASH_ERASE_FAILED;
    }
    header = &flash_active->pkt_buf.header;
    flash_copy_bytes((const Uint8 *)FLASH_SIGNATURE, (Uint8 *)header->format_id,
                     FLASH_SIGNATURE_BYTES);
    flash_active->fmtype = format;
    header->type = format;
    header->blockno = flash_active->fmblock;
    result = syBtFlWrite(flash_active->blk_adr, header, FLASH_HEADER_BYTES);
    if (result != FLASH_HEADER_BYTES) {
        return FLASH_WRITE_FAILED;
    }
    flash_erase_cache(flash_active->blk_buf, flash_active->blk_size);
    /* @hack redundant: the test of result, though the function returns
     * result either way. return flash_load_cache(flash_active) would do */
    result = flash_load_cache(flash_active);
    if (result != FMD_OK) {
        return result;
    }
    return FMD_OK;
}

Sint32 fmMngRead(Sint32 offset, void *buf)
{
    if (flash_active == NULL) {
        return FLASH_NO_BANK;
    }
    if (flash_active->exec_mode != REQUEST_NONE) {
        return FLASH_ALREADY_PENDING;
    }
    if (offset < 0 || offset >= flash_active->blk_pat_adr) {
        return FLASH_BAD_OFFSET;
    }
    flash_active->exec_mode = REQUEST_READ;
    flash_active->exec_stat = FLASH_STATE_PENDING;
    flash_active->exec_ofs = offset;
    flash_active->exec_buf = buf;
    return FMD_OK;
}

Sint32 fmMngWrite(Sint32 offset, void *buf)
{
    if (flash_active == NULL) {
        return FLASH_NO_BANK;
    }
    if (flash_active->exec_mode != REQUEST_NONE) {
        return FLASH_ALREADY_PENDING;
    }
    if (offset < 0 || offset >= flash_active->blk_pat_adr) {
        return FLASH_BAD_OFFSET;
    }
    flash_active->exec_mode = REQUEST_WRITE;
    flash_active->exec_stat = FLASH_STATE_PENDING;
    flash_active->exec_ofs = offset;
    flash_active->exec_buf = buf;
    flash_active->exec_flag = PHASE_APPEND;
    return FMD_OK;
}

Sint32 fmMngExecServer(void)
{
    if (flash_active == NULL) {
        return FLASH_STATE_NONE;
    }
    switch (flash_active->exec_mode) {
    case REQUEST_NONE:
        break;
    case REQUEST_READ: {
        Sint32 result = flash_read_packet(flash_active);

        flash_active->exec_mode = REQUEST_NONE;
        if (result != FMD_OK) {
            flash_active->exec_stat = FMD_STAT_ERR;
        } else {
            flash_active->exec_stat = FMD_STAT_COMPLETE;
        }
        flash_active->exec_err = result;
        break;
    }
    case REQUEST_WRITE: {
        Sint32 result = flash_write_step(flash_active);

        if (result == PHASE_REMAINS) {
            break;
        }
        flash_active->exec_mode = REQUEST_NONE;
        if (result != FMD_OK) {
            flash_active->exec_stat = FMD_STAT_ERR;
        } else {
            flash_active->exec_stat = FMD_STAT_COMPLETE;
        }
        flash_active->exec_err = result;
        break;
    }
    /* @hack redundant: an arm for an exec_mode no code sets */
    default:
        flash_active->exec_mode = REQUEST_NONE;
        flash_active->exec_stat = FLASH_STATE_NONE;
        break;
    }
    return flash_active->exec_stat;
}

Sint32 fmMngGetErr(void)
{
    if (flash_active == NULL) {
        return FLASH_NO_BANK;
    }
    return flash_active->exec_err;
}

void fmMngFinish(void)
{
    flash_active = NULL;
}

Sint32 fmMngRefresh(void)
{
    /* @hack spelling: result, where the erase in its test and a return on
     * each path would do */
    Sint32 result;

    if (flash_active == NULL) {
        return FLASH_NO_BANK;
    }
    result = syBtFlDeleteSct(flash_active->blk_adr);
    if (result == BOOT_FLASH_FAILURE) {
        result = FLASH_ERASE_FAILED;
    } else {
        result = flash_store_cache(flash_active);
    }
    return result;
}

Sint32 fmMngGetFree(void)
{
    if (flash_active == NULL) {
        return FLASH_NO_BANK;
    }
    return flash_active->blk_pat_adr - flash_active->blk_fre_adr;
}
