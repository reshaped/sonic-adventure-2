/* backup file images. an image is its header, its icons, any visual and any
 * save data, in that order. zeros pad it to whole blocks */
#include <sg_bup.h>
#include <sg_xpt.h>
#include <string.h>

#include "vms.h"

#define ICONS_MAX 3
#define VISUAL_TYPE_MAX BUD_VISUALTYPE_C
#define ICON_SIZE 512
/* the bytes of a visual of each type */
#define VISUAL_A_SIZE 8064
#define VISUAL_B_SIZE 4544
#define VISUAL_C_SIZE 2048

/* the header's bytes before its palette */
#define PALETTE_OFFSET 96

/* buAnalyzeBackupFileImage refuses an image larger than this before its
 * padding */
#define IMAGE_SIZE_MAX 0xe1000

#define CRC_POLYNOMIAL 0x1021
#define CRC_TOP_BIT 0x8000

#define VMS_COMMENT_SIZE 16
#define BTR_COMMENT_SIZE 32
#define GAME_NAME_SIZE 16

struct image_header {
    char vms_comment[VMS_COMMENT_SIZE];
    char btr_comment[BTR_COMMENT_SIZE];
    char game_name[GAME_NAME_SIZE];
    Uint16 icon_num;
    Uint16 icon_speed;
    Uint16 visual_type;
    /* buCalcCRC of the image before its padding, taken with crc 0.
     * buAnalyzeBackupFileImage doesn't check an image whose crc is 0 */
    Uint16 crc;
    Uint32 save_size;
    Uint8 unk_4c[20];
    Uint16 palette[16];
};

/* no code writes these tables. they lack const so that SHC places them in
 * the data section, where the original program has them */
static Uint32 icon_sizes[ICONS_MAX + 1] = {0, ICON_SIZE, 2 * ICON_SIZE,
                                           3 * ICON_SIZE};
static Uint32 visual_sizes[VISUAL_TYPE_MAX + 1] = {
    0, VISUAL_A_SIZE, VISUAL_B_SIZE, VISUAL_C_SIZE};

