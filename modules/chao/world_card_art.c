#include <sg_xpt.h>
#include "chao/world_card_art.h"
#include "chao/walk_assets.h"
#include "chao/walk_card.h"

const char world_card_filename[16] = "SONIC2___ALF";
/* the bytes before the file's data: its 128-byte header and three icon
 * frames of 512 bytes */
const Uint32 world_card_header_bytes = 128 + (3 * 512);
const char world_card_comment[18] = "CHAO FILE";
const char world_card_title[34] = "SONIC ADVENTURE 2 / CHAO File";
const Uint16 world_card_palette[16] = {
#include "chao/world_card_art/world_card_palette.inc"
};

/* three frames of 32 by 32 pixels at four bits a pixel. a row is 16 bytes,
 * two palette indices to a byte */
const Uint8 world_card_icon[3][512] = {
#include "chao/world_card_art/world_card_icon.inc"
};

const char world_card_task_name[15] = "ALVM_WorldExec";
const char walk_vmu_filename[13] = "SONIC2____VM";
const char walk_signature[12] = "CHAO2ACCEPT";
const Sint32 walk_game_blocks = 128;
const char walk_sprite_neutral_normal[16] = "OSMP2CNN.PRS";
const char walk_sprite_neutral_swim[16] = "OSMP2CNS.PRS";
const char walk_sprite_neutral_fly[16] = "OSMP2CNF.PRS";
const char walk_sprite_neutral_run[16] = "OSMP2CNR.PRS";
const char walk_sprite_neutral_power[16] = "OSMP2CNP.PRS";
const char walk_sprite_neutral_chaos[16] = "OSMP2CNC.PRS";
const char walk_sprite_hero_normal[16] = "OSMP2CHN.PRS";
const char walk_sprite_hero_swim[16] = "OSMP2CHS.PRS";
const char walk_sprite_hero_fly[16] = "OSMP2CHF.PRS";
const char walk_sprite_hero_run[16] = "OSMP2CHR.PRS";
const char walk_sprite_hero_power[16] = "OSMP2CHP.PRS";
const char walk_sprite_hero_chaos[16] = "OSMP2CHC.PRS";
const char walk_sprite_dark_normal[16] = "OSMP2CDN.PRS";
const char walk_sprite_dark_swim[16] = "OSMP2CDS.PRS";
const char walk_sprite_dark_fly[16] = "OSMP2CDF.PRS";
const char walk_sprite_dark_run[16] = "OSMP2CDR.PRS";
const char walk_sprite_dark_power[16] = "OSMP2CDP.PRS";
const char walk_sprite_dark_chaos[16] = "OSMP2CDC.PRS";

const char walk_legacy_signature[12] = "CHAO ACCEPT";
const char walk_label_disk_absent[12] = "(NO DISK)";
const char walk_label_disk_reading[12] = "(LOADING)";
const char walk_label_disk_loaded[12] = "HOGEHOGE";
const char walk_label_disk_empty[12] = "(NO FILE)";
const char walk_label_cursor[4] = ">>";
const char walk_label_chao[4] = "C";
const char walk_label_empty[4] = "-";
const char walk_label_name[4] = "%s";
const char walk_label_egg[4] = "E";
const char walk_label_item[4] = "%c";
const char walk_label_confirm[4] = "OK";
const char walk_label_underline[4] = "=";
const char walk_menu_task_name[16] = "OsanpoMenuExec";
const char walk_job_task_name[12] = "OsampoExec";
static const char walk_program_names[5][16] = {
#include "chao/world_card_art/walk_program_names.inc"
};
static const char walk_encounter_names[5][2][16] = {
#include "chao/world_card_art/walk_encounter_names.inc"
};

Uint16 world_card_icon_count = 3;
Uint16 world_card_icon_speed = 8;
void *world_card_visual = NULL;
Uint16 world_card_visual_type = 0;

const char *walk_programs[5] = {
#include "chao/world_card_art/walk_programs.inc"
};
const char *walk_encounter_files[10] = {
#include "chao/world_card_art/walk_encounter_files.inc"
};
struct walk_menu *garden_walk_menu = NULL;
