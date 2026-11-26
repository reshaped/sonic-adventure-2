#include <ninja.h>
#include <sg_xpt.h>

#include "collision/world.h"
#include "stg27/scenery_land.h"

/* The background land references its archive and each texture in this pool. */
const char colony_lamp_land_filenames[25] = {
#include "stg27/lamp_land/colony_lamp_land_filenames.inc"
};

/* STG27.PRS's background land, texture lists and chunk models, from the
 * disc. its D lies between poison_land.c and sky_land.c */

/* the data up to the last that a pointer names ahead of its definition, as
 * tentative definitions in the disc's order: GCC places each datum at its
 * first definition */
static struct land_entry colony_lamp_land_entries[117];
struct land_table colony_lamp_land;
static NJS_TEXNAME colony_lamp_land_textures[1];
NJS_TEXLIST colony_lamp_texlist;
static Sint16 colony_lamp_land_draw_97_polygons[63];
static Sint32 colony_lamp_land_draw_97_vertices[35];
static NJS_CNK_MODEL colony_lamp_land_draw_97_model;
NJS_CNK_OBJECT colony_scenery_8;
static Sint16 colony_lamp_land_draw_50_polygons[51];
static Sint32 colony_lamp_land_draw_50_vertices[23];
static NJS_CNK_MODEL colony_lamp_land_draw_50_model;
NJS_CNK_OBJECT colony_scenery_5;
static Sint16 colony_lamp_land_draw_100_polygons[75];
static Sint32 colony_lamp_land_draw_100_vertices[51];
static NJS_CNK_MODEL colony_lamp_land_draw_100_model;
NJS_CNK_OBJECT colony_scenery_7;
static Sint16 colony_lamp_land_draw_5_polygons[29];
static Sint32 colony_lamp_land_draw_5_vertices[27];
static NJS_CNK_MODEL colony_lamp_land_draw_5_model;
NJS_CNK_OBJECT colony_scenery_6;
static Sint16 colony_lamp_land_draw_4_polygons[63];
static Sint32 colony_lamp_land_draw_4_vertices[35];
static NJS_CNK_MODEL colony_lamp_land_draw_4_model;
NJS_CNK_OBJECT colony_scenery_4;
static Sint16 colony_lamp_land_draw_85_polygons[29];
static Sint32 colony_lamp_land_draw_85_vertices[27];
static NJS_CNK_MODEL colony_lamp_land_draw_85_model;
NJS_CNK_OBJECT colony_scenery_3;
static Sint16 colony_lamp_land_draw_82_polygons[29];
static Sint32 colony_lamp_land_draw_82_vertices[27];
static NJS_CNK_MODEL colony_lamp_land_draw_82_model;
NJS_CNK_OBJECT colony_scenery_2;
static Sint16 colony_lamp_land_draw_58_polygons[29];
static Sint32 colony_lamp_land_draw_58_vertices[27];
static NJS_CNK_MODEL colony_lamp_land_draw_58_model;
NJS_CNK_OBJECT colony_scenery_1;
static Sint16 colony_lamp_land_draw_0_polygons[49];
static Sint32 colony_lamp_land_draw_0_vertices[35];
static NJS_CNK_MODEL colony_lamp_land_draw_0_model;
static NJS_CNK_OBJECT colony_lamp_land_draw_46;
static Sint16 colony_lamp_land_draw_3_polygons[67];
static Sint32 colony_lamp_land_draw_3_vertices[51];
static NJS_CNK_MODEL colony_lamp_land_draw_3_model;
static NJS_CNK_OBJECT colony_lamp_land_draw_45;
static Sint16 colony_lamp_land_draw_18_polygons[199];
static Sint32 colony_lamp_land_draw_18_vertices[155];
static NJS_CNK_MODEL colony_lamp_land_draw_18_model;
NJS_CNK_OBJECT colony_scenery_0;
static NJS_CNK_OBJECT colony_lamp_land_draw_113;
static NJS_CNK_OBJECT colony_lamp_land_draw_112;
static NJS_CNK_OBJECT colony_lamp_land_draw_111;
static NJS_CNK_OBJECT colony_lamp_land_draw_110;
static NJS_CNK_OBJECT colony_lamp_land_draw_109;
static NJS_CNK_OBJECT colony_lamp_land_draw_108;
static NJS_CNK_OBJECT colony_lamp_land_draw_107;
static NJS_CNK_OBJECT colony_lamp_land_draw_104;
static NJS_CNK_OBJECT colony_lamp_land_draw_103;
static NJS_CNK_OBJECT colony_lamp_land_draw_102;
static NJS_CNK_OBJECT colony_lamp_land_draw_101;
static NJS_CNK_OBJECT colony_lamp_land_draw_100;
static NJS_CNK_OBJECT colony_lamp_land_draw_99;
static NJS_CNK_OBJECT colony_lamp_land_draw_98;
static NJS_CNK_OBJECT colony_lamp_land_draw_97;
static NJS_CNK_OBJECT colony_lamp_land_draw_96;
static NJS_CNK_OBJECT colony_lamp_land_draw_95;
static NJS_CNK_OBJECT colony_lamp_land_draw_94;
static NJS_CNK_OBJECT colony_lamp_land_draw_93;
static NJS_CNK_OBJECT colony_lamp_land_draw_92;
static NJS_CNK_OBJECT colony_lamp_land_draw_91;
static NJS_CNK_OBJECT colony_lamp_land_draw_90;
static NJS_CNK_OBJECT colony_lamp_land_draw_89;
static NJS_CNK_OBJECT colony_lamp_land_draw_88;
static NJS_CNK_OBJECT colony_lamp_land_draw_87;
static NJS_CNK_OBJECT colony_lamp_land_draw_86;
static NJS_CNK_OBJECT colony_lamp_land_draw_84;
static NJS_CNK_OBJECT colony_lamp_land_draw_83;
static NJS_CNK_OBJECT colony_lamp_land_draw_81;
static NJS_CNK_OBJECT colony_lamp_land_draw_80;
static NJS_CNK_OBJECT colony_lamp_land_draw_79;
static NJS_CNK_OBJECT colony_lamp_land_draw_78;
static NJS_CNK_OBJECT colony_lamp_land_draw_77;
static NJS_CNK_OBJECT colony_lamp_land_draw_76;
static NJS_CNK_OBJECT colony_lamp_land_draw_75;
static NJS_CNK_OBJECT colony_lamp_land_draw_74;
static NJS_CNK_OBJECT colony_lamp_land_draw_73;
static NJS_CNK_OBJECT colony_lamp_land_draw_71;
static NJS_CNK_OBJECT colony_lamp_land_draw_70;
static NJS_CNK_OBJECT colony_lamp_land_draw_69;
static NJS_CNK_OBJECT colony_lamp_land_draw_68;
static NJS_CNK_OBJECT colony_lamp_land_draw_67;
static NJS_CNK_OBJECT colony_lamp_land_draw_66;
static NJS_CNK_OBJECT colony_lamp_land_draw_65;
static NJS_CNK_OBJECT colony_lamp_land_draw_64;
static NJS_CNK_OBJECT colony_lamp_land_draw_63;
static NJS_CNK_OBJECT colony_lamp_land_draw_62;
static NJS_CNK_OBJECT colony_lamp_land_draw_61;
static NJS_CNK_OBJECT colony_lamp_land_draw_60;
static NJS_CNK_OBJECT colony_lamp_land_draw_59;
static NJS_CNK_OBJECT colony_lamp_land_draw_58;
static NJS_CNK_OBJECT colony_lamp_land_draw_57;
static NJS_CNK_OBJECT colony_lamp_land_draw_56;
static NJS_CNK_OBJECT colony_lamp_land_draw_55;
static NJS_CNK_OBJECT colony_lamp_land_draw_54;
static NJS_CNK_OBJECT colony_lamp_land_draw_53;
static NJS_CNK_OBJECT colony_lamp_land_draw_52;
static NJS_CNK_OBJECT colony_lamp_land_draw_51;
static NJS_CNK_OBJECT colony_lamp_land_draw_50;
static NJS_CNK_OBJECT colony_lamp_land_draw_49;
static NJS_CNK_OBJECT colony_lamp_land_draw_48;
static NJS_CNK_OBJECT colony_lamp_land_draw_47;
static NJS_CNK_OBJECT colony_lamp_land_draw_44;
static NJS_CNK_OBJECT colony_lamp_land_draw_43;
static NJS_CNK_OBJECT colony_lamp_land_draw_42;
static NJS_CNK_OBJECT colony_lamp_land_draw_41;
static NJS_CNK_OBJECT colony_lamp_land_draw_40;
static NJS_CNK_OBJECT colony_lamp_land_draw_39;
static NJS_CNK_OBJECT colony_lamp_land_draw_38;
static NJS_CNK_OBJECT colony_lamp_land_draw_37;
static NJS_CNK_OBJECT colony_lamp_land_draw_36;
static NJS_CNK_OBJECT colony_lamp_land_draw_35;
static NJS_CNK_OBJECT colony_lamp_land_draw_34;
static NJS_CNK_OBJECT colony_lamp_land_draw_33;
static NJS_CNK_OBJECT colony_lamp_land_draw_32;
static NJS_CNK_OBJECT colony_lamp_land_draw_31;
static NJS_CNK_OBJECT colony_lamp_land_draw_30;
static NJS_CNK_OBJECT colony_lamp_land_draw_29;
static NJS_CNK_OBJECT colony_lamp_land_draw_28;
static NJS_CNK_OBJECT colony_lamp_land_draw_27;
static NJS_CNK_OBJECT colony_lamp_land_draw_26;
static NJS_CNK_OBJECT colony_lamp_land_draw_25;
static NJS_CNK_OBJECT colony_lamp_land_draw_24;
static NJS_CNK_OBJECT colony_lamp_land_draw_23;
static NJS_CNK_OBJECT colony_lamp_land_draw_22;
static NJS_CNK_OBJECT colony_lamp_land_draw_21;
static NJS_CNK_OBJECT colony_lamp_land_draw_20;
static NJS_CNK_OBJECT colony_lamp_land_draw_19;
static NJS_CNK_OBJECT colony_lamp_land_draw_17;
static NJS_CNK_OBJECT colony_lamp_land_draw_16;
static NJS_CNK_OBJECT colony_lamp_land_draw_15;
static NJS_CNK_OBJECT colony_lamp_land_draw_14;
static NJS_CNK_OBJECT colony_lamp_land_draw_13;
static NJS_CNK_OBJECT colony_lamp_land_draw_12;
static NJS_CNK_OBJECT colony_lamp_land_draw_11;
static NJS_CNK_OBJECT colony_lamp_land_draw_10;
static NJS_CNK_OBJECT colony_lamp_land_draw_9;
static NJS_CNK_OBJECT colony_lamp_land_draw_8;
static NJS_CNK_OBJECT colony_lamp_land_draw_7;
static NJS_CNK_OBJECT colony_lamp_land_draw_6;
static NJS_CNK_OBJECT colony_lamp_land_draw_5;
static NJS_CNK_OBJECT colony_lamp_land_draw_4;
static NJS_CNK_OBJECT colony_lamp_land_draw_3;
static NJS_CNK_OBJECT colony_lamp_land_draw_2;
static NJS_CNK_OBJECT colony_lamp_land_draw_1;
static NJS_CNK_OBJECT colony_lamp_land_draw_0;

