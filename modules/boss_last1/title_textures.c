#include <ninja.h>
#include <sg_xpt.h>

#include "boss_last1/arena.h"
#include "boss_last1/combat_art.h"
#include "boss_last1/texture_names.h"
#include "player/player.h"
#include "shared/motions.h"

#include "boss_last1/title_textures/text.inc"

/* the names that a texture list's textures point at, each starting on a 4-byte
 * boundary */
const char biolizard_title_textures_filenames[268] = {
#include "boss_last1/title_textures/biolizard_title_textures_filenames.inc"
};

/* BOSS_LAST1.PRS's texture lists, chunk models, motions, their registry and
 * texture names, from the disc. its C lies between this file's own C and
 * sounds.c, and its D lies between biolizard_model_textures.c and boss.c */
static NJS_TEXNAME title_textures[26] = {
#include "boss_last1/title_textures/title_textures.inc"
};
NJS_TEXLIST biolizard_title_texlist = {
#include "boss_last1/title_textures/biolizard_title_texlist.inc"
};
static Sint16 energy_bowl_inner_polygons[973] = {
#include "boss_last1/title_textures/energy_bowl_inner_polygons.inc"
};
static Sint32 energy_bowl_inner_vertices[807] = {
#include "boss_last1/title_textures/energy_bowl_inner_vertices.inc"
};
static NJS_CNK_MODEL energy_bowl_inner_mesh = {
#include "boss_last1/title_textures/energy_bowl_inner_mesh.inc"
};
static NJS_CNK_OBJECT energy_bowl_inner = {
#include "boss_last1/title_textures/energy_bowl_inner.inc"
};
static Sint16 energy_bowl_polygons[345] = {
#include "boss_last1/title_textures/energy_bowl_polygons.inc"
};
static Sint32 energy_bowl_vertices[807] = {
#include "boss_last1/title_textures/energy_bowl_vertices.inc"
};
static NJS_CNK_MODEL energy_bowl_mesh = {
#include "boss_last1/title_textures/energy_bowl_mesh.inc"
};
NJS_CNK_OBJECT biolizard_energy_bowl = {
#include "boss_last1/title_textures/biolizard_energy_bowl.inc"
};
static Sint16 energy_thunder_polygons[1001] = {
#include "boss_last1/title_textures/energy_thunder_polygons.inc"
};
static Sint32 energy_thunder_vertices[807] = {
#include "boss_last1/title_textures/energy_thunder_vertices.inc"
};
static NJS_CNK_MODEL energy_thunder_mesh = {
#include "boss_last1/title_textures/energy_thunder_mesh.inc"
};
NJS_CNK_OBJECT biolizard_energy_thunder = {
#include "boss_last1/title_textures/biolizard_energy_thunder.inc"
};
static NJS_TEXNAME effect_textures[10] = {
#include "boss_last1/title_textures/effect_textures.inc"
};
NJS_TEXLIST biolizard_effect_texlist = {
#include "boss_last1/title_textures/biolizard_effect_texlist.inc"
};
static NJS_MKEY_A biolizard_energy_bowl_motion_joint_0_angles[29] = {
#include "boss_last1/title_textures/biolizard_energy_bowl_motion_joint_0_angles.inc"
};
static NJS_MKEY_A biolizard_energy_bowl_motion_joint_1_angles[29] = {
#include "boss_last1/title_textures/biolizard_energy_bowl_motion_joint_1_angles.inc"
};
static NJS_MDATA2 biolizard_energy_bowl_motion_tracks[2] = {
#include "boss_last1/title_textures/biolizard_energy_bowl_motion_tracks.inc"
};
NJS_MOTION biolizard_energy_bowl_motion = {
#include "boss_last1/title_textures/biolizard_energy_bowl_motion.inc"
};
static Sint16 spark_a_polygons[25] = {
#include "boss_last1/title_textures/spark_a_polygons.inc"
};
static Sint32 spark_a_vertices[27] = {
#include "boss_last1/title_textures/spark_a_vertices.inc"
};
static NJS_CNK_MODEL spark_a_mesh = {
#include "boss_last1/title_textures/spark_a_mesh.inc"
};
NJS_CNK_OBJECT biolizard_spark_a = {
#include "boss_last1/title_textures/biolizard_spark_a.inc"
};
static Sint16 spark_b_polygons[205] = {
#include "boss_last1/title_textures/spark_b_polygons.inc"
};
static Sint32 spark_b_vertices[159] = {
#include "boss_last1/title_textures/spark_b_vertices.inc"
};
static NJS_CNK_MODEL spark_b_mesh = {
#include "boss_last1/title_textures/spark_b_mesh.inc"
};
NJS_CNK_OBJECT biolizard_spark_b = {
#include "boss_last1/title_textures/biolizard_spark_b.inc"
};
static NJS_MKEY_A biolizard_energy_thunder_motion_joint_0_angles[29] = {
#include "boss_last1/title_textures/biolizard_energy_thunder_motion_joint_0_angles.inc"
};
static NJS_MDATA2 biolizard_energy_thunder_motion_tracks[1] = {
#include "boss_last1/title_textures/biolizard_energy_thunder_motion_tracks.inc"
};
NJS_MOTION biolizard_energy_thunder_motion = {
#include "boss_last1/title_textures/biolizard_energy_thunder_motion.inc"
};
static Sint16 unused_sphere_inner_polygons[161] = {
#include "boss_last1/title_textures/unused_sphere_inner_polygons.inc"
};
static Sint32 unused_sphere_inner_vertices[351] = {
#include "boss_last1/title_textures/unused_sphere_inner_vertices.inc"
};
static NJS_CNK_MODEL unused_sphere_inner_mesh = {
#include "boss_last1/title_textures/unused_sphere_inner_mesh.inc"
};
static NJS_CNK_OBJECT unused_sphere_inner = {
#include "boss_last1/title_textures/unused_sphere_inner.inc"
};
static Sint16 unused_sphere_polygons[303] = {
#include "boss_last1/title_textures/unused_sphere_polygons.inc"
};
static Sint32 unused_sphere_vertices[207] = {
#include "boss_last1/title_textures/unused_sphere_vertices.inc"
};
static NJS_CNK_MODEL unused_sphere_mesh = {
#include "boss_last1/title_textures/unused_sphere_mesh.inc"
};
static NJS_CNK_OBJECT unused_sphere = {
#include "boss_last1/title_textures/unused_sphere.inc"
};
static Sint16 sphere_near_core_polygons[161] = {
#include "boss_last1/title_textures/sphere_near_core_polygons.inc"
};
static Sint32 sphere_near_core_vertices[351] = {
#include "boss_last1/title_textures/sphere_near_core_vertices.inc"
};
static NJS_CNK_MODEL sphere_near_core_mesh = {
#include "boss_last1/title_textures/sphere_near_core_mesh.inc"
};
NJS_CNK_OBJECT biolizard_sphere_near_core = {
#include "boss_last1/title_textures/biolizard_sphere_near_core.inc"
};
static Sint16 sphere_near_shell_polygons[303] = {
#include "boss_last1/title_textures/sphere_near_shell_polygons.inc"
};
static Sint32 sphere_near_shell_vertices[207] = {
#include "boss_last1/title_textures/sphere_near_shell_vertices.inc"
};
static NJS_CNK_MODEL sphere_near_shell_mesh = {
#include "boss_last1/title_textures/sphere_near_shell_mesh.inc"
};
NJS_CNK_OBJECT biolizard_sphere_near_shell = {
#include "boss_last1/title_textures/biolizard_sphere_near_shell.inc"
};
static NJS_CNK_OBJECT near_sphere = {
#include "boss_last1/title_textures/near_sphere.inc"
};
static Sint16 sphere_far_core_polygons[45] = {
#include "boss_last1/title_textures/sphere_far_core_polygons.inc"
};
static Sint32 sphere_far_core_vertices[99] = {
#include "boss_last1/title_textures/sphere_far_core_vertices.inc"
};
static NJS_CNK_MODEL sphere_far_core_mesh = {
#include "boss_last1/title_textures/sphere_far_core_mesh.inc"
};
NJS_CNK_OBJECT biolizard_sphere_far_core = {
#include "boss_last1/title_textures/biolizard_sphere_far_core.inc"
};
static Sint16 sphere_far_shell_polygons[115] = {
#include "boss_last1/title_textures/sphere_far_shell_polygons.inc"
};
static Sint32 sphere_far_shell_vertices[99] = {
#include "boss_last1/title_textures/sphere_far_shell_vertices.inc"
};
static NJS_CNK_MODEL sphere_far_shell_mesh = {
#include "boss_last1/title_textures/sphere_far_shell_mesh.inc"
};
NJS_CNK_OBJECT biolizard_sphere_far_shell = {
#include "boss_last1/title_textures/biolizard_sphere_far_shell.inc"
};
static NJS_CNK_OBJECT far_sphere = {
#include "boss_last1/title_textures/far_sphere.inc"
};
static Sint16 sphere_middle_core_polygons[97] = {
#include "boss_last1/title_textures/sphere_middle_core_polygons.inc"
};
static Sint32 sphere_middle_core_vertices[195] = {
#include "boss_last1/title_textures/sphere_middle_core_vertices.inc"
};
static NJS_CNK_MODEL sphere_middle_core_mesh = {
#include "boss_last1/title_textures/sphere_middle_core_mesh.inc"
};
NJS_CNK_OBJECT biolizard_sphere_middle_core = {
#include "boss_last1/title_textures/biolizard_sphere_middle_core.inc"
};
static Sint16 sphere_middle_shell_polygons[199] = {
#include "boss_last1/title_textures/sphere_middle_shell_polygons.inc"
};
static Sint32 sphere_middle_shell_vertices[159] = {
#include "boss_last1/title_textures/sphere_middle_shell_vertices.inc"
};
static NJS_CNK_MODEL sphere_middle_shell_mesh = {
#include "boss_last1/title_textures/sphere_middle_shell_mesh.inc"
};
NJS_CNK_OBJECT biolizard_sphere_middle_shell = {
#include "boss_last1/title_textures/biolizard_sphere_middle_shell.inc"
};
static NJS_CNK_OBJECT middle_sphere = {
#include "boss_last1/title_textures/middle_sphere.inc"
};
static Sint16 support_base_polygons[87] = {
#include "boss_last1/title_textures/support_base_polygons.inc"
};
static Sint32 support_base_vertices[201] = {
#include "boss_last1/title_textures/support_base_vertices.inc"
};
static NJS_CNK_MODEL support_base_mesh = {
#include "boss_last1/title_textures/support_base_mesh.inc"
};
NJS_CNK_OBJECT biolizard_support_base = {
#include "boss_last1/title_textures/biolizard_support_base.inc"
};
static Sint16 support_pump_polygons[67] = {
#include "boss_last1/title_textures/support_pump_polygons.inc"
};
static Sint32 support_pump_vertices[99] = {
#include "boss_last1/title_textures/support_pump_vertices.inc"
};
static NJS_CNK_MODEL support_pump_mesh = {
#include "boss_last1/title_textures/support_pump_mesh.inc"
};
NJS_CNK_OBJECT biolizard_support_pump = {
#include "boss_last1/title_textures/biolizard_support_pump.inc"
};
static NJS_MKEY_F biolizard_support_motion_joint_0_position[7] = {
#include "boss_last1/title_textures/biolizard_support_motion_joint_0_position.inc"
};
static NJS_MDATA2 biolizard_support_motion_tracks[1] = {
#include "boss_last1/title_textures/biolizard_support_motion_tracks.inc"
};
NJS_MOTION biolizard_support_motion = {
#include "boss_last1/title_textures/biolizard_support_motion.inc"
};
static Sint16 sphere_flash_polygons[159] = {
#include "boss_last1/title_textures/sphere_flash_polygons.inc"
};
static Sint32 sphere_flash_vertices[351] = {
#include "boss_last1/title_textures/sphere_flash_vertices.inc"
};
static NJS_CNK_MODEL sphere_flash_mesh = {
#include "boss_last1/title_textures/sphere_flash_mesh.inc"
};
NJS_CNK_OBJECT biolizard_sphere_flash = {
#include "boss_last1/title_textures/biolizard_sphere_flash.inc"
};
static Sint16 energy_shadow_polygons[155] = {
#include "boss_last1/title_textures/energy_shadow_polygons.inc"
};
static Sint32 energy_shadow_vertices[103] = {
#include "boss_last1/title_textures/energy_shadow_vertices.inc"
};
NJS_CNK_MODEL biolizard_energy_shadow_mesh = {
#include "boss_last1/title_textures/biolizard_energy_shadow_mesh.inc"
};
static NJS_CNK_OBJECT energy_shadow = {
#include "boss_last1/title_textures/energy_shadow.inc"
};
static NJS_MKEY_F player_lift_motion_joint_2_position[32] = {
#include "boss_last1/title_textures/player_lift_motion_joint_2_position.inc"
};
static NJS_MKEY_A player_lift_motion_joint_4_angles[32] = {
#include "boss_last1/title_textures/player_lift_motion_joint_4_angles.inc"
};
static NJS_MKEY_A player_lift_motion_joint_7_angles[32] = {
#include "boss_last1/title_textures/player_lift_motion_joint_7_angles.inc"
};
static NJS_MKEY_A player_lift_motion_joint_8_angles[32] = {
#include "boss_last1/title_textures/player_lift_motion_joint_8_angles.inc"
};
static NJS_MKEY_A player_lift_motion_joint_9_angles[32] = {
#include "boss_last1/title_textures/player_lift_motion_joint_9_angles.inc"
};
static NJS_MKEY_A player_lift_motion_joint_13_angles[32] = {
#include "boss_last1/title_textures/player_lift_motion_joint_13_angles.inc"
};
static NJS_MKEY_A player_lift_motion_joint_14_angles[32] = {
#include "boss_last1/title_textures/player_lift_motion_joint_14_angles.inc"
};
static NJS_MKEY_A player_lift_motion_joint_17_angles[32] = {
#include "boss_last1/title_textures/player_lift_motion_joint_17_angles.inc"
};
static NJS_MKEY_A player_lift_motion_joint_18_angles[32] = {
#include "boss_last1/title_textures/player_lift_motion_joint_18_angles.inc"
};
static NJS_MKEY_A player_lift_motion_joint_19_angles[32] = {
#include "boss_last1/title_textures/player_lift_motion_joint_19_angles.inc"
};
static NJS_MKEY_A player_lift_motion_joint_23_angles[32] = {
#include "boss_last1/title_textures/player_lift_motion_joint_23_angles.inc"
};
static NJS_MKEY_A player_lift_motion_joint_24_angles[32] = {
#include "boss_last1/title_textures/player_lift_motion_joint_24_angles.inc"
};
static NJS_MKEY_A player_lift_motion_joint_27_angles[32] = {
#include "boss_last1/title_textures/player_lift_motion_joint_27_angles.inc"
};
static NJS_MKEY_A player_lift_motion_joint_28_angles[2] = {
#include "boss_last1/title_textures/player_lift_motion_joint_28_angles.inc"
};
static NJS_MKEY_A player_lift_motion_joint_29_angles[2] = {
#include "boss_last1/title_textures/player_lift_motion_joint_29_angles.inc"
};
static NJS_MKEY_A player_lift_motion_joint_31_angles[32] = {
#include "boss_last1/title_textures/player_lift_motion_joint_31_angles.inc"
};
static NJS_MKEY_A player_lift_motion_joint_32_angles[32] = {
#include "boss_last1/title_textures/player_lift_motion_joint_32_angles.inc"
};
static NJS_MKEY_A player_lift_motion_joint_33_angles[32] = {
#include "boss_last1/title_textures/player_lift_motion_joint_33_angles.inc"
};
static NJS_MKEY_A player_lift_motion_joint_37_angles[32] = {
#include "boss_last1/title_textures/player_lift_motion_joint_37_angles.inc"
};
static NJS_MKEY_A player_lift_motion_joint_38_angles[32] = {
#include "boss_last1/title_textures/player_lift_motion_joint_38_angles.inc"
};
static NJS_MKEY_A player_lift_motion_joint_39_angles[32] = {
#include "boss_last1/title_textures/player_lift_motion_joint_39_angles.inc"
};
static NJS_MKEY_A player_lift_motion_joint_41_angles[32] = {
#include "boss_last1/title_textures/player_lift_motion_joint_41_angles.inc"
};
static NJS_MKEY_A player_lift_motion_joint_44_angles[32] = {
#include "boss_last1/title_textures/player_lift_motion_joint_44_angles.inc"
};
static NJS_MKEY_A player_lift_motion_joint_51_angles[32] = {
#include "boss_last1/title_textures/player_lift_motion_joint_51_angles.inc"
};
static NJS_MKEY_A player_lift_motion_joint_53_angles[32] = {
#include "boss_last1/title_textures/player_lift_motion_joint_53_angles.inc"
};
static NJS_MKEY_A player_lift_motion_joint_56_angles[32] = {
#include "boss_last1/title_textures/player_lift_motion_joint_56_angles.inc"
};
static NJS_MKEY_A player_lift_motion_joint_57_angles[32] = {
#include "boss_last1/title_textures/player_lift_motion_joint_57_angles.inc"
};
static NJS_MKEY_A player_lift_motion_joint_58_angles[32] = {
#include "boss_last1/title_textures/player_lift_motion_joint_58_angles.inc"
};
static NJS_MKEY_A player_lift_motion_joint_60_angles[32] = {
#include "boss_last1/title_textures/player_lift_motion_joint_60_angles.inc"
};
static NJS_MDATA2 player_lift_motion_tracks[62] = {
#include "boss_last1/title_textures/player_lift_motion_tracks.inc"
};
static NJS_MOTION player_lift_motion = {
#include "boss_last1/title_textures/player_lift_motion.inc"
};
struct motion_entry module_motions[2] = {
#include "boss_last1/title_textures/module_motions.inc"
};
