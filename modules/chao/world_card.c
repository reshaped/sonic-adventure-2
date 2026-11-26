/* the Chao world's file on the save drive, and Chao Adventure 2, the walk
 * game a VMU runs. the garden installs the walk game with a Chao, an egg and
 * items, and reads them back. the player picks the VMU in the walk menu, and
 * on the receive screen what to take */
#include <ninja.h>
#include <sg_bup.h>
#include <sg_xpt.h>
#include <shinobi.h>
#include <stdlib.h>
#include <string.h>

#include "chao/garden_save.h"
#include "chao/garden_state.h"
#include "chao/genes.h"
#include "chao/identity.h"
#include "chao/life.h"
#include "chao/residents.h"
#include "chao/transfer.h"
#include "chao/tree.h"
#include "chao/walk_assets.h"
#include "chao/walk_card.h"
#include "chao/walk_screen.h"
#include "chao/world.h"
#include "chao/world_card.h"
#include "chao/world_card_art.h"
#include "save/card.h"
#include "save/save.h"
#include "sound/sound.h"
#include "stage/stage.h"
#include "system/game_state.h"
#include "system/loader.h"
#include "system/pad.h"
#include "system/task.h"
#include "system/util.h"

#define CARD_BLOCK_BYTES 512
/* a backup file's header, and each frame of its icon */
#define CARD_HEADER_BYTES 128
#define ICON_FRAME_BYTES 512
#define WORLD_ICON_FRAMES 3
/* the world file's bytes before its save data */
#define WORLD_FILE_PREFIX                                                      \
    (CARD_HEADER_BYTES + (WORLD_ICON_FRAMES * ICON_FRAME_BYTES))
/* the block that holds the save data's first byte, and that byte's place in
 * the block */
#define WORLD_DATA_BLOCK (WORLD_FILE_PREFIX / CARD_BLOCK_BYTES)
#define WORLD_DATA_AT (WORLD_FILE_PREFIX % CARD_BLOCK_BYTES)
enum world_card_step {
    WORLD_CHECK_CARD = 1,
    WORLD_READ = 2,
    WORLD_COMPARE = 3,
    WORLD_SAVE = 4,
    WORLD_WRITING = 5,
    WORLD_DONE = 255
};
struct world_card_work {
    Uint8 step;
    struct garden_card_request *request;
    Uint8 *buffer; /* the file a read loads, or the block a write compares */
#ifdef NONMATCHING
    /* the size of buffer, by which this side bounds the file's checks */
    Uint32 buffer_size;
#endif
};
struct garden_card_request garden_card_request;

static void set_world_header(BUS_BACKUPFILEHEADER *header, void *data,
                             Uint32 size)
{
    memcpy(header->vms_comment, world_card_comment,
           sizeof(header->vms_comment));
    memcpy(header->btr_comment, world_card_title, sizeof(header->btr_comment));
    memcpy(header->game_name, card_game_name, sizeof(header->game_name));
    header->icon_palette = (void *)world_card_palette;
    header->icon_data = (void *)world_card_icon;
    header->icon_num = world_card_icon_count;
    header->icon_speed = world_card_icon_speed;
    header->visual_data = world_card_visual;
    header->visual_type = world_card_visual_type;
    header->reserved = 0;
    header->save_data = data;
    header->save_size = size;
}

static void destroy_world_card(struct task *task)
{
    struct world_card_work *work = task->extra.p;
    if (work) {
        if (work->buffer) {
            syFree(work->buffer);
        }
        syFree(work);
        task->extra.p = NULL;
    }
}
static void draw_world_card(struct task *task)
{
}

#ifdef NONMATCHING
/* this side bounds the analysis by the buffer's size */
#define ANALYZE_WORLD_FILE(header, work)                                       \
    buAnalyzeBackupFileImageSized((header), (work)->buffer, (work)->buffer_size)
#else
#define ANALYZE_WORLD_FILE(header, work)                                       \
    buAnalyzeBackupFileImage((header), (work)->buffer)
#endif

static void step_world_card(struct task *task)
{
    struct card_status status;
    BUS_BACKUPFILEHEADER header;
    struct world_card_work *work;
    struct garden_card_request *request;
    work = task->extra.p;
    request = work->request;
    if (!task->draw || !task->destructor) {
        task->draw = draw_world_card;
        task->destructor = destroy_world_card;
    }
    switch (work->step) {
    case WORLD_READ:
        if (card_get_status(request->drive, &status) != 0) {
            request->state = GARDEN_FILE_ERROR;
            work->step = WORLD_DONE;
        } else if (!status.busy) {
            if (status.error) {
                request->state = GARDEN_FILE_ERROR;
            } else {
                switch (ANALYZE_WORLD_FILE(&header, work)) {
                default:
                case BUD_ERR_BUPFILE_ILLEGAL:
                case BUD_ERR_BUPFILE_CRC:
                    request->state = GARDEN_FILE_INVALID;
                    break;
                case 0:
                    /* the copy takes request->size bytes, whatever
                     * header.save_size says. for a file this game wrote,
                     * they lie inside the buffer */
#ifdef NONMATCHING
                    /* this side stages the file by garden_file_stage, which
                     * checks the payload before it copies it. a file it refuses
                     * is marked GARDEN_FILE_INVALID */
                    if (!garden_file_stage(request, header.save_data,
                                           header.save_size)) {
                        request->state = GARDEN_FILE_INVALID;
                        break;
                    }
#else
                    memcpy(request->data, header.save_data, request->size);
#endif
                    request->state = GARDEN_FILE_OK;
                }
            }
            work->step = WORLD_DONE;
        }
        break;
    case WORLD_CHECK_CARD:
        if (buIsReady(request->drive)) {
            if (card_get_status(request->drive, &status) != 0) {
                request->state = GARDEN_FILE_ERROR;
                work->step = WORLD_DONE;
            } else if (!status.busy) {
                if (garden_file_loaded) {
                    if ((work->buffer = syMalloc(CARD_BLOCK_BYTES)) != NULL) {
                        if (card_load_file_ex(request->drive,
                                              world_card_filename, work->buffer,
                                              WORLD_DATA_BLOCK, 1) == 0) {
                            work->step = WORLD_COMPARE;
                        } else {
                            request->state = GARDEN_FILE_ERROR;
                            work->step = WORLD_DONE;
                            syFree(work->buffer);
                            work->buffer = NULL;
                            game_state.card_warning = SAVE_NOT_OURS;
                        }
                    } else {
                        request->state = GARDEN_FILE_ERROR;
                        work->step = WORLD_DONE;
                    }
                } else {
                    work->step = WORLD_SAVE;
                }
            } else {
                request->state = GARDEN_FILE_ERROR;
                work->step = WORLD_DONE;
            }
        } else {
            request->state = GARDEN_FILE_ERROR;
            work->step = WORLD_DONE;
            game_state.card_warning = SAVE_NOT_READY;
        }
        break;
    case WORLD_COMPARE:
        if (card_get_status(request->drive, &status) != 0) {
            request->state = GARDEN_FILE_ERROR;
            work->step = WORLD_DONE;
        } else if (!status.busy) {
            if (!status.error) {
                if (memcmp(work->buffer + WORLD_DATA_AT, request->data,
                           sizeof(struct garden_origin)) == 0) {
                    work->step = WORLD_SAVE;
                } else {
                    request->state = GARDEN_FILE_ERROR;
                    work->step = WORLD_DONE;
                    game_state.card_warning = SAVE_NOT_OURS;
                }
            } else {
                request->state = GARDEN_FILE_ERROR;
                work->step = WORLD_DONE;
            }
        }
        break;
    case WORLD_SAVE:
        if (buIsReady(request->drive)) {
            if (card_get_status(request->drive, &status) != 0) {
                request->state = GARDEN_FILE_ERROR;
                work->step = WORLD_DONE;
            } else if (!status.busy) {
                set_world_header(&header, request->data, request->size);
                /* @bug logic: retries each frame a save the card refuses,
                 * for as long as the card stays ready and idle. the request
                 * stays GARDEN_FILE_BUSY, since a refusal leaves step at
                 * WORLD_SAVE (BF at 0x0c51981a) */
                if (card_save_file(request->drive, world_card_filename, &header,
                                   0) == 0) {
                    work->step = WORLD_WRITING;
                }
#ifdef NONMATCHING
                /* this side ends the request with GARDEN_FILE_ERROR where the
                 * write can't start */
                else {
                    request->state = GARDEN_FILE_ERROR;
                    work->step = WORLD_DONE;
                }
#endif
            } else {
                request->state = GARDEN_FILE_ERROR;
                work->step = WORLD_DONE;
            }
        } else {
            request->state = GARDEN_FILE_ERROR;
            work->step = WORLD_DONE;
        }
        break;
    case WORLD_WRITING:
        if (card_get_status(request->drive, &status) != 0) {
            request->state = GARDEN_FILE_ERROR;
            work->step = WORLD_DONE;
        } else if (!status.busy) {
            if (!status.error) {
                request->state = GARDEN_FILE_OK;
            } else {
                request->state = GARDEN_FILE_ERROR;
            }
            work->step = WORLD_DONE;
        }
        break;
    /* @hack redundant: the empty default */
    default:
        break;
    }
    if (work->step == WORLD_DONE) {
        task_kill(task);
    }
}
static struct task *start_world_card(int step,
                                     struct garden_card_request *request)
{
    struct task *task;
    struct world_card_work *work;
    task = task_create(0, 2, step_world_card, world_card_task_name);
    if (task) {
        work = syMalloc(sizeof(*work));
        if (!work) {
            task_destroy(task);
            task = NULL;
        } else {
            task->extra.p = work;
            work->step = step;
            work->request = request;
            work->buffer = NULL;
            task->destructor = destroy_world_card;
        }
    }
    return task;
}
int garden_card_write(struct garden_card_request *request)
{
    struct task *task;
    struct world_card_work *work;
    int result = -1;
    request->drive = card_save_drive();
    request->state = GARDEN_FILE_ERROR;
    if (request->drive >= 0 && request->drive < CARD_DRIVES) {
        if ((task = start_world_card(WORLD_CHECK_CARD, request)) != NULL) {
            work = task->extra.p;
            work->request = request;
            result = 0;
            request->state = GARDEN_FILE_BUSY;
        }
        if (result != 0 && task) {
            task_kill(task);
        }
    }
    return result;
}
int garden_card_read(struct garden_card_request *request)
{
    struct task *task;
    struct world_card_work *work;
    int blocks;
    int result = -1;
    request->drive = card_save_drive();
    request->state = GARDEN_FILE_ERROR;
    if (request->drive < 0 || request->drive >= CARD_DRIVES) {
        return -1;
    }
#ifdef NONMATCHING
    /* this side marks a card without the file GARDEN_FILE_ABSENT, so that
     * garden_file_recover starts a new garden only there. any other failure
     * stays GARDEN_FILE_ERROR */
    if (!buIsReady(request->drive)) {
        return -1;
    }
    {
        Sint32 exists = buIsExistFile(request->drive, world_card_filename);
        if (exists != 0) {
            if (exists == BUD_ERR_FILE_NOT_FOUND) {
                request->state = GARDEN_FILE_ABSENT;
            }
            return -1;
        }
    }
#else
    if (!buIsReady(request->drive) ||
        buIsExistFile(request->drive, world_card_filename) != 0) {
        return -1;
    }
#endif
    if ((task = start_world_card(WORLD_READ, request)) != NULL) {
        work = task->extra.p;
        work->request = request;
#ifdef NONMATCHING
        /* this side sizes the buffer from the file's own blocks. a file of no
         * blocks, or of too many, is marked GARDEN_FILE_INVALID */
        blocks = buGetFileSize(request->drive, world_card_filename);
        if (blocks <= 0 || blocks > 0x7fffffff / CARD_BLOCK_BYTES ||
            request->size <= 0) {
            request->state = GARDEN_FILE_INVALID;
            task_kill(task);
            return -1;
        }
        work->buffer_size = blocks * CARD_BLOCK_BYTES;
#else
        blocks =
            ((request->size + (WORLD_FILE_PREFIX - 1)) / CARD_BLOCK_BYTES) + 1;
#endif
        if ((work->buffer = syMalloc(blocks * CARD_BLOCK_BYTES)) != NULL &&
            card_load_file_ex(request->drive, world_card_filename, work->buffer,
                              0, blocks) == 0) {
            request->state = GARDEN_FILE_BUSY;
            result = 0;
        }
    }
    if (result != 0 && task) {
        task_kill(task);
    }
    return result;
}

/* the walk game's file, an executable on the VMU. its first block names the
 * game at VMU_SIGNATURE_OFFSET. it gives each part's byte offset in the file
 * as a little-endian 16-bit word */
#define VMU_HEADER_BLOCK 0
#define VMU_SIGNATURE_OFFSET 0x1e0
#define VMU_LEGACY_CHAO_OFFSET 0x1ec /* in Sonic Adventure's walk game */
#define VMU_CHAO_OFFSET 0x1f0
#define VMU_EGG_OFFSET 0x1f2
#define VMU_ITEM_OFFSET 0x1f4
#define VMU_ENCOUNTER_OFFSET 0x1f6
#define VMU_SPRITE_OFFSET 0x1f8
#define VMU_LANGUAGE_OFFSET 0x1fd /* the text language it was installed in */
/* nonzero where the install is to delete the game and write nothing */
#define VMU_DELETE_OFFSET 0x1fe
#define VMU_LOCK_OFFSET 0x1ff /* nonzero while the garden writes the parts */
#define WALK_TYPE(vms_chao) ((vms_chao)->saved.record.saved.parameters.type)
#define WALK_INT16(data, offset) ((data)[offset] | ((data)[(offset) + 1] << 8))
#define WALK_OFFSETS(job)                                                      \
    {                                                                          \
        (job)->chao_offset = WALK_INT16((job)->header, VMU_CHAO_OFFSET);       \
        (job)->egg_offset = WALK_INT16((job)->header, VMU_EGG_OFFSET);         \
        (job)->item_offset = WALK_INT16((job)->header, VMU_ITEM_OFFSET);       \
        (job)->encounter_offset =                                              \
            WALK_INT16((job)->header, VMU_ENCOUNTER_OFFSET);                   \
        (job)->sprite_offset = WALK_INT16((job)->header, VMU_SPRITE_OFFSET);   \
    }
/* the blocks of a garden_vms_chao, of a legacy_chao, of the items and of the
 * sprites */
#define WALK_CHAO_BLOCKS (sizeof(struct garden_vms_chao) / CARD_BLOCK_BYTES)
#define LEGACY_CHAO_BLOCKS 1
#define WALK_ITEM_BLOCKS 1
#define WALK_SPRITE_BLOCKS 9

/* INSTALL_ENCOUNTERS fills the walk game's encounter part with encounters it
 * picks at random, none twice. walk_encounter_files holds ENCOUNTER_FILES
 * files for each text language. from the language's first, the install picks
 * LARGE_PICKED of LARGE_ENCOUNTERS. from the one at SMALL_FILE, it picks
 * SMALL_PICKED of SMALL_ENCOUNTERS */
#define ENCOUNTER_FILES 2
#define SMALL_FILE 1
#define LARGE_ENCOUNTERS 13
#define LARGE_ENCOUNTER_BYTES 4096
#define LARGE_PICKED 2
#define SMALL_ENCOUNTERS 32
#define SMALL_ENCOUNTER_BYTES 2048
#define SMALL_PICKED 8
#define ENCOUNTER_BYTES                                                        \
    ((LARGE_PICKED * LARGE_ENCOUNTER_BYTES) +                                  \
     (SMALL_PICKED * SMALL_ENCOUNTER_BYTES))
/* an encounter whose byte at ENCOUNTER_FLAGS_OFFSET holds ENCOUNTER_EXCLUDED
 * is never picked */
#define ENCOUNTER_FLAGS_OFFSET 3
#define ENCOUNTER_EXCLUDED 0x80
#ifdef NONMATCHING
/* the plain form the hack tag names */
#define ENCOUNTER_BIT(index) (1U << (index))
#else
/* @hack type: an int 1 shifted, where 1U would define the shift by 31 */
#define ENCOUNTER_BIT(index) (1 << (index))
#endif

enum walk_install_state {
    INSTALL_FIND,
    INSTALL_CHECK_HEADER,
    INSTALL_DEFRAG,
    INSTALL_PROGRAM,
    INSTALL_WAIT_PROGRAM,
    INSTALL_LOCK,
    INSTALL_WAIT_LOCK,
    INSTALL_UNLOCK,
    INSTALL_WAIT_UNLOCK,
    INSTALL_CHAO,
    INSTALL_WAIT_CHAO,
    INSTALL_EGG,
    INSTALL_WAIT_EGG,
    INSTALL_ITEMS,
    INSTALL_WAIT_ITEMS,
    INSTALL_SPRITES,
    INSTALL_WAIT_SPRITES,
    INSTALL_ENCOUNTERS,
    INSTALL_WAIT_ENCOUNTERS,
    INSTALL_REPLACE,
    INSTALL_WAIT_REPLACE,
    INSTALL_REMOVE,
    INSTALL_WAIT_REMOVE
};
#define INSTALL_NEXT(next)                                                     \
    {                                                                          \
        install->state = (next);                                               \
        result = GARDEN_TRANSFER_BUSY;                                         \
    }
