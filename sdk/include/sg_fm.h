/* Shinobi flash memory manager */
#ifndef SG_FM_H
#define SG_FM_H
#include <sg_xpt.h>

/* a bank is a block of the flash, numbered as syBtFlGetInfo's part. the
 * manager keeps one bank open, and caches it whole in the caller's
 * workspace.
 *
 * the caller reads and writes data by an offset, FLASH_DATA_BYTES bytes at
 * each. a write appends a packet to the bank's log. a read gives the data of
 * the latest packet at its offset, or 0xff bytes where none is.
 *
 * a write that finds the log full erases the bank. it then writes the
 * latest packet at each offset back, as a new log */
#define FLASH_DATA_BYTES 60

/* the types a bank's header can give. only a FLASH_TYPE_LOG bank's packets
 * are read and written */
#define FLASH_TYPE_LOG 0
#define FLASH_TYPE_SEALED 1

/* a call's result: FMD_OK, or what failed */
#define FMD_OK 0
/* a call the manager can't take */
#define FLASH_NO_BANK (-11) /* no bank is open */
#define FLASH_BAD_BANK (-7) /* a bank other than 1 to 4 */
#define FLASH_BAD_OFFSET (-5) /* an offset outside the bank's */
#define FLASH_BAD_TYPE (-6) /* no such type, or a bank not FLASH_TYPE_LOG */
#define FLASH_ALREADY_PENDING (-9) /* a request is in progress */
/* what the bank holds */
#define FLASH_BAD_HEADER (-8) /* its signature, bank or type is wrong */
#define FLASH_BAD_CRC (-4) /* the packet read fails its CRC */
/* a flash call that failed */
#define FLASH_READ_FAILED (-1)
#define FLASH_WRITE_FAILED (-2)
#define FLASH_ERASE_FAILED (-3)

/* fmMngExecServer's states. it returns FLASH_STATE_PENDING while a request is
 * in progress. once the request finishes, it returns FMD_STAT_COMPLETE or
 * FMD_STAT_ERR until the next request starts. it returns
 * FLASH_STATE_NONE before any request since fmMngInit, and with no bank
 * open */
#define FLASH_STATE_NONE 0
#define FMD_STAT_COMPLETE 1
#define FLASH_STATE_PENDING 2
#define FMD_STAT_ERR 3

/* a bank of size bytes holds size / FLASH_PACKET_BYTES packets. the first is
 * the bank's header. the map takes the last ones. the packets between are
 * the log, and the map marks how many of them are written. an offset runs
 * from 0 to one less than the log's packets */
#define FLASH_PACKET_BYTES 64

/* a formatted bank's header opens with FLASH_SIGNATURE, which is
 * FLASH_SIGNATURE_BYTES characters with no terminator. fmMngInit compares all
 * of them but the last FLASH_SIGNATURE_UNCHECKED. the bank's number and its
 * type follow, and FLASH_HEADER_BYTES counts all three, the bytes
 * fmMngFormat writes */
#define FLASH_SIGNATURE "KATANA_FLASH____"
#define FLASH_SIGNATURE_BYTES 16
#define FLASH_SIGNATURE_UNCHECKED 4
#define FLASH_HEADER_BYTES 18

/* a bank's first packet */
typedef struct FMS_BLK_HDR {
    char format_id[FLASH_SIGNATURE_BYTES];
    Sint8 blockno; /* the bank's number */
    Sint8 type; /* a FLASH_TYPE_ value */
    Sint8 reserved[FLASH_PACKET_BYTES - FLASH_HEADER_BYTES]; /* left erased */
} FM_BLKHDR;

/* the bytes of a packet's CRC, which end it */
#define FLASH_CRC_BYTES 2
/* the offset an erased packet reads. such a packet ends the log on flash,
 * and stands for an offset never written in the cache */
#define FLASH_OFFSET_ERASED 0xffff

/* a packet of the log: its offset, the caller's data and the CRC of both */
typedef struct FMS_PKT_NRM {
    Uint16 offset;
    Uint8 data[FLASH_DATA_BYTES];
    Uint16 chk;
} FM_PKTN;

/* the bytes of the bank that one packet of the map has bits for: a packet
 * holds FLASH_PACKET_BYTES * 8 bits, and each stands for a packet. the map
 * takes a packet for every such run of the bank's bytes, rounded up */
#define FLASH_BYTES_PER_MAP_PACKET 32768

/* the manager's record of the open bank. on flash, packet 0 is the header
 * and packets 1 to blk_pat_adr are the log. the map follows, from packet
 * blk_pat_adr + 1 */
