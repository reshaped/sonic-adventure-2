/* STG30.PRS's paths and path list, from the disc. its D lies between the
 * section's start and sky_pack.c */
#include "stage/path.h"
#include "stage/path_task.h"
#include "stg30/paths.h"

static struct path_point rail_01_points[24] = {
#include "stg30/paths/rail_01_points.inc"
};
static struct path rail_01 = {
#include "stg30/paths/rail_01.inc"
};
static struct path_point rail_02_points[5] = {
#include "stg30/paths/rail_02_points.inc"
};
static struct path rail_02 = {
#include "stg30/paths/rail_02.inc"
};
static struct path_point rail_03_points[21] = {
#include "stg30/paths/rail_03_points.inc"
};
static struct path rail_03 = {
#include "stg30/paths/rail_03.inc"
};
static struct path_point rail_04_points[5] = {
#include "stg30/paths/rail_04_points.inc"
};
static struct path rail_04 = {
#include "stg30/paths/rail_04.inc"
};
static struct path_point rail_05_points[21] = {
#include "stg30/paths/rail_05_points.inc"
};
static struct path rail_05 = {
#include "stg30/paths/rail_05.inc"
};
static struct path_point rail_06_points[40] = {
#include "stg30/paths/rail_06_points.inc"
};
static struct path rail_06 = {
#include "stg30/paths/rail_06.inc"
};
static struct path_point rail_07_points[40] = {
#include "stg30/paths/rail_07_points.inc"
};
static struct path rail_07 = {
#include "stg30/paths/rail_07.inc"
};
static struct path_point rail_08_points[25] = {
#include "stg30/paths/rail_08_points.inc"
};
static struct path rail_08 = {
#include "stg30/paths/rail_08.inc"
};
static struct path_point rail_09_points[22] = {
#include "stg30/paths/rail_09_points.inc"
};
static struct path rail_09 = {
#include "stg30/paths/rail_09.inc"
};
static struct path_point rail_10_points[22] = {
#include "stg30/paths/rail_10_points.inc"
};
static struct path rail_10 = {
#include "stg30/paths/rail_10.inc"
};
static struct path_point rail_11_points[5] = {
#include "stg30/paths/rail_11_points.inc"
};
static struct path rail_11 = {
#include "stg30/paths/rail_11.inc"
};
static struct path_point rail_12_points[96] = {
#include "stg30/paths/rail_12_points.inc"
};
static struct path rail_12 = {
#include "stg30/paths/rail_12.inc"
};
static struct path_point rail_13_points[5] = {
#include "stg30/paths/rail_13_points.inc"
};
static struct path rail_13 = {
#include "stg30/paths/rail_13.inc"
};
static struct path_point rail_14_points[21] = {
#include "stg30/paths/rail_14_points.inc"
};
static struct path rail_14 = {
#include "stg30/paths/rail_14.inc"
};
static struct path_point rail_15_points[9] = {
#include "stg30/paths/rail_15_points.inc"
};
static struct path rail_15 = {
#include "stg30/paths/rail_15.inc"
};
static struct path_point rail_16_points[9] = {
#include "stg30/paths/rail_16_points.inc"
};
static struct path rail_16 = {
#include "stg30/paths/rail_16.inc"
};
static struct path_point rail_17_points[9] = {
#include "stg30/paths/rail_17_points.inc"
};
static struct path rail_17 = {
#include "stg30/paths/rail_17.inc"
};
static struct path_point rail_18_points[100] = {
#include "stg30/paths/rail_18_points.inc"
};
static struct path rail_18 = {
#include "stg30/paths/rail_18.inc"
};
static struct path_point rail_19_points[12] = {
#include "stg30/paths/rail_19_points.inc"
};
static struct path rail_19 = {
#include "stg30/paths/rail_19.inc"
};
static struct path_point rail_20_points[11] = {
#include "stg30/paths/rail_20_points.inc"
};
static struct path rail_20 = {
#include "stg30/paths/rail_20.inc"
};
static struct path_point rail_21_points[26] = {
#include "stg30/paths/rail_21_points.inc"
};
static struct path rail_21 = {
#include "stg30/paths/rail_21.inc"
};
static struct path_point rail_22_points[46] = {
#include "stg30/paths/rail_22_points.inc"
};
static struct path rail_22 = {
#include "stg30/paths/rail_22.inc"
};
static struct path_point rail_23_points[60] = {
#include "stg30/paths/rail_23_points.inc"
};
static struct path rail_23 = {
#include "stg30/paths/rail_23.inc"
};
static struct path_point rail_24_points[18] = {
#include "stg30/paths/rail_24_points.inc"
};
static struct path rail_24 = {
#include "stg30/paths/rail_24.inc"
};
static struct path_point rail_25_points[9] = {
#include "stg30/paths/rail_25_points.inc"
};
static struct path rail_25 = {
#include "stg30/paths/rail_25.inc"
};
static struct path_point rail_26_points[9] = {
#include "stg30/paths/rail_26_points.inc"
};
static struct path rail_26 = {
#include "stg30/paths/rail_26.inc"
};
static struct path_point rail_27_points[9] = {
#include "stg30/paths/rail_27_points.inc"
};
static struct path rail_27 = {
#include "stg30/paths/rail_27.inc"
};
static struct path_point rail_28_points[9] = {
#include "stg30/paths/rail_28_points.inc"
};
static struct path rail_28 = {
#include "stg30/paths/rail_28.inc"
};
static struct path_point rail_29_points[9] = {
#include "stg30/paths/rail_29_points.inc"
};
static struct path rail_29 = {
#include "stg30/paths/rail_29.inc"
};
static struct path_point rail_30_points[34] = {
#include "stg30/paths/rail_30_points.inc"
};
static struct path rail_30 = {
#include "stg30/paths/rail_30.inc"
};
static struct path_point rail_31_points[21] = {
#include "stg30/paths/rail_31_points.inc"
};
static struct path rail_31 = {
#include "stg30/paths/rail_31.inc"
};
static struct path_point rail_32_points[5] = {
#include "stg30/paths/rail_32_points.inc"
};
static struct path rail_32 = {
#include "stg30/paths/rail_32.inc"
};
static struct path_point rail_33_points[13] = {
#include "stg30/paths/rail_33_points.inc"
};
static struct path rail_33 = {
#include "stg30/paths/rail_33.inc"
};
static struct path_point rail_34_points[26] = {
#include "stg30/paths/rail_34_points.inc"
};
static struct path rail_34 = {
#include "stg30/paths/rail_34.inc"
};
static struct path_point rail_35_points[19] = {
#include "stg30/paths/rail_35_points.inc"
};
static struct path rail_35 = {
#include "stg30/paths/rail_35.inc"
};
static struct path_point rail_36_points[41] = {
#include "stg30/paths/rail_36_points.inc"
};
static struct path rail_36 = {
#include "stg30/paths/rail_36.inc"
};
static struct path_point rail_37_points[38] = {
#include "stg30/paths/rail_37_points.inc"
};
static struct path rail_37 = {
#include "stg30/paths/rail_37.inc"
};
static struct path_point rail_38_points[45] = {
#include "stg30/paths/rail_38_points.inc"
};
static struct path rail_38 = {
#include "stg30/paths/rail_38.inc"
};
static struct path_point rail_39_points[13] = {
#include "stg30/paths/rail_39_points.inc"
};
static struct path rail_39 = {
#include "stg30/paths/rail_39.inc"
};
static struct path_point rail_40_points[50] = {
#include "stg30/paths/rail_40_points.inc"
};
static struct path rail_40 = {
#include "stg30/paths/rail_40.inc"
};
static struct path_point rail_41_points[64] = {
#include "stg30/paths/rail_41_points.inc"
};
static struct path rail_41 = {
#include "stg30/paths/rail_41.inc"
};
static struct path_point rail_42_points[135] = {
#include "stg30/paths/rail_42_points.inc"
};
static struct path rail_42 = {
#include "stg30/paths/rail_42.inc"
};
static struct path_point rail_43_points[21] = {
#include "stg30/paths/rail_43_points.inc"
};
static struct path rail_43 = {
#include "stg30/paths/rail_43.inc"
};
static struct path_point rail_44_points[19] = {
#include "stg30/paths/rail_44_points.inc"
};
static struct path rail_44 = {
#include "stg30/paths/rail_44.inc"
};
static struct path_point rail_45_points[21] = {
#include "stg30/paths/rail_45_points.inc"
};
static struct path rail_45 = {
#include "stg30/paths/rail_45.inc"
};
static struct path_point rail_46_points[15] = {
#include "stg30/paths/rail_46_points.inc"
};
static struct path rail_46 = {
#include "stg30/paths/rail_46.inc"
};
static struct path_point rail_47_points[15] = {
#include "stg30/paths/rail_47_points.inc"
};
static struct path rail_47 = {
#include "stg30/paths/rail_47.inc"
};
static struct path_point rail_48_points[35] = {
#include "stg30/paths/rail_48_points.inc"
};
static struct path rail_48 = {
#include "stg30/paths/rail_48.inc"
};
static struct path_point rail_49_points[64] = {
#include "stg30/paths/rail_49_points.inc"
};
static struct path rail_49 = {
#include "stg30/paths/rail_49.inc"
};
static struct path_point rail_50_points[9] = {
#include "stg30/paths/rail_50_points.inc"
};
static struct path rail_50 = {
#include "stg30/paths/rail_50.inc"
};
static struct path_point rail_51_points[14] = {
#include "stg30/paths/rail_51_points.inc"
};
static struct path rail_51 = {
#include "stg30/paths/rail_51.inc"
};
static struct path_point rail_52_points[24] = {
#include "stg30/paths/rail_52_points.inc"
};
static struct path rail_52 = {
#include "stg30/paths/rail_52.inc"
};
static struct path_point rail_53_points[15] = {
#include "stg30/paths/rail_53_points.inc"
};
static struct path rail_53 = {
#include "stg30/paths/rail_53.inc"
};
static struct path_point rail_54_points[83] = {
#include "stg30/paths/rail_54_points.inc"
};
static struct path rail_54 = {
#include "stg30/paths/rail_54.inc"
};
static struct path_point rail_55_points[83] = {
#include "stg30/paths/rail_55_points.inc"
};
static struct path rail_55 = {
#include "stg30/paths/rail_55.inc"
};
static struct path_point rail_56_points[19] = {
#include "stg30/paths/rail_56_points.inc"
};
static struct path rail_56 = {
#include "stg30/paths/rail_56.inc"
};
static struct path_point rail_57_points[19] = {
#include "stg30/paths/rail_57_points.inc"
};
static struct path rail_57 = {
#include "stg30/paths/rail_57.inc"
};
static struct path_point rail_58_points[9] = {
#include "stg30/paths/rail_58_points.inc"
};
static struct path rail_58 = {
#include "stg30/paths/rail_58.inc"
};
static struct path_point rail_59_points[29] = {
#include "stg30/paths/rail_59_points.inc"
};
static struct path rail_59 = {
#include "stg30/paths/rail_59.inc"
};
static struct path_point rail_60_points[20] = {
#include "stg30/paths/rail_60_points.inc"
};
static struct path rail_60 = {
#include "stg30/paths/rail_60.inc"
};
static struct path_point rail_61_points[4] = {
#include "stg30/paths/rail_61_points.inc"
};
static struct path rail_61 = {
#include "stg30/paths/rail_61.inc"
};
struct path *rush_paths[62] = {
#include "stg30/paths/rush_paths.inc"
};