#define WAIT_FOR_INSTALL(on_success)                                           \
    {                                                                          \
        struct card_status status;                                             \
        if (card_get_status(install->drive, &status) == 0) {                   \
            if (!status.busy) {                                                \
                if (!status.error) {                                           \
                    on_success;                                                \
                } else {                                                       \
                    garden_walk_message(WALK_NOTICE_CARD);                     \
                }                                                              \
            } else {                                                           \
                result = GARDEN_TRANSFER_BUSY;                                 \
            }                                                                  \
        } else {                                                               \
            garden_walk_message(WALK_NOTICE_CARD);                             \
        }                                                                      \
    }
#define WRITE_WALK_PART(part, offset, blocks, waiting, skipped)                \
    if (install->part) {                                                       \
        if (card_rewrite_exec_file(                                            \
                install->drive, walk_vmu_filename, install->part,              \
                install->offset / CARD_BLOCK_BYTES, blocks) == 0) {            \
            INSTALL_NEXT(waiting);                                             \
        } else {                                                               \
            garden_walk_message(WALK_NOTICE_CARD);                             \
        }                                                                      \
    } else {                                                                   \
        INSTALL_NEXT(skipped);                                                 \
    }

/* READ_WALK_PROGRAM(image) decodes the walk game for game_state's text
 * language into image, which must hold walk_game_blocks blocks. it is true
 * where the decode succeeds */
#ifdef NONMATCHING
/* loader_read_prs takes no capacity, so this side checks the
 * language and the decoded size */
static Bool read_walk_program(Uint8 *image)
{
    int language = game_state.text_language;
    Sint32 size = walk_game_blocks * CARD_BLOCK_BYTES;

    if ((unsigned)language >=
        sizeof(walk_programs) / sizeof(walk_programs[0])) {
        return FALSE;
    }
    return loader_read_prs_checked(walk_programs[language], image,
                                   (Uint32)size) == size;
}
#define READ_WALK_PROGRAM(image) read_walk_program(image)
#else
#define READ_WALK_PROGRAM(image)                                               \
    (loader_read_prs(walk_programs[game_state.text_language], (image)) == 0)
#endif

Sint32 garden_install_walk_game(struct walk_install *install)
{
    Sint32 result = GARDEN_TRANSFER_FAILED;
    switch (install->state) {
    case INSTALL_FIND: {
        char filename[16];
        install->image = NULL;
        install->header = NULL;
        if (buFindExecFile(install->drive, filename) == 0) {
            if (strcmp(filename, walk_vmu_filename) == 0) {
                if ((install->header = syMalloc(CARD_BLOCK_BYTES)) != NULL) {
                    if (card_load_file_ex(install->drive, filename,
                                          install->header, VMU_HEADER_BLOCK,
                                          1) != 0) {
                        garden_walk_message(WALK_NOTICE_BAD_FILE);
                    } else {
                        INSTALL_NEXT(INSTALL_CHECK_HEADER);
                    }
                } else {
                    garden_walk_message(WALK_NOTICE_CARD);
                }
            } else {
                garden_walk_message(WALK_NOTICE_OTHER_GAME);
            }
        } else if (buGetDiskFree(install->drive, BUD_FILETYPE_EXECUTABLE) >=
                   walk_game_blocks) {
            INSTALL_NEXT(INSTALL_PROGRAM);
        } else if (buGetDiskFree(install->drive, BUD_FILETYPE_NORMAL) >=
                   walk_game_blocks) {
            if ((install->image = syMalloc(CARD_BLOCK_BYTES)) != NULL) {
                if (buDefragDisk(install->drive, install->image) == 0) {
                    INSTALL_NEXT(INSTALL_DEFRAG);
                } else {
                    garden_walk_message(WALK_NOTICE_NO_SPACE);
                }
            } else {
                garden_walk_message(WALK_NOTICE_CARD);
            }
        } else {
            garden_walk_message(WALK_NOTICE_NO_SPACE);
        }
        break;
    }
    case INSTALL_CHECK_HEADER: {
        struct card_status status;
        if (card_get_status(install->drive, &status) == 0) {
            if (!status.busy) {
                if (!status.error) {
                    if (memcmp(install->header + VMU_SIGNATURE_OFFSET,
                               walk_signature, sizeof(walk_signature)) == 0) {
                        if (install->header[VMU_DELETE_OFFSET]) {
                            syFree(install->header);
                            install->header = NULL;
                            INSTALL_NEXT(INSTALL_REMOVE);
                        } else if (!install->header[VMU_LOCK_OFFSET] &&
                                   install->header[VMU_LANGUAGE_OFFSET] ==
                                       game_state.text_language) {
                            WALK_OFFSETS(install);
                            INSTALL_NEXT(INSTALL_LOCK);
                        } else {
                            syFree(install->header);
                            install->header = NULL;
                            INSTALL_NEXT(INSTALL_REPLACE);
                        }
                    } else {
                        garden_walk_message(WALK_NOTICE_BAD_FILE);
                    }
                } else {
                    garden_walk_message(WALK_NOTICE_CARD);
                }
            } else {
                result = GARDEN_TRANSFER_BUSY;
            }
        } else {
            garden_walk_message(WALK_NOTICE_CARD);
        }
        break;
    }
    case INSTALL_DEFRAG:
        if (!buStat(install->drive)) {
            switch (buGetLastError(install->drive)) {
            case 0:
                syFree(install->image);
                install->image = NULL;
                INSTALL_NEXT(INSTALL_PROGRAM);
                break;
            default:
                garden_walk_message(WALK_NOTICE_CARD);
                break;
            }
        } else {
            result = GARDEN_TRANSFER_BUSY;
        }
        break;
    case INSTALL_PROGRAM:
        if ((install->header = syMalloc(walk_game_blocks * CARD_BLOCK_BYTES)) !=
            NULL) {
            if (READ_WALK_PROGRAM(install->header)) {
                WALK_OFFSETS(install);
                if (card_save_exec_file(install->drive, walk_vmu_filename,
                                        install->header, walk_game_blocks,
                                        0) == 0) {
                    INSTALL_NEXT(INSTALL_WAIT_PROGRAM);
                } else {
                    garden_walk_message(WALK_NOTICE_CARD);
                }
            } else {
                garden_walk_message(WALK_NOTICE_CARD);
            }
        } else {
            garden_walk_message(WALK_NOTICE_CARD);
        }
        break;
    case INSTALL_WAIT_PROGRAM:
        WAIT_FOR_INSTALL(INSTALL_NEXT(INSTALL_LOCK));
        break;
    case INSTALL_LOCK:
        install->header[VMU_LOCK_OFFSET] = 1;
        if (card_rewrite_exec_file(install->drive, walk_vmu_filename,
                                   install->header, VMU_HEADER_BLOCK, 1) == 0) {
            INSTALL_NEXT(INSTALL_WAIT_LOCK);
        } else {
            garden_walk_message(WALK_NOTICE_BAD_FILE);
        }
        break;
    case INSTALL_WAIT_LOCK:
        WAIT_FOR_INSTALL(INSTALL_NEXT(INSTALL_CHAO));
        break;
    case INSTALL_UNLOCK:
        install->header[VMU_LOCK_OFFSET] = 0;
        install->header[VMU_DELETE_OFFSET] = 0;
        install->header[VMU_LANGUAGE_OFFSET] = game_state.text_language;
        if (card_rewrite_exec_file(install->drive, walk_vmu_filename,
                                   install->header, VMU_HEADER_BLOCK, 1) == 0) {
            INSTALL_NEXT(INSTALL_WAIT_UNLOCK);
        } else {
            garden_walk_message(WALK_NOTICE_CARD);
        }
        break;
    case INSTALL_WAIT_UNLOCK:
        WAIT_FOR_INSTALL(result = GARDEN_TRANSFER_DONE);
        break;
    case INSTALL_CHAO:
        WRITE_WALK_PART(chao, chao_offset, WALK_CHAO_BLOCKS, INSTALL_WAIT_CHAO,
                        INSTALL_EGG);
        break;
    case INSTALL_WAIT_CHAO:
        WAIT_FOR_INSTALL(INSTALL_NEXT(INSTALL_EGG));
        break;
    case INSTALL_EGG:
        WRITE_WALK_PART(egg, egg_offset, WALK_CHAO_BLOCKS, INSTALL_WAIT_EGG,
                        INSTALL_ITEMS);
        break;
    case INSTALL_WAIT_EGG:
        WAIT_FOR_INSTALL(INSTALL_NEXT(INSTALL_ITEMS));
        break;
    case INSTALL_ITEMS:
        WRITE_WALK_PART(items, item_offset, WALK_ITEM_BLOCKS,
                        INSTALL_WAIT_ITEMS, INSTALL_SPRITES);
        break;
    case INSTALL_WAIT_ITEMS:
        WAIT_FOR_INSTALL(INSTALL_NEXT(INSTALL_SPRITES));
        break;
    case INSTALL_SPRITES:
        if (install->chao && WALK_TYPE(install->chao)) {
            const char *name;
            switch (WALK_TYPE(install->chao)) {
            case CHAO_CHILD:
            case CHAO_HERO_TYPE(CHAO_CHILD):
            case CHAO_DARK_TYPE(CHAO_CHILD):
            case CHAO_NORMAL_ADULT:
            default:
                name = walk_sprite_neutral_normal;
                break;
            case CHAO_SWIM_ADULT:
                name = walk_sprite_neutral_swim;
                break;
            case CHAO_FLY_ADULT:
                name = walk_sprite_neutral_fly;
                break;
            case CHAO_RUN_ADULT:
                name = walk_sprite_neutral_run;
                break;
            case CHAO_POWER_ADULT:
                name = walk_sprite_neutral_power;
                break;
            case CHAO_CHAOS:
                name = walk_sprite_neutral_chaos;
                break;
            case CHAO_HERO_TYPE(CHAO_NORMAL_ADULT):
                name = walk_sprite_hero_normal;
                break;
            case CHAO_HERO_TYPE(CHAO_SWIM_ADULT):
                name = walk_sprite_hero_swim;
                break;
            case CHAO_HERO_TYPE(CHAO_FLY_ADULT):
                name = walk_sprite_hero_fly;
                break;
            case CHAO_HERO_TYPE(CHAO_RUN_ADULT):
                name = walk_sprite_hero_run;
                break;
            case CHAO_HERO_TYPE(CHAO_POWER_ADULT):
                name = walk_sprite_hero_power;
                break;
            case CHAO_HERO_CHAOS:
                name = walk_sprite_hero_chaos;
                break;
            case CHAO_DARK_TYPE(CHAO_NORMAL_ADULT):
                name = walk_sprite_dark_normal;
                break;
            case CHAO_DARK_TYPE(CHAO_SWIM_ADULT):
                name = walk_sprite_dark_swim;
                break;
            case CHAO_DARK_TYPE(CHAO_FLY_ADULT):
                name = walk_sprite_dark_fly;
                break;
            case CHAO_DARK_TYPE(CHAO_RUN_ADULT):
                name = walk_sprite_dark_run;
                break;
            case CHAO_DARK_TYPE(CHAO_POWER_ADULT):
                name = walk_sprite_dark_power;
                break;
            case CHAO_DARK_CHAOS:
                name = walk_sprite_dark_chaos;
                break;
            }
            if ((install->image = loader_read_prs_alloc(name)) != NULL) {
                if (card_rewrite_exec_file(
                        install->drive, walk_vmu_filename, install->image,
                        install->sprite_offset / CARD_BLOCK_BYTES,
                        WALK_SPRITE_BLOCKS) == 0) {
                    INSTALL_NEXT(INSTALL_WAIT_SPRITES);
                } else {
                    syFree(install->image);
                    install->image = NULL;
                    garden_walk_message(WALK_NOTICE_CARD);
                }
            } else {
                garden_walk_message(WALK_NOTICE_CARD);
            }
        } else {
            INSTALL_NEXT(INSTALL_ENCOUNTERS);
        }
        break;
    case INSTALL_WAIT_SPRITES:
        WAIT_FOR_INSTALL(syFree(install->image); install->image = NULL;
                         INSTALL_NEXT(INSTALL_ENCOUNTERS));
        break;
    case INSTALL_ENCOUNTERS:
        if (install->chao && WALK_TYPE(install->chao)) {
            Sint32 count;
            Uint8(*large)[LARGE_ENCOUNTER_BYTES];
            Uint8(*small)[SMALL_ENCOUNTER_BYTES];
            Uint32 used;
            Sint32 index;
            Uint8 *record;
            const char *name;
            Sint32 language;
            if ((install->image = syMalloc(ENCOUNTER_BYTES)) != NULL) {
                name = walk_encounter_files[game_state.text_language *
                                            ENCOUNTER_FILES];
                if ((large = loader_read_prs_alloc(name)) != NULL) {
                    used = 0;
                    count = 0;
                    while (count < LARGE_PICKED) {
#ifdef NONMATCHING
                        /* this side gives up once every large encounter has
                         * been picked. the matching side loops for good */
                        if (used == 0x1fffU) {
                            syFree(large);
                            goto failed_encounters;
                        }
#endif
                        index = rand() * ((Float)LARGE_ENCOUNTERS * RAND_SCALE);
                        if (index >= 0 && index < LARGE_ENCOUNTERS &&
                            !(used & (ENCOUNTER_BIT(index)))) {
                            used |= ENCOUNTER_BIT(index);
                            record = large[index];
                            if (!(record[ENCOUNTER_FLAGS_OFFSET] &
                                  ENCOUNTER_EXCLUDED)) {
                                memcpy(install->image +
                                           (count * LARGE_ENCOUNTER_BYTES),
                                       record, LARGE_ENCOUNTER_BYTES);
                                /* @hack spelling: count = count + 1, where
                                 * count++ would do */
                                count = count + 1;
                            }
                        }
                    }
                    syFree(large);
                    /* @hack spelling: the language in a local, and the
                     * array offset to SMALL_FILE before its index, where
                     * one index would do */
                    language = game_state.text_language;
                    name = (walk_encounter_files +
                            SMALL_FILE)[language * ENCOUNTER_FILES];
                    if ((small = loader_read_prs_alloc(name)) != NULL) {
                        used = 0;
                        count = 0;
                        while (count < SMALL_PICKED) {
#ifdef NONMATCHING
                            /* this side gives up once every small encounter has
                             * been picked, where the matching side loops for
                             * good */
                            if (used == 0xffffffffUL) {
                                syFree(small);
                                goto failed_encounters;
                            }
#endif
                            index =
                                rand() * ((Float)SMALL_ENCOUNTERS * RAND_SCALE);
                            /* @hack spelling: the tests of index nested,
                             * where one test would do */
                            if (index >= 0) {
                                if (index < SMALL_ENCOUNTERS &&
                                    !(used & (ENCOUNTER_BIT(index)))) {
                                    used |= ENCOUNTER_BIT(index);
                                    record = small[index];
                                    if (!(record[ENCOUNTER_FLAGS_OFFSET] &
                                          ENCOUNTER_EXCLUDED)) {
                                        memcpy(
                                            install->image +
                                                (LARGE_PICKED *
                                                 LARGE_ENCOUNTER_BYTES) +
                                                (count * SMALL_ENCOUNTER_BYTES),
                                            record, SMALL_ENCOUNTER_BYTES);
                                        count++;
                                    }
                                }
                            }
                        }
                        syFree(small);
                    } else {
                    failed_encounters:
                        garden_walk_message(WALK_NOTICE_CARD);
                        syFree(install->image);
                        install->image = NULL;
                        break;
                    }
                    if (card_rewrite_exec_file(
                            install->drive, walk_vmu_filename, install->image,
                            install->encounter_offset / CARD_BLOCK_BYTES,
                            ENCOUNTER_BYTES / CARD_BLOCK_BYTES) == 0) {
                        INSTALL_NEXT(INSTALL_WAIT_ENCOUNTERS);
                    } else {
                        garden_walk_message(WALK_NOTICE_CARD);
                    }
                } else {
                    /* @hack spelling: a goto into the small file's failure
                     * arm, where a copy of the arm would do */
                    goto failed_encounters;
                }
            } else {
                garden_walk_message(WALK_NOTICE_CARD);
            }
        } else {
            INSTALL_NEXT(INSTALL_UNLOCK);
        }
        break;
    case INSTALL_WAIT_ENCOUNTERS: {
        struct card_status status;
        if (card_get_status(install->drive, &status) == 0) {
            if (!status.busy) {
                /* @bug logic: accepts a write that failed. the second test
                 * reads status.busy in place of status.error, so it always
                 * passes (MOV.L @(16,R15),R3 at 0x0c51a53a) */
#ifdef NONMATCHING
                /* this side repairs the bug tagged above: it tests
                 * status.error */
                if (!status.error)
#else
                if (!status.busy)
#endif
                {
                    syFree(install->image);
                    install->image = NULL;
                    INSTALL_NEXT(INSTALL_UNLOCK);
                } else {
                    garden_walk_message(WALK_NOTICE_CARD);
                }
            } else {
                result = GARDEN_TRANSFER_BUSY;
            }
        } else {
            garden_walk_message(WALK_NOTICE_CARD);
        }
        break;
    }
    case INSTALL_REPLACE:
        if (buIsReady(install->drive)) {
            if (buDeleteFile(install->drive, walk_vmu_filename) == 0) {
                INSTALL_NEXT(INSTALL_WAIT_REPLACE);
            } else {
                result = GARDEN_TRANSFER_BUSY;
            }
        } else {
            /* @hack redundant: the empty arm for a card that isn't ready */
        }
        break;
    case INSTALL_WAIT_REPLACE: {
        struct card_status status;
        if (buIsReady(install->drive)) {
            if (card_get_status(install->drive, &status) == 0) {
                if (!status.busy) {
                    /* @bug logic: accepts a delete that failed, as
                     * INSTALL_WAIT_ENCOUNTERS accepts a write (MOV.L
                     * @(16,R15),R3 at 0x0c51a5de) */
#ifdef NONMATCHING
                    /* this side repairs the bug tagged above: it tests
                     * status.error */
                    if (!status.error)
#else
                    if (!status.busy)
#endif
                    {
                        INSTALL_NEXT(INSTALL_PROGRAM);
                    } else {
                        garden_walk_message(WALK_NOTICE_CARD);
                    }
                } else {
                    result = GARDEN_TRANSFER_BUSY;
                }
            } else {
                garden_walk_message(WALK_NOTICE_CARD);
            }
        } else {
            garden_walk_message(WALK_NOTICE_CARD);
        }
        break;
    }
    case INSTALL_REMOVE:
        if (buIsReady(install->drive)) {
            if (buDeleteFile(install->drive, walk_vmu_filename) == 0) {
                INSTALL_NEXT(INSTALL_WAIT_REMOVE);
            } else {
                result = GARDEN_TRANSFER_BUSY;
            }
        } else {
            garden_walk_message(WALK_NOTICE_CARD);
        }
        break;
    case INSTALL_WAIT_REMOVE: {
        struct card_status status;
        if (buIsReady(install->drive)) {
            if (card_get_status(install->drive, &status) == 0) {
                if (!status.busy) {
                    /* @bug logic: accepts a delete that failed, as
                     * INSTALL_WAIT_ENCOUNTERS accepts a write (MOV.L
                     * @(16,R15),R3 at 0x0c51a64c) */
#ifdef NONMATCHING
                    /* this side repairs the bug tagged above: it tests
                     * status.error */
                    if (!status.error)
#else
                    if (!status.busy)
#endif
                    {
                        result = GARDEN_TRANSFER_DONE;
                    } else {
                        garden_walk_message(WALK_NOTICE_CARD);
                    }
                } else {
                    result = GARDEN_TRANSFER_BUSY;
                }
            } else {
                garden_walk_message(WALK_NOTICE_CARD);
            }
        } else {
            garden_walk_message(WALK_NOTICE_CARD);
        }
        break;
    }
    }
    if (result != GARDEN_TRANSFER_BUSY) {
        if (install->image) {
            syFree(install->image);
            install->image = NULL;
        }
        if (install->header) {
            syFree(install->header);
            install->header = NULL;
        }
    }
    return result;
}