Sint32 buMakeBackupFileImage(void *buffer, const BUS_BACKUPFILEHEADER *header)
{
    struct image_header *image;
    Sint32 i;
    char *to;
    const char *from;
    Uint32 icon_bytes;
    Uint32 visual_bytes;
    Uint32 size;
    Uint32 icon_offset;
    Uint32 palette_offset;
    Uint32 visual_offset;
    Uint32 save_offset;

    if (header->icon_num == 0) {
        return BUD_ERR_INVALID_PARAM;
    }
    if (header->icon_num > ICONS_MAX) {
        return BUD_ERR_INVALID_PARAM;
    }
    if (header->visual_type > VISUAL_TYPE_MAX) {
        return BUD_ERR_INVALID_PARAM;
    }
    if (header->icon_palette == NULL) {
        return BUD_ERR_INVALID_PARAM;
    }
    if (header->icon_data == NULL) {
        return BUD_ERR_INVALID_PARAM;
    }
    if (header->visual_type != BUD_VISUALTYPE_NONE &&
        header->visual_data == NULL) {
        return BUD_ERR_INVALID_PARAM;
    }
    image = buffer;
    memset(image, 0, PALETTE_OFFSET);
    memset(image->vms_comment, ' ', sizeof(image->vms_comment));
    memset(image->btr_comment, ' ', sizeof(image->btr_comment));
    /* @hack redundant: the clear of game_name, which the first memset has
     * already zeroed */
    memset(image->game_name, 0, sizeof(image->game_name));
    to = image->vms_comment;
    from = header->vms_comment;
    for (i = 0; i < VMS_COMMENT_SIZE; i++) {
        if (!*from) {
            break;
        }
        *to++ = *from++;
    }
    to = image->btr_comment;
    from = header->btr_comment;
    for (i = 0; i < BTR_COMMENT_SIZE; i++) {
        if (!*from) {
            break;
        }
        *to++ = *from++;
    }
    to = image->game_name;
    from = (const char *)header->game_name;
    for (i = 0; i < GAME_NAME_SIZE; i++) {
        if (!*from) {
            break;
        }
        *to++ = *from++;
    }
    /* @hack redundant: the masks of icon_num and visual_type, which the
     * opening checks already keep within ICONS_MAX and VISUAL_TYPE_MAX */
    icon_bytes = icon_sizes[header->icon_num & ICONS_MAX];
    visual_bytes = visual_sizes[header->visual_type & VISUAL_TYPE_MAX];
    size = icon_bytes + visual_bytes + header->save_size +
           sizeof(struct image_header);
    image->icon_speed = header->icon_speed;
    image->icon_num = header->icon_num;
    /* @hack spelling: the locals palette_offset and icon_offset, where
     * PALETTE_OFFSET and sizeof(struct image_header) at their uses would do */
    palette_offset = PALETTE_OFFSET;
    icon_offset = sizeof(struct image_header);
    /* @hack redundant: the test of visual_data, which the opening checks
     * have already made */
    if (header->visual_type == BUD_VISUALTYPE_NONE ||
        header->visual_data == NULL) {
        /* @hack redundant: the store to visual_type, which the first memset
         * has already zeroed */
        image->visual_type = BUD_VISUALTYPE_NONE;
        visual_offset = 0;
    } else {
        image->visual_type = header->visual_type;
        visual_offset = icon_bytes + sizeof(struct image_header);
    }
    if (header->save_size == 0 || header->save_data == NULL) {
        /* @hack redundant: the store to save_size, which the first memset
         * has already zeroed */
        image->save_size = 0;
        save_offset = 0;
    } else {
        image->save_size = header->save_size;
        save_offset = icon_bytes + visual_bytes + sizeof(struct image_header);
    }
    /* @hack redundant: the test of icon_bytes, which is never 0 for 1 to
     * ICONS_MAX icons */
    if (icon_bytes) {
        memcpy((char *)buffer + palette_offset, header->icon_palette,
               sizeof(image->palette));
        memcpy((char *)buffer + icon_offset, header->icon_data, icon_bytes);
    }
    if (visual_bytes) {
        memcpy((char *)buffer + visual_offset, header->visual_data,
               visual_bytes);
    }
    /* @bug memory: copies save_size bytes from address 0 over the image's
     * start where save_data is null and save_size isn't 0 (JSR at
     * 0x8c0c098c). for a null save_data, the branch above sets
     * image->save_size and save_offset to 0 (MOV.L at 0x8c0c0902 and
     * 0x8c0c0906). this test reads header->save_size instead (MOV.L at
     * 0x8c0c0972) */
    if (header->save_size) {
        memcpy((char *)buffer + save_offset, header->save_data,
               header->save_size);
    }
    if (size % VMS_BLOCK_SIZE) {
        memset((char *)buffer + size, 0,
               VMS_BLOCK_SIZE - (size % VMS_BLOCK_SIZE));
    }
    image->crc = buCalcCRC(buffer, size);
    return (size / VMS_BLOCK_SIZE) + ((size % VMS_BLOCK_SIZE) ? 1 : 0);
}

Sint32 buCalcBackupFileSize(Uint32 icon_num, Uint32 visual_type,
                            Uint32 save_size)
{
    Uint32 size;

    if (icon_num == 0) {
        return BUD_ERR_INVALID_PARAM;
    }
    if (icon_num > ICONS_MAX) {
        return BUD_ERR_INVALID_PARAM;
    }
    if (visual_type > VISUAL_TYPE_MAX) {
        return BUD_ERR_INVALID_PARAM;
    }
    size = icon_sizes[icon_num] + visual_sizes[visual_type] + save_size +
           sizeof(struct image_header);
    return (size / VMS_BLOCK_SIZE) + ((size % VMS_BLOCK_SIZE) ? 1 : 0);
}

