#include <ninja.h>
#include <sg_xpt.h>

#include "collision/world.h"
#include "stg22/stage_data.h"

/* the land's file names, which its land table and texture list point at. each
 * starts on a 4-byte boundary */
const char crazy_gadget_land_filenames[752] = {
#include "stg22/terrain/crazy_gadget_land_filenames.inc"
};

/* STG22.PRS's land, texture lists, chunk models and Basic models, from the
 * disc. its D lies between background.c and object_textures.c */

/* the data up to the last that a pointer names ahead of its definition, as
 * tentative definitions in the disc's order: GCC places each datum at its
 * first definition */
static struct land_entry entries[1291];
struct land_table crazy_gadget_land;
static NJS_TEXNAME textures[52];
NJS_TEXLIST gadget_land_texlist;
static Sint16 draw_211_polygons[1339];
static Sint32 draw_211_vertices[1463];
static NJS_CNK_MODEL draw_211_model;
static NJS_CNK_OBJECT draw_975;
static Sint16 draw_294_polygons[327];
static Sint32 draw_294_vertices[367];
static NJS_CNK_MODEL draw_294_model;
static NJS_CNK_OBJECT draw_973;
static Sint16 draw_972_polygons[829];
static Sint32 draw_972_vertices[795];
static NJS_CNK_MODEL draw_972_model;
static NJS_CNK_OBJECT draw_972;
static Sint16 draw_212_polygons[1053];
static Sint32 draw_212_vertices[563];
static NJS_CNK_MODEL draw_212_model;
static NJS_CNK_OBJECT draw_971;
static Sint16 draw_267_polygons[1271];
static Sint32 draw_267_vertices[1059];
static NJS_CNK_MODEL draw_267_model;
static NJS_CNK_OBJECT draw_968;
static Sint16 draw_265_polygons[147];
static Sint32 draw_265_vertices[139];
static NJS_CNK_MODEL draw_265_model;
static NJS_CNK_OBJECT draw_967;
static Sint16 draw_262_polygons[371];
static Sint32 draw_262_vertices[323];
static NJS_CNK_MODEL draw_262_model;
static NJS_CNK_OBJECT draw_963;
static Sint16 draw_207_polygons[941];
static Sint32 draw_207_vertices[563];
static NJS_CNK_MODEL draw_207_model;
static NJS_CNK_OBJECT draw_961;
static Sint16 draw_957_polygons[109];
static Sint32 draw_957_vertices[51];
static NJS_CNK_MODEL draw_957_model;
static NJS_CNK_OBJECT draw_957;
static Sint16 draw_578_polygons[805];
static Sint32 draw_578_vertices[647];
static NJS_CNK_MODEL draw_578_model;
static NJS_CNK_OBJECT draw_956;
static Sint16 draw_280_polygons[1545];
static Sint32 draw_280_vertices[479];
static NJS_CNK_MODEL draw_280_model;
static NJS_CNK_OBJECT draw_955;
static Sint16 draw_278_polygons[1545];
static Sint32 draw_278_vertices[479];
static NJS_CNK_MODEL draw_278_model;
static NJS_CNK_OBJECT draw_954;
static Sint16 draw_283_polygons[1939];
static Sint32 draw_283_vertices[2139];
static NJS_CNK_MODEL draw_283_model;
static NJS_CNK_OBJECT draw_953;
static Sint16 draw_564_polygons[877];
static Sint32 draw_564_vertices[555];
static NJS_CNK_MODEL draw_564_model;
static NJS_CNK_OBJECT draw_951;
static Sint16 draw_580_polygons[167];
static Sint32 draw_580_vertices[151];
static NJS_CNK_MODEL draw_580_model;
static NJS_CNK_OBJECT draw_950;
static Sint16 draw_574_polygons[55];
static Sint32 draw_574_vertices[51];
static NJS_CNK_MODEL draw_574_model;
static NJS_CNK_OBJECT draw_949;
static Sint16 draw_573_polygons[167];
static Sint32 draw_573_vertices[159];
static NJS_CNK_MODEL draw_573_model;
static NJS_CNK_OBJECT draw_945;
static Sint16 draw_558_polygons[317];
static Sint32 draw_558_vertices[347];
static NJS_CNK_MODEL draw_558_model;
static NJS_CNK_OBJECT draw_943;
static Sint16 draw_427_polygons[861];
static Sint32 draw_427_vertices[659];
static NJS_CNK_MODEL draw_427_model;
static NJS_CNK_OBJECT draw_940;
static Sint16 draw_565_polygons[675];
static Sint32 draw_565_vertices[531];
static NJS_CNK_MODEL draw_565_model;
static NJS_CNK_OBJECT draw_937;
static Sint16 draw_552_polygons[149];
static Sint32 draw_552_vertices[163];
static NJS_CNK_MODEL draw_552_model;
static NJS_CNK_OBJECT draw_933;
static Sint16 draw_550_polygons[221];
static Sint32 draw_550_vertices[243];
static NJS_CNK_MODEL draw_550_model;
static NJS_CNK_OBJECT draw_926;
static Sint16 draw_206_polygons[383];
static Sint32 draw_206_vertices[355];
static NJS_CNK_MODEL draw_206_model;
static NJS_CNK_OBJECT draw_924;
static Sint16 draw_266_polygons[355];
static Sint32 draw_266_vertices[283];
static NJS_CNK_MODEL draw_266_model;
static NJS_CNK_OBJECT draw_919;
static Sint16 draw_370_polygons[57];
static Sint32 draw_370_vertices[35];
static NJS_CNK_MODEL draw_370_model;
static NJS_CNK_OBJECT draw_902;
static Sint16 draw_259_polygons[565];
static Sint32 draw_259_vertices[515];
static NJS_CNK_MODEL draw_259_model;
static NJS_CNK_OBJECT draw_893;
static Sint16 draw_257_polygons[1967];
static Sint32 draw_257_vertices[1187];
static NJS_CNK_MODEL draw_257_model;
static NJS_CNK_OBJECT draw_892;
static Sint16 draw_256_polygons[1357];
static Sint32 draw_256_vertices[1331];
static NJS_CNK_MODEL draw_256_model;
static NJS_CNK_OBJECT draw_891;
static Sint16 draw_842_polygons[539];
static Sint32 draw_842_vertices[483];
static NJS_CNK_MODEL draw_842_model;
static NJS_CNK_OBJECT draw_889;
static Sint16 draw_333_polygons[331];
static Sint32 draw_333_vertices[291];
static NJS_CNK_MODEL draw_333_model;
static NJS_CNK_OBJECT draw_888;
static Sint16 draw_854_polygons[651];
static Sint32 draw_854_vertices[1263];
static NJS_CNK_MODEL draw_854_model;
static NJS_CNK_OBJECT draw_854;
static Sint16 draw_818_polygons[539];
static Sint32 draw_818_vertices[483];
static NJS_CNK_MODEL draw_818_model;
static NJS_CNK_OBJECT draw_841;
static Sint16 draw_220_polygons[591];
static Sint32 draw_220_vertices[635];
static NJS_CNK_MODEL draw_220_model;
static NJS_CNK_OBJECT draw_817;
static Sint16 draw_815_polygons[163];
static Sint32 draw_815_vertices[163];
static NJS_CNK_MODEL draw_815_model;
static NJS_CNK_OBJECT draw_815;
static Sint16 draw_458_polygons[323];
static Sint32 draw_458_vertices[363];
static NJS_CNK_MODEL draw_458_model;
static NJS_CNK_OBJECT draw_812;
static Sint16 draw_452_polygons[163];
static Sint32 draw_452_vertices[163];
static NJS_CNK_MODEL draw_452_model;
static NJS_CNK_OBJECT draw_810;
static Sint16 draw_451_polygons[79];
static Sint32 draw_451_vertices[83];
static NJS_CNK_MODEL draw_451_model;
static NJS_CNK_OBJECT draw_805;
static Sint16 draw_418_polygons[211];
static Sint32 draw_418_vertices[235];
static NJS_CNK_MODEL draw_418_model;
static NJS_CNK_OBJECT draw_803;
static Sint16 draw_450_polygons[273];
static Sint32 draw_450_vertices[243];
static NJS_CNK_MODEL draw_450_model;
static NJS_CNK_OBJECT draw_802;
static Sint16 draw_796_polygons[163];
static Sint32 draw_796_vertices[163];
static NJS_CNK_MODEL draw_796_model;
static NJS_CNK_OBJECT draw_796;
static Sint16 draw_443_polygons[227];
static Sint32 draw_443_vertices[171];
static NJS_CNK_MODEL draw_443_model;
static NJS_CNK_OBJECT draw_795;
static Sint16 draw_460_polygons[339];
static Sint32 draw_460_vertices[251];
static NJS_CNK_MODEL draw_460_model;
static NJS_CNK_OBJECT draw_794;
static Sint16 draw_441_polygons[301];
static Sint32 draw_441_vertices[207];
static NJS_CNK_MODEL draw_441_model;
static NJS_CNK_OBJECT draw_793;
static Sint16 draw_440_polygons[69];
static Sint32 draw_440_vertices[51];
static NJS_CNK_MODEL draw_440_model;
static NJS_CNK_OBJECT draw_792;
static Sint16 draw_439_polygons[135];
static Sint32 draw_439_vertices[123];
static NJS_CNK_MODEL draw_439_model;
static NJS_CNK_OBJECT draw_791;
static Sint16 draw_438_polygons[533];
static Sint32 draw_438_vertices[411];
static NJS_CNK_MODEL draw_438_model;
static NJS_CNK_OBJECT draw_790;
static Sint16 draw_424_polygons[373];
static Sint32 draw_424_vertices[291];
static NJS_CNK_MODEL draw_424_model;
static NJS_CNK_OBJECT draw_789;
static Sint16 draw_426_polygons[389];
static Sint32 draw_426_vertices[391];
static NJS_CNK_MODEL draw_426_model;
static NJS_CNK_OBJECT draw_779;
static Sint16 draw_425_polygons[389];
static Sint32 draw_425_vertices[391];
static NJS_CNK_MODEL draw_425_model;
static NJS_CNK_OBJECT draw_778;
static Sint16 draw_416_polygons[149];
static Sint32 draw_416_vertices[159];
static NJS_CNK_MODEL draw_416_model;
static NJS_CNK_OBJECT draw_776;
static Sint16 draw_773_polygons[239];
static Sint32 draw_773_vertices[203];
static NJS_CNK_MODEL draw_773_model;
static NJS_CNK_OBJECT draw_773;
static Sint16 draw_422_polygons[439];
static Sint32 draw_422_vertices[243];
static NJS_CNK_MODEL draw_422_model;
static NJS_CNK_OBJECT draw_772;
static Sint16 draw_367_polygons[295];
static Sint32 draw_367_vertices[131];
static NJS_CNK_MODEL draw_367_model;
static NJS_CNK_OBJECT draw_770;
static Sint16 draw_369_polygons[239];
static Sint32 draw_369_vertices[259];
static NJS_CNK_MODEL draw_369_model;
static NJS_CNK_OBJECT draw_766;
static Sint16 draw_402_polygons[799];
static Sint32 draw_402_vertices[643];
static NJS_CNK_MODEL draw_402_model;
static NJS_CNK_OBJECT draw_765;
static Sint16 draw_757_polygons[37];
static Sint32 draw_757_vertices[35];
static NJS_CNK_MODEL draw_757_model;
static NJS_CNK_OBJECT draw_760;
static Sint16 draw_756_polygons[281];
static Sint32 draw_756_vertices[179];
static NJS_CNK_MODEL draw_756_model;
static NJS_CNK_OBJECT draw_756;
static Sint16 draw_234_polygons[247];
static Sint32 draw_234_vertices[183];
NJS_CNK_MODEL gadget_pulse_marker_a;
static NJS_CNK_OBJECT draw_755;
static Sint16 draw_754_polygons[145];
static Sint32 draw_754_vertices[91];
static NJS_CNK_MODEL draw_754_model;
static NJS_CNK_OBJECT draw_754;
static Sint16 draw_694_polygons[63];
static Sint32 draw_694_vertices[35];
static NJS_CNK_MODEL draw_694_model;
static NJS_CNK_OBJECT draw_694;
static Sint16 draw_641_polygons[117];
static Sint32 draw_641_vertices[67];
static NJS_CNK_MODEL draw_641_model;
static NJS_CNK_OBJECT draw_641;
static Sint16 draw_226_polygons[169];
static Sint32 draw_226_vertices[123];
NJS_CNK_MODEL gadget_pulse_marker_b;
static NJS_CNK_OBJECT draw_640;
static Sint16 draw_638_polygons[147];
static Sint32 draw_638_vertices[83];
static NJS_CNK_MODEL draw_638_model;
static NJS_CNK_OBJECT draw_638;
static Sint16 draw_636_polygons[323];
static Sint32 draw_636_vertices[179];
static NJS_CNK_MODEL draw_636_model;
static NJS_CNK_OBJECT draw_636;
static Sint16 draw_635_polygons[121];
static Sint32 draw_635_vertices[67];
static NJS_CNK_MODEL draw_635_model;
static NJS_CNK_OBJECT draw_635;
static Sint16 draw_634_polygons[103];
static Sint32 draw_634_vertices[51];
static NJS_CNK_MODEL draw_634_model;
static NJS_CNK_OBJECT draw_634;
static Sint16 draw_633_polygons[705];
static Sint32 draw_633_vertices[547];
static NJS_CNK_MODEL draw_633_model;
static NJS_CNK_OBJECT draw_633;
static Sint16 draw_629_polygons[43];
static Sint32 draw_629_vertices[43];
static NJS_CNK_MODEL draw_629_model;
static NJS_CNK_OBJECT draw_632;
static Sint16 draw_329_polygons[123];
static Sint32 draw_329_vertices[51];
static NJS_CNK_MODEL draw_329_model;
static NJS_CNK_OBJECT draw_628;
static Sint16 draw_252_polygons[619];
static Sint32 draw_252_vertices[475];
static NJS_CNK_MODEL draw_252_model;
static NJS_CNK_OBJECT draw_608;
static Sint16 draw_251_polygons[135];
static Sint32 draw_251_vertices[83];
static NJS_CNK_MODEL draw_251_model;
static NJS_CNK_OBJECT draw_603;
static Sint16 draw_250_polygons[227];
static Sint32 draw_250_vertices[167];
static NJS_CNK_MODEL draw_250_model;
static NJS_CNK_OBJECT draw_602;
static Sint16 draw_249_polygons[111];
static Sint32 draw_249_vertices[91];
static NJS_CNK_MODEL draw_249_model;
static NJS_CNK_OBJECT draw_601;
static Sint16 draw_219_polygons[173];
static Sint32 draw_219_vertices[99];
static NJS_CNK_MODEL draw_219_model;
static NJS_CNK_OBJECT draw_600;
static Sint16 draw_546_polygons[25];
static Sint32 draw_546_vertices[19];
static NJS_CNK_MODEL draw_546_model;
static NJS_CNK_OBJECT draw_546;
static Sint16 draw_317_polygons[955];
static Sint32 draw_317_vertices[867];
static NJS_CNK_MODEL draw_317_model;
static NJS_CNK_OBJECT draw_544;
static Sint16 draw_515_polygons[789];
static Sint32 draw_515_vertices[1543];
static NJS_CNK_MODEL draw_515_model;
static NJS_CNK_OBJECT draw_515;
static Sint16 draw_305_polygons[955];
static Sint32 draw_305_vertices[867];
static NJS_CNK_MODEL draw_305_model;
static NJS_CNK_OBJECT draw_503;
static Sint16 draw_442_polygons[289];
static Sint32 draw_442_vertices[323];
static NJS_CNK_MODEL draw_442_model;
static NJS_CNK_OBJECT draw_442;
static Sint16 draw_413_polygons[81];
static Sint32 draw_413_vertices[59];
static NJS_CNK_MODEL draw_413_model;
static NJS_CNK_OBJECT draw_413;
static Sint16 draw_390_polygons[39];
static Sint32 draw_390_vertices[27];
static NJS_CNK_MODEL draw_390_model;
static NJS_CNK_OBJECT draw_391;
static Sint16 draw_372_polygons[1271];
static Sint32 draw_372_vertices[1059];
static NJS_CNK_MODEL draw_372_model;
static NJS_CNK_OBJECT draw_372;
static Sint16 draw_359_polygons[273];
static Sint32 draw_359_vertices[259];
static NJS_CNK_MODEL draw_359_model;
static NJS_CNK_OBJECT draw_359;
static Sint16 draw_347_polygons[747];
static Sint32 draw_347_vertices[675];
static NJS_CNK_MODEL draw_347_model;
static NJS_CNK_OBJECT draw_347;
static Sint16 draw_346_polygons[747];
static Sint32 draw_346_vertices[675];
static NJS_CNK_MODEL draw_346_model;
static NJS_CNK_OBJECT draw_346;
static Sint16 draw_331_polygons[123];
static Sint32 draw_331_vertices[67];
static NJS_CNK_MODEL draw_331_model;
static NJS_CNK_OBJECT draw_332;
static Sint16 draw_285_polygons[1015];
static Sint32 draw_285_vertices[835];
static NJS_CNK_MODEL draw_285_model;
static NJS_CNK_OBJECT draw_285;
static Sint16 draw_258_polygons[411];
static Sint32 draw_258_vertices[291];
static NJS_CNK_MODEL draw_258_model;
static NJS_CNK_OBJECT draw_258;
static Sint16 draw_248_polygons[165];
static Sint32 draw_248_vertices[123];
static NJS_CNK_MODEL draw_248_model;
static NJS_CNK_OBJECT draw_248;
static Sint16 draw_238_polygons[579];
static Sint32 draw_238_vertices[487];
static NJS_CNK_MODEL draw_238_model;
static NJS_CNK_OBJECT draw_238;
static Sint16 draw_229_polygons[519];
static Sint32 draw_229_vertices[499];
static NJS_CNK_MODEL draw_229_model;
static NJS_CNK_OBJECT draw_229;
static Sint16 draw_28_polygons[229];
static Sint32 draw_28_vertices[211];
static NJS_CNK_MODEL draw_28_model;
static NJS_CNK_OBJECT draw_205;
static Sint16 draw_200_polygons[37];
static Sint32 draw_200_vertices[35];
static NJS_CNK_MODEL draw_200_model;
static NJS_CNK_OBJECT draw_203;
static Sint16 draw_26_polygons[829];
static Sint32 draw_26_vertices[795];
static NJS_CNK_MODEL draw_26_model;
static NJS_CNK_OBJECT draw_27;
static Sint16 draw_22_polygons[37];
static Sint32 draw_22_vertices[35];
static NJS_CNK_MODEL draw_22_model;
static NJS_CNK_OBJECT draw_25;
static Sint16 draw_18_polygons[37];
static Sint32 draw_18_vertices[35];
static NJS_CNK_MODEL draw_18_model;
static NJS_CNK_OBJECT draw_21;
static Sint16 draw_10_polygons[203];
static Sint32 draw_10_vertices[135];
static NJS_CNK_MODEL draw_10_model;
static NJS_CNK_OBJECT draw_17;
static Sint16 draw_9_polygons[691];
static Sint32 draw_9_vertices[355];
static NJS_CNK_MODEL draw_9_model;
static NJS_CNK_OBJECT draw_9;
static Sint16 draw_8_polygons[1573];
static Sint32 draw_8_vertices[875];
static NJS_CNK_MODEL draw_8_model;
static NJS_CNK_OBJECT draw_8;
static Sint16 draw_7_polygons[1599];
static Sint32 draw_7_vertices[887];
static NJS_CNK_MODEL draw_7_model;
static NJS_CNK_OBJECT draw_7;
static Sint16 draw_6_polygons[1551];
static Sint32 draw_6_vertices[795];
static NJS_CNK_MODEL draw_6_model;
static NJS_CNK_OBJECT draw_6;
static Sint16 draw_5_polygons[1947];
static Sint32 draw_5_vertices[1063];
static NJS_CNK_MODEL draw_5_model;
static NJS_CNK_OBJECT draw_5;
static Sint16 draw_4_polygons[847];
static Sint32 draw_4_vertices[483];
static NJS_CNK_MODEL draw_4_model;
static NJS_CNK_OBJECT draw_4;
static Sint16 draw_3_polygons[447];
static Sint32 draw_3_vertices[443];
static NJS_CNK_MODEL draw_3_model;
static NJS_CNK_OBJECT draw_3;
static Sint16 draw_2_polygons[543];
static Sint32 draw_2_vertices[407];
static NJS_CNK_MODEL draw_2_model;
static NJS_CNK_OBJECT draw_2;
static Sint16 draw_1_polygons[173];
static Sint32 draw_1_vertices[99];
static NJS_CNK_MODEL draw_1_model;
static NJS_CNK_OBJECT draw_1;
static Sint16 draw_0_polygons[199];
static Sint32 draw_0_vertices[99];
static NJS_CNK_MODEL draw_0_model;
static NJS_CNK_OBJECT draw_0;
static NJS_MATERIAL collision_0_materials[1];
static Sint16 collision_0_faces[8];
static NJS_MESHSET collision_0_meshsets[1];
static NJS_POINT3 collision_0_points[6];
static NJS_MODEL collision_0_model;
static NJS_OBJECT collision_0;
static NJS_MATERIAL collision_2_materials[1];
static Sint16 collision_2_faces[15];
static NJS_MESHSET collision_2_meshsets[1];
static NJS_POINT3 collision_2_points[8];
static NJS_MODEL collision_2_model;
static NJS_OBJECT collision_2;
static NJS_MATERIAL collision_3_materials[1];
static Sint16 collision_3_faces[52];
static NJS_MESHSET collision_3_meshsets[1];
static NJS_POINT3 collision_3_points[30];
static NJS_MODEL collision_3_model;
static NJS_OBJECT collision_3;
static NJS_MATERIAL collision_4_materials[1];
static Sint16 collision_4_faces[42];
static NJS_MESHSET collision_4_meshsets[1];
static NJS_POINT3 collision_4_points[28];
static NJS_MODEL collision_4_model;
static NJS_OBJECT collision_4;
static NJS_MATERIAL collision_5_materials[1];
static Sint16 collision_5_faces[17];
static NJS_MESHSET collision_5_meshsets[1];
static NJS_POINT3 collision_5_points[9];
static NJS_MODEL collision_5_model;
static NJS_OBJECT collision_5;
static NJS_MATERIAL collision_7_materials[1];
static Sint16 collision_7_faces[47];
static NJS_MESHSET collision_7_meshsets[1];
static NJS_POINT3 collision_7_points[23];
static NJS_MODEL collision_7_model;
static NJS_OBJECT collision_7;
static NJS_MATERIAL collision_8_materials[1];
static Sint16 collision_8_faces[16];
static NJS_MESHSET collision_8_meshsets[1];
static NJS_POINT3 collision_8_points[9];
static NJS_MODEL collision_8_model;
static NJS_OBJECT collision_8;
static NJS_MATERIAL collision_10_materials[1];
static Sint16 collision_10_faces[36];
static NJS_MESHSET collision_10_meshsets[1];
static NJS_POINT3 collision_10_points[20];
static NJS_MODEL collision_10_model;
static NJS_OBJECT collision_10;
static NJS_MATERIAL collision_11_materials[1];
static Sint16 collision_11_faces[36];
static NJS_MESHSET collision_11_meshsets[1];
static NJS_POINT3 collision_11_points[22];
static NJS_MODEL collision_11_model;
static NJS_OBJECT collision_11;
static NJS_MATERIAL collision_17_materials[1];
static Sint16 collision_17_faces[14];
static NJS_MESHSET collision_17_meshsets[1];
static NJS_POINT3 collision_17_points[12];
static NJS_MODEL collision_17_model;
static NJS_OBJECT collision_17;
static NJS_MATERIAL collision_41_materials[1];
static Sint16 collision_41_faces[24];
static NJS_MESHSET collision_41_meshsets[1];
static NJS_POINT3 collision_41_points[16];
static NJS_MODEL collision_41_model;
static NJS_OBJECT collision_41;
static NJS_MATERIAL collision_42_materials[1];
static Sint16 collision_42_faces[18];
static NJS_MESHSET collision_42_meshsets[1];
static NJS_POINT3 collision_42_points[12];
static NJS_MODEL collision_42_model;
static NJS_OBJECT collision_42;
static NJS_MATERIAL collision_44_materials[1];
static Sint16 collision_44_faces[7];
static NJS_MESHSET collision_44_meshsets[1];
static NJS_POINT3 collision_44_points[6];
static NJS_MODEL collision_44_model;
static NJS_OBJECT collision_44;
static NJS_MATERIAL collision_46_materials[1];
static Sint16 collision_46_faces[38];
static NJS_MESHSET collision_46_meshsets[1];
static NJS_POINT3 collision_46_points[22];
static NJS_MODEL collision_46_model;
static NJS_OBJECT collision_46;
static NJS_MATERIAL collision_47_materials[1];
static Sint16 collision_47_faces[19];
static NJS_MESHSET collision_47_meshsets[1];
static NJS_POINT3 collision_47_points[12];
static NJS_MODEL collision_47_model;
static NJS_OBJECT collision_47;
static NJS_MATERIAL collision_48_materials[1];
static Sint16 collision_48_faces[14];
static NJS_MESHSET collision_48_meshsets[1];
static NJS_POINT3 collision_48_points[12];
static NJS_MODEL collision_48_model;
static NJS_OBJECT collision_48;
static NJS_MATERIAL collision_55_materials[1];
static Sint16 collision_55_faces[15];
static NJS_MESHSET collision_55_meshsets[1];
static NJS_POINT3 collision_55_points[12];
static NJS_MODEL collision_55_model;
static NJS_OBJECT collision_55;
static NJS_MATERIAL collision_57_materials[1];
static Sint16 collision_57_faces[36];
static NJS_MESHSET collision_57_meshsets[1];
static NJS_POINT3 collision_57_points[20];
static NJS_MODEL collision_57_model;
static NJS_OBJECT collision_57;
static NJS_MATERIAL collision_58_materials[1];
static Sint16 collision_58_faces[28];
static NJS_MESHSET collision_58_meshsets[1];
static NJS_POINT3 collision_58_points[18];
static NJS_MODEL collision_58_model;
static NJS_OBJECT collision_58;
static NJS_MATERIAL collision_59_materials[1];
static Sint16 collision_59_faces[28];
static NJS_MESHSET collision_59_meshsets[1];
static NJS_POINT3 collision_59_points[18];
static NJS_MODEL collision_59_model;
static NJS_OBJECT collision_59;
static NJS_MATERIAL collision_60_materials[1];
static Sint16 collision_60_faces[38];
static NJS_MESHSET collision_60_meshsets[1];
static NJS_POINT3 collision_60_points[24];
static NJS_MODEL collision_60_model;
static NJS_OBJECT collision_60;
static NJS_MATERIAL collision_67_materials[1];
static Sint16 collision_67_faces[10];
static NJS_MESHSET collision_67_meshsets[1];
static NJS_POINT3 collision_67_points[8];
static NJS_MODEL collision_67_model;
static NJS_OBJECT collision_67;
static NJS_MATERIAL collision_94_materials[1];
static Sint16 collision_94_faces[39];
static NJS_MESHSET collision_94_meshsets[1];
static NJS_POINT3 collision_94_points[24];
static NJS_MODEL collision_94_model;
static NJS_OBJECT collision_94;
static NJS_MATERIAL collision_95_materials[1];
static Sint16 collision_95_faces[14];
static NJS_MESHSET collision_95_meshsets[1];
static NJS_POINT3 collision_95_points[12];
static NJS_MODEL collision_95_model;
static NJS_OBJECT collision_95;
static NJS_MATERIAL collision_97_materials[1];
static Sint16 collision_97_faces[57];
static NJS_MESHSET collision_97_meshsets[1];
static NJS_POINT3 collision_97_points[27];
static NJS_MODEL collision_97_model;
static NJS_OBJECT collision_97;
static NJS_MATERIAL collision_98_materials[1];
static Sint16 collision_98_faces[14];
static NJS_MESHSET collision_98_meshsets[1];
static NJS_POINT3 collision_98_points[10];
static NJS_MODEL collision_98_model;
static NJS_OBJECT collision_98;
static NJS_MATERIAL collision_100_materials[1];
static Sint16 collision_100_faces[26];
static NJS_MESHSET collision_100_meshsets[1];
static NJS_POINT3 collision_100_points[18];
static NJS_MODEL collision_100_model;
static NJS_OBJECT collision_100;
static NJS_MATERIAL collision_102_materials[1];
static Sint16 collision_102_faces[42];
static NJS_MESHSET collision_102_meshsets[1];
static NJS_POINT3 collision_102_points[24];
static NJS_MODEL collision_102_model;
static NJS_OBJECT collision_102;
static NJS_MATERIAL collision_103_materials[1];
static Sint16 collision_103_faces[14];
static NJS_MESHSET collision_103_meshsets[1];
static NJS_POINT3 collision_103_points[12];
static NJS_MODEL collision_103_model;
static NJS_OBJECT collision_103;
static NJS_MATERIAL collision_107_materials[1];
static Sint16 collision_107_faces[19];
static NJS_MESHSET collision_107_meshsets[1];
static NJS_POINT3 collision_107_points[12];
static NJS_MODEL collision_107_model;
static NJS_OBJECT collision_107;
static NJS_MATERIAL collision_108_materials[1];
static Sint16 collision_108_faces[35];
static NJS_MESHSET collision_108_meshsets[1];
static NJS_POINT3 collision_108_points[19];
static NJS_MODEL collision_108_model;
static NJS_OBJECT collision_108;
static NJS_MATERIAL collision_109_materials[1];
static Sint16 collision_109_faces[118];
static NJS_MESHSET collision_109_meshsets[1];
static NJS_POINT3 collision_109_points[53];
static NJS_MODEL collision_109_model;
static NJS_OBJECT collision_109;
static NJS_MATERIAL collision_111_materials[1];
static Sint16 collision_111_faces[9];
static NJS_MESHSET collision_111_meshsets[1];
static NJS_POINT3 collision_111_points[8];
static NJS_MODEL collision_111_model;
static NJS_OBJECT collision_111;
static NJS_MATERIAL collision_113_materials[1];
static Sint16 collision_113_faces[17];
static NJS_MESHSET collision_113_meshsets[1];
static NJS_POINT3 collision_113_points[10];
static NJS_MODEL collision_113_model;
static NJS_OBJECT collision_113;
static NJS_MATERIAL collision_115_materials[1];
static Sint16 collision_115_faces[34];
static NJS_MESHSET collision_115_meshsets[1];
static NJS_POINT3 collision_115_points[18];
static NJS_MODEL collision_115_model;
static NJS_OBJECT collision_115;
static NJS_MATERIAL collision_118_materials[1];
static Sint16 collision_118_faces[67];
static NJS_MESHSET collision_118_meshsets[1];
static NJS_POINT3 collision_118_points[26];
static NJS_MODEL collision_118_model;
static NJS_OBJECT collision_118;
static NJS_MATERIAL collision_119_materials[1];
static Sint16 collision_119_faces[45];
static NJS_MESHSET collision_119_meshsets[1];
static NJS_POINT3 collision_119_points[21];
static NJS_MODEL collision_119_model;
static NJS_OBJECT collision_119;
static NJS_MATERIAL collision_120_materials[1];
static Sint16 collision_120_faces[28];
static NJS_MESHSET collision_120_meshsets[1];
static NJS_POINT3 collision_120_points[18];
static NJS_MODEL collision_120_model;
static NJS_OBJECT collision_120;
static NJS_MATERIAL collision_121_materials[1];
static Sint16 collision_121_faces[30];
static NJS_MESHSET collision_121_meshsets[1];
static NJS_POINT3 collision_121_points[23];
static NJS_MODEL collision_121_model;
static NJS_OBJECT collision_121;
static NJS_MATERIAL collision_122_materials[1];
static Sint16 collision_122_faces[59];
static NJS_MESHSET collision_122_meshsets[1];
static NJS_POINT3 collision_122_points[26];
static NJS_MODEL collision_122_model;
static NJS_OBJECT collision_122;
static NJS_MATERIAL collision_123_materials[1];
static Sint16 collision_123_faces[55];
static NJS_MESHSET collision_123_meshsets[1];
static NJS_POINT3 collision_123_points[27];
static NJS_MODEL collision_123_model;
static NJS_OBJECT collision_123;
static NJS_MATERIAL collision_124_materials[1];
static Sint16 collision_124_faces[20];
static NJS_MESHSET collision_124_meshsets[1];
static NJS_POINT3 collision_124_points[9];
static NJS_MODEL collision_124_model;
static NJS_OBJECT collision_124;
static NJS_MATERIAL collision_125_materials[1];
static Sint16 collision_125_faces[7];
static NJS_MESHSET collision_125_meshsets[1];
static NJS_POINT3 collision_125_points[6];
static NJS_MODEL collision_125_model;
static NJS_OBJECT collision_125;
static NJS_MATERIAL collision_127_materials[1];
static Sint16 collision_127_faces[109];
static NJS_MESHSET collision_127_meshsets[1];
static NJS_POINT3 collision_127_points[48];
static NJS_MODEL collision_127_model;
static NJS_OBJECT collision_127;
static NJS_MATERIAL collision_129_materials[1];
static Sint16 collision_129_faces[11];
static NJS_MESHSET collision_129_meshsets[1];
static NJS_POINT3 collision_129_points[8];
static NJS_MODEL collision_129_model;
static NJS_OBJECT collision_129;
static NJS_MATERIAL collision_131_materials[1];
static Sint16 collision_131_faces[53];
static NJS_MESHSET collision_131_meshsets[1];
static NJS_POINT3 collision_131_points[29];
static NJS_MODEL collision_131_model;
static NJS_OBJECT collision_131;
static NJS_MATERIAL collision_133_materials[1];
static Sint16 collision_133_faces[51];
static NJS_MESHSET collision_133_meshsets[1];
static NJS_POINT3 collision_133_points[20];
static NJS_MODEL collision_133_model;
static NJS_OBJECT collision_133;
static NJS_MATERIAL collision_134_materials[1];
static Sint16 collision_134_faces[47];
static NJS_MESHSET collision_134_meshsets[1];
static NJS_POINT3 collision_134_points[30];
static NJS_MODEL collision_134_model;
static NJS_OBJECT collision_134;
static NJS_MATERIAL collision_136_materials[1];
static Sint16 collision_136_faces[20];
static NJS_MESHSET collision_136_meshsets[1];
static NJS_POINT3 collision_136_points[9];
static NJS_MODEL collision_136_model;
static NJS_OBJECT collision_136;
static NJS_MATERIAL collision_137_materials[1];
static Sint16 collision_137_faces[52];
static NJS_MESHSET collision_137_meshsets[1];
static NJS_POINT3 collision_137_points[24];
static NJS_MODEL collision_137_model;
static NJS_OBJECT collision_137;
static NJS_MATERIAL collision_144_materials[1];
static Sint16 collision_144_faces[20];
static NJS_MESHSET collision_144_meshsets[1];
static NJS_POINT3 collision_144_points[12];
static NJS_MODEL collision_144_model;
static NJS_OBJECT collision_144;
static NJS_MATERIAL collision_168_materials[1];
static Sint16 collision_168_faces[15];
static NJS_MESHSET collision_168_meshsets[1];
static NJS_POINT3 collision_168_points[13];
static NJS_MODEL collision_168_model;
static NJS_OBJECT collision_168;
static NJS_MATERIAL collision_169_materials[1];
static Sint16 collision_169_faces[61];
static NJS_MESHSET collision_169_meshsets[1];
static NJS_POINT3 collision_169_points[31];
static NJS_MODEL collision_169_model;
static NJS_OBJECT collision_169;
static NJS_MATERIAL collision_171_materials[1];
static Sint16 collision_171_faces[14];
static NJS_MESHSET collision_171_meshsets[1];
static NJS_POINT3 collision_171_points[12];
static NJS_MODEL collision_171_model;
static NJS_OBJECT collision_171;
static NJS_MATERIAL collision_178_materials[1];
static Sint16 collision_178_faces[68];
static NJS_MESHSET collision_178_meshsets[1];
static NJS_POINT3 collision_178_points[31];
static NJS_MODEL collision_178_model;
static NJS_OBJECT collision_178;
static NJS_MATERIAL collision_182_materials[1];
static Sint16 collision_182_faces[35];
static NJS_MESHSET collision_182_meshsets[1];
static NJS_POINT3 collision_182_points[24];
static NJS_MODEL collision_182_model;
static NJS_OBJECT collision_182;
static NJS_MATERIAL collision_186_materials[1];
static Sint16 collision_186_faces[40];
static NJS_MESHSET collision_186_meshsets[1];
static NJS_POINT3 collision_186_points[12];
static NJS_MODEL collision_186_model;
static NJS_OBJECT collision_186;
static NJS_MATERIAL collision_189_materials[1];
static Sint16 collision_189_faces[56];
static NJS_MESHSET collision_189_meshsets[1];
static NJS_POINT3 collision_189_points[25];
static NJS_MODEL collision_189_model;
static NJS_OBJECT collision_189;
static NJS_MATERIAL collision_191_materials[1];
static Sint16 collision_191_faces[20];
static NJS_MESHSET collision_191_meshsets[1];
static NJS_POINT3 collision_191_points[10];
static NJS_MODEL collision_191_model;
static NJS_OBJECT collision_191;
static NJS_MATERIAL collision_223_materials[1];
static Sint16 collision_223_faces[40];
static NJS_MESHSET collision_223_meshsets[1];
static NJS_POINT3 collision_223_points[23];
static NJS_MODEL collision_223_model;
static NJS_OBJECT collision_223;
static NJS_MATERIAL collision_225_materials[1];
static Sint16 collision_225_faces[25];
static NJS_MESHSET collision_225_meshsets[1];
static NJS_POINT3 collision_225_points[18];
static NJS_MODEL collision_225_model;
static NJS_OBJECT collision_225;
static NJS_MATERIAL collision_227_materials[1];
static Sint16 collision_227_faces[7];
static NJS_MESHSET collision_227_meshsets[1];
static NJS_POINT3 collision_227_points[6];
static NJS_MODEL collision_227_model;
static NJS_OBJECT collision_227;
static NJS_MATERIAL collision_231_materials[1];
static Sint16 collision_231_faces[14];
static NJS_MESHSET collision_231_meshsets[1];
static NJS_POINT3 collision_231_points[12];
static NJS_MODEL collision_231_model;
static NJS_OBJECT collision_231;
static NJS_MATERIAL collision_232_materials[1];
static Sint16 collision_232_faces[22];
static NJS_MESHSET collision_232_meshsets[1];
static NJS_POINT3 collision_232_points[15];
static NJS_MODEL collision_232_model;
static NJS_OBJECT collision_232;
static NJS_MATERIAL collision_233_materials[1];
static Sint16 collision_233_faces[22];
static NJS_MESHSET collision_233_meshsets[1];
static NJS_POINT3 collision_233_points[15];
static NJS_MODEL collision_233_model;
static NJS_OBJECT collision_233;
static NJS_MATERIAL collision_234_materials[1];
static Sint16 collision_234_faces[22];
static NJS_MESHSET collision_234_meshsets[1];
static NJS_POINT3 collision_234_points[15];
static NJS_MODEL collision_234_model;
static NJS_OBJECT collision_234;
static NJS_MATERIAL collision_236_materials[1];
static Sint16 collision_236_faces[27];
static NJS_MESHSET collision_236_meshsets[1];
static NJS_POINT3 collision_236_points[24];
static NJS_MODEL collision_236_model;
static NJS_OBJECT collision_236;
static NJS_MATERIAL collision_238_materials[1];
static Sint16 collision_238_faces[21];
static NJS_MESHSET collision_238_meshsets[1];
static NJS_POINT3 collision_238_points[12];
static NJS_MODEL collision_238_model;
static NJS_OBJECT collision_238;
static NJS_MATERIAL collision_240_materials[1];
static Sint16 collision_240_faces[16];
static NJS_MESHSET collision_240_meshsets[1];
static NJS_POINT3 collision_240_points[8];
static NJS_MODEL collision_240_model;
static NJS_OBJECT collision_240;
static NJS_MATERIAL collision_242_materials[1];
static Sint16 collision_242_faces[46];
static NJS_MESHSET collision_242_meshsets[1];
static NJS_POINT3 collision_242_points[22];
static NJS_MODEL collision_242_model;
static NJS_OBJECT collision_242;
static NJS_MATERIAL collision_244_materials[1];
static Sint16 collision_244_faces[25];
static NJS_MESHSET collision_244_meshsets[1];
static NJS_POINT3 collision_244_points[18];
static NJS_MODEL collision_244_model;
static NJS_OBJECT collision_244;
static NJS_MATERIAL collision_247_materials[1];
static Sint16 collision_247_faces[28];
static NJS_MESHSET collision_247_meshsets[1];
static NJS_POINT3 collision_247_points[20];
static NJS_MODEL collision_247_model;
static NJS_OBJECT collision_247;
static NJS_MATERIAL collision_249_materials[1];
static Sint16 collision_249_faces[57];
static NJS_MESHSET collision_249_meshsets[1];
static NJS_POINT3 collision_249_points[24];
static NJS_MODEL collision_249_model;
static NJS_OBJECT collision_249;
static NJS_MATERIAL collision_250_materials[1];
static Sint16 collision_250_faces[18];
static NJS_MESHSET collision_250_meshsets[1];
static NJS_POINT3 collision_250_points[9];
static NJS_MODEL collision_250_model;
static NJS_OBJECT collision_250;
static NJS_MATERIAL collision_251_materials[1];
static Sint16 collision_251_faces[56];
static NJS_MESHSET collision_251_meshsets[1];
static NJS_POINT3 collision_251_points[24];
static NJS_MODEL collision_251_model;
static NJS_OBJECT collision_251;
static NJS_MATERIAL collision_252_materials[1];
static Sint16 collision_252_faces[14];
static NJS_MESHSET collision_252_meshsets[1];
static NJS_POINT3 collision_252_points[12];
static NJS_MODEL collision_252_model;
static NJS_OBJECT collision_252;
static NJS_MATERIAL collision_262_materials[1];
static Sint16 collision_262_faces[19];
static NJS_MESHSET collision_262_meshsets[1];
static NJS_POINT3 collision_262_points[16];
static NJS_MODEL collision_262_model;
static NJS_OBJECT collision_262;
static NJS_MATERIAL collision_263_materials[1];
static Sint16 collision_263_faces[14];
static NJS_MESHSET collision_263_meshsets[1];
static NJS_POINT3 collision_263_points[12];
static NJS_MODEL collision_263_model;
static NJS_OBJECT collision_263;
static NJS_MATERIAL collision_264_materials[1];
static Sint16 collision_264_faces[10];
static NJS_MESHSET collision_264_meshsets[1];
static NJS_POINT3 collision_264_points[8];
static NJS_MODEL collision_264_model;
static NJS_OBJECT collision_264;
static NJS_MATERIAL collision_265_materials[1];
static Sint16 collision_265_faces[38];
static NJS_MESHSET collision_265_meshsets[1];
static NJS_POINT3 collision_265_points[24];
static NJS_MODEL collision_265_model;
static NJS_OBJECT collision_265;
static NJS_MATERIAL collision_266_materials[1];
static Sint16 collision_266_faces[14];
static NJS_MESHSET collision_266_meshsets[1];
static NJS_POINT3 collision_266_points[12];
static NJS_MODEL collision_266_model;
static NJS_OBJECT collision_266;
static NJS_MATERIAL collision_272_materials[1];
static Sint16 collision_272_faces[97];
static NJS_MESHSET collision_272_meshsets[1];
static NJS_POINT3 collision_272_points[42];
static NJS_MODEL collision_272_model;
static NJS_OBJECT collision_272;
static NJS_MATERIAL collision_273_materials[1];
static Sint16 collision_273_faces[97];
static NJS_MESHSET collision_273_meshsets[1];
static NJS_POINT3 collision_273_points[44];
static NJS_MODEL collision_273_model;
static NJS_OBJECT collision_273;
static NJS_MATERIAL collision_274_materials[1];
static Sint16 collision_274_faces[65];
static NJS_MESHSET collision_274_meshsets[1];
static NJS_POINT3 collision_274_points[24];
static NJS_MODEL collision_274_model;
static NJS_OBJECT collision_274;
static NJS_MATERIAL collision_275_materials[1];
static Sint16 collision_275_faces[80];
static NJS_MESHSET collision_275_meshsets[1];
static NJS_POINT3 collision_275_points[32];
static NJS_MODEL collision_275_model;
static NJS_OBJECT collision_275;
static NJS_MATERIAL collision_276_materials[1];
static Sint16 collision_276_faces[74];
static NJS_MESHSET collision_276_meshsets[1];
static NJS_POINT3 collision_276_points[30];
static NJS_MODEL collision_276_model;
static NJS_OBJECT collision_276;
static NJS_MATERIAL collision_277_materials[1];
static Sint16 collision_277_faces[30];
static NJS_MESHSET collision_277_meshsets[1];
static NJS_POINT3 collision_277_points[20];
static NJS_MODEL collision_277_model;
static NJS_OBJECT collision_277;
static NJS_MATERIAL collision_278_materials[1];
static Sint16 collision_278_faces[25];
static NJS_MESHSET collision_278_meshsets[1];
static NJS_POINT3 collision_278_points[20];
static NJS_MODEL collision_278_model;
static NJS_OBJECT collision_278;
static NJS_MATERIAL collision_279_materials[1];
static Sint16 collision_279_faces[7];
static NJS_MESHSET collision_279_meshsets[1];
static NJS_POINT3 collision_279_points[6];
static NJS_MODEL collision_279_model;
static NJS_OBJECT collision_279;
static NJS_MATERIAL collision_280_materials[1];
static Sint16 collision_280_faces[23];
static NJS_MESHSET collision_280_meshsets[1];
static NJS_POINT3 collision_280_points[16];
static NJS_MODEL collision_280_model;
static NJS_OBJECT collision_280;
static NJS_MATERIAL collision_281_materials[1];
static Sint16 collision_281_faces[11];
static NJS_MESHSET collision_281_meshsets[1];
static NJS_POINT3 collision_281_points[10];
static NJS_MODEL collision_281_model;
static NJS_OBJECT collision_281;
static NJS_MATERIAL collision_282_materials[1];
static Sint16 collision_282_faces[7];
static NJS_MESHSET collision_282_meshsets[1];
static NJS_POINT3 collision_282_points[6];
static NJS_MODEL collision_282_model;
static NJS_OBJECT collision_282;
static NJS_MATERIAL collision_283_materials[1];
static Sint16 collision_283_faces[9];
static NJS_MESHSET collision_283_meshsets[1];
static NJS_POINT3 collision_283_points[8];
static NJS_MODEL collision_283_model;
static NJS_OBJECT collision_283;
static NJS_MATERIAL collision_284_materials[1];
static Sint16 collision_284_faces[9];
static NJS_MESHSET collision_284_meshsets[1];
static NJS_POINT3 collision_284_points[8];
static NJS_MODEL collision_284_model;
static NJS_OBJECT collision_284;
static NJS_MATERIAL collision_285_materials[1];
static Sint16 collision_285_faces[36];
static NJS_MESHSET collision_285_meshsets[1];
static NJS_POINT3 collision_285_points[26];
static NJS_MODEL collision_285_model;
static NJS_OBJECT collision_285;
static NJS_MATERIAL collision_286_materials[1];
static Sint16 collision_286_faces[32];
static NJS_MESHSET collision_286_meshsets[1];
static NJS_POINT3 collision_286_points[22];
static NJS_MODEL collision_286_model;
static NJS_OBJECT collision_286;
static NJS_MATERIAL collision_287_materials[1];
static Sint16 collision_287_faces[36];
static NJS_MESHSET collision_287_meshsets[1];
static NJS_POINT3 collision_287_points[26];
static NJS_MODEL collision_287_model;
static NJS_OBJECT collision_287;
static NJS_MATERIAL collision_288_materials[1];
static Sint16 collision_288_faces[9];
static NJS_MESHSET collision_288_meshsets[1];
static NJS_POINT3 collision_288_points[8];
static NJS_MODEL collision_288_model;
static NJS_OBJECT collision_288;
static NJS_MATERIAL collision_289_materials[1];
static Sint16 collision_289_faces[18];
static NJS_MESHSET collision_289_meshsets[1];
static NJS_POINT3 collision_289_points[14];
static NJS_MODEL collision_289_model;
static NJS_OBJECT collision_289;
static NJS_MATERIAL collision_290_materials[1];
static Sint16 collision_290_faces[40];
static NJS_MESHSET collision_290_meshsets[1];
static NJS_POINT3 collision_290_points[30];
static NJS_MODEL collision_290_model;
static NJS_OBJECT collision_290;
static NJS_MATERIAL collision_291_materials[1];
static Sint16 collision_291_faces[14];
static NJS_MESHSET collision_291_meshsets[1];
static NJS_POINT3 collision_291_points[10];
static NJS_MODEL collision_291_model;
static NJS_OBJECT collision_291;
static NJS_MATERIAL collision_292_materials[1];
static Sint16 collision_292_faces[16];
static NJS_MESHSET collision_292_meshsets[1];
static NJS_POINT3 collision_292_points[12];
static NJS_MODEL collision_292_model;
static NJS_OBJECT collision_292;
static NJS_MATERIAL collision_293_materials[1];
static Sint16 collision_293_faces[43];
static NJS_MESHSET collision_293_meshsets[1];
static NJS_POINT3 collision_293_points[24];
static NJS_MODEL collision_293_model;
static NJS_OBJECT collision_293;
static NJS_MATERIAL collision_294_materials[1];
static Sint16 collision_294_faces[47];
static NJS_MESHSET collision_294_meshsets[1];
static NJS_POINT3 collision_294_points[29];
static NJS_MODEL collision_294_model;
static NJS_OBJECT collision_294;
static NJS_MATERIAL collision_295_materials[1];
static Sint16 collision_295_faces[16];
static NJS_MESHSET collision_295_meshsets[1];
static NJS_POINT3 collision_295_points[10];
static NJS_MODEL collision_295_model;
static NJS_OBJECT collision_295;
static NJS_MATERIAL collision_296_materials[1];
static Sint16 collision_296_faces[26];
static NJS_MESHSET collision_296_meshsets[1];
static NJS_POINT3 collision_296_points[18];
static NJS_MODEL collision_296_model;
static NJS_OBJECT collision_296;
static NJS_MATERIAL collision_297_materials[1];
static Sint16 collision_297_faces[14];
static NJS_MESHSET collision_297_meshsets[1];
static NJS_POINT3 collision_297_points[9];
static NJS_MODEL collision_297_model;
static NJS_OBJECT collision_297;
static NJS_MATERIAL collision_298_materials[1];
static Sint16 collision_298_faces[9];
static NJS_MESHSET collision_298_meshsets[1];
static NJS_POINT3 collision_298_points[8];
static NJS_MODEL collision_298_model;
static NJS_OBJECT collision_298;
static NJS_MATERIAL collision_299_materials[1];
static Sint16 collision_299_faces[7];
static NJS_MESHSET collision_299_meshsets[1];
static NJS_POINT3 collision_299_points[6];
static NJS_MODEL collision_299_model;
static NJS_OBJECT collision_299;
static NJS_MATERIAL collision_300_materials[1];
static Sint16 collision_300_faces[10];
static NJS_MESHSET collision_300_meshsets[1];
static NJS_POINT3 collision_300_points[6];
static NJS_MODEL collision_300_model;
static NJS_OBJECT collision_300;
static NJS_MATERIAL collision_305_materials[1];
static Sint16 collision_305_faces[10];
static NJS_MESHSET collision_305_meshsets[1];
static NJS_POINT3 collision_305_points[6];
static NJS_MODEL collision_305_model;
static NJS_OBJECT collision_305;
static NJS_MATERIAL collision_306_materials[1];
static Sint16 collision_306_faces[10];
static NJS_MESHSET collision_306_meshsets[1];
static NJS_POINT3 collision_306_points[6];
static NJS_MODEL collision_306_model;
static NJS_OBJECT collision_306;
static NJS_MATERIAL collision_307_materials[1];
static Sint16 collision_307_faces[58];
static NJS_MESHSET collision_307_meshsets[1];
static NJS_POINT3 collision_307_points[33];
static NJS_MODEL collision_307_model;
static NJS_OBJECT collision_307;
static NJS_MATERIAL collision_308_materials[1];
static Sint16 collision_308_faces[50];
static NJS_MESHSET collision_308_meshsets[1];
static NJS_POINT3 collision_308_points[27];
static NJS_MODEL collision_308_model;
static NJS_OBJECT collision_308;
static NJS_MATERIAL collision_309_materials[1];
static Sint16 collision_309_faces[58];
static NJS_MESHSET collision_309_meshsets[1];
static NJS_POINT3 collision_309_points[33];
static NJS_MODEL collision_309_model;
static NJS_OBJECT collision_309;
static NJS_MATERIAL collision_310_materials[1];
static Sint16 collision_310_faces[10];
static NJS_MESHSET collision_310_meshsets[1];
static NJS_POINT3 collision_310_points[6];
static NJS_MODEL collision_310_model;
static NJS_OBJECT collision_310;
static NJS_MATERIAL collision_311_materials[1];
static Sint16 collision_311_faces[28];
static NJS_MESHSET collision_311_meshsets[1];
static NJS_POINT3 collision_311_points[15];
static NJS_MODEL collision_311_model;
static NJS_OBJECT collision_311;
static NJS_MATERIAL collision_312_materials[1];
static Sint16 collision_312_faces[70];
static NJS_MESHSET collision_312_meshsets[1];
static NJS_POINT3 collision_312_points[42];
static NJS_MODEL collision_312_model;
static NJS_OBJECT collision_312;
static NJS_MATERIAL collision_313_materials[1];
static Sint16 collision_313_faces[14];
static NJS_MESHSET collision_313_meshsets[1];
static NJS_POINT3 collision_313_points[9];
static NJS_MODEL collision_313_model;
static NJS_OBJECT collision_313;
static NJS_MATERIAL collision_314_materials[1];
static Sint16 collision_314_faces[25];
static NJS_MESHSET collision_314_meshsets[1];
static NJS_POINT3 collision_314_points[15];
static NJS_MODEL collision_314_model;
static NJS_OBJECT collision_314;
static NJS_CNK_OBJECT draw_974;
static NJS_CNK_OBJECT draw_970;
static NJS_CNK_OBJECT draw_969;
static NJS_CNK_OBJECT draw_966;
static NJS_CNK_OBJECT draw_965;
static NJS_CNK_OBJECT draw_964;
static NJS_CNK_OBJECT draw_962;
static NJS_CNK_OBJECT draw_960;
static NJS_CNK_OBJECT draw_959;
static NJS_CNK_OBJECT draw_958;
static NJS_CNK_OBJECT draw_952;
static NJS_CNK_OBJECT draw_948;
static NJS_CNK_OBJECT draw_947;
static NJS_CNK_OBJECT draw_946;
static NJS_CNK_OBJECT draw_944;
static NJS_CNK_OBJECT draw_942;
static NJS_CNK_OBJECT draw_941;
static NJS_CNK_OBJECT draw_939;
static NJS_CNK_OBJECT draw_938;
static NJS_CNK_OBJECT draw_936;
static NJS_CNK_OBJECT draw_935;
static NJS_CNK_OBJECT draw_934;
static NJS_CNK_OBJECT draw_932;
static NJS_CNK_OBJECT draw_931;
static NJS_CNK_OBJECT draw_930;
static NJS_CNK_OBJECT draw_929;
static NJS_CNK_OBJECT draw_928;
static NJS_CNK_OBJECT draw_927;
static NJS_CNK_OBJECT draw_925;
static NJS_CNK_OBJECT draw_923;
static NJS_CNK_OBJECT draw_922;
static NJS_CNK_OBJECT draw_921;
static NJS_CNK_OBJECT draw_920;
static NJS_CNK_OBJECT draw_918;
static NJS_CNK_OBJECT draw_917;
static NJS_CNK_OBJECT draw_916;
static NJS_CNK_OBJECT draw_915;
static NJS_CNK_OBJECT draw_914;
static NJS_CNK_OBJECT draw_913;
static NJS_CNK_OBJECT draw_912;
static NJS_CNK_OBJECT draw_911;
static NJS_CNK_OBJECT draw_910;
static NJS_CNK_OBJECT draw_909;
static NJS_CNK_OBJECT draw_908;
static NJS_CNK_OBJECT draw_907;
static NJS_CNK_OBJECT draw_906;
static NJS_CNK_OBJECT draw_905;
static NJS_CNK_OBJECT draw_904;
static NJS_CNK_OBJECT draw_903;
static NJS_CNK_OBJECT draw_901;
static NJS_CNK_OBJECT draw_900;
static NJS_CNK_OBJECT draw_899;
static NJS_CNK_OBJECT draw_898;
static NJS_CNK_OBJECT draw_897;
static NJS_CNK_OBJECT draw_896;
static NJS_CNK_OBJECT draw_895;
static NJS_CNK_OBJECT draw_894;
static NJS_CNK_OBJECT draw_890;
static NJS_CNK_OBJECT draw_887;
static NJS_CNK_OBJECT draw_886;
static NJS_CNK_OBJECT draw_885;
static NJS_CNK_OBJECT draw_884;
static NJS_CNK_OBJECT draw_883;
static NJS_CNK_OBJECT draw_882;
static NJS_CNK_OBJECT draw_881;
static NJS_CNK_OBJECT draw_880;
static NJS_CNK_OBJECT draw_879;
static NJS_CNK_OBJECT draw_878;
static NJS_CNK_OBJECT draw_877;
static NJS_CNK_OBJECT draw_876;
static NJS_CNK_OBJECT draw_875;
static NJS_CNK_OBJECT draw_874;
static NJS_CNK_OBJECT draw_873;
static NJS_CNK_OBJECT draw_872;
static NJS_CNK_OBJECT draw_871;
static NJS_CNK_OBJECT draw_870;
static NJS_CNK_OBJECT draw_869;
static NJS_CNK_OBJECT draw_868;
static NJS_CNK_OBJECT draw_867;
static NJS_CNK_OBJECT draw_866;
static NJS_CNK_OBJECT draw_865;
static NJS_CNK_OBJECT draw_864;
static NJS_CNK_OBJECT draw_863;
static NJS_CNK_OBJECT draw_862;
static NJS_CNK_OBJECT draw_861;
static NJS_CNK_OBJECT draw_860;
static NJS_CNK_OBJECT draw_859;
static NJS_CNK_OBJECT draw_858;
static NJS_CNK_OBJECT draw_857;
static NJS_CNK_OBJECT draw_856;
static NJS_CNK_OBJECT draw_855;
static NJS_CNK_OBJECT draw_853;
static NJS_CNK_OBJECT draw_852;
static NJS_CNK_OBJECT draw_851;
static NJS_CNK_OBJECT draw_850;
static NJS_CNK_OBJECT draw_849;
static NJS_CNK_OBJECT draw_848;
static NJS_CNK_OBJECT draw_847;
static NJS_CNK_OBJECT draw_846;
static NJS_CNK_OBJECT draw_845;
static NJS_CNK_OBJECT draw_844;
static NJS_CNK_OBJECT draw_843;
static NJS_CNK_OBJECT draw_842;
static NJS_CNK_OBJECT draw_840;
static NJS_CNK_OBJECT draw_839;
static NJS_CNK_OBJECT draw_838;
static NJS_CNK_OBJECT draw_837;
static NJS_CNK_OBJECT draw_836;
static NJS_CNK_OBJECT draw_835;
static NJS_CNK_OBJECT draw_834;
static NJS_CNK_OBJECT draw_833;
static NJS_CNK_OBJECT draw_832;
static NJS_CNK_OBJECT draw_831;
static NJS_CNK_OBJECT draw_830;
static NJS_CNK_OBJECT draw_829;
static NJS_CNK_OBJECT draw_828;
static NJS_CNK_OBJECT draw_827;
static NJS_CNK_OBJECT draw_826;
static NJS_CNK_OBJECT draw_825;
static NJS_CNK_OBJECT draw_824;
static NJS_CNK_OBJECT draw_823;
static NJS_CNK_OBJECT draw_822;
static NJS_CNK_OBJECT draw_821;
static NJS_CNK_OBJECT draw_820;
static NJS_CNK_OBJECT draw_819;
static NJS_CNK_OBJECT draw_818;
static NJS_CNK_OBJECT draw_816;
static NJS_CNK_OBJECT draw_814;
static NJS_CNK_OBJECT draw_813;
static NJS_CNK_OBJECT draw_811;
static NJS_CNK_OBJECT draw_809;
static NJS_CNK_OBJECT draw_808;
static NJS_CNK_OBJECT draw_807;
static NJS_CNK_OBJECT draw_806;
static NJS_CNK_OBJECT draw_804;
static NJS_CNK_OBJECT draw_801;
static NJS_CNK_OBJECT draw_800;
static NJS_CNK_OBJECT draw_799;
static NJS_CNK_OBJECT draw_798;
static NJS_CNK_OBJECT draw_797;
static NJS_CNK_OBJECT draw_788;
static NJS_CNK_OBJECT draw_787;
static NJS_CNK_OBJECT draw_786;
static NJS_CNK_OBJECT draw_785;
static NJS_CNK_OBJECT draw_784;
static NJS_CNK_OBJECT draw_783;
static NJS_CNK_OBJECT draw_782;
static NJS_CNK_OBJECT draw_781;
static NJS_CNK_OBJECT draw_780;
static NJS_CNK_OBJECT draw_777;
static NJS_CNK_OBJECT draw_775;
static NJS_CNK_OBJECT draw_774;
static NJS_CNK_OBJECT draw_771;
static NJS_CNK_OBJECT draw_769;
static NJS_CNK_OBJECT draw_768;
static NJS_CNK_OBJECT draw_767;
static NJS_CNK_OBJECT draw_764;
static NJS_CNK_OBJECT draw_763;
static NJS_CNK_OBJECT draw_762;
static NJS_CNK_OBJECT draw_761;
static NJS_CNK_OBJECT draw_759;
static NJS_CNK_OBJECT draw_758;
static NJS_CNK_OBJECT draw_757;
static NJS_CNK_OBJECT draw_753;
static NJS_CNK_OBJECT draw_752;
static NJS_CNK_OBJECT draw_751;
static NJS_CNK_OBJECT draw_750;
static NJS_CNK_OBJECT draw_749;
static NJS_CNK_OBJECT draw_748;
static NJS_CNK_OBJECT draw_747;
static NJS_CNK_OBJECT draw_746;
static NJS_CNK_OBJECT draw_745;
static NJS_CNK_OBJECT draw_744;
static NJS_CNK_OBJECT draw_743;
static NJS_CNK_OBJECT draw_742;
static NJS_CNK_OBJECT draw_741;
static NJS_CNK_OBJECT draw_740;
static NJS_CNK_OBJECT draw_739;
static NJS_CNK_OBJECT draw_738;
static NJS_CNK_OBJECT draw_737;
static NJS_CNK_OBJECT draw_736;
static NJS_CNK_OBJECT draw_735;
static NJS_CNK_OBJECT draw_734;
static NJS_CNK_OBJECT draw_733;
static NJS_CNK_OBJECT draw_732;
static NJS_CNK_OBJECT draw_731;
static NJS_CNK_OBJECT draw_730;
static NJS_CNK_OBJECT draw_729;
static NJS_CNK_OBJECT draw_728;
static NJS_CNK_OBJECT draw_727;
static NJS_CNK_OBJECT draw_726;
static NJS_CNK_OBJECT draw_725;
static NJS_CNK_OBJECT draw_724;
static NJS_CNK_OBJECT draw_723;
static NJS_CNK_OBJECT draw_722;
static NJS_CNK_OBJECT draw_721;
static NJS_CNK_OBJECT draw_720;
static NJS_CNK_OBJECT draw_719;
static NJS_CNK_OBJECT draw_718;
static NJS_CNK_OBJECT draw_717;
static NJS_CNK_OBJECT draw_716;
static NJS_CNK_OBJECT draw_715;
static NJS_CNK_OBJECT draw_714;
static NJS_CNK_OBJECT draw_713;
static NJS_CNK_OBJECT draw_712;
static NJS_CNK_OBJECT draw_711;
static NJS_CNK_OBJECT draw_710;
static NJS_CNK_OBJECT draw_709;
static NJS_CNK_OBJECT draw_708;
static NJS_CNK_OBJECT draw_707;
static NJS_CNK_OBJECT draw_706;
static NJS_CNK_OBJECT draw_705;
static NJS_CNK_OBJECT draw_704;
static NJS_CNK_OBJECT draw_703;
static NJS_CNK_OBJECT draw_702;
static NJS_CNK_OBJECT draw_701;
static NJS_CNK_OBJECT draw_700;
static NJS_CNK_OBJECT draw_699;
static NJS_CNK_OBJECT draw_698;
static NJS_CNK_OBJECT draw_697;
static NJS_CNK_OBJECT draw_696;
static NJS_CNK_OBJECT draw_695;
static NJS_CNK_OBJECT draw_693;
static NJS_CNK_OBJECT draw_692;
static NJS_CNK_OBJECT draw_691;
static NJS_CNK_OBJECT draw_690;
static NJS_CNK_OBJECT draw_689;
static NJS_CNK_OBJECT draw_688;
static NJS_CNK_OBJECT draw_687;
static NJS_CNK_OBJECT draw_686;
static NJS_CNK_OBJECT draw_685;
static NJS_CNK_OBJECT draw_684;
static NJS_CNK_OBJECT draw_683;
static NJS_CNK_OBJECT draw_682;
static NJS_CNK_OBJECT draw_681;
static NJS_CNK_OBJECT draw_680;
static NJS_CNK_OBJECT draw_679;
static NJS_CNK_OBJECT draw_678;
static NJS_CNK_OBJECT draw_677;
static NJS_CNK_OBJECT draw_676;
static NJS_CNK_OBJECT draw_675;
static NJS_CNK_OBJECT draw_674;
static NJS_CNK_OBJECT draw_673;
static NJS_CNK_OBJECT draw_672;
static NJS_CNK_OBJECT draw_671;
static NJS_CNK_OBJECT draw_670;
static NJS_CNK_OBJECT draw_669;
static NJS_CNK_OBJECT draw_668;
static NJS_CNK_OBJECT draw_667;
static NJS_CNK_OBJECT draw_666;
static NJS_CNK_OBJECT draw_665;
static NJS_CNK_OBJECT draw_664;
static NJS_CNK_OBJECT draw_663;
static NJS_CNK_OBJECT draw_662;
static NJS_CNK_OBJECT draw_661;
static NJS_CNK_OBJECT draw_660;
static NJS_CNK_OBJECT draw_659;
static NJS_CNK_OBJECT draw_658;
static NJS_CNK_OBJECT draw_657;
static NJS_CNK_OBJECT draw_656;
static NJS_CNK_OBJECT draw_655;
static NJS_CNK_OBJECT draw_654;
static NJS_CNK_OBJECT draw_653;
static NJS_CNK_OBJECT draw_652;
static NJS_CNK_OBJECT draw_651;
static NJS_CNK_OBJECT draw_650;
static NJS_CNK_OBJECT draw_649;
static NJS_CNK_OBJECT draw_648;
static NJS_CNK_OBJECT draw_647;
static NJS_CNK_OBJECT draw_646;
static NJS_CNK_OBJECT draw_645;
static NJS_CNK_OBJECT draw_644;
static NJS_CNK_OBJECT draw_643;
static NJS_CNK_OBJECT draw_642;
static NJS_CNK_OBJECT draw_639;
static NJS_CNK_OBJECT draw_637;
static NJS_CNK_OBJECT draw_631;
static NJS_CNK_OBJECT draw_630;
static NJS_CNK_OBJECT draw_629;
static NJS_CNK_OBJECT draw_627;
static NJS_CNK_OBJECT draw_626;
static NJS_CNK_OBJECT draw_625;
static NJS_CNK_OBJECT draw_624;
static NJS_CNK_OBJECT draw_623;
static NJS_CNK_OBJECT draw_622;
static NJS_CNK_OBJECT draw_621;
static NJS_CNK_OBJECT draw_620;
static NJS_CNK_OBJECT draw_619;
static NJS_CNK_OBJECT draw_618;
static NJS_CNK_OBJECT draw_617;
static NJS_CNK_OBJECT draw_616;
static NJS_CNK_OBJECT draw_615;
static NJS_CNK_OBJECT draw_614;
static NJS_CNK_OBJECT draw_613;
static NJS_CNK_OBJECT draw_612;
static NJS_CNK_OBJECT draw_611;
static NJS_CNK_OBJECT draw_610;
static NJS_CNK_OBJECT draw_609;
static NJS_CNK_OBJECT draw_607;
static NJS_CNK_OBJECT draw_606;
static NJS_CNK_OBJECT draw_605;
static NJS_CNK_OBJECT draw_604;
static NJS_CNK_OBJECT draw_599;
static NJS_CNK_OBJECT draw_598;
static NJS_CNK_OBJECT draw_597;
static NJS_CNK_OBJECT draw_596;
static NJS_CNK_OBJECT draw_595;
static NJS_CNK_OBJECT draw_594;
static NJS_CNK_OBJECT draw_593;
static NJS_CNK_OBJECT draw_592;
static NJS_CNK_OBJECT draw_591;
static NJS_CNK_OBJECT draw_590;
static NJS_CNK_OBJECT draw_589;
static NJS_CNK_OBJECT draw_588;
static NJS_CNK_OBJECT draw_587;
static NJS_CNK_OBJECT draw_586;
static NJS_CNK_OBJECT draw_585;
static NJS_CNK_OBJECT draw_584;
static NJS_CNK_OBJECT draw_583;
static NJS_CNK_OBJECT draw_582;
static NJS_CNK_OBJECT draw_581;
static NJS_CNK_OBJECT draw_580;
static NJS_CNK_OBJECT draw_579;
static NJS_CNK_OBJECT draw_578;
static NJS_CNK_OBJECT draw_577;
static NJS_CNK_OBJECT draw_576;
static NJS_CNK_OBJECT draw_575;
static NJS_CNK_OBJECT draw_574;
static NJS_CNK_OBJECT draw_573;
static NJS_CNK_OBJECT draw_572;
static NJS_CNK_OBJECT draw_571;
static NJS_CNK_OBJECT draw_570;
static NJS_CNK_OBJECT draw_569;
static NJS_CNK_OBJECT draw_568;
static NJS_CNK_OBJECT draw_567;
static NJS_CNK_OBJECT draw_566;
static NJS_CNK_OBJECT draw_565;
static NJS_CNK_OBJECT draw_564;
static NJS_CNK_OBJECT draw_563;
static NJS_CNK_OBJECT draw_562;
static NJS_CNK_OBJECT draw_561;
static NJS_CNK_OBJECT draw_560;
static NJS_CNK_OBJECT draw_559;
static NJS_CNK_OBJECT draw_558;
static NJS_CNK_OBJECT draw_557;
static NJS_CNK_OBJECT draw_556;
static NJS_CNK_OBJECT draw_555;
static NJS_CNK_OBJECT draw_554;
static NJS_CNK_OBJECT draw_553;
static NJS_CNK_OBJECT draw_552;
static NJS_CNK_OBJECT draw_551;
static NJS_CNK_OBJECT draw_550;
static NJS_CNK_OBJECT draw_549;
static NJS_CNK_OBJECT draw_548;
static NJS_CNK_OBJECT draw_547;
static NJS_CNK_OBJECT draw_545;
static NJS_CNK_OBJECT draw_543;
static NJS_CNK_OBJECT draw_542;
static NJS_CNK_OBJECT draw_541;
static NJS_CNK_OBJECT draw_540;
static NJS_CNK_OBJECT draw_539;
static NJS_CNK_OBJECT draw_538;
static NJS_CNK_OBJECT draw_537;
static NJS_CNK_OBJECT draw_536;
static NJS_CNK_OBJECT draw_535;
static NJS_CNK_OBJECT draw_534;
static NJS_CNK_OBJECT draw_533;
static NJS_CNK_OBJECT draw_532;
static NJS_CNK_OBJECT draw_531;
static NJS_CNK_OBJECT draw_530;
static NJS_CNK_OBJECT draw_529;
static NJS_CNK_OBJECT draw_528;
static NJS_CNK_OBJECT draw_527;
static NJS_CNK_OBJECT draw_526;
static NJS_CNK_OBJECT draw_525;
static NJS_CNK_OBJECT draw_524;
static NJS_CNK_OBJECT draw_523;
static NJS_CNK_OBJECT draw_522;
static NJS_CNK_OBJECT draw_521;
static NJS_CNK_OBJECT draw_520;
static NJS_CNK_OBJECT draw_519;
static NJS_CNK_OBJECT draw_518;
static NJS_CNK_OBJECT draw_517;
static NJS_CNK_OBJECT draw_516;
static NJS_CNK_OBJECT draw_514;
static NJS_CNK_OBJECT draw_513;
static NJS_CNK_OBJECT draw_512;
static NJS_CNK_OBJECT draw_511;
static NJS_CNK_OBJECT draw_510;
static NJS_CNK_OBJECT draw_509;
static NJS_CNK_OBJECT draw_508;
static NJS_CNK_OBJECT draw_507;
static NJS_CNK_OBJECT draw_506;
static NJS_CNK_OBJECT draw_505;
static NJS_CNK_OBJECT draw_504;
static NJS_CNK_OBJECT draw_502;
static NJS_CNK_OBJECT draw_501;
static NJS_CNK_OBJECT draw_500;
static NJS_CNK_OBJECT draw_499;
static NJS_CNK_OBJECT draw_498;
static NJS_CNK_OBJECT draw_497;
static NJS_CNK_OBJECT draw_496;
static NJS_CNK_OBJECT draw_495;
static NJS_CNK_OBJECT draw_494;
static NJS_CNK_OBJECT draw_493;
static NJS_CNK_OBJECT draw_492;
static NJS_CNK_OBJECT draw_491;
static NJS_CNK_OBJECT draw_490;
static NJS_CNK_OBJECT draw_489;
static NJS_CNK_OBJECT draw_488;
static NJS_CNK_OBJECT draw_487;
static NJS_CNK_OBJECT draw_486;
static NJS_CNK_OBJECT draw_485;
static NJS_CNK_OBJECT draw_484;
static NJS_CNK_OBJECT draw_483;
static NJS_CNK_OBJECT draw_482;
static NJS_CNK_OBJECT draw_481;
static NJS_CNK_OBJECT draw_480;
static NJS_CNK_OBJECT draw_479;
static NJS_CNK_OBJECT draw_478;
static NJS_CNK_OBJECT draw_477;
static NJS_CNK_OBJECT draw_476;
static NJS_CNK_OBJECT draw_475;
static NJS_CNK_OBJECT draw_474;
static NJS_CNK_OBJECT draw_473;
static NJS_CNK_OBJECT draw_472;
static NJS_CNK_OBJECT draw_471;
static NJS_CNK_OBJECT draw_470;
static NJS_CNK_OBJECT draw_469;
static NJS_CNK_OBJECT draw_468;
static NJS_CNK_OBJECT draw_467;
static NJS_CNK_OBJECT draw_466;
static NJS_CNK_OBJECT draw_465;
static NJS_CNK_OBJECT draw_464;
static NJS_CNK_OBJECT draw_463;
static NJS_CNK_OBJECT draw_462;
static NJS_CNK_OBJECT draw_461;
static NJS_CNK_OBJECT draw_460;
static NJS_CNK_OBJECT draw_459;
static NJS_CNK_OBJECT draw_458;
static NJS_CNK_OBJECT draw_457;
static NJS_CNK_OBJECT draw_456;
static NJS_CNK_OBJECT draw_455;
static NJS_CNK_OBJECT draw_454;
static NJS_CNK_OBJECT draw_453;
static NJS_CNK_OBJECT draw_452;
static NJS_CNK_OBJECT draw_451;
static NJS_CNK_OBJECT draw_450;
static NJS_CNK_OBJECT draw_449;
static NJS_CNK_OBJECT draw_448;
static NJS_CNK_OBJECT draw_447;
static NJS_CNK_OBJECT draw_446;
static NJS_CNK_OBJECT draw_445;
static NJS_CNK_OBJECT draw_444;
static NJS_CNK_OBJECT draw_443;
static NJS_CNK_OBJECT draw_441;
static NJS_CNK_OBJECT draw_440;
static NJS_CNK_OBJECT draw_439;
static NJS_CNK_OBJECT draw_438;
static NJS_CNK_OBJECT draw_437;
static NJS_CNK_OBJECT draw_436;
static NJS_CNK_OBJECT draw_435;
static NJS_CNK_OBJECT draw_434;
static NJS_CNK_OBJECT draw_433;
static NJS_CNK_OBJECT draw_432;
static NJS_CNK_OBJECT draw_431;
static NJS_CNK_OBJECT draw_430;
static NJS_CNK_OBJECT draw_429;
static NJS_CNK_OBJECT draw_428;
static NJS_CNK_OBJECT draw_427;
static NJS_CNK_OBJECT draw_426;
static NJS_CNK_OBJECT draw_425;
static NJS_CNK_OBJECT draw_424;
static NJS_CNK_OBJECT draw_423;
static NJS_CNK_OBJECT draw_422;
static NJS_CNK_OBJECT draw_421;
static NJS_CNK_OBJECT draw_420;
static NJS_CNK_OBJECT draw_419;
static NJS_CNK_OBJECT draw_418;
static NJS_CNK_OBJECT draw_417;
static NJS_CNK_OBJECT draw_416;
static NJS_CNK_OBJECT draw_415;
static NJS_CNK_OBJECT draw_414;
static NJS_CNK_OBJECT draw_412;
static NJS_CNK_OBJECT draw_411;
static NJS_CNK_OBJECT draw_410;
static NJS_CNK_OBJECT draw_409;
static NJS_CNK_OBJECT draw_408;
static NJS_CNK_OBJECT draw_407;
static NJS_CNK_OBJECT draw_406;
static NJS_CNK_OBJECT draw_405;
static NJS_CNK_OBJECT draw_404;
static NJS_CNK_OBJECT draw_403;
static NJS_CNK_OBJECT draw_402;
static NJS_CNK_OBJECT draw_401;
static NJS_CNK_OBJECT draw_400;
static NJS_CNK_OBJECT draw_399;
static NJS_CNK_OBJECT draw_398;
static NJS_CNK_OBJECT draw_397;
static NJS_CNK_OBJECT draw_396;
static NJS_CNK_OBJECT draw_395;
static NJS_CNK_OBJECT draw_394;
static NJS_CNK_OBJECT draw_393;
static NJS_CNK_OBJECT draw_392;
static NJS_CNK_OBJECT draw_390;
static NJS_CNK_OBJECT draw_389;
static NJS_CNK_OBJECT draw_388;
static NJS_CNK_OBJECT draw_387;
static NJS_CNK_OBJECT draw_386;
static NJS_CNK_OBJECT draw_385;
static NJS_CNK_OBJECT draw_384;
static NJS_CNK_OBJECT draw_383;
static NJS_CNK_OBJECT draw_382;
static NJS_CNK_OBJECT draw_381;
static NJS_CNK_OBJECT draw_380;
static NJS_CNK_OBJECT draw_379;
static NJS_CNK_OBJECT draw_378;
static NJS_CNK_OBJECT draw_377;
static NJS_CNK_OBJECT draw_376;
static NJS_CNK_OBJECT draw_375;
static NJS_CNK_OBJECT draw_374;
static NJS_CNK_OBJECT draw_373;
static NJS_CNK_OBJECT draw_371;
static NJS_CNK_OBJECT draw_370;
static NJS_CNK_OBJECT draw_369;
static NJS_CNK_OBJECT draw_368;
static NJS_CNK_OBJECT draw_367;
static NJS_CNK_OBJECT draw_366;
static NJS_CNK_OBJECT draw_365;
static NJS_CNK_OBJECT draw_364;
static NJS_CNK_OBJECT draw_363;
static NJS_CNK_OBJECT draw_362;
static NJS_CNK_OBJECT draw_361;
static NJS_CNK_OBJECT draw_360;
static NJS_CNK_OBJECT draw_358;
static NJS_CNK_OBJECT draw_357;
static NJS_CNK_OBJECT draw_356;
static NJS_CNK_OBJECT draw_355;
static NJS_CNK_OBJECT draw_354;
static NJS_CNK_OBJECT draw_353;
static NJS_CNK_OBJECT draw_352;
static NJS_CNK_OBJECT draw_351;
static NJS_CNK_OBJECT draw_350;
static NJS_CNK_OBJECT draw_349;
static NJS_CNK_OBJECT draw_348;
static NJS_CNK_OBJECT draw_345;
static NJS_CNK_OBJECT draw_344;
static NJS_CNK_OBJECT draw_343;
static NJS_CNK_OBJECT draw_342;
static NJS_CNK_OBJECT draw_341;
static NJS_CNK_OBJECT draw_340;
static NJS_CNK_OBJECT draw_339;
static NJS_CNK_OBJECT draw_338;
static NJS_CNK_OBJECT draw_337;
static NJS_CNK_OBJECT draw_336;
static NJS_CNK_OBJECT draw_335;
static NJS_CNK_OBJECT draw_334;
static NJS_CNK_OBJECT draw_333;
static NJS_CNK_OBJECT draw_331;
static NJS_CNK_OBJECT draw_330;
static NJS_CNK_OBJECT draw_329;
static NJS_CNK_OBJECT draw_328;
static NJS_CNK_OBJECT draw_327;
static NJS_CNK_OBJECT draw_326;
static NJS_CNK_OBJECT draw_325;
static NJS_CNK_OBJECT draw_324;
static NJS_CNK_OBJECT draw_323;
static NJS_CNK_OBJECT draw_322;
static NJS_CNK_OBJECT draw_321;
static NJS_CNK_OBJECT draw_320;
static NJS_CNK_OBJECT draw_319;
static NJS_CNK_OBJECT draw_318;
static NJS_CNK_OBJECT draw_317;
static NJS_CNK_OBJECT draw_316;
static NJS_CNK_OBJECT draw_315;
static NJS_CNK_OBJECT draw_314;
static NJS_CNK_OBJECT draw_313;
static NJS_CNK_OBJECT draw_312;
static NJS_CNK_OBJECT draw_311;
static NJS_CNK_OBJECT draw_310;
static NJS_CNK_OBJECT draw_309;
static NJS_CNK_OBJECT draw_308;
static NJS_CNK_OBJECT draw_307;
static NJS_CNK_OBJECT draw_306;
static NJS_CNK_OBJECT draw_305;
static NJS_CNK_OBJECT draw_304;
static NJS_CNK_OBJECT draw_303;
static NJS_CNK_OBJECT draw_302;
static NJS_CNK_OBJECT draw_301;
static NJS_CNK_OBJECT draw_300;
static NJS_CNK_OBJECT draw_299;
static NJS_CNK_OBJECT draw_298;
static NJS_CNK_OBJECT draw_297;
static NJS_CNK_OBJECT draw_296;
static NJS_CNK_OBJECT draw_295;
static NJS_CNK_OBJECT draw_294;
static NJS_CNK_OBJECT draw_293;
static NJS_CNK_OBJECT draw_292;
static NJS_CNK_OBJECT draw_291;
static NJS_CNK_OBJECT draw_290;
static NJS_CNK_OBJECT draw_289;
static NJS_CNK_OBJECT draw_288;
static NJS_CNK_OBJECT draw_287;
static NJS_CNK_OBJECT draw_286;
static NJS_CNK_OBJECT draw_284;
static NJS_CNK_OBJECT draw_283;
static NJS_CNK_OBJECT draw_282;
static NJS_CNK_OBJECT draw_281;
static NJS_CNK_OBJECT draw_280;
static NJS_CNK_OBJECT draw_279;
static NJS_CNK_OBJECT draw_278;
static NJS_CNK_OBJECT draw_277;
static NJS_CNK_OBJECT draw_276;
static NJS_CNK_OBJECT draw_275;
static NJS_CNK_OBJECT draw_274;
static NJS_CNK_OBJECT draw_273;
static NJS_CNK_OBJECT draw_272;
static NJS_CNK_OBJECT draw_271;
static NJS_CNK_OBJECT draw_270;
static NJS_CNK_OBJECT draw_269;
static NJS_CNK_OBJECT draw_268;
static NJS_CNK_OBJECT draw_267;
static NJS_CNK_OBJECT draw_266;
static NJS_CNK_OBJECT draw_265;
static NJS_CNK_OBJECT draw_264;
static NJS_CNK_OBJECT draw_263;
static NJS_CNK_OBJECT draw_262;
static NJS_CNK_OBJECT draw_261;
static NJS_CNK_OBJECT draw_260;
static NJS_CNK_OBJECT draw_259;
static NJS_CNK_OBJECT draw_257;
static NJS_CNK_OBJECT draw_256;
static NJS_CNK_OBJECT draw_255;
static NJS_CNK_OBJECT draw_254;
static NJS_CNK_OBJECT draw_253;
static NJS_CNK_OBJECT draw_252;
static NJS_CNK_OBJECT draw_251;
static NJS_CNK_OBJECT draw_250;
static NJS_CNK_OBJECT draw_249;
static NJS_CNK_OBJECT draw_247;
static NJS_CNK_OBJECT draw_246;
static NJS_CNK_OBJECT draw_245;
static NJS_CNK_OBJECT draw_244;
static NJS_CNK_OBJECT draw_243;
static NJS_CNK_OBJECT draw_242;
static NJS_CNK_OBJECT draw_241;
static NJS_CNK_OBJECT draw_240;
static NJS_CNK_OBJECT draw_239;
static NJS_CNK_OBJECT draw_237;
static NJS_CNK_OBJECT draw_236;
static NJS_CNK_OBJECT draw_235;
static NJS_CNK_OBJECT draw_234;
static NJS_CNK_OBJECT draw_233;
static NJS_CNK_OBJECT draw_232;
static NJS_CNK_OBJECT draw_231;
static NJS_CNK_OBJECT draw_230;
static NJS_CNK_OBJECT draw_228;
static NJS_CNK_OBJECT draw_227;
static NJS_CNK_OBJECT draw_226;
static NJS_CNK_OBJECT draw_225;
static NJS_CNK_OBJECT draw_224;
static NJS_CNK_OBJECT draw_223;
static NJS_CNK_OBJECT draw_222;
static NJS_CNK_OBJECT draw_221;
static NJS_CNK_OBJECT draw_220;
static NJS_CNK_OBJECT draw_219;
static NJS_CNK_OBJECT draw_218;
static NJS_CNK_OBJECT draw_217;
static NJS_CNK_OBJECT draw_216;
static NJS_CNK_OBJECT draw_215;
static NJS_CNK_OBJECT draw_214;
static NJS_CNK_OBJECT draw_213;
static NJS_CNK_OBJECT draw_212;
static NJS_CNK_OBJECT draw_211;
static NJS_CNK_OBJECT draw_210;
static NJS_CNK_OBJECT draw_209;
static NJS_CNK_OBJECT draw_208;
static NJS_CNK_OBJECT draw_207;
static NJS_CNK_OBJECT draw_206;
static NJS_CNK_OBJECT draw_204;
static NJS_CNK_OBJECT draw_202;
static NJS_CNK_OBJECT draw_201;
static NJS_CNK_OBJECT draw_200;
static NJS_CNK_OBJECT draw_199;
static NJS_CNK_OBJECT draw_198;
static NJS_CNK_OBJECT draw_197;
static NJS_CNK_OBJECT draw_196;
static NJS_CNK_OBJECT draw_195;
static NJS_CNK_OBJECT draw_194;
static NJS_CNK_OBJECT draw_193;
static NJS_CNK_OBJECT draw_192;
static NJS_CNK_OBJECT draw_191;
static NJS_CNK_OBJECT draw_190;
static NJS_CNK_OBJECT draw_189;
static NJS_CNK_OBJECT draw_188;
static NJS_CNK_OBJECT draw_187;
static NJS_CNK_OBJECT draw_186;
static NJS_CNK_OBJECT draw_185;
static NJS_CNK_OBJECT draw_184;
static NJS_CNK_OBJECT draw_183;
static NJS_CNK_OBJECT draw_182;
static NJS_CNK_OBJECT draw_181;
static NJS_CNK_OBJECT draw_180;
static NJS_CNK_OBJECT draw_179;
static NJS_CNK_OBJECT draw_178;
static NJS_CNK_OBJECT draw_177;
static NJS_CNK_OBJECT draw_176;
static NJS_CNK_OBJECT draw_175;
static NJS_CNK_OBJECT draw_174;
static NJS_CNK_OBJECT draw_173;
static NJS_CNK_OBJECT draw_172;
static NJS_CNK_OBJECT draw_171;
static NJS_CNK_OBJECT draw_170;
static NJS_CNK_OBJECT draw_169;
static NJS_CNK_OBJECT draw_168;
static NJS_CNK_OBJECT draw_167;
static NJS_CNK_OBJECT draw_166;
static NJS_CNK_OBJECT draw_165;
static NJS_CNK_OBJECT draw_164;
static NJS_CNK_OBJECT draw_163;
static NJS_CNK_OBJECT draw_162;
static NJS_CNK_OBJECT draw_161;
static NJS_CNK_OBJECT draw_160;
static NJS_CNK_OBJECT draw_159;
static NJS_CNK_OBJECT draw_158;
static NJS_CNK_OBJECT draw_157;
static NJS_CNK_OBJECT draw_156;
static NJS_CNK_OBJECT draw_155;
static NJS_CNK_OBJECT draw_154;
static NJS_CNK_OBJECT draw_153;
static NJS_CNK_OBJECT draw_152;
static NJS_CNK_OBJECT draw_151;
static NJS_CNK_OBJECT draw_150;
static NJS_CNK_OBJECT draw_149;
static NJS_CNK_OBJECT draw_148;
static NJS_CNK_OBJECT draw_147;
static NJS_CNK_OBJECT draw_146;
static NJS_CNK_OBJECT draw_145;
static NJS_CNK_OBJECT draw_144;
static NJS_CNK_OBJECT draw_143;
static NJS_CNK_OBJECT draw_142;
static NJS_CNK_OBJECT draw_141;
static NJS_CNK_OBJECT draw_140;
static NJS_CNK_OBJECT draw_139;
static NJS_CNK_OBJECT draw_138;
static NJS_CNK_OBJECT draw_137;
static NJS_CNK_OBJECT draw_136;
static NJS_CNK_OBJECT draw_135;
static NJS_CNK_OBJECT draw_134;
static NJS_CNK_OBJECT draw_133;
static NJS_CNK_OBJECT draw_132;
static NJS_CNK_OBJECT draw_131;
static NJS_CNK_OBJECT draw_130;
static NJS_CNK_OBJECT draw_129;
static NJS_CNK_OBJECT draw_128;
static NJS_CNK_OBJECT draw_127;
static NJS_CNK_OBJECT draw_126;
static NJS_CNK_OBJECT draw_125;
static NJS_CNK_OBJECT draw_124;
static NJS_CNK_OBJECT draw_123;
static NJS_CNK_OBJECT draw_122;
static NJS_CNK_OBJECT draw_121;
static NJS_CNK_OBJECT draw_120;
static NJS_CNK_OBJECT draw_119;
static NJS_CNK_OBJECT draw_118;
static NJS_CNK_OBJECT draw_117;
static NJS_CNK_OBJECT draw_116;
static NJS_CNK_OBJECT draw_115;
static NJS_CNK_OBJECT draw_114;
static NJS_CNK_OBJECT draw_113;
static NJS_CNK_OBJECT draw_112;
static NJS_CNK_OBJECT draw_111;
static NJS_CNK_OBJECT draw_110;
static NJS_CNK_OBJECT draw_109;
static NJS_CNK_OBJECT draw_108;
static NJS_CNK_OBJECT draw_107;
static NJS_CNK_OBJECT draw_106;
static NJS_CNK_OBJECT draw_105;
static NJS_CNK_OBJECT draw_104;
static NJS_CNK_OBJECT draw_103;
static NJS_CNK_OBJECT draw_102;
static NJS_CNK_OBJECT draw_101;
static NJS_CNK_OBJECT draw_100;
static NJS_CNK_OBJECT draw_99;
static NJS_CNK_OBJECT draw_98;
static NJS_CNK_OBJECT draw_97;
static NJS_CNK_OBJECT draw_96;
static NJS_CNK_OBJECT draw_95;
static NJS_CNK_OBJECT draw_94;
static NJS_CNK_OBJECT draw_93;
static NJS_CNK_OBJECT draw_92;
static NJS_CNK_OBJECT draw_91;
static NJS_CNK_OBJECT draw_90;
static NJS_CNK_OBJECT draw_89;
static NJS_CNK_OBJECT draw_88;
static NJS_CNK_OBJECT draw_87;
static NJS_CNK_OBJECT draw_86;
static NJS_CNK_OBJECT draw_85;
static NJS_CNK_OBJECT draw_84;
static NJS_CNK_OBJECT draw_83;
static NJS_CNK_OBJECT draw_82;
static NJS_CNK_OBJECT draw_81;
static NJS_CNK_OBJECT draw_80;
static NJS_CNK_OBJECT draw_79;
static NJS_CNK_OBJECT draw_78;
static NJS_CNK_OBJECT draw_77;
static NJS_CNK_OBJECT draw_76;
static NJS_CNK_OBJECT draw_75;
static NJS_CNK_OBJECT draw_74;
static NJS_CNK_OBJECT draw_73;
static NJS_CNK_OBJECT draw_72;
static NJS_CNK_OBJECT draw_71;
static NJS_CNK_OBJECT draw_70;
static NJS_CNK_OBJECT draw_69;
static NJS_CNK_OBJECT draw_68;
static NJS_CNK_OBJECT draw_67;
static NJS_CNK_OBJECT draw_66;
static NJS_CNK_OBJECT draw_65;
static NJS_CNK_OBJECT draw_64;
static NJS_CNK_OBJECT draw_63;
static NJS_CNK_OBJECT draw_62;
static NJS_CNK_OBJECT draw_61;
static NJS_CNK_OBJECT draw_60;
static NJS_CNK_OBJECT draw_59;
static NJS_CNK_OBJECT draw_58;
static NJS_CNK_OBJECT draw_57;
static NJS_CNK_OBJECT draw_56;
static NJS_CNK_OBJECT draw_55;
static NJS_CNK_OBJECT draw_54;
static NJS_CNK_OBJECT draw_53;
static NJS_CNK_OBJECT draw_52;
static NJS_CNK_OBJECT draw_51;
static NJS_CNK_OBJECT draw_50;
static NJS_CNK_OBJECT draw_49;
static NJS_CNK_OBJECT draw_48;
static NJS_CNK_OBJECT draw_47;
static NJS_CNK_OBJECT draw_46;
static NJS_CNK_OBJECT draw_45;
static NJS_CNK_OBJECT draw_44;
static NJS_CNK_OBJECT draw_43;
static NJS_CNK_OBJECT draw_42;
static NJS_CNK_OBJECT draw_41;
static NJS_CNK_OBJECT draw_40;
static NJS_CNK_OBJECT draw_39;
static NJS_CNK_OBJECT draw_38;
static NJS_CNK_OBJECT draw_37;
static NJS_CNK_OBJECT draw_36;
static NJS_CNK_OBJECT draw_35;
static NJS_CNK_OBJECT draw_34;
static NJS_CNK_OBJECT draw_33;
static NJS_CNK_OBJECT draw_32;
static NJS_CNK_OBJECT draw_31;
static NJS_CNK_OBJECT draw_30;
static NJS_CNK_OBJECT draw_29;
static NJS_CNK_OBJECT draw_28;
static NJS_CNK_OBJECT draw_26;
static NJS_CNK_OBJECT draw_24;
static NJS_CNK_OBJECT draw_23;
static NJS_CNK_OBJECT draw_22;
static NJS_CNK_OBJECT draw_20;
static NJS_CNK_OBJECT draw_19;
static NJS_CNK_OBJECT draw_18;
static NJS_CNK_OBJECT draw_16;
static NJS_CNK_OBJECT draw_15;
static NJS_CNK_OBJECT draw_14;
static NJS_CNK_OBJECT draw_13;
static NJS_CNK_OBJECT draw_12;
static NJS_CNK_OBJECT draw_11;
static NJS_CNK_OBJECT draw_10;
static NJS_OBJECT collision_1;
static NJS_OBJECT collision_6;
static NJS_OBJECT collision_9;
static NJS_OBJECT collision_12;
static NJS_OBJECT collision_13;
static NJS_OBJECT collision_14;
static NJS_OBJECT collision_15;
static NJS_OBJECT collision_16;
static NJS_OBJECT collision_18;
static NJS_OBJECT collision_19;
static NJS_OBJECT collision_20;
static NJS_OBJECT collision_21;
static NJS_OBJECT collision_22;
static NJS_OBJECT collision_23;
static NJS_OBJECT collision_24;
static NJS_OBJECT collision_25;
static NJS_OBJECT collision_26;
static NJS_OBJECT collision_27;
static NJS_OBJECT collision_28;
static NJS_OBJECT collision_29;
static NJS_OBJECT collision_30;
static NJS_OBJECT collision_31;
static NJS_OBJECT collision_32;
static NJS_OBJECT collision_33;
static NJS_OBJECT collision_34;
static NJS_OBJECT collision_35;
static NJS_OBJECT collision_36;
static NJS_OBJECT collision_37;
static NJS_OBJECT collision_38;
static NJS_OBJECT collision_39;
static NJS_OBJECT collision_40;
static NJS_OBJECT collision_43;
static NJS_OBJECT collision_45;
static NJS_OBJECT collision_49;
static NJS_OBJECT collision_50;
static NJS_OBJECT collision_51;
static NJS_OBJECT collision_52;
static NJS_OBJECT collision_53;
static NJS_OBJECT collision_54;
static NJS_OBJECT collision_56;
static NJS_OBJECT collision_61;
static NJS_OBJECT collision_62;
static NJS_OBJECT collision_63;
static NJS_OBJECT collision_64;
static NJS_OBJECT collision_65;
static NJS_OBJECT collision_66;
static NJS_OBJECT collision_68;
static NJS_OBJECT collision_69;
static NJS_OBJECT collision_70;
static NJS_OBJECT collision_71;
static NJS_OBJECT collision_72;
static NJS_OBJECT collision_73;
static NJS_OBJECT collision_74;
static NJS_OBJECT collision_75;
static NJS_OBJECT collision_76;
static NJS_OBJECT collision_77;
static NJS_OBJECT collision_78;
static NJS_OBJECT collision_79;
static NJS_OBJECT collision_80;
static NJS_OBJECT collision_81;
static NJS_OBJECT collision_82;
static NJS_OBJECT collision_83;
static NJS_OBJECT collision_84;
static NJS_OBJECT collision_85;
static NJS_OBJECT collision_86;
static NJS_OBJECT collision_87;
static NJS_OBJECT collision_88;
static NJS_OBJECT collision_89;
static NJS_OBJECT collision_90;
static NJS_OBJECT collision_91;
static NJS_OBJECT collision_92;
static NJS_OBJECT collision_93;
static NJS_OBJECT collision_96;
static NJS_OBJECT collision_99;
static NJS_OBJECT collision_101;
static NJS_OBJECT collision_104;
static NJS_OBJECT collision_105;
static NJS_OBJECT collision_106;
static NJS_OBJECT collision_110;
static NJS_OBJECT collision_112;
static NJS_OBJECT collision_114;
static NJS_OBJECT collision_116;
static NJS_OBJECT collision_117;
static NJS_OBJECT collision_126;
static NJS_OBJECT collision_128;
static NJS_OBJECT collision_130;
static NJS_OBJECT collision_132;
static NJS_OBJECT collision_135;
static NJS_OBJECT collision_138;
static NJS_OBJECT collision_139;
static NJS_OBJECT collision_140;
static NJS_OBJECT collision_141;
static NJS_OBJECT collision_142;
static NJS_OBJECT collision_143;
static NJS_OBJECT collision_145;
static NJS_OBJECT collision_146;
static NJS_OBJECT collision_147;
static NJS_OBJECT collision_148;
static NJS_OBJECT collision_149;
static NJS_OBJECT collision_150;
static NJS_OBJECT collision_151;
static NJS_OBJECT collision_152;
static NJS_OBJECT collision_153;
static NJS_OBJECT collision_154;
static NJS_OBJECT collision_155;
static NJS_OBJECT collision_156;
static NJS_OBJECT collision_157;
static NJS_OBJECT collision_158;
static NJS_OBJECT collision_159;
static NJS_OBJECT collision_160;
static NJS_OBJECT collision_161;
static NJS_OBJECT collision_162;
static NJS_OBJECT collision_163;
static NJS_OBJECT collision_164;
static NJS_OBJECT collision_165;
static NJS_OBJECT collision_166;
static NJS_OBJECT collision_167;
static NJS_OBJECT collision_170;
static NJS_OBJECT collision_172;
static NJS_OBJECT collision_173;
static NJS_OBJECT collision_174;
static NJS_OBJECT collision_175;
static NJS_OBJECT collision_176;
static NJS_OBJECT collision_177;
static NJS_OBJECT collision_179;
static NJS_OBJECT collision_180;
static NJS_OBJECT collision_181;
static NJS_OBJECT collision_183;
static NJS_OBJECT collision_184;
static NJS_OBJECT collision_185;
static NJS_OBJECT collision_187;
static NJS_OBJECT collision_188;
static NJS_OBJECT collision_190;
static NJS_OBJECT collision_192;
static NJS_OBJECT collision_193;
static NJS_OBJECT collision_194;
static NJS_OBJECT collision_195;
static NJS_OBJECT collision_196;
static NJS_OBJECT collision_197;
static NJS_OBJECT collision_198;
static NJS_OBJECT collision_199;
static NJS_OBJECT collision_200;
static NJS_OBJECT collision_201;
static NJS_OBJECT collision_202;
static NJS_OBJECT collision_203;
static NJS_OBJECT collision_204;
static NJS_OBJECT collision_205;
static NJS_OBJECT collision_206;
static NJS_OBJECT collision_207;
static NJS_OBJECT collision_208;
static NJS_OBJECT collision_209;
static NJS_OBJECT collision_210;
static NJS_OBJECT collision_211;
static NJS_OBJECT collision_212;
static NJS_OBJECT collision_213;
static NJS_OBJECT collision_214;
static NJS_OBJECT collision_215;
static NJS_OBJECT collision_216;
static NJS_OBJECT collision_217;
static NJS_OBJECT collision_218;
static NJS_OBJECT collision_219;
static NJS_OBJECT collision_220;
static NJS_OBJECT collision_221;
static NJS_OBJECT collision_222;
static NJS_OBJECT collision_224;
static NJS_OBJECT collision_226;
static NJS_OBJECT collision_228;
static NJS_OBJECT collision_229;
static NJS_OBJECT collision_230;
static NJS_OBJECT collision_235;
static NJS_OBJECT collision_237;
static NJS_OBJECT collision_239;
static NJS_OBJECT collision_241;
static NJS_OBJECT collision_243;
static NJS_OBJECT collision_245;
static NJS_OBJECT collision_246;
static NJS_OBJECT collision_248;
static NJS_OBJECT collision_253;
static NJS_OBJECT collision_254;
static NJS_OBJECT collision_255;
static NJS_OBJECT collision_256;
static NJS_OBJECT collision_257;
static NJS_OBJECT collision_258;
static NJS_OBJECT collision_259;
static NJS_OBJECT collision_260;
static NJS_OBJECT collision_261;
static NJS_OBJECT collision_267;
static NJS_OBJECT collision_268;
static NJS_OBJECT collision_269;
static NJS_OBJECT collision_270;
static NJS_OBJECT collision_271;
static NJS_OBJECT collision_301;
static NJS_OBJECT collision_302;
static NJS_OBJECT collision_303;
static NJS_OBJECT collision_304;

static struct land_entry entries[1291] = {
#include "stg22/terrain/entries.inc"
};
struct land_table crazy_gadget_land = {
#include "stg22/terrain/crazy_gadget_land.inc"
};
static NJS_TEXNAME textures[52] = {
#include "stg22/terrain/textures.inc"
};
NJS_TEXLIST gadget_land_texlist = {
#include "stg22/terrain/gadget_land_texlist.inc"
};
static Sint16 draw_211_polygons[1339] = {
#include "stg22/terrain/draw_211_polygons.inc"
};
static Sint32 draw_211_vertices[1463] = {
#include "stg22/terrain/draw_211_vertices.inc"
};
static NJS_CNK_MODEL draw_211_model = {
#include "stg22/terrain/draw_211_model.inc"
};
static NJS_CNK_OBJECT draw_975 = {
#include "stg22/terrain/draw_975.inc"
};
static Sint16 draw_294_polygons[327] = {
#include "stg22/terrain/draw_294_polygons.inc"
};
static Sint32 draw_294_vertices[367] = {
#include "stg22/terrain/draw_294_vertices.inc"
};
static NJS_CNK_MODEL draw_294_model = {
#include "stg22/terrain/draw_294_model.inc"
};
static NJS_CNK_OBJECT draw_973 = {
#include "stg22/terrain/draw_973.inc"
};
static Sint16 draw_972_polygons[829] = {
#include "stg22/terrain/draw_972_polygons.inc"
};
static Sint32 draw_972_vertices[795] = {
#include "stg22/terrain/draw_972_vertices.inc"
};
static NJS_CNK_MODEL draw_972_model = {
#include "stg22/terrain/draw_972_model.inc"
};
static NJS_CNK_OBJECT draw_972 = {
#include "stg22/terrain/draw_972.inc"
};
static Sint16 draw_212_polygons[1053] = {
#include "stg22/terrain/draw_212_polygons.inc"
};
static Sint32 draw_212_vertices[563] = {
#include "stg22/terrain/draw_212_vertices.inc"
};
static NJS_CNK_MODEL draw_212_model = {
#include "stg22/terrain/draw_212_model.inc"
};
static NJS_CNK_OBJECT draw_971 = {
#include "stg22/terrain/draw_971.inc"
};
static Sint16 draw_267_polygons[1271] = {
#include "stg22/terrain/draw_267_polygons.inc"
};
static Sint32 draw_267_vertices[1059] = {
#include "stg22/terrain/draw_267_vertices.inc"
};
static NJS_CNK_MODEL draw_267_model = {
#include "stg22/terrain/draw_267_model.inc"
};
static NJS_CNK_OBJECT draw_968 = {
#include "stg22/terrain/draw_968.inc"
};
static Sint16 draw_265_polygons[147] = {
#include "stg22/terrain/draw_265_polygons.inc"
};
static Sint32 draw_265_vertices[139] = {
#include "stg22/terrain/draw_265_vertices.inc"
};
static NJS_CNK_MODEL draw_265_model = {
#include "stg22/terrain/draw_265_model.inc"
};
static NJS_CNK_OBJECT draw_967 = {
#include "stg22/terrain/draw_967.inc"
};
static Sint16 draw_262_polygons[371] = {
#include "stg22/terrain/draw_262_polygons.inc"
};
static Sint32 draw_262_vertices[323] = {
#include "stg22/terrain/draw_262_vertices.inc"
};
static NJS_CNK_MODEL draw_262_model = {
#include "stg22/terrain/draw_262_model.inc"
};
static NJS_CNK_OBJECT draw_963 = {
#include "stg22/terrain/draw_963.inc"
};
static Sint16 draw_207_polygons[941] = {
#include "stg22/terrain/draw_207_polygons.inc"
};
static Sint32 draw_207_vertices[563] = {
#include "stg22/terrain/draw_207_vertices.inc"
};
static NJS_CNK_MODEL draw_207_model = {
#include "stg22/terrain/draw_207_model.inc"
};
static NJS_CNK_OBJECT draw_961 = {
#include "stg22/terrain/draw_961.inc"
};
static Sint16 draw_957_polygons[109] = {
#include "stg22/terrain/draw_957_polygons.inc"
};
static Sint32 draw_957_vertices[51] = {
#include "stg22/terrain/draw_957_vertices.inc"
};
static NJS_CNK_MODEL draw_957_model = {
#include "stg22/terrain/draw_957_model.inc"
};
static NJS_CNK_OBJECT draw_957 = {
#include "stg22/terrain/draw_957.inc"
};
static Sint16 draw_578_polygons[805] = {
#include "stg22/terrain/draw_578_polygons.inc"
};
static Sint32 draw_578_vertices[647] = {
#include "stg22/terrain/draw_578_vertices.inc"
};
static NJS_CNK_MODEL draw_578_model = {
#include "stg22/terrain/draw_578_model.inc"
};
static NJS_CNK_OBJECT draw_956 = {
#include "stg22/terrain/draw_956.inc"
};
static Sint16 draw_280_polygons[1545] = {
#include "stg22/terrain/draw_280_polygons.inc"
};
static Sint32 draw_280_vertices[479] = {
#include "stg22/terrain/draw_280_vertices.inc"
};
static NJS_CNK_MODEL draw_280_model = {
#include "stg22/terrain/draw_280_model.inc"
};
static NJS_CNK_OBJECT draw_955 = {
#include "stg22/terrain/draw_955.inc"
};
static Sint16 draw_278_polygons[1545] = {
#include "stg22/terrain/draw_278_polygons.inc"
};
static Sint32 draw_278_vertices[479] = {
#include "stg22/terrain/draw_278_vertices.inc"
};
static NJS_CNK_MODEL draw_278_model = {
#include "stg22/terrain/draw_278_model.inc"
};
static NJS_CNK_OBJECT draw_954 = {
#include "stg22/terrain/draw_954.inc"
};
static Sint16 draw_283_polygons[1939] = {
#include "stg22/terrain/draw_283_polygons.inc"
};
static Sint32 draw_283_vertices[2139] = {
#include "stg22/terrain/draw_283_vertices.inc"
};
static NJS_CNK_MODEL draw_283_model = {
#include "stg22/terrain/draw_283_model.inc"
};
static NJS_CNK_OBJECT draw_953 = {
#include "stg22/terrain/draw_953.inc"
};
static Sint16 draw_564_polygons[877] = {
#include "stg22/terrain/draw_564_polygons.inc"
};
static Sint32 draw_564_vertices[555] = {
#include "stg22/terrain/draw_564_vertices.inc"
};
static NJS_CNK_MODEL draw_564_model = {
#include "stg22/terrain/draw_564_model.inc"
};
static NJS_CNK_OBJECT draw_951 = {
#include "stg22/terrain/draw_951.inc"
};
static Sint16 draw_580_polygons[167] = {
#include "stg22/terrain/draw_580_polygons.inc"
};
static Sint32 draw_580_vertices[151] = {
#include "stg22/terrain/draw_580_vertices.inc"
};
static NJS_CNK_MODEL draw_580_model = {
#include "stg22/terrain/draw_580_model.inc"
};
static NJS_CNK_OBJECT draw_950 = {
#include "stg22/terrain/draw_950.inc"
};
static Sint16 draw_574_polygons[55] = {
#include "stg22/terrain/draw_574_polygons.inc"
};
static Sint32 draw_574_vertices[51] = {
#include "stg22/terrain/draw_574_vertices.inc"
};
static NJS_CNK_MODEL draw_574_model = {
#include "stg22/terrain/draw_574_model.inc"
};
static NJS_CNK_OBJECT draw_949 = {
#include "stg22/terrain/draw_949.inc"
};
static Sint16 draw_573_polygons[167] = {
#include "stg22/terrain/draw_573_polygons.inc"
};
static Sint32 draw_573_vertices[159] = {
#include "stg22/terrain/draw_573_vertices.inc"
};
static NJS_CNK_MODEL draw_573_model = {
#include "stg22/terrain/draw_573_model.inc"
};
static NJS_CNK_OBJECT draw_945 = {
#include "stg22/terrain/draw_945.inc"
};
static Sint16 draw_558_polygons[317] = {
#include "stg22/terrain/draw_558_polygons.inc"
};
static Sint32 draw_558_vertices[347] = {
#include "stg22/terrain/draw_558_vertices.inc"
};
static NJS_CNK_MODEL draw_558_model = {
#include "stg22/terrain/draw_558_model.inc"
};
static NJS_CNK_OBJECT draw_943 = {
#include "stg22/terrain/draw_943.inc"
};
static Sint16 draw_427_polygons[861] = {
#include "stg22/terrain/draw_427_polygons.inc"
};
static Sint32 draw_427_vertices[659] = {
#include "stg22/terrain/draw_427_vertices.inc"
};
static NJS_CNK_MODEL draw_427_model = {
#include "stg22/terrain/draw_427_model.inc"
};
static NJS_CNK_OBJECT draw_940 = {
#include "stg22/terrain/draw_940.inc"
};
static Sint16 draw_565_polygons[675] = {
#include "stg22/terrain/draw_565_polygons.inc"
};
static Sint32 draw_565_vertices[531] = {
#include "stg22/terrain/draw_565_vertices.inc"
};
static NJS_CNK_MODEL draw_565_model = {
#include "stg22/terrain/draw_565_model.inc"
};
static NJS_CNK_OBJECT draw_937 = {
#include "stg22/terrain/draw_937.inc"
};
static Sint16 draw_552_polygons[149] = {
#include "stg22/terrain/draw_552_polygons.inc"
};
static Sint32 draw_552_vertices[163] = {
#include "stg22/terrain/draw_552_vertices.inc"
};
static NJS_CNK_MODEL draw_552_model = {
#include "stg22/terrain/draw_552_model.inc"
};
static NJS_CNK_OBJECT draw_933 = {
#include "stg22/terrain/draw_933.inc"
};
static Sint16 draw_550_polygons[221] = {
#include "stg22/terrain/draw_550_polygons.inc"
};
static Sint32 draw_550_vertices[243] = {
#include "stg22/terrain/draw_550_vertices.inc"
};
static NJS_CNK_MODEL draw_550_model = {
#include "stg22/terrain/draw_550_model.inc"
};
static NJS_CNK_OBJECT draw_926 = {
#include "stg22/terrain/draw_926.inc"
};
static Sint16 draw_206_polygons[383] = {
#include "stg22/terrain/draw_206_polygons.inc"
};
static Sint32 draw_206_vertices[355] = {
#include "stg22/terrain/draw_206_vertices.inc"
};
static NJS_CNK_MODEL draw_206_model = {
#include "stg22/terrain/draw_206_model.inc"
};
static NJS_CNK_OBJECT draw_924 = {
#include "stg22/terrain/draw_924.inc"
};
static Sint16 draw_266_polygons[355] = {
#include "stg22/terrain/draw_266_polygons.inc"
};
static Sint32 draw_266_vertices[283] = {
#include "stg22/terrain/draw_266_vertices.inc"
};
static NJS_CNK_MODEL draw_266_model = {
#include "stg22/terrain/draw_266_model.inc"
};
static NJS_CNK_OBJECT draw_919 = {
#include "stg22/terrain/draw_919.inc"
};
static Sint16 draw_370_polygons[57] = {
#include "stg22/terrain/draw_370_polygons.inc"
};
static Sint32 draw_370_vertices[35] = {
#include "stg22/terrain/draw_370_vertices.inc"
};
static NJS_CNK_MODEL draw_370_model = {
#include "stg22/terrain/draw_370_model.inc"
};
static NJS_CNK_OBJECT draw_902 = {
#include "stg22/terrain/draw_902.inc"
};
static Sint16 draw_259_polygons[565] = {
#include "stg22/terrain/draw_259_polygons.inc"
};
static Sint32 draw_259_vertices[515] = {
#include "stg22/terrain/draw_259_vertices.inc"
};
static NJS_CNK_MODEL draw_259_model = {
#include "stg22/terrain/draw_259_model.inc"
};
static NJS_CNK_OBJECT draw_893 = {
#include "stg22/terrain/draw_893.inc"
};
static Sint16 draw_257_polygons[1967] = {
#include "stg22/terrain/draw_257_polygons.inc"
};
static Sint32 draw_257_vertices[1187] = {
#include "stg22/terrain/draw_257_vertices.inc"
};
static NJS_CNK_MODEL draw_257_model = {
#include "stg22/terrain/draw_257_model.inc"
};
static NJS_CNK_OBJECT draw_892 = {
#include "stg22/terrain/draw_892.inc"
};
static Sint16 draw_256_polygons[1357] = {
#include "stg22/terrain/draw_256_polygons.inc"
};
static Sint32 draw_256_vertices[1331] = {
#include "stg22/terrain/draw_256_vertices.inc"
};
static NJS_CNK_MODEL draw_256_model = {
#include "stg22/terrain/draw_256_model.inc"
};
static NJS_CNK_OBJECT draw_891 = {
#include "stg22/terrain/draw_891.inc"
};
static Sint16 draw_842_polygons[539] = {
#include "stg22/terrain/draw_842_polygons.inc"
};
static Sint32 draw_842_vertices[483] = {
#include "stg22/terrain/draw_842_vertices.inc"
};
static NJS_CNK_MODEL draw_842_model = {
#include "stg22/terrain/draw_842_model.inc"
};
static NJS_CNK_OBJECT draw_889 = {
#include "stg22/terrain/draw_889.inc"
};
static Sint16 draw_333_polygons[331] = {
#include "stg22/terrain/draw_333_polygons.inc"
};
static Sint32 draw_333_vertices[291] = {
#include "stg22/terrain/draw_333_vertices.inc"
};
static NJS_CNK_MODEL draw_333_model = {
#include "stg22/terrain/draw_333_model.inc"
};
static NJS_CNK_OBJECT draw_888 = {
#include "stg22/terrain/draw_888.inc"
};
static Sint16 draw_854_polygons[651] = {
#include "stg22/terrain/draw_854_polygons.inc"
};
static Sint32 draw_854_vertices[1263] = {
#include "stg22/terrain/draw_854_vertices.inc"
};
static NJS_CNK_MODEL draw_854_model = {
#include "stg22/terrain/draw_854_model.inc"
};
static NJS_CNK_OBJECT draw_854 = {
#include "stg22/terrain/draw_854.inc"
};
static Sint16 draw_818_polygons[539] = {
#include "stg22/terrain/draw_818_polygons.inc"
};
static Sint32 draw_818_vertices[483] = {
#include "stg22/terrain/draw_818_vertices.inc"
};
static NJS_CNK_MODEL draw_818_model = {
#include "stg22/terrain/draw_818_model.inc"
};
static NJS_CNK_OBJECT draw_841 = {
#include "stg22/terrain/draw_841.inc"
};
static Sint16 draw_220_polygons[591] = {
#include "stg22/terrain/draw_220_polygons.inc"
};
static Sint32 draw_220_vertices[635] = {
#include "stg22/terrain/draw_220_vertices.inc"
};
static NJS_CNK_MODEL draw_220_model = {
#include "stg22/terrain/draw_220_model.inc"
};
static NJS_CNK_OBJECT draw_817 = {
#include "stg22/terrain/draw_817.inc"
};
static Sint16 draw_815_polygons[163] = {
#include "stg22/terrain/draw_815_polygons.inc"
};
static Sint32 draw_815_vertices[163] = {
#include "stg22/terrain/draw_815_vertices.inc"
};
static NJS_CNK_MODEL draw_815_model = {
#include "stg22/terrain/draw_815_model.inc"
};
static NJS_CNK_OBJECT draw_815 = {
#include "stg22/terrain/draw_815.inc"
};
static Sint16 draw_458_polygons[323] = {
#include "stg22/terrain/draw_458_polygons.inc"
};
static Sint32 draw_458_vertices[363] = {
#include "stg22/terrain/draw_458_vertices.inc"
};
static NJS_CNK_MODEL draw_458_model = {
#include "stg22/terrain/draw_458_model.inc"
};
static NJS_CNK_OBJECT draw_812 = {
#include "stg22/terrain/draw_812.inc"
};
static Sint16 draw_452_polygons[163] = {
#include "stg22/terrain/draw_452_polygons.inc"
};
static Sint32 draw_452_vertices[163] = {
#include "stg22/terrain/draw_452_vertices.inc"
};
static NJS_CNK_MODEL draw_452_model = {
#include "stg22/terrain/draw_452_model.inc"
};
static NJS_CNK_OBJECT draw_810 = {
#include "stg22/terrain/draw_810.inc"
};
static Sint16 draw_451_polygons[79] = {
#include "stg22/terrain/draw_451_polygons.inc"
};
static Sint32 draw_451_vertices[83] = {
#include "stg22/terrain/draw_451_vertices.inc"
};
static NJS_CNK_MODEL draw_451_model = {
#include "stg22/terrain/draw_451_model.inc"
};
static NJS_CNK_OBJECT draw_805 = {
#include "stg22/terrain/draw_805.inc"
};
static Sint16 draw_418_polygons[211] = {
#include "stg22/terrain/draw_418_polygons.inc"
};
static Sint32 draw_418_vertices[235] = {
#include "stg22/terrain/draw_418_vertices.inc"
};
static NJS_CNK_MODEL draw_418_model = {
#include "stg22/terrain/draw_418_model.inc"
};
static NJS_CNK_OBJECT draw_803 = {
#include "stg22/terrain/draw_803.inc"
};
static Sint16 draw_450_polygons[273] = {
#include "stg22/terrain/draw_450_polygons.inc"
};
static Sint32 draw_450_vertices[243] = {
#include "stg22/terrain/draw_450_vertices.inc"
};
static NJS_CNK_MODEL draw_450_model = {
#include "stg22/terrain/draw_450_model.inc"
};
static NJS_CNK_OBJECT draw_802 = {
#include "stg22/terrain/draw_802.inc"
};
static Sint16 draw_796_polygons[163] = {
#include "stg22/terrain/draw_796_polygons.inc"
};
static Sint32 draw_796_vertices[163] = {
#include "stg22/terrain/draw_796_vertices.inc"
};
static NJS_CNK_MODEL draw_796_model = {
#include "stg22/terrain/draw_796_model.inc"
};
static NJS_CNK_OBJECT draw_796 = {
#include "stg22/terrain/draw_796.inc"
};
static Sint16 draw_443_polygons[227] = {
#include "stg22/terrain/draw_443_polygons.inc"
};
static Sint32 draw_443_vertices[171] = {
#include "stg22/terrain/draw_443_vertices.inc"
};
static NJS_CNK_MODEL draw_443_model = {
#include "stg22/terrain/draw_443_model.inc"
};
static NJS_CNK_OBJECT draw_795 = {
#include "stg22/terrain/draw_795.inc"
};
static Sint16 draw_460_polygons[339] = {
#include "stg22/terrain/draw_460_polygons.inc"
};
static Sint32 draw_460_vertices[251] = {
#include "stg22/terrain/draw_460_vertices.inc"
};
static NJS_CNK_MODEL draw_460_model = {
#include "stg22/terrain/draw_460_model.inc"
};
static NJS_CNK_OBJECT draw_794 = {
#include "stg22/terrain/draw_794.inc"
};
static Sint16 draw_441_polygons[301] = {
#include "stg22/terrain/draw_441_polygons.inc"
};
static Sint32 draw_441_vertices[207] = {
#include "stg22/terrain/draw_441_vertices.inc"
};
static NJS_CNK_MODEL draw_441_model = {
#include "stg22/terrain/draw_441_model.inc"
};
static NJS_CNK_OBJECT draw_793 = {
#include "stg22/terrain/draw_793.inc"
};
static Sint16 draw_440_polygons[69] = {
#include "stg22/terrain/draw_440_polygons.inc"
};
static Sint32 draw_440_vertices[51] = {
#include "stg22/terrain/draw_440_vertices.inc"
};
static NJS_CNK_MODEL draw_440_model = {
#include "stg22/terrain/draw_440_model.inc"
};
static NJS_CNK_OBJECT draw_792 = {
#include "stg22/terrain/draw_792.inc"
};
static Sint16 draw_439_polygons[135] = {
#include "stg22/terrain/draw_439_polygons.inc"
};
static Sint32 draw_439_vertices[123] = {
#include "stg22/terrain/draw_439_vertices.inc"
};
static NJS_CNK_MODEL draw_439_model = {
#include "stg22/terrain/draw_439_model.inc"
};
static NJS_CNK_OBJECT draw_791 = {
#include "stg22/terrain/draw_791.inc"
};
static Sint16 draw_438_polygons[533] = {
#include "stg22/terrain/draw_438_polygons.inc"
};
static Sint32 draw_438_vertices[411] = {
#include "stg22/terrain/draw_438_vertices.inc"
};
static NJS_CNK_MODEL draw_438_model = {
#include "stg22/terrain/draw_438_model.inc"
};
static NJS_CNK_OBJECT draw_790 = {
#include "stg22/terrain/draw_790.inc"
};
static Sint16 draw_424_polygons[373] = {
#include "stg22/terrain/draw_424_polygons.inc"
};
static Sint32 draw_424_vertices[291] = {
#include "stg22/terrain/draw_424_vertices.inc"
};
static NJS_CNK_MODEL draw_424_model = {
#include "stg22/terrain/draw_424_model.inc"
};
static NJS_CNK_OBJECT draw_789 = {
#include "stg22/terrain/draw_789.inc"
};
static Sint16 draw_426_polygons[389] = {
#include "stg22/terrain/draw_426_polygons.inc"
};
static Sint32 draw_426_vertices[391] = {
#include "stg22/terrain/draw_426_vertices.inc"
};
static NJS_CNK_MODEL draw_426_model = {
#include "stg22/terrain/draw_426_model.inc"
};
static NJS_CNK_OBJECT draw_779 = {
#include "stg22/terrain/draw_779.inc"
};
static Sint16 draw_425_polygons[389] = {
#include "stg22/terrain/draw_425_polygons.inc"
};
static Sint32 draw_425_vertices[391] = {
#include "stg22/terrain/draw_425_vertices.inc"
};
static NJS_CNK_MODEL draw_425_model = {
#include "stg22/terrain/draw_425_model.inc"
};
static NJS_CNK_OBJECT draw_778 = {
#include "stg22/terrain/draw_778.inc"
};
static Sint16 draw_416_polygons[149] = {
#include "stg22/terrain/draw_416_polygons.inc"
};
static Sint32 draw_416_vertices[159] = {
#include "stg22/terrain/draw_416_vertices.inc"
};
static NJS_CNK_MODEL draw_416_model = {
#include "stg22/terrain/draw_416_model.inc"
};
static NJS_CNK_OBJECT draw_776 = {
#include "stg22/terrain/draw_776.inc"
};
static Sint16 draw_773_polygons[239] = {
#include "stg22/terrain/draw_773_polygons.inc"
};
static Sint32 draw_773_vertices[203] = {
#include "stg22/terrain/draw_773_vertices.inc"
};
static NJS_CNK_MODEL draw_773_model = {
#include "stg22/terrain/draw_773_model.inc"
};
static NJS_CNK_OBJECT draw_773 = {
#include "stg22/terrain/draw_773.inc"
};
static Sint16 draw_422_polygons[439] = {
#include "stg22/terrain/draw_422_polygons.inc"
};
static Sint32 draw_422_vertices[243] = {
#include "stg22/terrain/draw_422_vertices.inc"
};
static NJS_CNK_MODEL draw_422_model = {
#include "stg22/terrain/draw_422_model.inc"
};
static NJS_CNK_OBJECT draw_772 = {
#include "stg22/terrain/draw_772.inc"
};
static Sint16 draw_367_polygons[295] = {
#include "stg22/terrain/draw_367_polygons.inc"
};
static Sint32 draw_367_vertices[131] = {
#include "stg22/terrain/draw_367_vertices.inc"
};
static NJS_CNK_MODEL draw_367_model = {
#include "stg22/terrain/draw_367_model.inc"
};
static NJS_CNK_OBJECT draw_770 = {
#include "stg22/terrain/draw_770.inc"
};
static Sint16 draw_369_polygons[239] = {
#include "stg22/terrain/draw_369_polygons.inc"
};
static Sint32 draw_369_vertices[259] = {
#include "stg22/terrain/draw_369_vertices.inc"
};
static NJS_CNK_MODEL draw_369_model = {
#include "stg22/terrain/draw_369_model.inc"
};
static NJS_CNK_OBJECT draw_766 = {
#include "stg22/terrain/draw_766.inc"
};
static Sint16 draw_402_polygons[799] = {
#include "stg22/terrain/draw_402_polygons.inc"
};
static Sint32 draw_402_vertices[643] = {
#include "stg22/terrain/draw_402_vertices.inc"
};
static NJS_CNK_MODEL draw_402_model = {
#include "stg22/terrain/draw_402_model.inc"
};
static NJS_CNK_OBJECT draw_765 = {
#include "stg22/terrain/draw_765.inc"
};
static Sint16 draw_757_polygons[37] = {
#include "stg22/terrain/draw_757_polygons.inc"
};
static Sint32 draw_757_vertices[35] = {
#include "stg22/terrain/draw_757_vertices.inc"
};
static NJS_CNK_MODEL draw_757_model = {
#include "stg22/terrain/draw_757_model.inc"
};
static NJS_CNK_OBJECT draw_760 = {
#include "stg22/terrain/draw_760.inc"
};
static Sint16 draw_756_polygons[281] = {
#include "stg22/terrain/draw_756_polygons.inc"
};
static Sint32 draw_756_vertices[179] = {
#include "stg22/terrain/draw_756_vertices.inc"
};
static NJS_CNK_MODEL draw_756_model = {
#include "stg22/terrain/draw_756_model.inc"
};
static NJS_CNK_OBJECT draw_756 = {
#include "stg22/terrain/draw_756.inc"
};
static Sint16 draw_234_polygons[247] = {
#include "stg22/terrain/draw_234_polygons.inc"
};
static Sint32 draw_234_vertices[183] = {
#include "stg22/terrain/draw_234_vertices.inc"
};
NJS_CNK_MODEL gadget_pulse_marker_a = {
#include "stg22/terrain/gadget_pulse_marker_a.inc"
};
static NJS_CNK_OBJECT draw_755 = {
#include "stg22/terrain/draw_755.inc"
};
static Sint16 draw_754_polygons[145] = {
#include "stg22/terrain/draw_754_polygons.inc"
};
static Sint32 draw_754_vertices[91] = {
#include "stg22/terrain/draw_754_vertices.inc"
};
static NJS_CNK_MODEL draw_754_model = {
#include "stg22/terrain/draw_754_model.inc"
};
static NJS_CNK_OBJECT draw_754 = {
#include "stg22/terrain/draw_754.inc"
};
static Sint16 draw_694_polygons[63] = {
#include "stg22/terrain/draw_694_polygons.inc"
};
static Sint32 draw_694_vertices[35] = {
#include "stg22/terrain/draw_694_vertices.inc"
};
static NJS_CNK_MODEL draw_694_model = {
#include "stg22/terrain/draw_694_model.inc"
};
static NJS_CNK_OBJECT draw_694 = {
#include "stg22/terrain/draw_694.inc"
};
static Sint16 draw_641_polygons[117] = {
#include "stg22/terrain/draw_641_polygons.inc"
};
static Sint32 draw_641_vertices[67] = {
#include "stg22/terrain/draw_641_vertices.inc"
};
static NJS_CNK_MODEL draw_641_model = {
#include "stg22/terrain/draw_641_model.inc"
};
static NJS_CNK_OBJECT draw_641 = {
#include "stg22/terrain/draw_641.inc"
};
static Sint16 draw_226_polygons[169] = {
#include "stg22/terrain/draw_226_polygons.inc"
};
static Sint32 draw_226_vertices[123] = {
#include "stg22/terrain/draw_226_vertices.inc"
};
NJS_CNK_MODEL gadget_pulse_marker_b = {
#include "stg22/terrain/gadget_pulse_marker_b.inc"
};
static NJS_CNK_OBJECT draw_640 = {
#include "stg22/terrain/draw_640.inc"
};
static Sint16 draw_638_polygons[147] = {
#include "stg22/terrain/draw_638_polygons.inc"
};
static Sint32 draw_638_vertices[83] = {
#include "stg22/terrain/draw_638_vertices.inc"
};
static NJS_CNK_MODEL draw_638_model = {
#include "stg22/terrain/draw_638_model.inc"
};
static NJS_CNK_OBJECT draw_638 = {
#include "stg22/terrain/draw_638.inc"
};
static Sint16 draw_636_polygons[323] = {
#include "stg22/terrain/draw_636_polygons.inc"
};
static Sint32 draw_636_vertices[179] = {
#include "stg22/terrain/draw_636_vertices.inc"
};
static NJS_CNK_MODEL draw_636_model = {
#include "stg22/terrain/draw_636_model.inc"
};
static NJS_CNK_OBJECT draw_636 = {
#include "stg22/terrain/draw_636.inc"
};
static Sint16 draw_635_polygons[121] = {
#include "stg22/terrain/draw_635_polygons.inc"
};
static Sint32 draw_635_vertices[67] = {
#include "stg22/terrain/draw_635_vertices.inc"
};
static NJS_CNK_MODEL draw_635_model = {
#include "stg22/terrain/draw_635_model.inc"
};
static NJS_CNK_OBJECT draw_635 = {
#include "stg22/terrain/draw_635.inc"
};
static Sint16 draw_634_polygons[103] = {
#include "stg22/terrain/draw_634_polygons.inc"
};
static Sint32 draw_634_vertices[51] = {
#include "stg22/terrain/draw_634_vertices.inc"
};
static NJS_CNK_MODEL draw_634_model = {
#include "stg22/terrain/draw_634_model.inc"
};
static NJS_CNK_OBJECT draw_634 = {
#include "stg22/terrain/draw_634.inc"
};
static Sint16 draw_633_polygons[705] = {
#include "stg22/terrain/draw_633_polygons.inc"
};
static Sint32 draw_633_vertices[547] = {
#include "stg22/terrain/draw_633_vertices.inc"
};
static NJS_CNK_MODEL draw_633_model = {
#include "stg22/terrain/draw_633_model.inc"
};
static NJS_CNK_OBJECT draw_633 = {
#include "stg22/terrain/draw_633.inc"
};
static Sint16 draw_629_polygons[43] = {
#include "stg22/terrain/draw_629_polygons.inc"
};
static Sint32 draw_629_vertices[43] = {
#include "stg22/terrain/draw_629_vertices.inc"
};
static NJS_CNK_MODEL draw_629_model = {
#include "stg22/terrain/draw_629_model.inc"
};
static NJS_CNK_OBJECT draw_632 = {
#include "stg22/terrain/draw_632.inc"
};
static Sint16 draw_329_polygons[123] = {
#include "stg22/terrain/draw_329_polygons.inc"
};
static Sint32 draw_329_vertices[51] = {
#include "stg22/terrain/draw_329_vertices.inc"
};
static NJS_CNK_MODEL draw_329_model = {
#include "stg22/terrain/draw_329_model.inc"
};
static NJS_CNK_OBJECT draw_628 = {
#include "stg22/terrain/draw_628.inc"
};
static Sint16 draw_252_polygons[619] = {
#include "stg22/terrain/draw_252_polygons.inc"
};
static Sint32 draw_252_vertices[475] = {
#include "stg22/terrain/draw_252_vertices.inc"
};
static NJS_CNK_MODEL draw_252_model = {
#include "stg22/terrain/draw_252_model.inc"
};
static NJS_CNK_OBJECT draw_608 = {
#include "stg22/terrain/draw_608.inc"
};
static Sint16 draw_251_polygons[135] = {
#include "stg22/terrain/draw_251_polygons.inc"
};
static Sint32 draw_251_vertices[83] = {
#include "stg22/terrain/draw_251_vertices.inc"
};
static NJS_CNK_MODEL draw_251_model = {
#include "stg22/terrain/draw_251_model.inc"
};
static NJS_CNK_OBJECT draw_603 = {
#include "stg22/terrain/draw_603.inc"
};
static Sint16 draw_250_polygons[227] = {
#include "stg22/terrain/draw_250_polygons.inc"
};
static Sint32 draw_250_vertices[167] = {
#include "stg22/terrain/draw_250_vertices.inc"
};
static NJS_CNK_MODEL draw_250_model = {
#include "stg22/terrain/draw_250_model.inc"
};
static NJS_CNK_OBJECT draw_602 = {
#include "stg22/terrain/draw_602.inc"
};
static Sint16 draw_249_polygons[111] = {
#include "stg22/terrain/draw_249_polygons.inc"
};
static Sint32 draw_249_vertices[91] = {
#include "stg22/terrain/draw_249_vertices.inc"
};
static NJS_CNK_MODEL draw_249_model = {
#include "stg22/terrain/draw_249_model.inc"
};
static NJS_CNK_OBJECT draw_601 = {
#include "stg22/terrain/draw_601.inc"
};
static Sint16 draw_219_polygons[173] = {
#include "stg22/terrain/draw_219_polygons.inc"
};
static Sint32 draw_219_vertices[99] = {
#include "stg22/terrain/draw_219_vertices.inc"
};
static NJS_CNK_MODEL draw_219_model = {
#include "stg22/terrain/draw_219_model.inc"
};
static NJS_CNK_OBJECT draw_600 = {
#include "stg22/terrain/draw_600.inc"
};
static Sint16 draw_546_polygons[25] = {
#include "stg22/terrain/draw_546_polygons.inc"
};
static Sint32 draw_546_vertices[19] = {
#include "stg22/terrain/draw_546_vertices.inc"
};
static NJS_CNK_MODEL draw_546_model = {
#include "stg22/terrain/draw_546_model.inc"
};
static NJS_CNK_OBJECT draw_546 = {
#include "stg22/terrain/draw_546.inc"
};
static Sint16 draw_317_polygons[955] = {
#include "stg22/terrain/draw_317_polygons.inc"
};
static Sint32 draw_317_vertices[867] = {
#include "stg22/terrain/draw_317_vertices.inc"
};
static NJS_CNK_MODEL draw_317_model = {
#include "stg22/terrain/draw_317_model.inc"
};
static NJS_CNK_OBJECT draw_544 = {
#include "stg22/terrain/draw_544.inc"
};
static Sint16 draw_515_polygons[789] = {
#include "stg22/terrain/draw_515_polygons.inc"
};
static Sint32 draw_515_vertices[1543] = {
#include "stg22/terrain/draw_515_vertices.inc"
};
static NJS_CNK_MODEL draw_515_model = {
#include "stg22/terrain/draw_515_model.inc"
};
static NJS_CNK_OBJECT draw_515 = {
#include "stg22/terrain/draw_515.inc"
};
static Sint16 draw_305_polygons[955] = {
#include "stg22/terrain/draw_305_polygons.inc"
};
static Sint32 draw_305_vertices[867] = {
#include "stg22/terrain/draw_305_vertices.inc"
};
static NJS_CNK_MODEL draw_305_model = {
#include "stg22/terrain/draw_305_model.inc"
};
static NJS_CNK_OBJECT draw_503 = {
#include "stg22/terrain/draw_503.inc"
};
static Sint16 draw_442_polygons[289] = {
#include "stg22/terrain/draw_442_polygons.inc"
};
static Sint32 draw_442_vertices[323] = {
#include "stg22/terrain/draw_442_vertices.inc"
};
static NJS_CNK_MODEL draw_442_model = {
#include "stg22/terrain/draw_442_model.inc"
};
static NJS_CNK_OBJECT draw_442 = {
#include "stg22/terrain/draw_442.inc"
};
static Sint16 draw_413_polygons[81] = {
#include "stg22/terrain/draw_413_polygons.inc"
};
static Sint32 draw_413_vertices[59] = {
#include "stg22/terrain/draw_413_vertices.inc"
};
static NJS_CNK_MODEL draw_413_model = {
#include "stg22/terrain/draw_413_model.inc"
};
static NJS_CNK_OBJECT draw_413 = {
#include "stg22/terrain/draw_413.inc"
};
static Sint16 draw_390_polygons[39] = {
#include "stg22/terrain/draw_390_polygons.inc"
};
static Sint32 draw_390_vertices[27] = {
#include "stg22/terrain/draw_390_vertices.inc"
};
static NJS_CNK_MODEL draw_390_model = {
#include "stg22/terrain/draw_390_model.inc"
};
static NJS_CNK_OBJECT draw_391 = {
#include "stg22/terrain/draw_391.inc"
};
static Sint16 draw_372_polygons[1271] = {
#include "stg22/terrain/draw_372_polygons.inc"
};
static Sint32 draw_372_vertices[1059] = {
#include "stg22/terrain/draw_372_vertices.inc"
};
static NJS_CNK_MODEL draw_372_model = {
#include "stg22/terrain/draw_372_model.inc"
};
static NJS_CNK_OBJECT draw_372 = {
#include "stg22/terrain/draw_372.inc"
};
static Sint16 draw_359_polygons[273] = {
#include "stg22/terrain/draw_359_polygons.inc"
};
static Sint32 draw_359_vertices[259] = {
#include "stg22/terrain/draw_359_vertices.inc"
};
static NJS_CNK_MODEL draw_359_model = {
#include "stg22/terrain/draw_359_model.inc"
};
static NJS_CNK_OBJECT draw_359 = {
#include "stg22/terrain/draw_359.inc"
};
static Sint16 draw_347_polygons[747] = {
#include "stg22/terrain/draw_347_polygons.inc"
};
static Sint32 draw_347_vertices[675] = {
#include "stg22/terrain/draw_347_vertices.inc"
};
static NJS_CNK_MODEL draw_347_model = {
#include "stg22/terrain/draw_347_model.inc"
};
static NJS_CNK_OBJECT draw_347 = {
#include "stg22/terrain/draw_347.inc"
};
static Sint16 draw_346_polygons[747] = {
#include "stg22/terrain/draw_346_polygons.inc"
};
static Sint32 draw_346_vertices[675] = {
#include "stg22/terrain/draw_346_vertices.inc"
};
static NJS_CNK_MODEL draw_346_model = {
#include "stg22/terrain/draw_346_model.inc"
};
static NJS_CNK_OBJECT draw_346 = {
#include "stg22/terrain/draw_346.inc"
};
static Sint16 draw_331_polygons[123] = {
#include "stg22/terrain/draw_331_polygons.inc"
};
static Sint32 draw_331_vertices[67] = {
#include "stg22/terrain/draw_331_vertices.inc"
};
static NJS_CNK_MODEL draw_331_model = {
#include "stg22/terrain/draw_331_model.inc"
};
static NJS_CNK_OBJECT draw_332 = {
#include "stg22/terrain/draw_332.inc"
};
static Sint16 draw_285_polygons[1015] = {
#include "stg22/terrain/draw_285_polygons.inc"
};
static Sint32 draw_285_vertices[835] = {
#include "stg22/terrain/draw_285_vertices.inc"
};
static NJS_CNK_MODEL draw_285_model = {
#include "stg22/terrain/draw_285_model.inc"
};
static NJS_CNK_OBJECT draw_285 = {
#include "stg22/terrain/draw_285.inc"
};
static Sint16 draw_258_polygons[411] = {
#include "stg22/terrain/draw_258_polygons.inc"
};
static Sint32 draw_258_vertices[291] = {
#include "stg22/terrain/draw_258_vertices.inc"
};
static NJS_CNK_MODEL draw_258_model = {
#include "stg22/terrain/draw_258_model.inc"
};
static NJS_CNK_OBJECT draw_258 = {
#include "stg22/terrain/draw_258.inc"
};
static Sint16 draw_248_polygons[165] = {
#include "stg22/terrain/draw_248_polygons.inc"
};
static Sint32 draw_248_vertices[123] = {
#include "stg22/terrain/draw_248_vertices.inc"
};
static NJS_CNK_MODEL draw_248_model = {
#include "stg22/terrain/draw_248_model.inc"
};
static NJS_CNK_OBJECT draw_248 = {
#include "stg22/terrain/draw_248.inc"
};
static Sint16 draw_238_polygons[579] = {
#include "stg22/terrain/draw_238_polygons.inc"
};
static Sint32 draw_238_vertices[487] = {
#include "stg22/terrain/draw_238_vertices.inc"
};
static NJS_CNK_MODEL draw_238_model = {
#include "stg22/terrain/draw_238_model.inc"
};
static NJS_CNK_OBJECT draw_238 = {
#include "stg22/terrain/draw_238.inc"
};
static Sint16 draw_229_polygons[519] = {
#include "stg22/terrain/draw_229_polygons.inc"
};
static Sint32 draw_229_vertices[499] = {
#include "stg22/terrain/draw_229_vertices.inc"
};
static NJS_CNK_MODEL draw_229_model = {
#include "stg22/terrain/draw_229_model.inc"
};
static NJS_CNK_OBJECT draw_229 = {
#include "stg22/terrain/draw_229.inc"
};
static Sint16 draw_28_polygons[229] = {
#include "stg22/terrain/draw_28_polygons.inc"
};
static Sint32 draw_28_vertices[211] = {
#include "stg22/terrain/draw_28_vertices.inc"
};
static NJS_CNK_MODEL draw_28_model = {
#include "stg22/terrain/draw_28_model.inc"
};
static NJS_CNK_OBJECT draw_205 = {
#include "stg22/terrain/draw_205.inc"
};
static Sint16 draw_200_polygons[37] = {
#include "stg22/terrain/draw_200_polygons.inc"
};
static Sint32 draw_200_vertices[35] = {
#include "stg22/terrain/draw_200_vertices.inc"
};
static NJS_CNK_MODEL draw_200_model = {
#include "stg22/terrain/draw_200_model.inc"
};
static NJS_CNK_OBJECT draw_203 = {
#include "stg22/terrain/draw_203.inc"
};
static Sint16 draw_26_polygons[829] = {
#include "stg22/terrain/draw_26_polygons.inc"
};
static Sint32 draw_26_vertices[795] = {
#include "stg22/terrain/draw_26_vertices.inc"
};
static NJS_CNK_MODEL draw_26_model = {
#include "stg22/terrain/draw_26_model.inc"
};
static NJS_CNK_OBJECT draw_27 = {
#include "stg22/terrain/draw_27.inc"
};
static Sint16 draw_22_polygons[37] = {
#include "stg22/terrain/draw_22_polygons.inc"
};
static Sint32 draw_22_vertices[35] = {
#include "stg22/terrain/draw_22_vertices.inc"
};
static NJS_CNK_MODEL draw_22_model = {
#include "stg22/terrain/draw_22_model.inc"
};
static NJS_CNK_OBJECT draw_25 = {
#include "stg22/terrain/draw_25.inc"
};
static Sint16 draw_18_polygons[37] = {
#include "stg22/terrain/draw_18_polygons.inc"
};
static Sint32 draw_18_vertices[35] = {
#include "stg22/terrain/draw_18_vertices.inc"
};
static NJS_CNK_MODEL draw_18_model = {
#include "stg22/terrain/draw_18_model.inc"
};
static NJS_CNK_OBJECT draw_21 = {
#include "stg22/terrain/draw_21.inc"
};
static Sint16 draw_10_polygons[203] = {
#include "stg22/terrain/draw_10_polygons.inc"
};
static Sint32 draw_10_vertices[135] = {
#include "stg22/terrain/draw_10_vertices.inc"
};
static NJS_CNK_MODEL draw_10_model = {
#include "stg22/terrain/draw_10_model.inc"
};
static NJS_CNK_OBJECT draw_17 = {
#include "stg22/terrain/draw_17.inc"
};
static Sint16 draw_9_polygons[691] = {
#include "stg22/terrain/draw_9_polygons.inc"
};
static Sint32 draw_9_vertices[355] = {
#include "stg22/terrain/draw_9_vertices.inc"
};
static NJS_CNK_MODEL draw_9_model = {
#include "stg22/terrain/draw_9_model.inc"
};
static NJS_CNK_OBJECT draw_9 = {
#include "stg22/terrain/draw_9.inc"
};
static Sint16 draw_8_polygons[1573] = {
#include "stg22/terrain/draw_8_polygons.inc"
};
static Sint32 draw_8_vertices[875] = {
#include "stg22/terrain/draw_8_vertices.inc"
};
static NJS_CNK_MODEL draw_8_model = {
#include "stg22/terrain/draw_8_model.inc"
};
static NJS_CNK_OBJECT draw_8 = {
#include "stg22/terrain/draw_8.inc"
};
static Sint16 draw_7_polygons[1599] = {
#include "stg22/terrain/draw_7_polygons.inc"
};
static Sint32 draw_7_vertices[887] = {
#include "stg22/terrain/draw_7_vertices.inc"
};
static NJS_CNK_MODEL draw_7_model = {
#include "stg22/terrain/draw_7_model.inc"
};
static NJS_CNK_OBJECT draw_7 = {
#include "stg22/terrain/draw_7.inc"
};
static Sint16 draw_6_polygons[1551] = {
#include "stg22/terrain/draw_6_polygons.inc"
};
static Sint32 draw_6_vertices[795] = {
#include "stg22/terrain/draw_6_vertices.inc"
};
static NJS_CNK_MODEL draw_6_model = {
#include "stg22/terrain/draw_6_model.inc"
};
static NJS_CNK_OBJECT draw_6 = {
#include "stg22/terrain/draw_6.inc"
};
static Sint16 draw_5_polygons[1947] = {
#include "stg22/terrain/draw_5_polygons.inc"
};
static Sint32 draw_5_vertices[1063] = {
#include "stg22/terrain/draw_5_vertices.inc"
};
static NJS_CNK_MODEL draw_5_model = {
#include "stg22/terrain/draw_5_model.inc"
};
static NJS_CNK_OBJECT draw_5 = {
#include "stg22/terrain/draw_5.inc"
};
static Sint16 draw_4_polygons[847] = {
#include "stg22/terrain/draw_4_polygons.inc"
};
static Sint32 draw_4_vertices[483] = {
#include "stg22/terrain/draw_4_vertices.inc"
};
static NJS_CNK_MODEL draw_4_model = {
#include "stg22/terrain/draw_4_model.inc"
};
static NJS_CNK_OBJECT draw_4 = {
#include "stg22/terrain/draw_4.inc"
};
static Sint16 draw_3_polygons[447] = {
#include "stg22/terrain/draw_3_polygons.inc"
};
static Sint32 draw_3_vertices[443] = {
#include "stg22/terrain/draw_3_vertices.inc"
};
static NJS_CNK_MODEL draw_3_model = {
#include "stg22/terrain/draw_3_model.inc"
};
static NJS_CNK_OBJECT draw_3 = {
#include "stg22/terrain/draw_3.inc"
};
static Sint16 draw_2_polygons[543] = {
#include "stg22/terrain/draw_2_polygons.inc"
};
static Sint32 draw_2_vertices[407] = {
#include "stg22/terrain/draw_2_vertices.inc"
};
static NJS_CNK_MODEL draw_2_model = {
#include "stg22/terrain/draw_2_model.inc"
};
static NJS_CNK_OBJECT draw_2 = {
#include "stg22/terrain/draw_2.inc"
};
static Sint16 draw_1_polygons[173] = {
#include "stg22/terrain/draw_1_polygons.inc"
};
static Sint32 draw_1_vertices[99] = {
#include "stg22/terrain/draw_1_vertices.inc"
};
static NJS_CNK_MODEL draw_1_model = {
#include "stg22/terrain/draw_1_model.inc"
};
static NJS_CNK_OBJECT draw_1 = {
#include "stg22/terrain/draw_1.inc"
};
static Sint16 draw_0_polygons[199] = {
#include "stg22/terrain/draw_0_polygons.inc"
};
static Sint32 draw_0_vertices[99] = {
#include "stg22/terrain/draw_0_vertices.inc"
};
static NJS_CNK_MODEL draw_0_model = {
#include "stg22/terrain/draw_0_model.inc"
};
static NJS_CNK_OBJECT draw_0 = {
#include "stg22/terrain/draw_0.inc"
};
static NJS_MATERIAL collision_0_materials[1] = {
#include "stg22/terrain/collision_0_materials.inc"
};
static Sint16 collision_0_faces[8] = {
#include "stg22/terrain/collision_0_faces.inc"
};
static NJS_MESHSET collision_0_meshsets[1] = {
#include "stg22/terrain/collision_0_meshsets.inc"
};
static NJS_POINT3 collision_0_points[6] = {
#include "stg22/terrain/collision_0_points.inc"
};
static NJS_MODEL collision_0_model = {
#include "stg22/terrain/collision_0_model.inc"
};
static NJS_OBJECT collision_0 = {
#include "stg22/terrain/collision_0.inc"
};
static NJS_MATERIAL collision_2_materials[1] = {
#include "stg22/terrain/collision_2_materials.inc"
};
static Sint16 collision_2_faces[15] = {
#include "stg22/terrain/collision_2_faces.inc"
};
static NJS_MESHSET collision_2_meshsets[1] = {
#include "stg22/terrain/collision_2_meshsets.inc"
};
static NJS_POINT3 collision_2_points[8] = {
#include "stg22/terrain/collision_2_points.inc"
};
static NJS_MODEL collision_2_model = {
#include "stg22/terrain/collision_2_model.inc"
};
static NJS_OBJECT collision_2 = {
#include "stg22/terrain/collision_2.inc"
};
static NJS_MATERIAL collision_3_materials[1] = {
#include "stg22/terrain/collision_3_materials.inc"
};
static Sint16 collision_3_faces[52] = {
#include "stg22/terrain/collision_3_faces.inc"
};
static NJS_MESHSET collision_3_meshsets[1] = {
#include "stg22/terrain/collision_3_meshsets.inc"
};
static NJS_POINT3 collision_3_points[30] = {
#include "stg22/terrain/collision_3_points.inc"
};
static NJS_MODEL collision_3_model = {
#include "stg22/terrain/collision_3_model.inc"
};
static NJS_OBJECT collision_3 = {
#include "stg22/terrain/collision_3.inc"
};
static NJS_MATERIAL collision_4_materials[1] = {
#include "stg22/terrain/collision_4_materials.inc"
};
static Sint16 collision_4_faces[42] = {
#include "stg22/terrain/collision_4_faces.inc"
};
static NJS_MESHSET collision_4_meshsets[1] = {
#include "stg22/terrain/collision_4_meshsets.inc"
};
static NJS_POINT3 collision_4_points[28] = {
#include "stg22/terrain/collision_4_points.inc"
};
static NJS_MODEL collision_4_model = {
#include "stg22/terrain/collision_4_model.inc"
};
static NJS_OBJECT collision_4 = {
#include "stg22/terrain/collision_4.inc"
};
static NJS_MATERIAL collision_5_materials[1] = {
#include "stg22/terrain/collision_5_materials.inc"
};
static Sint16 collision_5_faces[17] = {
#include "stg22/terrain/collision_5_faces.inc"
};
static NJS_MESHSET collision_5_meshsets[1] = {
#include "stg22/terrain/collision_5_meshsets.inc"
};
static NJS_POINT3 collision_5_points[9] = {
#include "stg22/terrain/collision_5_points.inc"
};
static NJS_MODEL collision_5_model = {
#include "stg22/terrain/collision_5_model.inc"
};
static NJS_OBJECT collision_5 = {
#include "stg22/terrain/collision_5.inc"
};
static NJS_MATERIAL collision_7_materials[1] = {
#include "stg22/terrain/collision_7_materials.inc"
};
static Sint16 collision_7_faces[47] = {
#include "stg22/terrain/collision_7_faces.inc"
};
static NJS_MESHSET collision_7_meshsets[1] = {
#include "stg22/terrain/collision_7_meshsets.inc"
};
static NJS_POINT3 collision_7_points[23] = {
#include "stg22/terrain/collision_7_points.inc"
};
static NJS_MODEL collision_7_model = {
#include "stg22/terrain/collision_7_model.inc"
};
static NJS_OBJECT collision_7 = {
#include "stg22/terrain/collision_7.inc"
};
static NJS_MATERIAL collision_8_materials[1] = {
#include "stg22/terrain/collision_8_materials.inc"
};
static Sint16 collision_8_faces[16] = {
#include "stg22/terrain/collision_8_faces.inc"
};
static NJS_MESHSET collision_8_meshsets[1] = {
#include "stg22/terrain/collision_8_meshsets.inc"
};
static NJS_POINT3 collision_8_points[9] = {
#include "stg22/terrain/collision_8_points.inc"
};
static NJS_MODEL collision_8_model = {
#include "stg22/terrain/collision_8_model.inc"
};
static NJS_OBJECT collision_8 = {
#include "stg22/terrain/collision_8.inc"
};
static NJS_MATERIAL collision_10_materials[1] = {
#include "stg22/terrain/collision_10_materials.inc"
};
static Sint16 collision_10_faces[36] = {
#include "stg22/terrain/collision_10_faces.inc"
};
static NJS_MESHSET collision_10_meshsets[1] = {
#include "stg22/terrain/collision_10_meshsets.inc"
};
static NJS_POINT3 collision_10_points[20] = {
#include "stg22/terrain/collision_10_points.inc"
};
static NJS_MODEL collision_10_model = {
#include "stg22/terrain/collision_10_model.inc"
};
static NJS_OBJECT collision_10 = {
#include "stg22/terrain/collision_10.inc"
};
static NJS_MATERIAL collision_11_materials[1] = {
#include "stg22/terrain/collision_11_materials.inc"
};
static Sint16 collision_11_faces[36] = {
#include "stg22/terrain/collision_11_faces.inc"
};
static NJS_MESHSET collision_11_meshsets[1] = {
#include "stg22/terrain/collision_11_meshsets.inc"
};
static NJS_POINT3 collision_11_points[22] = {
#include "stg22/terrain/collision_11_points.inc"
};
static NJS_MODEL collision_11_model = {
#include "stg22/terrain/collision_11_model.inc"
};
static NJS_OBJECT collision_11 = {
#include "stg22/terrain/collision_11.inc"
};
static NJS_MATERIAL collision_17_materials[1] = {
#include "stg22/terrain/collision_17_materials.inc"
};
static Sint16 collision_17_faces[14] = {
#include "stg22/terrain/collision_17_faces.inc"
};
static NJS_MESHSET collision_17_meshsets[1] = {
#include "stg22/terrain/collision_17_meshsets.inc"
};
static NJS_POINT3 collision_17_points[12] = {
#include "stg22/terrain/collision_17_points.inc"
};
static NJS_MODEL collision_17_model = {
#include "stg22/terrain/collision_17_model.inc"
};
static NJS_OBJECT collision_17 = {
#include "stg22/terrain/collision_17.inc"
};
static NJS_MATERIAL collision_41_materials[1] = {
#include "stg22/terrain/collision_41_materials.inc"
};
static Sint16 collision_41_faces[24] = {
#include "stg22/terrain/collision_41_faces.inc"
};
static NJS_MESHSET collision_41_meshsets[1] = {
#include "stg22/terrain/collision_41_meshsets.inc"
};
static NJS_POINT3 collision_41_points[16] = {
#include "stg22/terrain/collision_41_points.inc"
};
static NJS_MODEL collision_41_model = {
#include "stg22/terrain/collision_41_model.inc"
};
static NJS_OBJECT collision_41 = {
#include "stg22/terrain/collision_41.inc"
};
static NJS_MATERIAL collision_42_materials[1] = {
#include "stg22/terrain/collision_42_materials.inc"
};
static Sint16 collision_42_faces[18] = {
#include "stg22/terrain/collision_42_faces.inc"
};
static NJS_MESHSET collision_42_meshsets[1] = {
#include "stg22/terrain/collision_42_meshsets.inc"
};
static NJS_POINT3 collision_42_points[12] = {
#include "stg22/terrain/collision_42_points.inc"
};
static NJS_MODEL collision_42_model = {
#include "stg22/terrain/collision_42_model.inc"
};
static NJS_OBJECT collision_42 = {
#include "stg22/terrain/collision_42.inc"
};
static NJS_MATERIAL collision_44_materials[1] = {
#include "stg22/terrain/collision_44_materials.inc"
};
static Sint16 collision_44_faces[7] = {
#include "stg22/terrain/collision_44_faces.inc"
};
static NJS_MESHSET collision_44_meshsets[1] = {
#include "stg22/terrain/collision_44_meshsets.inc"
};
static NJS_POINT3 collision_44_points[6] = {
#include "stg22/terrain/collision_44_points.inc"
};
static NJS_MODEL collision_44_model = {
#include "stg22/terrain/collision_44_model.inc"
};
static NJS_OBJECT collision_44 = {
#include "stg22/terrain/collision_44.inc"
};
static NJS_MATERIAL collision_46_materials[1] = {
#include "stg22/terrain/collision_46_materials.inc"
};
static Sint16 collision_46_faces[38] = {
#include "stg22/terrain/collision_46_faces.inc"
};
static NJS_MESHSET collision_46_meshsets[1] = {
#include "stg22/terrain/collision_46_meshsets.inc"
};
static NJS_POINT3 collision_46_points[22] = {
#include "stg22/terrain/collision_46_points.inc"
};
static NJS_MODEL collision_46_model = {
#include "stg22/terrain/collision_46_model.inc"
};
static NJS_OBJECT collision_46 = {
#include "stg22/terrain/collision_46.inc"
};
static NJS_MATERIAL collision_47_materials[1] = {
#include "stg22/terrain/collision_47_materials.inc"
};
static Sint16 collision_47_faces[19] = {
#include "stg22/terrain/collision_47_faces.inc"
};
static NJS_MESHSET collision_47_meshsets[1] = {
#include "stg22/terrain/collision_47_meshsets.inc"
};
static NJS_POINT3 collision_47_points[12] = {
#include "stg22/terrain/collision_47_points.inc"
};
static NJS_MODEL collision_47_model = {
#include "stg22/terrain/collision_47_model.inc"
};
static NJS_OBJECT collision_47 = {
#include "stg22/terrain/collision_47.inc"
};
static NJS_MATERIAL collision_48_materials[1] = {
#include "stg22/terrain/collision_48_materials.inc"
};
static Sint16 collision_48_faces[14] = {
#include "stg22/terrain/collision_48_faces.inc"
};
static NJS_MESHSET collision_48_meshsets[1] = {
#include "stg22/terrain/collision_48_meshsets.inc"
};
static NJS_POINT3 collision_48_points[12] = {
#include "stg22/terrain/collision_48_points.inc"
};
static NJS_MODEL collision_48_model = {
#include "stg22/terrain/collision_48_model.inc"
};
static NJS_OBJECT collision_48 = {
#include "stg22/terrain/collision_48.inc"
};
static NJS_MATERIAL collision_55_materials[1] = {
#include "stg22/terrain/collision_55_materials.inc"
};
static Sint16 collision_55_faces[15] = {
#include "stg22/terrain/collision_55_faces.inc"
};
static NJS_MESHSET collision_55_meshsets[1] = {
#include "stg22/terrain/collision_55_meshsets.inc"
};
static NJS_POINT3 collision_55_points[12] = {
#include "stg22/terrain/collision_55_points.inc"
};
static NJS_MODEL collision_55_model = {
#include "stg22/terrain/collision_55_model.inc"
};
static NJS_OBJECT collision_55 = {
#include "stg22/terrain/collision_55.inc"
};
static NJS_MATERIAL collision_57_materials[1] = {
#include "stg22/terrain/collision_57_materials.inc"
};
static Sint16 collision_57_faces[36] = {
#include "stg22/terrain/collision_57_faces.inc"
};
static NJS_MESHSET collision_57_meshsets[1] = {
#include "stg22/terrain/collision_57_meshsets.inc"
};
static NJS_POINT3 collision_57_points[20] = {
#include "stg22/terrain/collision_57_points.inc"
};
static NJS_MODEL collision_57_model = {
#include "stg22/terrain/collision_57_model.inc"
};
static NJS_OBJECT collision_57 = {
#include "stg22/terrain/collision_57.inc"
};
static NJS_MATERIAL collision_58_materials[1] = {
#include "stg22/terrain/collision_58_materials.inc"
};
static Sint16 collision_58_faces[28] = {
#include "stg22/terrain/collision_58_faces.inc"
};
static NJS_MESHSET collision_58_meshsets[1] = {
#include "stg22/terrain/collision_58_meshsets.inc"
};
static NJS_POINT3 collision_58_points[18] = {
#include "stg22/terrain/collision_58_points.inc"
};
static NJS_MODEL collision_58_model = {
#include "stg22/terrain/collision_58_model.inc"
};
static NJS_OBJECT collision_58 = {
#include "stg22/terrain/collision_58.inc"
};
static NJS_MATERIAL collision_59_materials[1] = {
#include "stg22/terrain/collision_59_materials.inc"
};
static Sint16 collision_59_faces[28] = {
#include "stg22/terrain/collision_59_faces.inc"
};
static NJS_MESHSET collision_59_meshsets[1] = {
#include "stg22/terrain/collision_59_meshsets.inc"
};
static NJS_POINT3 collision_59_points[18] = {
#include "stg22/terrain/collision_59_points.inc"
};
static NJS_MODEL collision_59_model = {
#include "stg22/terrain/collision_59_model.inc"
};
static NJS_OBJECT collision_59 = {
#include "stg22/terrain/collision_59.inc"
};
static NJS_MATERIAL collision_60_materials[1] = {
#include "stg22/terrain/collision_60_materials.inc"
};
static Sint16 collision_60_faces[38] = {
#include "stg22/terrain/collision_60_faces.inc"
};
static NJS_MESHSET collision_60_meshsets[1] = {
#include "stg22/terrain/collision_60_meshsets.inc"
};
static NJS_POINT3 collision_60_points[24] = {
#include "stg22/terrain/collision_60_points.inc"
};
static NJS_MODEL collision_60_model = {
#include "stg22/terrain/collision_60_model.inc"
};
static NJS_OBJECT collision_60 = {
#include "stg22/terrain/collision_60.inc"
};
static NJS_MATERIAL collision_67_materials[1] = {
#include "stg22/terrain/collision_67_materials.inc"
};
static Sint16 collision_67_faces[10] = {
#include "stg22/terrain/collision_67_faces.inc"
};
static NJS_MESHSET collision_67_meshsets[1] = {
#include "stg22/terrain/collision_67_meshsets.inc"
};
static NJS_POINT3 collision_67_points[8] = {
#include "stg22/terrain/collision_67_points.inc"
};
static NJS_MODEL collision_67_model = {
#include "stg22/terrain/collision_67_model.inc"
};
static NJS_OBJECT collision_67 = {
#include "stg22/terrain/collision_67.inc"
};
static NJS_MATERIAL collision_94_materials[1] = {
#include "stg22/terrain/collision_94_materials.inc"
};
static Sint16 collision_94_faces[39] = {
#include "stg22/terrain/collision_94_faces.inc"
};
static NJS_MESHSET collision_94_meshsets[1] = {
#include "stg22/terrain/collision_94_meshsets.inc"
};
static NJS_POINT3 collision_94_points[24] = {
#include "stg22/terrain/collision_94_points.inc"
};
static NJS_MODEL collision_94_model = {
#include "stg22/terrain/collision_94_model.inc"
};
static NJS_OBJECT collision_94 = {
#include "stg22/terrain/collision_94.inc"
};
static NJS_MATERIAL collision_95_materials[1] = {
#include "stg22/terrain/collision_95_materials.inc"
};
static Sint16 collision_95_faces[14] = {
#include "stg22/terrain/collision_95_faces.inc"
};
static NJS_MESHSET collision_95_meshsets[1] = {
#include "stg22/terrain/collision_95_meshsets.inc"
};
static NJS_POINT3 collision_95_points[12] = {
#include "stg22/terrain/collision_95_points.inc"
};
static NJS_MODEL collision_95_model = {
#include "stg22/terrain/collision_95_model.inc"
};
static NJS_OBJECT collision_95 = {
#include "stg22/terrain/collision_95.inc"
};
static NJS_MATERIAL collision_97_materials[1] = {
#include "stg22/terrain/collision_97_materials.inc"
};
static Sint16 collision_97_faces[57] = {
#include "stg22/terrain/collision_97_faces.inc"
};
static NJS_MESHSET collision_97_meshsets[1] = {
#include "stg22/terrain/collision_97_meshsets.inc"
};
static NJS_POINT3 collision_97_points[27] = {
#include "stg22/terrain/collision_97_points.inc"
};
static NJS_MODEL collision_97_model = {
#include "stg22/terrain/collision_97_model.inc"
};
static NJS_OBJECT collision_97 = {
#include "stg22/terrain/collision_97.inc"
};
static NJS_MATERIAL collision_98_materials[1] = {
#include "stg22/terrain/collision_98_materials.inc"
};
static Sint16 collision_98_faces[14] = {
#include "stg22/terrain/collision_98_faces.inc"
};
static NJS_MESHSET collision_98_meshsets[1] = {
#include "stg22/terrain/collision_98_meshsets.inc"
};
static NJS_POINT3 collision_98_points[10] = {
#include "stg22/terrain/collision_98_points.inc"
};
static NJS_MODEL collision_98_model = {
#include "stg22/terrain/collision_98_model.inc"
};
static NJS_OBJECT collision_98 = {
#include "stg22/terrain/collision_98.inc"
};
static NJS_MATERIAL collision_100_materials[1] = {
#include "stg22/terrain/collision_100_materials.inc"
};
static Sint16 collision_100_faces[26] = {
#include "stg22/terrain/collision_100_faces.inc"
};
static NJS_MESHSET collision_100_meshsets[1] = {
#include "stg22/terrain/collision_100_meshsets.inc"
};
static NJS_POINT3 collision_100_points[18] = {
#include "stg22/terrain/collision_100_points.inc"
};
static NJS_MODEL collision_100_model = {
#include "stg22/terrain/collision_100_model.inc"
};
static NJS_OBJECT collision_100 = {
#include "stg22/terrain/collision_100.inc"
};
static NJS_MATERIAL collision_102_materials[1] = {
#include "stg22/terrain/collision_102_materials.inc"
};
static Sint16 collision_102_faces[42] = {
#include "stg22/terrain/collision_102_faces.inc"
};
static NJS_MESHSET collision_102_meshsets[1] = {
#include "stg22/terrain/collision_102_meshsets.inc"
};
static NJS_POINT3 collision_102_points[24] = {
#include "stg22/terrain/collision_102_points.inc"
};
static NJS_MODEL collision_102_model = {
#include "stg22/terrain/collision_102_model.inc"
};
static NJS_OBJECT collision_102 = {
#include "stg22/terrain/collision_102.inc"
};
static NJS_MATERIAL collision_103_materials[1] = {
#include "stg22/terrain/collision_103_materials.inc"
};
static Sint16 collision_103_faces[14] = {
#include "stg22/terrain/collision_103_faces.inc"
};
static NJS_MESHSET collision_103_meshsets[1] = {
#include "stg22/terrain/collision_103_meshsets.inc"
};
static NJS_POINT3 collision_103_points[12] = {
#include "stg22/terrain/collision_103_points.inc"
};
static NJS_MODEL collision_103_model = {
#include "stg22/terrain/collision_103_model.inc"
};
static NJS_OBJECT collision_103 = {
#include "stg22/terrain/collision_103.inc"
};
static NJS_MATERIAL collision_107_materials[1] = {
#include "stg22/terrain/collision_107_materials.inc"
};
static Sint16 collision_107_faces[19] = {
#include "stg22/terrain/collision_107_faces.inc"
};
static NJS_MESHSET collision_107_meshsets[1] = {
#include "stg22/terrain/collision_107_meshsets.inc"
};
static NJS_POINT3 collision_107_points[12] = {
#include "stg22/terrain/collision_107_points.inc"
};
static NJS_MODEL collision_107_model = {
#include "stg22/terrain/collision_107_model.inc"
};
static NJS_OBJECT collision_107 = {
#include "stg22/terrain/collision_107.inc"
};
static NJS_MATERIAL collision_108_materials[1] = {
#include "stg22/terrain/collision_108_materials.inc"
};
static Sint16 collision_108_faces[35] = {
#include "stg22/terrain/collision_108_faces.inc"
};
static NJS_MESHSET collision_108_meshsets[1] = {
#include "stg22/terrain/collision_108_meshsets.inc"
};
static NJS_POINT3 collision_108_points[19] = {
#include "stg22/terrain/collision_108_points.inc"
};
static NJS_MODEL collision_108_model = {
#include "stg22/terrain/collision_108_model.inc"
};
static NJS_OBJECT collision_108 = {
#include "stg22/terrain/collision_108.inc"
};
static NJS_MATERIAL collision_109_materials[1] = {
#include "stg22/terrain/collision_109_materials.inc"
};
static Sint16 collision_109_faces[118] = {
#include "stg22/terrain/collision_109_faces.inc"
};
static NJS_MESHSET collision_109_meshsets[1] = {
#include "stg22/terrain/collision_109_meshsets.inc"
};
static NJS_POINT3 collision_109_points[53] = {
#include "stg22/terrain/collision_109_points.inc"
};
static NJS_MODEL collision_109_model = {
#include "stg22/terrain/collision_109_model.inc"
};
static NJS_OBJECT collision_109 = {
#include "stg22/terrain/collision_109.inc"
};
static NJS_MATERIAL collision_111_materials[1] = {
#include "stg22/terrain/collision_111_materials.inc"
};
static Sint16 collision_111_faces[9] = {
#include "stg22/terrain/collision_111_faces.inc"
};
static NJS_MESHSET collision_111_meshsets[1] = {
#include "stg22/terrain/collision_111_meshsets.inc"
};
static NJS_POINT3 collision_111_points[8] = {
#include "stg22/terrain/collision_111_points.inc"
};
static NJS_MODEL collision_111_model = {
#include "stg22/terrain/collision_111_model.inc"
};
static NJS_OBJECT collision_111 = {
#include "stg22/terrain/collision_111.inc"
};
static NJS_MATERIAL collision_113_materials[1] = {
#include "stg22/terrain/collision_113_materials.inc"
};
static Sint16 collision_113_faces[17] = {
#include "stg22/terrain/collision_113_faces.inc"
};
static NJS_MESHSET collision_113_meshsets[1] = {
#include "stg22/terrain/collision_113_meshsets.inc"
};
static NJS_POINT3 collision_113_points[10] = {
#include "stg22/terrain/collision_113_points.inc"
};
static NJS_MODEL collision_113_model = {
#include "stg22/terrain/collision_113_model.inc"
};
static NJS_OBJECT collision_113 = {
#include "stg22/terrain/collision_113.inc"
};
static NJS_MATERIAL collision_115_materials[1] = {
#include "stg22/terrain/collision_115_materials.inc"
};
static Sint16 collision_115_faces[34] = {
#include "stg22/terrain/collision_115_faces.inc"
};
static NJS_MESHSET collision_115_meshsets[1] = {
#include "stg22/terrain/collision_115_meshsets.inc"
};
static NJS_POINT3 collision_115_points[18] = {
#include "stg22/terrain/collision_115_points.inc"
};
static NJS_MODEL collision_115_model = {
#include "stg22/terrain/collision_115_model.inc"
};
static NJS_OBJECT collision_115 = {
#include "stg22/terrain/collision_115.inc"
};
static NJS_MATERIAL collision_118_materials[1] = {
#include "stg22/terrain/collision_118_materials.inc"
};
static Sint16 collision_118_faces[67] = {
#include "stg22/terrain/collision_118_faces.inc"
};
static NJS_MESHSET collision_118_meshsets[1] = {
#include "stg22/terrain/collision_118_meshsets.inc"
};
static NJS_POINT3 collision_118_points[26] = {
#include "stg22/terrain/collision_118_points.inc"
};
static NJS_MODEL collision_118_model = {
#include "stg22/terrain/collision_118_model.inc"
};
static NJS_OBJECT collision_118 = {
#include "stg22/terrain/collision_118.inc"
};
static NJS_MATERIAL collision_119_materials[1] = {
#include "stg22/terrain/collision_119_materials.inc"
};
static Sint16 collision_119_faces[45] = {
#include "stg22/terrain/collision_119_faces.inc"
};
static NJS_MESHSET collision_119_meshsets[1] = {
#include "stg22/terrain/collision_119_meshsets.inc"
};
static NJS_POINT3 collision_119_points[21] = {
#include "stg22/terrain/collision_119_points.inc"
};
static NJS_MODEL collision_119_model = {
#include "stg22/terrain/collision_119_model.inc"
};
static NJS_OBJECT collision_119 = {
#include "stg22/terrain/collision_119.inc"
};
static NJS_MATERIAL collision_120_materials[1] = {
#include "stg22/terrain/collision_120_materials.inc"
};
static Sint16 collision_120_faces[28] = {
#include "stg22/terrain/collision_120_faces.inc"
};
static NJS_MESHSET collision_120_meshsets[1] = {
#include "stg22/terrain/collision_120_meshsets.inc"
};
static NJS_POINT3 collision_120_points[18] = {
#include "stg22/terrain/collision_120_points.inc"
};
static NJS_MODEL collision_120_model = {
#include "stg22/terrain/collision_120_model.inc"
};
static NJS_OBJECT collision_120 = {
#include "stg22/terrain/collision_120.inc"
};
static NJS_MATERIAL collision_121_materials[1] = {
#include "stg22/terrain/collision_121_materials.inc"
};
static Sint16 collision_121_faces[30] = {
#include "stg22/terrain/collision_121_faces.inc"
};
static NJS_MESHSET collision_121_meshsets[1] = {
#include "stg22/terrain/collision_121_meshsets.inc"
};
static NJS_POINT3 collision_121_points[23] = {
#include "stg22/terrain/collision_121_points.inc"
};
static NJS_MODEL collision_121_model = {
#include "stg22/terrain/collision_121_model.inc"
};
static NJS_OBJECT collision_121 = {
#include "stg22/terrain/collision_121.inc"
};
static NJS_MATERIAL collision_122_materials[1] = {
#include "stg22/terrain/collision_122_materials.inc"
};
static Sint16 collision_122_faces[59] = {
#include "stg22/terrain/collision_122_faces.inc"
};
static NJS_MESHSET collision_122_meshsets[1] = {
#include "stg22/terrain/collision_122_meshsets.inc"
};
static NJS_POINT3 collision_122_points[26] = {
#include "stg22/terrain/collision_122_points.inc"
};
static NJS_MODEL collision_122_model = {
#include "stg22/terrain/collision_122_model.inc"
};
static NJS_OBJECT collision_122 = {
#include "stg22/terrain/collision_122.inc"
};
static NJS_MATERIAL collision_123_materials[1] = {
#include "stg22/terrain/collision_123_materials.inc"
};
static Sint16 collision_123_faces[55] = {
#include "stg22/terrain/collision_123_faces.inc"
};
static NJS_MESHSET collision_123_meshsets[1] = {
#include "stg22/terrain/collision_123_meshsets.inc"
};
static NJS_POINT3 collision_123_points[27] = {
#include "stg22/terrain/collision_123_points.inc"
};
static NJS_MODEL collision_123_model = {
#include "stg22/terrain/collision_123_model.inc"
};
static NJS_OBJECT collision_123 = {
#include "stg22/terrain/collision_123.inc"
};
static NJS_MATERIAL collision_124_materials[1] = {
#include "stg22/terrain/collision_124_materials.inc"
};
static Sint16 collision_124_faces[20] = {
#include "stg22/terrain/collision_124_faces.inc"
};
static NJS_MESHSET collision_124_meshsets[1] = {
#include "stg22/terrain/collision_124_meshsets.inc"
};
static NJS_POINT3 collision_124_points[9] = {
#include "stg22/terrain/collision_124_points.inc"
};
static NJS_MODEL collision_124_model = {
#include "stg22/terrain/collision_124_model.inc"
};
static NJS_OBJECT collision_124 = {
#include "stg22/terrain/collision_124.inc"
};
static NJS_MATERIAL collision_125_materials[1] = {
#include "stg22/terrain/collision_125_materials.inc"
};
static Sint16 collision_125_faces[7] = {
#include "stg22/terrain/collision_125_faces.inc"
};
static NJS_MESHSET collision_125_meshsets[1] = {
#include "stg22/terrain/collision_125_meshsets.inc"
};
static NJS_POINT3 collision_125_points[6] = {
#include "stg22/terrain/collision_125_points.inc"
};
static NJS_MODEL collision_125_model = {
#include "stg22/terrain/collision_125_model.inc"
};
static NJS_OBJECT collision_125 = {
#include "stg22/terrain/collision_125.inc"
};
static NJS_MATERIAL collision_127_materials[1] = {
#include "stg22/terrain/collision_127_materials.inc"
};
static Sint16 collision_127_faces[109] = {
#include "stg22/terrain/collision_127_faces.inc"
};
static NJS_MESHSET collision_127_meshsets[1] = {
#include "stg22/terrain/collision_127_meshsets.inc"
};
static NJS_POINT3 collision_127_points[48] = {
#include "stg22/terrain/collision_127_points.inc"
};
static NJS_MODEL collision_127_model = {
#include "stg22/terrain/collision_127_model.inc"
};
static NJS_OBJECT collision_127 = {
#include "stg22/terrain/collision_127.inc"
};
static NJS_MATERIAL collision_129_materials[1] = {
#include "stg22/terrain/collision_129_materials.inc"
};
static Sint16 collision_129_faces[11] = {
#include "stg22/terrain/collision_129_faces.inc"
};
static NJS_MESHSET collision_129_meshsets[1] = {
#include "stg22/terrain/collision_129_meshsets.inc"
};
static NJS_POINT3 collision_129_points[8] = {
#include "stg22/terrain/collision_129_points.inc"
};
static NJS_MODEL collision_129_model = {
#include "stg22/terrain/collision_129_model.inc"
};
static NJS_OBJECT collision_129 = {
#include "stg22/terrain/collision_129.inc"
};
static NJS_MATERIAL collision_131_materials[1] = {
#include "stg22/terrain/collision_131_materials.inc"
};
static Sint16 collision_131_faces[53] = {
#include "stg22/terrain/collision_131_faces.inc"
};
static NJS_MESHSET collision_131_meshsets[1] = {
#include "stg22/terrain/collision_131_meshsets.inc"
};
static NJS_POINT3 collision_131_points[29] = {
#include "stg22/terrain/collision_131_points.inc"
};
static NJS_MODEL collision_131_model = {
#include "stg22/terrain/collision_131_model.inc"
};
static NJS_OBJECT collision_131 = {
#include "stg22/terrain/collision_131.inc"
};
static NJS_MATERIAL collision_133_materials[1] = {
#include "stg22/terrain/collision_133_materials.inc"
};
static Sint16 collision_133_faces[51] = {
#include "stg22/terrain/collision_133_faces.inc"
};
static NJS_MESHSET collision_133_meshsets[1] = {
#include "stg22/terrain/collision_133_meshsets.inc"
};
static NJS_POINT3 collision_133_points[20] = {
#include "stg22/terrain/collision_133_points.inc"
};
static NJS_MODEL collision_133_model = {
#include "stg22/terrain/collision_133_model.inc"
};
static NJS_OBJECT collision_133 = {
#include "stg22/terrain/collision_133.inc"
};
static NJS_MATERIAL collision_134_materials[1] = {
#include "stg22/terrain/collision_134_materials.inc"
};
static Sint16 collision_134_faces[47] = {
#include "stg22/terrain/collision_134_faces.inc"
};
static NJS_MESHSET collision_134_meshsets[1] = {
#include "stg22/terrain/collision_134_meshsets.inc"
};
static NJS_POINT3 collision_134_points[30] = {
#include "stg22/terrain/collision_134_points.inc"
};
static NJS_MODEL collision_134_model = {
#include "stg22/terrain/collision_134_model.inc"
};
static NJS_OBJECT collision_134 = {
#include "stg22/terrain/collision_134.inc"
};
static NJS_MATERIAL collision_136_materials[1] = {
#include "stg22/terrain/collision_136_materials.inc"
};
static Sint16 collision_136_faces[20] = {
#include "stg22/terrain/collision_136_faces.inc"
};
static NJS_MESHSET collision_136_meshsets[1] = {
#include "stg22/terrain/collision_136_meshsets.inc"
};
static NJS_POINT3 collision_136_points[9] = {
#include "stg22/terrain/collision_136_points.inc"
};
static NJS_MODEL collision_136_model = {
#include "stg22/terrain/collision_136_model.inc"
};
static NJS_OBJECT collision_136 = {
#include "stg22/terrain/collision_136.inc"
};
static NJS_MATERIAL collision_137_materials[1] = {
#include "stg22/terrain/collision_137_materials.inc"
};
static Sint16 collision_137_faces[52] = {
#include "stg22/terrain/collision_137_faces.inc"
};
static NJS_MESHSET collision_137_meshsets[1] = {
#include "stg22/terrain/collision_137_meshsets.inc"
};
static NJS_POINT3 collision_137_points[24] = {
#include "stg22/terrain/collision_137_points.inc"
};
static NJS_MODEL collision_137_model = {
#include "stg22/terrain/collision_137_model.inc"
};
static NJS_OBJECT collision_137 = {
#include "stg22/terrain/collision_137.inc"
};
static NJS_MATERIAL collision_144_materials[1] = {
#include "stg22/terrain/collision_144_materials.inc"
};
static Sint16 collision_144_faces[20] = {
#include "stg22/terrain/collision_144_faces.inc"
};
static NJS_MESHSET collision_144_meshsets[1] = {
#include "stg22/terrain/collision_144_meshsets.inc"
};
static NJS_POINT3 collision_144_points[12] = {
#include "stg22/terrain/collision_144_points.inc"
};
static NJS_MODEL collision_144_model = {
#include "stg22/terrain/collision_144_model.inc"
};
static NJS_OBJECT collision_144 = {
#include "stg22/terrain/collision_144.inc"
};
static NJS_MATERIAL collision_168_materials[1] = {
#include "stg22/terrain/collision_168_materials.inc"
};
static Sint16 collision_168_faces[15] = {
#include "stg22/terrain/collision_168_faces.inc"
};
static NJS_MESHSET collision_168_meshsets[1] = {
#include "stg22/terrain/collision_168_meshsets.inc"
};
static NJS_POINT3 collision_168_points[13] = {
#include "stg22/terrain/collision_168_points.inc"
};
static NJS_MODEL collision_168_model = {
#include "stg22/terrain/collision_168_model.inc"
};
static NJS_OBJECT collision_168 = {
#include "stg22/terrain/collision_168.inc"
};
static NJS_MATERIAL collision_169_materials[1] = {
#include "stg22/terrain/collision_169_materials.inc"
};
static Sint16 collision_169_faces[61] = {
#include "stg22/terrain/collision_169_faces.inc"
};
static NJS_MESHSET collision_169_meshsets[1] = {
#include "stg22/terrain/collision_169_meshsets.inc"
};
static NJS_POINT3 collision_169_points[31] = {
#include "stg22/terrain/collision_169_points.inc"
};
static NJS_MODEL collision_169_model = {
#include "stg22/terrain/collision_169_model.inc"
};
static NJS_OBJECT collision_169 = {
#include "stg22/terrain/collision_169.inc"
};
static NJS_MATERIAL collision_171_materials[1] = {
#include "stg22/terrain/collision_171_materials.inc"
};
static Sint16 collision_171_faces[14] = {
#include "stg22/terrain/collision_171_faces.inc"
};
static NJS_MESHSET collision_171_meshsets[1] = {
#include "stg22/terrain/collision_171_meshsets.inc"
};
static NJS_POINT3 collision_171_points[12] = {
#include "stg22/terrain/collision_171_points.inc"
};
static NJS_MODEL collision_171_model = {
#include "stg22/terrain/collision_171_model.inc"
};
static NJS_OBJECT collision_171 = {
#include "stg22/terrain/collision_171.inc"
};
static NJS_MATERIAL collision_178_materials[1] = {
#include "stg22/terrain/collision_178_materials.inc"
};
static Sint16 collision_178_faces[68] = {
#include "stg22/terrain/collision_178_faces.inc"
};
static NJS_MESHSET collision_178_meshsets[1] = {
#include "stg22/terrain/collision_178_meshsets.inc"
};
static NJS_POINT3 collision_178_points[31] = {
#include "stg22/terrain/collision_178_points.inc"
};
static NJS_MODEL collision_178_model = {
#include "stg22/terrain/collision_178_model.inc"
};
static NJS_OBJECT collision_178 = {
#include "stg22/terrain/collision_178.inc"
};
static NJS_MATERIAL collision_182_materials[1] = {
#include "stg22/terrain/collision_182_materials.inc"
};
static Sint16 collision_182_faces[35] = {
#include "stg22/terrain/collision_182_faces.inc"
};
static NJS_MESHSET collision_182_meshsets[1] = {
#include "stg22/terrain/collision_182_meshsets.inc"
};
static NJS_POINT3 collision_182_points[24] = {
#include "stg22/terrain/collision_182_points.inc"
};
static NJS_MODEL collision_182_model = {
#include "stg22/terrain/collision_182_model.inc"
};
static NJS_OBJECT collision_182 = {
#include "stg22/terrain/collision_182.inc"
};
static NJS_MATERIAL collision_186_materials[1] = {
#include "stg22/terrain/collision_186_materials.inc"
};
static Sint16 collision_186_faces[40] = {
#include "stg22/terrain/collision_186_faces.inc"
};
static NJS_MESHSET collision_186_meshsets[1] = {
#include "stg22/terrain/collision_186_meshsets.inc"
};
static NJS_POINT3 collision_186_points[12] = {
#include "stg22/terrain/collision_186_points.inc"
};
static NJS_MODEL collision_186_model = {
#include "stg22/terrain/collision_186_model.inc"
};
static NJS_OBJECT collision_186 = {
#include "stg22/terrain/collision_186.inc"
};
static NJS_MATERIAL collision_189_materials[1] = {
#include "stg22/terrain/collision_189_materials.inc"
};
static Sint16 collision_189_faces[56] = {
#include "stg22/terrain/collision_189_faces.inc"
};
static NJS_MESHSET collision_189_meshsets[1] = {
#include "stg22/terrain/collision_189_meshsets.inc"
};
static NJS_POINT3 collision_189_points[25] = {
#include "stg22/terrain/collision_189_points.inc"
};
static NJS_MODEL collision_189_model = {
#include "stg22/terrain/collision_189_model.inc"
};
static NJS_OBJECT collision_189 = {
#include "stg22/terrain/collision_189.inc"
};
static NJS_MATERIAL collision_191_materials[1] = {
#include "stg22/terrain/collision_191_materials.inc"
};
static Sint16 collision_191_faces[20] = {
#include "stg22/terrain/collision_191_faces.inc"
};
static NJS_MESHSET collision_191_meshsets[1] = {
#include "stg22/terrain/collision_191_meshsets.inc"
};
static NJS_POINT3 collision_191_points[10] = {
#include "stg22/terrain/collision_191_points.inc"
};
static NJS_MODEL collision_191_model = {
#include "stg22/terrain/collision_191_model.inc"
};
static NJS_OBJECT collision_191 = {
#include "stg22/terrain/collision_191.inc"
};
static NJS_MATERIAL collision_223_materials[1] = {
#include "stg22/terrain/collision_223_materials.inc"
};
static Sint16 collision_223_faces[40] = {
#include "stg22/terrain/collision_223_faces.inc"
};
static NJS_MESHSET collision_223_meshsets[1] = {
#include "stg22/terrain/collision_223_meshsets.inc"
};
static NJS_POINT3 collision_223_points[23] = {
#include "stg22/terrain/collision_223_points.inc"
};
static NJS_MODEL collision_223_model = {
#include "stg22/terrain/collision_223_model.inc"
};
static NJS_OBJECT collision_223 = {
#include "stg22/terrain/collision_223.inc"
};
static NJS_MATERIAL collision_225_materials[1] = {
#include "stg22/terrain/collision_225_materials.inc"
};
static Sint16 collision_225_faces[25] = {
#include "stg22/terrain/collision_225_faces.inc"
};
static NJS_MESHSET collision_225_meshsets[1] = {
#include "stg22/terrain/collision_225_meshsets.inc"
};
static NJS_POINT3 collision_225_points[18] = {
#include "stg22/terrain/collision_225_points.inc"
};
static NJS_MODEL collision_225_model = {
#include "stg22/terrain/collision_225_model.inc"
};
static NJS_OBJECT collision_225 = {
#include "stg22/terrain/collision_225.inc"
};
static NJS_MATERIAL collision_227_materials[1] = {
#include "stg22/terrain/collision_227_materials.inc"
};
static Sint16 collision_227_faces[7] = {
#include "stg22/terrain/collision_227_faces.inc"
};
static NJS_MESHSET collision_227_meshsets[1] = {
#include "stg22/terrain/collision_227_meshsets.inc"
};
static NJS_POINT3 collision_227_points[6] = {
#include "stg22/terrain/collision_227_points.inc"
};
static NJS_MODEL collision_227_model = {
#include "stg22/terrain/collision_227_model.inc"
};
static NJS_OBJECT collision_227 = {
#include "stg22/terrain/collision_227.inc"
};
static NJS_MATERIAL collision_231_materials[1] = {
#include "stg22/terrain/collision_231_materials.inc"
};
static Sint16 collision_231_faces[14] = {
#include "stg22/terrain/collision_231_faces.inc"
};
static NJS_MESHSET collision_231_meshsets[1] = {
#include "stg22/terrain/collision_231_meshsets.inc"
};
static NJS_POINT3 collision_231_points[12] = {
#include "stg22/terrain/collision_231_points.inc"
};
static NJS_MODEL collision_231_model = {
#include "stg22/terrain/collision_231_model.inc"
};
static NJS_OBJECT collision_231 = {
#include "stg22/terrain/collision_231.inc"
};
static NJS_MATERIAL collision_232_materials[1] = {
#include "stg22/terrain/collision_232_materials.inc"
};
static Sint16 collision_232_faces[22] = {
#include "stg22/terrain/collision_232_faces.inc"
};
static NJS_MESHSET collision_232_meshsets[1] = {
#include "stg22/terrain/collision_232_meshsets.inc"
};
static NJS_POINT3 collision_232_points[15] = {
#include "stg22/terrain/collision_232_points.inc"
};
static NJS_MODEL collision_232_model = {
#include "stg22/terrain/collision_232_model.inc"
};
static NJS_OBJECT collision_232 = {
#include "stg22/terrain/collision_232.inc"
};
static NJS_MATERIAL collision_233_materials[1] = {
#include "stg22/terrain/collision_233_materials.inc"
};
static Sint16 collision_233_faces[22] = {
#include "stg22/terrain/collision_233_faces.inc"
};
static NJS_MESHSET collision_233_meshsets[1] = {
#include "stg22/terrain/collision_233_meshsets.inc"
};
static NJS_POINT3 collision_233_points[15] = {
#include "stg22/terrain/collision_233_points.inc"
};
static NJS_MODEL collision_233_model = {
#include "stg22/terrain/collision_233_model.inc"
};
static NJS_OBJECT collision_233 = {
#include "stg22/terrain/collision_233.inc"
};
static NJS_MATERIAL collision_234_materials[1] = {
#include "stg22/terrain/collision_234_materials.inc"
};
static Sint16 collision_234_faces[22] = {
#include "stg22/terrain/collision_234_faces.inc"
};
static NJS_MESHSET collision_234_meshsets[1] = {
#include "stg22/terrain/collision_234_meshsets.inc"
};
static NJS_POINT3 collision_234_points[15] = {
#include "stg22/terrain/collision_234_points.inc"
};
static NJS_MODEL collision_234_model = {
#include "stg22/terrain/collision_234_model.inc"
};
static NJS_OBJECT collision_234 = {
#include "stg22/terrain/collision_234.inc"
};
static NJS_MATERIAL collision_236_materials[1] = {
#include "stg22/terrain/collision_236_materials.inc"
};
static Sint16 collision_236_faces[27] = {
#include "stg22/terrain/collision_236_faces.inc"
};
static NJS_MESHSET collision_236_meshsets[1] = {
#include "stg22/terrain/collision_236_meshsets.inc"
};
static NJS_POINT3 collision_236_points[24] = {
#include "stg22/terrain/collision_236_points.inc"
};
static NJS_MODEL collision_236_model = {
#include "stg22/terrain/collision_236_model.inc"
};
static NJS_OBJECT collision_236 = {
#include "stg22/terrain/collision_236.inc"
};
static NJS_MATERIAL collision_238_materials[1] = {
#include "stg22/terrain/collision_238_materials.inc"
};
static Sint16 collision_238_faces[21] = {
#include "stg22/terrain/collision_238_faces.inc"
};
static NJS_MESHSET collision_238_meshsets[1] = {
#include "stg22/terrain/collision_238_meshsets.inc"
};
static NJS_POINT3 collision_238_points[12] = {
#include "stg22/terrain/collision_238_points.inc"
};
static NJS_MODEL collision_238_model = {
#include "stg22/terrain/collision_238_model.inc"
};
static NJS_OBJECT collision_238 = {
#include "stg22/terrain/collision_238.inc"
};
static NJS_MATERIAL collision_240_materials[1] = {
#include "stg22/terrain/collision_240_materials.inc"
};
static Sint16 collision_240_faces[16] = {
#include "stg22/terrain/collision_240_faces.inc"
};
static NJS_MESHSET collision_240_meshsets[1] = {
#include "stg22/terrain/collision_240_meshsets.inc"
};
static NJS_POINT3 collision_240_points[8] = {
#include "stg22/terrain/collision_240_points.inc"
};
static NJS_MODEL collision_240_model = {
#include "stg22/terrain/collision_240_model.inc"
};
static NJS_OBJECT collision_240 = {
#include "stg22/terrain/collision_240.inc"
};
static NJS_MATERIAL collision_242_materials[1] = {
#include "stg22/terrain/collision_242_materials.inc"
};
static Sint16 collision_242_faces[46] = {
#include "stg22/terrain/collision_242_faces.inc"
};
static NJS_MESHSET collision_242_meshsets[1] = {
#include "stg22/terrain/collision_242_meshsets.inc"
};
static NJS_POINT3 collision_242_points[22] = {
#include "stg22/terrain/collision_242_points.inc"
};
static NJS_MODEL collision_242_model = {
#include "stg22/terrain/collision_242_model.inc"
};
static NJS_OBJECT collision_242 = {
#include "stg22/terrain/collision_242.inc"
};
static NJS_MATERIAL collision_244_materials[1] = {
#include "stg22/terrain/collision_244_materials.inc"
};
static Sint16 collision_244_faces[25] = {
#include "stg22/terrain/collision_244_faces.inc"
};
static NJS_MESHSET collision_244_meshsets[1] = {
#include "stg22/terrain/collision_244_meshsets.inc"
};
static NJS_POINT3 collision_244_points[18] = {
#include "stg22/terrain/collision_244_points.inc"
};
static NJS_MODEL collision_244_model = {
#include "stg22/terrain/collision_244_model.inc"
};
static NJS_OBJECT collision_244 = {
#include "stg22/terrain/collision_244.inc"
};
static NJS_MATERIAL collision_247_materials[1] = {
#include "stg22/terrain/collision_247_materials.inc"
};
static Sint16 collision_247_faces[28] = {
#include "stg22/terrain/collision_247_faces.inc"
};
static NJS_MESHSET collision_247_meshsets[1] = {
#include "stg22/terrain/collision_247_meshsets.inc"
};
static NJS_POINT3 collision_247_points[20] = {
#include "stg22/terrain/collision_247_points.inc"
};
static NJS_MODEL collision_247_model = {
#include "stg22/terrain/collision_247_model.inc"
};
static NJS_OBJECT collision_247 = {
#include "stg22/terrain/collision_247.inc"
};
static NJS_MATERIAL collision_249_materials[1] = {
#include "stg22/terrain/collision_249_materials.inc"
};
static Sint16 collision_249_faces[57] = {
#include "stg22/terrain/collision_249_faces.inc"
};
static NJS_MESHSET collision_249_meshsets[1] = {
#include "stg22/terrain/collision_249_meshsets.inc"
};
static NJS_POINT3 collision_249_points[24] = {
#include "stg22/terrain/collision_249_points.inc"
};
static NJS_MODEL collision_249_model = {
#include "stg22/terrain/collision_249_model.inc"
};
static NJS_OBJECT collision_249 = {
#include "stg22/terrain/collision_249.inc"
};
static NJS_MATERIAL collision_250_materials[1] = {
#include "stg22/terrain/collision_250_materials.inc"
};
static Sint16 collision_250_faces[18] = {
#include "stg22/terrain/collision_250_faces.inc"
};
static NJS_MESHSET collision_250_meshsets[1] = {
#include "stg22/terrain/collision_250_meshsets.inc"
};
static NJS_POINT3 collision_250_points[9] = {
#include "stg22/terrain/collision_250_points.inc"
};
static NJS_MODEL collision_250_model = {
#include "stg22/terrain/collision_250_model.inc"
};
static NJS_OBJECT collision_250 = {
#include "stg22/terrain/collision_250.inc"
};
static NJS_MATERIAL collision_251_materials[1] = {
#include "stg22/terrain/collision_251_materials.inc"
};
static Sint16 collision_251_faces[56] = {
#include "stg22/terrain/collision_251_faces.inc"
};
static NJS_MESHSET collision_251_meshsets[1] = {
#include "stg22/terrain/collision_251_meshsets.inc"
};
static NJS_POINT3 collision_251_points[24] = {
#include "stg22/terrain/collision_251_points.inc"
};
static NJS_MODEL collision_251_model = {
#include "stg22/terrain/collision_251_model.inc"
};
static NJS_OBJECT collision_251 = {
#include "stg22/terrain/collision_251.inc"
};
static NJS_MATERIAL collision_252_materials[1] = {
#include "stg22/terrain/collision_252_materials.inc"
};
static Sint16 collision_252_faces[14] = {
#include "stg22/terrain/collision_252_faces.inc"
};
static NJS_MESHSET collision_252_meshsets[1] = {
#include "stg22/terrain/collision_252_meshsets.inc"
};
static NJS_POINT3 collision_252_points[12] = {
#include "stg22/terrain/collision_252_points.inc"
};
static NJS_MODEL collision_252_model = {
#include "stg22/terrain/collision_252_model.inc"
};
static NJS_OBJECT collision_252 = {
#include "stg22/terrain/collision_252.inc"
};
static NJS_MATERIAL collision_262_materials[1] = {
#include "stg22/terrain/collision_262_materials.inc"
};
static Sint16 collision_262_faces[19] = {
#include "stg22/terrain/collision_262_faces.inc"
};
static NJS_MESHSET collision_262_meshsets[1] = {
#include "stg22/terrain/collision_262_meshsets.inc"
};
static NJS_POINT3 collision_262_points[16] = {
#include "stg22/terrain/collision_262_points.inc"
};
static NJS_MODEL collision_262_model = {
#include "stg22/terrain/collision_262_model.inc"
};
static NJS_OBJECT collision_262 = {
#include "stg22/terrain/collision_262.inc"
};
static NJS_MATERIAL collision_263_materials[1] = {
#include "stg22/terrain/collision_263_materials.inc"
};
static Sint16 collision_263_faces[14] = {
#include "stg22/terrain/collision_263_faces.inc"
};
static NJS_MESHSET collision_263_meshsets[1] = {
#include "stg22/terrain/collision_263_meshsets.inc"
};
static NJS_POINT3 collision_263_points[12] = {
#include "stg22/terrain/collision_263_points.inc"
};
static NJS_MODEL collision_263_model = {
#include "stg22/terrain/collision_263_model.inc"
};
static NJS_OBJECT collision_263 = {
#include "stg22/terrain/collision_263.inc"
};
static NJS_MATERIAL collision_264_materials[1] = {
#include "stg22/terrain/collision_264_materials.inc"
};
static Sint16 collision_264_faces[10] = {
#include "stg22/terrain/collision_264_faces.inc"
};
static NJS_MESHSET collision_264_meshsets[1] = {
#include "stg22/terrain/collision_264_meshsets.inc"
};
static NJS_POINT3 collision_264_points[8] = {
#include "stg22/terrain/collision_264_points.inc"
};
static NJS_MODEL collision_264_model = {
#include "stg22/terrain/collision_264_model.inc"
};
static NJS_OBJECT collision_264 = {
#include "stg22/terrain/collision_264.inc"
};
static NJS_MATERIAL collision_265_materials[1] = {
#include "stg22/terrain/collision_265_materials.inc"
};
static Sint16 collision_265_faces[38] = {
#include "stg22/terrain/collision_265_faces.inc"
};
static NJS_MESHSET collision_265_meshsets[1] = {
#include "stg22/terrain/collision_265_meshsets.inc"
};
static NJS_POINT3 collision_265_points[24] = {
#include "stg22/terrain/collision_265_points.inc"
};
static NJS_MODEL collision_265_model = {
#include "stg22/terrain/collision_265_model.inc"
};
static NJS_OBJECT collision_265 = {
#include "stg22/terrain/collision_265.inc"
};
static NJS_MATERIAL collision_266_materials[1] = {
#include "stg22/terrain/collision_266_materials.inc"
};
static Sint16 collision_266_faces[14] = {
#include "stg22/terrain/collision_266_faces.inc"
};
static NJS_MESHSET collision_266_meshsets[1] = {
#include "stg22/terrain/collision_266_meshsets.inc"
};
static NJS_POINT3 collision_266_points[12] = {
#include "stg22/terrain/collision_266_points.inc"
};
static NJS_MODEL collision_266_model = {
#include "stg22/terrain/collision_266_model.inc"
};
static NJS_OBJECT collision_266 = {
#include "stg22/terrain/collision_266.inc"
};
static NJS_MATERIAL collision_272_materials[1] = {
#include "stg22/terrain/collision_272_materials.inc"
};
static Sint16 collision_272_faces[97] = {
#include "stg22/terrain/collision_272_faces.inc"
};
static NJS_MESHSET collision_272_meshsets[1] = {
#include "stg22/terrain/collision_272_meshsets.inc"
};
static NJS_POINT3 collision_272_points[42] = {
#include "stg22/terrain/collision_272_points.inc"
};
static NJS_MODEL collision_272_model = {
#include "stg22/terrain/collision_272_model.inc"
};
static NJS_OBJECT collision_272 = {
#include "stg22/terrain/collision_272.inc"
};
static NJS_MATERIAL collision_273_materials[1] = {
#include "stg22/terrain/collision_273_materials.inc"
};
static Sint16 collision_273_faces[97] = {
#include "stg22/terrain/collision_273_faces.inc"
};
static NJS_MESHSET collision_273_meshsets[1] = {
#include "stg22/terrain/collision_273_meshsets.inc"
};
static NJS_POINT3 collision_273_points[44] = {
#include "stg22/terrain/collision_273_points.inc"
};
static NJS_MODEL collision_273_model = {
#include "stg22/terrain/collision_273_model.inc"
};
static NJS_OBJECT collision_273 = {
#include "stg22/terrain/collision_273.inc"
};
static NJS_MATERIAL collision_274_materials[1] = {
#include "stg22/terrain/collision_274_materials.inc"
};
static Sint16 collision_274_faces[65] = {
#include "stg22/terrain/collision_274_faces.inc"
};
static NJS_MESHSET collision_274_meshsets[1] = {
#include "stg22/terrain/collision_274_meshsets.inc"
};
static NJS_POINT3 collision_274_points[24] = {
#include "stg22/terrain/collision_274_points.inc"
};
static NJS_MODEL collision_274_model = {
#include "stg22/terrain/collision_274_model.inc"
};
static NJS_OBJECT collision_274 = {
#include "stg22/terrain/collision_274.inc"
};
static NJS_MATERIAL collision_275_materials[1] = {
#include "stg22/terrain/collision_275_materials.inc"
};
static Sint16 collision_275_faces[80] = {
#include "stg22/terrain/collision_275_faces.inc"
};
static NJS_MESHSET collision_275_meshsets[1] = {
#include "stg22/terrain/collision_275_meshsets.inc"
};
static NJS_POINT3 collision_275_points[32] = {
#include "stg22/terrain/collision_275_points.inc"
};
static NJS_MODEL collision_275_model = {
#include "stg22/terrain/collision_275_model.inc"
};
static NJS_OBJECT collision_275 = {
#include "stg22/terrain/collision_275.inc"
};
static NJS_MATERIAL collision_276_materials[1] = {
#include "stg22/terrain/collision_276_materials.inc"
};
static Sint16 collision_276_faces[74] = {
#include "stg22/terrain/collision_276_faces.inc"
};
static NJS_MESHSET collision_276_meshsets[1] = {
#include "stg22/terrain/collision_276_meshsets.inc"
};
static NJS_POINT3 collision_276_points[30] = {
#include "stg22/terrain/collision_276_points.inc"
};
static NJS_MODEL collision_276_model = {
#include "stg22/terrain/collision_276_model.inc"
};
static NJS_OBJECT collision_276 = {
#include "stg22/terrain/collision_276.inc"
};
static NJS_MATERIAL collision_277_materials[1] = {
#include "stg22/terrain/collision_277_materials.inc"
};
static Sint16 collision_277_faces[30] = {
#include "stg22/terrain/collision_277_faces.inc"
};
static NJS_MESHSET collision_277_meshsets[1] = {
#include "stg22/terrain/collision_277_meshsets.inc"
};
static NJS_POINT3 collision_277_points[20] = {
#include "stg22/terrain/collision_277_points.inc"
};
static NJS_MODEL collision_277_model = {
#include "stg22/terrain/collision_277_model.inc"
};
static NJS_OBJECT collision_277 = {
#include "stg22/terrain/collision_277.inc"
};
static NJS_MATERIAL collision_278_materials[1] = {
#include "stg22/terrain/collision_278_materials.inc"
};
static Sint16 collision_278_faces[25] = {
#include "stg22/terrain/collision_278_faces.inc"
};
static NJS_MESHSET collision_278_meshsets[1] = {
#include "stg22/terrain/collision_278_meshsets.inc"
};
static NJS_POINT3 collision_278_points[20] = {
#include "stg22/terrain/collision_278_points.inc"
};
static NJS_MODEL collision_278_model = {
#include "stg22/terrain/collision_278_model.inc"
};
static NJS_OBJECT collision_278 = {
#include "stg22/terrain/collision_278.inc"
};
static NJS_MATERIAL collision_279_materials[1] = {
#include "stg22/terrain/collision_279_materials.inc"
};
static Sint16 collision_279_faces[7] = {
#include "stg22/terrain/collision_279_faces.inc"
};
static NJS_MESHSET collision_279_meshsets[1] = {
#include "stg22/terrain/collision_279_meshsets.inc"
};
static NJS_POINT3 collision_279_points[6] = {
#include "stg22/terrain/collision_279_points.inc"
};
static NJS_MODEL collision_279_model = {
#include "stg22/terrain/collision_279_model.inc"
};
static NJS_OBJECT collision_279 = {
#include "stg22/terrain/collision_279.inc"
};
static NJS_MATERIAL collision_280_materials[1] = {
#include "stg22/terrain/collision_280_materials.inc"
};
static Sint16 collision_280_faces[23] = {
#include "stg22/terrain/collision_280_faces.inc"
};
static NJS_MESHSET collision_280_meshsets[1] = {
#include "stg22/terrain/collision_280_meshsets.inc"
};
static NJS_POINT3 collision_280_points[16] = {
#include "stg22/terrain/collision_280_points.inc"
};
static NJS_MODEL collision_280_model = {
#include "stg22/terrain/collision_280_model.inc"
};
static NJS_OBJECT collision_280 = {
#include "stg22/terrain/collision_280.inc"
};
static NJS_MATERIAL collision_281_materials[1] = {
#include "stg22/terrain/collision_281_materials.inc"
};
static Sint16 collision_281_faces[11] = {
#include "stg22/terrain/collision_281_faces.inc"
};
static NJS_MESHSET collision_281_meshsets[1] = {
#include "stg22/terrain/collision_281_meshsets.inc"
};
static NJS_POINT3 collision_281_points[10] = {
#include "stg22/terrain/collision_281_points.inc"
};
static NJS_MODEL collision_281_model = {
#include "stg22/terrain/collision_281_model.inc"
};
static NJS_OBJECT collision_281 = {
#include "stg22/terrain/collision_281.inc"
};
static NJS_MATERIAL collision_282_materials[1] = {
#include "stg22/terrain/collision_282_materials.inc"
};
static Sint16 collision_282_faces[7] = {
#include "stg22/terrain/collision_282_faces.inc"
};
static NJS_MESHSET collision_282_meshsets[1] = {
#include "stg22/terrain/collision_282_meshsets.inc"
};
static NJS_POINT3 collision_282_points[6] = {
#include "stg22/terrain/collision_282_points.inc"
};
static NJS_MODEL collision_282_model = {
#include "stg22/terrain/collision_282_model.inc"
};
static NJS_OBJECT collision_282 = {
#include "stg22/terrain/collision_282.inc"
};
static NJS_MATERIAL collision_283_materials[1] = {
#include "stg22/terrain/collision_283_materials.inc"
};
static Sint16 collision_283_faces[9] = {
#include "stg22/terrain/collision_283_faces.inc"
};
static NJS_MESHSET collision_283_meshsets[1] = {
#include "stg22/terrain/collision_283_meshsets.inc"
};
static NJS_POINT3 collision_283_points[8] = {
#include "stg22/terrain/collision_283_points.inc"
};
static NJS_MODEL collision_283_model = {
#include "stg22/terrain/collision_283_model.inc"
};
static NJS_OBJECT collision_283 = {
#include "stg22/terrain/collision_283.inc"
};
static NJS_MATERIAL collision_284_materials[1] = {
#include "stg22/terrain/collision_284_materials.inc"
};
static Sint16 collision_284_faces[9] = {
#include "stg22/terrain/collision_284_faces.inc"
};
static NJS_MESHSET collision_284_meshsets[1] = {
#include "stg22/terrain/collision_284_meshsets.inc"
};
static NJS_POINT3 collision_284_points[8] = {
#include "stg22/terrain/collision_284_points.inc"
};
static NJS_MODEL collision_284_model = {
#include "stg22/terrain/collision_284_model.inc"
};
static NJS_OBJECT collision_284 = {
#include "stg22/terrain/collision_284.inc"
};
static NJS_MATERIAL collision_285_materials[1] = {
#include "stg22/terrain/collision_285_materials.inc"
};
static Sint16 collision_285_faces[36] = {
#include "stg22/terrain/collision_285_faces.inc"
};
static NJS_MESHSET collision_285_meshsets[1] = {
#include "stg22/terrain/collision_285_meshsets.inc"
};
static NJS_POINT3 collision_285_points[26] = {
#include "stg22/terrain/collision_285_points.inc"
};
static NJS_MODEL collision_285_model = {
#include "stg22/terrain/collision_285_model.inc"
};
static NJS_OBJECT collision_285 = {
#include "stg22/terrain/collision_285.inc"
};
static NJS_MATERIAL collision_286_materials[1] = {
#include "stg22/terrain/collision_286_materials.inc"
};
static Sint16 collision_286_faces[32] = {
#include "stg22/terrain/collision_286_faces.inc"
};
static NJS_MESHSET collision_286_meshsets[1] = {
#include "stg22/terrain/collision_286_meshsets.inc"
};
static NJS_POINT3 collision_286_points[22] = {
#include "stg22/terrain/collision_286_points.inc"
};
static NJS_MODEL collision_286_model = {
#include "stg22/terrain/collision_286_model.inc"
};
static NJS_OBJECT collision_286 = {
#include "stg22/terrain/collision_286.inc"
};
static NJS_MATERIAL collision_287_materials[1] = {
#include "stg22/terrain/collision_287_materials.inc"
};
static Sint16 collision_287_faces[36] = {
#include "stg22/terrain/collision_287_faces.inc"
};
static NJS_MESHSET collision_287_meshsets[1] = {
#include "stg22/terrain/collision_287_meshsets.inc"
};
static NJS_POINT3 collision_287_points[26] = {
#include "stg22/terrain/collision_287_points.inc"
};
static NJS_MODEL collision_287_model = {
#include "stg22/terrain/collision_287_model.inc"
};
static NJS_OBJECT collision_287 = {
#include "stg22/terrain/collision_287.inc"
};
static NJS_MATERIAL collision_288_materials[1] = {
#include "stg22/terrain/collision_288_materials.inc"
};
static Sint16 collision_288_faces[9] = {
#include "stg22/terrain/collision_288_faces.inc"
};
static NJS_MESHSET collision_288_meshsets[1] = {
#include "stg22/terrain/collision_288_meshsets.inc"
};
static NJS_POINT3 collision_288_points[8] = {
#include "stg22/terrain/collision_288_points.inc"
};
static NJS_MODEL collision_288_model = {
#include "stg22/terrain/collision_288_model.inc"
};
static NJS_OBJECT collision_288 = {
#include "stg22/terrain/collision_288.inc"
};
static NJS_MATERIAL collision_289_materials[1] = {
#include "stg22/terrain/collision_289_materials.inc"
};
static Sint16 collision_289_faces[18] = {
#include "stg22/terrain/collision_289_faces.inc"
};
static NJS_MESHSET collision_289_meshsets[1] = {
#include "stg22/terrain/collision_289_meshsets.inc"
};
static NJS_POINT3 collision_289_points[14] = {
#include "stg22/terrain/collision_289_points.inc"
};
static NJS_MODEL collision_289_model = {
#include "stg22/terrain/collision_289_model.inc"
};
static NJS_OBJECT collision_289 = {
#include "stg22/terrain/collision_289.inc"
};
static NJS_MATERIAL collision_290_materials[1] = {
#include "stg22/terrain/collision_290_materials.inc"
};
static Sint16 collision_290_faces[40] = {
#include "stg22/terrain/collision_290_faces.inc"
};
static NJS_MESHSET collision_290_meshsets[1] = {
#include "stg22/terrain/collision_290_meshsets.inc"
};
static NJS_POINT3 collision_290_points[30] = {
#include "stg22/terrain/collision_290_points.inc"
};
static NJS_MODEL collision_290_model = {
#include "stg22/terrain/collision_290_model.inc"
};
static NJS_OBJECT collision_290 = {
#include "stg22/terrain/collision_290.inc"
};
static NJS_MATERIAL collision_291_materials[1] = {
#include "stg22/terrain/collision_291_materials.inc"
};
static Sint16 collision_291_faces[14] = {
#include "stg22/terrain/collision_291_faces.inc"
};
static NJS_MESHSET collision_291_meshsets[1] = {
#include "stg22/terrain/collision_291_meshsets.inc"
};
static NJS_POINT3 collision_291_points[10] = {
#include "stg22/terrain/collision_291_points.inc"
};
static NJS_MODEL collision_291_model = {
#include "stg22/terrain/collision_291_model.inc"
};
static NJS_OBJECT collision_291 = {
#include "stg22/terrain/collision_291.inc"
};
static NJS_MATERIAL collision_292_materials[1] = {
#include "stg22/terrain/collision_292_materials.inc"
};
static Sint16 collision_292_faces[16] = {
#include "stg22/terrain/collision_292_faces.inc"
};
static NJS_MESHSET collision_292_meshsets[1] = {
#include "stg22/terrain/collision_292_meshsets.inc"
};
static NJS_POINT3 collision_292_points[12] = {
#include "stg22/terrain/collision_292_points.inc"
};
static NJS_MODEL collision_292_model = {
#include "stg22/terrain/collision_292_model.inc"
};
static NJS_OBJECT collision_292 = {
#include "stg22/terrain/collision_292.inc"
};
static NJS_MATERIAL collision_293_materials[1] = {
#include "stg22/terrain/collision_293_materials.inc"
};
static Sint16 collision_293_faces[43] = {
#include "stg22/terrain/collision_293_faces.inc"
};
static NJS_MESHSET collision_293_meshsets[1] = {
#include "stg22/terrain/collision_293_meshsets.inc"
};
static NJS_POINT3 collision_293_points[24] = {
#include "stg22/terrain/collision_293_points.inc"
};
static NJS_MODEL collision_293_model = {
#include "stg22/terrain/collision_293_model.inc"
};
static NJS_OBJECT collision_293 = {
#include "stg22/terrain/collision_293.inc"
};
static NJS_MATERIAL collision_294_materials[1] = {
#include "stg22/terrain/collision_294_materials.inc"
};
static Sint16 collision_294_faces[47] = {
#include "stg22/terrain/collision_294_faces.inc"
};
static NJS_MESHSET collision_294_meshsets[1] = {
#include "stg22/terrain/collision_294_meshsets.inc"
};
static NJS_POINT3 collision_294_points[29] = {
#include "stg22/terrain/collision_294_points.inc"
};
static NJS_MODEL collision_294_model = {
#include "stg22/terrain/collision_294_model.inc"
};
static NJS_OBJECT collision_294 = {
#include "stg22/terrain/collision_294.inc"
};
static NJS_MATERIAL collision_295_materials[1] = {
#include "stg22/terrain/collision_295_materials.inc"
};
static Sint16 collision_295_faces[16] = {
#include "stg22/terrain/collision_295_faces.inc"
};
static NJS_MESHSET collision_295_meshsets[1] = {
#include "stg22/terrain/collision_295_meshsets.inc"
};
static NJS_POINT3 collision_295_points[10] = {
#include "stg22/terrain/collision_295_points.inc"
};
static NJS_MODEL collision_295_model = {
#include "stg22/terrain/collision_295_model.inc"
};
static NJS_OBJECT collision_295 = {
#include "stg22/terrain/collision_295.inc"
};
static NJS_MATERIAL collision_296_materials[1] = {
#include "stg22/terrain/collision_296_materials.inc"
};
static Sint16 collision_296_faces[26] = {
#include "stg22/terrain/collision_296_faces.inc"
};
static NJS_MESHSET collision_296_meshsets[1] = {
#include "stg22/terrain/collision_296_meshsets.inc"
};
static NJS_POINT3 collision_296_points[18] = {
#include "stg22/terrain/collision_296_points.inc"
};
static NJS_MODEL collision_296_model = {
#include "stg22/terrain/collision_296_model.inc"
};
static NJS_OBJECT collision_296 = {
#include "stg22/terrain/collision_296.inc"
};
static NJS_MATERIAL collision_297_materials[1] = {
#include "stg22/terrain/collision_297_materials.inc"
};
static Sint16 collision_297_faces[14] = {
#include "stg22/terrain/collision_297_faces.inc"
};
static NJS_MESHSET collision_297_meshsets[1] = {
#include "stg22/terrain/collision_297_meshsets.inc"
};
static NJS_POINT3 collision_297_points[9] = {
#include "stg22/terrain/collision_297_points.inc"
};
static NJS_MODEL collision_297_model = {
#include "stg22/terrain/collision_297_model.inc"
};
static NJS_OBJECT collision_297 = {
#include "stg22/terrain/collision_297.inc"
};
static NJS_MATERIAL collision_298_materials[1] = {
#include "stg22/terrain/collision_298_materials.inc"
};
static Sint16 collision_298_faces[9] = {
#include "stg22/terrain/collision_298_faces.inc"
};
static NJS_MESHSET collision_298_meshsets[1] = {
#include "stg22/terrain/collision_298_meshsets.inc"
};
static NJS_POINT3 collision_298_points[8] = {
#include "stg22/terrain/collision_298_points.inc"
};
static NJS_MODEL collision_298_model = {
#include "stg22/terrain/collision_298_model.inc"
};
static NJS_OBJECT collision_298 = {
#include "stg22/terrain/collision_298.inc"
};
static NJS_MATERIAL collision_299_materials[1] = {
#include "stg22/terrain/collision_299_materials.inc"
};
static Sint16 collision_299_faces[7] = {
#include "stg22/terrain/collision_299_faces.inc"
};
static NJS_MESHSET collision_299_meshsets[1] = {
#include "stg22/terrain/collision_299_meshsets.inc"
};
static NJS_POINT3 collision_299_points[6] = {
#include "stg22/terrain/collision_299_points.inc"
};
static NJS_MODEL collision_299_model = {
#include "stg22/terrain/collision_299_model.inc"
};
static NJS_OBJECT collision_299 = {
#include "stg22/terrain/collision_299.inc"
};
static NJS_MATERIAL collision_300_materials[1] = {
#include "stg22/terrain/collision_300_materials.inc"
};
static Sint16 collision_300_faces[10] = {
#include "stg22/terrain/collision_300_faces.inc"
};
static NJS_MESHSET collision_300_meshsets[1] = {
#include "stg22/terrain/collision_300_meshsets.inc"
};
static NJS_POINT3 collision_300_points[6] = {
#include "stg22/terrain/collision_300_points.inc"
};
static NJS_MODEL collision_300_model = {
#include "stg22/terrain/collision_300_model.inc"
};
static NJS_OBJECT collision_300 = {
#include "stg22/terrain/collision_300.inc"
};
static NJS_MATERIAL collision_305_materials[1] = {
#include "stg22/terrain/collision_305_materials.inc"
};
static Sint16 collision_305_faces[10] = {
#include "stg22/terrain/collision_305_faces.inc"
};
static NJS_MESHSET collision_305_meshsets[1] = {
#include "stg22/terrain/collision_305_meshsets.inc"
};
static NJS_POINT3 collision_305_points[6] = {
#include "stg22/terrain/collision_305_points.inc"
};
static NJS_MODEL collision_305_model = {
#include "stg22/terrain/collision_305_model.inc"
};
static NJS_OBJECT collision_305 = {
#include "stg22/terrain/collision_305.inc"
};
static NJS_MATERIAL collision_306_materials[1] = {
#include "stg22/terrain/collision_306_materials.inc"
};
static Sint16 collision_306_faces[10] = {
#include "stg22/terrain/collision_306_faces.inc"
};
static NJS_MESHSET collision_306_meshsets[1] = {
#include "stg22/terrain/collision_306_meshsets.inc"
};
static NJS_POINT3 collision_306_points[6] = {
#include "stg22/terrain/collision_306_points.inc"
};
static NJS_MODEL collision_306_model = {
#include "stg22/terrain/collision_306_model.inc"
};
static NJS_OBJECT collision_306 = {
#include "stg22/terrain/collision_306.inc"
};
static NJS_MATERIAL collision_307_materials[1] = {
#include "stg22/terrain/collision_307_materials.inc"
};
static Sint16 collision_307_faces[58] = {
#include "stg22/terrain/collision_307_faces.inc"
};
static NJS_MESHSET collision_307_meshsets[1] = {
#include "stg22/terrain/collision_307_meshsets.inc"
};
static NJS_POINT3 collision_307_points[33] = {
#include "stg22/terrain/collision_307_points.inc"
};
static NJS_MODEL collision_307_model = {
#include "stg22/terrain/collision_307_model.inc"
};
static NJS_OBJECT collision_307 = {
#include "stg22/terrain/collision_307.inc"
};
static NJS_MATERIAL collision_308_materials[1] = {
#include "stg22/terrain/collision_308_materials.inc"
};
static Sint16 collision_308_faces[50] = {
#include "stg22/terrain/collision_308_faces.inc"
};
static NJS_MESHSET collision_308_meshsets[1] = {
#include "stg22/terrain/collision_308_meshsets.inc"
};
static NJS_POINT3 collision_308_points[27] = {
#include "stg22/terrain/collision_308_points.inc"
};
static NJS_MODEL collision_308_model = {
#include "stg22/terrain/collision_308_model.inc"
};
static NJS_OBJECT collision_308 = {
#include "stg22/terrain/collision_308.inc"
};
static NJS_MATERIAL collision_309_materials[1] = {
#include "stg22/terrain/collision_309_materials.inc"
};
static Sint16 collision_309_faces[58] = {
#include "stg22/terrain/collision_309_faces.inc"
};
static NJS_MESHSET collision_309_meshsets[1] = {
#include "stg22/terrain/collision_309_meshsets.inc"
};
static NJS_POINT3 collision_309_points[33] = {
#include "stg22/terrain/collision_309_points.inc"
};
static NJS_MODEL collision_309_model = {
#include "stg22/terrain/collision_309_model.inc"
};
static NJS_OBJECT collision_309 = {
#include "stg22/terrain/collision_309.inc"
};
static NJS_MATERIAL collision_310_materials[1] = {
#include "stg22/terrain/collision_310_materials.inc"
};
static Sint16 collision_310_faces[10] = {
#include "stg22/terrain/collision_310_faces.inc"
};
static NJS_MESHSET collision_310_meshsets[1] = {
#include "stg22/terrain/collision_310_meshsets.inc"
};
static NJS_POINT3 collision_310_points[6] = {
#include "stg22/terrain/collision_310_points.inc"
};
static NJS_MODEL collision_310_model = {
#include "stg22/terrain/collision_310_model.inc"
};
static NJS_OBJECT collision_310 = {
#include "stg22/terrain/collision_310.inc"
};
static NJS_MATERIAL collision_311_materials[1] = {
#include "stg22/terrain/collision_311_materials.inc"
};
static Sint16 collision_311_faces[28] = {
#include "stg22/terrain/collision_311_faces.inc"
};
static NJS_MESHSET collision_311_meshsets[1] = {
#include "stg22/terrain/collision_311_meshsets.inc"
};
static NJS_POINT3 collision_311_points[15] = {
#include "stg22/terrain/collision_311_points.inc"
};
static NJS_MODEL collision_311_model = {
#include "stg22/terrain/collision_311_model.inc"
};
static NJS_OBJECT collision_311 = {
#include "stg22/terrain/collision_311.inc"
};
static NJS_MATERIAL collision_312_materials[1] = {
#include "stg22/terrain/collision_312_materials.inc"
};
static Sint16 collision_312_faces[70] = {
#include "stg22/terrain/collision_312_faces.inc"
};
static NJS_MESHSET collision_312_meshsets[1] = {
#include "stg22/terrain/collision_312_meshsets.inc"
};
static NJS_POINT3 collision_312_points[42] = {
#include "stg22/terrain/collision_312_points.inc"
};
static NJS_MODEL collision_312_model = {
#include "stg22/terrain/collision_312_model.inc"
};
static NJS_OBJECT collision_312 = {
#include "stg22/terrain/collision_312.inc"
};
static NJS_MATERIAL collision_313_materials[1] = {
#include "stg22/terrain/collision_313_materials.inc"
};
static Sint16 collision_313_faces[14] = {
#include "stg22/terrain/collision_313_faces.inc"
};
static NJS_MESHSET collision_313_meshsets[1] = {
#include "stg22/terrain/collision_313_meshsets.inc"
};
static NJS_POINT3 collision_313_points[9] = {
#include "stg22/terrain/collision_313_points.inc"
};
static NJS_MODEL collision_313_model = {
#include "stg22/terrain/collision_313_model.inc"
};
static NJS_OBJECT collision_313 = {
#include "stg22/terrain/collision_313.inc"
};
static NJS_MATERIAL collision_314_materials[1] = {
#include "stg22/terrain/collision_314_materials.inc"
};
static Sint16 collision_314_faces[25] = {
#include "stg22/terrain/collision_314_faces.inc"
};
static NJS_MESHSET collision_314_meshsets[1] = {
#include "stg22/terrain/collision_314_meshsets.inc"
};
static NJS_POINT3 collision_314_points[15] = {
#include "stg22/terrain/collision_314_points.inc"
};
static NJS_MODEL collision_314_model = {
#include "stg22/terrain/collision_314_model.inc"
};
static NJS_OBJECT collision_314 = {
#include "stg22/terrain/collision_314.inc"
};
static NJS_CNK_OBJECT draw_974 = {
#include "stg22/terrain/draw_974.inc"
};
static NJS_CNK_OBJECT draw_970 = {
#include "stg22/terrain/draw_970.inc"
};
static NJS_CNK_OBJECT draw_969 = {
#include "stg22/terrain/draw_969.inc"
};
static NJS_CNK_OBJECT draw_966 = {
#include "stg22/terrain/draw_966.inc"
};
static NJS_CNK_OBJECT draw_965 = {
#include "stg22/terrain/draw_965.inc"
};
static NJS_CNK_OBJECT draw_964 = {
#include "stg22/terrain/draw_964.inc"
};
static NJS_CNK_OBJECT draw_962 = {
#include "stg22/terrain/draw_962.inc"
};
static NJS_CNK_OBJECT draw_960 = {
#include "stg22/terrain/draw_960.inc"
};
static NJS_CNK_OBJECT draw_959 = {
#include "stg22/terrain/draw_959.inc"
};
static NJS_CNK_OBJECT draw_958 = {
#include "stg22/terrain/draw_958.inc"
};
static NJS_CNK_OBJECT draw_952 = {
#include "stg22/terrain/draw_952.inc"
};
static NJS_CNK_OBJECT draw_948 = {
#include "stg22/terrain/draw_948.inc"
};
static NJS_CNK_OBJECT draw_947 = {
#include "stg22/terrain/draw_947.inc"
};
static NJS_CNK_OBJECT draw_946 = {
#include "stg22/terrain/draw_946.inc"
};
static NJS_CNK_OBJECT draw_944 = {
#include "stg22/terrain/draw_944.inc"
};
static NJS_CNK_OBJECT draw_942 = {
#include "stg22/terrain/draw_942.inc"
};
static NJS_CNK_OBJECT draw_941 = {
#include "stg22/terrain/draw_941.inc"
};
static NJS_CNK_OBJECT draw_939 = {
#include "stg22/terrain/draw_939.inc"
};
static NJS_CNK_OBJECT draw_938 = {
#include "stg22/terrain/draw_938.inc"
};
static NJS_CNK_OBJECT draw_936 = {
#include "stg22/terrain/draw_936.inc"
};
static NJS_CNK_OBJECT draw_935 = {
#include "stg22/terrain/draw_935.inc"
};
static NJS_CNK_OBJECT draw_934 = {
#include "stg22/terrain/draw_934.inc"
};
static NJS_CNK_OBJECT draw_932 = {
#include "stg22/terrain/draw_932.inc"
};
static NJS_CNK_OBJECT draw_931 = {
#include "stg22/terrain/draw_931.inc"
};
static NJS_CNK_OBJECT draw_930 = {
#include "stg22/terrain/draw_930.inc"
};
static NJS_CNK_OBJECT draw_929 = {
#include "stg22/terrain/draw_929.inc"
};
static NJS_CNK_OBJECT draw_928 = {
#include "stg22/terrain/draw_928.inc"
};
static NJS_CNK_OBJECT draw_927 = {
#include "stg22/terrain/draw_927.inc"
};
static NJS_CNK_OBJECT draw_925 = {
#include "stg22/terrain/draw_925.inc"
};
static NJS_CNK_OBJECT draw_923 = {
#include "stg22/terrain/draw_923.inc"
};
static NJS_CNK_OBJECT draw_922 = {
#include "stg22/terrain/draw_922.inc"
};
static NJS_CNK_OBJECT draw_921 = {
#include "stg22/terrain/draw_921.inc"
};
static NJS_CNK_OBJECT draw_920 = {
#include "stg22/terrain/draw_920.inc"
};
static NJS_CNK_OBJECT draw_918 = {
#include "stg22/terrain/draw_918.inc"
};
static NJS_CNK_OBJECT draw_917 = {
#include "stg22/terrain/draw_917.inc"
};
static NJS_CNK_OBJECT draw_916 = {
#include "stg22/terrain/draw_916.inc"
};
static NJS_CNK_OBJECT draw_915 = {
#include "stg22/terrain/draw_915.inc"
};
static NJS_CNK_OBJECT draw_914 = {
#include "stg22/terrain/draw_914.inc"
};
static NJS_CNK_OBJECT draw_913 = {
#include "stg22/terrain/draw_913.inc"
};
static NJS_CNK_OBJECT draw_912 = {
#include "stg22/terrain/draw_912.inc"
};
static NJS_CNK_OBJECT draw_911 = {
#include "stg22/terrain/draw_911.inc"
};
static NJS_CNK_OBJECT draw_910 = {
#include "stg22/terrain/draw_910.inc"
};
static NJS_CNK_OBJECT draw_909 = {
#include "stg22/terrain/draw_909.inc"
};
static NJS_CNK_OBJECT draw_908 = {
#include "stg22/terrain/draw_908.inc"
};
static NJS_CNK_OBJECT draw_907 = {
#include "stg22/terrain/draw_907.inc"
};
static NJS_CNK_OBJECT draw_906 = {
#include "stg22/terrain/draw_906.inc"
};
static NJS_CNK_OBJECT draw_905 = {
#include "stg22/terrain/draw_905.inc"
};
static NJS_CNK_OBJECT draw_904 = {
#include "stg22/terrain/draw_904.inc"
};
static NJS_CNK_OBJECT draw_903 = {
#include "stg22/terrain/draw_903.inc"
};
static NJS_CNK_OBJECT draw_901 = {
#include "stg22/terrain/draw_901.inc"
};
static NJS_CNK_OBJECT draw_900 = {
#include "stg22/terrain/draw_900.inc"
};
static NJS_CNK_OBJECT draw_899 = {
#include "stg22/terrain/draw_899.inc"
};
static NJS_CNK_OBJECT draw_898 = {
#include "stg22/terrain/draw_898.inc"
};
static NJS_CNK_OBJECT draw_897 = {
#include "stg22/terrain/draw_897.inc"
};
static NJS_CNK_OBJECT draw_896 = {
#include "stg22/terrain/draw_896.inc"
};
static NJS_CNK_OBJECT draw_895 = {
#include "stg22/terrain/draw_895.inc"
};
static NJS_CNK_OBJECT draw_894 = {
#include "stg22/terrain/draw_894.inc"
};
static NJS_CNK_OBJECT draw_890 = {
#include "stg22/terrain/draw_890.inc"
};
static NJS_CNK_OBJECT draw_887 = {
#include "stg22/terrain/draw_887.inc"
};
static NJS_CNK_OBJECT draw_886 = {
#include "stg22/terrain/draw_886.inc"
};
static NJS_CNK_OBJECT draw_885 = {
#include "stg22/terrain/draw_885.inc"
};
static NJS_CNK_OBJECT draw_884 = {
#include "stg22/terrain/draw_884.inc"
};
static NJS_CNK_OBJECT draw_883 = {
#include "stg22/terrain/draw_883.inc"
};
static NJS_CNK_OBJECT draw_882 = {
#include "stg22/terrain/draw_882.inc"
};
static NJS_CNK_OBJECT draw_881 = {
#include "stg22/terrain/draw_881.inc"
};
static NJS_CNK_OBJECT draw_880 = {
#include "stg22/terrain/draw_880.inc"
};
static NJS_CNK_OBJECT draw_879 = {
#include "stg22/terrain/draw_879.inc"
};
static NJS_CNK_OBJECT draw_878 = {
#include "stg22/terrain/draw_878.inc"
};
static NJS_CNK_OBJECT draw_877 = {
#include "stg22/terrain/draw_877.inc"
};
static NJS_CNK_OBJECT draw_876 = {
#include "stg22/terrain/draw_876.inc"
};
static NJS_CNK_OBJECT draw_875 = {
#include "stg22/terrain/draw_875.inc"
};
static NJS_CNK_OBJECT draw_874 = {
#include "stg22/terrain/draw_874.inc"
};
static NJS_CNK_OBJECT draw_873 = {
#include "stg22/terrain/draw_873.inc"
};
static NJS_CNK_OBJECT draw_872 = {
#include "stg22/terrain/draw_872.inc"
};
static NJS_CNK_OBJECT draw_871 = {
#include "stg22/terrain/draw_871.inc"
};
static NJS_CNK_OBJECT draw_870 = {
#include "stg22/terrain/draw_870.inc"
};
static NJS_CNK_OBJECT draw_869 = {
#include "stg22/terrain/draw_869.inc"
};
static NJS_CNK_OBJECT draw_868 = {
#include "stg22/terrain/draw_868.inc"
};
static NJS_CNK_OBJECT draw_867 = {
#include "stg22/terrain/draw_867.inc"
};
static NJS_CNK_OBJECT draw_866 = {
#include "stg22/terrain/draw_866.inc"
};
static NJS_CNK_OBJECT draw_865 = {
#include "stg22/terrain/draw_865.inc"
};
static NJS_CNK_OBJECT draw_864 = {
#include "stg22/terrain/draw_864.inc"
};
static NJS_CNK_OBJECT draw_863 = {
#include "stg22/terrain/draw_863.inc"
};
static NJS_CNK_OBJECT draw_862 = {
#include "stg22/terrain/draw_862.inc"
};
static NJS_CNK_OBJECT draw_861 = {
#include "stg22/terrain/draw_861.inc"
};
static NJS_CNK_OBJECT draw_860 = {
#include "stg22/terrain/draw_860.inc"
};
static NJS_CNK_OBJECT draw_859 = {
#include "stg22/terrain/draw_859.inc"
};
static NJS_CNK_OBJECT draw_858 = {
#include "stg22/terrain/draw_858.inc"
};
static NJS_CNK_OBJECT draw_857 = {
#include "stg22/terrain/draw_857.inc"
};
static NJS_CNK_OBJECT draw_856 = {
#include "stg22/terrain/draw_856.inc"
};
static NJS_CNK_OBJECT draw_855 = {
#include "stg22/terrain/draw_855.inc"
};
static NJS_CNK_OBJECT draw_853 = {
#include "stg22/terrain/draw_853.inc"
};
static NJS_CNK_OBJECT draw_852 = {
#include "stg22/terrain/draw_852.inc"
};
static NJS_CNK_OBJECT draw_851 = {
#include "stg22/terrain/draw_851.inc"
};
static NJS_CNK_OBJECT draw_850 = {
#include "stg22/terrain/draw_850.inc"
};
static NJS_CNK_OBJECT draw_849 = {
#include "stg22/terrain/draw_849.inc"
};
static NJS_CNK_OBJECT draw_848 = {
#include "stg22/terrain/draw_848.inc"
};
static NJS_CNK_OBJECT draw_847 = {
#include "stg22/terrain/draw_847.inc"
};
static NJS_CNK_OBJECT draw_846 = {
#include "stg22/terrain/draw_846.inc"
};
static NJS_CNK_OBJECT draw_845 = {
#include "stg22/terrain/draw_845.inc"
};
static NJS_CNK_OBJECT draw_844 = {
#include "stg22/terrain/draw_844.inc"
};
static NJS_CNK_OBJECT draw_843 = {
#include "stg22/terrain/draw_843.inc"
};
static NJS_CNK_OBJECT draw_842 = {
#include "stg22/terrain/draw_842.inc"
};
static NJS_CNK_OBJECT draw_840 = {
#include "stg22/terrain/draw_840.inc"
};
static NJS_CNK_OBJECT draw_839 = {
#include "stg22/terrain/draw_839.inc"
};
static NJS_CNK_OBJECT draw_838 = {
#include "stg22/terrain/draw_838.inc"
};
static NJS_CNK_OBJECT draw_837 = {
#include "stg22/terrain/draw_837.inc"
};
static NJS_CNK_OBJECT draw_836 = {
#include "stg22/terrain/draw_836.inc"
};
static NJS_CNK_OBJECT draw_835 = {
#include "stg22/terrain/draw_835.inc"
};
static NJS_CNK_OBJECT draw_834 = {
#include "stg22/terrain/draw_834.inc"
};
static NJS_CNK_OBJECT draw_833 = {
#include "stg22/terrain/draw_833.inc"
};
static NJS_CNK_OBJECT draw_832 = {
#include "stg22/terrain/draw_832.inc"
};
static NJS_CNK_OBJECT draw_831 = {
#include "stg22/terrain/draw_831.inc"
};
static NJS_CNK_OBJECT draw_830 = {
#include "stg22/terrain/draw_830.inc"
};
static NJS_CNK_OBJECT draw_829 = {
#include "stg22/terrain/draw_829.inc"
};
static NJS_CNK_OBJECT draw_828 = {
#include "stg22/terrain/draw_828.inc"
};
static NJS_CNK_OBJECT draw_827 = {
#include "stg22/terrain/draw_827.inc"
};
static NJS_CNK_OBJECT draw_826 = {
#include "stg22/terrain/draw_826.inc"
};
static NJS_CNK_OBJECT draw_825 = {
#include "stg22/terrain/draw_825.inc"
};
static NJS_CNK_OBJECT draw_824 = {
#include "stg22/terrain/draw_824.inc"
};
static NJS_CNK_OBJECT draw_823 = {
#include "stg22/terrain/draw_823.inc"
};
static NJS_CNK_OBJECT draw_822 = {
#include "stg22/terrain/draw_822.inc"
};
static NJS_CNK_OBJECT draw_821 = {
#include "stg22/terrain/draw_821.inc"
};
static NJS_CNK_OBJECT draw_820 = {
#include "stg22/terrain/draw_820.inc"
};
static NJS_CNK_OBJECT draw_819 = {
#include "stg22/terrain/draw_819.inc"
};
static NJS_CNK_OBJECT draw_818 = {
#include "stg22/terrain/draw_818.inc"
};
static NJS_CNK_OBJECT draw_816 = {
#include "stg22/terrain/draw_816.inc"
};
static NJS_CNK_OBJECT draw_814 = {
#include "stg22/terrain/draw_814.inc"
};
static NJS_CNK_OBJECT draw_813 = {
#include "stg22/terrain/draw_813.inc"
};
static NJS_CNK_OBJECT draw_811 = {
#include "stg22/terrain/draw_811.inc"
};
static NJS_CNK_OBJECT draw_809 = {
#include "stg22/terrain/draw_809.inc"
};
static NJS_CNK_OBJECT draw_808 = {
#include "stg22/terrain/draw_808.inc"
};
static NJS_CNK_OBJECT draw_807 = {
#include "stg22/terrain/draw_807.inc"
};
static NJS_CNK_OBJECT draw_806 = {
#include "stg22/terrain/draw_806.inc"
};
static NJS_CNK_OBJECT draw_804 = {
#include "stg22/terrain/draw_804.inc"
};
static NJS_CNK_OBJECT draw_801 = {
#include "stg22/terrain/draw_801.inc"
};
static NJS_CNK_OBJECT draw_800 = {
#include "stg22/terrain/draw_800.inc"
};
static NJS_CNK_OBJECT draw_799 = {
#include "stg22/terrain/draw_799.inc"
};
static NJS_CNK_OBJECT draw_798 = {
#include "stg22/terrain/draw_798.inc"
};
static NJS_CNK_OBJECT draw_797 = {
#include "stg22/terrain/draw_797.inc"
};
static NJS_CNK_OBJECT draw_788 = {
#include "stg22/terrain/draw_788.inc"
};
static NJS_CNK_OBJECT draw_787 = {
#include "stg22/terrain/draw_787.inc"
};
static NJS_CNK_OBJECT draw_786 = {
#include "stg22/terrain/draw_786.inc"
};
static NJS_CNK_OBJECT draw_785 = {
#include "stg22/terrain/draw_785.inc"
};
static NJS_CNK_OBJECT draw_784 = {
#include "stg22/terrain/draw_784.inc"
};
static NJS_CNK_OBJECT draw_783 = {
#include "stg22/terrain/draw_783.inc"
};
static NJS_CNK_OBJECT draw_782 = {
#include "stg22/terrain/draw_782.inc"
};
static NJS_CNK_OBJECT draw_781 = {
#include "stg22/terrain/draw_781.inc"
};
static NJS_CNK_OBJECT draw_780 = {
#include "stg22/terrain/draw_780.inc"
};
static NJS_CNK_OBJECT draw_777 = {
#include "stg22/terrain/draw_777.inc"
};
static NJS_CNK_OBJECT draw_775 = {
#include "stg22/terrain/draw_775.inc"
};
static NJS_CNK_OBJECT draw_774 = {
#include "stg22/terrain/draw_774.inc"
};
static NJS_CNK_OBJECT draw_771 = {
#include "stg22/terrain/draw_771.inc"
};
static NJS_CNK_OBJECT draw_769 = {
#include "stg22/terrain/draw_769.inc"
};
static NJS_CNK_OBJECT draw_768 = {
#include "stg22/terrain/draw_768.inc"
};
static NJS_CNK_OBJECT draw_767 = {
#include "stg22/terrain/draw_767.inc"
};
static NJS_CNK_OBJECT draw_764 = {
#include "stg22/terrain/draw_764.inc"
};
static NJS_CNK_OBJECT draw_763 = {
#include "stg22/terrain/draw_763.inc"
};
static NJS_CNK_OBJECT draw_762 = {
#include "stg22/terrain/draw_762.inc"
};
static NJS_CNK_OBJECT draw_761 = {
#include "stg22/terrain/draw_761.inc"
};
static NJS_CNK_OBJECT draw_759 = {
#include "stg22/terrain/draw_759.inc"
};
static NJS_CNK_OBJECT draw_758 = {
#include "stg22/terrain/draw_758.inc"
};
static NJS_CNK_OBJECT draw_757 = {
#include "stg22/terrain/draw_757.inc"
};
static NJS_CNK_OBJECT draw_753 = {
#include "stg22/terrain/draw_753.inc"
};
static NJS_CNK_OBJECT draw_752 = {
#include "stg22/terrain/draw_752.inc"
};
static NJS_CNK_OBJECT draw_751 = {
#include "stg22/terrain/draw_751.inc"
};
static NJS_CNK_OBJECT draw_750 = {
#include "stg22/terrain/draw_750.inc"
};
static NJS_CNK_OBJECT draw_749 = {
#include "stg22/terrain/draw_749.inc"
};
static NJS_CNK_OBJECT draw_748 = {
#include "stg22/terrain/draw_748.inc"
};
static NJS_CNK_OBJECT draw_747 = {
#include "stg22/terrain/draw_747.inc"
};
static NJS_CNK_OBJECT draw_746 = {
#include "stg22/terrain/draw_746.inc"
};
static NJS_CNK_OBJECT draw_745 = {
#include "stg22/terrain/draw_745.inc"
};
static NJS_CNK_OBJECT draw_744 = {
#include "stg22/terrain/draw_744.inc"
};
static NJS_CNK_OBJECT draw_743 = {
#include "stg22/terrain/draw_743.inc"
};
static NJS_CNK_OBJECT draw_742 = {
#include "stg22/terrain/draw_742.inc"
};
static NJS_CNK_OBJECT draw_741 = {
#include "stg22/terrain/draw_741.inc"
};
static NJS_CNK_OBJECT draw_740 = {
#include "stg22/terrain/draw_740.inc"
};
static NJS_CNK_OBJECT draw_739 = {
#include "stg22/terrain/draw_739.inc"
};
static NJS_CNK_OBJECT draw_738 = {
#include "stg22/terrain/draw_738.inc"
};
static NJS_CNK_OBJECT draw_737 = {
#include "stg22/terrain/draw_737.inc"
};
static NJS_CNK_OBJECT draw_736 = {
#include "stg22/terrain/draw_736.inc"
};
static NJS_CNK_OBJECT draw_735 = {
#include "stg22/terrain/draw_735.inc"
};
static NJS_CNK_OBJECT draw_734 = {
#include "stg22/terrain/draw_734.inc"
};
static NJS_CNK_OBJECT draw_733 = {
#include "stg22/terrain/draw_733.inc"
};
static NJS_CNK_OBJECT draw_732 = {
#include "stg22/terrain/draw_732.inc"
};
static NJS_CNK_OBJECT draw_731 = {
#include "stg22/terrain/draw_731.inc"
};
static NJS_CNK_OBJECT draw_730 = {
#include "stg22/terrain/draw_730.inc"
};
static NJS_CNK_OBJECT draw_729 = {
#include "stg22/terrain/draw_729.inc"
};
static NJS_CNK_OBJECT draw_728 = {
#include "stg22/terrain/draw_728.inc"
};
static NJS_CNK_OBJECT draw_727 = {
#include "stg22/terrain/draw_727.inc"
};
static NJS_CNK_OBJECT draw_726 = {
#include "stg22/terrain/draw_726.inc"
};
static NJS_CNK_OBJECT draw_725 = {
#include "stg22/terrain/draw_725.inc"
};
static NJS_CNK_OBJECT draw_724 = {
#include "stg22/terrain/draw_724.inc"
};
static NJS_CNK_OBJECT draw_723 = {
#include "stg22/terrain/draw_723.inc"
};
static NJS_CNK_OBJECT draw_722 = {
#include "stg22/terrain/draw_722.inc"
};
static NJS_CNK_OBJECT draw_721 = {
#include "stg22/terrain/draw_721.inc"
};
static NJS_CNK_OBJECT draw_720 = {
#include "stg22/terrain/draw_720.inc"
};
static NJS_CNK_OBJECT draw_719 = {
#include "stg22/terrain/draw_719.inc"
};
static NJS_CNK_OBJECT draw_718 = {
#include "stg22/terrain/draw_718.inc"
};
static NJS_CNK_OBJECT draw_717 = {
#include "stg22/terrain/draw_717.inc"
};
static NJS_CNK_OBJECT draw_716 = {
#include "stg22/terrain/draw_716.inc"
};
static NJS_CNK_OBJECT draw_715 = {
#include "stg22/terrain/draw_715.inc"
};
static NJS_CNK_OBJECT draw_714 = {
#include "stg22/terrain/draw_714.inc"
};
static NJS_CNK_OBJECT draw_713 = {
#include "stg22/terrain/draw_713.inc"
};
static NJS_CNK_OBJECT draw_712 = {
#include "stg22/terrain/draw_712.inc"
};
static NJS_CNK_OBJECT draw_711 = {
#include "stg22/terrain/draw_711.inc"
};
static NJS_CNK_OBJECT draw_710 = {
#include "stg22/terrain/draw_710.inc"
};
static NJS_CNK_OBJECT draw_709 = {
#include "stg22/terrain/draw_709.inc"
};
static NJS_CNK_OBJECT draw_708 = {
#include "stg22/terrain/draw_708.inc"
};
static NJS_CNK_OBJECT draw_707 = {
#include "stg22/terrain/draw_707.inc"
};
static NJS_CNK_OBJECT draw_706 = {
#include "stg22/terrain/draw_706.inc"
};
static NJS_CNK_OBJECT draw_705 = {
#include "stg22/terrain/draw_705.inc"
};
static NJS_CNK_OBJECT draw_704 = {
#include "stg22/terrain/draw_704.inc"
};
static NJS_CNK_OBJECT draw_703 = {
#include "stg22/terrain/draw_703.inc"
};
static NJS_CNK_OBJECT draw_702 = {
#include "stg22/terrain/draw_702.inc"
};
static NJS_CNK_OBJECT draw_701 = {
#include "stg22/terrain/draw_701.inc"
};
static NJS_CNK_OBJECT draw_700 = {
#include "stg22/terrain/draw_700.inc"
};
static NJS_CNK_OBJECT draw_699 = {
#include "stg22/terrain/draw_699.inc"
};
static NJS_CNK_OBJECT draw_698 = {
#include "stg22/terrain/draw_698.inc"
};
static NJS_CNK_OBJECT draw_697 = {
#include "stg22/terrain/draw_697.inc"
};
static NJS_CNK_OBJECT draw_696 = {
#include "stg22/terrain/draw_696.inc"
};
static NJS_CNK_OBJECT draw_695 = {
#include "stg22/terrain/draw_695.inc"
};
static NJS_CNK_OBJECT draw_693 = {
#include "stg22/terrain/draw_693.inc"
};
static NJS_CNK_OBJECT draw_692 = {
#include "stg22/terrain/draw_692.inc"
};
static NJS_CNK_OBJECT draw_691 = {
#include "stg22/terrain/draw_691.inc"
};
static NJS_CNK_OBJECT draw_690 = {
#include "stg22/terrain/draw_690.inc"
};
static NJS_CNK_OBJECT draw_689 = {
#include "stg22/terrain/draw_689.inc"
};
static NJS_CNK_OBJECT draw_688 = {
#include "stg22/terrain/draw_688.inc"
};
static NJS_CNK_OBJECT draw_687 = {
#include "stg22/terrain/draw_687.inc"
};
static NJS_CNK_OBJECT draw_686 = {
#include "stg22/terrain/draw_686.inc"
};
static NJS_CNK_OBJECT draw_685 = {
#include "stg22/terrain/draw_685.inc"
};
static NJS_CNK_OBJECT draw_684 = {
#include "stg22/terrain/draw_684.inc"
};
static NJS_CNK_OBJECT draw_683 = {
#include "stg22/terrain/draw_683.inc"
};
static NJS_CNK_OBJECT draw_682 = {
#include "stg22/terrain/draw_682.inc"
};
static NJS_CNK_OBJECT draw_681 = {
#include "stg22/terrain/draw_681.inc"
};
static NJS_CNK_OBJECT draw_680 = {
#include "stg22/terrain/draw_680.inc"
};
static NJS_CNK_OBJECT draw_679 = {
#include "stg22/terrain/draw_679.inc"
};
static NJS_CNK_OBJECT draw_678 = {
#include "stg22/terrain/draw_678.inc"
};
static NJS_CNK_OBJECT draw_677 = {
#include "stg22/terrain/draw_677.inc"
};
static NJS_CNK_OBJECT draw_676 = {
#include "stg22/terrain/draw_676.inc"
};
static NJS_CNK_OBJECT draw_675 = {
#include "stg22/terrain/draw_675.inc"
};
static NJS_CNK_OBJECT draw_674 = {
#include "stg22/terrain/draw_674.inc"
};
static NJS_CNK_OBJECT draw_673 = {
#include "stg22/terrain/draw_673.inc"
};
static NJS_CNK_OBJECT draw_672 = {
#include "stg22/terrain/draw_672.inc"
};
static NJS_CNK_OBJECT draw_671 = {
#include "stg22/terrain/draw_671.inc"
};
static NJS_CNK_OBJECT draw_670 = {
#include "stg22/terrain/draw_670.inc"
};
static NJS_CNK_OBJECT draw_669 = {
#include "stg22/terrain/draw_669.inc"
};
static NJS_CNK_OBJECT draw_668 = {
#include "stg22/terrain/draw_668.inc"
};
static NJS_CNK_OBJECT draw_667 = {
#include "stg22/terrain/draw_667.inc"
};
static NJS_CNK_OBJECT draw_666 = {
#include "stg22/terrain/draw_666.inc"
};
static NJS_CNK_OBJECT draw_665 = {
#include "stg22/terrain/draw_665.inc"
};
static NJS_CNK_OBJECT draw_664 = {
#include "stg22/terrain/draw_664.inc"
};
static NJS_CNK_OBJECT draw_663 = {
#include "stg22/terrain/draw_663.inc"
};
static NJS_CNK_OBJECT draw_662 = {
#include "stg22/terrain/draw_662.inc"
};
static NJS_CNK_OBJECT draw_661 = {
#include "stg22/terrain/draw_661.inc"
};
static NJS_CNK_OBJECT draw_660 = {
#include "stg22/terrain/draw_660.inc"
};
static NJS_CNK_OBJECT draw_659 = {
#include "stg22/terrain/draw_659.inc"
};
static NJS_CNK_OBJECT draw_658 = {
#include "stg22/terrain/draw_658.inc"
};
static NJS_CNK_OBJECT draw_657 = {
#include "stg22/terrain/draw_657.inc"
};
static NJS_CNK_OBJECT draw_656 = {
#include "stg22/terrain/draw_656.inc"
};
static NJS_CNK_OBJECT draw_655 = {
#include "stg22/terrain/draw_655.inc"
};
static NJS_CNK_OBJECT draw_654 = {
#include "stg22/terrain/draw_654.inc"
};
static NJS_CNK_OBJECT draw_653 = {
#include "stg22/terrain/draw_653.inc"
};
static NJS_CNK_OBJECT draw_652 = {
#include "stg22/terrain/draw_652.inc"
};
static NJS_CNK_OBJECT draw_651 = {
#include "stg22/terrain/draw_651.inc"
};
static NJS_CNK_OBJECT draw_650 = {
#include "stg22/terrain/draw_650.inc"
};
static NJS_CNK_OBJECT draw_649 = {
#include "stg22/terrain/draw_649.inc"
};
static NJS_CNK_OBJECT draw_648 = {
#include "stg22/terrain/draw_648.inc"
};
static NJS_CNK_OBJECT draw_647 = {
#include "stg22/terrain/draw_647.inc"
};
static NJS_CNK_OBJECT draw_646 = {
#include "stg22/terrain/draw_646.inc"
};
static NJS_CNK_OBJECT draw_645 = {
#include "stg22/terrain/draw_645.inc"
};
static NJS_CNK_OBJECT draw_644 = {
#include "stg22/terrain/draw_644.inc"
};
static NJS_CNK_OBJECT draw_643 = {
#include "stg22/terrain/draw_643.inc"
};
static NJS_CNK_OBJECT draw_642 = {
#include "stg22/terrain/draw_642.inc"
};
static NJS_CNK_OBJECT draw_639 = {
#include "stg22/terrain/draw_639.inc"
};
static NJS_CNK_OBJECT draw_637 = {
#include "stg22/terrain/draw_637.inc"
};
static NJS_CNK_OBJECT draw_631 = {
#include "stg22/terrain/draw_631.inc"
};
static NJS_CNK_OBJECT draw_630 = {
#include "stg22/terrain/draw_630.inc"
};
static NJS_CNK_OBJECT draw_629 = {
#include "stg22/terrain/draw_629.inc"
};
static NJS_CNK_OBJECT draw_627 = {
#include "stg22/terrain/draw_627.inc"
};
static NJS_CNK_OBJECT draw_626 = {
#include "stg22/terrain/draw_626.inc"
};
static NJS_CNK_OBJECT draw_625 = {
#include "stg22/terrain/draw_625.inc"
};
static NJS_CNK_OBJECT draw_624 = {
#include "stg22/terrain/draw_624.inc"
};
static NJS_CNK_OBJECT draw_623 = {
#include "stg22/terrain/draw_623.inc"
};
static NJS_CNK_OBJECT draw_622 = {
#include "stg22/terrain/draw_622.inc"
};
static NJS_CNK_OBJECT draw_621 = {
#include "stg22/terrain/draw_621.inc"
};
static NJS_CNK_OBJECT draw_620 = {
#include "stg22/terrain/draw_620.inc"
};
static NJS_CNK_OBJECT draw_619 = {
#include "stg22/terrain/draw_619.inc"
};
static NJS_CNK_OBJECT draw_618 = {
#include "stg22/terrain/draw_618.inc"
};
static NJS_CNK_OBJECT draw_617 = {
#include "stg22/terrain/draw_617.inc"
};
static NJS_CNK_OBJECT draw_616 = {
#include "stg22/terrain/draw_616.inc"
};
static NJS_CNK_OBJECT draw_615 = {
#include "stg22/terrain/draw_615.inc"
};
static NJS_CNK_OBJECT draw_614 = {
#include "stg22/terrain/draw_614.inc"
};
static NJS_CNK_OBJECT draw_613 = {
#include "stg22/terrain/draw_613.inc"
};
static NJS_CNK_OBJECT draw_612 = {
#include "stg22/terrain/draw_612.inc"
};
static NJS_CNK_OBJECT draw_611 = {
#include "stg22/terrain/draw_611.inc"
};
static NJS_CNK_OBJECT draw_610 = {
#include "stg22/terrain/draw_610.inc"
};
static NJS_CNK_OBJECT draw_609 = {
#include "stg22/terrain/draw_609.inc"
};
static NJS_CNK_OBJECT draw_607 = {
#include "stg22/terrain/draw_607.inc"
};
static NJS_CNK_OBJECT draw_606 = {
#include "stg22/terrain/draw_606.inc"
};
static NJS_CNK_OBJECT draw_605 = {
#include "stg22/terrain/draw_605.inc"
};
static NJS_CNK_OBJECT draw_604 = {
#include "stg22/terrain/draw_604.inc"
};
static NJS_CNK_OBJECT draw_599 = {
#include "stg22/terrain/draw_599.inc"
};
static NJS_CNK_OBJECT draw_598 = {
#include "stg22/terrain/draw_598.inc"
};
static NJS_CNK_OBJECT draw_597 = {
#include "stg22/terrain/draw_597.inc"
};
static NJS_CNK_OBJECT draw_596 = {
#include "stg22/terrain/draw_596.inc"
};
static NJS_CNK_OBJECT draw_595 = {
#include "stg22/terrain/draw_595.inc"
};
static NJS_CNK_OBJECT draw_594 = {
#include "stg22/terrain/draw_594.inc"
};
static NJS_CNK_OBJECT draw_593 = {
#include "stg22/terrain/draw_593.inc"
};
static NJS_CNK_OBJECT draw_592 = {
#include "stg22/terrain/draw_592.inc"
};
static NJS_CNK_OBJECT draw_591 = {
#include "stg22/terrain/draw_591.inc"
};
static NJS_CNK_OBJECT draw_590 = {
#include "stg22/terrain/draw_590.inc"
};
static NJS_CNK_OBJECT draw_589 = {
#include "stg22/terrain/draw_589.inc"
};
static NJS_CNK_OBJECT draw_588 = {
#include "stg22/terrain/draw_588.inc"
};
static NJS_CNK_OBJECT draw_587 = {
#include "stg22/terrain/draw_587.inc"
};
static NJS_CNK_OBJECT draw_586 = {
#include "stg22/terrain/draw_586.inc"
};
static NJS_CNK_OBJECT draw_585 = {
#include "stg22/terrain/draw_585.inc"
};
static NJS_CNK_OBJECT draw_584 = {
#include "stg22/terrain/draw_584.inc"
};
static NJS_CNK_OBJECT draw_583 = {
#include "stg22/terrain/draw_583.inc"
};
static NJS_CNK_OBJECT draw_582 = {
#include "stg22/terrain/draw_582.inc"
};
static NJS_CNK_OBJECT draw_581 = {
#include "stg22/terrain/draw_581.inc"
};
static NJS_CNK_OBJECT draw_580 = {
#include "stg22/terrain/draw_580.inc"
};
static NJS_CNK_OBJECT draw_579 = {
#include "stg22/terrain/draw_579.inc"
};
static NJS_CNK_OBJECT draw_578 = {
#include "stg22/terrain/draw_578.inc"
};
static NJS_CNK_OBJECT draw_577 = {
#include "stg22/terrain/draw_577.inc"
};
static NJS_CNK_OBJECT draw_576 = {
#include "stg22/terrain/draw_576.inc"
};
static NJS_CNK_OBJECT draw_575 = {
#include "stg22/terrain/draw_575.inc"
};
static NJS_CNK_OBJECT draw_574 = {
#include "stg22/terrain/draw_574.inc"
};
static NJS_CNK_OBJECT draw_573 = {
#include "stg22/terrain/draw_573.inc"
};
static NJS_CNK_OBJECT draw_572 = {
#include "stg22/terrain/draw_572.inc"
};
static NJS_CNK_OBJECT draw_571 = {
#include "stg22/terrain/draw_571.inc"
};
static NJS_CNK_OBJECT draw_570 = {
#include "stg22/terrain/draw_570.inc"
};
static NJS_CNK_OBJECT draw_569 = {
#include "stg22/terrain/draw_569.inc"
};
static NJS_CNK_OBJECT draw_568 = {
#include "stg22/terrain/draw_568.inc"
};
static NJS_CNK_OBJECT draw_567 = {
#include "stg22/terrain/draw_567.inc"
};
static NJS_CNK_OBJECT draw_566 = {
#include "stg22/terrain/draw_566.inc"
};
static NJS_CNK_OBJECT draw_565 = {
#include "stg22/terrain/draw_565.inc"
};
static NJS_CNK_OBJECT draw_564 = {
#include "stg22/terrain/draw_564.inc"
};
static NJS_CNK_OBJECT draw_563 = {
#include "stg22/terrain/draw_563.inc"
};
static NJS_CNK_OBJECT draw_562 = {
#include "stg22/terrain/draw_562.inc"
};
static NJS_CNK_OBJECT draw_561 = {
#include "stg22/terrain/draw_561.inc"
};
static NJS_CNK_OBJECT draw_560 = {
#include "stg22/terrain/draw_560.inc"
};
static NJS_CNK_OBJECT draw_559 = {
#include "stg22/terrain/draw_559.inc"
};
static NJS_CNK_OBJECT draw_558 = {
#include "stg22/terrain/draw_558.inc"
};
static NJS_CNK_OBJECT draw_557 = {
#include "stg22/terrain/draw_557.inc"
};
static NJS_CNK_OBJECT draw_556 = {
#include "stg22/terrain/draw_556.inc"
};
static NJS_CNK_OBJECT draw_555 = {
#include "stg22/terrain/draw_555.inc"
};
static NJS_CNK_OBJECT draw_554 = {
#include "stg22/terrain/draw_554.inc"
};
static NJS_CNK_OBJECT draw_553 = {
#include "stg22/terrain/draw_553.inc"
};
static NJS_CNK_OBJECT draw_552 = {
#include "stg22/terrain/draw_552.inc"
};
static NJS_CNK_OBJECT draw_551 = {
#include "stg22/terrain/draw_551.inc"
};
static NJS_CNK_OBJECT draw_550 = {
#include "stg22/terrain/draw_550.inc"
};
static NJS_CNK_OBJECT draw_549 = {
#include "stg22/terrain/draw_549.inc"
};
static NJS_CNK_OBJECT draw_548 = {
#include "stg22/terrain/draw_548.inc"
};
static NJS_CNK_OBJECT draw_547 = {
#include "stg22/terrain/draw_547.inc"
};
static NJS_CNK_OBJECT draw_545 = {
#include "stg22/terrain/draw_545.inc"
};
static NJS_CNK_OBJECT draw_543 = {
#include "stg22/terrain/draw_543.inc"
};
static NJS_CNK_OBJECT draw_542 = {
#include "stg22/terrain/draw_542.inc"
};
static NJS_CNK_OBJECT draw_541 = {
#include "stg22/terrain/draw_541.inc"
};
static NJS_CNK_OBJECT draw_540 = {
#include "stg22/terrain/draw_540.inc"
};
static NJS_CNK_OBJECT draw_539 = {
#include "stg22/terrain/draw_539.inc"
};
static NJS_CNK_OBJECT draw_538 = {
#include "stg22/terrain/draw_538.inc"
};
static NJS_CNK_OBJECT draw_537 = {
#include "stg22/terrain/draw_537.inc"
};
static NJS_CNK_OBJECT draw_536 = {
#include "stg22/terrain/draw_536.inc"
};
static NJS_CNK_OBJECT draw_535 = {
#include "stg22/terrain/draw_535.inc"
};
static NJS_CNK_OBJECT draw_534 = {
#include "stg22/terrain/draw_534.inc"
};
static NJS_CNK_OBJECT draw_533 = {
#include "stg22/terrain/draw_533.inc"
};
static NJS_CNK_OBJECT draw_532 = {
#include "stg22/terrain/draw_532.inc"
};
static NJS_CNK_OBJECT draw_531 = {
#include "stg22/terrain/draw_531.inc"
};
static NJS_CNK_OBJECT draw_530 = {
#include "stg22/terrain/draw_530.inc"
};
static NJS_CNK_OBJECT draw_529 = {
#include "stg22/terrain/draw_529.inc"
};
static NJS_CNK_OBJECT draw_528 = {
#include "stg22/terrain/draw_528.inc"
};
static NJS_CNK_OBJECT draw_527 = {
#include "stg22/terrain/draw_527.inc"
};
static NJS_CNK_OBJECT draw_526 = {
#include "stg22/terrain/draw_526.inc"
};
static NJS_CNK_OBJECT draw_525 = {
#include "stg22/terrain/draw_525.inc"
};
static NJS_CNK_OBJECT draw_524 = {
#include "stg22/terrain/draw_524.inc"
};
static NJS_CNK_OBJECT draw_523 = {
#include "stg22/terrain/draw_523.inc"
};
static NJS_CNK_OBJECT draw_522 = {
#include "stg22/terrain/draw_522.inc"
};
static NJS_CNK_OBJECT draw_521 = {
#include "stg22/terrain/draw_521.inc"
};
static NJS_CNK_OBJECT draw_520 = {
#include "stg22/terrain/draw_520.inc"
};
static NJS_CNK_OBJECT draw_519 = {
#include "stg22/terrain/draw_519.inc"
};
static NJS_CNK_OBJECT draw_518 = {
#include "stg22/terrain/draw_518.inc"
};
static NJS_CNK_OBJECT draw_517 = {
#include "stg22/terrain/draw_517.inc"
};
static NJS_CNK_OBJECT draw_516 = {
#include "stg22/terrain/draw_516.inc"
};
static NJS_CNK_OBJECT draw_514 = {
#include "stg22/terrain/draw_514.inc"
};
static NJS_CNK_OBJECT draw_513 = {
#include "stg22/terrain/draw_513.inc"
};
static NJS_CNK_OBJECT draw_512 = {
#include "stg22/terrain/draw_512.inc"
};
static NJS_CNK_OBJECT draw_511 = {
#include "stg22/terrain/draw_511.inc"
};
static NJS_CNK_OBJECT draw_510 = {
#include "stg22/terrain/draw_510.inc"
};
static NJS_CNK_OBJECT draw_509 = {
#include "stg22/terrain/draw_509.inc"
};
static NJS_CNK_OBJECT draw_508 = {
#include "stg22/terrain/draw_508.inc"
};
static NJS_CNK_OBJECT draw_507 = {
#include "stg22/terrain/draw_507.inc"
};
static NJS_CNK_OBJECT draw_506 = {
#include "stg22/terrain/draw_506.inc"
};
static NJS_CNK_OBJECT draw_505 = {
#include "stg22/terrain/draw_505.inc"
};
static NJS_CNK_OBJECT draw_504 = {
#include "stg22/terrain/draw_504.inc"
};
static NJS_CNK_OBJECT draw_502 = {
#include "stg22/terrain/draw_502.inc"
};
static NJS_CNK_OBJECT draw_501 = {
#include "stg22/terrain/draw_501.inc"
};
static NJS_CNK_OBJECT draw_500 = {
#include "stg22/terrain/draw_500.inc"
};
static NJS_CNK_OBJECT draw_499 = {
#include "stg22/terrain/draw_499.inc"
};
static NJS_CNK_OBJECT draw_498 = {
#include "stg22/terrain/draw_498.inc"
};
static NJS_CNK_OBJECT draw_497 = {
#include "stg22/terrain/draw_497.inc"
};
static NJS_CNK_OBJECT draw_496 = {
#include "stg22/terrain/draw_496.inc"
};
static NJS_CNK_OBJECT draw_495 = {
#include "stg22/terrain/draw_495.inc"
};
static NJS_CNK_OBJECT draw_494 = {
#include "stg22/terrain/draw_494.inc"
};
static NJS_CNK_OBJECT draw_493 = {
#include "stg22/terrain/draw_493.inc"
};
static NJS_CNK_OBJECT draw_492 = {
#include "stg22/terrain/draw_492.inc"
};
static NJS_CNK_OBJECT draw_491 = {
#include "stg22/terrain/draw_491.inc"
};
static NJS_CNK_OBJECT draw_490 = {
#include "stg22/terrain/draw_490.inc"
};
static NJS_CNK_OBJECT draw_489 = {
#include "stg22/terrain/draw_489.inc"
};
static NJS_CNK_OBJECT draw_488 = {
#include "stg22/terrain/draw_488.inc"
};
static NJS_CNK_OBJECT draw_487 = {
#include "stg22/terrain/draw_487.inc"
};
static NJS_CNK_OBJECT draw_486 = {
#include "stg22/terrain/draw_486.inc"
};
static NJS_CNK_OBJECT draw_485 = {
#include "stg22/terrain/draw_485.inc"
};
static NJS_CNK_OBJECT draw_484 = {
#include "stg22/terrain/draw_484.inc"
};
static NJS_CNK_OBJECT draw_483 = {
#include "stg22/terrain/draw_483.inc"
};
static NJS_CNK_OBJECT draw_482 = {
#include "stg22/terrain/draw_482.inc"
};
static NJS_CNK_OBJECT draw_481 = {
#include "stg22/terrain/draw_481.inc"
};
static NJS_CNK_OBJECT draw_480 = {
#include "stg22/terrain/draw_480.inc"
};
static NJS_CNK_OBJECT draw_479 = {
#include "stg22/terrain/draw_479.inc"
};
static NJS_CNK_OBJECT draw_478 = {
#include "stg22/terrain/draw_478.inc"
};
static NJS_CNK_OBJECT draw_477 = {
#include "stg22/terrain/draw_477.inc"
};
static NJS_CNK_OBJECT draw_476 = {
#include "stg22/terrain/draw_476.inc"
};
static NJS_CNK_OBJECT draw_475 = {
#include "stg22/terrain/draw_475.inc"
};
static NJS_CNK_OBJECT draw_474 = {
#include "stg22/terrain/draw_474.inc"
};
static NJS_CNK_OBJECT draw_473 = {
#include "stg22/terrain/draw_473.inc"
};
static NJS_CNK_OBJECT draw_472 = {
#include "stg22/terrain/draw_472.inc"
};
static NJS_CNK_OBJECT draw_471 = {
#include "stg22/terrain/draw_471.inc"
};
static NJS_CNK_OBJECT draw_470 = {
#include "stg22/terrain/draw_470.inc"
};
static NJS_CNK_OBJECT draw_469 = {
#include "stg22/terrain/draw_469.inc"
};
static NJS_CNK_OBJECT draw_468 = {
#include "stg22/terrain/draw_468.inc"
};
static NJS_CNK_OBJECT draw_467 = {
#include "stg22/terrain/draw_467.inc"
};
static NJS_CNK_OBJECT draw_466 = {
#include "stg22/terrain/draw_466.inc"
};
static NJS_CNK_OBJECT draw_465 = {
#include "stg22/terrain/draw_465.inc"
};
static NJS_CNK_OBJECT draw_464 = {
#include "stg22/terrain/draw_464.inc"
};
static NJS_CNK_OBJECT draw_463 = {
#include "stg22/terrain/draw_463.inc"
};
static NJS_CNK_OBJECT draw_462 = {
#include "stg22/terrain/draw_462.inc"
};
static NJS_CNK_OBJECT draw_461 = {
#include "stg22/terrain/draw_461.inc"
};
static NJS_CNK_OBJECT draw_460 = {
#include "stg22/terrain/draw_460.inc"
};
static NJS_CNK_OBJECT draw_459 = {
#include "stg22/terrain/draw_459.inc"
};
static NJS_CNK_OBJECT draw_458 = {
#include "stg22/terrain/draw_458.inc"
};
static NJS_CNK_OBJECT draw_457 = {
#include "stg22/terrain/draw_457.inc"
};
static NJS_CNK_OBJECT draw_456 = {
#include "stg22/terrain/draw_456.inc"
};
static NJS_CNK_OBJECT draw_455 = {
#include "stg22/terrain/draw_455.inc"
};
static NJS_CNK_OBJECT draw_454 = {
#include "stg22/terrain/draw_454.inc"
};
static NJS_CNK_OBJECT draw_453 = {
#include "stg22/terrain/draw_453.inc"
};
static NJS_CNK_OBJECT draw_452 = {
#include "stg22/terrain/draw_452.inc"
};
static NJS_CNK_OBJECT draw_451 = {
#include "stg22/terrain/draw_451.inc"
};
static NJS_CNK_OBJECT draw_450 = {
#include "stg22/terrain/draw_450.inc"
};
static NJS_CNK_OBJECT draw_449 = {
#include "stg22/terrain/draw_449.inc"
};
static NJS_CNK_OBJECT draw_448 = {
#include "stg22/terrain/draw_448.inc"
};
static NJS_CNK_OBJECT draw_447 = {
#include "stg22/terrain/draw_447.inc"
};
static NJS_CNK_OBJECT draw_446 = {
#include "stg22/terrain/draw_446.inc"
};
static NJS_CNK_OBJECT draw_445 = {
#include "stg22/terrain/draw_445.inc"
};
static NJS_CNK_OBJECT draw_444 = {
#include "stg22/terrain/draw_444.inc"
};
static NJS_CNK_OBJECT draw_443 = {
#include "stg22/terrain/draw_443.inc"
};
static NJS_CNK_OBJECT draw_441 = {
#include "stg22/terrain/draw_441.inc"
};
static NJS_CNK_OBJECT draw_440 = {
#include "stg22/terrain/draw_440.inc"
};
static NJS_CNK_OBJECT draw_439 = {
#include "stg22/terrain/draw_439.inc"
};
static NJS_CNK_OBJECT draw_438 = {
#include "stg22/terrain/draw_438.inc"
};
static NJS_CNK_OBJECT draw_437 = {
#include "stg22/terrain/draw_437.inc"
};
static NJS_CNK_OBJECT draw_436 = {
#include "stg22/terrain/draw_436.inc"
};
static NJS_CNK_OBJECT draw_435 = {
#include "stg22/terrain/draw_435.inc"
};
static NJS_CNK_OBJECT draw_434 = {
#include "stg22/terrain/draw_434.inc"
};
static NJS_CNK_OBJECT draw_433 = {
#include "stg22/terrain/draw_433.inc"
};
static NJS_CNK_OBJECT draw_432 = {
#include "stg22/terrain/draw_432.inc"
};
static NJS_CNK_OBJECT draw_431 = {
#include "stg22/terrain/draw_431.inc"
};
static NJS_CNK_OBJECT draw_430 = {
#include "stg22/terrain/draw_430.inc"
};
static NJS_CNK_OBJECT draw_429 = {
#include "stg22/terrain/draw_429.inc"
};
static NJS_CNK_OBJECT draw_428 = {
#include "stg22/terrain/draw_428.inc"
};
static NJS_CNK_OBJECT draw_427 = {
#include "stg22/terrain/draw_427.inc"
};
static NJS_CNK_OBJECT draw_426 = {
#include "stg22/terrain/draw_426.inc"
};
static NJS_CNK_OBJECT draw_425 = {
#include "stg22/terrain/draw_425.inc"
};
static NJS_CNK_OBJECT draw_424 = {
#include "stg22/terrain/draw_424.inc"
};
static NJS_CNK_OBJECT draw_423 = {
#include "stg22/terrain/draw_423.inc"
};
static NJS_CNK_OBJECT draw_422 = {
#include "stg22/terrain/draw_422.inc"
};
static NJS_CNK_OBJECT draw_421 = {
#include "stg22/terrain/draw_421.inc"
};
static NJS_CNK_OBJECT draw_420 = {
#include "stg22/terrain/draw_420.inc"
};
static NJS_CNK_OBJECT draw_419 = {
#include "stg22/terrain/draw_419.inc"
};
static NJS_CNK_OBJECT draw_418 = {
#include "stg22/terrain/draw_418.inc"
};
static NJS_CNK_OBJECT draw_417 = {
#include "stg22/terrain/draw_417.inc"
};
static NJS_CNK_OBJECT draw_416 = {
#include "stg22/terrain/draw_416.inc"
};
static NJS_CNK_OBJECT draw_415 = {
#include "stg22/terrain/draw_415.inc"
};
static NJS_CNK_OBJECT draw_414 = {
#include "stg22/terrain/draw_414.inc"
};
static NJS_CNK_OBJECT draw_412 = {
#include "stg22/terrain/draw_412.inc"
};
static NJS_CNK_OBJECT draw_411 = {
#include "stg22/terrain/draw_411.inc"
};
static NJS_CNK_OBJECT draw_410 = {
#include "stg22/terrain/draw_410.inc"
};
static NJS_CNK_OBJECT draw_409 = {
#include "stg22/terrain/draw_409.inc"
};
static NJS_CNK_OBJECT draw_408 = {
#include "stg22/terrain/draw_408.inc"
};
static NJS_CNK_OBJECT draw_407 = {
#include "stg22/terrain/draw_407.inc"
};
static NJS_CNK_OBJECT draw_406 = {
#include "stg22/terrain/draw_406.inc"
};
static NJS_CNK_OBJECT draw_405 = {
#include "stg22/terrain/draw_405.inc"
};
static NJS_CNK_OBJECT draw_404 = {
#include "stg22/terrain/draw_404.inc"
};
static NJS_CNK_OBJECT draw_403 = {
#include "stg22/terrain/draw_403.inc"
};
static NJS_CNK_OBJECT draw_402 = {
#include "stg22/terrain/draw_402.inc"
};
static NJS_CNK_OBJECT draw_401 = {
#include "stg22/terrain/draw_401.inc"
};
static NJS_CNK_OBJECT draw_400 = {
#include "stg22/terrain/draw_400.inc"
};
static NJS_CNK_OBJECT draw_399 = {
#include "stg22/terrain/draw_399.inc"
};
static NJS_CNK_OBJECT draw_398 = {
#include "stg22/terrain/draw_398.inc"
};
static NJS_CNK_OBJECT draw_397 = {
#include "stg22/terrain/draw_397.inc"
};
static NJS_CNK_OBJECT draw_396 = {
#include "stg22/terrain/draw_396.inc"
};
static NJS_CNK_OBJECT draw_395 = {
#include "stg22/terrain/draw_395.inc"
};
static NJS_CNK_OBJECT draw_394 = {
#include "stg22/terrain/draw_394.inc"
};
static NJS_CNK_OBJECT draw_393 = {
#include "stg22/terrain/draw_393.inc"
};
static NJS_CNK_OBJECT draw_392 = {
#include "stg22/terrain/draw_392.inc"
};
static NJS_CNK_OBJECT draw_390 = {
#include "stg22/terrain/draw_390.inc"
};
static NJS_CNK_OBJECT draw_389 = {
#include "stg22/terrain/draw_389.inc"
};
static NJS_CNK_OBJECT draw_388 = {
#include "stg22/terrain/draw_388.inc"
};
static NJS_CNK_OBJECT draw_387 = {
#include "stg22/terrain/draw_387.inc"
};
static NJS_CNK_OBJECT draw_386 = {
#include "stg22/terrain/draw_386.inc"
};
static NJS_CNK_OBJECT draw_385 = {
#include "stg22/terrain/draw_385.inc"
};
static NJS_CNK_OBJECT draw_384 = {
#include "stg22/terrain/draw_384.inc"
};
static NJS_CNK_OBJECT draw_383 = {
#include "stg22/terrain/draw_383.inc"
};
static NJS_CNK_OBJECT draw_382 = {
#include "stg22/terrain/draw_382.inc"
};
static NJS_CNK_OBJECT draw_381 = {
#include "stg22/terrain/draw_381.inc"
};
static NJS_CNK_OBJECT draw_380 = {
#include "stg22/terrain/draw_380.inc"
};
static NJS_CNK_OBJECT draw_379 = {
#include "stg22/terrain/draw_379.inc"
};
static NJS_CNK_OBJECT draw_378 = {
#include "stg22/terrain/draw_378.inc"
};
static NJS_CNK_OBJECT draw_377 = {
#include "stg22/terrain/draw_377.inc"
};
static NJS_CNK_OBJECT draw_376 = {
#include "stg22/terrain/draw_376.inc"
};
static NJS_CNK_OBJECT draw_375 = {
#include "stg22/terrain/draw_375.inc"
};
static NJS_CNK_OBJECT draw_374 = {
#include "stg22/terrain/draw_374.inc"
};
static NJS_CNK_OBJECT draw_373 = {
#include "stg22/terrain/draw_373.inc"
};
static NJS_CNK_OBJECT draw_371 = {
#include "stg22/terrain/draw_371.inc"
};
static NJS_CNK_OBJECT draw_370 = {
#include "stg22/terrain/draw_370.inc"
};
static NJS_CNK_OBJECT draw_369 = {
#include "stg22/terrain/draw_369.inc"
};
static NJS_CNK_OBJECT draw_368 = {
#include "stg22/terrain/draw_368.inc"
};
static NJS_CNK_OBJECT draw_367 = {
#include "stg22/terrain/draw_367.inc"
};
static NJS_CNK_OBJECT draw_366 = {
#include "stg22/terrain/draw_366.inc"
};
static NJS_CNK_OBJECT draw_365 = {
#include "stg22/terrain/draw_365.inc"
};
static NJS_CNK_OBJECT draw_364 = {
#include "stg22/terrain/draw_364.inc"
};
static NJS_CNK_OBJECT draw_363 = {
#include "stg22/terrain/draw_363.inc"
};
static NJS_CNK_OBJECT draw_362 = {
#include "stg22/terrain/draw_362.inc"
};
static NJS_CNK_OBJECT draw_361 = {
#include "stg22/terrain/draw_361.inc"
};
static NJS_CNK_OBJECT draw_360 = {
#include "stg22/terrain/draw_360.inc"
};
static NJS_CNK_OBJECT draw_358 = {
#include "stg22/terrain/draw_358.inc"
};
static NJS_CNK_OBJECT draw_357 = {
#include "stg22/terrain/draw_357.inc"
};
static NJS_CNK_OBJECT draw_356 = {
#include "stg22/terrain/draw_356.inc"
};
static NJS_CNK_OBJECT draw_355 = {
#include "stg22/terrain/draw_355.inc"
};
static NJS_CNK_OBJECT draw_354 = {
#include "stg22/terrain/draw_354.inc"
};
static NJS_CNK_OBJECT draw_353 = {
#include "stg22/terrain/draw_353.inc"
};
static NJS_CNK_OBJECT draw_352 = {
#include "stg22/terrain/draw_352.inc"
};
static NJS_CNK_OBJECT draw_351 = {
#include "stg22/terrain/draw_351.inc"
};
static NJS_CNK_OBJECT draw_350 = {
#include "stg22/terrain/draw_350.inc"
};
static NJS_CNK_OBJECT draw_349 = {
#include "stg22/terrain/draw_349.inc"
};
static NJS_CNK_OBJECT draw_348 = {
#include "stg22/terrain/draw_348.inc"
};
static NJS_CNK_OBJECT draw_345 = {
#include "stg22/terrain/draw_345.inc"
};
static NJS_CNK_OBJECT draw_344 = {
#include "stg22/terrain/draw_344.inc"
};
static NJS_CNK_OBJECT draw_343 = {
#include "stg22/terrain/draw_343.inc"
};
static NJS_CNK_OBJECT draw_342 = {
#include "stg22/terrain/draw_342.inc"
};
static NJS_CNK_OBJECT draw_341 = {
#include "stg22/terrain/draw_341.inc"
};
static NJS_CNK_OBJECT draw_340 = {
#include "stg22/terrain/draw_340.inc"
};
static NJS_CNK_OBJECT draw_339 = {
#include "stg22/terrain/draw_339.inc"
};
static NJS_CNK_OBJECT draw_338 = {
#include "stg22/terrain/draw_338.inc"
};
static NJS_CNK_OBJECT draw_337 = {
#include "stg22/terrain/draw_337.inc"
};
static NJS_CNK_OBJECT draw_336 = {
#include "stg22/terrain/draw_336.inc"
};
static NJS_CNK_OBJECT draw_335 = {
#include "stg22/terrain/draw_335.inc"
};
static NJS_CNK_OBJECT draw_334 = {
#include "stg22/terrain/draw_334.inc"
};
static NJS_CNK_OBJECT draw_333 = {
#include "stg22/terrain/draw_333.inc"
};
static NJS_CNK_OBJECT draw_331 = {
#include "stg22/terrain/draw_331.inc"
};
static NJS_CNK_OBJECT draw_330 = {
#include "stg22/terrain/draw_330.inc"
};
static NJS_CNK_OBJECT draw_329 = {
#include "stg22/terrain/draw_329.inc"
};
static NJS_CNK_OBJECT draw_328 = {
#include "stg22/terrain/draw_328.inc"
};
static NJS_CNK_OBJECT draw_327 = {
#include "stg22/terrain/draw_327.inc"
};
static NJS_CNK_OBJECT draw_326 = {
#include "stg22/terrain/draw_326.inc"
};
static NJS_CNK_OBJECT draw_325 = {
#include "stg22/terrain/draw_325.inc"
};
static NJS_CNK_OBJECT draw_324 = {
#include "stg22/terrain/draw_324.inc"
};
static NJS_CNK_OBJECT draw_323 = {
#include "stg22/terrain/draw_323.inc"
};
static NJS_CNK_OBJECT draw_322 = {
#include "stg22/terrain/draw_322.inc"
};
static NJS_CNK_OBJECT draw_321 = {
#include "stg22/terrain/draw_321.inc"
};
static NJS_CNK_OBJECT draw_320 = {
#include "stg22/terrain/draw_320.inc"
};
static NJS_CNK_OBJECT draw_319 = {
#include "stg22/terrain/draw_319.inc"
};
static NJS_CNK_OBJECT draw_318 = {
#include "stg22/terrain/draw_318.inc"
};
static NJS_CNK_OBJECT draw_317 = {
#include "stg22/terrain/draw_317.inc"
};
static NJS_CNK_OBJECT draw_316 = {
#include "stg22/terrain/draw_316.inc"
};
static NJS_CNK_OBJECT draw_315 = {
#include "stg22/terrain/draw_315.inc"
};
static NJS_CNK_OBJECT draw_314 = {
#include "stg22/terrain/draw_314.inc"
};
static NJS_CNK_OBJECT draw_313 = {
#include "stg22/terrain/draw_313.inc"
};
static NJS_CNK_OBJECT draw_312 = {
#include "stg22/terrain/draw_312.inc"
};
static NJS_CNK_OBJECT draw_311 = {
#include "stg22/terrain/draw_311.inc"
};
static NJS_CNK_OBJECT draw_310 = {
#include "stg22/terrain/draw_310.inc"
};
static NJS_CNK_OBJECT draw_309 = {
#include "stg22/terrain/draw_309.inc"
};
static NJS_CNK_OBJECT draw_308 = {
#include "stg22/terrain/draw_308.inc"
};
static NJS_CNK_OBJECT draw_307 = {
#include "stg22/terrain/draw_307.inc"
};
static NJS_CNK_OBJECT draw_306 = {
#include "stg22/terrain/draw_306.inc"
};
static NJS_CNK_OBJECT draw_305 = {
#include "stg22/terrain/draw_305.inc"
};
static NJS_CNK_OBJECT draw_304 = {
#include "stg22/terrain/draw_304.inc"
};
static NJS_CNK_OBJECT draw_303 = {
#include "stg22/terrain/draw_303.inc"
};
static NJS_CNK_OBJECT draw_302 = {
#include "stg22/terrain/draw_302.inc"
};
static NJS_CNK_OBJECT draw_301 = {
#include "stg22/terrain/draw_301.inc"
};
static NJS_CNK_OBJECT draw_300 = {
#include "stg22/terrain/draw_300.inc"
};
static NJS_CNK_OBJECT draw_299 = {
#include "stg22/terrain/draw_299.inc"
};
static NJS_CNK_OBJECT draw_298 = {
#include "stg22/terrain/draw_298.inc"
};
static NJS_CNK_OBJECT draw_297 = {
#include "stg22/terrain/draw_297.inc"
};
static NJS_CNK_OBJECT draw_296 = {
#include "stg22/terrain/draw_296.inc"
};
static NJS_CNK_OBJECT draw_295 = {
#include "stg22/terrain/draw_295.inc"
};
static NJS_CNK_OBJECT draw_294 = {
#include "stg22/terrain/draw_294.inc"
};
static NJS_CNK_OBJECT draw_293 = {
#include "stg22/terrain/draw_293.inc"
};
static NJS_CNK_OBJECT draw_292 = {
#include "stg22/terrain/draw_292.inc"
};
static NJS_CNK_OBJECT draw_291 = {
#include "stg22/terrain/draw_291.inc"
};
static NJS_CNK_OBJECT draw_290 = {
#include "stg22/terrain/draw_290.inc"
};
static NJS_CNK_OBJECT draw_289 = {
#include "stg22/terrain/draw_289.inc"
};
static NJS_CNK_OBJECT draw_288 = {
#include "stg22/terrain/draw_288.inc"
};
static NJS_CNK_OBJECT draw_287 = {
#include "stg22/terrain/draw_287.inc"
};
static NJS_CNK_OBJECT draw_286 = {
#include "stg22/terrain/draw_286.inc"
};
static NJS_CNK_OBJECT draw_284 = {
#include "stg22/terrain/draw_284.inc"
};
static NJS_CNK_OBJECT draw_283 = {
#include "stg22/terrain/draw_283.inc"
};
static NJS_CNK_OBJECT draw_282 = {
#include "stg22/terrain/draw_282.inc"
};
static NJS_CNK_OBJECT draw_281 = {
#include "stg22/terrain/draw_281.inc"
};
static NJS_CNK_OBJECT draw_280 = {
#include "stg22/terrain/draw_280.inc"
};
static NJS_CNK_OBJECT draw_279 = {
#include "stg22/terrain/draw_279.inc"
};
static NJS_CNK_OBJECT draw_278 = {
#include "stg22/terrain/draw_278.inc"
};
static NJS_CNK_OBJECT draw_277 = {
#include "stg22/terrain/draw_277.inc"
};
static NJS_CNK_OBJECT draw_276 = {
#include "stg22/terrain/draw_276.inc"
};
static NJS_CNK_OBJECT draw_275 = {
#include "stg22/terrain/draw_275.inc"
};
static NJS_CNK_OBJECT draw_274 = {
#include "stg22/terrain/draw_274.inc"
};
static NJS_CNK_OBJECT draw_273 = {
#include "stg22/terrain/draw_273.inc"
};
static NJS_CNK_OBJECT draw_272 = {
#include "stg22/terrain/draw_272.inc"
};
static NJS_CNK_OBJECT draw_271 = {
#include "stg22/terrain/draw_271.inc"
};
static NJS_CNK_OBJECT draw_270 = {
#include "stg22/terrain/draw_270.inc"
};
static NJS_CNK_OBJECT draw_269 = {
#include "stg22/terrain/draw_269.inc"
};
static NJS_CNK_OBJECT draw_268 = {
#include "stg22/terrain/draw_268.inc"
};
static NJS_CNK_OBJECT draw_267 = {
#include "stg22/terrain/draw_267.inc"
};
static NJS_CNK_OBJECT draw_266 = {
#include "stg22/terrain/draw_266.inc"
};
static NJS_CNK_OBJECT draw_265 = {
#include "stg22/terrain/draw_265.inc"
};
static NJS_CNK_OBJECT draw_264 = {
#include "stg22/terrain/draw_264.inc"
};
static NJS_CNK_OBJECT draw_263 = {
#include "stg22/terrain/draw_263.inc"
};
static NJS_CNK_OBJECT draw_262 = {
#include "stg22/terrain/draw_262.inc"
};
static NJS_CNK_OBJECT draw_261 = {
#include "stg22/terrain/draw_261.inc"
};
static NJS_CNK_OBJECT draw_260 = {
#include "stg22/terrain/draw_260.inc"
};
static NJS_CNK_OBJECT draw_259 = {
#include "stg22/terrain/draw_259.inc"
};
static NJS_CNK_OBJECT draw_257 = {
#include "stg22/terrain/draw_257.inc"
};
static NJS_CNK_OBJECT draw_256 = {
#include "stg22/terrain/draw_256.inc"
};
static NJS_CNK_OBJECT draw_255 = {
#include "stg22/terrain/draw_255.inc"
};
static NJS_CNK_OBJECT draw_254 = {
#include "stg22/terrain/draw_254.inc"
};
static NJS_CNK_OBJECT draw_253 = {
#include "stg22/terrain/draw_253.inc"
};
static NJS_CNK_OBJECT draw_252 = {
#include "stg22/terrain/draw_252.inc"
};
static NJS_CNK_OBJECT draw_251 = {
#include "stg22/terrain/draw_251.inc"
};
static NJS_CNK_OBJECT draw_250 = {
#include "stg22/terrain/draw_250.inc"
};
static NJS_CNK_OBJECT draw_249 = {
#include "stg22/terrain/draw_249.inc"
};
static NJS_CNK_OBJECT draw_247 = {
#include "stg22/terrain/draw_247.inc"
};
static NJS_CNK_OBJECT draw_246 = {
#include "stg22/terrain/draw_246.inc"
};
static NJS_CNK_OBJECT draw_245 = {
#include "stg22/terrain/draw_245.inc"
};
static NJS_CNK_OBJECT draw_244 = {
#include "stg22/terrain/draw_244.inc"
};
static NJS_CNK_OBJECT draw_243 = {
#include "stg22/terrain/draw_243.inc"
};
static NJS_CNK_OBJECT draw_242 = {
#include "stg22/terrain/draw_242.inc"
};
static NJS_CNK_OBJECT draw_241 = {
#include "stg22/terrain/draw_241.inc"
};
static NJS_CNK_OBJECT draw_240 = {
#include "stg22/terrain/draw_240.inc"
};
static NJS_CNK_OBJECT draw_239 = {
#include "stg22/terrain/draw_239.inc"
};
static NJS_CNK_OBJECT draw_237 = {
#include "stg22/terrain/draw_237.inc"
};
static NJS_CNK_OBJECT draw_236 = {
#include "stg22/terrain/draw_236.inc"
};
static NJS_CNK_OBJECT draw_235 = {
#include "stg22/terrain/draw_235.inc"
};
static NJS_CNK_OBJECT draw_234 = {
#include "stg22/terrain/draw_234.inc"
};
static NJS_CNK_OBJECT draw_233 = {
#include "stg22/terrain/draw_233.inc"
};
static NJS_CNK_OBJECT draw_232 = {
#include "stg22/terrain/draw_232.inc"
};
static NJS_CNK_OBJECT draw_231 = {
#include "stg22/terrain/draw_231.inc"
};
static NJS_CNK_OBJECT draw_230 = {
#include "stg22/terrain/draw_230.inc"
};
static NJS_CNK_OBJECT draw_228 = {
#include "stg22/terrain/draw_228.inc"
};
static NJS_CNK_OBJECT draw_227 = {
#include "stg22/terrain/draw_227.inc"
};
static NJS_CNK_OBJECT draw_226 = {
#include "stg22/terrain/draw_226.inc"
};
static NJS_CNK_OBJECT draw_225 = {
#include "stg22/terrain/draw_225.inc"
};
static NJS_CNK_OBJECT draw_224 = {
#include "stg22/terrain/draw_224.inc"
};
static NJS_CNK_OBJECT draw_223 = {
#include "stg22/terrain/draw_223.inc"
};
static NJS_CNK_OBJECT draw_222 = {
#include "stg22/terrain/draw_222.inc"
};
static NJS_CNK_OBJECT draw_221 = {
#include "stg22/terrain/draw_221.inc"
};
static NJS_CNK_OBJECT draw_220 = {
#include "stg22/terrain/draw_220.inc"
};
static NJS_CNK_OBJECT draw_219 = {
#include "stg22/terrain/draw_219.inc"
};
static NJS_CNK_OBJECT draw_218 = {
#include "stg22/terrain/draw_218.inc"
};
static NJS_CNK_OBJECT draw_217 = {
#include "stg22/terrain/draw_217.inc"
};
static NJS_CNK_OBJECT draw_216 = {
#include "stg22/terrain/draw_216.inc"
};
static NJS_CNK_OBJECT draw_215 = {
#include "stg22/terrain/draw_215.inc"
};
static NJS_CNK_OBJECT draw_214 = {
#include "stg22/terrain/draw_214.inc"
};
static NJS_CNK_OBJECT draw_213 = {
#include "stg22/terrain/draw_213.inc"
};
static NJS_CNK_OBJECT draw_212 = {
#include "stg22/terrain/draw_212.inc"
};
static NJS_CNK_OBJECT draw_211 = {
#include "stg22/terrain/draw_211.inc"
};
static NJS_CNK_OBJECT draw_210 = {
#include "stg22/terrain/draw_210.inc"
};
static NJS_CNK_OBJECT draw_209 = {
#include "stg22/terrain/draw_209.inc"
};
static NJS_CNK_OBJECT draw_208 = {
#include "stg22/terrain/draw_208.inc"
};
static NJS_CNK_OBJECT draw_207 = {
#include "stg22/terrain/draw_207.inc"
};
static NJS_CNK_OBJECT draw_206 = {
#include "stg22/terrain/draw_206.inc"
};
static NJS_CNK_OBJECT draw_204 = {
#include "stg22/terrain/draw_204.inc"
};
static NJS_CNK_OBJECT draw_202 = {
#include "stg22/terrain/draw_202.inc"
};
static NJS_CNK_OBJECT draw_201 = {
#include "stg22/terrain/draw_201.inc"
};
static NJS_CNK_OBJECT draw_200 = {
#include "stg22/terrain/draw_200.inc"
};
static NJS_CNK_OBJECT draw_199 = {
#include "stg22/terrain/draw_199.inc"
};
static NJS_CNK_OBJECT draw_198 = {
#include "stg22/terrain/draw_198.inc"
};
static NJS_CNK_OBJECT draw_197 = {
#include "stg22/terrain/draw_197.inc"
};
static NJS_CNK_OBJECT draw_196 = {
#include "stg22/terrain/draw_196.inc"
};
static NJS_CNK_OBJECT draw_195 = {
#include "stg22/terrain/draw_195.inc"
};
static NJS_CNK_OBJECT draw_194 = {
#include "stg22/terrain/draw_194.inc"
};
static NJS_CNK_OBJECT draw_193 = {
#include "stg22/terrain/draw_193.inc"
};
static NJS_CNK_OBJECT draw_192 = {
#include "stg22/terrain/draw_192.inc"
};
static NJS_CNK_OBJECT draw_191 = {
#include "stg22/terrain/draw_191.inc"
};
static NJS_CNK_OBJECT draw_190 = {
#include "stg22/terrain/draw_190.inc"
};
static NJS_CNK_OBJECT draw_189 = {
#include "stg22/terrain/draw_189.inc"
};
static NJS_CNK_OBJECT draw_188 = {
#include "stg22/terrain/draw_188.inc"
};
static NJS_CNK_OBJECT draw_187 = {
#include "stg22/terrain/draw_187.inc"
};
static NJS_CNK_OBJECT draw_186 = {
#include "stg22/terrain/draw_186.inc"
};
static NJS_CNK_OBJECT draw_185 = {
#include "stg22/terrain/draw_185.inc"
};
static NJS_CNK_OBJECT draw_184 = {
#include "stg22/terrain/draw_184.inc"
};
static NJS_CNK_OBJECT draw_183 = {
#include "stg22/terrain/draw_183.inc"
};
static NJS_CNK_OBJECT draw_182 = {
#include "stg22/terrain/draw_182.inc"
};
static NJS_CNK_OBJECT draw_181 = {
#include "stg22/terrain/draw_181.inc"
};
static NJS_CNK_OBJECT draw_180 = {
#include "stg22/terrain/draw_180.inc"
};
static NJS_CNK_OBJECT draw_179 = {
#include "stg22/terrain/draw_179.inc"
};
static NJS_CNK_OBJECT draw_178 = {
#include "stg22/terrain/draw_178.inc"
};
static NJS_CNK_OBJECT draw_177 = {
#include "stg22/terrain/draw_177.inc"
};
static NJS_CNK_OBJECT draw_176 = {
#include "stg22/terrain/draw_176.inc"
};
static NJS_CNK_OBJECT draw_175 = {
#include "stg22/terrain/draw_175.inc"
};
static NJS_CNK_OBJECT draw_174 = {
#include "stg22/terrain/draw_174.inc"
};
static NJS_CNK_OBJECT draw_173 = {
#include "stg22/terrain/draw_173.inc"
};
static NJS_CNK_OBJECT draw_172 = {
#include "stg22/terrain/draw_172.inc"
};
static NJS_CNK_OBJECT draw_171 = {
#include "stg22/terrain/draw_171.inc"
};
static NJS_CNK_OBJECT draw_170 = {
#include "stg22/terrain/draw_170.inc"
};
static NJS_CNK_OBJECT draw_169 = {
#include "stg22/terrain/draw_169.inc"
};
static NJS_CNK_OBJECT draw_168 = {
#include "stg22/terrain/draw_168.inc"
};
static NJS_CNK_OBJECT draw_167 = {
#include "stg22/terrain/draw_167.inc"
};
static NJS_CNK_OBJECT draw_166 = {
#include "stg22/terrain/draw_166.inc"
};
static NJS_CNK_OBJECT draw_165 = {
#include "stg22/terrain/draw_165.inc"
};
static NJS_CNK_OBJECT draw_164 = {
#include "stg22/terrain/draw_164.inc"
};
static NJS_CNK_OBJECT draw_163 = {
#include "stg22/terrain/draw_163.inc"
};
static NJS_CNK_OBJECT draw_162 = {
#include "stg22/terrain/draw_162.inc"
};
static NJS_CNK_OBJECT draw_161 = {
#include "stg22/terrain/draw_161.inc"
};
static NJS_CNK_OBJECT draw_160 = {
#include "stg22/terrain/draw_160.inc"
};
static NJS_CNK_OBJECT draw_159 = {
#include "stg22/terrain/draw_159.inc"
};
static NJS_CNK_OBJECT draw_158 = {
#include "stg22/terrain/draw_158.inc"
};
static NJS_CNK_OBJECT draw_157 = {
#include "stg22/terrain/draw_157.inc"
};
static NJS_CNK_OBJECT draw_156 = {
#include "stg22/terrain/draw_156.inc"
};
static NJS_CNK_OBJECT draw_155 = {
#include "stg22/terrain/draw_155.inc"
};
static NJS_CNK_OBJECT draw_154 = {
#include "stg22/terrain/draw_154.inc"
};
static NJS_CNK_OBJECT draw_153 = {
#include "stg22/terrain/draw_153.inc"
};
static NJS_CNK_OBJECT draw_152 = {
#include "stg22/terrain/draw_152.inc"
};
static NJS_CNK_OBJECT draw_151 = {
#include "stg22/terrain/draw_151.inc"
};
static NJS_CNK_OBJECT draw_150 = {
#include "stg22/terrain/draw_150.inc"
};
static NJS_CNK_OBJECT draw_149 = {
#include "stg22/terrain/draw_149.inc"
};
static NJS_CNK_OBJECT draw_148 = {
#include "stg22/terrain/draw_148.inc"
};
static NJS_CNK_OBJECT draw_147 = {
#include "stg22/terrain/draw_147.inc"
};
static NJS_CNK_OBJECT draw_146 = {
#include "stg22/terrain/draw_146.inc"
};
static NJS_CNK_OBJECT draw_145 = {
#include "stg22/terrain/draw_145.inc"
};
static NJS_CNK_OBJECT draw_144 = {
#include "stg22/terrain/draw_144.inc"
};
static NJS_CNK_OBJECT draw_143 = {
#include "stg22/terrain/draw_143.inc"
};
static NJS_CNK_OBJECT draw_142 = {
#include "stg22/terrain/draw_142.inc"
};
static NJS_CNK_OBJECT draw_141 = {
#include "stg22/terrain/draw_141.inc"
};
static NJS_CNK_OBJECT draw_140 = {
#include "stg22/terrain/draw_140.inc"
};
static NJS_CNK_OBJECT draw_139 = {
#include "stg22/terrain/draw_139.inc"
};
static NJS_CNK_OBJECT draw_138 = {
#include "stg22/terrain/draw_138.inc"
};
static NJS_CNK_OBJECT draw_137 = {
#include "stg22/terrain/draw_137.inc"
};
static NJS_CNK_OBJECT draw_136 = {
#include "stg22/terrain/draw_136.inc"
};
static NJS_CNK_OBJECT draw_135 = {
#include "stg22/terrain/draw_135.inc"
};
static NJS_CNK_OBJECT draw_134 = {
#include "stg22/terrain/draw_134.inc"
};
static NJS_CNK_OBJECT draw_133 = {
#include "stg22/terrain/draw_133.inc"
};
static NJS_CNK_OBJECT draw_132 = {
#include "stg22/terrain/draw_132.inc"
};
static NJS_CNK_OBJECT draw_131 = {
#include "stg22/terrain/draw_131.inc"
};
static NJS_CNK_OBJECT draw_130 = {
#include "stg22/terrain/draw_130.inc"
};
static NJS_CNK_OBJECT draw_129 = {
#include "stg22/terrain/draw_129.inc"
};
static NJS_CNK_OBJECT draw_128 = {
#include "stg22/terrain/draw_128.inc"
};
static NJS_CNK_OBJECT draw_127 = {
#include "stg22/terrain/draw_127.inc"
};
static NJS_CNK_OBJECT draw_126 = {
#include "stg22/terrain/draw_126.inc"
};
static NJS_CNK_OBJECT draw_125 = {
#include "stg22/terrain/draw_125.inc"
};
static NJS_CNK_OBJECT draw_124 = {
#include "stg22/terrain/draw_124.inc"
};
static NJS_CNK_OBJECT draw_123 = {
#include "stg22/terrain/draw_123.inc"
};
static NJS_CNK_OBJECT draw_122 = {
#include "stg22/terrain/draw_122.inc"
};
static NJS_CNK_OBJECT draw_121 = {
#include "stg22/terrain/draw_121.inc"
};
static NJS_CNK_OBJECT draw_120 = {
#include "stg22/terrain/draw_120.inc"
};
static NJS_CNK_OBJECT draw_119 = {
#include "stg22/terrain/draw_119.inc"
};
static NJS_CNK_OBJECT draw_118 = {
#include "stg22/terrain/draw_118.inc"
};
static NJS_CNK_OBJECT draw_117 = {
#include "stg22/terrain/draw_117.inc"
};
static NJS_CNK_OBJECT draw_116 = {
#include "stg22/terrain/draw_116.inc"
};
static NJS_CNK_OBJECT draw_115 = {
#include "stg22/terrain/draw_115.inc"
};
static NJS_CNK_OBJECT draw_114 = {
#include "stg22/terrain/draw_114.inc"
};
static NJS_CNK_OBJECT draw_113 = {
#include "stg22/terrain/draw_113.inc"
};
static NJS_CNK_OBJECT draw_112 = {
#include "stg22/terrain/draw_112.inc"
};
static NJS_CNK_OBJECT draw_111 = {
#include "stg22/terrain/draw_111.inc"
};
static NJS_CNK_OBJECT draw_110 = {
#include "stg22/terrain/draw_110.inc"
};
static NJS_CNK_OBJECT draw_109 = {
#include "stg22/terrain/draw_109.inc"
};
static NJS_CNK_OBJECT draw_108 = {
#include "stg22/terrain/draw_108.inc"
};
static NJS_CNK_OBJECT draw_107 = {
#include "stg22/terrain/draw_107.inc"
};
static NJS_CNK_OBJECT draw_106 = {
#include "stg22/terrain/draw_106.inc"
};
static NJS_CNK_OBJECT draw_105 = {
#include "stg22/terrain/draw_105.inc"
};
static NJS_CNK_OBJECT draw_104 = {
#include "stg22/terrain/draw_104.inc"
};
static NJS_CNK_OBJECT draw_103 = {
#include "stg22/terrain/draw_103.inc"
};
static NJS_CNK_OBJECT draw_102 = {
#include "stg22/terrain/draw_102.inc"
};
static NJS_CNK_OBJECT draw_101 = {
#include "stg22/terrain/draw_101.inc"
};
static NJS_CNK_OBJECT draw_100 = {
#include "stg22/terrain/draw_100.inc"
};
static NJS_CNK_OBJECT draw_99 = {
#include "stg22/terrain/draw_99.inc"
};
static NJS_CNK_OBJECT draw_98 = {
#include "stg22/terrain/draw_98.inc"
};
static NJS_CNK_OBJECT draw_97 = {
#include "stg22/terrain/draw_97.inc"
};
static NJS_CNK_OBJECT draw_96 = {
#include "stg22/terrain/draw_96.inc"
};
static NJS_CNK_OBJECT draw_95 = {
#include "stg22/terrain/draw_95.inc"
};
static NJS_CNK_OBJECT draw_94 = {
#include "stg22/terrain/draw_94.inc"
};
static NJS_CNK_OBJECT draw_93 = {
#include "stg22/terrain/draw_93.inc"
};
static NJS_CNK_OBJECT draw_92 = {
#include "stg22/terrain/draw_92.inc"
};
static NJS_CNK_OBJECT draw_91 = {
#include "stg22/terrain/draw_91.inc"
};
static NJS_CNK_OBJECT draw_90 = {
#include "stg22/terrain/draw_90.inc"
};
static NJS_CNK_OBJECT draw_89 = {
#include "stg22/terrain/draw_89.inc"
};
static NJS_CNK_OBJECT draw_88 = {
#include "stg22/terrain/draw_88.inc"
};
static NJS_CNK_OBJECT draw_87 = {
#include "stg22/terrain/draw_87.inc"
};
static NJS_CNK_OBJECT draw_86 = {
#include "stg22/terrain/draw_86.inc"
};
static NJS_CNK_OBJECT draw_85 = {
#include "stg22/terrain/draw_85.inc"
};
static NJS_CNK_OBJECT draw_84 = {
#include "stg22/terrain/draw_84.inc"
};
static NJS_CNK_OBJECT draw_83 = {
#include "stg22/terrain/draw_83.inc"
};
static NJS_CNK_OBJECT draw_82 = {
#include "stg22/terrain/draw_82.inc"
};
static NJS_CNK_OBJECT draw_81 = {
#include "stg22/terrain/draw_81.inc"
};
static NJS_CNK_OBJECT draw_80 = {
#include "stg22/terrain/draw_80.inc"
};
static NJS_CNK_OBJECT draw_79 = {
#include "stg22/terrain/draw_79.inc"
};
static NJS_CNK_OBJECT draw_78 = {
#include "stg22/terrain/draw_78.inc"
};
static NJS_CNK_OBJECT draw_77 = {
#include "stg22/terrain/draw_77.inc"
};
static NJS_CNK_OBJECT draw_76 = {
#include "stg22/terrain/draw_76.inc"
};
static NJS_CNK_OBJECT draw_75 = {
#include "stg22/terrain/draw_75.inc"
};
static NJS_CNK_OBJECT draw_74 = {
#include "stg22/terrain/draw_74.inc"
};
static NJS_CNK_OBJECT draw_73 = {
#include "stg22/terrain/draw_73.inc"
};
static NJS_CNK_OBJECT draw_72 = {
#include "stg22/terrain/draw_72.inc"
};
static NJS_CNK_OBJECT draw_71 = {
#include "stg22/terrain/draw_71.inc"
};
static NJS_CNK_OBJECT draw_70 = {
#include "stg22/terrain/draw_70.inc"
};
static NJS_CNK_OBJECT draw_69 = {
#include "stg22/terrain/draw_69.inc"
};
static NJS_CNK_OBJECT draw_68 = {
#include "stg22/terrain/draw_68.inc"
};
static NJS_CNK_OBJECT draw_67 = {
#include "stg22/terrain/draw_67.inc"
};
static NJS_CNK_OBJECT draw_66 = {
#include "stg22/terrain/draw_66.inc"
};
static NJS_CNK_OBJECT draw_65 = {
#include "stg22/terrain/draw_65.inc"
};
static NJS_CNK_OBJECT draw_64 = {
#include "stg22/terrain/draw_64.inc"
};
static NJS_CNK_OBJECT draw_63 = {
#include "stg22/terrain/draw_63.inc"
};
static NJS_CNK_OBJECT draw_62 = {
#include "stg22/terrain/draw_62.inc"
};
static NJS_CNK_OBJECT draw_61 = {
#include "stg22/terrain/draw_61.inc"
};
static NJS_CNK_OBJECT draw_60 = {
#include "stg22/terrain/draw_60.inc"
};
static NJS_CNK_OBJECT draw_59 = {
#include "stg22/terrain/draw_59.inc"
};
static NJS_CNK_OBJECT draw_58 = {
#include "stg22/terrain/draw_58.inc"
};
static NJS_CNK_OBJECT draw_57 = {
#include "stg22/terrain/draw_57.inc"
};
static NJS_CNK_OBJECT draw_56 = {
#include "stg22/terrain/draw_56.inc"
};
static NJS_CNK_OBJECT draw_55 = {
#include "stg22/terrain/draw_55.inc"
};
static NJS_CNK_OBJECT draw_54 = {
#include "stg22/terrain/draw_54.inc"
};
static NJS_CNK_OBJECT draw_53 = {
#include "stg22/terrain/draw_53.inc"
};
static NJS_CNK_OBJECT draw_52 = {
#include "stg22/terrain/draw_52.inc"
};
static NJS_CNK_OBJECT draw_51 = {
#include "stg22/terrain/draw_51.inc"
};
static NJS_CNK_OBJECT draw_50 = {
#include "stg22/terrain/draw_50.inc"
};
static NJS_CNK_OBJECT draw_49 = {
#include "stg22/terrain/draw_49.inc"
};
static NJS_CNK_OBJECT draw_48 = {
#include "stg22/terrain/draw_48.inc"
};
static NJS_CNK_OBJECT draw_47 = {
#include "stg22/terrain/draw_47.inc"
};
static NJS_CNK_OBJECT draw_46 = {
#include "stg22/terrain/draw_46.inc"
};
static NJS_CNK_OBJECT draw_45 = {
#include "stg22/terrain/draw_45.inc"
};
static NJS_CNK_OBJECT draw_44 = {
#include "stg22/terrain/draw_44.inc"
};
static NJS_CNK_OBJECT draw_43 = {
#include "stg22/terrain/draw_43.inc"
};
static NJS_CNK_OBJECT draw_42 = {
#include "stg22/terrain/draw_42.inc"
};
static NJS_CNK_OBJECT draw_41 = {
#include "stg22/terrain/draw_41.inc"
};
static NJS_CNK_OBJECT draw_40 = {
#include "stg22/terrain/draw_40.inc"
};
static NJS_CNK_OBJECT draw_39 = {
#include "stg22/terrain/draw_39.inc"
};
static NJS_CNK_OBJECT draw_38 = {
#include "stg22/terrain/draw_38.inc"
};
static NJS_CNK_OBJECT draw_37 = {
#include "stg22/terrain/draw_37.inc"
};
static NJS_CNK_OBJECT draw_36 = {
#include "stg22/terrain/draw_36.inc"
};
static NJS_CNK_OBJECT draw_35 = {
#include "stg22/terrain/draw_35.inc"
};
static NJS_CNK_OBJECT draw_34 = {
#include "stg22/terrain/draw_34.inc"
};
static NJS_CNK_OBJECT draw_33 = {
#include "stg22/terrain/draw_33.inc"
};
static NJS_CNK_OBJECT draw_32 = {
#include "stg22/terrain/draw_32.inc"
};
static NJS_CNK_OBJECT draw_31 = {
#include "stg22/terrain/draw_31.inc"
};
static NJS_CNK_OBJECT draw_30 = {
#include "stg22/terrain/draw_30.inc"
};
static NJS_CNK_OBJECT draw_29 = {
#include "stg22/terrain/draw_29.inc"
};
static NJS_CNK_OBJECT draw_28 = {
#include "stg22/terrain/draw_28.inc"
};
static NJS_CNK_OBJECT draw_26 = {
#include "stg22/terrain/draw_26.inc"
};
static NJS_CNK_OBJECT draw_24 = {
#include "stg22/terrain/draw_24.inc"
};
static NJS_CNK_OBJECT draw_23 = {
#include "stg22/terrain/draw_23.inc"
};
static NJS_CNK_OBJECT draw_22 = {
#include "stg22/terrain/draw_22.inc"
};
static NJS_CNK_OBJECT draw_20 = {
#include "stg22/terrain/draw_20.inc"
};
static NJS_CNK_OBJECT draw_19 = {
#include "stg22/terrain/draw_19.inc"
};
static NJS_CNK_OBJECT draw_18 = {
#include "stg22/terrain/draw_18.inc"
};
static NJS_CNK_OBJECT draw_16 = {
#include "stg22/terrain/draw_16.inc"
};
static NJS_CNK_OBJECT draw_15 = {
#include "stg22/terrain/draw_15.inc"
};
static NJS_CNK_OBJECT draw_14 = {
#include "stg22/terrain/draw_14.inc"
};
static NJS_CNK_OBJECT draw_13 = {
#include "stg22/terrain/draw_13.inc"
};
static NJS_CNK_OBJECT draw_12 = {
#include "stg22/terrain/draw_12.inc"
};
static NJS_CNK_OBJECT draw_11 = {
#include "stg22/terrain/draw_11.inc"
};
static NJS_CNK_OBJECT draw_10 = {
#include "stg22/terrain/draw_10.inc"
};
static NJS_OBJECT collision_1 = {
#include "stg22/terrain/collision_1.inc"
};
static NJS_OBJECT collision_6 = {
#include "stg22/terrain/collision_6.inc"
};
static NJS_OBJECT collision_9 = {
#include "stg22/terrain/collision_9.inc"
};
static NJS_OBJECT collision_12 = {
#include "stg22/terrain/collision_12.inc"
};
static NJS_OBJECT collision_13 = {
#include "stg22/terrain/collision_13.inc"
};
static NJS_OBJECT collision_14 = {
#include "stg22/terrain/collision_14.inc"
};
static NJS_OBJECT collision_15 = {
#include "stg22/terrain/collision_15.inc"
};
static NJS_OBJECT collision_16 = {
#include "stg22/terrain/collision_16.inc"
};
static NJS_OBJECT collision_18 = {
#include "stg22/terrain/collision_18.inc"
};
static NJS_OBJECT collision_19 = {
#include "stg22/terrain/collision_19.inc"
};
static NJS_OBJECT collision_20 = {
#include "stg22/terrain/collision_20.inc"
};
static NJS_OBJECT collision_21 = {
#include "stg22/terrain/collision_21.inc"
};
static NJS_OBJECT collision_22 = {
#include "stg22/terrain/collision_22.inc"
};
static NJS_OBJECT collision_23 = {
#include "stg22/terrain/collision_23.inc"
};
static NJS_OBJECT collision_24 = {
#include "stg22/terrain/collision_24.inc"
};
static NJS_OBJECT collision_25 = {
#include "stg22/terrain/collision_25.inc"
};
static NJS_OBJECT collision_26 = {
#include "stg22/terrain/collision_26.inc"
};
static NJS_OBJECT collision_27 = {
#include "stg22/terrain/collision_27.inc"
};
static NJS_OBJECT collision_28 = {
#include "stg22/terrain/collision_28.inc"
};
static NJS_OBJECT collision_29 = {
#include "stg22/terrain/collision_29.inc"
};
static NJS_OBJECT collision_30 = {
#include "stg22/terrain/collision_30.inc"
};
static NJS_OBJECT collision_31 = {
#include "stg22/terrain/collision_31.inc"
};
static NJS_OBJECT collision_32 = {
#include "stg22/terrain/collision_32.inc"
};
static NJS_OBJECT collision_33 = {
#include "stg22/terrain/collision_33.inc"
};
static NJS_OBJECT collision_34 = {
#include "stg22/terrain/collision_34.inc"
};
static NJS_OBJECT collision_35 = {
#include "stg22/terrain/collision_35.inc"
};
static NJS_OBJECT collision_36 = {
#include "stg22/terrain/collision_36.inc"
};
static NJS_OBJECT collision_37 = {
#include "stg22/terrain/collision_37.inc"
};
static NJS_OBJECT collision_38 = {
#include "stg22/terrain/collision_38.inc"
};
static NJS_OBJECT collision_39 = {
#include "stg22/terrain/collision_39.inc"
};
static NJS_OBJECT collision_40 = {
#include "stg22/terrain/collision_40.inc"
};
static NJS_OBJECT collision_43 = {
#include "stg22/terrain/collision_43.inc"
};
static NJS_OBJECT collision_45 = {
#include "stg22/terrain/collision_45.inc"
};
static NJS_OBJECT collision_49 = {
#include "stg22/terrain/collision_49.inc"
};
static NJS_OBJECT collision_50 = {
#include "stg22/terrain/collision_50.inc"
};
static NJS_OBJECT collision_51 = {
#include "stg22/terrain/collision_51.inc"
};
static NJS_OBJECT collision_52 = {
#include "stg22/terrain/collision_52.inc"
};
static NJS_OBJECT collision_53 = {
#include "stg22/terrain/collision_53.inc"
};
static NJS_OBJECT collision_54 = {
#include "stg22/terrain/collision_54.inc"
};
static NJS_OBJECT collision_56 = {
#include "stg22/terrain/collision_56.inc"
};
static NJS_OBJECT collision_61 = {
#include "stg22/terrain/collision_61.inc"
};
static NJS_OBJECT collision_62 = {
#include "stg22/terrain/collision_62.inc"
};
static NJS_OBJECT collision_63 = {
#include "stg22/terrain/collision_63.inc"
};
static NJS_OBJECT collision_64 = {
#include "stg22/terrain/collision_64.inc"
};
static NJS_OBJECT collision_65 = {
#include "stg22/terrain/collision_65.inc"
};
static NJS_OBJECT collision_66 = {
#include "stg22/terrain/collision_66.inc"
};
static NJS_OBJECT collision_68 = {
#include "stg22/terrain/collision_68.inc"
};
static NJS_OBJECT collision_69 = {
#include "stg22/terrain/collision_69.inc"
};
static NJS_OBJECT collision_70 = {
#include "stg22/terrain/collision_70.inc"
};
static NJS_OBJECT collision_71 = {
#include "stg22/terrain/collision_71.inc"
};
static NJS_OBJECT collision_72 = {
#include "stg22/terrain/collision_72.inc"
};
static NJS_OBJECT collision_73 = {
#include "stg22/terrain/collision_73.inc"
};
static NJS_OBJECT collision_74 = {
#include "stg22/terrain/collision_74.inc"
};
static NJS_OBJECT collision_75 = {
#include "stg22/terrain/collision_75.inc"
};
static NJS_OBJECT collision_76 = {
#include "stg22/terrain/collision_76.inc"
};
static NJS_OBJECT collision_77 = {
#include "stg22/terrain/collision_77.inc"
};
static NJS_OBJECT collision_78 = {
#include "stg22/terrain/collision_78.inc"
};
static NJS_OBJECT collision_79 = {
#include "stg22/terrain/collision_79.inc"
};
static NJS_OBJECT collision_80 = {
#include "stg22/terrain/collision_80.inc"
};
static NJS_OBJECT collision_81 = {
#include "stg22/terrain/collision_81.inc"
};
static NJS_OBJECT collision_82 = {
#include "stg22/terrain/collision_82.inc"
};
static NJS_OBJECT collision_83 = {
#include "stg22/terrain/collision_83.inc"
};
static NJS_OBJECT collision_84 = {
#include "stg22/terrain/collision_84.inc"
};
static NJS_OBJECT collision_85 = {
#include "stg22/terrain/collision_85.inc"
};
static NJS_OBJECT collision_86 = {
#include "stg22/terrain/collision_86.inc"
};
static NJS_OBJECT collision_87 = {
#include "stg22/terrain/collision_87.inc"
};
static NJS_OBJECT collision_88 = {
#include "stg22/terrain/collision_88.inc"
};
static NJS_OBJECT collision_89 = {
#include "stg22/terrain/collision_89.inc"
};
static NJS_OBJECT collision_90 = {
#include "stg22/terrain/collision_90.inc"
};
static NJS_OBJECT collision_91 = {
#include "stg22/terrain/collision_91.inc"
};
static NJS_OBJECT collision_92 = {
#include "stg22/terrain/collision_92.inc"
};
static NJS_OBJECT collision_93 = {
#include "stg22/terrain/collision_93.inc"
};
static NJS_OBJECT collision_96 = {
#include "stg22/terrain/collision_96.inc"
};
static NJS_OBJECT collision_99 = {
#include "stg22/terrain/collision_99.inc"
};
static NJS_OBJECT collision_101 = {
#include "stg22/terrain/collision_101.inc"
};
static NJS_OBJECT collision_104 = {
#include "stg22/terrain/collision_104.inc"
};
static NJS_OBJECT collision_105 = {
#include "stg22/terrain/collision_105.inc"
};
static NJS_OBJECT collision_106 = {
#include "stg22/terrain/collision_106.inc"
};
static NJS_OBJECT collision_110 = {
#include "stg22/terrain/collision_110.inc"
};
static NJS_OBJECT collision_112 = {
#include "stg22/terrain/collision_112.inc"
};
static NJS_OBJECT collision_114 = {
#include "stg22/terrain/collision_114.inc"
};
static NJS_OBJECT collision_116 = {
#include "stg22/terrain/collision_116.inc"
};
static NJS_OBJECT collision_117 = {
#include "stg22/terrain/collision_117.inc"
};
static NJS_OBJECT collision_126 = {
#include "stg22/terrain/collision_126.inc"
};
static NJS_OBJECT collision_128 = {
#include "stg22/terrain/collision_128.inc"
};
static NJS_OBJECT collision_130 = {
#include "stg22/terrain/collision_130.inc"
};
static NJS_OBJECT collision_132 = {
#include "stg22/terrain/collision_132.inc"
};
static NJS_OBJECT collision_135 = {
#include "stg22/terrain/collision_135.inc"
};
static NJS_OBJECT collision_138 = {
#include "stg22/terrain/collision_138.inc"
};
static NJS_OBJECT collision_139 = {
#include "stg22/terrain/collision_139.inc"
};
static NJS_OBJECT collision_140 = {
#include "stg22/terrain/collision_140.inc"
};
static NJS_OBJECT collision_141 = {
#include "stg22/terrain/collision_141.inc"
};
static NJS_OBJECT collision_142 = {
#include "stg22/terrain/collision_142.inc"
};
static NJS_OBJECT collision_143 = {
#include "stg22/terrain/collision_143.inc"
};
static NJS_OBJECT collision_145 = {
#include "stg22/terrain/collision_145.inc"
};
static NJS_OBJECT collision_146 = {
#include "stg22/terrain/collision_146.inc"
};
static NJS_OBJECT collision_147 = {
#include "stg22/terrain/collision_147.inc"
};
static NJS_OBJECT collision_148 = {
#include "stg22/terrain/collision_148.inc"
};
static NJS_OBJECT collision_149 = {
#include "stg22/terrain/collision_149.inc"
};
static NJS_OBJECT collision_150 = {
#include "stg22/terrain/collision_150.inc"
};
static NJS_OBJECT collision_151 = {
#include "stg22/terrain/collision_151.inc"
};
static NJS_OBJECT collision_152 = {
#include "stg22/terrain/collision_152.inc"
};
static NJS_OBJECT collision_153 = {
#include "stg22/terrain/collision_153.inc"
};
static NJS_OBJECT collision_154 = {
#include "stg22/terrain/collision_154.inc"
};
static NJS_OBJECT collision_155 = {
#include "stg22/terrain/collision_155.inc"
};
static NJS_OBJECT collision_156 = {
#include "stg22/terrain/collision_156.inc"
};
static NJS_OBJECT collision_157 = {
#include "stg22/terrain/collision_157.inc"
};
static NJS_OBJECT collision_158 = {
#include "stg22/terrain/collision_158.inc"
};
static NJS_OBJECT collision_159 = {
#include "stg22/terrain/collision_159.inc"
};
static NJS_OBJECT collision_160 = {
#include "stg22/terrain/collision_160.inc"
};
static NJS_OBJECT collision_161 = {
#include "stg22/terrain/collision_161.inc"
};
static NJS_OBJECT collision_162 = {
#include "stg22/terrain/collision_162.inc"
};
static NJS_OBJECT collision_163 = {
#include "stg22/terrain/collision_163.inc"
};
static NJS_OBJECT collision_164 = {
#include "stg22/terrain/collision_164.inc"
};
static NJS_OBJECT collision_165 = {
#include "stg22/terrain/collision_165.inc"
};
static NJS_OBJECT collision_166 = {
#include "stg22/terrain/collision_166.inc"
};
static NJS_OBJECT collision_167 = {
#include "stg22/terrain/collision_167.inc"
};
static NJS_OBJECT collision_170 = {
#include "stg22/terrain/collision_170.inc"
};
static NJS_OBJECT collision_172 = {
#include "stg22/terrain/collision_172.inc"
};
static NJS_OBJECT collision_173 = {
#include "stg22/terrain/collision_173.inc"
};
static NJS_OBJECT collision_174 = {
#include "stg22/terrain/collision_174.inc"
};
static NJS_OBJECT collision_175 = {
#include "stg22/terrain/collision_175.inc"
};
static NJS_OBJECT collision_176 = {
#include "stg22/terrain/collision_176.inc"
};
static NJS_OBJECT collision_177 = {
#include "stg22/terrain/collision_177.inc"
};
static NJS_OBJECT collision_179 = {
#include "stg22/terrain/collision_179.inc"
};
static NJS_OBJECT collision_180 = {
#include "stg22/terrain/collision_180.inc"
};
static NJS_OBJECT collision_181 = {
#include "stg22/terrain/collision_181.inc"
};
static NJS_OBJECT collision_183 = {
#include "stg22/terrain/collision_183.inc"
};
static NJS_OBJECT collision_184 = {
#include "stg22/terrain/collision_184.inc"
};
static NJS_OBJECT collision_185 = {
#include "stg22/terrain/collision_185.inc"
};
static NJS_OBJECT collision_187 = {
#include "stg22/terrain/collision_187.inc"
};
static NJS_OBJECT collision_188 = {
#include "stg22/terrain/collision_188.inc"
};
static NJS_OBJECT collision_190 = {
#include "stg22/terrain/collision_190.inc"
};
static NJS_OBJECT collision_192 = {
#include "stg22/terrain/collision_192.inc"
};
static NJS_OBJECT collision_193 = {
#include "stg22/terrain/collision_193.inc"
};
static NJS_OBJECT collision_194 = {
#include "stg22/terrain/collision_194.inc"
};
static NJS_OBJECT collision_195 = {
#include "stg22/terrain/collision_195.inc"
};
static NJS_OBJECT collision_196 = {
#include "stg22/terrain/collision_196.inc"
};
static NJS_OBJECT collision_197 = {
#include "stg22/terrain/collision_197.inc"
};
static NJS_OBJECT collision_198 = {
#include "stg22/terrain/collision_198.inc"
};
static NJS_OBJECT collision_199 = {
#include "stg22/terrain/collision_199.inc"
};
static NJS_OBJECT collision_200 = {
#include "stg22/terrain/collision_200.inc"
};
static NJS_OBJECT collision_201 = {
#include "stg22/terrain/collision_201.inc"
};
static NJS_OBJECT collision_202 = {
#include "stg22/terrain/collision_202.inc"
};
static NJS_OBJECT collision_203 = {
#include "stg22/terrain/collision_203.inc"
};
static NJS_OBJECT collision_204 = {
#include "stg22/terrain/collision_204.inc"
};
static NJS_OBJECT collision_205 = {
#include "stg22/terrain/collision_205.inc"
};
static NJS_OBJECT collision_206 = {
#include "stg22/terrain/collision_206.inc"
};
static NJS_OBJECT collision_207 = {
#include "stg22/terrain/collision_207.inc"
};
static NJS_OBJECT collision_208 = {
#include "stg22/terrain/collision_208.inc"
};
static NJS_OBJECT collision_209 = {
#include "stg22/terrain/collision_209.inc"
};
static NJS_OBJECT collision_210 = {
#include "stg22/terrain/collision_210.inc"
};
static NJS_OBJECT collision_211 = {
#include "stg22/terrain/collision_211.inc"
};
static NJS_OBJECT collision_212 = {
#include "stg22/terrain/collision_212.inc"
};
static NJS_OBJECT collision_213 = {
#include "stg22/terrain/collision_213.inc"
};
static NJS_OBJECT collision_214 = {
#include "stg22/terrain/collision_214.inc"
};
static NJS_OBJECT collision_215 = {
#include "stg22/terrain/collision_215.inc"
};
static NJS_OBJECT collision_216 = {
#include "stg22/terrain/collision_216.inc"
};
static NJS_OBJECT collision_217 = {
#include "stg22/terrain/collision_217.inc"
};
static NJS_OBJECT collision_218 = {
#include "stg22/terrain/collision_218.inc"
};
static NJS_OBJECT collision_219 = {
#include "stg22/terrain/collision_219.inc"
};
static NJS_OBJECT collision_220 = {
#include "stg22/terrain/collision_220.inc"
};
static NJS_OBJECT collision_221 = {
#include "stg22/terrain/collision_221.inc"
};
static NJS_OBJECT collision_222 = {
#include "stg22/terrain/collision_222.inc"
};
static NJS_OBJECT collision_224 = {
#include "stg22/terrain/collision_224.inc"
};
static NJS_OBJECT collision_226 = {
#include "stg22/terrain/collision_226.inc"
};
static NJS_OBJECT collision_228 = {
#include "stg22/terrain/collision_228.inc"
};
static NJS_OBJECT collision_229 = {
#include "stg22/terrain/collision_229.inc"
};
static NJS_OBJECT collision_230 = {
#include "stg22/terrain/collision_230.inc"
};
static NJS_OBJECT collision_235 = {
#include "stg22/terrain/collision_235.inc"
};
static NJS_OBJECT collision_237 = {
#include "stg22/terrain/collision_237.inc"
};
static NJS_OBJECT collision_239 = {
#include "stg22/terrain/collision_239.inc"
};
static NJS_OBJECT collision_241 = {
#include "stg22/terrain/collision_241.inc"
};
static NJS_OBJECT collision_243 = {
#include "stg22/terrain/collision_243.inc"
};
static NJS_OBJECT collision_245 = {
#include "stg22/terrain/collision_245.inc"
};
static NJS_OBJECT collision_246 = {
#include "stg22/terrain/collision_246.inc"
};
static NJS_OBJECT collision_248 = {
#include "stg22/terrain/collision_248.inc"
};
static NJS_OBJECT collision_253 = {
#include "stg22/terrain/collision_253.inc"
};
static NJS_OBJECT collision_254 = {
#include "stg22/terrain/collision_254.inc"
};
static NJS_OBJECT collision_255 = {
#include "stg22/terrain/collision_255.inc"
};
static NJS_OBJECT collision_256 = {
#include "stg22/terrain/collision_256.inc"
};
static NJS_OBJECT collision_257 = {
#include "stg22/terrain/collision_257.inc"
};
static NJS_OBJECT collision_258 = {
#include "stg22/terrain/collision_258.inc"
};
static NJS_OBJECT collision_259 = {
#include "stg22/terrain/collision_259.inc"
};
static NJS_OBJECT collision_260 = {
#include "stg22/terrain/collision_260.inc"
};
static NJS_OBJECT collision_261 = {
#include "stg22/terrain/collision_261.inc"
};
static NJS_OBJECT collision_267 = {
#include "stg22/terrain/collision_267.inc"
};
static NJS_OBJECT collision_268 = {
#include "stg22/terrain/collision_268.inc"
};
static NJS_OBJECT collision_269 = {
#include "stg22/terrain/collision_269.inc"
};
static NJS_OBJECT collision_270 = {
#include "stg22/terrain/collision_270.inc"
};
static NJS_OBJECT collision_271 = {
#include "stg22/terrain/collision_271.inc"
};
static NJS_OBJECT collision_301 = {
#include "stg22/terrain/collision_301.inc"
};
static NJS_OBJECT collision_302 = {
#include "stg22/terrain/collision_302.inc"
};
static NJS_OBJECT collision_303 = {
#include "stg22/terrain/collision_303.inc"
};
static NJS_OBJECT collision_304 = {
#include "stg22/terrain/collision_304.inc"
};