enum walk_read_state {
    READ_FIND,
    READ_LEGACY_HEADER,
    READ_WALK_HEADER,
    READ_CHAO,
    READ_WAIT_CHAO,
    READ_EGG,
    READ_WAIT_EGG,
    READ_ITEMS,
    READ_WAIT_ITEMS
};
/* a read->flags bit: the VMU runs Sonic Adventure's walk game. the read loads
 * LEGACY_CHAO_BLOCKS at the Chao's offset as its Chao. it loads as many from
 * the block after that offset as its egg, and no items */
#define WALK_LEGACY 1
#define READ_NEXT(next)                                                        \
    {                                                                          \
        read->state = (next);                                                  \
        result = GARDEN_TRANSFER_BUSY;                                         \
    }
#define WAIT_FOR_READ(on_success)                                              \
    {                                                                          \
        struct card_status status;                                             \
        /* @hack redundant: the empty arms of these tests */                   \
        if (card_get_status(read->drive, &status) == 0) {                      \
            if (!status.busy) {                                                \
                if (!status.error) {                                           \
                    on_success;                                                \
                } else {                                                       \
                }                                                              \
            } else {                                                           \
                result = GARDEN_TRANSFER_BUSY;                                 \
            }                                                                  \
        } else {                                                               \
        }                                                                      \
    }
Sint32 garden_read_walk_file(struct walk_read *read)
{
    Sint32 result = GARDEN_TRANSFER_FAILED;
    switch (read->state) {
    case READ_FIND:
        read->header = NULL;
        read->flags = 0;
        /* @hack redundant: the empty arms of this test and those inside it */
        if (buFindExecFile(read->drive, read->filename) == 0) {
            if (strcmp(read->filename, walk_vmu_filename) == 0) {
                if ((read->header = syMalloc(CARD_BLOCK_BYTES)) != NULL) {
                    if (card_load_file_ex(read->drive, read->filename,
                                          read->header, VMU_HEADER_BLOCK,
                                          1) != 0) {
                    } else {
                        READ_NEXT(READ_WALK_HEADER);
                    }
                } else {
                }
            } else {
                if ((read->header = syMalloc(CARD_BLOCK_BYTES)) != NULL) {
                    if (card_load_file_ex(read->drive, read->filename,
                                          read->header, VMU_HEADER_BLOCK,
                                          1) == 0) {
                        READ_NEXT(READ_LEGACY_HEADER);
                    } else {
                    }
                } else {
                }
            }
        } else {
        }
        break;
    case READ_LEGACY_HEADER: {
        struct card_status status;
        /* @hack redundant: the empty arms of this test and those inside it */
        if (card_get_status(read->drive, &status) == 0) {
            if (!status.busy) {
                if (!status.error) {
                    if (memcmp(read->header + VMU_SIGNATURE_OFFSET,
                               walk_legacy_signature,
                               sizeof(walk_legacy_signature)) == 0) {
                        read->chao_offset =
                            WALK_INT16(read->header, VMU_LEGACY_CHAO_OFFSET);
                        read->egg_offset = read->chao_offset + CARD_BLOCK_BYTES;
                        read->item_offset = 0;
                        read->flags |= WALK_LEGACY;
                        syFree(read->header);
                        read->header = NULL;
                        READ_NEXT(READ_CHAO);
                    } else {
                    }
                } else {
                }
            } else {
                result = GARDEN_TRANSFER_BUSY;
            }
        } else {
        }
        break;
    }
    case READ_WALK_HEADER: {
        struct card_status status;
        /* @hack redundant: the empty arms of this test and those inside it */
        if (card_get_status(read->drive, &status) == 0) {
            if (!status.busy) {
                if (!status.error) {
                    if (memcmp(read->header + VMU_SIGNATURE_OFFSET,
                               walk_signature, sizeof(walk_signature)) == 0) {
                        if (read->header[VMU_LOCK_OFFSET]) {
                        } else {
                            read->chao_offset =
                                WALK_INT16(read->header, VMU_CHAO_OFFSET);
                            read->egg_offset =
                                WALK_INT16(read->header, VMU_EGG_OFFSET);
                            read->item_offset =
                                WALK_INT16(read->header, VMU_ITEM_OFFSET);
                            syFree(read->header);
                            read->header = NULL;
                            READ_NEXT(READ_CHAO);
                        }
                    } else {
                    }
                } else {
                }
            } else {
                result = GARDEN_TRANSFER_BUSY;
            }
        } else {
        }
        break;
    }
    case READ_CHAO:
        if (read->chao) {
            Uint32 blocks = WALK_CHAO_BLOCKS;
            if (read->flags & WALK_LEGACY) {
                blocks = LEGACY_CHAO_BLOCKS;
            }
            if (card_load_file_ex(read->drive, read->filename, read->chao,
                                  read->chao_offset / CARD_BLOCK_BYTES,
                                  blocks) == 0) {
                READ_NEXT(READ_WAIT_CHAO);
            } else {
                /* @hack redundant: the empty arm for a load that can't start */
            }
        } else {
            /* @bug logic: fails a read that has no Chao buffer, though the
             * arm moves the state on to READ_EGG. result keeps the function's
             * first value, GARDEN_TRANSFER_FAILED (MOV.L R2,@R14 at
             * 0x0c51a95a, MOV #4,R12 at 0x0c51a6fe) */
            read->state = READ_EGG;
#ifdef NONMATCHING
            /* this side repairs the bug tagged above: the read goes on as
             * busy */
            result = GARDEN_TRANSFER_BUSY;
#endif
        }
        break;
    case READ_WAIT_CHAO:
        WAIT_FOR_READ(READ_NEXT(READ_EGG));
        break;
    case READ_EGG:
        if (read->egg) {
            Uint32 blocks = WALK_CHAO_BLOCKS;
            if (read->flags & WALK_LEGACY) {
                blocks = LEGACY_CHAO_BLOCKS;
            }
            if (card_load_file_ex(read->drive, read->filename, read->egg,
                                  read->egg_offset / CARD_BLOCK_BYTES,
                                  blocks) == 0) {
                READ_NEXT(READ_WAIT_EGG);
            } else {
                /* @hack redundant: the empty arm for a load that can't start */
            }
        } else {
            READ_NEXT(READ_ITEMS);
        }
        break;
    case READ_WAIT_EGG:
        WAIT_FOR_READ(READ_NEXT(READ_ITEMS));
        break;
    case READ_ITEMS:
        if (read->items && read->item_offset) {
            if (card_load_file_ex(read->drive, read->filename, read->items,
                                  read->item_offset / CARD_BLOCK_BYTES,
                                  WALK_ITEM_BLOCKS) == 0) {
                READ_NEXT(READ_WAIT_ITEMS);
            } else {
                /* @hack redundant: the empty arm for a load that can't start */
            }
        } else {
            result = GARDEN_TRANSFER_DONE;
        }
        break;
    case READ_WAIT_ITEMS:
        WAIT_FOR_READ(result = GARDEN_TRANSFER_DONE);
        break;
    }
    if (result != GARDEN_TRANSFER_BUSY) {
        if (read->header) {
            syFree(read->header);
            read->header = NULL;
        }
    }
    return result;
}

/* the states of a drive's slot. SLOT_NO_WALK holds a mounted card the slot
 * got no walk game from. the card has none, or the slot's allocation, read or
 * egg rewrite failed */
enum walk_slot_state {
    SLOT_EMPTY,
    SLOT_READING,
    SLOT_REWRITING,
    SLOT_LOADED,
    SLOT_NO_WALK
};
/* a walk game's items block. the send screen sends nothing to a card whose
 * byte at WALK_NO_SEND_OFFSET is 1. the items' count lies at
 * WALK_ITEM_COUNT_OFFSET, and their ids follow it, at most WALK_MAX_ITEMS. a
 * send keeps the block's first WALK_ITEM_HEAD_BYTES and clears the rest */
#define WALK_NO_SEND_OFFSET 0
/* the byte a drive cell reads as the count, where the content cells read the
 * one at WALK_ITEM_COUNT_OFFSET. the cell only sets CELL_ITEMS from it, which
 * nothing acts on */
#define WALK_CELL_COUNT_OFFSET 1
#define WALK_ITEM_COUNT_OFFSET 2
#define WALK_ITEM_IDS_OFFSET 3
#define WALK_ITEM_HEAD_BYTES 16
/* drives->mode: what garden_draw_walk_drives shows */
enum drive_display_mode {
    DRIVE_HIDDEN = 0,
    DRIVE_STATUS = 1,
    DRIVE_CONTENTS = 3
};
#define WALK_DEBUG_GRAY 0xffc0c0c0UL
#define WALK_DEBUG_YELLOW 0xffffff00UL
#define WALK_DEBUG_WHITE 0xffffffffUL
/* the debug display's places. DEBUG_LIST starts a row for each drive, and the
 * cursor's mark stands at DEBUG_MARK. a row's contents stand DEBUG_STEP
 * apart */
#define DEBUG_LIST NJM_LOCATION(16, 6)
#define DEBUG_MARK NJM_LOCATION(14, 6)
#define DEBUG_NAME NJM_LOCATION(12, 3)
#define DEBUG_STEP NJM_LOCATION(2, 0)
#define DEBUG_CONFIRM NJM_LOCATION(20, 0)
#define DEBUG_BELOW NJM_LOCATION(0, 1)
/* the offset of chao_parameters.name in a garden_vms_chao */
#define WALK_CHAO_NAME_OFFSET 8
/* a drive's read, and the records it loads. 16 bytes nothing reads lie
 * between them */
struct walk_slot_buffer {
    struct walk_read read;
    Uint8 unused[16];
    struct garden_vms_chao chao, egg;
    Uint8 items[CARD_BLOCK_BYTES];
};