static struct land_entry colony_lamp_land_entries[117] = {
#include "stg27/lamp_land/colony_lamp_land_entries.inc"
};
struct land_table colony_lamp_land = {
#include "stg27/lamp_land/colony_lamp_land.inc"
};
static NJS_TEXNAME colony_lamp_land_textures[1] = {
#include "stg27/lamp_land/colony_lamp_land_textures.inc"
};
NJS_TEXLIST colony_lamp_texlist = {
#include "stg27/lamp_land/colony_lamp_texlist.inc"
};
static Sint16 colony_lamp_land_draw_97_polygons[63] = {
#include "stg27/lamp_land/colony_lamp_land_draw_97_polygons.inc"
};
static Sint32 colony_lamp_land_draw_97_vertices[35] = {
#include "stg27/lamp_land/colony_lamp_land_draw_97_vertices.inc"
};
static NJS_CNK_MODEL colony_lamp_land_draw_97_model = {
#include "stg27/lamp_land/colony_lamp_land_draw_97_model.inc"
};
NJS_CNK_OBJECT colony_scenery_8 = {
#include "stg27/lamp_land/colony_scenery_8.inc"
};
static Sint16 colony_lamp_land_draw_50_polygons[51] = {
#include "stg27/lamp_land/colony_lamp_land_draw_50_polygons.inc"
};
static Sint32 colony_lamp_land_draw_50_vertices[23] = {
#include "stg27/lamp_land/colony_lamp_land_draw_50_vertices.inc"
};
static NJS_CNK_MODEL colony_lamp_land_draw_50_model = {
#include "stg27/lamp_land/colony_lamp_land_draw_50_model.inc"
};
NJS_CNK_OBJECT colony_scenery_5 = {
#include "stg27/lamp_land/colony_scenery_5.inc"
};
static Sint16 colony_lamp_land_draw_100_polygons[75] = {
#include "stg27/lamp_land/colony_lamp_land_draw_100_polygons.inc"
};
static Sint32 colony_lamp_land_draw_100_vertices[51] = {
#include "stg27/lamp_land/colony_lamp_land_draw_100_vertices.inc"
};
static NJS_CNK_MODEL colony_lamp_land_draw_100_model = {
#include "stg27/lamp_land/colony_lamp_land_draw_100_model.inc"
};
NJS_CNK_OBJECT colony_scenery_7 = {
#include "stg27/lamp_land/colony_scenery_7.inc"
};
static Sint16 colony_lamp_land_draw_5_polygons[29] = {
#include "stg27/lamp_land/colony_lamp_land_draw_5_polygons.inc"
};
static Sint32 colony_lamp_land_draw_5_vertices[27] = {
#include "stg27/lamp_land/colony_lamp_land_draw_5_vertices.inc"
};
static NJS_CNK_MODEL colony_lamp_land_draw_5_model = {
#include "stg27/lamp_land/colony_lamp_land_draw_5_model.inc"
};
NJS_CNK_OBJECT colony_scenery_6 = {
#include "stg27/lamp_land/colony_scenery_6.inc"
};
static Sint16 colony_lamp_land_draw_4_polygons[63] = {
#include "stg27/lamp_land/colony_lamp_land_draw_4_polygons.inc"
};
static Sint32 colony_lamp_land_draw_4_vertices[35] = {
#include "stg27/lamp_land/colony_lamp_land_draw_4_vertices.inc"
};
static NJS_CNK_MODEL colony_lamp_land_draw_4_model = {
#include "stg27/lamp_land/colony_lamp_land_draw_4_model.inc"
};
NJS_CNK_OBJECT colony_scenery_4 = {
#include "stg27/lamp_land/colony_scenery_4.inc"
};
static Sint16 colony_lamp_land_draw_85_polygons[29] = {
#include "stg27/lamp_land/colony_lamp_land_draw_85_polygons.inc"
};
static Sint32 colony_lamp_land_draw_85_vertices[27] = {
#include "stg27/lamp_land/colony_lamp_land_draw_85_vertices.inc"
};
static NJS_CNK_MODEL colony_lamp_land_draw_85_model = {
#include "stg27/lamp_land/colony_lamp_land_draw_85_model.inc"
};
NJS_CNK_OBJECT colony_scenery_3 = {
#include "stg27/lamp_land/colony_scenery_3.inc"
};
static Sint16 colony_lamp_land_draw_82_polygons[29] = {
#include "stg27/lamp_land/colony_lamp_land_draw_82_polygons.inc"
};
static Sint32 colony_lamp_land_draw_82_vertices[27] = {
#include "stg27/lamp_land/colony_lamp_land_draw_82_vertices.inc"
};
static NJS_CNK_MODEL colony_lamp_land_draw_82_model = {
#include "stg27/lamp_land/colony_lamp_land_draw_82_model.inc"
};
NJS_CNK_OBJECT colony_scenery_2 = {
#include "stg27/lamp_land/colony_scenery_2.inc"
};
static Sint16 colony_lamp_land_draw_58_polygons[29] = {
#include "stg27/lamp_land/colony_lamp_land_draw_58_polygons.inc"
};
static Sint32 colony_lamp_land_draw_58_vertices[27] = {
#include "stg27/lamp_land/colony_lamp_land_draw_58_vertices.inc"
};
static NJS_CNK_MODEL colony_lamp_land_draw_58_model = {
#include "stg27/lamp_land/colony_lamp_land_draw_58_model.inc"
};
NJS_CNK_OBJECT colony_scenery_1 = {
#include "stg27/lamp_land/colony_scenery_1.inc"
};
static Sint16 colony_lamp_land_draw_0_polygons[49] = {
#include "stg27/lamp_land/colony_lamp_land_draw_0_polygons.inc"
};
static Sint32 colony_lamp_land_draw_0_vertices[35] = {
#include "stg27/lamp_land/colony_lamp_land_draw_0_vertices.inc"
};
static NJS_CNK_MODEL colony_lamp_land_draw_0_model = {
#include "stg27/lamp_land/colony_lamp_land_draw_0_model.inc"
};
static NJS_CNK_OBJECT colony_lamp_land_draw_46 = {
#include "stg27/lamp_land/colony_lamp_land_draw_46.inc"
};
static Sint16 colony_lamp_land_draw_3_polygons[67] = {
#include "stg27/lamp_land/colony_lamp_land_draw_3_polygons.inc"
};
static Sint32 colony_lamp_land_draw_3_vertices[51] = {
#include "stg27/lamp_land/colony_lamp_land_draw_3_vertices.inc"
};
static NJS_CNK_MODEL colony_lamp_land_draw_3_model = {
#include "stg27/lamp_land/colony_lamp_land_draw_3_model.inc"
};
static NJS_CNK_OBJECT colony_lamp_land_draw_45 = {
#include "stg27/lamp_land/colony_lamp_land_draw_45.inc"
};
static Sint16 colony_lamp_land_draw_18_polygons[199] = {
#include "stg27/lamp_land/colony_lamp_land_draw_18_polygons.inc"
};
static Sint32 colony_lamp_land_draw_18_vertices[155] = {
#include "stg27/lamp_land/colony_lamp_land_draw_18_vertices.inc"
};
static NJS_CNK_MODEL colony_lamp_land_draw_18_model = {
#include "stg27/lamp_land/colony_lamp_land_draw_18_model.inc"
};
NJS_CNK_OBJECT colony_scenery_0 = {
#include "stg27/lamp_land/colony_scenery_0.inc"
};
static NJS_CNK_OBJECT colony_lamp_land_draw_113 = {
#include "stg27/lamp_land/colony_lamp_land_draw_113.inc"
};
static NJS_CNK_OBJECT colony_lamp_land_draw_112 = {
#include "stg27/lamp_land/colony_lamp_land_draw_112.inc"
};
static NJS_CNK_OBJECT colony_lamp_land_draw_111 = {
#include "stg27/lamp_land/colony_lamp_land_draw_111.inc"
};
static NJS_CNK_OBJECT colony_lamp_land_draw_110 = {
#include "stg27/lamp_land/colony_lamp_land_draw_110.inc"
};
static NJS_CNK_OBJECT colony_lamp_land_draw_109 = {
#include "stg27/lamp_land/colony_lamp_land_draw_109.inc"
};
static NJS_CNK_OBJECT colony_lamp_land_draw_108 = {
#include "stg27/lamp_land/colony_lamp_land_draw_108.inc"
};
static NJS_CNK_OBJECT colony_lamp_land_draw_107 = {
#include "stg27/lamp_land/colony_lamp_land_draw_107.inc"
};
static NJS_CNK_OBJECT colony_lamp_land_draw_104 = {
#include "stg27/lamp_land/colony_lamp_land_draw_104.inc"
};
static NJS_CNK_OBJECT colony_lamp_land_draw_103 = {
#include "stg27/lamp_land/colony_lamp_land_draw_103.inc"
};
static NJS_CNK_OBJECT colony_lamp_land_draw_102 = {
#include "stg27/lamp_land/colony_lamp_land_draw_102.inc"
};
static NJS_CNK_OBJECT colony_lamp_land_draw_101 = {
#include "stg27/lamp_land/colony_lamp_land_draw_101.inc"
};
static NJS_CNK_OBJECT colony_lamp_land_draw_100 = {
#include "stg27/lamp_land/colony_lamp_land_draw_100.inc"
};
static NJS_CNK_OBJECT colony_lamp_land_draw_99 = {
#include "stg27/lamp_land/colony_lamp_land_draw_99.inc"
};
static NJS_CNK_OBJECT colony_lamp_land_draw_98 = {
#include "stg27/lamp_land/colony_lamp_land_draw_98.inc"
};
static NJS_CNK_OBJECT colony_lamp_land_draw_97 = {
#include "stg27/lamp_land/colony_lamp_land_draw_97.inc"
};
static NJS_CNK_OBJECT colony_lamp_land_draw_96 = {
#include "stg27/lamp_land/colony_lamp_land_draw_96.inc"
};
static NJS_CNK_OBJECT colony_lamp_land_draw_95 = {
#include "stg27/lamp_land/colony_lamp_land_draw_95.inc"
};
static NJS_CNK_OBJECT colony_lamp_land_draw_94 = {
#include "stg27/lamp_land/colony_lamp_land_draw_94.inc"
};
static NJS_CNK_OBJECT colony_lamp_land_draw_93 = {
#include "stg27/lamp_land/colony_lamp_land_draw_93.inc"
};
static NJS_CNK_OBJECT colony_lamp_land_draw_92 = {
#include "stg27/lamp_land/colony_lamp_land_draw_92.inc"
};
static NJS_CNK_OBJECT colony_lamp_land_draw_91 = {
#include "stg27/lamp_land/colony_lamp_land_draw_91.inc"
};
static NJS_CNK_OBJECT colony_lamp_land_draw_90 = {
#include "stg27/lamp_land/colony_lamp_land_draw_90.inc"
};
static NJS_CNK_OBJECT colony_lamp_land_draw_89 = {
#include "stg27/lamp_land/colony_lamp_land_draw_89.inc"
};
static NJS_CNK_OBJECT colony_lamp_land_draw_88 = {
#include "stg27/lamp_land/colony_lamp_land_draw_88.inc"
};
static NJS_CNK_OBJECT colony_lamp_land_draw_87 = {
#include "stg27/lamp_land/colony_lamp_land_draw_87.inc"
};
static NJS_CNK_OBJECT colony_lamp_land_draw_86 = {
#include "stg27/lamp_land/colony_lamp_land_draw_86.inc"
};
static NJS_CNK_OBJECT colony_lamp_land_draw_84 = {
#include "stg27/lamp_land/colony_lamp_land_draw_84.inc"
};
static NJS_CNK_OBJECT colony_lamp_land_draw_83 = {
#include "stg27/lamp_land/colony_lamp_land_draw_83.inc"
};
static NJS_CNK_OBJECT colony_lamp_land_draw_81 = {
#include "stg27/lamp_land/colony_lamp_land_draw_81.inc"
};
static NJS_CNK_OBJECT colony_lamp_land_draw_80 = {
#include "stg27/lamp_land/colony_lamp_land_draw_80.inc"
};
static NJS_CNK_OBJECT colony_lamp_land_draw_79 = {
#include "stg27/lamp_land/colony_lamp_land_draw_79.inc"
};
static NJS_CNK_OBJECT colony_lamp_land_draw_78 = {
#include "stg27/lamp_land/colony_lamp_land_draw_78.inc"
};
static NJS_CNK_OBJECT colony_lamp_land_draw_77 = {
#include "stg27/lamp_land/colony_lamp_land_draw_77.inc"
};
static NJS_CNK_OBJECT colony_lamp_land_draw_76 = {
#include "stg27/lamp_land/colony_lamp_land_draw_76.inc"
};
static NJS_CNK_OBJECT colony_lamp_land_draw_75 = {
#include "stg27/lamp_land/colony_lamp_land_draw_75.inc"
};
static NJS_CNK_OBJECT colony_lamp_land_draw_74 = {
#include "stg27/lamp_land/colony_lamp_land_draw_74.inc"
};
static NJS_CNK_OBJECT colony_lamp_land_draw_73 = {
#include "stg27/lamp_land/colony_lamp_land_draw_73.inc"
};
static NJS_CNK_OBJECT colony_lamp_land_draw_71 = {
#include "stg27/lamp_land/colony_lamp_land_draw_71.inc"
};
static NJS_CNK_OBJECT colony_lamp_land_draw_70 = {
#include "stg27/lamp_land/colony_lamp_land_draw_70.inc"
};
static NJS_CNK_OBJECT colony_lamp_land_draw_69 = {
#include "stg27/lamp_land/colony_lamp_land_draw_69.inc"
};
static NJS_CNK_OBJECT colony_lamp_land_draw_68 = {
#include "stg27/lamp_land/colony_lamp_land_draw_68.inc"
};
static NJS_CNK_OBJECT colony_lamp_land_draw_67 = {
#include "stg27/lamp_land/colony_lamp_land_draw_67.inc"
};
static NJS_CNK_OBJECT colony_lamp_land_draw_66 = {
#include "stg27/lamp_land/colony_lamp_land_draw_66.inc"
};
static NJS_CNK_OBJECT colony_lamp_land_draw_65 = {
#include "stg27/lamp_land/colony_lamp_land_draw_65.inc"
};
static NJS_CNK_OBJECT colony_lamp_land_draw_64 = {
#include "stg27/lamp_land/colony_lamp_land_draw_64.inc"
};
static NJS_CNK_OBJECT colony_lamp_land_draw_63 = {
#include "stg27/lamp_land/colony_lamp_land_draw_63.inc"
};
static NJS_CNK_OBJECT colony_lamp_land_draw_62 = {
#include "stg27/lamp_land/colony_lamp_land_draw_62.inc"
};
static NJS_CNK_OBJECT colony_lamp_land_draw_61 = {
#include "stg27/lamp_land/colony_lamp_land_draw_61.inc"
};
static NJS_CNK_OBJECT colony_lamp_land_draw_60 = {
#include "stg27/lamp_land/colony_lamp_land_draw_60.inc"
};
static NJS_CNK_OBJECT colony_lamp_land_draw_59 = {
#include "stg27/lamp_land/colony_lamp_land_draw_59.inc"
};
static NJS_CNK_OBJECT colony_lamp_land_draw_58 = {
#include "stg27/lamp_land/colony_lamp_land_draw_58.inc"
};
static NJS_CNK_OBJECT colony_lamp_land_draw_57 = {
#include "stg27/lamp_land/colony_lamp_land_draw_57.inc"
};
static NJS_CNK_OBJECT colony_lamp_land_draw_56 = {
#include "stg27/lamp_land/colony_lamp_land_draw_56.inc"
};
static NJS_CNK_OBJECT colony_lamp_land_draw_55 = {
#include "stg27/lamp_land/colony_lamp_land_draw_55.inc"
};
static NJS_CNK_OBJECT colony_lamp_land_draw_54 = {
#include "stg27/lamp_land/colony_lamp_land_draw_54.inc"
};
static NJS_CNK_OBJECT colony_lamp_land_draw_53 = {
#include "stg27/lamp_land/colony_lamp_land_draw_53.inc"
};
static NJS_CNK_OBJECT colony_lamp_land_draw_52 = {
#include "stg27/lamp_land/colony_lamp_land_draw_52.inc"
};
static NJS_CNK_OBJECT colony_lamp_land_draw_51 = {
#include "stg27/lamp_land/colony_lamp_land_draw_51.inc"
};
static NJS_CNK_OBJECT colony_lamp_land_draw_50 = {
#include "stg27/lamp_land/colony_lamp_land_draw_50.inc"
};
static NJS_CNK_OBJECT colony_lamp_land_draw_49 = {
#include "stg27/lamp_land/colony_lamp_land_draw_49.inc"
};
static NJS_CNK_OBJECT colony_lamp_land_draw_48 = {
#include "stg27/lamp_land/colony_lamp_land_draw_48.inc"
};
static NJS_CNK_OBJECT colony_lamp_land_draw_47 = {
#include "stg27/lamp_land/colony_lamp_land_draw_47.inc"
};
static NJS_CNK_OBJECT colony_lamp_land_draw_44 = {
#include "stg27/lamp_land/colony_lamp_land_draw_44.inc"
};
static NJS_CNK_OBJECT colony_lamp_land_draw_43 = {
#include "stg27/lamp_land/colony_lamp_land_draw_43.inc"
};
static NJS_CNK_OBJECT colony_lamp_land_draw_42 = {
#include "stg27/lamp_land/colony_lamp_land_draw_42.inc"
};
static NJS_CNK_OBJECT colony_lamp_land_draw_41 = {
#include "stg27/lamp_land/colony_lamp_land_draw_41.inc"
};
static NJS_CNK_OBJECT colony_lamp_land_draw_40 = {
#include "stg27/lamp_land/colony_lamp_land_draw_40.inc"
};
static NJS_CNK_OBJECT colony_lamp_land_draw_39 = {
#include "stg27/lamp_land/colony_lamp_land_draw_39.inc"
};
static NJS_CNK_OBJECT colony_lamp_land_draw_38 = {
#include "stg27/lamp_land/colony_lamp_land_draw_38.inc"
};
static NJS_CNK_OBJECT colony_lamp_land_draw_37 = {
#include "stg27/lamp_land/colony_lamp_land_draw_37.inc"
};
static NJS_CNK_OBJECT colony_lamp_land_draw_36 = {
#include "stg27/lamp_land/colony_lamp_land_draw_36.inc"
};
static NJS_CNK_OBJECT colony_lamp_land_draw_35 = {
#include "stg27/lamp_land/colony_lamp_land_draw_35.inc"
};
static NJS_CNK_OBJECT colony_lamp_land_draw_34 = {
#include "stg27/lamp_land/colony_lamp_land_draw_34.inc"
};
static NJS_CNK_OBJECT colony_lamp_land_draw_33 = {
#include "stg27/lamp_land/colony_lamp_land_draw_33.inc"
};
static NJS_CNK_OBJECT colony_lamp_land_draw_32 = {
#include "stg27/lamp_land/colony_lamp_land_draw_32.inc"
};
static NJS_CNK_OBJECT colony_lamp_land_draw_31 = {
#include "stg27/lamp_land/colony_lamp_land_draw_31.inc"
};
static NJS_CNK_OBJECT colony_lamp_land_draw_30 = {
#include "stg27/lamp_land/colony_lamp_land_draw_30.inc"
};
static NJS_CNK_OBJECT colony_lamp_land_draw_29 = {
#include "stg27/lamp_land/colony_lamp_land_draw_29.inc"
};
static NJS_CNK_OBJECT colony_lamp_land_draw_28 = {
#include "stg27/lamp_land/colony_lamp_land_draw_28.inc"
};
static NJS_CNK_OBJECT colony_lamp_land_draw_27 = {
#include "stg27/lamp_land/colony_lamp_land_draw_27.inc"
};
static NJS_CNK_OBJECT colony_lamp_land_draw_26 = {
#include "stg27/lamp_land/colony_lamp_land_draw_26.inc"
};
static NJS_CNK_OBJECT colony_lamp_land_draw_25 = {
#include "stg27/lamp_land/colony_lamp_land_draw_25.inc"
};
static NJS_CNK_OBJECT colony_lamp_land_draw_24 = {
#include "stg27/lamp_land/colony_lamp_land_draw_24.inc"
};
static NJS_CNK_OBJECT colony_lamp_land_draw_23 = {
#include "stg27/lamp_land/colony_lamp_land_draw_23.inc"
};
static NJS_CNK_OBJECT colony_lamp_land_draw_22 = {
#include "stg27/lamp_land/colony_lamp_land_draw_22.inc"
};
static NJS_CNK_OBJECT colony_lamp_land_draw_21 = {
#include "stg27/lamp_land/colony_lamp_land_draw_21.inc"
};
static NJS_CNK_OBJECT colony_lamp_land_draw_20 = {
#include "stg27/lamp_land/colony_lamp_land_draw_20.inc"
};
static NJS_CNK_OBJECT colony_lamp_land_draw_19 = {
#include "stg27/lamp_land/colony_lamp_land_draw_19.inc"
};
static NJS_CNK_OBJECT colony_lamp_land_draw_17 = {
#include "stg27/lamp_land/colony_lamp_land_draw_17.inc"
};
static NJS_CNK_OBJECT colony_lamp_land_draw_16 = {
#include "stg27/lamp_land/colony_lamp_land_draw_16.inc"
};
static NJS_CNK_OBJECT colony_lamp_land_draw_15 = {
#include "stg27/lamp_land/colony_lamp_land_draw_15.inc"
};
static NJS_CNK_OBJECT colony_lamp_land_draw_14 = {
#include "stg27/lamp_land/colony_lamp_land_draw_14.inc"
};
static NJS_CNK_OBJECT colony_lamp_land_draw_13 = {
#include "stg27/lamp_land/colony_lamp_land_draw_13.inc"
};
static NJS_CNK_OBJECT colony_lamp_land_draw_12 = {
#include "stg27/lamp_land/colony_lamp_land_draw_12.inc"
};
static NJS_CNK_OBJECT colony_lamp_land_draw_11 = {
#include "stg27/lamp_land/colony_lamp_land_draw_11.inc"
};
static NJS_CNK_OBJECT colony_lamp_land_draw_10 = {
#include "stg27/lamp_land/colony_lamp_land_draw_10.inc"
};
static NJS_CNK_OBJECT colony_lamp_land_draw_9 = {
#include "stg27/lamp_land/colony_lamp_land_draw_9.inc"
};
static NJS_CNK_OBJECT colony_lamp_land_draw_8 = {
#include "stg27/lamp_land/colony_lamp_land_draw_8.inc"
};
static NJS_CNK_OBJECT colony_lamp_land_draw_7 = {
#include "stg27/lamp_land/colony_lamp_land_draw_7.inc"
};
static NJS_CNK_OBJECT colony_lamp_land_draw_6 = {
#include "stg27/lamp_land/colony_lamp_land_draw_6.inc"
};
static NJS_CNK_OBJECT colony_lamp_land_draw_5 = {
#include "stg27/lamp_land/colony_lamp_land_draw_5.inc"
};
static NJS_CNK_OBJECT colony_lamp_land_draw_4 = {
#include "stg27/lamp_land/colony_lamp_land_draw_4.inc"
};
static NJS_CNK_OBJECT colony_lamp_land_draw_3 = {
#include "stg27/lamp_land/colony_lamp_land_draw_3.inc"
};
static NJS_CNK_OBJECT colony_lamp_land_draw_2 = {
#include "stg27/lamp_land/colony_lamp_land_draw_2.inc"
};
static NJS_CNK_OBJECT colony_lamp_land_draw_1 = {
#include "stg27/lamp_land/colony_lamp_land_draw_1.inc"
};
static NJS_CNK_OBJECT colony_lamp_land_draw_0 = {
#include "stg27/lamp_land/colony_lamp_land_draw_0.inc"
};
