/* reads from the disc, and the head of the overlay in the stage slot. the
 * reads take whole files, PRS files, a stage's overlay and its camera files */
#ifndef SYSTEM_LOADER_H
#define SYSTEM_LOADER_H
#include <sg_xpt.h>
#include <shinobi.h>

#include "system/areas.h"

/* the sectors that size bytes take, a last sector in part counted whole */
#define SECTORS(size) (((size) + (GDD_FS_SCTSIZE - 1)) >> SCT_SHIFT)

struct task;

/* the head of a stage's packed camera file, stg<stage>cam.prs, decoded. the
 * file's parts follow the head, end to end. they are a camset file and a
 * camlin file, then the two a demo takes in their place */
#define CAMERA_PARTS 4
struct camera_file {
    /* the count includes this header and the declared parts */
    Sint32 count;
    Sint32 sizes[CAMERA_PARTS]; /* each part's bytes */
    Sint32 unk_14[3];
    Uint8 parts[1];
};

/* where loader_read_camera_files decodes the packed camera file: the second
 * megabyte of WORK_AREA */
#ifdef NONMATCHING
/* the hack's plain form: this side's objects don't define camera_file, which
 * only symbols.txt places */
#define CAMERA_FILE ((struct camera_file *)WORK_AREA_FILE)
#else
/* @hack type: the file as an object that no source defines, where a cast of
 * WORK_AREA_FILE would do. symbols.txt places it at WORK_AREA_FILE */
extern struct camera_file camera_file;
#define CAMERA_FILE (&camera_file)
#endif

/* clear the five camera areas, and read the current stage's camera files into
 * them. it calls camset_free_marked last. below, a demo means game_state.demo
 * is DEMO_ON or DEMO_ON_NO_FILE.
 *
 * where stg<stage>cam.prs opens, it decodes into CAMERA_FILE. the file's first
 * part goes to CAMSET_AREA, and its second to both camlin areas. a demo takes
 * the third and fourth parts in their place, where the file has all
 * CAMERA_PARTS. no part of size 0 is copied, and none once the head's count is
 * 0. CAMSET_AREA_0 and CAMSET_AREA_1 stay clear.
 *
 * where it won't open, each area reads a file of its own in whole sectors. an
 * area whose file won't open stays clear. CAMSET_AREA takes camset<stage>.bin,
 * and CAMLIN_AREA_0 camlin<stage>.bin. CAMSET_AREA_0 takes
 * camset<stage><c>.bin, where c is the letter of stage_character(0). while slot
 * 1 holds a player, CAMSET_AREA_1 takes the same for stage_character(1), and
 * CAMLIN_AREA_1 takes camlin<stage>.bin. each name gives the stage in four
 * digits. a demo's names add a D before the dot. The port validates all
 * packed extents before copying either table. With an absent or invalid
 * archive it tries the loose files and leaves each unreadable table empty. */
void loader_read_camera_files(void);
/* where game_state.tray_opened is set and game_state.card_state isn't
 * CARD_SAVING, leave the game for the boot ROM through sbExitSystem and then
 * syBtExit. otherwise set tray_opened when gdFsIsTrayOpen returns TRUE */
void loader_poll_drive(void);
/* an error callback for the GD file system. it sets
 * game_state.tray_opened for GDD_ERR_TRAYOPEND or GDD_ERR_UNITATTENT, and
 * ignores any other error */
void loader_note_error(void *obj, Sint32 err);
#ifdef NONMATCHING
/* stop the game after a load it can't go on without has failed. call it once
 * loader_read_file_checked, loader_read_prs_checked, loader_read_prs_buffer,
 * loader_read_prs_or_file or loader_start_program has failed, with name the
 * file that load was given, or null. each of them records how it failed, and
 * port_stop shows name with that record: the words loader.c lists, and the
 * GD-ROM's error for a read, else 0. it doesn't return.
 *
 * every read fails while the drive's tray is open, and a read fails with
 * GDD_ERR_UNITATTENT after the disc changes. so where the tray is open, or
 * game_state.tray_opened is set, it draws nothing. it waits instead for the
 * vertical blank's loader_poll_drive, which leaves for the boot ROM once no
 * card save is running */
void loader_stop(const char *name) __attribute__((noreturn));
/* read the current stage's files, and admit its overlay in STAGE_SLOT before
 * any of the overlay's code runs. the files are loader_read_stage_overlay's,
 * and the Chao world's model and motion archives. a demo's file is read before
 * the overlay, and must decode to DEMO_FILE_BYTES, as each archive must to its
 * own size. a file that fails to load stops the game, as loader_stop says, and
 * so does a stage with no overlay, as NO OVERLAY */
void loader_prepare_stage(void);
/* run the init of the overlay loader_prepare_stage admitted. an init that
 * leaves the overlay no step inside it stops the game, as loader_stop says,
 * as NO STEP with no name. the screen's SLOT names the overlay */
void loader_start_stage(void);
Bool loader_start_program(const char *name, const char *tag);
Bool loader_install_chao_area(const char *name, const char *tag, void *slot,
                              Uint32 capacity);
#else
/* read the current stage's overlay into STAGE_SLOT with loader_read_prs.
 * below STAGE_BOSS_BIGFOOT the overlay is stg<stage>.prs. while
 * game_state.demo is DEMO_ON, such a stage then reads demo<stage>.prs into
 * DEMO_AREA. while it is DEMO_ON_NO_FILE, the stage reads no demo file, and
 * sets demo to DEMO_ON. from STAGE_BOSS_BIGFOOT up, only the eight
 * STAGE_BOSS_ stages, the two STAGE_CART_ stages and STAGE_CHAO have an
 * overlay. a file that won't open leaves its area as it was, and nothing
 * reports it */