void garden_draw_walk_drives(struct walk_drives *drives)
{
    struct walk_slot *slot;
#ifdef NONMATCHING
#else
    /* @hack redundant: status, a local nothing reads */
    struct card_status status;
#endif
    Uint32 location;
    Sint32 i;
    Sint32 count;
    Uint8 *data;
    switch (drives->mode) {
    case DRIVE_STATUS:
        njPrintColor(WALK_DEBUG_GRAY);
        for (i = 0; i < CARD_DRIVES; i++) {
            slot = &drives->slots[i];
            location = DEBUG_LIST + i;
            switch (slot->state) {
            case SLOT_EMPTY:
                njPrint(location, walk_label_disk_absent);
                break;
            case SLOT_READING:
                njPrint(location, walk_label_disk_reading);
                break;
            case SLOT_LOADED:
                njPrint(location, walk_label_disk_loaded);
                break;
            case SLOT_NO_WALK:
                njPrint(location, walk_label_disk_empty);
                break;
            }
        }
        njPrint(DEBUG_MARK + drives->cursor, walk_label_cursor);
        break;
    case DRIVE_CONTENTS:
        location = drives->cursor + DEBUG_LIST;
        njPrintColor((drives->flags & (1 << WALK_CHAO_CELL)) ? WALK_DEBUG_YELLOW
                                                             : WALK_DEBUG_GRAY);
        njPrint(location, (drives->chao && WALK_TYPE(drives->chao))
                              ? walk_label_chao
                              : walk_label_empty);
        /* @bug memory: reads the Chao's name through drives->chao where
         * that is null. the test above expects a null (TST R2,R2 at
         * 0x0c51abc2), but njPrint takes the name untested (ADD #8,R2 at
         * 0x0c51abe8) */
#ifdef NONMATCHING
        /* this side repairs the bug tagged above. it prints no name without a
         * Chao, and ends the name it prints with a zero */
        if (drives->chao) {
            char name[sizeof(drives->chao->saved.record.saved.parameters.name) +
                      1];
            memcpy(name, drives->chao->saved.record.saved.parameters.name,
                   sizeof(name) - 1);
            name[sizeof(name) - 1] = '\0';
            njPrint(DEBUG_NAME, walk_label_name, name);
        }
#else
        {
            /* @hack spelling: the name as drives->chao plus its offset, where
             * the name field would do */
            njPrint(DEBUG_NAME, walk_label_name,
                    (const char *)drives->chao + WALK_CHAO_NAME_OFFSET);
        }
#endif
        njPrintColor((drives->flags & (1 << WALK_EGG_CELL)) ? WALK_DEBUG_YELLOW
                                                            : WALK_DEBUG_GRAY);
        njPrint(location + (WALK_EGG_CELL * DEBUG_STEP),
                (drives->egg && WALK_TYPE(drives->egg)) ? walk_label_egg
                                                        : walk_label_empty);
        count = (data = drives->items) ? data[WALK_ITEM_COUNT_OFFSET] : 0;
#ifdef NONMATCHING
        /* this side prints no more items than WALK_MAX_ITEMS */
        if (count > WALK_MAX_ITEMS) {
            count = WALK_MAX_ITEMS;
        }
#endif
        for (i = 0; i < count; i++) {
            njPrintColor((drives->flags & (1 << (i + WALK_ITEM_CELL)))
                             ? WALK_DEBUG_YELLOW
                             : WALK_DEBUG_GRAY);
            njPrint(location + ((i + WALK_ITEM_CELL) * DEBUG_STEP),
                    walk_label_item, data[i + WALK_ITEM_IDS_OFFSET]);
        }
        njPrintColor(WALK_DEBUG_GRAY);
        for (i = count; i < WALK_MAX_ITEMS; i++) {
            njPrint(location + ((i + WALK_ITEM_CELL) * DEBUG_STEP),
                    walk_label_empty);
        }
        njPrint(location + DEBUG_CONFIRM, walk_label_confirm);
        njPrintColor(WALK_DEBUG_WHITE);
        njPrint(location + DEBUG_BELOW + (drives->column * DEBUG_STEP),
                walk_label_underline);
        break;
    }
}
void garden_update_walk_drives(struct walk_drives *drives)
{
    Sint32 drive;
    struct walk_slot *slot;
    struct walk_read *read;
    struct card_status status;
    struct chao_save *saved;
    struct chao_save *source;
    Uint8 *buffer;
    for (drive = 0; drive < CARD_DRIVES; drive++) {
        slot = &drives->slots[drive];
        card_get_status(drive, &status);
        switch (slot->state) {
        case SLOT_EMPTY:
            if (status.mounted) {
                if ((slot->read = syMalloc(sizeof(struct walk_slot_buffer))) !=
                    NULL) {
                    /* @hack spelling: the egg and the items at their offsets
                     * past the Chao, where their fields would do */
                    buffer =
                        (Uint8 *)&((struct walk_slot_buffer *)slot->read)->chao;
                    slot->read->drive = drive;
                    slot->read->chao = (struct garden_vms_chao *)buffer;
                    slot->read->egg =
                        (struct garden_vms_chao *)(buffer +
                                                   sizeof(
                                                       struct garden_vms_chao));
                    slot->read->items =
                        buffer + (2 * sizeof(struct garden_vms_chao));
                    slot->read->state = READ_FIND;
                    slot->state = SLOT_READING;
                } else {
                    slot->state = SLOT_NO_WALK;
                }
            }
            break;
        case SLOT_READING:
            switch (garden_read_walk_file(slot->read)) {
            case GARDEN_TRANSFER_DONE:
                saved = (struct chao_save *)(read = slot->read)->egg;
                if (saved && !(read->flags & WALK_LEGACY)) {
                    struct chao_parameters *parameters =
                        &saved->record.saved.parameters;
                    /* @hack redundant: the test of parameters, a field's
                     * address, which is never null */
                    if (parameters && parameters->type != CHAO_EGG &&
                        parameters->type != CHAO_NONE) {
                        source = (struct chao_save *)read->chao;
                        if (source) {
                            buffer =
                                syCalloc(1, sizeof(struct garden_vms_chao));
                            if (buffer) {
                                slot->buffer = buffer;
                                memcpy(buffer, saved,
                                       sizeof(struct garden_vms_chao));
                                chao_make_saved_egg(
                                    source, (struct chao_save *)slot->buffer);
                                if (card_rewrite_exec_file(
                                        drive, walk_vmu_filename, slot->buffer,
                                        read->egg_offset / CARD_BLOCK_BYTES,
                                        WALK_CHAO_BLOCKS) == 0) {
                                    garden_walk_menu->busy = TRUE;
                                    slot->state = SLOT_REWRITING;
                                    return;
                                }
                            }
                        }
                    }
                }
                slot->state = SLOT_LOADED;
                break;
            case GARDEN_TRANSFER_FAILED:
                syFree(slot->read);
                slot->read = NULL;
                slot->state = SLOT_NO_WALK;
                break;
            }
            break;
        case SLOT_REWRITING:
            read = slot->read;
            garden_walk_menu->busy = TRUE;
            if (card_get_status(read->drive, &status) == 0) {
                if (!status.busy) {
                    if (!status.error) {
                        syFree(slot->buffer);
                        slot->buffer = NULL;
                        syFree(slot->read);
                        slot->read = NULL;
                        slot->state = SLOT_EMPTY;
                    } else {
                        syFree(slot->buffer);
                        slot->buffer = NULL;
                        slot->state = SLOT_NO_WALK;
                    }
                }
                break;
            } else {
                syFree(slot->buffer);
                slot->buffer = NULL;
                slot->state = SLOT_NO_WALK;
            }
            break;
        case SLOT_LOADED:
            if (!status.mounted) {
                syFree(slot->read);
                slot->read = NULL;
                slot->state = SLOT_EMPTY;
            }
            break;
        case SLOT_NO_WALK:
            if (!status.mounted) {
                /* @bug memory: leaks the read of a slot whose egg rewrite
                 * failed. the failure frees only the egg's buffer (JSR @R12
                 * at 0x0c51af14 and 0x0c51af1e). once the card comes out,
                 * this arm empties the slot with its read still set (MOV.L
                 * R13,@R14 at 0x0c51af66). SLOT_EMPTY's next read replaces
                 * it */
#ifdef NONMATCHING
                /* this side repairs the bug tagged above: it frees the read as
                 * it empties the slot */
                if (slot->read) {
                    syFree(slot->read);
                    slot->read = NULL;
                }
#endif
                slot->state = SLOT_EMPTY;
            }
            break;
        }
    }
}

#define COPY_WALK_CONTENTS(drives, read)                                       \
    {                                                                          \
        if ((drives)->chao) {                                                  \
            memcpy((drives)->chao, (read)->chao,                               \
                   sizeof(struct garden_vms_chao));                            \
        }                                                                      \
        if ((drives)->egg) {                                                   \
            memcpy((drives)->egg, (read)->egg,                                 \
                   sizeof(struct garden_vms_chao));                            \
        }                                                                      \
        if ((drives)->items) {                                                 \
            memcpy((drives)->items, (read)->items, CARD_BLOCK_BYTES);          \
        }                                                                      \
    }
/* drives->result after a send's select: whether it copied the card's contents
 * into drives. it copies none from a card with no walk game */
enum walk_drives_result { WALK_NOTHING_COPIED, WALK_CONTENTS_COPIED };
Sint32 garden_select_walk_drive(struct walk_drives *drives, Sint32 index)
{
    Sint32 result = GARDEN_TRANSFER_FAILED;
    struct walk_slot *slot = &drives->slots[index];
    Sint32 i;
    switch (drives->request) {
    case WALK_COLLECT:
        switch (slot->state) {
        case SLOT_LOADED:
            if (drives->chao && !(slot->read->flags & WALK_LEGACY)) {
                memcpy(drives->chao, slot->read->chao,
                       sizeof(struct garden_vms_chao));
            }
            if (drives->egg) {
                if (slot->read->flags & WALK_LEGACY) {
#ifdef NONMATCHING
                    /* this side copies no egg from a slot without a buffer */
                    if (!slot->buffer) {
                        break;
                    }
#endif
                    memcpy(drives->egg, slot->buffer,
                           sizeof(struct garden_vms_chao));
                } else {
                    memcpy(drives->egg, slot->read->egg,
                           sizeof(struct garden_vms_chao));
                }
            }
            if (drives->items) {
                memcpy(drives->items, slot->read->items, CARD_BLOCK_BYTES);
            }
            drives->mode = DRIVE_CONTENTS;
            drives->column = 0;
            drives->flags = 0;
            result = GARDEN_TRANSFER_DONE;
            break;
        }
        break;
    case WALK_SEND_CHAO:
        switch (slot->state) {
        case SLOT_NO_WALK:
            drives->result = WALK_NOTHING_COPIED;
            result = GARDEN_TRANSFER_DONE;
            break;
        case SLOT_LOADED:
            if (WALK_TYPE(slot->read->chao) == CHAO_NONE) {
                COPY_WALK_CONTENTS(drives, slot->read);
                drives->result = WALK_CONTENTS_COPIED;
                result = GARDEN_TRANSFER_DONE;
            } else if (slot->read->flags & WALK_LEGACY) {
                result = WALK_SELECT_LEGACY;
            } else {
                result = GARDEN_TRANSFER_FAILED;
            }
            break;
        }
        break;
    case WALK_SEND_EGG:
        switch (slot->state) {
        case SLOT_NO_WALK:
            drives->result = WALK_NOTHING_COPIED;
            result = GARDEN_TRANSFER_DONE;
            break;
        case SLOT_LOADED:
            if (WALK_TYPE(slot->read->egg) == CHAO_NONE) {
                COPY_WALK_CONTENTS(drives, slot->read);
                drives->result = WALK_CONTENTS_COPIED;
                result = GARDEN_TRANSFER_DONE;
            } else {
                result = GARDEN_TRANSFER_FAILED;
            }
            break;
        }
    }
    for (i = 0; i < CARD_DRIVES; i++) {
        slot = &drives->slots[i];
        if (slot->read) {
            syFree(slot->read);
            slot->read = NULL;
        }
        if (slot->buffer) {
            syFree(slot->buffer);
            slot->buffer = NULL;
        }
    }
    return result;
}
void garden_update_occupied_drives(struct walk_slot *slots)
{
    struct walk_slot *slot = slots;
    /* @hack type: drive a Sint8, where an int would do */
    Sint8 drive;
    for (drive = 0; drive < CARD_DRIVES; drive++, slot++) {
        if (slot->state == SLOT_LOADED || slot->state == SLOT_NO_WALK) {
            garden_walk_menu->occupied |= 1 << drive;
            if (slot->state == SLOT_LOADED && slot->read) {
                if ((slot->read->flags & WALK_LEGACY) && slot->read->chao &&
                    !slot->buffer) {
                    slot->buffer = syCalloc(1, sizeof(struct garden_vms_chao));
                    if (slot->buffer) {
                        chao_import_legacy(
                            (struct legacy_chao *)slot->read->chao,
                            (struct chao_save *)slot->buffer);
                    }
                }
            }
        } else {
            garden_walk_menu->occupied &= ~(1 << drive);
            if (slot->state == SLOT_EMPTY && slot->buffer) {
                syFree(slot->buffer);
                slot->buffer = NULL;
            }
        }
    }
}
Sint8 garden_walk_drive_type(Sint8 drive)
{
    if (drive < 0 || drive > CARD_DRIVES - 1) {
        return WALK_DRIVE_NONE;
    }
    /* @hack type: occupied read as a Sint8, where its Uint8 would do */
    if ((Sint8)garden_walk_menu->occupied & (1 << drive)) {
        if (is_multi_bank(drive)) {
            return WALK_DRIVE_MULTI_BANK;
        }
        return WALK_DRIVE_CARD;
    }
    return WALK_DRIVE_NONE;
}

/* the walk menu's cells, each a task in garden_walk_menu->cells, stand in
 * columns of WALK_GRID_ROWS. the content cells show the Chao, the egg and
 * the items of the drive on its page */
#define CELL_COLUMN(id) ((id) / WALK_GRID_ROWS)
#define CELL_ROW(id) ((id) % WALK_GRID_ROWS)
/* garden_walk_menu->send: the screen the menu shows */
enum walk_screen_kind { RECEIVE_SCREEN, SEND_SCREEN };

enum walk_content_kind {
    WALK_CONTENT_CHAO,
    WALK_CONTENT_EGG,
    WALK_CONTENT_ITEM
};
/* a content cell's flags: it shows a preview, and the preview's Chao comes
 * from Sonic Adventure's walk game */
enum walk_content_flags { CONTENT_AVAILABLE = 1, CONTENT_LEGACY = 2 };
/* the first item id that is a fruit. the ids below it are seeds */
#define WALK_FIRST_FRUIT 9
/* a cell task's work, which CELL_WORK lays over its struct task_work. flags
 * holds walk_cell_flags, and column and row lie in ang.x and ang.z */
struct walk_cell_work {
    Sint8 state, substate;
    Uint8 id, tick;
    Sint16 flags;
    Uint16 timer;
    Sint32 column, unk_0c, row;
};
#define CELL_WORK(task) ((struct walk_cell_work *)(task)->work)
struct walk_drive_cell {
    struct task *frame, *chao, *egg;
    struct walk_slot *slot;
};
struct walk_content_cell {
    Sint8 drive, kind;
    Sint8 item; /* the item's id, or -1 for the Chao or the egg */
    Sint8 column, row; /* the cell's place, which nothing reads */
    Uint8 flags;
    struct task *preview;
};
#define SET_CELL_STATE(work, next)                                             \
    ((work)->state = (next), (work)->substate = 0)
/* a drive cell's states. the cell waits for its drive's slot to reach
 * SLOT_LOADED or SLOT_NO_WALK, then opens its frame. it shows the drive's Chao
 * and egg until the slot empties */
enum drive_cell_state {
    DRIVE_CELL_START,
    DRIVE_CELL_DELAY,
    DRIVE_CELL_WAIT,
    DRIVE_CELL_OPEN,
    DRIVE_CELL_SHOWN,
    DRIVE_CELL_FROZEN,
    DRIVE_CELL_CLOSE,
    DRIVE_CELL_END
};
/* a content cell's states. CONTENT_CELL_EMPTY has nothing to show.
 * CONTENT_CELL_SHOWN unchecks a checked cell the garden has no room for. it
 * moves one the garden has room for to CONTENT_CELL_PICKED, until the player
 * unchecks it */
enum content_cell_state {
    CONTENT_CELL_START,
    CONTENT_CELL_WAIT,
    CONTENT_CELL_LOAD,
    CONTENT_CELL_EMPTY,
    CONTENT_CELL_SHOWN,
    CONTENT_CELL_PICKED,
    CONTENT_CELL_FROZEN,
    CONTENT_CELL_END
};
/* a content cell's waits, in frames */
#define CONTENT_CELL_DELAY_FRAMES 20 /* before it first loads */
#define CONTENT_CELL_SETTLE_FRAMES 5 /* after a load */
#define CONTENT_CELL_RELOAD_FRAMES 10 /* before it loads another drive's */
/* where the name of the Chao under the cursor shows. a cell that opens or
 * loads under the cursor puts it at NAME_X and NAME_Y. a shown drive cell the
 * cursor comes to puts it at DRIVE_NAME_X and DRIVE_NAME_Y.
 * DRIVE_COLUMN_WIDTH and DRIVE_ROW_HEIGHT move it by the cell's column and
 * row */
#define NAME_X 65.0F
#define NAME_Y 86.0F
#define NAME_Z (-5.0F)
#define DRIVE_NAME_X 78.0F
#define DRIVE_NAME_Y 89.0F
#define DRIVE_COLUMN_WIDTH 128.0F
#define DRIVE_ROW_HEIGHT 131.0F
/* the frames a preview takes to blend into a motion: a blend, a slower one,
 * and a cut */
#define PREVIEW_BLEND 10
#define PREVIEW_SLOW_BLEND 15
#define PREVIEW_CUT 1
/* a drive cell's frames: its delay before it looks at its slot, and its
 * opening. its frame starts DRIVE_FRAME_LEAD frames before the opening ends */
#define DRIVE_CELL_DELAY_FRAMES 60
#define DRIVE_CELL_OPEN_FRAMES 10
#define DRIVE_FRAME_LEAD 5