typedef struct FMS_WORKAREA {
    Sint16 fmblock; /* the open bank's number, 1 to 4 */
    /* the type the bank's header gives. fmMngInit stores it before it checks
     * it, and fmMngFormat stores the type it writes */
    Sint16 fmtype;
    Sint32 blk_adr; /* the bank's flash address */
    Sint32 blk_size; /* the bank's bytes */
    /* the caller's workspace, which caches the bank. it holds the header
     * and the map where the flash does. in place of the log it holds a slot
     * for each offset. packet n + 1 is the slot for offset n, and holds the
     * latest packet at that offset */
    void *blk_buf;
    Sint32 blk_fre_adr; /* the packets the log holds */
    /* the most packets the log can hold, which is also how many offsets
     * there are */
    Sint32 blk_pat_adr;
    /* the packets the map takes. the map has a bit for each place in the
     * log, in order. each byte's most significant bit comes first. a bit is
     * clear for each packet the log holds, and set past them */
    Sint32 blk_pat_size;
    Sint32 exec_mode; /* the request to run: none, a read or a write */
    Sint32 exec_stat; /* fmMngExecServer's result, one of its states above */
    Sint32 exec_flag; /* the write's next phase */
    Sint32 exec_ofs; /* the offset fmMngRead or fmMngWrite was given */
    Sint32 exec_err; /* the result of the last request to finish */
    void *exec_buf; /* the buf fmMngRead or fmMngWrite was given */
    /* a packet as it is read from flash, or built to be written to it. the
     * bank's header goes through it too */
    union {
        FM_PKTN packet;
        FM_BLKHDR header;
    } pkt_buf;
} FM_WRK;

/* open bank, 1 to 4, and cache it in workspace. any pending request is
 * dropped. workspace must be aligned to 4 and as large as the bank. it must
 * stay valid until fmMngFinish, or until a later fmMngInit opens a bank.
 *
 * it returns FMD_OK once the bank is cached. for a bank outside 1 to 4 it
 * returns FLASH_BAD_BANK, and changes nothing.
 *
 * every other failure leaves the bank open, so that fmMngFormat can format
 * it. the call returns FLASH_BAD_TYPE for a FLASH_TYPE_SEALED bank. it
 * returns FLASH_BAD_HEADER for a header with the wrong signature, bank or
 * type. it returns FLASH_READ_FAILED when a flash read fails. after
 * FLASH_BAD_HEADER or FLASH_READ_FAILED the workspace isn't cached, and only
 * fmMngFormat or fmMngFinish may follow */
Sint32 fmMngInit(Sint32 bank, void *workspace);
/* erase the open bank, write a header whose type is format, and cache the
 * bank again as fmMngInit does. a pending request stays pending. it returns
 * FMD_OK once the bank is cached. it returns FLASH_BAD_TYPE for no such
 * type, before erasing, and for FLASH_TYPE_SEALED, after. it returns
 * FLASH_NO_BANK with no bank open, and FLASH_ERASE_FAILED,
 * FLASH_WRITE_FAILED or FLASH_READ_FAILED when a flash call fails */
Sint32 fmMngFormat(Sint32 format);
/* close the bank and drop its request, after which workspace need not stay
 * valid. a write dropped after its erase leaves the bank erased */
void fmMngFinish(void);
/* request a copy of the FLASH_DATA_BYTES bytes of data at offset: fmMngRead
 * into buf, and fmMngWrite from buf. fmMngExecServer runs the request, and
 * copies the bytes on its first call for it. buf must stay valid until
 * then.
 *
 * each returns FMD_OK when it takes the request, and FLASH_NO_BANK with no
 * bank open. each returns FLASH_ALREADY_PENDING while a request is in
 * progress, and FLASH_BAD_OFFSET for an offset outside the bank's */
Sint32 fmMngRead(Sint32 offset, void *buf);
Sint32 fmMngWrite(Sint32 offset, void *buf);
/* run a step of the pending request, and return the state. a read takes one
 * call. a write takes one, or three when the log is full. the second call
 * then erases the bank, and the third writes the cache back as a new log. a
 * write whose erase fails ends at the second.
 *
 * once the request finishes, fmMngGetErr gives its result. a read gives
 * FMD_OK, or FLASH_BAD_CRC for a packet that fails its CRC. FLASH_BAD_CRC
 * still leaves the bytes copied. a write gives FMD_OK, FLASH_WRITE_FAILED
 * or FLASH_ERASE_FAILED. either gives FLASH_BAD_TYPE for a bank not
 * FLASH_TYPE_LOG */
Sint32 fmMngExecServer(void);
/* the result of the last request to finish since fmMngInit, or FMD_OK
 * before any. it returns FLASH_NO_BANK with no bank open */
Sint32 fmMngGetErr(void);
/* erase the open bank and write back the latest packet at each offset as a
 * new log. a pending request stays pending. it returns FMD_OK once the log
 * is written. it returns FLASH_BAD_TYPE for a bank not FLASH_TYPE_LOG, and
 * leaves the bank erased. it returns FLASH_NO_BANK with no bank open, and
 * FLASH_ERASE_FAILED or FLASH_WRITE_FAILED when a flash call fails */
Sint32 fmMngRefresh(void);
/* the packets the log can take before a write erases the bank, or
 * FLASH_NO_BANK with no bank open */
Sint32 fmMngGetFree(void);

/* the CRC-16 of the size bytes at data, the kind a packet's chk holds. its
 * polynomial is 0x1021, taken most significant bit first. it starts at
 * 0xffff. the result is the CRC with each of its 16 bits inverted, and 0
 * above them. a size of 0 or less returns 0 */
Uint32 fmcalccrc(Sint32 size, const Uint8 *data);

#endif