void loader_read_stage_overlay(void);
#endif
#ifdef NONMATCHING
/* Read whole sectors into capacity bytes. Return the file length on success
 * or -1 on failure. A failed transfer can leave a partial sector in dest. */
Sint32 loader_read_file_checked(const char *name, void *dest, Uint32 capacity);
/* Return the decoded length, or -1 on failure. The checked form uses
 * WORK_AREA as input. The buffer form reads into the end of the supplied
 * buffer, then decodes into its beginning. It decodes from a heap copy of
 * the input only where decoding in place would write over input before
 * reading it. */
Sint32 loader_read_prs_checked(const char *name, void *dest, Uint32 capacity);
Sint32 loader_read_prs_buffer(const char *name, void *buffer, Uint32 capacity);
#endif
/* Read a file through WORK_AREA and return a Shinobi heap copy, or NULL on
 * failure. The port checks that the rounded transfer fits the scratch area. */
void *loader_read_alloc(const char *name);
void loader_free(void *memory);
/* Read a PRS file through WORK_AREA and return a Shinobi heap copy of its
 * decoded bytes, or NULL on failure. The port bounds the file and expansion
 * and copies exactly the decoded extent. The original copies whole words. */
void *loader_read_prs_alloc(const char *name);
/* read the file name into dest in whole sectors, so dest needs room for the
 * file's size rounded up to a sector. it returns -1 when the open fails, and
 * 0 otherwise */
int loader_read_file(const char *name, void *dest);
/* loader_read_file, but it returns the file's size in bytes, or 0 when the
 * open fails */
Sint32 loader_read_file_get_size(const char *name, void *dest);
/* read the PRS file name into WORK_AREA and decode it into dest. the file
 * must fit WORK_AREA in whole sectors. the decoded bytes must not reach the
 * part of the file the decoder has yet to read. neither is checked. it
 * invalidates the instruction cache, for the code dest may hold. it returns
 * -1 or 0 as loader_read_file does, not the decoded size */
int loader_read_prs(const char *name, void *dest);
/* loader_read_prs, but it returns the decoded size in bytes, or -1 when the
 * open fails */
Sint32 loader_read_prs_get_size(const char *name, void *dest);
/* a file's size in bytes, or -1 when the open or the size query fails */
Sint32 loader_file_size(const char *name);
/* read the file name into WORK_AREA, or where the disc has its PRS form, decode
 * the form there. it invalidates the instruction cache.
 *
 * the form's name is name cut at its first dot, with .prs added. a name that
 * starts with e is always read as it is. the form reads and decodes as in
 * loader_read_prs_alloc, and the result is its decoded size. the result is -1
 * where the form opens for its size but not for its read.
 *
 * for a file read as it is, the result is its size, or 0 where it won't open.
 * name must hold a dot. name and the form's name must each fit 64 bytes with
 * their NUL. neither is checked in the original. The port reads either form
 * within WORK_AREA, returns -1 on failure and flushes the cache on success. */
Sint32 loader_read_prs_or_file(const char *name);
/* the head every overlay of STAGE_SLOT opens with, up to its step. it holds the
 * name's eight bytes, then three words. head.c in the overlay's directory under
 * modules/ defines it. the game reads only the head's functions.
 * overlay_run_init and overlay_run_exit call init and exit, and program_overlay
 * and stage_overlay add the step */
struct overlay_head {
    /* the overlay's name in capitals, padded with spaces, with no NUL.
     * nothing reads it */
    char name[8];
    /* nothing reads it. a stage's head holds 0, and a program's 1 to 5 */
    Sint32 unk_08;
    void (*init)(void);
    void (*exit)(void);
};
#define OVERLAY_HEAD ((struct overlay_head *)STAGE_SLOT)

/* the head with its sixth word, the step, which differs by the kind of overlay.
 * main calls a program's step each frame while the program runs. it returns an
 * enum overlay_result of main.h. a stage's step is that of the stage's task,
 * which stage/stage.c creates as play starts. a stage's object list follows its
 * head, but nothing reads it there. the stage's init passes it to set_start by
 * name */
struct program_overlay {
    struct overlay_head head;
    int (*step)(void);
};
struct stage_overlay {
    struct overlay_head head;
    void (*step)(struct task *task);
};
#define PROGRAM_OVERLAY ((struct program_overlay *)STAGE_SLOT)
#define STAGE_OVERLAY ((struct stage_overlay *)STAGE_SLOT)

/* the head at a Chao area's base. CHAO.PRS reads only step, the step of the
 * task it creates for the area. an area that has an object list holds it
 * after the head. the area's step gives the list to set_start by name */
struct chao_area_head {
    /* the area's name in capitals, with no NUL */
    char name[8];
    void (*step)(struct task *task);
};

/* what a head.c writes after its head's name. for SHC it is empty: #pragma
 * section HEAD and the tables put the head at the overlay's base */
#ifdef NONMATCHING
/* GCC ignores #pragma section, and drops a static datum that no C names. so
 * its build marks the head used, in the section its link puts at the
 * overlay's base */
#define HEAD_SECTION __attribute__((used, section(".head")))
#else
#define HEAD_SECTION
#endif

/* call the init function of the overlay in STAGE_SLOT, if its head holds one,
 * then clear the head's pointer to it, so that it runs once.
 * overlay_run_exit does the same with the exit function */
void overlay_run_init(void);
void overlay_run_exit(void);

#endif