static void step_drive_cell(struct task *task)
{
    struct walk_cell_work *work = CELL_WORK(task);
    struct walk_drive_cell *cell = task->extra.p;
    struct walk_slot *slot;
    struct walk_read *read;
    NJS_POINT3 position;
    int cursor;
    Sint8 *count_byte;
    int count;
#ifdef NONMATCHING
#else
    /* @hack redundant: slot_state, which nothing reads */
    /* NOLINTBEGIN(clang-diagnostic-unused-but-set-variable) */
    int slot_state;
    /* NOLINTEND(clang-diagnostic-unused-but-set-variable) */
#endif
    if (game_state.time_stopped) {
        return;
    }
    if (garden_walk_menu->mode == WALK_MENU_FREEZE) {
        SET_CELL_STATE(work, DRIVE_CELL_FROZEN);
    }
    switch (work->state) {
    case DRIVE_CELL_START:
        work->timer = DRIVE_CELL_DELAY_FRAMES;
        SET_CELL_STATE(work, DRIVE_CELL_DELAY);
        break;
    case DRIVE_CELL_DELAY:
        if (work->timer > 0) {
            work->timer--;
        } else {
            SET_CELL_STATE(work, DRIVE_CELL_WAIT);
        }
        break;
    case DRIVE_CELL_WAIT:
        cursor = garden_walk_menu->cursor;
        if (cursor == work->id && !(work->flags & CELL_FOCUS)) {
            work->flags |= CELL_FOCUS;
            garden_walk_change_message(6);
        }
        if (cursor != work->id && (work->flags & CELL_FOCUS)) {
            work->flags &= ~CELL_FOCUS;
        }
        slot = cell->slot;
        if (slot &&
            (slot->state == SLOT_LOADED || slot->state == SLOT_NO_WALK)) {
            work->timer = DRIVE_CELL_OPEN_FRAMES;
            cell->frame = walk_create_drive_frame(
                task, work->id, work->timer - DRIVE_FRAME_LEAD);
            if (cell->frame) {
                work->flags |= CELL_FRAME;
                if (cell->slot->state == SLOT_LOADED &&
                    !(cell->slot->read->flags & WALK_LEGACY)) {
                    work->flags |= CELL_CURRENT_FORMAT;
                }
            }
            SET_CELL_STATE(work, DRIVE_CELL_OPEN);
        }
        break;
    case DRIVE_CELL_OPEN:
        if (work->timer > 0) {
            work->timer--;
        } else {
            read = (slot = cell->slot)->read;
            if (read && !(read->flags & WALK_LEGACY)) {
                if (read->chao) {
                    cell->chao = walk_create_chao_preview(
                        task, work->column, work->row, read->chao, work->id);
                    if (cell->chao) {
                        work->flags |= CELL_CHAO;
                        if (work->id == garden_walk_menu->cursor) {
                            walk_set_preview_motion(
                                cell->chao, WALK_PREVIEW_FOCUS, PREVIEW_BLEND);
                            position.x = NAME_X;
                            position.y = NAME_Y;
                            position.z = NAME_Z;
                            walk_show_preview_name(cell->chao, &position);
                            work->flags |= CELL_FOCUS;
                        } else {
                            walk_set_preview_motion(
                                cell->chao, WALK_PREVIEW_IDLE, PREVIEW_BLEND);
                        }
                    }
                }
                if (read->egg) {
                    cell->egg = walk_create_egg_preview(
                        task, work->column, work->row, read->egg, work->id);
                    if (cell->egg) {
                        work->flags |= CELL_EGG;
                    }
                }
                if (read->items) {
                    /* @hack spelling: the count read through a pointer to
                     * its byte, where a read of the byte would do */
                    count_byte = (Sint8 *)&read->items[WALK_CELL_COUNT_OFFSET];
                    count = *count_byte;
                    if (count > 0) {
                        work->flags |= CELL_ITEMS;
                    }
                }
            }
#ifdef NONMATCHING
#else
            slot_state = slot->state;
#endif
            SET_CELL_STATE(work, DRIVE_CELL_SHOWN);
        }
        break;
    case DRIVE_CELL_SHOWN:
        if (cell->slot->state == SLOT_EMPTY) {
            if (cell->frame) {
                task_kill(cell->frame);
                work->flags &= ~CELL_FRAME;
                work->flags &= ~CELL_CURRENT_FORMAT;
                cell->frame = NULL;
            }
            if (cell->chao) {
                task_kill(cell->chao);
                cell->chao = NULL;
                work->flags &= ~CELL_CHAO;
            }
            if (cell->egg) {
                task_kill(cell->egg);
                cell->egg = NULL;
                work->flags &= ~CELL_EGG;
            }
            /* @hack redundant: the test of CELL_ITEMS before its clear */
            if (work->flags & CELL_ITEMS) {
                work->flags &= ~CELL_ITEMS;
            }
            SET_CELL_STATE(work, DRIVE_CELL_WAIT);
            break;
        }
        cursor = garden_walk_menu->cursor;
        if (cursor == work->id && !(work->flags & CELL_FOCUS)) {
            work->flags |= CELL_FOCUS;
            if (cell->chao) {
                /* NOLINTBEGIN(bugprone-integer-division) */
                position.x = DRIVE_NAME_X + ((Float)CELL_COLUMN(work->id) *
                                             DRIVE_COLUMN_WIDTH);
                /* NOLINTEND(bugprone-integer-division) */
                position.y = DRIVE_NAME_Y +
                             ((Float)CELL_ROW(work->id) * DRIVE_ROW_HEIGHT);
                position.z = NAME_Z;
                walk_show_preview_name(cell->chao, &position);
                garden_walk_message(8);
                walk_set_preview_motion(cell->chao, WALK_PREVIEW_REFUSE,
                                        PREVIEW_SLOW_BLEND);
            } else {
                garden_walk_change_message(6);
            }
        } else if (cursor != work->id && (work->flags & CELL_FOCUS)) {
            work->flags &= ~CELL_FOCUS;
            if (cell->chao) {
                walk_set_preview_motion(cell->chao, WALK_PREVIEW_IDLE,
                                        PREVIEW_BLEND);
                walk_hide_preview_name(cell->chao);
            }
        }
        break;
    case DRIVE_CELL_FROZEN:
        break;
    case DRIVE_CELL_CLOSE:
        if (cell->chao) {
            task_kill(cell->chao);
            cell->chao = NULL;
        }
        if (cell->egg) {
            task_kill(cell->egg);
            cell->egg = NULL;
        }
        if (cell->frame) {
            task_kill(cell->frame);
            cell->frame = NULL;
        }
        SET_CELL_STATE(work, DRIVE_CELL_END);
        break;
    case DRIVE_CELL_END:
        task_kill(task);
        return;
    }
    switch (work->state) {
    case DRIVE_CELL_START:
    case DRIVE_CELL_DELAY:
    case DRIVE_CELL_OPEN:
    case DRIVE_CELL_CLOSE:
    case DRIVE_CELL_END:
        garden_walk_menu->busy = TRUE;
        break;
    }
    if (work->state < DRIVE_CELL_CLOSE &&
        walk_menu_mode() == WALK_MENU_CLOSING) {
        SET_CELL_STATE(work, DRIVE_CELL_CLOSE);
    }
}
static void draw_drive_cell(struct task *task)
{
}
static void destroy_drive_cell(struct task *task)
{
    struct walk_cell_work *work = CELL_WORK(task);
    struct walk_drive_cell *cell = task->extra.p;
#ifdef NONMATCHING
    /* this side destroys a cell that has no extra without reading it. a cell
     * whose create failed has none */
    if (!cell) {
        return;
    }
#endif
    if (cell->chao) {
        task_kill(cell->chao);
        cell->chao = NULL;
    }
    if (cell->egg) {
        task_kill(cell->egg);
        cell->egg = NULL;
    }
    if (cell->frame) {
        task_kill(cell->frame);
        cell->frame = NULL;
    }
    task_destroy_children(task);
    garden_walk_menu->cells[work->id] = NULL;
}
static struct task *create_drive_cell(struct task *parent, int id,
                                      struct walk_slot *slot)
{
    struct task *task;
    struct walk_drive_cell *cell;
    struct walk_cell_work *work;
    if (garden_walk_menu->cells[id]) {
        return NULL;
    }
    if (!parent) {
        return NULL;
    }
    task = task_create_child(TASK_WORK, step_drive_cell, parent);
    if (!task) {
        return NULL;
    }
    task->draw = draw_drive_cell;
    task->destructor = destroy_drive_cell;
    cell = syCalloc(1, sizeof(*cell));
    if (!cell) {
        task_destroy(task);
        return NULL;
    }
    task->extra.p = cell;
    work = CELL_WORK(task);
    work->id = id;
    work->column = CELL_COLUMN(id);
    work->row = CELL_ROW(id);
    cell->slot = slot;
    garden_walk_menu->cells[work->id] = task;
    return task;
}
struct task *walk_cell_egg_preview(int id)
{
    struct task *task = garden_walk_menu->cells[id];
    struct walk_drive_cell *cell;
    if (!task) {
        return NULL;
    }
    cell = task->extra.p;
    if (!cell) {
        return NULL;
    }
    return cell->egg;
}

/* the room_for_walk_ functions say whether the garden has room for what the
 * cell with id skip brings. they count what the garden holds and what the
 * other checked cells bring */
#define FRUIT_CAPACITY 10
#define PLANT_CAPACITY 4 /* grown trees and seeds */
/* COUNT_CHECKED_ITEMS(test) adds to count each checked item cell with a
 * preview whose test holds. it skips the cell with id skip. it steps the
 * caller's i and at through the cells. it sets the caller's task, work and
 * cell to each, and test reads cell */
#define COUNT_CHECKED_ITEMS(test)                                              \
    /* @hack spelling: at = at + 1, where at++ would do */                     \
    for (at = &garden_walk_menu->cells[WALK_ITEM_CELL], i = WALK_ITEM_CELL;    \
         i < WALK_CONTENT_CELLS; at = at + 1, i++) {                           \
        task = *at;                                                            \
        CHECK_LIVE_CELL(task);                                                 \
        work = CELL_WORK(task);                                                \
        cell = task->extra.p;                                                  \
        /* @hack redundant: the else arm that continues at the loop's end */   \
        if (i != skip) {                                                       \
            if ((test) && (cell->flags & CONTENT_AVAILABLE) &&                 \
                (work->flags & CELL_CHECKED)) {                                \
                count++;                                                       \
            }                                                                  \
        } else {                                                               \
            continue;                                                          \
        }                                                                      \
    }
#ifdef NONMATCHING
/* this side skips a cell whose task is missing */
#define CHECK_LIVE_CELL(task)                                                  \
    if (!(task)) {                                                             \
        continue;                                                              \
    }
#else
#define CHECK_LIVE_CELL(task)
#endif

static Bool room_for_walk_fruit(int skip)
{
    int count;
    int i;
    struct task **at;
    struct task *task;
    struct walk_cell_work *work;
    struct walk_content_cell *cell;
    count = garden_active_fruit_count();
    COUNT_CHECKED_ITEMS(cell->item >= WALK_FIRST_FRUIT);
    if (count < FRUIT_CAPACITY) {
        return TRUE;
    }
    return FALSE;
}
static Bool room_for_walk_seed(int skip)
{
    int count;
    int i;
    struct task **at;
    struct task *task;
    struct walk_cell_work *work;
    struct walk_content_cell *cell;
    count =
        garden_planted_tree_count() + garden_object_count(GARDEN_OBJECT_SEED);
    COUNT_CHECKED_ITEMS(cell->item < WALK_FIRST_FRUIT);
    if (count < PLANT_CAPACITY) {
        return TRUE;
    }
    return FALSE;
}
static Bool room_for_walk_chao(int skip)
{
    int count;
    int i;
    struct task **at;
    struct walk_cell_work *work;
    struct walk_content_cell *cell;
    /* @hack redundant: a garden_active_fruit_count whose count nothing reads */
    garden_active_fruit_count();
    count = 0;
    for (at = &garden_walk_menu->cells[WALK_CHAO_CELL], i = WALK_CHAO_CELL;
         i < WALK_ITEM_CELL; at++, i++) {
        CHECK_LIVE_CELL(*at);
        work = CELL_WORK(*at);
        cell = (*at)->extra.p;
        /* @hack redundant: the else arm that continues at the loop's end */
        if (i != skip) {
            if ((cell->flags & CONTENT_AVAILABLE) &&
                (work->flags & CELL_CHECKED)) {
                count++;
            }
        } else {
            continue;
        }
    }
    if (garden_resident_count() + count < CHAO_PER_GARDEN &&
        garden_total_chao() + count < CHAO_SAVE_SLOTS) {
        return TRUE;
    }
    return FALSE;
}

