/* the game's fixed areas of RAM, in address order. RAM appears both at
 * 0x0c000000 and at 0x8c000000. each address here is in the view that the code
 * using it loads */
#ifndef SYSTEM_AREAS_H
#define SYSTEM_AREAS_H
#include <sg_xpt.h>

/* a 2 MB scratch area, up to WORK_AREA_END. each use writes over what the
 * last one left, and the loader's reads are among them */
#define WORK_AREA ((Uint8 *)0x8c300000)
/* the second megabyte of WORK_AREA. a stage's camera file, each animal's file
 * and the block grid's bitmap are read there */
#define WORK_AREA_FILE (WORK_AREA + 0x100000)
#define WORK_AREA_END ((Uint8 *)0x8c500000)

/* the base of the slot that the stages, the bosses and the other programs
 * take turns in */
#define STAGE_SLOT ((Uint8 *)0x8c500000)

/* Chao archive sections use P0 addresses. The loader writes the same RAM
 * through the P1 alias, as declared in the Chao image link. */
#define CHAO_MODEL_SLOT ((Uint8 *)0x8c698700)
#define CHAO_MODEL_BYTES 2235540U
#define CHAO_MOTION_SLOT ((Uint8 *)0x8c8ba800)
#define CHAO_MOTION_BYTES 1065680U

/* STAGE_SLOT as an object that no source defines, which symbols.txt places
 * at the slot's address */
extern Uint8 stage_slot[];

/* the model workspace. it starts where a stage's or a boss's image ends,
 * rounded up to a 32-byte boundary */
#ifdef NONMATCHING
/* the GNU stage layout defines end past the image's data and bss. this
 * side's objects don't define stage_model_build_area, which only symbols.txt
 * places */
#include <stdint.h>

extern unsigned char end[];

#define STAGE_MODEL_BUILD_AREA                                                 \
    ((Uint8 *)(((uintptr_t)end + 31U) & ~(uintptr_t)31U))
#else
extern Uint8 stage_model_build_area[];
#define STAGE_MODEL_BUILD_AREA stage_model_build_area
#endif

/* the work area system_boot_init gives syCfgInit */
#define CONFIG_WORK ((void *)0x8c900000)

/* the Chao race loads its land and glass pointers at the start of
 * CHAO_RACE_LAND_AREA. the Chao lobby loads its land pointer at the start of
 * CHAO_LOBBY_LAND_AREA. only one Chao scene holds these addresses at a time */
#define CHAO_RACE_LAND_AREA ((Uint8 *)0x8ca40000)
#define CHAO_LOBBY_LAND_AREA ((Uint8 *)0x8ca80000)

/* where the race and the lobby compile their land, below DEMO_AREA */
#define CHAO_LAND_BUILD_AREA ((Uint8 *)0x8caa0000)

/* the start of an area many uses read a file into. a stage's demo file is
 * one, as are a mini event's me<event>.prs and each file save_downloads_find
 * reads. texture_load_prs_file reads a file into the bytes below it.
 * world_build_land stops before a model that would reach it */
#define DEMO_AREA ((Uint8 *)0x8cb00000)

/* a mini event's messages, script and requests, in 48, 12 and 4 KB. these
 * areas and the camera areas lie end to end, up to CAMLIN_AREA_END */
#define MESSAGE_AREA ((Uint8 *)0x0cbd0000)
/* DEMO_AREA ends where MESSAGE_AREA starts in the same RAM, for every file
 * read into it but a stage's demo file */
#ifdef NONMATCHING
/* the bytes this side's checked reads take DEMO_AREA to hold. the | gives
 * MESSAGE_AREA the P1 address DEMO_AREA has */
#define DEMO_AREA_BYTES                                                        \
    ((Uint32)(((uintptr_t)MESSAGE_AREA | 0x80000000U) - (uintptr_t)DEMO_AREA))
/* the bytes a stage's demo file must decode to on this side. each of the
 * disc's six is a dump of RAM from DEMO_AREA up to CAMSET_AREA, which runs on
 * over a mini event's three areas. pad_run_demo reads a frame for each step
 * from DEMO_AREA, with no bound, so a shorter file would leave it the RAM's
 * older bytes for frames. the file and a mini event's files hold that RAM by
 * turns, since mini_event_load loads each area an event reads again before
 * the event starts */
#define DEMO_FILE_BYTES                                                        \
    ((Uint32)((uintptr_t)CAMSET_AREA - (uintptr_t)DEMO_AREA))
#endif
#define SCRIPT_AREA ((Uint8 *)0x0cbdc000)
#define REQUEST_AREA ((Uint8 *)0x0cbdf000)
/* a stage's camera files, as loader_read_camera_files reads them.
 * CAMSET_AREA holds 64 KB, and each of the other four 16 KB. an area whose
 * name ends in 0 or 1 holds player 0's or player 1's file */
#define CAMSET_AREA ((Uint8 *)0x8cbe0000)
#define CAMSET_AREA_0 ((Uint8 *)0x8cbf0000)
#define CAMSET_AREA_1 ((Uint8 *)0x8cbf4000)
#define CAMLIN_AREA_0 ((Uint8 *)0x8cbf8000)
#define CAMLIN_AREA_1 ((Uint8 *)0x8cbfc000)
#define CAMLIN_AREA_END ((Uint8 *)0x8cc00000)

#endif
