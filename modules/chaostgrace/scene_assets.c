#include <ninja.h>
#include "chao/race_record.h"
#include "chaostgrace/scene.h"
#include "chaostgrace/scene_files.h"

const struct race_podium race_podium = {
#include "chaostgrace/scene_assets/race_podium.inc"
};
const char race_award_music[16] = "chao_r_a.adx";
const char race_beginner_music[16] = "chao_r_b.adx";
const char race_jewel_music[16] = "chao_r_j.adx";
const char race_challenge_music[16] = "chao_r_c.adx";
const char race_dark_music[16] = "chao_r_d.adx";
const char race_hero_music[16] = "chao_r_h.adx";
const char race_neutral_land_file[24] = "ChaoStgRaceLandN.PRS";
const char race_hero_land_file[24] = "ChaoStgRaceLandH.PRS";
const char race_dark_land_file[24] = "ChaoStgRaceLandD.PRS";
static const char neutral_set[24], hero_set[24], dark_set[24];
static const char beginner_swim[8], beginner_fly[8], beginner_run[8],
    beginner_power[8];
static const char jewel_swim[8], jewel_fly[8], jewel_run[8], jewel_power[8],
    jewel_il[8], jewel_long[8];
const struct race_land_files race_land_sets = {
    {neutral_set, hero_set, dark_set}};
/* clang-format off */
const struct race_track_files race_track_sets = {
    {beginner_swim, beginner_fly, beginner_run, beginner_power,
     jewel_swim, jewel_fly, jewel_run, jewel_power, jewel_il, jewel_long}};
/* clang-format on */
const char race_set_prefix[12] = "ChaoSetRace";
const char race_set_suffix[8] = ".bin";
const char race_neutral_objects[20] = "AL_RACE_OBJ_N_TEX";
const char race_neutral_water[20] = "AL_RACE_WATER_TEX";
const char race_hero_objects[20] = "AL_RACE_OBJ_H_TEX";
const char race_hero_water[24] = "AL_RACE_WATER_HERO_TEX";
const char race_dark_objects[20] = "AL_RACE_OBJ_D_TEX";
const char race_dark_water[24] = "AL_RACE_WATER_DARK_TEX";
const char race_hud_textures[16] = "AL_RACE_ADV_TEX";
const char race_animal_textures[16] = "AL_MINIMAL_TEX";
const char race_item_textures[20] = "AL_RACE_OBJ_C_TEX";
const char race_toy_textures[8] = "AL_TOY";
const char race_light_file[16] = "stg50_light.bin";
const char race_messages[16] = "msgChaoEntrance";
const char race_glass_name[20] = "AL_GlassExecutor";

const char race_effect_archive[16] = "efftex_common";
static const char neutral_set[24] = "ChaoSetRaceLandNeut.bin";
static const char hero_set[24] = "ChaoSetRaceLandHero.bin";
static const char dark_set[24] = "ChaoSetRaceLandDark.bin";
static const char beginner_swim[8] = "BegSwm";
static const char beginner_fly[8] = "BegFly";
static const char beginner_run[8] = "BegRun";
static const char beginner_power[8] = "BegPow";
static const char jewel_swim[8] = "JwlSwm";
static const char jewel_fly[8] = "JwlFly";
static const char jewel_run[8] = "JwlRun";
static const char jewel_power[8] = "JwlPow";
static const char jewel_il[8] = "JwlIL";
static const char jewel_long[8] = "JwlLng";

NJS_VECTOR race_narrow_offsets[RACE_ENTRANTS] = {
#include "chaostgrace/scene_assets/race_narrow_offsets.inc"
};
NJS_VECTOR race_wide_offsets[RACE_ENTRANTS] = {
#include "chaostgrace/scene_assets/race_wide_offsets.inc"
};