static void step_content_cell(struct task *task)
{
    struct walk_cell_work *work = CELL_WORK(task);
    struct walk_content_cell *cell = task->extra.p;
    struct walk_drives *drives;
    struct walk_job *job;
    struct walk_slot *slot;
    struct walk_read *read;
    NJS_POINT3 position;
    int index;
    Bool allowed;
    Sint8 item_index;
    Sint8 item;
    Sint8 *count_byte;
    Sint8 column;
    Sint8 row;
    int count;
    if (game_state.time_stopped) {
        return;
    }
    job = garden_walk_menu->job;
    drives = job->work.drives;
    if (garden_walk_menu->mode == WALK_MENU_FREEZE) {
        SET_CELL_STATE(work, CONTENT_CELL_FROZEN);
    }
    if (!drives) {
        task_kill(task);
        walk_set_menu_busy(TRUE);
        return;
    }
    index = cell->drive;
    slot = &drives->slots[index];
#ifdef NONMATCHING
#else
    /* @hack redundant: a test of index that never runs */
    if (0) {
        if (index) {
        }
    }
#endif
    switch (work->state) {
    case CONTENT_CELL_START:
        work->timer = CONTENT_CELL_DELAY_FRAMES;
        walk_set_menu_busy(TRUE);
        SET_CELL_STATE(work, CONTENT_CELL_WAIT);
        break;
    case CONTENT_CELL_WAIT:
        if (work->timer > 0) {
            if (cell->preview) {
                task_kill(cell->preview);
                cell->preview = NULL;
                cell->flags &= ~CONTENT_AVAILABLE;
                cell->flags &= ~CONTENT_LEGACY;
            }
            walk_set_menu_busy(TRUE);
            work->timer--;
        } else if (slot->state == SLOT_LOADED && slot->read) {
            if (!garden_walk_menu->busy) {
                SET_CELL_STATE(work, CONTENT_CELL_LOAD);
                walk_set_menu_busy(TRUE);
                return;
            }
        }
        break;
    case CONTENT_CELL_LOAD:
        if (slot->state == SLOT_LOADED && slot->read) {
            read = slot->read;
            walk_set_menu_busy(TRUE);
            /* @hack type: flags read as a Uint32, where its Sint32 would do */
            if ((Uint32)read->flags & WALK_LEGACY) {
                if (cell->kind == WALK_CONTENT_CHAO && slot->buffer) {
                    cell->preview = walk_create_chao_preview(
                        task, CELL_COLUMN(WALK_EGG_CELL),
                        CELL_ROW(WALK_EGG_CELL),
                        (struct garden_vms_chao *)slot->buffer,
                        WALK_CONTENT_PREVIEW);
                    if (cell->preview) {
                        cell->flags |= CONTENT_AVAILABLE;
                        cell->flags |= CONTENT_LEGACY;
                        garden_walk_message(40);
                    }
                }
            } else {
                switch (cell->kind) {
                case WALK_CONTENT_CHAO:
                    if (slot->read->chao) {
                        cell->preview = walk_create_chao_preview(
                            task, CELL_COLUMN(WALK_CHAO_CELL),
                            CELL_ROW(WALK_CHAO_CELL), read->chao,
                            WALK_CONTENT_PREVIEW);
                        if (cell->preview) {
                            cell->flags |= CONTENT_AVAILABLE;
                            cell->item = -1;
                            if (work->id == garden_walk_menu->cursor) {
                                walk_set_preview_motion(cell->preview,
                                                        WALK_PREVIEW_FOCUS,
                                                        PREVIEW_BLEND);
                                position.x = NAME_X;
                                position.y = NAME_Y;
                                position.z = NAME_Z;
                                walk_preview_chao(cell->preview);
                                walk_show_preview_name(cell->preview,
                                                       &position);
                                work->flags |= CELL_FOCUS;
                            } else {
                                walk_set_preview_motion(cell->preview,
                                                        WALK_PREVIEW_IDLE,
                                                        PREVIEW_BLEND);
                            }
                        }
                    }
                    break;
                case WALK_CONTENT_EGG:
                    if (slot->read->egg) {
                        cell->preview = walk_create_egg_preview(
                            task, CELL_COLUMN(WALK_EGG_CELL),
                            CELL_ROW(WALK_EGG_CELL), read->egg,
                            WALK_CONTENT_PREVIEW);
                        if (cell->preview) {
                            cell->flags |= CONTENT_AVAILABLE;
                            cell->item = -1;
                        }
                    }
                    break;
                case WALK_CONTENT_ITEM:
                    if (slot->read->items) {
                        item_index = work->id - WALK_ITEM_CELL;
                        /* @hack spelling: the count read through a pointer
                         * to its byte, where a read of the byte would do */
                        count_byte =
                            (Sint8 *)&read->items[WALK_ITEM_COUNT_OFFSET];
                        count = *count_byte;
                        if (item_index < count) {
                            item =
                                (Sint8)read
                                    ->items[item_index + WALK_ITEM_IDS_OFFSET];
                            column = CELL_COLUMN(work->id);
                            row = CELL_ROW(work->id);
                            cell->preview = walk_create_item_preview(
                                task, column, row, item);
                            if (cell->preview) {
                                cell->flags |= CONTENT_AVAILABLE;
                                cell->item = item;
                            }
                        }
                    }
                    break;
                }
                if (work->id == garden_walk_menu->cursor) {
                    work->flags |= CELL_FOCUS;
                }
                garden_walk_change_message(22);
            }
            if (cell->flags & CONTENT_AVAILABLE) {
                SET_CELL_STATE(work, CONTENT_CELL_SHOWN);
            } else {
                SET_CELL_STATE(work, CONTENT_CELL_EMPTY);
            }
            work->timer = CONTENT_CELL_SETTLE_FRAMES;
            return;
        } else {
            SET_CELL_STATE(work, CONTENT_CELL_WAIT);
            work->timer = CONTENT_CELL_SETTLE_FRAMES;
        }
        break;
    case CONTENT_CELL_EMPTY:
        if (work->timer > 0) {
            walk_set_menu_busy(TRUE);
            work->timer--;
            /* @hack redundant: a break before the case's own */
            break;
        }
        break;
    case CONTENT_CELL_SHOWN:
        if (work->timer > 0) {
            walk_set_menu_busy(TRUE);
            work->timer--;
            break;
        }
        if (work->flags & CELL_CHECKED) {
            allowed = FALSE;
            walk_set_menu_busy(TRUE);
            /* @hack spelling: each room_for_walk_ result compared with TRUE,
             * where a test of it would do */
            switch (cell->kind) {
            case WALK_CONTENT_CHAO:
                if (!(cell->flags & CONTENT_LEGACY)) {
                    if (room_for_walk_chao(work->id) == TRUE) {
                        walk_set_preview_motion(
                            cell->preview, WALK_PREVIEW_ACCEPT, PREVIEW_BLEND);
                        allowed = TRUE;
                    } else {
                        walk_set_preview_motion(
                            cell->preview, WALK_PREVIEW_REFUSE, PREVIEW_BLEND);
                        garden_walk_message(59);
                    }
                } else {
                    if (room_for_walk_chao(work->id) == TRUE) {
                        garden_walk_message(45);
                        allowed = TRUE;
                    } else {
                        garden_walk_message(61);
                    }
                }
                break;
            case WALK_CONTENT_EGG:
                if (room_for_walk_chao(work->id) == TRUE) {
                    allowed = TRUE;
                } else {
                    garden_walk_message(61);
                }
                break;
            case WALK_CONTENT_ITEM:
                if (cell->item < WALK_FIRST_FRUIT) {
                    if (room_for_walk_seed(work->id) == TRUE) {
                        allowed = TRUE;
                    } else {
                        garden_walk_message(57);
                    }
                } else {
                    if (room_for_walk_fruit(work->id) == TRUE) {
                        allowed = TRUE;
                    } else {
                        garden_walk_message(55);
                    }
                }
                break;
            }
            /* @hack spelling: allowed compared with TRUE, where a test of it
             * would do */
            if (allowed == TRUE) {
                SET_CELL_STATE(work, CONTENT_CELL_PICKED);
            } else {
                work->flags &= ~CELL_CHECKED;
            }
        }
        if (work->id == garden_walk_menu->cursor &&
            !(work->flags & CELL_FOCUS) && cell->kind == WALK_CONTENT_CHAO) {
            walk_set_preview_motion(cell->preview, WALK_PREVIEW_FOCUS,
                                    PREVIEW_BLEND);
            position.x = NAME_X;
            position.y = NAME_Y;
            position.z = NAME_Z;
            walk_show_preview_name(cell->preview, &position);
            work->flags |= CELL_FOCUS;
        }
        if (work->id != garden_walk_menu->cursor &&
            (work->flags & CELL_FOCUS) && cell->kind == WALK_CONTENT_CHAO) {
            walk_set_preview_motion(cell->preview, WALK_PREVIEW_IDLE,
                                    PREVIEW_BLEND);
            walk_hide_preview_name(cell->preview);
            work->flags &= ~CELL_FOCUS;
        }
        break;
    case CONTENT_CELL_PICKED:
        if (!(work->flags & CELL_CHECKED)) {
            walk_set_menu_busy(TRUE);
            if (cell->kind == WALK_CONTENT_CHAO) {
                walk_set_preview_motion(cell->preview, WALK_PREVIEW_FOCUS,
                                        PREVIEW_CUT);
            }
            SET_CELL_STATE(work, CONTENT_CELL_SHOWN);
        }
        break;
    case CONTENT_CELL_FROZEN:
        break;
    case CONTENT_CELL_END:
        if (cell->preview) {
            task_kill(cell->preview);
            cell->preview = NULL;
            cell->flags &= ~CONTENT_AVAILABLE;
            /* @hack redundant: the test of CONTENT_LEGACY before its clear */
            if (cell->flags & CONTENT_LEGACY) {
                cell->flags &= ~CONTENT_LEGACY;
            }
        }
        task_kill(task);
        walk_set_menu_busy(TRUE);
        return;
    default:
        break;
    }
    switch (work->state) {
    case CONTENT_CELL_SHOWN:
    case CONTENT_CELL_EMPTY:
    case CONTENT_CELL_PICKED:
        if (index != drives->cursor || slot->state == SLOT_EMPTY) {
            if (cell->preview) {
                task_kill(cell->preview);
                cell->preview = NULL;
                cell->flags &= ~CONTENT_AVAILABLE;
                cell->flags &= ~CONTENT_LEGACY;
            }
            work->flags &= ~CELL_CHECKED;
            work->flags &= ~CELL_FOCUS;
            SET_CELL_STATE(work, CONTENT_CELL_WAIT);
            work->timer = CONTENT_CELL_RELOAD_FRAMES;
            cell->drive = drives->cursor;
            walk_set_menu_busy(TRUE);
            return;
        }
    }
    if (work->state < CONTENT_CELL_FROZEN) {
        cell->drive = drives->cursor;
    }
    if (work->state < CONTENT_CELL_END &&
        walk_menu_mode() == WALK_MENU_CLOSING) {
        SET_CELL_STATE(work, CONTENT_CELL_END);
    }
}
static void destroy_content_cell(struct task *task)
{
    struct walk_cell_work *work = CELL_WORK(task);
    struct walk_content_cell *cell = task->extra.p;
#ifdef NONMATCHING
    /* this side destroys a cell that has no extra without reading it. a cell
     * whose create failed has none */
    if (!cell) {
        return;
    }
#endif
    if (cell->preview) {
        task_kill(cell->preview);
        cell->preview = NULL;
    }
    syFree(cell);
    task->extra.p = NULL;
    garden_walk_menu->cells[work->id] = NULL;
}
static struct task *create_content_cell(struct task *parent, Sint8 id)
{
    struct task *task;
    struct walk_content_cell *cell;
    struct walk_cell_work *work;
    if (garden_walk_menu->cells[id]) {
        return NULL;
    }
    if (!parent) {
        return NULL;
    }
    task = task_create_child(TASK_WORK, step_content_cell, parent);
    if (!task) {
        return NULL;
    }
    task->draw = NULL;
    task->destructor = destroy_content_cell;
    work = CELL_WORK(task);
    cell = syCalloc(1, sizeof(*cell));
    if (!cell) {
        task_destroy(task);
        return NULL;
    }
    task->extra.p = cell;
    work->id = id;
    switch (id) {
    case WALK_CHAO_CELL:
        cell->kind = WALK_CONTENT_CHAO;
        break;
    case WALK_EGG_CELL:
        cell->kind = WALK_CONTENT_EGG;
        break;
    default:
        cell->kind = WALK_CONTENT_ITEM;
        break;
    }
    cell->column = CELL_COLUMN(id);
    cell->row = CELL_ROW(id);
    walk_set_menu_busy(TRUE);
    garden_walk_menu->cells[id] = task;
    return task;
}
int walk_menu_mode(void)
{
    if (garden_walk_menu == NULL) {
        return -1;
    }
    return garden_walk_menu->mode;
}
int walk_menu_busy(void)
{
    if (garden_walk_menu == NULL) {
        return -1;
    }
    return garden_walk_menu->busy;
}
int walk_set_menu_busy(int busy)
{
    if (garden_walk_menu == NULL) {
        return -1;
    }
    garden_walk_menu->busy = busy;
    return TRUE;
}
/* TRUE where the card the garden saves to is ready, or where it saves to
 * none. otherwise it moves the menu to MENU_WAIT_CARD and returns FALSE.
 * MENU_WAIT_CARD resumes the menu's state once the card is ready */
static Bool wait_for_save_card(void)
{
    Sint32 drive;
    drive = garden_walk_menu->drive;
    if (drive < 0) {
        return TRUE;
    }
    /* @hack spelling: the result compared with TRUE, where a test of it would
     * do */
    if (buIsReady(drive) == TRUE) {
        return TRUE;
    }
    if (garden_walk_menu->state != MENU_WAIT_CARD) {
        garden_walk_menu->previous = garden_walk_menu->state;
        garden_walk_menu->state = MENU_WAIT_CARD;
        garden_walk_message(63);
    }
    return FALSE;
}

enum walk_menu_sound {
    WALK_SOUND_PAGE = 0x1007,
    WALK_SOUND_SELECT = 0x100e,
    WALK_SOUND_BACK = 0x1010,
    WALK_SOUND_MOVE = 0x8000
};
#define PLAY_WALK_SOUND(id) sound_play((id), NULL, 0, 0)
/* garden_walk_menu->cursor where the receive screen's cursor stands on row
 * WALK_GRID_ROWS, below the cells, to confirm */
#define CONFIRM_CURSOR (-1)
/* garden_walk_menu->selected where the send screen has picked no drive */
#define NO_DRIVE_PICKED (-1)
/* the delays, in frames, of the menu's title and message bar, and of its
 * frame and cursor */
#define TITLE_DELAY 20
#define FRAME_DELAY 60
/* the menu's waits, in frames */
#define MENU_ENTER_FRAMES 20 /* before the menu takes input */
#define PAGE_TURN_FRAMES 30
#define LEGACY_COLLECT_FRAMES 5 /* before a legacy Chao's collect ends */
#define SAVE_DELAY_FRAMES 5 /* after the install, before the save starts */
#define SAVE_WAIT_FRAMES 120 /* before the menu first asks the save's state */
#define RESUME_FRAMES 5 /* after the save card comes back */
#define RESULT_FRAMES 100
/* garden_walk_message's id for no message */
#define NO_MESSAGE (-1)
/* garden_walk_menu->busy once MENU_CLOSE has run a frame. the next frame
 * kills the menu */
#define BUSY_CLOSING 2
#define SCAN_MENU_DRIVES(drives)                                               \
    garden_update_walk_drives(drives);                                         \
    garden_update_occupied_drives((drives)->slots);                            \
    {                                                                          \
        int i;                                                                 \
        struct walk_slot *slot;                                                \
        for (i = 0; i < CARD_DRIVES; i++) {                                    \
            slot = &(drives)->slots[i];                                        \
            if (slot->state == SLOT_READING ||                                 \
                slot->state == SLOT_REWRITING) {                               \
                walk_set_menu_busy(TRUE);                                      \
                break;                                                         \
            }                                                                  \
        }                                                                      \
    }
#define WALK_CONTENT_MOVE(button, within, action)                              \
    if ((pad_press_repeat[0] & (button)) && (within)) {                        \
        if (!quiet) {                                                          \
            garden_walk_change_message(22);                                    \
        }                                                                      \
        PLAY_WALK_SOUND(WALK_SOUND_MOVE);                                      \
        action;                                                                \
    }
#define WALK_DRIVE_MOVE(button, within, action)                                \
    if (pad_press_repeat[0] & (button)) {                                      \
        if (within) {                                                          \
            PLAY_WALK_SOUND(WALK_SOUND_MOVE);                                  \
            action;                                                            \
        }                                                                      \
    }