Sint32 buAnalyzeBackupFileImage(BUS_BACKUPFILEHEADER *header, void *buffer)
{
    struct image_header *image;
    Sint32 i;
    char *to;
    char *from;
    Uint32 icon_bytes;
    Uint32 visual_bytes;
    Uint32 size;
    Uint16 crc;
    Uint16 saved_crc;

    image = buffer;
    if (image->icon_num == 0) {
        return BUD_ERR_BUPFILE_ILLEGAL;
    }
    if (image->icon_num > ICONS_MAX) {
        return BUD_ERR_BUPFILE_ILLEGAL;
    }
    if (image->visual_type > VISUAL_TYPE_MAX) {
        return BUD_ERR_BUPFILE_ILLEGAL;
    }
    /* this test keeps the sum below from wrapping */
    if (image->save_size > IMAGE_SIZE_MAX) {
        return BUD_ERR_BUPFILE_ILLEGAL;
    }
    icon_bytes = icon_sizes[image->icon_num];
    visual_bytes = visual_sizes[image->visual_type];
    size = icon_bytes + visual_bytes + image->save_size +
           sizeof(struct image_header);
    if (size > IMAGE_SIZE_MAX) {
        return BUD_ERR_BUPFILE_ILLEGAL;
    }
    to = header->vms_comment;
    from = image->vms_comment;
    for (i = 0; i < VMS_COMMENT_SIZE; i++) {
        *to++ = *from++;
    }
    *to = 0;
    to = header->btr_comment;
    from = image->btr_comment;
    for (i = 0; i < BTR_COMMENT_SIZE; i++) {
        *to++ = *from++;
    }
    *to = 0;
#ifdef NONMATCHING
    /* this side repairs the bug tagged below: it copies the name and writes no
     * null past it */
    memcpy(header->game_name, image->game_name, sizeof(header->game_name));
#else
    to = (char *)header->game_name;
    from = image->game_name;
    for (i = 0; i < GAME_NAME_SIZE; i++) {
        *to++ = *from++;
    }
    /* @bug memory: writes a null past the end of header->game_name, onto
     * icon_palette's first byte (MOV.B R2,@R3 at 0x8c0c0bfa). the store to
     * header->icon_palette below overwrites the null (MOV.L at 0x8c0c0c20) */
    *to = 0;
#endif
    header->icon_speed = image->icon_speed;
    header->icon_num = image->icon_num;
    /* @hack spelling: from set in two steps, to buffer and then past the
     * image's header, where one assignment would do */
    from = (char *)buffer;
    header->icon_palette = (char *)buffer + PALETTE_OFFSET;
    from += sizeof(struct image_header);
    header->icon_data = from;
    from += icon_bytes;
    header->visual_type = image->visual_type;
    if (image->visual_type == BUD_VISUALTYPE_NONE) {
        header->visual_data = NULL;
    } else {
        header->visual_data = from;
        from += visual_bytes;
    }
    header->save_size = image->save_size;
    if (image->save_size == 0) {
        header->save_data = NULL;
    } else {
        header->save_data = from;
    }
    saved_crc = image->crc;
    /* @hack redundant: the else after the return */
    if (saved_crc == 0) {
        return 0;
    } else {
        image->crc = 0;
        crc = buCalcCRC(buffer, size);
        image->crc = saved_crc;
        if (crc != saved_crc) {
            return BUD_ERR_BUPFILE_CRC;
        }
        return 0;
    }
}

#ifdef NONMATCHING
/* this side's callers pass the image's size, which buAnalyzeBackupFileImage
 * can't check. a header, icons, visual or save data past bytes fails with
 * BUD_ERR_BUPFILE_ILLEGAL before buAnalyzeBackupFileImage reads them */
Sint32 buAnalyzeBackupFileImageSized(BUS_BACKUPFILEHEADER *header, void *buffer,
                                     Uint32 bytes)
{
    struct image_header *image = buffer;
    BUS_BACKUPFILEHEADER parsed;
    Uint32 prefix;
    Sint32 result;

    if (!header || !buffer || bytes < sizeof(*image)) {
        return BUD_ERR_BUPFILE_ILLEGAL;
    }
    if (!image->icon_num || image->icon_num > ICONS_MAX ||
        image->visual_type > VISUAL_TYPE_MAX) {
        return BUD_ERR_BUPFILE_ILLEGAL;
    }
    prefix = sizeof(*image) + icon_sizes[image->icon_num] +
             visual_sizes[image->visual_type];
    if (prefix > bytes || image->save_size > bytes - prefix) {
        return BUD_ERR_BUPFILE_ILLEGAL;
    }
    memset(&parsed, 0, sizeof(parsed));
    result = buAnalyzeBackupFileImage(&parsed, buffer);
    if (result == 0) {
        *header = parsed;
    }
    return result;
}
#endif

Uint16 buCalcCRC(const void *buffer, Uint32 size)
{
    Uint32 i;
    Sint32 bit;
    Uint32 crc;
    /* @hack type: next as a char pointer, where a Uint8 pointer would do. a
     * char's sign extension changes only crc's bits above 15, which the
     * result drops */
    const char *next;

    next = buffer;
    crc = 0;
    for (i = 0; i < size; i++) {
        crc ^= (Uint32)*next++ << 8;
        for (bit = 0; bit < 8; bit++) {
            if (crc & CRC_TOP_BIT) {
                crc = (crc << 1) ^ CRC_POLYNOMIAL;
            } else {
                crc = crc << 1;
            }
        }
    }
    return crc;
}
