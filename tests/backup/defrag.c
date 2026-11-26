/* vmsfs_xdefrag on a card that holds a block no file starts at and no block
 * links to, below a file of two blocks. The card is 16 blocks: blocks 0 to 9
 * are the user area, 12 the directory, 13 the FAT and 15 the system block. */
#include <assert.h>
#include <string.h>

#include "../../sdk/backup/vmsphys.c"

#define BLOCKS 16
#define USER_BLOCKS 10
#define DIRECTORY 12
#define FAT 13
#define SYSTEM 15
/* the lone block, and the file's first and second blocks */
#define LONE 1
#define FIRST 4
#define SECOND 3

static Uint8 card[BLOCKS][VMS_BLOCK_SIZE];
static Uint8 system_cache[VMS_BLOCK_SIZE];
static Uint8 fat_cache[VMS_BLOCK_SIZE];
static Uint8 directory_cache[VMS_BLOCK_SIZE];
static Uint8 defrag_buffer[VMS_BLOCK_SIZE];

/* the card's words, little-endian */
static Uint16 read_word(void *buffer, Sint32 offset)
{
    Uint8 *at = (Uint8 *)buffer + offset;

    return at[0] | at[1] << 8;
}

static void write_word(void *buffer, Sint32 offset, Uint16 value)
{
    Uint8 *at = (Uint8 *)buffer + offset;

    at[0] = value;
    at[1] = value >> 8;
}

Sint32 vmsio_isready(Sint32 drive)
{
    return drive == 0;
}

Sint32 vmsio_ismount(Sint32 drive)
{
    return drive == 0;
}

Sint32 buIsFormat(Sint32 drive)
{
    (void)drive;
    return 0;
}

Sint32 vmsfs_read_retry(Sint32 drive, Sint32 block, void *buffer)
{
    assert(drive == 0 && block >= 0 && block < BLOCKS);
    memcpy(buffer, card[block], VMS_BLOCK_SIZE);
    return 0;
}

Sint32 vmsfs_write_retry(Sint32 drive, Sint32 block, const void *buffer)
{
    assert(drive == 0 && block >= 0 && block < BLOCKS);
    memcpy(card[block], buffer, VMS_BLOCK_SIZE);
    return 0;
}

Sint32 vmsfs_bank_info_retry(Sint32 drive, Sint32 *current, Sint32 *locked,
                             Sint32 *count)
{
    (void)drive;
    (void)current;
    (void)count;
    *locked = 0;
    return 0;
}

Sint32 vmsfs_bank_lock_retry(Sint32 drive, Sint32 locked)
{
    (void)drive;
    (void)locked;
    return 0;
}

static Uint16 fat_entry(Sint32 block)
{
    return read_word(fat_cache, block * FAT_ENTRY_SIZE);
}

static int filled_with(Sint32 block, Uint8 value)
{
    Sint32 i;

    for (i = 0; i < VMS_BLOCK_SIZE; i++) {
        if (card[block][i] != value) {
            return 0;
        }
    }
    return 1;
}

int main(void)
{
    static VMS_CONTEXT context = {0, 0, 0, read_word, write_word};
    static VMS_MEDIA media;
    VMS_DRIVE *record = &buVmsDrive[0];
    struct dir_entry *entry = (struct dir_entry *)directory_cache;
    Sint32 block;

    media.last_block = BLOCKS - 1;
    media.fat_block = FAT;
    media.fat_blocks = 1;
    media.directory_block = DIRECTORY;
    media.directory_blocks = 1;
    media.extra_block = USER_BLOCKS;
    media.extra_blocks = DIRECTORY - USER_BLOCKS;
    media.last_byte = VMS_BLOCK_SIZE - 1;
    media.user_blocks = USER_BLOCKS;
    media.exec_blocks = USER_BLOCKS;
    _vm = &context;
    record->enabled = 1;
    record->mounted = 1;
    record->media = &media;
    record->system_cache = system_cache;
    record->fat_cache = fat_cache;
    record->directory_cache = directory_cache;
    for (block = 0; block < BLOCKS; block++) {
        write_word(fat_cache, block * FAT_ENTRY_SIZE, FAT_FREE);
    }
    write_word(fat_cache, DIRECTORY * FAT_ENTRY_SIZE, FAT_LAST);
    write_word(fat_cache, FAT * FAT_ENTRY_SIZE, FAT_LAST);
    write_word(fat_cache, SYSTEM * FAT_ENTRY_SIZE, FAT_LAST);
    write_word(fat_cache, LONE * FAT_ENTRY_SIZE, FAT_LAST);
    write_word(fat_cache, FIRST * FAT_ENTRY_SIZE, SECOND);
    write_word(fat_cache, SECOND * FAT_ENTRY_SIZE, FAT_LAST);
    entry->kind = KIND_DATA;
    write_word(entry, DIR_FIRST_BLOCK, FIRST);
    write_word(entry, DIR_BLOCKS, 2);
    memset(card[LONE], LONE, VMS_BLOCK_SIZE);
    memset(card[FIRST], FIRST, VMS_BLOCK_SIZE);
    memset(card[SECOND], SECOND, VMS_BLOCK_SIZE);

    /* the lone block moves to the top of the user area, and the file below
     * it, second block first */
    assert(vmsfs_xdefrag(0, defrag_buffer) == 0);
    assert(filled_with(9, LONE) && fat_entry(9) == FAT_LAST);
    assert(filled_with(8, SECOND) && fat_entry(8) == FAT_LAST);
    assert(filled_with(7, FIRST) && fat_entry(7) == 8);
    assert(read_word(entry, DIR_FIRST_BLOCK) == 7);
    for (block = 0; block < 7; block++) {
        assert(fat_entry(block) == FAT_FREE);
    }
    /* the directory and the FAT reach the card */
    assert(memcmp(card[DIRECTORY], directory_cache, VMS_BLOCK_SIZE) == 0);
    assert(memcmp(card[FAT], fat_cache, VMS_BLOCK_SIZE) == 0);
    return 0;
}