void garden_walk_menu_step(struct task *task)
{
#ifdef NONMATCHING
#else
    /* @hack redundant: work and extra, the task's records, which nothing
     * reads */
    struct task_work *work = task->work;
    void *extra = task->extra.p;
#endif
    struct walk_job *job = garden_walk_menu->job;
    if (game_state.time_stopped) {
        return;
    }
    if (garden_walk_menu->state != MENU_CLOSE &&
        garden_walk_menu->mode == WALK_MENU_CLOSING) {
        switch (garden_walk_menu->state) {
        case MENU_ENTER:
        case MENU_PICK_CARD:
        case MENU_CHOOSE_CARD:
        case MENU_PICK_CONTENTS:
        case MENU_OPEN: {
            struct walk_drives *drives;
            int i;
            struct walk_slot *slot;
            drives = job->work.drives;
#ifdef NONMATCHING
            /* this side breaks without drives, which the loop below reads */
            if (!drives) {
                break;
            }
#endif
            for (i = 0; i < CARD_DRIVES; i++) {
                slot = &drives->slots[i];
                if (slot->read) {
                    syFree(slot->read);
                    slot->read = NULL;
                }
                if (slot->buffer) {
                    syFree(slot->buffer);
                    slot->buffer = NULL;
                }
            }
            if (drives->items) {
                syFree(drives->items);
                drives->items = NULL;
            }
        }
        }
        garden_walk_menu->state = MENU_CLOSE;
        return;
    }
    switch (garden_walk_menu->state) {
    case MENU_OPEN: {
        struct walk_drives *drives;
        int i;
        struct walk_slot *slot;
        drives = job->work.drives;
        /* @bug memory: writes through a null job->work.drives where
         * open_walk_menu failed to allocate them, or on the receive screen
         * their items (MOV.L R2,@(4,R3) at 0x0c51c540). open_walk_menu opens
         * the menu all the same (BT/S at 0x0c51d84e, 0x0c51d902 and
         * 0x0c51d914) */
#ifdef NONMATCHING
        /* this side repairs the bug tagged above. where open_walk_menu made no
         * drives, it fails the job and closes the menu */
        if (!drives) {
            job->result = GARDEN_TRANSFER_FAILED;
            garden_walk_menu->mode = WALK_MENU_CLOSING;
            return;
        }
#endif
        for (i = 0; i < CARD_DRIVES; i++) {
            slot = &drives->slots[i];
            slot->read = NULL;
            slot->state = SLOT_EMPTY;
            slot->buffer = NULL;
            if (garden_walk_menu->send == SEND_SCREEN) {
                create_drive_cell(task, i, slot);
            }
        }
        drives->cursor = 0;
    }
        walk_create_title(task, TITLE_DELAY);
        walk_create_message_bar(task, TITLE_DELAY);
        if (garden_walk_menu->send == SEND_SCREEN) {
            int i;
            walk_create_send_frame(task, FRAME_DELAY);
            for (i = 0; i < WALK_DRIVE_CELLS; i++) {
                walk_create_cell_highlight(task, i, WALK_DRIVE_CELLS);
            }
            garden_walk_menu->selected = NO_DRIVE_PICKED;
        } else {
            int i;
            struct walk_drives *drives;
            for (i = 0; i < WALK_CONTENT_CELLS; i++) {
                walk_create_cell_highlight(task, i, WALK_CONTENT_CELLS);
                create_content_cell(task, i);
            }
            walk_create_receive_button(task);
            walk_create_receive_frame(task, FRAME_DELAY);
            drives = job->work.drives;
            garden_walk_menu->selected = drives->cursor;
            walk_create_page_arrows(task);
        }
#ifdef NONMATCHING
        {
            int i;
            int needed =
                garden_walk_menu->send ? WALK_DRIVE_CELLS : WALK_CONTENT_CELLS;
            for (i = 0; i < needed; i++) {
                if (!garden_walk_menu->cells[i]) {
                    /* the menu reads each cell's task without testing it.
                     * this side closes the menu where one is
                     * missing */
                    garden_walk_menu->mode = WALK_MENU_CLOSING;
                    job->result = GARDEN_TRANSFER_FAILED;
                    return;
                }
            }
        }
#endif
        job->result = GARDEN_TRANSFER_IDLE;
        job->transfer->status = GARDEN_TRANSFER_IDLE;
        garden_walk_menu->busy = FALSE;
        garden_walk_menu->state = MENU_ENTER;
        garden_walk_menu->timer = MENU_ENTER_FRAMES;
        break;
    case MENU_ENTER:
        if (garden_walk_menu->timer > 0) {
            garden_walk_menu->timer--;
        } else if (!garden_walk_menu->busy) {
            if (!garden_walk_menu->send) {
                walk_create_cursor(task, FRAME_DELAY, WALK_CURSOR_CONTENTS);
                garden_walk_message(22);
                garden_walk_menu->state = MENU_PICK_CONTENTS;
                walk_create_drive_page(task, &garden_walk_menu->selected);
            } else {
                walk_create_cursor(task, FRAME_DELAY, WALK_CURSOR_DRIVES);
                garden_walk_menu->state = MENU_PICK_CARD;
                garden_walk_message(6);
            }
        }
        garden_walk_menu->busy = FALSE;
        job->result = GARDEN_TRANSFER_IDLE;
        job->transfer->status = GARDEN_TRANSFER_IDLE;
        break;
    case MENU_PICK_CONTENTS: {
        struct walk_drives *drives;
        drives = job->work.drives;
        SCAN_MENU_DRIVES(drives);
        garden_walk_menu->selected = drives->cursor;
        if (garden_walk_menu->timer > 0) {
            garden_walk_menu->timer--;
            return;
        }
        if (!garden_walk_menu->busy) {
            drives = job->work.drives;
            if (!wait_for_save_card()) {
                walk_set_menu_busy(TRUE);
                return;
            }
            if ((player_pads[0]->press & PDD_DGT_TR) &&
                drives->cursor < CARD_DRIVES - 1) {
                PLAY_WALK_SOUND(WALK_SOUND_PAGE);
                walk_set_menu_busy(TRUE);
                garden_walk_menu->timer = PAGE_TURN_FRAMES;
                drives->cursor++;
                return;
            }
            if ((player_pads[0]->press & PDD_DGT_TL) && drives->cursor > 0) {
                PLAY_WALK_SOUND(WALK_SOUND_PAGE);
                walk_set_menu_busy(TRUE);
                drives->cursor--;
                garden_walk_menu->timer = PAGE_TURN_FRAMES;
                return;
            }
            {
                struct walk_content_cell *cell;
                Bool quiet;
                cell = garden_walk_menu->cells[WALK_CHAO_CELL]->extra.p;
                if (cell->flags & CONTENT_LEGACY) {
                    quiet = TRUE;
                } else {
                    quiet = FALSE;
                }
                WALK_CONTENT_MOVE(PDD_DGT_KU, garden_walk_menu->row > 0,
                                  garden_walk_menu->row--);
                WALK_CONTENT_MOVE(PDD_DGT_KD,
                                  garden_walk_menu->row < WALK_GRID_ROWS,
                                  garden_walk_menu->row++);
                WALK_CONTENT_MOVE(PDD_DGT_KL, garden_walk_menu->column > 0,
                                  garden_walk_menu->column--);
                WALK_CONTENT_MOVE(PDD_DGT_KR,
                                  garden_walk_menu->column <
                                      WALK_CONTENT_COLUMNS - 1,
                                  garden_walk_menu->column++);
            }
            if (pad_press_repeat[0] & PDD_DGT_TB) {
                int index;
                index = garden_walk_menu->cursor;
                if (index >= 0) {
                    struct task *selected;
                    struct walk_content_cell *cell;
                    struct walk_cell_work *cell_work;
                    selected = garden_walk_menu->cells[index];
                    cell = selected->extra.p;
                    if (cell->flags & CONTENT_AVAILABLE) {
                        cell_work = CELL_WORK(selected);
                        if (cell_work->flags & CELL_CHECKED) {
                            PLAY_WALK_SOUND(WALK_SOUND_BACK);
                            garden_walk_change_message(22);
                            cell_work->flags &= ~CELL_CHECKED;
                            return;
                        } else {
                            garden_walk_menu->mode = WALK_MENU_CLOSING;
                            job->result = GARDEN_TRANSFER_CANCELLED;
                            return;
                        }
                    } else {
                        garden_walk_menu->mode = WALK_MENU_CLOSING;
                        job->result = GARDEN_TRANSFER_CANCELLED;
                        return;
                    }
                } else {
                    garden_walk_menu->column = 0;
                    garden_walk_menu->row = 0;
                    garden_walk_menu->cursor = 0;
                    return;
                }
            }
            if (pad_press_repeat[0] & PDD_DGT_TA) {
                int index;
                index = garden_walk_menu->cursor;
                if (index >= 0) {
                    struct task *selected;
                    struct walk_content_cell *cell;
                    struct walk_cell_work *cell_work;
                    selected = garden_walk_menu->cells[index];
                    if (selected) {
                        cell = selected->extra.p;
                        cell_work = CELL_WORK(selected);
                        if (cell->flags & CONTENT_AVAILABLE) {
                            if (cell_work->flags & CELL_CHECKED) {
                                PLAY_WALK_SOUND(WALK_SOUND_BACK);
                                cell_work->flags &= ~CELL_CHECKED;
                            } else {
                                PLAY_WALK_SOUND(WALK_SOUND_SELECT);
                                cell_work->flags |= CELL_CHECKED;
                            }
                        }
                    }
                } else {
                    int result;
                    result = GARDEN_TRANSFER_CANCELLED;
                    {
                        /* @hack type: i a Sint8, where an int would do */
                        Sint8 i;
                        struct walk_cell_work *cell_work;
                        for (i = 0; i < (Sint8)WALK_CONTENT_CELLS; i++) {
                            cell_work = CELL_WORK(garden_walk_menu->cells[i]);
                            if (cell_work->flags & CELL_CHECKED) {
                                PLAY_WALK_SOUND(WALK_SOUND_SELECT);
                                result = GARDEN_TRANSFER_DONE;
                                break;
                            }
                        }
                    }
                    if (result != GARDEN_TRANSFER_DONE) {
                        job->result = result;
                        garden_walk_menu->mode = WALK_MENU_CLOSING;
                        return;
                    }
                    {
                        struct walk_drives *picked;
                        picked = job->work.drives;
                        job->result =
                            garden_select_walk_drive(picked, picked->cursor);
                    }
                    switch (job->result) {
                    default:
                        return;
                    case GARDEN_TRANSFER_DONE: {
                        struct walk_cell_work *cell_work;
                        struct walk_content_cell *cell;
                        struct walk_drives *picked;
                        struct walk_install *install;
                        Uint8 *items;
                        int count;
                        int remain;
                        int i;
                        picked = job->work.drives;
                        items = picked->items;
                        cell = garden_walk_menu->cells[WALK_CHAO_CELL]->extra.p;
                        if (cell->flags & CONTENT_LEGACY) {
                            cell_work = CELL_WORK(
                                garden_walk_menu->cells[WALK_CHAO_CELL]);
                            if (cell_work->flags & CELL_CHECKED) {
                                job->transfer->chao_returned = 0;
                                job->transfer->egg_received = 1;
                                garden_walk_message(29);
                                garden_walk_menu->timer = LEGACY_COLLECT_FRAMES;
                                garden_walk_menu->state = MENU_COLLECT_CONTENTS;
                                job->transfer->status = GARDEN_TRANSFER_BUSY;
                                job->result = GARDEN_TRANSFER_BUSY;
                                if (picked->items) {
                                    syFree(picked->items);
                                    picked->items = NULL;
                                }
                            }
                            return;
                        }
                        install = job->work.install =
                            syMalloc(sizeof(*install));
                        if (install) {
                            cell_work = CELL_WORK(
                                garden_walk_menu->cells[WALK_CHAO_CELL]);
                            if (cell_work->flags & CELL_CHECKED) {
                                install->chao = syCalloc(WALK_CHAO_BLOCKS,
                                                         CARD_BLOCK_BYTES);
                                job->transfer->chao_returned = 1;
                            } else {
                                install->chao = NULL;
                                job->transfer->chao_returned = 0;
                            }
                            cell_work = CELL_WORK(
                                garden_walk_menu->cells[WALK_EGG_CELL]);
                            if (cell_work->flags & CELL_CHECKED) {
                                install->egg = syCalloc(WALK_CHAO_BLOCKS,
                                                        CARD_BLOCK_BYTES);
                                job->transfer->egg_received = 1;
                            } else {
                                install->egg = NULL;
                                job->transfer->egg_received = 0;
                            }
#ifdef NONMATCHING
                            if ((job->transfer->chao_returned &&
                                 !install->chao) ||
                                (job->transfer->egg_received &&
                                 !install->egg)) {
                                /* the install writes these zeroed records
                                 * over the Chao and the egg the garden takes.
                                 * without one, the card would keep what the
                                 * garden took, so this side takes
                                 * nothing */
                                if (install->chao) {
                                    syFree(install->chao);
                                }
                                if (install->egg) {
                                    syFree(install->egg);
                                }
                                syFree(install);
                                goto failed_pick;
                            }
#endif
                            job->transfer->item_count = 0;
                            count = items[WALK_ITEM_COUNT_OFFSET];
#ifdef NONMATCHING
                            /* this side takes no more items than
                             * WALK_MAX_ITEMS */
                            if (count > WALK_MAX_ITEMS) {
                                count = WALK_MAX_ITEMS;
                            }
#endif
                            remain = 0;
                            for (i = 0; i < count; i++) {
                                cell_work =
                                    CELL_WORK(garden_walk_menu
                                                  ->cells[i + WALK_ITEM_CELL]);
                                if (cell_work->flags & CELL_CHECKED) {
                                    job->transfer
                                        ->items[job->transfer->item_count++] =
                                        items[i + WALK_ITEM_IDS_OFFSET];
                                } else {
                                    items[remain++ + WALK_ITEM_IDS_OFFSET] =
                                        items[i + WALK_ITEM_IDS_OFFSET];
                                }
                            }
                            items[WALK_ITEM_COUNT_OFFSET] = remain;
                            install->items = items;
                            install->state = INSTALL_FIND;
                            install->drive = picked->cursor;
                            job->transfer->status = GARDEN_TRANSFER_BUSY;
                            job->transfer->drive = picked->cursor;
                            job->transfer->phase =
                                GARDEN_TRANSFER_PHASE_INSTALLING;
                            garden_walk_menu->state = MENU_COLLECT_CONTENTS;
                            garden_walk_menu->mode = WALK_MENU_FREEZE;
                            garden_walk_message(1);
                            syFree(picked);
                            return;
                        } else {
#ifdef NONMATCHING
                        /* this side's failed pick above frees the items from
                         * here */
                        failed_pick:
#endif
                            if (picked->items) {
                                syFree(picked->items);
                                picked->items = NULL;
                            }
                            syFree(picked);
                            job->work.drives = NULL;
                            job->result = GARDEN_TRANSFER_FAILED;
                            garden_walk_menu->mode = WALK_MENU_CLOSING;
                            garden_walk_message(NO_MESSAGE);
                            return;
                        }
                    }
#ifdef NONMATCHING
#else
                    /* @hack redundant: a break no path reaches */
                    break;
#endif
                    case GARDEN_TRANSFER_CANCELLED:
                    case GARDEN_TRANSFER_FAILED: {
                        struct walk_drives *picked;
                        picked = job->work.drives;
                        if (picked->items) {
                            syFree(picked->items);
#ifdef NONMATCHING
                            /* this side forgets the items it frees */
                            picked->items = NULL;
#endif
                        }
                        garden_walk_menu->mode = WALK_MENU_CLOSING;
                        return;
                    }
                    }
                }
            }
            if (garden_walk_menu->row < WALK_GRID_ROWS) {
                garden_walk_menu->cursor =
                    (garden_walk_menu->column * WALK_GRID_ROWS) +
                    garden_walk_menu->row;
            } else {
                garden_walk_menu->cursor = CONFIRM_CURSOR;
            }
        }
        garden_walk_menu->busy = FALSE;
        job->result = GARDEN_TRANSFER_IDLE;
        job->transfer->status = GARDEN_TRANSFER_IDLE;
    } break;
    case MENU_COLLECT_CONTENTS:
        if (garden_walk_menu->timer > 0) {
            struct walk_content_cell *cell;
            garden_walk_menu->timer--;
            cell = garden_walk_menu->cells[WALK_CHAO_CELL]->extra.p;
            if (!garden_walk_menu->timer && (cell->flags & CONTENT_LEGACY)) {
                job->result = GARDEN_TRANSFER_DONE;
                garden_walk_menu->timer = RESULT_FRAMES;
                garden_walk_menu->state = MENU_RESULT;
                return;
#ifdef NONMATCHING
#else
                /* @hack redundant: a break no path reaches */
                break;
#endif
            }
            break;
        }
        {
            struct walk_install *install;
            install = job->work.install;
            job->result = garden_install_walk_game(install);
            if (job->result == GARDEN_TRANSFER_BUSY) {
                break;
            }
            switch (job->result) {
            case GARDEN_TRANSFER_DONE:
                garden_walk_message(5);
                /* fall through */
            case GARDEN_TRANSFER_CANCELLED:
            case GARDEN_TRANSFER_FAILED:
                if (install->egg) {
                    syFree(install->egg);
                }
                if (install->chao) {
                    syFree(install->chao);
                }
                if (install->items) {
                    syFree(install->items);
                }
                syFree(install);
                job->work.install = NULL;
                garden_walk_menu->timer = RESULT_FRAMES;
                garden_walk_menu->state = MENU_RESULT;
                break;
            }
        }
        break;
    case MENU_PICK_CARD:
        if (garden_walk_menu->timer > 0) {
            garden_walk_menu->timer--;
            return;
        }
        {
            struct walk_drives *drives;
            drives = job->work.drives;
            SCAN_MENU_DRIVES(drives);
            if (!garden_walk_menu->busy) {
                if (!wait_for_save_card()) {
                    break;
                }
                if (pad_press_repeat[0] & PDD_DGT_TB) {
                    garden_walk_menu->mode = WALK_MENU_CLOSING;
                    job->result = GARDEN_TRANSFER_CANCELLED;
                    return;
                }
                if (pad_press_repeat[0] & PDD_DGT_TA) {
#ifdef NONMATCHING
#else
                    /* @hack redundant: cell, the drive cell's record, which
                     * nothing reads */
                    /* NOLINTBEGIN(clang-diagnostic-unused-but-set-variable) */
                    struct walk_drive_cell *cell;
                    /* NOLINTEND(clang-diagnostic-unused-but-set-variable) */
#endif
                    struct walk_cell_work *cell_work;
                    /* @hack type: index a Sint8, where an int would do */
                    Sint8 index;
                    index = garden_walk_menu->cursor;
#ifdef NONMATCHING
#else
                    cell = garden_walk_menu->cells[index]->extra.p;
#endif
                    cell_work = CELL_WORK(garden_walk_menu->cells[index]);
                    if (cell_work->flags & CELL_FRAME) {
                        cell_work->flags |= CELL_CHECKED;
                        if (cell_work->flags & CELL_CHAO) {
                            drives->cursor = index;
                            garden_walk_menu->mode = WALK_MENU_CLOSING;
                            job->result = GARDEN_TRANSFER_CANCELLED;
                            /* a wait nothing reads, since the menu closes */
                            garden_walk_menu->timer = 60;
                            return;
#ifdef NONMATCHING
#else
                            /* @hack redundant: a break no path reaches */
                            break;
#endif
                        } else {
                            int drive;
                            struct walk_slot *slot;
                            Uint8 no_send;
                            drive = index;
                            slot = &drives->slots[drive];
                            garden_walk_menu->selected = index;
                            drives->cursor = index;
                            if (slot->read && slot->read->items) {
                                no_send =
                                    slot->read->items[WALK_NO_SEND_OFFSET];
                                if (no_send == 1) {
                                    garden_walk_queue_message(16);
                                    garden_walk_menu->state = MENU_CHOOSE_CARD;
                                } else {
                                    garden_walk_menu->state = MENU_PREPARE_CARD;
                                }
                            } else {
                                garden_walk_menu->state = MENU_PREPARE_CARD;
                            }
                            PLAY_WALK_SOUND(WALK_SOUND_SELECT);
                            return;
                        }
                    }
                }
                WALK_DRIVE_MOVE(PDD_DGT_KU, garden_walk_menu->row > 0,
                                garden_walk_menu->row--);
                WALK_DRIVE_MOVE(PDD_DGT_KD,
                                garden_walk_menu->row < WALK_GRID_ROWS - 1,
                                garden_walk_menu->row++);
                WALK_DRIVE_MOVE(PDD_DGT_KL, garden_walk_menu->column > 0,
                                garden_walk_menu->column--);
                WALK_DRIVE_MOVE(PDD_DGT_KR,
                                garden_walk_menu->column <
                                    WALK_DRIVE_COLUMNS - 1,
                                garden_walk_menu->column++);
                garden_walk_menu->cursor =
                    (garden_walk_menu->column * WALK_GRID_ROWS) +
                    garden_walk_menu->row;
            }
        }
        job->result = GARDEN_TRANSFER_IDLE;
        job->transfer->status = GARDEN_TRANSFER_IDLE;
        garden_walk_menu->busy = FALSE;
        break;
    case MENU_CHOOSE_CARD:
        if (garden_walk_menu->timer > 0) {
            garden_walk_menu->timer--;
            return;
        }
        if (!garden_walk_menu->busy) {
            struct walk_drives *drives;
            int index;
#ifdef NONMATCHING
#else
            /* NOLINTBEGIN(clang-diagnostic-unused-but-set-variable) */
            struct walk_drive_cell *cell;
            struct walk_slot *slot;
            /* NOLINTEND(clang-diagnostic-unused-but-set-variable) */
#endif
            struct walk_cell_work *cell_work;
            drives = job->work.drives;
            index = garden_walk_menu->selected;
#ifdef NONMATCHING
#else
            /* @hack redundant: cell and slot, the picked drive's records,
             * which nothing reads */
            cell = garden_walk_menu->cells[index]->extra.p;
            slot = &drives->slots[index];
#endif
            garden_update_walk_drives(drives);
            if (!wait_for_save_card()) {
                break;
            }
            if (pad_press_repeat[0] & PDD_DGT_TB) {
                cell_work = CELL_WORK(garden_walk_menu->cells[index]);
                cell_work->flags &= ~CELL_CHECKED;
                garden_walk_menu->mode = 2;
                garden_walk_menu->cursor = garden_walk_menu->selected;
                garden_walk_menu->selected = NO_DRIVE_PICKED;
                garden_walk_message(6);
                garden_walk_menu->state = MENU_PICK_CARD;
#ifdef NONMATCHING
                /* the plain form the hack tag names */
                PLAY_WALK_SOUND(WALK_SOUND_BACK);
#else
                /* @hack type: sound_play called through a cast to its own
                 * type, where a plain call would do */
                ((Sint8 (*)(int, const void *, Sint8, Sint16))sound_play)(
                    WALK_SOUND_BACK, NULL, 0, 0);
#endif
                return;
            }
            if (pad_press_repeat[0] & PDD_DGT_TA) {
                garden_walk_menu->state = MENU_PICK_CARD;
                return;
            }
        }
        garden_walk_menu->busy = FALSE;
        /* @hack spelling: a goto to the step's end, where break would do */
        goto end_step;
    case MENU_PREPARE_CARD: {
        int result;
        int index;
        struct walk_drives *drives;
        struct walk_drives *picked;
        Uint8 *items;
        struct walk_install *install;
        index = garden_walk_menu->selected;
        drives = job->work.drives;
        drives->cursor = index;
        /* @bug memory: a second select reads a null slot->read.
         * garden_select_walk_drive frees each slot's read but keeps its
         * state. MENU_PREPARE_CARD tests the save card only after the select
         * (BSR at 0x0c51d30a). where the card is out, MENU_WAIT_CARD later
         * returns here, and the select runs again (MOV.L @(8,R2),R3 at
         * 0x0c51b052 and MOV.L @(12,R2),R3 at 0x0c51b0e6) */
#ifdef NONMATCHING
        /* this side repairs the bug tagged above: the select waits for the
         * save's card */
        if (!wait_for_save_card()) {
            break;
        }
#endif
        result = garden_select_walk_drive(drives, index);
        job->result = result;
        garden_walk_message(1);
        /* @hack spelling: an else after the break, where the code after the
         * test would do */
        if (!wait_for_save_card()) {
            break;
        } else {
            picked = job->work.drives;
            items = picked->items;
            switch (job->result) {
            case GARDEN_TRANSFER_DONE:
                if (picked->result) {
#ifdef NONMATCHING
#else
                    /* @hack redundant: a memset of no bytes */
                    memset(items, 0, 0);
#endif
                    memset(items + WALK_ITEM_HEAD_BYTES, 0,
                           CARD_BLOCK_BYTES - WALK_ITEM_HEAD_BYTES);
                } else {
                    syFree(items);
                    items = NULL;
                }
                install = job->work.install = syMalloc(sizeof(*install));
                if (install) {
                    install->state = INSTALL_FIND;
                    install->drive = picked->cursor;
                    install->chao = job->transfer->chao;
                    install->egg = job->transfer->egg;
                    install->items = items;
                    job->transfer->status = GARDEN_TRANSFER_BUSY;
                    job->result = GARDEN_TRANSFER_BUSY;
                    job->transfer->drive = picked->cursor;
                    garden_walk_menu->mode = WALK_MENU_FREEZE;
                    garden_walk_menu->state = MENU_WRITE_CARD;
                    job->transfer->phase = GARDEN_TRANSFER_PHASE_INSTALLING;
                } else {
                    if (items) {
                        syFree(items);
                    }
                    job->result = GARDEN_TRANSFER_FAILED;
                    garden_walk_menu->mode = WALK_MENU_CLOSING;
                }
                syFree(picked);
                break;
            case WALK_SELECT_LEGACY:
                if (items) {
                    syFree(items);
                }
                syFree(job->work.drives);
                job->work.drives = NULL;
                garden_walk_menu->timer = RESULT_FRAMES;
                garden_walk_menu->state = MENU_RESULT;
                garden_walk_message(WALK_NOTICE_OTHER_GAME);
                job->result = GARDEN_TRANSFER_FAILED;
                break;
            case GARDEN_TRANSFER_CANCELLED:
            case GARDEN_TRANSFER_FAILED:
                if (items) {
                    syFree(items);
                }
                syFree(job->work.drives);
                job->work.drives = NULL;
                garden_walk_menu->mode = WALK_MENU_CLOSING;
                break;
            }
        }
        garden_walk_menu->busy = FALSE;
    } break;
    case MENU_WRITE_CARD: {
        int result;
        struct walk_install *install;
        struct walk_install *finished;
        install = job->work.install;
        result = garden_install_walk_game(install);
        job->result = result;
        if (result != GARDEN_TRANSFER_BUSY) {
            finished = job->work.install;
            switch (result) {
            case GARDEN_TRANSFER_DONE:
                if (job->transfer->egg) {
                    syFree(job->transfer->egg);
                    job->transfer->egg = NULL;
                }
                job->transfer->phase = GARDEN_TRANSFER_PHASE_INSTALLED;
                if (garden_walk_menu->drive < 0) {
                    garden_walk_message(5);
                } else {
                    garden_walk_menu->state = MENU_START_SAVE;
                    garden_walk_menu->timer = SAVE_DELAY_FRAMES;
                    job->result = GARDEN_TRANSFER_BUSY;
                }
                /* fall through */
            case GARDEN_TRANSFER_CANCELLED:
            case GARDEN_TRANSFER_FAILED:
                if (finished->items) {
                    syFree(finished->items);
                    finished->items = NULL;
                }
                syFree(finished);
                job->work.install = NULL;
            }
            if (garden_walk_menu->state != MENU_START_SAVE) {
                garden_walk_menu->timer = RESULT_FRAMES;
                garden_walk_menu->state = MENU_RESULT;
            }
        }
    } break;
    case MENU_START_SAVE:
        if (garden_walk_menu->timer > 0) {
            garden_walk_menu->timer--;
        } else {
            garden_walk_message(2);
            garden_file_gather();
            garden_file_write();
            garden_walk_menu->timer = SAVE_WAIT_FRAMES;
            garden_walk_menu->state = MENU_WAIT_FILE;
        }
        break;
    case MENU_WAIT_FILE:
        if (garden_walk_menu->timer > 0) {
            garden_walk_menu->timer--;
        } else {
            int result;
            result = garden_file_state();
            if (result != GARDEN_FILE_BUSY) {
                switch (result) {
                case GARDEN_FILE_OK:
                    garden_walk_message(5);
                    break;
                case GARDEN_FILE_ABSENT:
                case GARDEN_FILE_ERROR:
                    garden_walk_message(WALK_NOTICE_CARD);
                }
                job->result = result;
                garden_walk_menu->timer = RESULT_FRAMES;
                garden_walk_menu->state = MENU_RESULT;
            }
        }
        break;
    case MENU_WAIT_CARD:
        /* @hack spelling: the result compared with TRUE, where a test of it
         * would do */
        if (wait_for_save_card() == TRUE) {
            Sint8 message;
            switch (garden_walk_menu->previous) {
            case MENU_PICK_CONTENTS:
                message = 22;
                break;
            case MENU_PICK_CARD:
                message = 6;
                break;
            case MENU_CHOOSE_CARD:
                message = 16;
                break;
            default:
                message = NO_MESSAGE;
            }
            garden_walk_message(message);
            garden_walk_menu->state = garden_walk_menu->previous;
            garden_walk_menu->timer = RESUME_FRAMES;
            return;
        }
        if (pad_press_repeat[0] & PDD_DGT_TB) {
            garden_walk_menu->mode = WALK_MENU_CLOSING;
            job->result = GARDEN_TRANSFER_CANCELLED;
        }
        break;
    case MENU_RESULT:
        if (garden_walk_menu->timer > 0) {
            garden_walk_menu->timer--;
        } else if (!garden_walk_menu->bar) {
            garden_walk_menu->mode = WALK_MENU_CLOSING;
        }
        break;
    case MENU_CLOSE:
        if (garden_walk_menu->busy == BUSY_CLOSING) {
            task_kill(task);
            return;
        }
        garden_walk_menu->busy = BUSY_CLOSING;
        break;
    }
end_step:;
}
void garden_walk_menu_draw(struct task *task)
{
}
void garden_walk_menu_destroy(struct task *task)
{
    struct walk_job *job;
    task_destroy_children(task);
#ifdef NONMATCHING
    /* this side reads no job where the menu is already gone */
    if (garden_walk_menu == NULL) {
        return;
    }
#endif
    job = garden_walk_menu->job;
    job->transfer->status = job->result;
    if (job->work.drives) {
        syFree(job->work.drives);
        job->work.drives = NULL;
    }
    syFree(garden_walk_menu->job);
    garden_walk_menu->job = NULL;
    task->extra.p = NULL;
    if (task->work) {
        syFree(task->work);
        task->work = NULL;
    }
    syFree(garden_walk_menu);
    task->motion.p = NULL;
    garden_walk_menu = NULL;
    pause_enable();
}

/* open the walk menu over transfer, on the screen send_mode names. for the
 * send screen, transfer must hold a Chao or an egg but not both. for the
 * receive screen it must hold both. otherwise the menu opens without drives,
 * and MENU_OPEN writes through their null. the menu's task keeps
 * garden_walk_menu in its motion, and the walk_job in its extra */
static struct task *open_walk_menu(Sint8 send_mode,
                                   struct garden_transfer *transfer)
{
    struct task *menu;
    struct walk_menu *manager;
    struct walk_job *job;
    struct walk_job *source;
    struct walk_drives *drives;
    struct garden_transfer *request;
    struct garden_vms_chao *chao;
    if (garden_walk_menu || !garden_walk_texture_list) {
        return NULL;
    }
    menu =
        task_create(TASK_WORK, 2, garden_walk_menu_step, walk_menu_task_name);
    if (!menu) {
        return NULL;
    }
    menu->draw = garden_walk_menu_draw;
    menu->destructor = garden_walk_menu_destroy;
    manager = syCalloc(1, sizeof(*manager));
    if (!manager) {
        task_destroy(menu);
        return NULL;
    }
    menu->motion.p = manager;
    job = syCalloc(1, sizeof(*job));
    if (!job) {
        task_destroy(menu);
        return NULL;
    }
    menu->extra.p = job;
#ifdef NONMATCHING
#else
    /* @hack redundant: a task_destroy that never runs */
    if (0) {
        task_destroy(menu);
    }
#endif
    garden_walk_menu = manager;
    garden_walk_menu->send = send_mode;
    garden_walk_menu->job = job;
    garden_walk_menu->drive = card_save_drive();
    job->transfer = transfer;
    job->work.drives = NULL;
    if (send_mode == SEND_SCREEN) {
        source = garden_walk_menu->job;
        source->transfer->status = GARDEN_TRANSFER_FAILED;
        request = source->transfer;
        chao = request->chao;
        if ((chao && !request->egg) || (!chao && request->egg)) {
            if ((drives = source->work.drives = syMalloc(sizeof(*drives))) !=
                NULL) {
                if ((drives->items = syMalloc(CARD_BLOCK_BYTES)) != NULL) {
                    drives->mode = DRIVE_HIDDEN;
                    drives->request =
                        source->transfer->chao ? WALK_SEND_CHAO : WALK_SEND_EGG;
                    drives->chao = NULL;
                    drives->egg = NULL;
                    source->transfer->status = GARDEN_TRANSFER_IDLE;
                    source->result = GARDEN_TRANSFER_IDLE;
                } else {
                    syFree(source->work.drives);
                    source->work.drives = NULL;
                    task_destroy(menu);
                    return NULL;
                }
            }
        }
        source->transfer->phase = GARDEN_TRANSFER_PHASE_MENU;
    } else {
        source = garden_walk_menu->job;
        source->transfer->status = GARDEN_TRANSFER_FAILED;
        if (source->transfer->chao && source->transfer->egg) {
            if ((drives = source->work.drives = syMalloc(sizeof(*drives))) !=
                NULL) {
                if ((drives->items = syMalloc(CARD_BLOCK_BYTES)) != NULL) {
                    drives->mode = DRIVE_HIDDEN;
                    drives->request = WALK_COLLECT;
                    drives->chao = source->transfer->chao;
                    drives->egg = source->transfer->egg;
                    source->transfer->status = GARDEN_TRANSFER_IDLE;
                    source->state = WALK_SCAN_RECEIVE;
                    source->result = GARDEN_TRANSFER_IDLE;
                } else {
                    syFree(source->work.drives);
                    source->work.drives = NULL;
                }
            }
            source->transfer->phase = GARDEN_TRANSFER_PHASE_MENU;
        }
    }
    pause_disable();
    return menu;
}
static void destroy_walk_job(struct task *task)
{
    if (task->extra.p) {
        syFree(task->extra.p);
        task->extra.p = NULL;
    }
}
static void draw_walk_job(struct task *task)
{
    struct walk_job *job = task->extra.p;
    switch (job->state) {
    case WALK_SCAN_SEND:
    case WALK_SCAN_RECEIVE:
        garden_draw_walk_drives(job->work.drives);
        break;
    }
}
static void step_walk_job(struct task *task)
{
    struct walk_job *job;
    struct walk_install *install;
    struct walk_read *read;
    job = task->extra.p;
    if (!task->draw || !task->destructor) {
        task->draw = draw_walk_job;
        task->destructor = destroy_walk_job;
    }
    switch (job->state) {
    case WALK_INSTALL:
        if ((install = job->work.install = syMalloc(sizeof(*install))) !=
            NULL) {
            install->state = INSTALL_FIND;
            install->drive = job->transfer->drive;
            install->chao = job->transfer->chao;
            install->egg = job->transfer->egg;
            install->items = NULL;
            job->transfer->status = GARDEN_TRANSFER_BUSY;
            job->state = WALK_INSTALLING;
        } else {
            job->transfer->status = GARDEN_TRANSFER_FAILED;
        }
        break;
    case WALK_INSTALLING:
        switch (job->transfer->status =
                    garden_install_walk_game(job->work.install)) {
        case GARDEN_TRANSFER_DONE:
        case GARDEN_TRANSFER_CANCELLED:
        case GARDEN_TRANSFER_FAILED:
            if (job->work.install->items) {
                syFree(job->work.install->items);
            }
            syFree(job->work.install);
            job->work.install = NULL;
            break;
        }
        break;
    case WALK_READ:
        if ((read = job->work.read = syMalloc(sizeof(*read))) != NULL) {
            read->state = READ_FIND;
            read->drive = job->transfer->drive;
            read->chao = job->transfer->chao;
            read->egg = job->transfer->egg;
            read->items = NULL;
            job->transfer->status = GARDEN_TRANSFER_BUSY;
            job->state = WALK_READING;
        } else {
            job->transfer->status = GARDEN_TRANSFER_FAILED;
        }
        break;
    case WALK_READING:
        switch (job->transfer->status = garden_read_walk_file(job->work.read)) {
        case GARDEN_TRANSFER_DONE:
        case GARDEN_TRANSFER_CANCELLED:
        case GARDEN_TRANSFER_FAILED:
            syFree(job->work.read);
            job->work.read = NULL;
            break;
        }
        break;
    }
    switch (job->transfer->status) {
    case GARDEN_TRANSFER_DONE:
    case GARDEN_TRANSFER_CANCELLED:
    case GARDEN_TRANSFER_FAILED:
        job->state = WALK_FINISHED;
        task_kill(task);
        break;
    }
}
static struct task *start_walk_job(int state, struct garden_transfer *transfer)
{
    struct task *task;
    struct walk_job *job;
    task = task_create(0, 2, step_walk_job, walk_job_task_name);
    if (task) {
        job = syMalloc(sizeof(*job));
        if (!job) {
            task_destroy(task);
            task = NULL;
        } else {
            task->extra.p = job;
            job->state = state;
            job->transfer = transfer;
            job->work.drives = NULL;
        }
    }
    return task;
}
int garden_write_vms_chao(struct garden_transfer *transfer)
{
    if (start_walk_job(WALK_INSTALL, transfer)) {
        return 0;
    }
    return -1;
}
int garden_read_vms_chao(struct garden_transfer *transfer)
{
    if (start_walk_job(WALK_READ, transfer)) {
        return 0;
    }
    return -1;
}
int garden_open_vmu_send(struct garden_transfer *transfer)
{
    if (open_walk_menu(SEND_SCREEN, transfer)) {
        return 0;
    }
    return -1;
}
int garden_open_vmu_receive(struct garden_transfer *transfer)
{
    if (open_walk_menu(RECEIVE_SCREEN, transfer)) {
        return 0;
    }
    return -1;
}
