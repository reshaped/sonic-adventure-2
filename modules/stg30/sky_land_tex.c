#include <ninja.h>
#include <sg_xpt.h>

#include "collision/world.h"
#include "stg30/sky.h"
#include "stg30/sky_land.h"

/* the file names that rush_sky_land and its texture list point at. each
 * starts on a 4-byte boundary */
const char rush_sky_land_filenames[151] = {
#include "stg30/sky_land_tex/rush_sky_land_filenames.inc"
};

/* STG30.PRS's background land, texture lists and chunk models, from the
 * disc. its D lies between space_motion.c and terrain.c */

/* the data up to the last that a pointer names ahead of its definition, as
 * tentative definitions in the disc's order: GCC places each datum at its
 * first definition */
static struct land_entry rush_sky_land_entries[514];
struct land_table rush_sky_land;
static NJS_TEXNAME rush_sky_land_textures[12];
NJS_TEXLIST rush_sky_land_texlist;
static Sint16 rush_sky_land_draw_67_polygons[433];
static Sint32 rush_sky_land_draw_67_vertices[243];
static NJS_CNK_MODEL rush_sky_land_draw_67_model;
NJS_CNK_OBJECT rush_scenery_9;
static Sint16 rush_sky_land_draw_128_polygons[433];
static Sint32 rush_sky_land_draw_128_vertices[247];
static NJS_CNK_MODEL rush_sky_land_draw_128_model;
NJS_CNK_OBJECT rush_scenery_6;
static Sint16 rush_sky_land_draw_454_polygons[1033];
static Sint32 rush_sky_land_draw_454_vertices[643];
static NJS_CNK_MODEL rush_sky_land_draw_454_model;
NJS_CNK_OBJECT rush_scenery_8;
static Sint16 rush_sky_land_draw_68_polygons[577];
static Sint32 rush_sky_land_draw_68_vertices[339];
static NJS_CNK_MODEL rush_sky_land_draw_68_model;
NJS_CNK_OBJECT rush_scenery_7;
static Sint16 rush_sky_land_draw_72_polygons[145];
static Sint32 rush_sky_land_draw_72_vertices[51];
static NJS_CNK_MODEL rush_sky_land_draw_72_model;
NJS_CNK_OBJECT rush_scenery_5;
static Sint16 rush_sky_land_draw_5_polygons[49];
static Sint32 rush_sky_land_draw_5_vertices[51];
static NJS_CNK_MODEL rush_sky_land_draw_5_model;
NJS_CNK_OBJECT rush_scenery_1;
static Sint16 rush_sky_land_draw_3_polygons[73];
static Sint32 rush_sky_land_draw_3_vertices[67];
static NJS_CNK_MODEL rush_sky_land_draw_3_model;
NJS_CNK_OBJECT rush_scenery_2;
static Sint16 rush_sky_land_draw_0_polygons[29];
static Sint32 rush_sky_land_draw_0_vertices[27];
static NJS_CNK_MODEL rush_sky_land_draw_0_model;
NJS_CNK_OBJECT rush_scenery_3;
static Sint16 rush_sky_land_draw_1_polygons[145];
static Sint32 rush_sky_land_draw_1_vertices[67];
static NJS_CNK_MODEL rush_sky_land_draw_1_model;
NJS_CNK_OBJECT rush_scenery_52;
static Sint16 rush_sky_land_draw_52_polygons[197];
static Sint32 rush_sky_land_draw_52_vertices[99];
static NJS_CNK_MODEL rush_sky_land_draw_52_model;
NJS_CNK_OBJECT rush_scenery_11;
static Sint16 rush_sky_land_draw_89_polygons[563];
static Sint32 rush_sky_land_draw_89_vertices[331];
static NJS_CNK_MODEL rush_sky_land_draw_89_model;
NJS_CNK_OBJECT rush_scenery_4;
static Sint16 rush_sky_land_draw_459_polygons[601];
static Sint32 rush_sky_land_draw_459_vertices[355];
static NJS_CNK_MODEL rush_sky_land_draw_459_model;
NJS_CNK_OBJECT rush_scenery_14;
static Sint16 rush_sky_land_draw_451_polygons[673];
static Sint32 rush_sky_land_draw_451_vertices[403];
static NJS_CNK_MODEL rush_sky_land_draw_451_model;
NJS_CNK_OBJECT rush_scenery_10;
static Sint16 rush_sky_land_draw_235_polygons[667];
static Sint32 rush_sky_land_draw_235_vertices[431];
static NJS_CNK_MODEL rush_sky_land_draw_235_model;
NJS_CNK_OBJECT rush_scenery_17;
static Sint16 rush_sky_land_draw_399_polygons[363];
static Sint32 rush_sky_land_draw_399_vertices[199];
static NJS_CNK_MODEL rush_sky_land_draw_399_model;
NJS_CNK_OBJECT rush_scenery_18;
static Sint16 rush_sky_land_draw_417_polygons[805];
static Sint32 rush_sky_land_draw_417_vertices[499];
static NJS_CNK_MODEL rush_sky_land_draw_417_model;
NJS_CNK_OBJECT rush_scenery_21;
static Sint16 rush_sky_land_draw_416_polygons[121];
static Sint32 rush_sky_land_draw_416_vertices[51];
static NJS_CNK_MODEL rush_sky_land_draw_416_model;
NJS_CNK_OBJECT rush_scenery_54;
static Sint16 rush_sky_land_draw_415_polygons[121];
static Sint32 rush_sky_land_draw_415_vertices[51];
static NJS_CNK_MODEL rush_sky_land_draw_415_model;
NJS_CNK_OBJECT rush_scenery_56;
static Sint16 rush_sky_land_draw_414_polygons[315];
static Sint32 rush_sky_land_draw_414_vertices[191];
static NJS_CNK_MODEL rush_sky_land_draw_414_model;
NJS_CNK_OBJECT rush_scenery_19;
static Sint16 rush_sky_land_draw_413_polygons[269];
static Sint32 rush_sky_land_draw_413_vertices[167];
static NJS_CNK_MODEL rush_sky_land_draw_413_model;
NJS_CNK_OBJECT rush_scenery_15;
static Sint16 rush_sky_land_draw_411_polygons[287];
static Sint32 rush_sky_land_draw_411_vertices[179];
static NJS_CNK_MODEL rush_sky_land_draw_411_model;
NJS_CNK_OBJECT rush_scenery_16;
static Sint16 rush_sky_land_draw_409_polygons[107];
static Sint32 rush_sky_land_draw_409_vertices[51];
static NJS_CNK_MODEL rush_sky_land_draw_409_model;
NJS_CNK_OBJECT rush_scenery_53;
static Sint16 rush_sky_land_draw_403_polygons[255];
static Sint32 rush_sky_land_draw_403_vertices[159];
static NJS_CNK_MODEL rush_sky_land_draw_403_model;
NJS_CNK_OBJECT rush_scenery_13;
static Sint16 rush_sky_land_draw_400_polygons[905];
static Sint32 rush_sky_land_draw_400_vertices[587];
static NJS_CNK_MODEL rush_sky_land_draw_400_model;
NJS_CNK_OBJECT rush_scenery_12;
static Sint16 rush_sky_land_draw_395_polygons[121];
static Sint32 rush_sky_land_draw_395_vertices[51];
static NJS_CNK_MODEL rush_sky_land_draw_395_model;
NJS_CNK_OBJECT rush_scenery_55;
static Sint16 rush_sky_land_draw_6_polygons[255];
static Sint32 rush_sky_land_draw_6_vertices[159];
static NJS_CNK_MODEL rush_sky_land_draw_6_model;
NJS_CNK_OBJECT rush_scenery_20;
static Sint16 rush_sky_land_draw_138_polygons[483];
static Sint32 rush_sky_land_draw_138_vertices[307];
static NJS_CNK_MODEL rush_sky_land_draw_138_model;
NJS_CNK_OBJECT rush_scenery_22;
static Sint16 rush_sky_land_draw_137_polygons[491];
static Sint32 rush_sky_land_draw_137_vertices[311];
static NJS_CNK_MODEL rush_sky_land_draw_137_model;
NJS_CNK_OBJECT rush_scenery_23;
static Sint16 rush_sky_land_draw_136_polygons[1259];
static Sint32 rush_sky_land_draw_136_vertices[827];
static NJS_CNK_MODEL rush_sky_land_draw_136_model;
NJS_CNK_OBJECT rush_scenery_24;
static Sint16 rush_sky_land_draw_371_polygons[273];
static Sint32 rush_sky_land_draw_371_vertices[147];
static NJS_CNK_MODEL rush_sky_land_draw_371_model;
NJS_CNK_OBJECT rush_scenery_0;
static Sint16 rush_sky_land_draw_73_polygons[197];
static Sint32 rush_sky_land_draw_73_vertices[99];
static NJS_CNK_MODEL rush_sky_land_draw_73_model;
NJS_CNK_OBJECT rush_scenery_26;
static Sint16 rush_sky_land_draw_348_polygons[927];
static Sint32 rush_sky_land_draw_348_vertices[587];
static NJS_CNK_MODEL rush_sky_land_draw_348_model;
NJS_CNK_OBJECT rush_scenery_25;
static Sint16 rush_sky_land_draw_347_polygons[297];
static Sint32 rush_sky_land_draw_347_vertices[175];
static NJS_CNK_MODEL rush_sky_land_draw_347_model;
NJS_CNK_OBJECT rush_scenery_27;
static Sint16 rush_sky_land_draw_327_polygons[309];
static Sint32 rush_sky_land_draw_327_vertices[195];
static NJS_CNK_MODEL rush_sky_land_draw_327_model;
NJS_CNK_OBJECT rush_scenery_28;
static Sint16 rush_sky_land_draw_12_polygons[219];
static Sint32 rush_sky_land_draw_12_vertices[99];
static NJS_CNK_MODEL rush_sky_land_draw_12_model;
NJS_CNK_OBJECT rush_scenery_29;
static Sint16 rush_sky_land_draw_293_polygons[87];
static Sint32 rush_sky_land_draw_293_vertices[51];
static NJS_CNK_MODEL rush_sky_land_draw_293_model;
NJS_CNK_OBJECT rush_scenery_30;
static Sint16 rush_sky_land_draw_186_polygons[289];
static Sint32 rush_sky_land_draw_186_vertices[147];
static NJS_CNK_MODEL rush_sky_land_draw_186_model;
NJS_CNK_OBJECT rush_scenery_32;
static Sint16 rush_sky_land_draw_287_polygons[697];
static Sint32 rush_sky_land_draw_287_vertices[419];
static NJS_CNK_MODEL rush_sky_land_draw_287_model;
NJS_CNK_OBJECT rush_scenery_31;
static Sint16 rush_sky_land_draw_231_polygons[271];
static Sint32 rush_sky_land_draw_231_vertices[243];
static NJS_CNK_MODEL rush_sky_land_draw_231_model;
NJS_CNK_OBJECT rush_scenery_33;
static Sint16 rush_sky_land_draw_242_polygons[269];
static Sint32 rush_sky_land_draw_242_vertices[147];
static NJS_CNK_MODEL rush_sky_land_draw_242_model;
NJS_CNK_OBJECT rush_scenery_34;
static Sint16 rush_sky_land_draw_71_polygons[563];
static Sint32 rush_sky_land_draw_71_vertices[343];
static NJS_CNK_MODEL rush_sky_land_draw_71_model;
NJS_CNK_OBJECT rush_scenery_36;
static Sint16 rush_sky_land_draw_195_polygons[529];
static Sint32 rush_sky_land_draw_195_vertices[307];
static NJS_CNK_MODEL rush_sky_land_draw_195_model;
NJS_CNK_OBJECT rush_scenery_35;
static Sint16 rush_sky_land_draw_193_polygons[113];
static Sint32 rush_sky_land_draw_193_vertices[35];
static NJS_CNK_MODEL rush_sky_land_draw_193_model;
NJS_CNK_OBJECT rush_scenery_37;
static Sint16 rush_sky_land_draw_192_polygons[1159];
static Sint32 rush_sky_land_draw_192_vertices[727];
static NJS_CNK_MODEL rush_sky_land_draw_192_model;
NJS_CNK_OBJECT rush_scenery_38;
static Sint16 rush_sky_land_draw_191_polygons[385];
static Sint32 rush_sky_land_draw_191_vertices[215];
static NJS_CNK_MODEL rush_sky_land_draw_191_model;
NJS_CNK_OBJECT rush_scenery_39;
static Sint16 rush_sky_land_draw_190_polygons[677];
static Sint32 rush_sky_land_draw_190_vertices[419];
static NJS_CNK_MODEL rush_sky_land_draw_190_model;
NJS_CNK_OBJECT rush_scenery_41;
static Sint16 rush_sky_land_draw_70_polygons[1025];
static Sint32 rush_sky_land_draw_70_vertices[635];
static NJS_CNK_MODEL rush_sky_land_draw_70_model;
NJS_CNK_OBJECT rush_scenery_40;
static Sint16 rush_sky_land_draw_117_polygons[635];
static Sint32 rush_sky_land_draw_117_vertices[391];
static NJS_CNK_MODEL rush_sky_land_draw_117_model;
NJS_CNK_OBJECT rush_scenery_42;
static Sint16 rush_sky_land_draw_170_polygons[673];
static Sint32 rush_sky_land_draw_170_vertices[415];
static NJS_CNK_MODEL rush_sky_land_draw_170_model;
NJS_CNK_OBJECT rush_scenery_44;
static Sint16 rush_sky_land_draw_94_polygons[1085];
static Sint32 rush_sky_land_draw_94_vertices[687];
static NJS_CNK_MODEL rush_sky_land_draw_94_model;
NJS_CNK_OBJECT rush_scenery_43;
static Sint16 rush_sky_land_draw_146_polygons[535];
static Sint32 rush_sky_land_draw_146_vertices[311];
static NJS_CNK_MODEL rush_sky_land_draw_146_model;
NJS_CNK_OBJECT rush_scenery_45;
static Sint16 rush_sky_land_draw_134_polygons[273];
static Sint32 rush_sky_land_draw_134_vertices[147];
static NJS_CNK_MODEL rush_sky_land_draw_134_model;
static NJS_CNK_OBJECT rush_sky_land_draw_134;
static Sint16 rush_sky_land_draw_129_polygons[1019];
static Sint32 rush_sky_land_draw_129_vertices[647];
static NJS_CNK_MODEL rush_sky_land_draw_129_model;
NJS_CNK_OBJECT rush_scenery_46;
static Sint16 rush_sky_land_draw_127_polygons[197];
static Sint32 rush_sky_land_draw_127_vertices[99];
static NJS_CNK_MODEL rush_sky_land_draw_127_model;
NJS_CNK_OBJECT rush_scenery_51;
static Sint16 rush_sky_land_draw_66_polygons[275];
static Sint32 rush_sky_land_draw_66_vertices[171];
static NJS_CNK_MODEL rush_sky_land_draw_66_model;
NJS_CNK_OBJECT rush_scenery_48;
static Sint16 rush_sky_land_draw_65_polygons[317];
static Sint32 rush_sky_land_draw_65_vertices[183];
static NJS_CNK_MODEL rush_sky_land_draw_65_model;
NJS_CNK_OBJECT rush_scenery_49;
static Sint16 rush_sky_land_draw_64_polygons[261];
static Sint32 rush_sky_land_draw_64_vertices[163];
static NJS_CNK_MODEL rush_sky_land_draw_64_model;
NJS_CNK_OBJECT rush_scenery_47;
static Sint16 rush_sky_land_draw_63_polygons[315];
static Sint32 rush_sky_land_draw_63_vertices[199];
static NJS_CNK_MODEL rush_sky_land_draw_63_model;
NJS_CNK_OBJECT rush_scenery_50;
static NJS_CNK_OBJECT rush_sky_land_draw_503;
static NJS_CNK_OBJECT rush_sky_land_draw_502;
static NJS_CNK_OBJECT rush_sky_land_draw_501;
static NJS_CNK_OBJECT rush_sky_land_draw_500;
static NJS_CNK_OBJECT rush_sky_land_draw_499;
static NJS_CNK_OBJECT rush_sky_land_draw_498;
static NJS_CNK_OBJECT rush_sky_land_draw_497;
static NJS_CNK_OBJECT rush_sky_land_draw_496;
static NJS_CNK_OBJECT rush_sky_land_draw_495;
static NJS_CNK_OBJECT rush_sky_land_draw_494;
static NJS_CNK_OBJECT rush_sky_land_draw_493;
static NJS_CNK_OBJECT rush_sky_land_draw_492;
static NJS_CNK_OBJECT rush_sky_land_draw_491;
static NJS_CNK_OBJECT rush_sky_land_draw_490;
static NJS_CNK_OBJECT rush_sky_land_draw_489;
static NJS_CNK_OBJECT rush_sky_land_draw_488;
static NJS_CNK_OBJECT rush_sky_land_draw_487;
static NJS_CNK_OBJECT rush_sky_land_draw_486;
static NJS_CNK_OBJECT rush_sky_land_draw_485;
static NJS_CNK_OBJECT rush_sky_land_draw_484;
static NJS_CNK_OBJECT rush_sky_land_draw_483;
static NJS_CNK_OBJECT rush_sky_land_draw_481;
static NJS_CNK_OBJECT rush_sky_land_draw_480;
static NJS_CNK_OBJECT rush_sky_land_draw_479;
static NJS_CNK_OBJECT rush_sky_land_draw_478;
static NJS_CNK_OBJECT rush_sky_land_draw_477;
static NJS_CNK_OBJECT rush_sky_land_draw_476;
static NJS_CNK_OBJECT rush_sky_land_draw_475;
static NJS_CNK_OBJECT rush_sky_land_draw_474;
static NJS_CNK_OBJECT rush_sky_land_draw_473;
static NJS_CNK_OBJECT rush_sky_land_draw_472;
static NJS_CNK_OBJECT rush_sky_land_draw_471;
static NJS_CNK_OBJECT rush_sky_land_draw_470;
static NJS_CNK_OBJECT rush_sky_land_draw_469;
static NJS_CNK_OBJECT rush_sky_land_draw_468;
static NJS_CNK_OBJECT rush_sky_land_draw_467;
static NJS_CNK_OBJECT rush_sky_land_draw_466;
static NJS_CNK_OBJECT rush_sky_land_draw_465;
static NJS_CNK_OBJECT rush_sky_land_draw_464;
static NJS_CNK_OBJECT rush_sky_land_draw_463;
static NJS_CNK_OBJECT rush_sky_land_draw_462;
static NJS_CNK_OBJECT rush_sky_land_draw_461;
static NJS_CNK_OBJECT rush_sky_land_draw_460;
static NJS_CNK_OBJECT rush_sky_land_draw_458;
static NJS_CNK_OBJECT rush_sky_land_draw_457;
static NJS_CNK_OBJECT rush_sky_land_draw_456;
static NJS_CNK_OBJECT rush_sky_land_draw_455;
static NJS_CNK_OBJECT rush_sky_land_draw_454;
static NJS_CNK_OBJECT rush_sky_land_draw_453;
static NJS_CNK_OBJECT rush_sky_land_draw_452;
static NJS_CNK_OBJECT rush_sky_land_draw_450;
static NJS_CNK_OBJECT rush_sky_land_draw_449;
static NJS_CNK_OBJECT rush_sky_land_draw_448;
static NJS_CNK_OBJECT rush_sky_land_draw_447;
static NJS_CNK_OBJECT rush_sky_land_draw_446;
static NJS_CNK_OBJECT rush_sky_land_draw_445;
static NJS_CNK_OBJECT rush_sky_land_draw_444;
static NJS_CNK_OBJECT rush_sky_land_draw_443;
static NJS_CNK_OBJECT rush_sky_land_draw_442;
static NJS_CNK_OBJECT rush_sky_land_draw_441;
static NJS_CNK_OBJECT rush_sky_land_draw_440;
static NJS_CNK_OBJECT rush_sky_land_draw_439;
static NJS_CNK_OBJECT rush_sky_land_draw_438;
static NJS_CNK_OBJECT rush_sky_land_draw_437;
static NJS_CNK_OBJECT rush_sky_land_draw_436;
static NJS_CNK_OBJECT rush_sky_land_draw_435;
static NJS_CNK_OBJECT rush_sky_land_draw_434;
static NJS_CNK_OBJECT rush_sky_land_draw_433;
static NJS_CNK_OBJECT rush_sky_land_draw_432;
static NJS_CNK_OBJECT rush_sky_land_draw_431;
static NJS_CNK_OBJECT rush_sky_land_draw_430;
static NJS_CNK_OBJECT rush_sky_land_draw_429;
static NJS_CNK_OBJECT rush_sky_land_draw_428;
static NJS_CNK_OBJECT rush_sky_land_draw_427;
static NJS_CNK_OBJECT rush_sky_land_draw_426;
static NJS_CNK_OBJECT rush_sky_land_draw_425;
static NJS_CNK_OBJECT rush_sky_land_draw_424;
static NJS_CNK_OBJECT rush_sky_land_draw_423;
static NJS_CNK_OBJECT rush_sky_land_draw_422;
static NJS_CNK_OBJECT rush_sky_land_draw_421;
static NJS_CNK_OBJECT rush_sky_land_draw_420;
static NJS_CNK_OBJECT rush_sky_land_draw_412;
static NJS_CNK_OBJECT rush_sky_land_draw_410;
static NJS_CNK_OBJECT rush_sky_land_draw_408;
static NJS_CNK_OBJECT rush_sky_land_draw_406;
static NJS_CNK_OBJECT rush_sky_land_draw_405;
static NJS_CNK_OBJECT rush_sky_land_draw_404;
static NJS_CNK_OBJECT rush_sky_land_draw_403;
static NJS_CNK_OBJECT rush_sky_land_draw_402;
static NJS_CNK_OBJECT rush_sky_land_draw_401;
static NJS_CNK_OBJECT rush_sky_land_draw_399;
static NJS_CNK_OBJECT rush_sky_land_draw_398;
static NJS_CNK_OBJECT rush_sky_land_draw_397;
static NJS_CNK_OBJECT rush_sky_land_draw_396;
static NJS_CNK_OBJECT rush_sky_land_draw_394;
static NJS_CNK_OBJECT rush_sky_land_draw_393;
static NJS_CNK_OBJECT rush_sky_land_draw_390;
static NJS_CNK_OBJECT rush_sky_land_draw_389;
static NJS_CNK_OBJECT rush_sky_land_draw_388;
static NJS_CNK_OBJECT rush_sky_land_draw_386;
static NJS_CNK_OBJECT rush_sky_land_draw_385;
static NJS_CNK_OBJECT rush_sky_land_draw_384;
static NJS_CNK_OBJECT rush_sky_land_draw_383;
static NJS_CNK_OBJECT rush_sky_land_draw_382;
static NJS_CNK_OBJECT rush_sky_land_draw_380;
static NJS_CNK_OBJECT rush_sky_land_draw_379;
static NJS_CNK_OBJECT rush_sky_land_draw_378;
static NJS_CNK_OBJECT rush_sky_land_draw_377;
static NJS_CNK_OBJECT rush_sky_land_draw_376;
static NJS_CNK_OBJECT rush_sky_land_draw_375;
static NJS_CNK_OBJECT rush_sky_land_draw_374;
static NJS_CNK_OBJECT rush_sky_land_draw_373;
static NJS_CNK_OBJECT rush_sky_land_draw_372;
static NJS_CNK_OBJECT rush_sky_land_draw_370;
static NJS_CNK_OBJECT rush_sky_land_draw_369;
static NJS_CNK_OBJECT rush_sky_land_draw_368;
static NJS_CNK_OBJECT rush_sky_land_draw_367;
static NJS_CNK_OBJECT rush_sky_land_draw_366;
static NJS_CNK_OBJECT rush_sky_land_draw_365;
static NJS_CNK_OBJECT rush_sky_land_draw_364;
static NJS_CNK_OBJECT rush_sky_land_draw_363;
static NJS_CNK_OBJECT rush_sky_land_draw_362;
static NJS_CNK_OBJECT rush_sky_land_draw_361;
static NJS_CNK_OBJECT rush_sky_land_draw_360;
static NJS_CNK_OBJECT rush_sky_land_draw_359;
static NJS_CNK_OBJECT rush_sky_land_draw_358;
static NJS_CNK_OBJECT rush_sky_land_draw_357;
static NJS_CNK_OBJECT rush_sky_land_draw_356;
static NJS_CNK_OBJECT rush_sky_land_draw_355;
static NJS_CNK_OBJECT rush_sky_land_draw_354;
static NJS_CNK_OBJECT rush_sky_land_draw_353;
static NJS_CNK_OBJECT rush_sky_land_draw_352;
static NJS_CNK_OBJECT rush_sky_land_draw_350;
static NJS_CNK_OBJECT rush_sky_land_draw_349;
static NJS_CNK_OBJECT rush_sky_land_draw_346;
static NJS_CNK_OBJECT rush_sky_land_draw_345;
static NJS_CNK_OBJECT rush_sky_land_draw_344;
static NJS_CNK_OBJECT rush_sky_land_draw_343;
static NJS_CNK_OBJECT rush_sky_land_draw_342;
static NJS_CNK_OBJECT rush_sky_land_draw_341;
static NJS_CNK_OBJECT rush_sky_land_draw_340;
static NJS_CNK_OBJECT rush_sky_land_draw_339;
static NJS_CNK_OBJECT rush_sky_land_draw_338;
static NJS_CNK_OBJECT rush_sky_land_draw_337;
static NJS_CNK_OBJECT rush_sky_land_draw_336;
static NJS_CNK_OBJECT rush_sky_land_draw_335;
static NJS_CNK_OBJECT rush_sky_land_draw_334;
static NJS_CNK_OBJECT rush_sky_land_draw_333;
static NJS_CNK_OBJECT rush_sky_land_draw_332;
static NJS_CNK_OBJECT rush_sky_land_draw_331;
static NJS_CNK_OBJECT rush_sky_land_draw_330;
static NJS_CNK_OBJECT rush_sky_land_draw_329;
static NJS_CNK_OBJECT rush_sky_land_draw_328;
static NJS_CNK_OBJECT rush_sky_land_draw_326;
static NJS_CNK_OBJECT rush_sky_land_draw_325;
static NJS_CNK_OBJECT rush_sky_land_draw_324;
static NJS_CNK_OBJECT rush_sky_land_draw_323;
static NJS_CNK_OBJECT rush_sky_land_draw_322;
static NJS_CNK_OBJECT rush_sky_land_draw_321;
static NJS_CNK_OBJECT rush_sky_land_draw_320;
static NJS_CNK_OBJECT rush_sky_land_draw_319;
static NJS_CNK_OBJECT rush_sky_land_draw_318;
static NJS_CNK_OBJECT rush_sky_land_draw_317;
static NJS_CNK_OBJECT rush_sky_land_draw_316;
static NJS_CNK_OBJECT rush_sky_land_draw_315;
static NJS_CNK_OBJECT rush_sky_land_draw_314;
static NJS_CNK_OBJECT rush_sky_land_draw_313;
static NJS_CNK_OBJECT rush_sky_land_draw_312;
static NJS_CNK_OBJECT rush_sky_land_draw_311;
static NJS_CNK_OBJECT rush_sky_land_draw_310;
static NJS_CNK_OBJECT rush_sky_land_draw_309;
static NJS_CNK_OBJECT rush_sky_land_draw_308;
static NJS_CNK_OBJECT rush_sky_land_draw_307;
static NJS_CNK_OBJECT rush_sky_land_draw_306;
static NJS_CNK_OBJECT rush_sky_land_draw_305;
static NJS_CNK_OBJECT rush_sky_land_draw_304;
static NJS_CNK_OBJECT rush_sky_land_draw_303;
static NJS_CNK_OBJECT rush_sky_land_draw_302;
static NJS_CNK_OBJECT rush_sky_land_draw_301;
static NJS_CNK_OBJECT rush_sky_land_draw_300;
static NJS_CNK_OBJECT rush_sky_land_draw_299;
static NJS_CNK_OBJECT rush_sky_land_draw_298;
static NJS_CNK_OBJECT rush_sky_land_draw_297;
static NJS_CNK_OBJECT rush_sky_land_draw_296;
static NJS_CNK_OBJECT rush_sky_land_draw_295;
static NJS_CNK_OBJECT rush_sky_land_draw_292;
static NJS_CNK_OBJECT rush_sky_land_draw_290;
static NJS_CNK_OBJECT rush_sky_land_draw_289;
static NJS_CNK_OBJECT rush_sky_land_draw_288;
static NJS_CNK_OBJECT rush_sky_land_draw_286;
static NJS_CNK_OBJECT rush_sky_land_draw_285;
static NJS_CNK_OBJECT rush_sky_land_draw_284;
static NJS_CNK_OBJECT rush_sky_land_draw_282;
static NJS_CNK_OBJECT rush_sky_land_draw_281;
static NJS_CNK_OBJECT rush_sky_land_draw_280;
static NJS_CNK_OBJECT rush_sky_land_draw_279;
static NJS_CNK_OBJECT rush_sky_land_draw_278;
static NJS_CNK_OBJECT rush_sky_land_draw_277;
static NJS_CNK_OBJECT rush_sky_land_draw_276;
static NJS_CNK_OBJECT rush_sky_land_draw_275;
static NJS_CNK_OBJECT rush_sky_land_draw_274;
static NJS_CNK_OBJECT rush_sky_land_draw_273;
static NJS_CNK_OBJECT rush_sky_land_draw_272;
static NJS_CNK_OBJECT rush_sky_land_draw_271;
static NJS_CNK_OBJECT rush_sky_land_draw_270;
static NJS_CNK_OBJECT rush_sky_land_draw_269;
static NJS_CNK_OBJECT rush_sky_land_draw_268;
static NJS_CNK_OBJECT rush_sky_land_draw_267;
static NJS_CNK_OBJECT rush_sky_land_draw_266;
static NJS_CNK_OBJECT rush_sky_land_draw_265;
static NJS_CNK_OBJECT rush_sky_land_draw_264;
static NJS_CNK_OBJECT rush_sky_land_draw_263;
static NJS_CNK_OBJECT rush_sky_land_draw_262;
static NJS_CNK_OBJECT rush_sky_land_draw_261;
static NJS_CNK_OBJECT rush_sky_land_draw_260;
static NJS_CNK_OBJECT rush_sky_land_draw_259;
static NJS_CNK_OBJECT rush_sky_land_draw_258;
static NJS_CNK_OBJECT rush_sky_land_draw_257;
static NJS_CNK_OBJECT rush_sky_land_draw_256;
static NJS_CNK_OBJECT rush_sky_land_draw_255;
static NJS_CNK_OBJECT rush_sky_land_draw_254;
static NJS_CNK_OBJECT rush_sky_land_draw_253;
static NJS_CNK_OBJECT rush_sky_land_draw_252;
static NJS_CNK_OBJECT rush_sky_land_draw_251;
static NJS_CNK_OBJECT rush_sky_land_draw_250;
static NJS_CNK_OBJECT rush_sky_land_draw_249;
static NJS_CNK_OBJECT rush_sky_land_draw_248;
static NJS_CNK_OBJECT rush_sky_land_draw_247;
static NJS_CNK_OBJECT rush_sky_land_draw_246;
static NJS_CNK_OBJECT rush_sky_land_draw_245;
static NJS_CNK_OBJECT rush_sky_land_draw_244;
static NJS_CNK_OBJECT rush_sky_land_draw_243;
static NJS_CNK_OBJECT rush_sky_land_draw_241;
static NJS_CNK_OBJECT rush_sky_land_draw_240;
static NJS_CNK_OBJECT rush_sky_land_draw_239;
static NJS_CNK_OBJECT rush_sky_land_draw_238;
static NJS_CNK_OBJECT rush_sky_land_draw_237;
static NJS_CNK_OBJECT rush_sky_land_draw_236;
static NJS_CNK_OBJECT rush_sky_land_draw_235;
static NJS_CNK_OBJECT rush_sky_land_draw_234;
static NJS_CNK_OBJECT rush_sky_land_draw_233;
static NJS_CNK_OBJECT rush_sky_land_draw_232;
static NJS_CNK_OBJECT rush_sky_land_draw_231;
static NJS_CNK_OBJECT rush_sky_land_draw_230;
static NJS_CNK_OBJECT rush_sky_land_draw_229;
static NJS_CNK_OBJECT rush_sky_land_draw_228;
static NJS_CNK_OBJECT rush_sky_land_draw_227;
static NJS_CNK_OBJECT rush_sky_land_draw_226;
static NJS_CNK_OBJECT rush_sky_land_draw_225;
static NJS_CNK_OBJECT rush_sky_land_draw_224;
static NJS_CNK_OBJECT rush_sky_land_draw_223;
static NJS_CNK_OBJECT rush_sky_land_draw_222;
static NJS_CNK_OBJECT rush_sky_land_draw_221;
static NJS_CNK_OBJECT rush_sky_land_draw_220;
static NJS_CNK_OBJECT rush_sky_land_draw_219;
static NJS_CNK_OBJECT rush_sky_land_draw_218;
static NJS_CNK_OBJECT rush_sky_land_draw_217;
static NJS_CNK_OBJECT rush_sky_land_draw_216;
static NJS_CNK_OBJECT rush_sky_land_draw_215;
static NJS_CNK_OBJECT rush_sky_land_draw_214;
static NJS_CNK_OBJECT rush_sky_land_draw_213;
static NJS_CNK_OBJECT rush_sky_land_draw_212;
static NJS_CNK_OBJECT rush_sky_land_draw_211;
static NJS_CNK_OBJECT rush_sky_land_draw_210;
static NJS_CNK_OBJECT rush_sky_land_draw_209;
static NJS_CNK_OBJECT rush_sky_land_draw_208;
static NJS_CNK_OBJECT rush_sky_land_draw_206;
static NJS_CNK_OBJECT rush_sky_land_draw_205;
static NJS_CNK_OBJECT rush_sky_land_draw_204;
static NJS_CNK_OBJECT rush_sky_land_draw_203;
static NJS_CNK_OBJECT rush_sky_land_draw_202;
static NJS_CNK_OBJECT rush_sky_land_draw_201;
static NJS_CNK_OBJECT rush_sky_land_draw_200;
static NJS_CNK_OBJECT rush_sky_land_draw_199;
static NJS_CNK_OBJECT rush_sky_land_draw_198;
static NJS_CNK_OBJECT rush_sky_land_draw_197;
static NJS_CNK_OBJECT rush_sky_land_draw_196;
static NJS_CNK_OBJECT rush_sky_land_draw_194;
static NJS_CNK_OBJECT rush_sky_land_draw_189;
static NJS_CNK_OBJECT rush_sky_land_draw_188;
static NJS_CNK_OBJECT rush_sky_land_draw_187;
static NJS_CNK_OBJECT rush_sky_land_draw_186;
static NJS_CNK_OBJECT rush_sky_land_draw_185;
static NJS_CNK_OBJECT rush_sky_land_draw_184;
static NJS_CNK_OBJECT rush_sky_land_draw_183;
static NJS_CNK_OBJECT rush_sky_land_draw_182;
static NJS_CNK_OBJECT rush_sky_land_draw_181;
static NJS_CNK_OBJECT rush_sky_land_draw_180;
static NJS_CNK_OBJECT rush_sky_land_draw_179;
static NJS_CNK_OBJECT rush_sky_land_draw_178;
static NJS_CNK_OBJECT rush_sky_land_draw_177;
static NJS_CNK_OBJECT rush_sky_land_draw_176;
static NJS_CNK_OBJECT rush_sky_land_draw_173;
static NJS_CNK_OBJECT rush_sky_land_draw_172;
static NJS_CNK_OBJECT rush_sky_land_draw_171;
static NJS_CNK_OBJECT rush_sky_land_draw_169;
static NJS_CNK_OBJECT rush_sky_land_draw_168;
static NJS_CNK_OBJECT rush_sky_land_draw_167;
static NJS_CNK_OBJECT rush_sky_land_draw_166;
static NJS_CNK_OBJECT rush_sky_land_draw_165;
static NJS_CNK_OBJECT rush_sky_land_draw_164;
static NJS_CNK_OBJECT rush_sky_land_draw_163;
static NJS_CNK_OBJECT rush_sky_land_draw_162;
static NJS_CNK_OBJECT rush_sky_land_draw_161;
static NJS_CNK_OBJECT rush_sky_land_draw_160;
static NJS_CNK_OBJECT rush_sky_land_draw_159;
static NJS_CNK_OBJECT rush_sky_land_draw_158;
static NJS_CNK_OBJECT rush_sky_land_draw_157;
static NJS_CNK_OBJECT rush_sky_land_draw_156;
static NJS_CNK_OBJECT rush_sky_land_draw_155;
static NJS_CNK_OBJECT rush_sky_land_draw_154;
static NJS_CNK_OBJECT rush_sky_land_draw_153;
static NJS_CNK_OBJECT rush_sky_land_draw_152;
static NJS_CNK_OBJECT rush_sky_land_draw_150;
static NJS_CNK_OBJECT rush_sky_land_draw_149;
static NJS_CNK_OBJECT rush_sky_land_draw_148;
static NJS_CNK_OBJECT rush_sky_land_draw_147;
static NJS_CNK_OBJECT rush_sky_land_draw_145;
static NJS_CNK_OBJECT rush_sky_land_draw_144;
static NJS_CNK_OBJECT rush_sky_land_draw_143;
static NJS_CNK_OBJECT rush_sky_land_draw_142;
static NJS_CNK_OBJECT rush_sky_land_draw_141;
static NJS_CNK_OBJECT rush_sky_land_draw_140;
static NJS_CNK_OBJECT rush_sky_land_draw_139;
static NJS_CNK_OBJECT rush_sky_land_draw_138;
static NJS_CNK_OBJECT rush_sky_land_draw_137;
static NJS_CNK_OBJECT rush_sky_land_draw_136;
static NJS_CNK_OBJECT rush_sky_land_draw_135;
static NJS_CNK_OBJECT rush_sky_land_draw_133;
static NJS_CNK_OBJECT rush_sky_land_draw_132;
static NJS_CNK_OBJECT rush_sky_land_draw_131;
static NJS_CNK_OBJECT rush_sky_land_draw_130;
static NJS_CNK_OBJECT rush_sky_land_draw_128;
static NJS_CNK_OBJECT rush_sky_land_draw_126;
static NJS_CNK_OBJECT rush_sky_land_draw_125;
static NJS_CNK_OBJECT rush_sky_land_draw_124;
static NJS_CNK_OBJECT rush_sky_land_draw_123;
static NJS_CNK_OBJECT rush_sky_land_draw_122;
static NJS_CNK_OBJECT rush_sky_land_draw_121;
static NJS_CNK_OBJECT rush_sky_land_draw_120;
static NJS_CNK_OBJECT rush_sky_land_draw_119;
static NJS_CNK_OBJECT rush_sky_land_draw_118;
static NJS_CNK_OBJECT rush_sky_land_draw_117;
static NJS_CNK_OBJECT rush_sky_land_draw_116;
static NJS_CNK_OBJECT rush_sky_land_draw_115;
static NJS_CNK_OBJECT rush_sky_land_draw_114;
static NJS_CNK_OBJECT rush_sky_land_draw_113;
static NJS_CNK_OBJECT rush_sky_land_draw_112;
static NJS_CNK_OBJECT rush_sky_land_draw_111;
static NJS_CNK_OBJECT rush_sky_land_draw_110;
static NJS_CNK_OBJECT rush_sky_land_draw_109;
static NJS_CNK_OBJECT rush_sky_land_draw_108;
static NJS_CNK_OBJECT rush_sky_land_draw_107;
static NJS_CNK_OBJECT rush_sky_land_draw_106;
static NJS_CNK_OBJECT rush_sky_land_draw_105;
static NJS_CNK_OBJECT rush_sky_land_draw_104;
static NJS_CNK_OBJECT rush_sky_land_draw_103;
static NJS_CNK_OBJECT rush_sky_land_draw_102;
static NJS_CNK_OBJECT rush_sky_land_draw_101;
static NJS_CNK_OBJECT rush_sky_land_draw_100;
static NJS_CNK_OBJECT rush_sky_land_draw_99;
static NJS_CNK_OBJECT rush_sky_land_draw_98;
static NJS_CNK_OBJECT rush_sky_land_draw_97;
static NJS_CNK_OBJECT rush_sky_land_draw_96;
static NJS_CNK_OBJECT rush_sky_land_draw_95;
static NJS_CNK_OBJECT rush_sky_land_draw_94;
static NJS_CNK_OBJECT rush_sky_land_draw_93;
static NJS_CNK_OBJECT rush_sky_land_draw_92;
static NJS_CNK_OBJECT rush_sky_land_draw_91;
static NJS_CNK_OBJECT rush_sky_land_draw_90;
static NJS_CNK_OBJECT rush_sky_land_draw_89;
static NJS_CNK_OBJECT rush_sky_land_draw_88;
static NJS_CNK_OBJECT rush_sky_land_draw_87;
static NJS_CNK_OBJECT rush_sky_land_draw_86;
static NJS_CNK_OBJECT rush_sky_land_draw_85;
static NJS_CNK_OBJECT rush_sky_land_draw_84;
static NJS_CNK_OBJECT rush_sky_land_draw_83;
static NJS_CNK_OBJECT rush_sky_land_draw_82;
static NJS_CNK_OBJECT rush_sky_land_draw_81;
static NJS_CNK_OBJECT rush_sky_land_draw_80;
static NJS_CNK_OBJECT rush_sky_land_draw_79;
static NJS_CNK_OBJECT rush_sky_land_draw_78;
static NJS_CNK_OBJECT rush_sky_land_draw_77;
static NJS_CNK_OBJECT rush_sky_land_draw_76;
static NJS_CNK_OBJECT rush_sky_land_draw_75;
static NJS_CNK_OBJECT rush_sky_land_draw_74;
static NJS_CNK_OBJECT rush_sky_land_draw_73;
static NJS_CNK_OBJECT rush_sky_land_draw_72;
static NJS_CNK_OBJECT rush_sky_land_draw_71;
static NJS_CNK_OBJECT rush_sky_land_draw_70;
static NJS_CNK_OBJECT rush_sky_land_draw_69;
static NJS_CNK_OBJECT rush_sky_land_draw_68;
static NJS_CNK_OBJECT rush_sky_land_draw_67;
static NJS_CNK_OBJECT rush_sky_land_draw_62;
static NJS_CNK_OBJECT rush_sky_land_draw_61;
static NJS_CNK_OBJECT rush_sky_land_draw_60;
static NJS_CNK_OBJECT rush_sky_land_draw_59;
static NJS_CNK_OBJECT rush_sky_land_draw_58;
static NJS_CNK_OBJECT rush_sky_land_draw_57;
static NJS_CNK_OBJECT rush_sky_land_draw_56;
static NJS_CNK_OBJECT rush_sky_land_draw_55;
static NJS_CNK_OBJECT rush_sky_land_draw_54;
static NJS_CNK_OBJECT rush_sky_land_draw_53;
static NJS_CNK_OBJECT rush_sky_land_draw_52;
static NJS_CNK_OBJECT rush_sky_land_draw_51;
static NJS_CNK_OBJECT rush_sky_land_draw_50;
static NJS_CNK_OBJECT rush_sky_land_draw_49;
static NJS_CNK_OBJECT rush_sky_land_draw_48;
static NJS_CNK_OBJECT rush_sky_land_draw_47;
static NJS_CNK_OBJECT rush_sky_land_draw_46;
static NJS_CNK_OBJECT rush_sky_land_draw_45;
static NJS_CNK_OBJECT rush_sky_land_draw_44;
static NJS_CNK_OBJECT rush_sky_land_draw_43;
static NJS_CNK_OBJECT rush_sky_land_draw_42;
static NJS_CNK_OBJECT rush_sky_land_draw_41;
static NJS_CNK_OBJECT rush_sky_land_draw_40;
static NJS_CNK_OBJECT rush_sky_land_draw_39;
static NJS_CNK_OBJECT rush_sky_land_draw_38;
static NJS_CNK_OBJECT rush_sky_land_draw_37;
static NJS_CNK_OBJECT rush_sky_land_draw_36;
static NJS_CNK_OBJECT rush_sky_land_draw_35;
static NJS_CNK_OBJECT rush_sky_land_draw_34;
static NJS_CNK_OBJECT rush_sky_land_draw_33;
static NJS_CNK_OBJECT rush_sky_land_draw_32;
static NJS_CNK_OBJECT rush_sky_land_draw_31;
static NJS_CNK_OBJECT rush_sky_land_draw_30;
static NJS_CNK_OBJECT rush_sky_land_draw_29;
static NJS_CNK_OBJECT rush_sky_land_draw_28;
static NJS_CNK_OBJECT rush_sky_land_draw_27;
static NJS_CNK_OBJECT rush_sky_land_draw_26;
static NJS_CNK_OBJECT rush_sky_land_draw_25;
static NJS_CNK_OBJECT rush_sky_land_draw_24;
static NJS_CNK_OBJECT rush_sky_land_draw_23;
static NJS_CNK_OBJECT rush_sky_land_draw_22;
static NJS_CNK_OBJECT rush_sky_land_draw_21;
static NJS_CNK_OBJECT rush_sky_land_draw_20;
static NJS_CNK_OBJECT rush_sky_land_draw_19;
static NJS_CNK_OBJECT rush_sky_land_draw_18;
static NJS_CNK_OBJECT rush_sky_land_draw_17;
static NJS_CNK_OBJECT rush_sky_land_draw_16;
static NJS_CNK_OBJECT rush_sky_land_draw_15;
static NJS_CNK_OBJECT rush_sky_land_draw_14;
static NJS_CNK_OBJECT rush_sky_land_draw_13;
static NJS_CNK_OBJECT rush_sky_land_draw_13_instance_1;
static NJS_CNK_OBJECT rush_sky_land_draw_13_instance_2;
static NJS_CNK_OBJECT rush_sky_land_draw_12;
static NJS_CNK_OBJECT rush_sky_land_draw_11;
static NJS_CNK_OBJECT rush_sky_land_draw_10;
static NJS_CNK_OBJECT rush_sky_land_draw_9;
static NJS_CNK_OBJECT rush_sky_land_draw_8;
static NJS_CNK_OBJECT rush_sky_land_draw_7;
static NJS_CNK_OBJECT rush_sky_land_draw_6;
static NJS_CNK_OBJECT rush_sky_land_draw_5;
static NJS_CNK_OBJECT rush_sky_land_draw_4;
static NJS_CNK_OBJECT rush_sky_land_draw_3;
static NJS_CNK_OBJECT rush_sky_land_draw_2;
static NJS_CNK_OBJECT rush_sky_land_draw_1;
static NJS_CNK_OBJECT rush_sky_land_draw_0;

static struct land_entry rush_sky_land_entries[514] = {
#include "stg30/sky_land_tex/rush_sky_land_entries.inc"
};
struct land_table rush_sky_land = {
#include "stg30/sky_land_tex/rush_sky_land.inc"
};
static NJS_TEXNAME rush_sky_land_textures[12] = {
#include "stg30/sky_land_tex/rush_sky_land_textures.inc"
};
NJS_TEXLIST rush_sky_land_texlist = {
#include "stg30/sky_land_tex/rush_sky_land_texlist.inc"
};
static Sint16 rush_sky_land_draw_67_polygons[433] = {
#include "stg30/sky_land_tex/rush_sky_land_draw_67_polygons.inc"
};
static Sint32 rush_sky_land_draw_67_vertices[243] = {
#include "stg30/sky_land_tex/rush_sky_land_draw_67_vertices.inc"
};
static NJS_CNK_MODEL rush_sky_land_draw_67_model = {
#include "stg30/sky_land_tex/rush_sky_land_draw_67_model.inc"
};
NJS_CNK_OBJECT rush_scenery_9 = {
#include "stg30/sky_land_tex/rush_scenery_9.inc"
};
static Sint16 rush_sky_land_draw_128_polygons[433] = {
#include "stg30/sky_land_tex/rush_sky_land_draw_128_polygons.inc"
};
static Sint32 rush_sky_land_draw_128_vertices[247] = {
#include "stg30/sky_land_tex/rush_sky_land_draw_128_vertices.inc"
};
static NJS_CNK_MODEL rush_sky_land_draw_128_model = {
#include "stg30/sky_land_tex/rush_sky_land_draw_128_model.inc"
};
NJS_CNK_OBJECT rush_scenery_6 = {
#include "stg30/sky_land_tex/rush_scenery_6.inc"
};
static Sint16 rush_sky_land_draw_454_polygons[1033] = {
#include "stg30/sky_land_tex/rush_sky_land_draw_454_polygons.inc"
};
static Sint32 rush_sky_land_draw_454_vertices[643] = {
#include "stg30/sky_land_tex/rush_sky_land_draw_454_vertices.inc"
};
static NJS_CNK_MODEL rush_sky_land_draw_454_model = {
#include "stg30/sky_land_tex/rush_sky_land_draw_454_model.inc"
};
NJS_CNK_OBJECT rush_scenery_8 = {
#include "stg30/sky_land_tex/rush_scenery_8.inc"
};
static Sint16 rush_sky_land_draw_68_polygons[577] = {
#include "stg30/sky_land_tex/rush_sky_land_draw_68_polygons.inc"
};
static Sint32 rush_sky_land_draw_68_vertices[339] = {
#include "stg30/sky_land_tex/rush_sky_land_draw_68_vertices.inc"
};
static NJS_CNK_MODEL rush_sky_land_draw_68_model = {
#include "stg30/sky_land_tex/rush_sky_land_draw_68_model.inc"
};
NJS_CNK_OBJECT rush_scenery_7 = {
#include "stg30/sky_land_tex/rush_scenery_7.inc"
};
static Sint16 rush_sky_land_draw_72_polygons[145] = {
#include "stg30/sky_land_tex/rush_sky_land_draw_72_polygons.inc"
};
static Sint32 rush_sky_land_draw_72_vertices[51] = {
#include "stg30/sky_land_tex/rush_sky_land_draw_72_vertices.inc"
};
static NJS_CNK_MODEL rush_sky_land_draw_72_model = {
#include "stg30/sky_land_tex/rush_sky_land_draw_72_model.inc"
};
NJS_CNK_OBJECT rush_scenery_5 = {
#include "stg30/sky_land_tex/rush_scenery_5.inc"
};
static Sint16 rush_sky_land_draw_5_polygons[49] = {
#include "stg30/sky_land_tex/rush_sky_land_draw_5_polygons.inc"
};
static Sint32 rush_sky_land_draw_5_vertices[51] = {
#include "stg30/sky_land_tex/rush_sky_land_draw_5_vertices.inc"
};
static NJS_CNK_MODEL rush_sky_land_draw_5_model = {
#include "stg30/sky_land_tex/rush_sky_land_draw_5_model.inc"
};
NJS_CNK_OBJECT rush_scenery_1 = {
#include "stg30/sky_land_tex/rush_scenery_1.inc"
};
static Sint16 rush_sky_land_draw_3_polygons[73] = {
#include "stg30/sky_land_tex/rush_sky_land_draw_3_polygons.inc"
};
static Sint32 rush_sky_land_draw_3_vertices[67] = {
#include "stg30/sky_land_tex/rush_sky_land_draw_3_vertices.inc"
};
static NJS_CNK_MODEL rush_sky_land_draw_3_model = {
#include "stg30/sky_land_tex/rush_sky_land_draw_3_model.inc"
};
NJS_CNK_OBJECT rush_scenery_2 = {
#include "stg30/sky_land_tex/rush_scenery_2.inc"
};
static Sint16 rush_sky_land_draw_0_polygons[29] = {
#include "stg30/sky_land_tex/rush_sky_land_draw_0_polygons.inc"
};
static Sint32 rush_sky_land_draw_0_vertices[27] = {
#include "stg30/sky_land_tex/rush_sky_land_draw_0_vertices.inc"
};
static NJS_CNK_MODEL rush_sky_land_draw_0_model = {
#include "stg30/sky_land_tex/rush_sky_land_draw_0_model.inc"
};
NJS_CNK_OBJECT rush_scenery_3 = {
#include "stg30/sky_land_tex/rush_scenery_3.inc"
};
static Sint16 rush_sky_land_draw_1_polygons[145] = {
#include "stg30/sky_land_tex/rush_sky_land_draw_1_polygons.inc"
};
static Sint32 rush_sky_land_draw_1_vertices[67] = {
#include "stg30/sky_land_tex/rush_sky_land_draw_1_vertices.inc"
};
static NJS_CNK_MODEL rush_sky_land_draw_1_model = {
#include "stg30/sky_land_tex/rush_sky_land_draw_1_model.inc"
};
NJS_CNK_OBJECT rush_scenery_52 = {
#include "stg30/sky_land_tex/rush_scenery_52.inc"
};
static Sint16 rush_sky_land_draw_52_polygons[197] = {
#include "stg30/sky_land_tex/rush_sky_land_draw_52_polygons.inc"
};
static Sint32 rush_sky_land_draw_52_vertices[99] = {
#include "stg30/sky_land_tex/rush_sky_land_draw_52_vertices.inc"
};
static NJS_CNK_MODEL rush_sky_land_draw_52_model = {
#include "stg30/sky_land_tex/rush_sky_land_draw_52_model.inc"
};
NJS_CNK_OBJECT rush_scenery_11 = {
#include "stg30/sky_land_tex/rush_scenery_11.inc"
};
static Sint16 rush_sky_land_draw_89_polygons[563] = {
#include "stg30/sky_land_tex/rush_sky_land_draw_89_polygons.inc"
};
static Sint32 rush_sky_land_draw_89_vertices[331] = {
#include "stg30/sky_land_tex/rush_sky_land_draw_89_vertices.inc"
};
static NJS_CNK_MODEL rush_sky_land_draw_89_model = {
#include "stg30/sky_land_tex/rush_sky_land_draw_89_model.inc"
};
NJS_CNK_OBJECT rush_scenery_4 = {
#include "stg30/sky_land_tex/rush_scenery_4.inc"
};
static Sint16 rush_sky_land_draw_459_polygons[601] = {
#include "stg30/sky_land_tex/rush_sky_land_draw_459_polygons.inc"
};
static Sint32 rush_sky_land_draw_459_vertices[355] = {
#include "stg30/sky_land_tex/rush_sky_land_draw_459_vertices.inc"
};
static NJS_CNK_MODEL rush_sky_land_draw_459_model = {
#include "stg30/sky_land_tex/rush_sky_land_draw_459_model.inc"
};
NJS_CNK_OBJECT rush_scenery_14 = {
#include "stg30/sky_land_tex/rush_scenery_14.inc"
};
static Sint16 rush_sky_land_draw_451_polygons[673] = {
#include "stg30/sky_land_tex/rush_sky_land_draw_451_polygons.inc"
};
static Sint32 rush_sky_land_draw_451_vertices[403] = {
#include "stg30/sky_land_tex/rush_sky_land_draw_451_vertices.inc"
};
static NJS_CNK_MODEL rush_sky_land_draw_451_model = {
#include "stg30/sky_land_tex/rush_sky_land_draw_451_model.inc"
};
NJS_CNK_OBJECT rush_scenery_10 = {
#include "stg30/sky_land_tex/rush_scenery_10.inc"
};
static Sint16 rush_sky_land_draw_235_polygons[667] = {
#include "stg30/sky_land_tex/rush_sky_land_draw_235_polygons.inc"
};
static Sint32 rush_sky_land_draw_235_vertices[431] = {
#include "stg30/sky_land_tex/rush_sky_land_draw_235_vertices.inc"
};
static NJS_CNK_MODEL rush_sky_land_draw_235_model = {
#include "stg30/sky_land_tex/rush_sky_land_draw_235_model.inc"
};
NJS_CNK_OBJECT rush_scenery_17 = {
#include "stg30/sky_land_tex/rush_scenery_17.inc"
};
static Sint16 rush_sky_land_draw_399_polygons[363] = {
#include "stg30/sky_land_tex/rush_sky_land_draw_399_polygons.inc"
};
static Sint32 rush_sky_land_draw_399_vertices[199] = {
#include "stg30/sky_land_tex/rush_sky_land_draw_399_vertices.inc"
};
static NJS_CNK_MODEL rush_sky_land_draw_399_model = {
#include "stg30/sky_land_tex/rush_sky_land_draw_399_model.inc"
};
NJS_CNK_OBJECT rush_scenery_18 = {
#include "stg30/sky_land_tex/rush_scenery_18.inc"
};
static Sint16 rush_sky_land_draw_417_polygons[805] = {
#include "stg30/sky_land_tex/rush_sky_land_draw_417_polygons.inc"
};
static Sint32 rush_sky_land_draw_417_vertices[499] = {
#include "stg30/sky_land_tex/rush_sky_land_draw_417_vertices.inc"
};
static NJS_CNK_MODEL rush_sky_land_draw_417_model = {
#include "stg30/sky_land_tex/rush_sky_land_draw_417_model.inc"
};
NJS_CNK_OBJECT rush_scenery_21 = {
#include "stg30/sky_land_tex/rush_scenery_21.inc"
};
static Sint16 rush_sky_land_draw_416_polygons[121] = {
#include "stg30/sky_land_tex/rush_sky_land_draw_416_polygons.inc"
};
static Sint32 rush_sky_land_draw_416_vertices[51] = {
#include "stg30/sky_land_tex/rush_sky_land_draw_416_vertices.inc"
};
static NJS_CNK_MODEL rush_sky_land_draw_416_model = {
#include "stg30/sky_land_tex/rush_sky_land_draw_416_model.inc"
};
NJS_CNK_OBJECT rush_scenery_54 = {
#include "stg30/sky_land_tex/rush_scenery_54.inc"
};
static Sint16 rush_sky_land_draw_415_polygons[121] = {
#include "stg30/sky_land_tex/rush_sky_land_draw_415_polygons.inc"
};
static Sint32 rush_sky_land_draw_415_vertices[51] = {
#include "stg30/sky_land_tex/rush_sky_land_draw_415_vertices.inc"
};
static NJS_CNK_MODEL rush_sky_land_draw_415_model = {
#include "stg30/sky_land_tex/rush_sky_land_draw_415_model.inc"
};
NJS_CNK_OBJECT rush_scenery_56 = {
#include "stg30/sky_land_tex/rush_scenery_56.inc"
};
static Sint16 rush_sky_land_draw_414_polygons[315] = {
#include "stg30/sky_land_tex/rush_sky_land_draw_414_polygons.inc"
};
static Sint32 rush_sky_land_draw_414_vertices[191] = {
#include "stg30/sky_land_tex/rush_sky_land_draw_414_vertices.inc"
};
static NJS_CNK_MODEL rush_sky_land_draw_414_model = {
#include "stg30/sky_land_tex/rush_sky_land_draw_414_model.inc"
};
NJS_CNK_OBJECT rush_scenery_19 = {
#include "stg30/sky_land_tex/rush_scenery_19.inc"
};
static Sint16 rush_sky_land_draw_413_polygons[269] = {
#include "stg30/sky_land_tex/rush_sky_land_draw_413_polygons.inc"
};
static Sint32 rush_sky_land_draw_413_vertices[167] = {
#include "stg30/sky_land_tex/rush_sky_land_draw_413_vertices.inc"
};
static NJS_CNK_MODEL rush_sky_land_draw_413_model = {
#include "stg30/sky_land_tex/rush_sky_land_draw_413_model.inc"
};
NJS_CNK_OBJECT rush_scenery_15 = {
#include "stg30/sky_land_tex/rush_scenery_15.inc"
};
static Sint16 rush_sky_land_draw_411_polygons[287] = {
#include "stg30/sky_land_tex/rush_sky_land_draw_411_polygons.inc"
};
static Sint32 rush_sky_land_draw_411_vertices[179] = {
#include "stg30/sky_land_tex/rush_sky_land_draw_411_vertices.inc"
};
static NJS_CNK_MODEL rush_sky_land_draw_411_model = {
#include "stg30/sky_land_tex/rush_sky_land_draw_411_model.inc"
};
NJS_CNK_OBJECT rush_scenery_16 = {
#include "stg30/sky_land_tex/rush_scenery_16.inc"
};
static Sint16 rush_sky_land_draw_409_polygons[107] = {
#include "stg30/sky_land_tex/rush_sky_land_draw_409_polygons.inc"
};
static Sint32 rush_sky_land_draw_409_vertices[51] = {
#include "stg30/sky_land_tex/rush_sky_land_draw_409_vertices.inc"
};
static NJS_CNK_MODEL rush_sky_land_draw_409_model = {
#include "stg30/sky_land_tex/rush_sky_land_draw_409_model.inc"
};
NJS_CNK_OBJECT rush_scenery_53 = {
#include "stg30/sky_land_tex/rush_scenery_53.inc"
};
static Sint16 rush_sky_land_draw_403_polygons[255] = {
#include "stg30/sky_land_tex/rush_sky_land_draw_403_polygons.inc"
};
static Sint32 rush_sky_land_draw_403_vertices[159] = {
#include "stg30/sky_land_tex/rush_sky_land_draw_403_vertices.inc"
};
static NJS_CNK_MODEL rush_sky_land_draw_403_model = {
#include "stg30/sky_land_tex/rush_sky_land_draw_403_model.inc"
};
NJS_CNK_OBJECT rush_scenery_13 = {
#include "stg30/sky_land_tex/rush_scenery_13.inc"
};
static Sint16 rush_sky_land_draw_400_polygons[905] = {
#include "stg30/sky_land_tex/rush_sky_land_draw_400_polygons.inc"
};
static Sint32 rush_sky_land_draw_400_vertices[587] = {
#include "stg30/sky_land_tex/rush_sky_land_draw_400_vertices.inc"
};
static NJS_CNK_MODEL rush_sky_land_draw_400_model = {
#include "stg30/sky_land_tex/rush_sky_land_draw_400_model.inc"
};
NJS_CNK_OBJECT rush_scenery_12 = {
#include "stg30/sky_land_tex/rush_scenery_12.inc"
};
static Sint16 rush_sky_land_draw_395_polygons[121] = {
#include "stg30/sky_land_tex/rush_sky_land_draw_395_polygons.inc"
};
static Sint32 rush_sky_land_draw_395_vertices[51] = {
#include "stg30/sky_land_tex/rush_sky_land_draw_395_vertices.inc"
};
static NJS_CNK_MODEL rush_sky_land_draw_395_model = {
#include "stg30/sky_land_tex/rush_sky_land_draw_395_model.inc"
};
NJS_CNK_OBJECT rush_scenery_55 = {
#include "stg30/sky_land_tex/rush_scenery_55.inc"
};
static Sint16 rush_sky_land_draw_6_polygons[255] = {
#include "stg30/sky_land_tex/rush_sky_land_draw_6_polygons.inc"
};
static Sint32 rush_sky_land_draw_6_vertices[159] = {
#include "stg30/sky_land_tex/rush_sky_land_draw_6_vertices.inc"
};
static NJS_CNK_MODEL rush_sky_land_draw_6_model = {
#include "stg30/sky_land_tex/rush_sky_land_draw_6_model.inc"
};
NJS_CNK_OBJECT rush_scenery_20 = {
#include "stg30/sky_land_tex/rush_scenery_20.inc"
};
static Sint16 rush_sky_land_draw_138_polygons[483] = {
#include "stg30/sky_land_tex/rush_sky_land_draw_138_polygons.inc"
};
static Sint32 rush_sky_land_draw_138_vertices[307] = {
#include "stg30/sky_land_tex/rush_sky_land_draw_138_vertices.inc"
};
static NJS_CNK_MODEL rush_sky_land_draw_138_model = {
#include "stg30/sky_land_tex/rush_sky_land_draw_138_model.inc"
};
NJS_CNK_OBJECT rush_scenery_22 = {
#include "stg30/sky_land_tex/rush_scenery_22.inc"
};
static Sint16 rush_sky_land_draw_137_polygons[491] = {
#include "stg30/sky_land_tex/rush_sky_land_draw_137_polygons.inc"
};
static Sint32 rush_sky_land_draw_137_vertices[311] = {
#include "stg30/sky_land_tex/rush_sky_land_draw_137_vertices.inc"
};
static NJS_CNK_MODEL rush_sky_land_draw_137_model = {
#include "stg30/sky_land_tex/rush_sky_land_draw_137_model.inc"
};
NJS_CNK_OBJECT rush_scenery_23 = {
#include "stg30/sky_land_tex/rush_scenery_23.inc"
};
static Sint16 rush_sky_land_draw_136_polygons[1259] = {
#include "stg30/sky_land_tex/rush_sky_land_draw_136_polygons.inc"
};
static Sint32 rush_sky_land_draw_136_vertices[827] = {
#include "stg30/sky_land_tex/rush_sky_land_draw_136_vertices.inc"
};
static NJS_CNK_MODEL rush_sky_land_draw_136_model = {
#include "stg30/sky_land_tex/rush_sky_land_draw_136_model.inc"
};
NJS_CNK_OBJECT rush_scenery_24 = {
#include "stg30/sky_land_tex/rush_scenery_24.inc"
};
static Sint16 rush_sky_land_draw_371_polygons[273] = {
#include "stg30/sky_land_tex/rush_sky_land_draw_371_polygons.inc"
};
static Sint32 rush_sky_land_draw_371_vertices[147] = {
#include "stg30/sky_land_tex/rush_sky_land_draw_371_vertices.inc"
};
static NJS_CNK_MODEL rush_sky_land_draw_371_model = {
#include "stg30/sky_land_tex/rush_sky_land_draw_371_model.inc"
};
NJS_CNK_OBJECT rush_scenery_0 = {
#include "stg30/sky_land_tex/rush_scenery_0.inc"
};
static Sint16 rush_sky_land_draw_73_polygons[197] = {
#include "stg30/sky_land_tex/rush_sky_land_draw_73_polygons.inc"
};
static Sint32 rush_sky_land_draw_73_vertices[99] = {
#include "stg30/sky_land_tex/rush_sky_land_draw_73_vertices.inc"
};
static NJS_CNK_MODEL rush_sky_land_draw_73_model = {
#include "stg30/sky_land_tex/rush_sky_land_draw_73_model.inc"
};
NJS_CNK_OBJECT rush_scenery_26 = {
#include "stg30/sky_land_tex/rush_scenery_26.inc"
};
static Sint16 rush_sky_land_draw_348_polygons[927] = {
#include "stg30/sky_land_tex/rush_sky_land_draw_348_polygons.inc"
};
static Sint32 rush_sky_land_draw_348_vertices[587] = {
#include "stg30/sky_land_tex/rush_sky_land_draw_348_vertices.inc"
};
static NJS_CNK_MODEL rush_sky_land_draw_348_model = {
#include "stg30/sky_land_tex/rush_sky_land_draw_348_model.inc"
};
NJS_CNK_OBJECT rush_scenery_25 = {
#include "stg30/sky_land_tex/rush_scenery_25.inc"
};
static Sint16 rush_sky_land_draw_347_polygons[297] = {
#include "stg30/sky_land_tex/rush_sky_land_draw_347_polygons.inc"
};
static Sint32 rush_sky_land_draw_347_vertices[175] = {
#include "stg30/sky_land_tex/rush_sky_land_draw_347_vertices.inc"
};
static NJS_CNK_MODEL rush_sky_land_draw_347_model = {
#include "stg30/sky_land_tex/rush_sky_land_draw_347_model.inc"
};
NJS_CNK_OBJECT rush_scenery_27 = {
#include "stg30/sky_land_tex/rush_scenery_27.inc"
};
static Sint16 rush_sky_land_draw_327_polygons[309] = {
#include "stg30/sky_land_tex/rush_sky_land_draw_327_polygons.inc"
};
static Sint32 rush_sky_land_draw_327_vertices[195] = {
#include "stg30/sky_land_tex/rush_sky_land_draw_327_vertices.inc"
};
static NJS_CNK_MODEL rush_sky_land_draw_327_model = {
#include "stg30/sky_land_tex/rush_sky_land_draw_327_model.inc"
};
NJS_CNK_OBJECT rush_scenery_28 = {
#include "stg30/sky_land_tex/rush_scenery_28.inc"
};
static Sint16 rush_sky_land_draw_12_polygons[219] = {
#include "stg30/sky_land_tex/rush_sky_land_draw_12_polygons.inc"
};
static Sint32 rush_sky_land_draw_12_vertices[99] = {
#include "stg30/sky_land_tex/rush_sky_land_draw_12_vertices.inc"
};
static NJS_CNK_MODEL rush_sky_land_draw_12_model = {
#include "stg30/sky_land_tex/rush_sky_land_draw_12_model.inc"
};
NJS_CNK_OBJECT rush_scenery_29 = {
#include "stg30/sky_land_tex/rush_scenery_29.inc"
};
static Sint16 rush_sky_land_draw_293_polygons[87] = {
#include "stg30/sky_land_tex/rush_sky_land_draw_293_polygons.inc"
};
static Sint32 rush_sky_land_draw_293_vertices[51] = {
#include "stg30/sky_land_tex/rush_sky_land_draw_293_vertices.inc"
};
static NJS_CNK_MODEL rush_sky_land_draw_293_model = {
#include "stg30/sky_land_tex/rush_sky_land_draw_293_model.inc"
};
NJS_CNK_OBJECT rush_scenery_30 = {
#include "stg30/sky_land_tex/rush_scenery_30.inc"
};
static Sint16 rush_sky_land_draw_186_polygons[289] = {
#include "stg30/sky_land_tex/rush_sky_land_draw_186_polygons.inc"
};
static Sint32 rush_sky_land_draw_186_vertices[147] = {
#include "stg30/sky_land_tex/rush_sky_land_draw_186_vertices.inc"
};
static NJS_CNK_MODEL rush_sky_land_draw_186_model = {
#include "stg30/sky_land_tex/rush_sky_land_draw_186_model.inc"
};
NJS_CNK_OBJECT rush_scenery_32 = {
#include "stg30/sky_land_tex/rush_scenery_32.inc"
};
static Sint16 rush_sky_land_draw_287_polygons[697] = {
#include "stg30/sky_land_tex/rush_sky_land_draw_287_polygons.inc"
};
static Sint32 rush_sky_land_draw_287_vertices[419] = {
#include "stg30/sky_land_tex/rush_sky_land_draw_287_vertices.inc"
};
static NJS_CNK_MODEL rush_sky_land_draw_287_model = {
#include "stg30/sky_land_tex/rush_sky_land_draw_287_model.inc"
};
NJS_CNK_OBJECT rush_scenery_31 = {
#include "stg30/sky_land_tex/rush_scenery_31.inc"
};
static Sint16 rush_sky_land_draw_231_polygons[271] = {
#include "stg30/sky_land_tex/rush_sky_land_draw_231_polygons.inc"
};
static Sint32 rush_sky_land_draw_231_vertices[243] = {
#include "stg30/sky_land_tex/rush_sky_land_draw_231_vertices.inc"
};
static NJS_CNK_MODEL rush_sky_land_draw_231_model = {
#include "stg30/sky_land_tex/rush_sky_land_draw_231_model.inc"
};
NJS_CNK_OBJECT rush_scenery_33 = {
#include "stg30/sky_land_tex/rush_scenery_33.inc"
};
static Sint16 rush_sky_land_draw_242_polygons[269] = {
#include "stg30/sky_land_tex/rush_sky_land_draw_242_polygons.inc"
};
static Sint32 rush_sky_land_draw_242_vertices[147] = {
#include "stg30/sky_land_tex/rush_sky_land_draw_242_vertices.inc"
};
static NJS_CNK_MODEL rush_sky_land_draw_242_model = {
#include "stg30/sky_land_tex/rush_sky_land_draw_242_model.inc"
};
NJS_CNK_OBJECT rush_scenery_34 = {
#include "stg30/sky_land_tex/rush_scenery_34.inc"
};
static Sint16 rush_sky_land_draw_71_polygons[563] = {
#include "stg30/sky_land_tex/rush_sky_land_draw_71_polygons.inc"
};
static Sint32 rush_sky_land_draw_71_vertices[343] = {
#include "stg30/sky_land_tex/rush_sky_land_draw_71_vertices.inc"
};
static NJS_CNK_MODEL rush_sky_land_draw_71_model = {
#include "stg30/sky_land_tex/rush_sky_land_draw_71_model.inc"
};
NJS_CNK_OBJECT rush_scenery_36 = {
#include "stg30/sky_land_tex/rush_scenery_36.inc"
};
static Sint16 rush_sky_land_draw_195_polygons[529] = {
#include "stg30/sky_land_tex/rush_sky_land_draw_195_polygons.inc"
};
static Sint32 rush_sky_land_draw_195_vertices[307] = {
#include "stg30/sky_land_tex/rush_sky_land_draw_195_vertices.inc"
};
static NJS_CNK_MODEL rush_sky_land_draw_195_model = {
#include "stg30/sky_land_tex/rush_sky_land_draw_195_model.inc"
};
NJS_CNK_OBJECT rush_scenery_35 = {
#include "stg30/sky_land_tex/rush_scenery_35.inc"
};
static Sint16 rush_sky_land_draw_193_polygons[113] = {
#include "stg30/sky_land_tex/rush_sky_land_draw_193_polygons.inc"
};
static Sint32 rush_sky_land_draw_193_vertices[35] = {
#include "stg30/sky_land_tex/rush_sky_land_draw_193_vertices.inc"
};
static NJS_CNK_MODEL rush_sky_land_draw_193_model = {
#include "stg30/sky_land_tex/rush_sky_land_draw_193_model.inc"
};
NJS_CNK_OBJECT rush_scenery_37 = {
#include "stg30/sky_land_tex/rush_scenery_37.inc"
};
static Sint16 rush_sky_land_draw_192_polygons[1159] = {
#include "stg30/sky_land_tex/rush_sky_land_draw_192_polygons.inc"
};
static Sint32 rush_sky_land_draw_192_vertices[727] = {
#include "stg30/sky_land_tex/rush_sky_land_draw_192_vertices.inc"
};
static NJS_CNK_MODEL rush_sky_land_draw_192_model = {
#include "stg30/sky_land_tex/rush_sky_land_draw_192_model.inc"
};
NJS_CNK_OBJECT rush_scenery_38 = {
#include "stg30/sky_land_tex/rush_scenery_38.inc"
};
static Sint16 rush_sky_land_draw_191_polygons[385] = {
#include "stg30/sky_land_tex/rush_sky_land_draw_191_polygons.inc"
};
static Sint32 rush_sky_land_draw_191_vertices[215] = {
#include "stg30/sky_land_tex/rush_sky_land_draw_191_vertices.inc"
};
static NJS_CNK_MODEL rush_sky_land_draw_191_model = {
#include "stg30/sky_land_tex/rush_sky_land_draw_191_model.inc"
};
NJS_CNK_OBJECT rush_scenery_39 = {
#include "stg30/sky_land_tex/rush_scenery_39.inc"
};
static Sint16 rush_sky_land_draw_190_polygons[677] = {
#include "stg30/sky_land_tex/rush_sky_land_draw_190_polygons.inc"
};
static Sint32 rush_sky_land_draw_190_vertices[419] = {
#include "stg30/sky_land_tex/rush_sky_land_draw_190_vertices.inc"
};
static NJS_CNK_MODEL rush_sky_land_draw_190_model = {
#include "stg30/sky_land_tex/rush_sky_land_draw_190_model.inc"
};
NJS_CNK_OBJECT rush_scenery_41 = {
#include "stg30/sky_land_tex/rush_scenery_41.inc"
};
static Sint16 rush_sky_land_draw_70_polygons[1025] = {
#include "stg30/sky_land_tex/rush_sky_land_draw_70_polygons.inc"
};
static Sint32 rush_sky_land_draw_70_vertices[635] = {
#include "stg30/sky_land_tex/rush_sky_land_draw_70_vertices.inc"
};
static NJS_CNK_MODEL rush_sky_land_draw_70_model = {
#include "stg30/sky_land_tex/rush_sky_land_draw_70_model.inc"
};
NJS_CNK_OBJECT rush_scenery_40 = {
#include "stg30/sky_land_tex/rush_scenery_40.inc"
};
static Sint16 rush_sky_land_draw_117_polygons[635] = {
#include "stg30/sky_land_tex/rush_sky_land_draw_117_polygons.inc"
};
static Sint32 rush_sky_land_draw_117_vertices[391] = {
#include "stg30/sky_land_tex/rush_sky_land_draw_117_vertices.inc"
};
static NJS_CNK_MODEL rush_sky_land_draw_117_model = {
#include "stg30/sky_land_tex/rush_sky_land_draw_117_model.inc"
};
NJS_CNK_OBJECT rush_scenery_42 = {
#include "stg30/sky_land_tex/rush_scenery_42.inc"
};
static Sint16 rush_sky_land_draw_170_polygons[673] = {
#include "stg30/sky_land_tex/rush_sky_land_draw_170_polygons.inc"
};
static Sint32 rush_sky_land_draw_170_vertices[415] = {
#include "stg30/sky_land_tex/rush_sky_land_draw_170_vertices.inc"
};
static NJS_CNK_MODEL rush_sky_land_draw_170_model = {
#include "stg30/sky_land_tex/rush_sky_land_draw_170_model.inc"
};
NJS_CNK_OBJECT rush_scenery_44 = {
#include "stg30/sky_land_tex/rush_scenery_44.inc"
};
static Sint16 rush_sky_land_draw_94_polygons[1085] = {
#include "stg30/sky_land_tex/rush_sky_land_draw_94_polygons.inc"
};
static Sint32 rush_sky_land_draw_94_vertices[687] = {
#include "stg30/sky_land_tex/rush_sky_land_draw_94_vertices.inc"
};
static NJS_CNK_MODEL rush_sky_land_draw_94_model = {
#include "stg30/sky_land_tex/rush_sky_land_draw_94_model.inc"
};
NJS_CNK_OBJECT rush_scenery_43 = {
#include "stg30/sky_land_tex/rush_scenery_43.inc"
};
static Sint16 rush_sky_land_draw_146_polygons[535] = {
#include "stg30/sky_land_tex/rush_sky_land_draw_146_polygons.inc"
};
static Sint32 rush_sky_land_draw_146_vertices[311] = {
#include "stg30/sky_land_tex/rush_sky_land_draw_146_vertices.inc"
};
static NJS_CNK_MODEL rush_sky_land_draw_146_model = {
#include "stg30/sky_land_tex/rush_sky_land_draw_146_model.inc"
};
NJS_CNK_OBJECT rush_scenery_45 = {
#include "stg30/sky_land_tex/rush_scenery_45.inc"
};
static Sint16 rush_sky_land_draw_134_polygons[273] = {
#include "stg30/sky_land_tex/rush_sky_land_draw_134_polygons.inc"
};
static Sint32 rush_sky_land_draw_134_vertices[147] = {
#include "stg30/sky_land_tex/rush_sky_land_draw_134_vertices.inc"
};
static NJS_CNK_MODEL rush_sky_land_draw_134_model = {
#include "stg30/sky_land_tex/rush_sky_land_draw_134_model.inc"
};
static NJS_CNK_OBJECT rush_sky_land_draw_134 = {
#include "stg30/sky_land_tex/rush_sky_land_draw_134.inc"
};
static Sint16 rush_sky_land_draw_129_polygons[1019] = {
#include "stg30/sky_land_tex/rush_sky_land_draw_129_polygons.inc"
};
static Sint32 rush_sky_land_draw_129_vertices[647] = {
#include "stg30/sky_land_tex/rush_sky_land_draw_129_vertices.inc"
};
static NJS_CNK_MODEL rush_sky_land_draw_129_model = {
#include "stg30/sky_land_tex/rush_sky_land_draw_129_model.inc"
};
NJS_CNK_OBJECT rush_scenery_46 = {
#include "stg30/sky_land_tex/rush_scenery_46.inc"
};
static Sint16 rush_sky_land_draw_127_polygons[197] = {
#include "stg30/sky_land_tex/rush_sky_land_draw_127_polygons.inc"
};
static Sint32 rush_sky_land_draw_127_vertices[99] = {
#include "stg30/sky_land_tex/rush_sky_land_draw_127_vertices.inc"
};
static NJS_CNK_MODEL rush_sky_land_draw_127_model = {
#include "stg30/sky_land_tex/rush_sky_land_draw_127_model.inc"
};
NJS_CNK_OBJECT rush_scenery_51 = {
#include "stg30/sky_land_tex/rush_scenery_51.inc"
};
static Sint16 rush_sky_land_draw_66_polygons[275] = {
#include "stg30/sky_land_tex/rush_sky_land_draw_66_polygons.inc"
};
static Sint32 rush_sky_land_draw_66_vertices[171] = {
#include "stg30/sky_land_tex/rush_sky_land_draw_66_vertices.inc"
};
static NJS_CNK_MODEL rush_sky_land_draw_66_model = {
#include "stg30/sky_land_tex/rush_sky_land_draw_66_model.inc"
};
NJS_CNK_OBJECT rush_scenery_48 = {
#include "stg30/sky_land_tex/rush_scenery_48.inc"
};
static Sint16 rush_sky_land_draw_65_polygons[317] = {
#include "stg30/sky_land_tex/rush_sky_land_draw_65_polygons.inc"
};
static Sint32 rush_sky_land_draw_65_vertices[183] = {
#include "stg30/sky_land_tex/rush_sky_land_draw_65_vertices.inc"
};
static NJS_CNK_MODEL rush_sky_land_draw_65_model = {
#include "stg30/sky_land_tex/rush_sky_land_draw_65_model.inc"
};
NJS_CNK_OBJECT rush_scenery_49 = {
#include "stg30/sky_land_tex/rush_scenery_49.inc"
};
static Sint16 rush_sky_land_draw_64_polygons[261] = {
#include "stg30/sky_land_tex/rush_sky_land_draw_64_polygons.inc"
};
static Sint32 rush_sky_land_draw_64_vertices[163] = {
#include "stg30/sky_land_tex/rush_sky_land_draw_64_vertices.inc"
};
static NJS_CNK_MODEL rush_sky_land_draw_64_model = {
#include "stg30/sky_land_tex/rush_sky_land_draw_64_model.inc"
};
NJS_CNK_OBJECT rush_scenery_47 = {
#include "stg30/sky_land_tex/rush_scenery_47.inc"
};
static Sint16 rush_sky_land_draw_63_polygons[315] = {
#include "stg30/sky_land_tex/rush_sky_land_draw_63_polygons.inc"
};
static Sint32 rush_sky_land_draw_63_vertices[199] = {
#include "stg30/sky_land_tex/rush_sky_land_draw_63_vertices.inc"
};
static NJS_CNK_MODEL rush_sky_land_draw_63_model = {
#include "stg30/sky_land_tex/rush_sky_land_draw_63_model.inc"
};
NJS_CNK_OBJECT rush_scenery_50 = {
#include "stg30/sky_land_tex/rush_scenery_50.inc"
};
static NJS_CNK_OBJECT rush_sky_land_draw_503 = {
#include "stg30/sky_land_tex/rush_sky_land_draw_503.inc"
};
static NJS_CNK_OBJECT rush_sky_land_draw_502 = {
#include "stg30/sky_land_tex/rush_sky_land_draw_502.inc"
};
static NJS_CNK_OBJECT rush_sky_land_draw_501 = {
#include "stg30/sky_land_tex/rush_sky_land_draw_501.inc"
};
static NJS_CNK_OBJECT rush_sky_land_draw_500 = {
#include "stg30/sky_land_tex/rush_sky_land_draw_500.inc"
};
static NJS_CNK_OBJECT rush_sky_land_draw_499 = {
#include "stg30/sky_land_tex/rush_sky_land_draw_499.inc"
};
static NJS_CNK_OBJECT rush_sky_land_draw_498 = {
#include "stg30/sky_land_tex/rush_sky_land_draw_498.inc"
};
static NJS_CNK_OBJECT rush_sky_land_draw_497 = {
#include "stg30/sky_land_tex/rush_sky_land_draw_497.inc"
};
static NJS_CNK_OBJECT rush_sky_land_draw_496 = {
#include "stg30/sky_land_tex/rush_sky_land_draw_496.inc"
};
static NJS_CNK_OBJECT rush_sky_land_draw_495 = {
#include "stg30/sky_land_tex/rush_sky_land_draw_495.inc"
};
static NJS_CNK_OBJECT rush_sky_land_draw_494 = {
#include "stg30/sky_land_tex/rush_sky_land_draw_494.inc"
};
static NJS_CNK_OBJECT rush_sky_land_draw_493 = {
#include "stg30/sky_land_tex/rush_sky_land_draw_493.inc"
};
static NJS_CNK_OBJECT rush_sky_land_draw_492 = {
#include "stg30/sky_land_tex/rush_sky_land_draw_492.inc"
};
static NJS_CNK_OBJECT rush_sky_land_draw_491 = {
#include "stg30/sky_land_tex/rush_sky_land_draw_491.inc"
};
static NJS_CNK_OBJECT rush_sky_land_draw_490 = {
#include "stg30/sky_land_tex/rush_sky_land_draw_490.inc"
};
static NJS_CNK_OBJECT rush_sky_land_draw_489 = {
#include "stg30/sky_land_tex/rush_sky_land_draw_489.inc"
};
static NJS_CNK_OBJECT rush_sky_land_draw_488 = {
#include "stg30/sky_land_tex/rush_sky_land_draw_488.inc"
};
static NJS_CNK_OBJECT rush_sky_land_draw_487 = {
#include "stg30/sky_land_tex/rush_sky_land_draw_487.inc"
};
static NJS_CNK_OBJECT rush_sky_land_draw_486 = {
#include "stg30/sky_land_tex/rush_sky_land_draw_486.inc"
};
static NJS_CNK_OBJECT rush_sky_land_draw_485 = {
#include "stg30/sky_land_tex/rush_sky_land_draw_485.inc"
};
static NJS_CNK_OBJECT rush_sky_land_draw_484 = {
#include "stg30/sky_land_tex/rush_sky_land_draw_484.inc"
};
static NJS_CNK_OBJECT rush_sky_land_draw_483 = {
#include "stg30/sky_land_tex/rush_sky_land_draw_483.inc"
};
static NJS_CNK_OBJECT rush_sky_land_draw_481 = {
#include "stg30/sky_land_tex/rush_sky_land_draw_481.inc"
};
static NJS_CNK_OBJECT rush_sky_land_draw_480 = {
#include "stg30/sky_land_tex/rush_sky_land_draw_480.inc"
};
static NJS_CNK_OBJECT rush_sky_land_draw_479 = {
#include "stg30/sky_land_tex/rush_sky_land_draw_479.inc"
};
static NJS_CNK_OBJECT rush_sky_land_draw_478 = {
#include "stg30/sky_land_tex/rush_sky_land_draw_478.inc"
};
static NJS_CNK_OBJECT rush_sky_land_draw_477 = {
#include "stg30/sky_land_tex/rush_sky_land_draw_477.inc"
};
static NJS_CNK_OBJECT rush_sky_land_draw_476 = {
#include "stg30/sky_land_tex/rush_sky_land_draw_476.inc"
};
static NJS_CNK_OBJECT rush_sky_land_draw_475 = {
#include "stg30/sky_land_tex/rush_sky_land_draw_475.inc"
};
static NJS_CNK_OBJECT rush_sky_land_draw_474 = {
#include "stg30/sky_land_tex/rush_sky_land_draw_474.inc"
};
static NJS_CNK_OBJECT rush_sky_land_draw_473 = {
#include "stg30/sky_land_tex/rush_sky_land_draw_473.inc"
};
static NJS_CNK_OBJECT rush_sky_land_draw_472 = {
#include "stg30/sky_land_tex/rush_sky_land_draw_472.inc"
};
static NJS_CNK_OBJECT rush_sky_land_draw_471 = {
#include "stg30/sky_land_tex/rush_sky_land_draw_471.inc"
};
static NJS_CNK_OBJECT rush_sky_land_draw_470 = {
#include "stg30/sky_land_tex/rush_sky_land_draw_470.inc"
};
static NJS_CNK_OBJECT rush_sky_land_draw_469 = {
#include "stg30/sky_land_tex/rush_sky_land_draw_469.inc"
};
static NJS_CNK_OBJECT rush_sky_land_draw_468 = {
#include "stg30/sky_land_tex/rush_sky_land_draw_468.inc"
};
static NJS_CNK_OBJECT rush_sky_land_draw_467 = {
#include "stg30/sky_land_tex/rush_sky_land_draw_467.inc"
};
static NJS_CNK_OBJECT rush_sky_land_draw_466 = {
#include "stg30/sky_land_tex/rush_sky_land_draw_466.inc"
};
static NJS_CNK_OBJECT rush_sky_land_draw_465 = {
#include "stg30/sky_land_tex/rush_sky_land_draw_465.inc"
};
static NJS_CNK_OBJECT rush_sky_land_draw_464 = {
#include "stg30/sky_land_tex/rush_sky_land_draw_464.inc"
};
static NJS_CNK_OBJECT rush_sky_land_draw_463 = {
#include "stg30/sky_land_tex/rush_sky_land_draw_463.inc"
};
static NJS_CNK_OBJECT rush_sky_land_draw_462 = {
#include "stg30/sky_land_tex/rush_sky_land_draw_462.inc"
};
static NJS_CNK_OBJECT rush_sky_land_draw_461 = {
#include "stg30/sky_land_tex/rush_sky_land_draw_461.inc"
};
static NJS_CNK_OBJECT rush_sky_land_draw_460 = {
#include "stg30/sky_land_tex/rush_sky_land_draw_460.inc"
};
static NJS_CNK_OBJECT rush_sky_land_draw_458 = {
#include "stg30/sky_land_tex/rush_sky_land_draw_458.inc"
};
static NJS_CNK_OBJECT rush_sky_land_draw_457 = {
#include "stg30/sky_land_tex/rush_sky_land_draw_457.inc"
};
static NJS_CNK_OBJECT rush_sky_land_draw_456 = {
#include "stg30/sky_land_tex/rush_sky_land_draw_456.inc"
};
static NJS_CNK_OBJECT rush_sky_land_draw_455 = {
#include "stg30/sky_land_tex/rush_sky_land_draw_455.inc"
};
static NJS_CNK_OBJECT rush_sky_land_draw_454 = {
#include "stg30/sky_land_tex/rush_sky_land_draw_454.inc"
};
static NJS_CNK_OBJECT rush_sky_land_draw_453 = {
#include "stg30/sky_land_tex/rush_sky_land_draw_453.inc"
};
static NJS_CNK_OBJECT rush_sky_land_draw_452 = {
#include "stg30/sky_land_tex/rush_sky_land_draw_452.inc"
};
static NJS_CNK_OBJECT rush_sky_land_draw_450 = {
#include "stg30/sky_land_tex/rush_sky_land_draw_450.inc"
};
static NJS_CNK_OBJECT rush_sky_land_draw_449 = {
#include "stg30/sky_land_tex/rush_sky_land_draw_449.inc"
};
static NJS_CNK_OBJECT rush_sky_land_draw_448 = {
#include "stg30/sky_land_tex/rush_sky_land_draw_448.inc"
};
static NJS_CNK_OBJECT rush_sky_land_draw_447 = {
#include "stg30/sky_land_tex/rush_sky_land_draw_447.inc"
};
static NJS_CNK_OBJECT rush_sky_land_draw_446 = {
#include "stg30/sky_land_tex/rush_sky_land_draw_446.inc"
};
static NJS_CNK_OBJECT rush_sky_land_draw_445 = {
#include "stg30/sky_land_tex/rush_sky_land_draw_445.inc"
};
static NJS_CNK_OBJECT rush_sky_land_draw_444 = {
#include "stg30/sky_land_tex/rush_sky_land_draw_444.inc"
};
static NJS_CNK_OBJECT rush_sky_land_draw_443 = {
#include "stg30/sky_land_tex/rush_sky_land_draw_443.inc"
};
static NJS_CNK_OBJECT rush_sky_land_draw_442 = {
#include "stg30/sky_land_tex/rush_sky_land_draw_442.inc"
};
static NJS_CNK_OBJECT rush_sky_land_draw_441 = {
#include "stg30/sky_land_tex/rush_sky_land_draw_441.inc"
};
static NJS_CNK_OBJECT rush_sky_land_draw_440 = {
#include "stg30/sky_land_tex/rush_sky_land_draw_440.inc"
};
static NJS_CNK_OBJECT rush_sky_land_draw_439 = {
#include "stg30/sky_land_tex/rush_sky_land_draw_439.inc"
};
static NJS_CNK_OBJECT rush_sky_land_draw_438 = {
#include "stg30/sky_land_tex/rush_sky_land_draw_438.inc"
};
static NJS_CNK_OBJECT rush_sky_land_draw_437 = {
#include "stg30/sky_land_tex/rush_sky_land_draw_437.inc"
};
static NJS_CNK_OBJECT rush_sky_land_draw_436 = {
#include "stg30/sky_land_tex/rush_sky_land_draw_436.inc"
};
static NJS_CNK_OBJECT rush_sky_land_draw_435 = {
#include "stg30/sky_land_tex/rush_sky_land_draw_435.inc"
};
static NJS_CNK_OBJECT rush_sky_land_draw_434 = {
#include "stg30/sky_land_tex/rush_sky_land_draw_434.inc"
};
static NJS_CNK_OBJECT rush_sky_land_draw_433 = {
#include "stg30/sky_land_tex/rush_sky_land_draw_433.inc"
};
static NJS_CNK_OBJECT rush_sky_land_draw_432 = {
#include "stg30/sky_land_tex/rush_sky_land_draw_432.inc"
};
static NJS_CNK_OBJECT rush_sky_land_draw_431 = {
#include "stg30/sky_land_tex/rush_sky_land_draw_431.inc"
};
static NJS_CNK_OBJECT rush_sky_land_draw_430 = {
#include "stg30/sky_land_tex/rush_sky_land_draw_430.inc"
};
static NJS_CNK_OBJECT rush_sky_land_draw_429 = {
#include "stg30/sky_land_tex/rush_sky_land_draw_429.inc"
};
static NJS_CNK_OBJECT rush_sky_land_draw_428 = {
#include "stg30/sky_land_tex/rush_sky_land_draw_428.inc"
};
static NJS_CNK_OBJECT rush_sky_land_draw_427 = {
#include "stg30/sky_land_tex/rush_sky_land_draw_427.inc"
};
static NJS_CNK_OBJECT rush_sky_land_draw_426 = {
#include "stg30/sky_land_tex/rush_sky_land_draw_426.inc"
};
static NJS_CNK_OBJECT rush_sky_land_draw_425 = {
#include "stg30/sky_land_tex/rush_sky_land_draw_425.inc"
};
static NJS_CNK_OBJECT rush_sky_land_draw_424 = {
#include "stg30/sky_land_tex/rush_sky_land_draw_424.inc"
};
static NJS_CNK_OBJECT rush_sky_land_draw_423 = {
#include "stg30/sky_land_tex/rush_sky_land_draw_423.inc"
};
static NJS_CNK_OBJECT rush_sky_land_draw_422 = {
#include "stg30/sky_land_tex/rush_sky_land_draw_422.inc"
};
static NJS_CNK_OBJECT rush_sky_land_draw_421 = {
#include "stg30/sky_land_tex/rush_sky_land_draw_421.inc"
};
static NJS_CNK_OBJECT rush_sky_land_draw_420 = {
#include "stg30/sky_land_tex/rush_sky_land_draw_420.inc"
};
static NJS_CNK_OBJECT rush_sky_land_draw_412 = {
#include "stg30/sky_land_tex/rush_sky_land_draw_412.inc"
};
static NJS_CNK_OBJECT rush_sky_land_draw_410 = {
#include "stg30/sky_land_tex/rush_sky_land_draw_410.inc"
};
static NJS_CNK_OBJECT rush_sky_land_draw_408 = {
#include "stg30/sky_land_tex/rush_sky_land_draw_408.inc"
};
static NJS_CNK_OBJECT rush_sky_land_draw_406 = {
#include "stg30/sky_land_tex/rush_sky_land_draw_406.inc"
};
static NJS_CNK_OBJECT rush_sky_land_draw_405 = {
#include "stg30/sky_land_tex/rush_sky_land_draw_405.inc"
};
static NJS_CNK_OBJECT rush_sky_land_draw_404 = {
#include "stg30/sky_land_tex/rush_sky_land_draw_404.inc"
};
static NJS_CNK_OBJECT rush_sky_land_draw_403 = {
#include "stg30/sky_land_tex/rush_sky_land_draw_403.inc"
};
static NJS_CNK_OBJECT rush_sky_land_draw_402 = {
#include "stg30/sky_land_tex/rush_sky_land_draw_402.inc"
};
static NJS_CNK_OBJECT rush_sky_land_draw_401 = {
#include "stg30/sky_land_tex/rush_sky_land_draw_401.inc"
};
static NJS_CNK_OBJECT rush_sky_land_draw_399 = {
#include "stg30/sky_land_tex/rush_sky_land_draw_399.inc"
};
static NJS_CNK_OBJECT rush_sky_land_draw_398 = {
#include "stg30/sky_land_tex/rush_sky_land_draw_398.inc"
};
static NJS_CNK_OBJECT rush_sky_land_draw_397 = {
#include "stg30/sky_land_tex/rush_sky_land_draw_397.inc"
};
static NJS_CNK_OBJECT rush_sky_land_draw_396 = {
#include "stg30/sky_land_tex/rush_sky_land_draw_396.inc"
};
static NJS_CNK_OBJECT rush_sky_land_draw_394 = {
#include "stg30/sky_land_tex/rush_sky_land_draw_394.inc"
};
static NJS_CNK_OBJECT rush_sky_land_draw_393 = {
#include "stg30/sky_land_tex/rush_sky_land_draw_393.inc"
};
static NJS_CNK_OBJECT rush_sky_land_draw_390 = {
#include "stg30/sky_land_tex/rush_sky_land_draw_390.inc"
};
static NJS_CNK_OBJECT rush_sky_land_draw_389 = {
#include "stg30/sky_land_tex/rush_sky_land_draw_389.inc"
};
static NJS_CNK_OBJECT rush_sky_land_draw_388 = {
#include "stg30/sky_land_tex/rush_sky_land_draw_388.inc"
};
static NJS_CNK_OBJECT rush_sky_land_draw_386 = {
#include "stg30/sky_land_tex/rush_sky_land_draw_386.inc"
};
static NJS_CNK_OBJECT rush_sky_land_draw_385 = {
#include "stg30/sky_land_tex/rush_sky_land_draw_385.inc"
};
static NJS_CNK_OBJECT rush_sky_land_draw_384 = {
#include "stg30/sky_land_tex/rush_sky_land_draw_384.inc"
};
static NJS_CNK_OBJECT rush_sky_land_draw_383 = {
#include "stg30/sky_land_tex/rush_sky_land_draw_383.inc"
};
static NJS_CNK_OBJECT rush_sky_land_draw_382 = {
#include "stg30/sky_land_tex/rush_sky_land_draw_382.inc"
};
static NJS_CNK_OBJECT rush_sky_land_draw_380 = {
#include "stg30/sky_land_tex/rush_sky_land_draw_380.inc"
};
static NJS_CNK_OBJECT rush_sky_land_draw_379 = {
#include "stg30/sky_land_tex/rush_sky_land_draw_379.inc"
};
static NJS_CNK_OBJECT rush_sky_land_draw_378 = {
#include "stg30/sky_land_tex/rush_sky_land_draw_378.inc"
};
static NJS_CNK_OBJECT rush_sky_land_draw_377 = {
#include "stg30/sky_land_tex/rush_sky_land_draw_377.inc"
};
static NJS_CNK_OBJECT rush_sky_land_draw_376 = {
#include "stg30/sky_land_tex/rush_sky_land_draw_376.inc"
};
static NJS_CNK_OBJECT rush_sky_land_draw_375 = {
#include "stg30/sky_land_tex/rush_sky_land_draw_375.inc"
};
static NJS_CNK_OBJECT rush_sky_land_draw_374 = {
#include "stg30/sky_land_tex/rush_sky_land_draw_374.inc"
};
static NJS_CNK_OBJECT rush_sky_land_draw_373 = {
#include "stg30/sky_land_tex/rush_sky_land_draw_373.inc"
};
static NJS_CNK_OBJECT rush_sky_land_draw_372 = {
#include "stg30/sky_land_tex/rush_sky_land_draw_372.inc"
};
static NJS_CNK_OBJECT rush_sky_land_draw_370 = {
#include "stg30/sky_land_tex/rush_sky_land_draw_370.inc"
};
static NJS_CNK_OBJECT rush_sky_land_draw_369 = {
#include "stg30/sky_land_tex/rush_sky_land_draw_369.inc"
};
static NJS_CNK_OBJECT rush_sky_land_draw_368 = {
#include "stg30/sky_land_tex/rush_sky_land_draw_368.inc"
};
static NJS_CNK_OBJECT rush_sky_land_draw_367 = {
#include "stg30/sky_land_tex/rush_sky_land_draw_367.inc"
};
static NJS_CNK_OBJECT rush_sky_land_draw_366 = {
#include "stg30/sky_land_tex/rush_sky_land_draw_366.inc"
};
static NJS_CNK_OBJECT rush_sky_land_draw_365 = {
#include "stg30/sky_land_tex/rush_sky_land_draw_365.inc"
};
static NJS_CNK_OBJECT rush_sky_land_draw_364 = {
#include "stg30/sky_land_tex/rush_sky_land_draw_364.inc"
};
static NJS_CNK_OBJECT rush_sky_land_draw_363 = {
#include "stg30/sky_land_tex/rush_sky_land_draw_363.inc"
};
static NJS_CNK_OBJECT rush_sky_land_draw_362 = {
#include "stg30/sky_land_tex/rush_sky_land_draw_362.inc"
};
static NJS_CNK_OBJECT rush_sky_land_draw_361 = {
#include "stg30/sky_land_tex/rush_sky_land_draw_361.inc"
};
static NJS_CNK_OBJECT rush_sky_land_draw_360 = {
#include "stg30/sky_land_tex/rush_sky_land_draw_360.inc"
};
static NJS_CNK_OBJECT rush_sky_land_draw_359 = {
#include "stg30/sky_land_tex/rush_sky_land_draw_359.inc"
};
static NJS_CNK_OBJECT rush_sky_land_draw_358 = {
#include "stg30/sky_land_tex/rush_sky_land_draw_358.inc"
};
static NJS_CNK_OBJECT rush_sky_land_draw_357 = {
#include "stg30/sky_land_tex/rush_sky_land_draw_357.inc"
};
static NJS_CNK_OBJECT rush_sky_land_draw_356 = {
#include "stg30/sky_land_tex/rush_sky_land_draw_356.inc"
};
static NJS_CNK_OBJECT rush_sky_land_draw_355 = {
#include "stg30/sky_land_tex/rush_sky_land_draw_355.inc"
};
static NJS_CNK_OBJECT rush_sky_land_draw_354 = {
#include "stg30/sky_land_tex/rush_sky_land_draw_354.inc"
};
static NJS_CNK_OBJECT rush_sky_land_draw_353 = {
#include "stg30/sky_land_tex/rush_sky_land_draw_353.inc"
};
static NJS_CNK_OBJECT rush_sky_land_draw_352 = {
#include "stg30/sky_land_tex/rush_sky_land_draw_352.inc"
};
static NJS_CNK_OBJECT rush_sky_land_draw_350 = {
#include "stg30/sky_land_tex/rush_sky_land_draw_350.inc"
};
static NJS_CNK_OBJECT rush_sky_land_draw_349 = {
#include "stg30/sky_land_tex/rush_sky_land_draw_349.inc"
};
static NJS_CNK_OBJECT rush_sky_land_draw_346 = {
#include "stg30/sky_land_tex/rush_sky_land_draw_346.inc"
};
static NJS_CNK_OBJECT rush_sky_land_draw_345 = {
#include "stg30/sky_land_tex/rush_sky_land_draw_345.inc"
};
static NJS_CNK_OBJECT rush_sky_land_draw_344 = {
#include "stg30/sky_land_tex/rush_sky_land_draw_344.inc"
};
static NJS_CNK_OBJECT rush_sky_land_draw_343 = {
#include "stg30/sky_land_tex/rush_sky_land_draw_343.inc"
};
static NJS_CNK_OBJECT rush_sky_land_draw_342 = {
#include "stg30/sky_land_tex/rush_sky_land_draw_342.inc"
};
static NJS_CNK_OBJECT rush_sky_land_draw_341 = {
#include "stg30/sky_land_tex/rush_sky_land_draw_341.inc"
};
static NJS_CNK_OBJECT rush_sky_land_draw_340 = {
#include "stg30/sky_land_tex/rush_sky_land_draw_340.inc"
};
static NJS_CNK_OBJECT rush_sky_land_draw_339 = {
#include "stg30/sky_land_tex/rush_sky_land_draw_339.inc"
};
static NJS_CNK_OBJECT rush_sky_land_draw_338 = {
#include "stg30/sky_land_tex/rush_sky_land_draw_338.inc"
};
static NJS_CNK_OBJECT rush_sky_land_draw_337 = {
#include "stg30/sky_land_tex/rush_sky_land_draw_337.inc"
};
static NJS_CNK_OBJECT rush_sky_land_draw_336 = {
#include "stg30/sky_land_tex/rush_sky_land_draw_336.inc"
};
static NJS_CNK_OBJECT rush_sky_land_draw_335 = {
#include "stg30/sky_land_tex/rush_sky_land_draw_335.inc"
};
static NJS_CNK_OBJECT rush_sky_land_draw_334 = {
#include "stg30/sky_land_tex/rush_sky_land_draw_334.inc"
};
static NJS_CNK_OBJECT rush_sky_land_draw_333 = {
#include "stg30/sky_land_tex/rush_sky_land_draw_333.inc"
};
static NJS_CNK_OBJECT rush_sky_land_draw_332 = {
#include "stg30/sky_land_tex/rush_sky_land_draw_332.inc"
};
static NJS_CNK_OBJECT rush_sky_land_draw_331 = {
#include "stg30/sky_land_tex/rush_sky_land_draw_331.inc"
};
static NJS_CNK_OBJECT rush_sky_land_draw_330 = {
#include "stg30/sky_land_tex/rush_sky_land_draw_330.inc"
};
static NJS_CNK_OBJECT rush_sky_land_draw_329 = {
#include "stg30/sky_land_tex/rush_sky_land_draw_329.inc"
};
static NJS_CNK_OBJECT rush_sky_land_draw_328 = {
#include "stg30/sky_land_tex/rush_sky_land_draw_328.inc"
};
static NJS_CNK_OBJECT rush_sky_land_draw_326 = {
#include "stg30/sky_land_tex/rush_sky_land_draw_326.inc"
};
static NJS_CNK_OBJECT rush_sky_land_draw_325 = {
#include "stg30/sky_land_tex/rush_sky_land_draw_325.inc"
};
static NJS_CNK_OBJECT rush_sky_land_draw_324 = {
#include "stg30/sky_land_tex/rush_sky_land_draw_324.inc"
};
static NJS_CNK_OBJECT rush_sky_land_draw_323 = {
#include "stg30/sky_land_tex/rush_sky_land_draw_323.inc"
};
static NJS_CNK_OBJECT rush_sky_land_draw_322 = {
#include "stg30/sky_land_tex/rush_sky_land_draw_322.inc"
};
static NJS_CNK_OBJECT rush_sky_land_draw_321 = {
#include "stg30/sky_land_tex/rush_sky_land_draw_321.inc"
};
static NJS_CNK_OBJECT rush_sky_land_draw_320 = {
#include "stg30/sky_land_tex/rush_sky_land_draw_320.inc"
};
static NJS_CNK_OBJECT rush_sky_land_draw_319 = {
#include "stg30/sky_land_tex/rush_sky_land_draw_319.inc"
};
static NJS_CNK_OBJECT rush_sky_land_draw_318 = {
#include "stg30/sky_land_tex/rush_sky_land_draw_318.inc"
};
static NJS_CNK_OBJECT rush_sky_land_draw_317 = {
#include "stg30/sky_land_tex/rush_sky_land_draw_317.inc"
};
static NJS_CNK_OBJECT rush_sky_land_draw_316 = {
#include "stg30/sky_land_tex/rush_sky_land_draw_316.inc"
};
static NJS_CNK_OBJECT rush_sky_land_draw_315 = {
#include "stg30/sky_land_tex/rush_sky_land_draw_315.inc"
};
static NJS_CNK_OBJECT rush_sky_land_draw_314 = {
#include "stg30/sky_land_tex/rush_sky_land_draw_314.inc"
};
static NJS_CNK_OBJECT rush_sky_land_draw_313 = {
#include "stg30/sky_land_tex/rush_sky_land_draw_313.inc"
};
static NJS_CNK_OBJECT rush_sky_land_draw_312 = {
#include "stg30/sky_land_tex/rush_sky_land_draw_312.inc"
};
static NJS_CNK_OBJECT rush_sky_land_draw_311 = {
#include "stg30/sky_land_tex/rush_sky_land_draw_311.inc"
};
static NJS_CNK_OBJECT rush_sky_land_draw_310 = {
#include "stg30/sky_land_tex/rush_sky_land_draw_310.inc"
};
static NJS_CNK_OBJECT rush_sky_land_draw_309 = {
#include "stg30/sky_land_tex/rush_sky_land_draw_309.inc"
};
static NJS_CNK_OBJECT rush_sky_land_draw_308 = {
#include "stg30/sky_land_tex/rush_sky_land_draw_308.inc"
};
static NJS_CNK_OBJECT rush_sky_land_draw_307 = {
#include "stg30/sky_land_tex/rush_sky_land_draw_307.inc"
};
static NJS_CNK_OBJECT rush_sky_land_draw_306 = {
#include "stg30/sky_land_tex/rush_sky_land_draw_306.inc"
};
static NJS_CNK_OBJECT rush_sky_land_draw_305 = {
#include "stg30/sky_land_tex/rush_sky_land_draw_305.inc"
};
static NJS_CNK_OBJECT rush_sky_land_draw_304 = {
#include "stg30/sky_land_tex/rush_sky_land_draw_304.inc"
};
static NJS_CNK_OBJECT rush_sky_land_draw_303 = {
#include "stg30/sky_land_tex/rush_sky_land_draw_303.inc"
};
static NJS_CNK_OBJECT rush_sky_land_draw_302 = {
#include "stg30/sky_land_tex/rush_sky_land_draw_302.inc"
};
static NJS_CNK_OBJECT rush_sky_land_draw_301 = {
#include "stg30/sky_land_tex/rush_sky_land_draw_301.inc"
};
static NJS_CNK_OBJECT rush_sky_land_draw_300 = {
#include "stg30/sky_land_tex/rush_sky_land_draw_300.inc"
};
static NJS_CNK_OBJECT rush_sky_land_draw_299 = {
#include "stg30/sky_land_tex/rush_sky_land_draw_299.inc"
};
static NJS_CNK_OBJECT rush_sky_land_draw_298 = {
#include "stg30/sky_land_tex/rush_sky_land_draw_298.inc"
};
static NJS_CNK_OBJECT rush_sky_land_draw_297 = {
#include "stg30/sky_land_tex/rush_sky_land_draw_297.inc"
};
static NJS_CNK_OBJECT rush_sky_land_draw_296 = {
#include "stg30/sky_land_tex/rush_sky_land_draw_296.inc"
};
static NJS_CNK_OBJECT rush_sky_land_draw_295 = {
#include "stg30/sky_land_tex/rush_sky_land_draw_295.inc"
};
static NJS_CNK_OBJECT rush_sky_land_draw_292 = {
#include "stg30/sky_land_tex/rush_sky_land_draw_292.inc"
};
static NJS_CNK_OBJECT rush_sky_land_draw_290 = {
#include "stg30/sky_land_tex/rush_sky_land_draw_290.inc"
};
static NJS_CNK_OBJECT rush_sky_land_draw_289 = {
#include "stg30/sky_land_tex/rush_sky_land_draw_289.inc"
};
static NJS_CNK_OBJECT rush_sky_land_draw_288 = {
#include "stg30/sky_land_tex/rush_sky_land_draw_288.inc"
};
static NJS_CNK_OBJECT rush_sky_land_draw_286 = {
#include "stg30/sky_land_tex/rush_sky_land_draw_286.inc"
};
static NJS_CNK_OBJECT rush_sky_land_draw_285 = {
#include "stg30/sky_land_tex/rush_sky_land_draw_285.inc"
};
static NJS_CNK_OBJECT rush_sky_land_draw_284 = {
#include "stg30/sky_land_tex/rush_sky_land_draw_284.inc"
};
static NJS_CNK_OBJECT rush_sky_land_draw_282 = {
#include "stg30/sky_land_tex/rush_sky_land_draw_282.inc"
};
static NJS_CNK_OBJECT rush_sky_land_draw_281 = {
#include "stg30/sky_land_tex/rush_sky_land_draw_281.inc"
};
static NJS_CNK_OBJECT rush_sky_land_draw_280 = {
#include "stg30/sky_land_tex/rush_sky_land_draw_280.inc"
};
static NJS_CNK_OBJECT rush_sky_land_draw_279 = {
#include "stg30/sky_land_tex/rush_sky_land_draw_279.inc"
};
static NJS_CNK_OBJECT rush_sky_land_draw_278 = {
#include "stg30/sky_land_tex/rush_sky_land_draw_278.inc"
};
static NJS_CNK_OBJECT rush_sky_land_draw_277 = {
#include "stg30/sky_land_tex/rush_sky_land_draw_277.inc"
};
static NJS_CNK_OBJECT rush_sky_land_draw_276 = {
#include "stg30/sky_land_tex/rush_sky_land_draw_276.inc"
};
static NJS_CNK_OBJECT rush_sky_land_draw_275 = {
#include "stg30/sky_land_tex/rush_sky_land_draw_275.inc"
};
static NJS_CNK_OBJECT rush_sky_land_draw_274 = {
#include "stg30/sky_land_tex/rush_sky_land_draw_274.inc"
};
static NJS_CNK_OBJECT rush_sky_land_draw_273 = {
#include "stg30/sky_land_tex/rush_sky_land_draw_273.inc"
};
static NJS_CNK_OBJECT rush_sky_land_draw_272 = {
#include "stg30/sky_land_tex/rush_sky_land_draw_272.inc"
};
static NJS_CNK_OBJECT rush_sky_land_draw_271 = {
#include "stg30/sky_land_tex/rush_sky_land_draw_271.inc"
};
static NJS_CNK_OBJECT rush_sky_land_draw_270 = {
#include "stg30/sky_land_tex/rush_sky_land_draw_270.inc"
};
static NJS_CNK_OBJECT rush_sky_land_draw_269 = {
#include "stg30/sky_land_tex/rush_sky_land_draw_269.inc"
};
static NJS_CNK_OBJECT rush_sky_land_draw_268 = {
#include "stg30/sky_land_tex/rush_sky_land_draw_268.inc"
};
static NJS_CNK_OBJECT rush_sky_land_draw_267 = {
#include "stg30/sky_land_tex/rush_sky_land_draw_267.inc"
};
static NJS_CNK_OBJECT rush_sky_land_draw_266 = {
#include "stg30/sky_land_tex/rush_sky_land_draw_266.inc"
};
static NJS_CNK_OBJECT rush_sky_land_draw_265 = {
#include "stg30/sky_land_tex/rush_sky_land_draw_265.inc"
};
static NJS_CNK_OBJECT rush_sky_land_draw_264 = {
#include "stg30/sky_land_tex/rush_sky_land_draw_264.inc"
};
static NJS_CNK_OBJECT rush_sky_land_draw_263 = {
#include "stg30/sky_land_tex/rush_sky_land_draw_263.inc"
};
static NJS_CNK_OBJECT rush_sky_land_draw_262 = {
#include "stg30/sky_land_tex/rush_sky_land_draw_262.inc"
};
static NJS_CNK_OBJECT rush_sky_land_draw_261 = {
#include "stg30/sky_land_tex/rush_sky_land_draw_261.inc"
};
static NJS_CNK_OBJECT rush_sky_land_draw_260 = {
#include "stg30/sky_land_tex/rush_sky_land_draw_260.inc"
};
static NJS_CNK_OBJECT rush_sky_land_draw_259 = {
#include "stg30/sky_land_tex/rush_sky_land_draw_259.inc"
};
static NJS_CNK_OBJECT rush_sky_land_draw_258 = {
#include "stg30/sky_land_tex/rush_sky_land_draw_258.inc"
};
static NJS_CNK_OBJECT rush_sky_land_draw_257 = {
#include "stg30/sky_land_tex/rush_sky_land_draw_257.inc"
};
static NJS_CNK_OBJECT rush_sky_land_draw_256 = {
#include "stg30/sky_land_tex/rush_sky_land_draw_256.inc"
};
static NJS_CNK_OBJECT rush_sky_land_draw_255 = {
#include "stg30/sky_land_tex/rush_sky_land_draw_255.inc"
};
static NJS_CNK_OBJECT rush_sky_land_draw_254 = {
#include "stg30/sky_land_tex/rush_sky_land_draw_254.inc"
};
static NJS_CNK_OBJECT rush_sky_land_draw_253 = {
#include "stg30/sky_land_tex/rush_sky_land_draw_253.inc"
};
static NJS_CNK_OBJECT rush_sky_land_draw_252 = {
#include "stg30/sky_land_tex/rush_sky_land_draw_252.inc"
};
static NJS_CNK_OBJECT rush_sky_land_draw_251 = {
#include "stg30/sky_land_tex/rush_sky_land_draw_251.inc"
};
static NJS_CNK_OBJECT rush_sky_land_draw_250 = {
#include "stg30/sky_land_tex/rush_sky_land_draw_250.inc"
};
static NJS_CNK_OBJECT rush_sky_land_draw_249 = {
#include "stg30/sky_land_tex/rush_sky_land_draw_249.inc"
};
static NJS_CNK_OBJECT rush_sky_land_draw_248 = {
#include "stg30/sky_land_tex/rush_sky_land_draw_248.inc"
};
static NJS_CNK_OBJECT rush_sky_land_draw_247 = {
#include "stg30/sky_land_tex/rush_sky_land_draw_247.inc"
};
static NJS_CNK_OBJECT rush_sky_land_draw_246 = {
#include "stg30/sky_land_tex/rush_sky_land_draw_246.inc"
};
static NJS_CNK_OBJECT rush_sky_land_draw_245 = {
#include "stg30/sky_land_tex/rush_sky_land_draw_245.inc"
};
static NJS_CNK_OBJECT rush_sky_land_draw_244 = {
#include "stg30/sky_land_tex/rush_sky_land_draw_244.inc"
};
static NJS_CNK_OBJECT rush_sky_land_draw_243 = {
#include "stg30/sky_land_tex/rush_sky_land_draw_243.inc"
};
static NJS_CNK_OBJECT rush_sky_land_draw_241 = {
#include "stg30/sky_land_tex/rush_sky_land_draw_241.inc"
};
static NJS_CNK_OBJECT rush_sky_land_draw_240 = {
#include "stg30/sky_land_tex/rush_sky_land_draw_240.inc"
};
static NJS_CNK_OBJECT rush_sky_land_draw_239 = {
#include "stg30/sky_land_tex/rush_sky_land_draw_239.inc"
};
static NJS_CNK_OBJECT rush_sky_land_draw_238 = {
#include "stg30/sky_land_tex/rush_sky_land_draw_238.inc"
};
static NJS_CNK_OBJECT rush_sky_land_draw_237 = {
#include "stg30/sky_land_tex/rush_sky_land_draw_237.inc"
};
static NJS_CNK_OBJECT rush_sky_land_draw_236 = {
#include "stg30/sky_land_tex/rush_sky_land_draw_236.inc"
};
static NJS_CNK_OBJECT rush_sky_land_draw_235 = {
#include "stg30/sky_land_tex/rush_sky_land_draw_235.inc"
};
static NJS_CNK_OBJECT rush_sky_land_draw_234 = {
#include "stg30/sky_land_tex/rush_sky_land_draw_234.inc"
};
static NJS_CNK_OBJECT rush_sky_land_draw_233 = {
#include "stg30/sky_land_tex/rush_sky_land_draw_233.inc"
};
static NJS_CNK_OBJECT rush_sky_land_draw_232 = {
#include "stg30/sky_land_tex/rush_sky_land_draw_232.inc"
};
static NJS_CNK_OBJECT rush_sky_land_draw_231 = {
#include "stg30/sky_land_tex/rush_sky_land_draw_231.inc"
};
static NJS_CNK_OBJECT rush_sky_land_draw_230 = {
#include "stg30/sky_land_tex/rush_sky_land_draw_230.inc"
};
static NJS_CNK_OBJECT rush_sky_land_draw_229 = {
#include "stg30/sky_land_tex/rush_sky_land_draw_229.inc"
};
static NJS_CNK_OBJECT rush_sky_land_draw_228 = {
#include "stg30/sky_land_tex/rush_sky_land_draw_228.inc"
};
static NJS_CNK_OBJECT rush_sky_land_draw_227 = {
#include "stg30/sky_land_tex/rush_sky_land_draw_227.inc"
};
static NJS_CNK_OBJECT rush_sky_land_draw_226 = {
#include "stg30/sky_land_tex/rush_sky_land_draw_226.inc"
};
static NJS_CNK_OBJECT rush_sky_land_draw_225 = {
#include "stg30/sky_land_tex/rush_sky_land_draw_225.inc"
};
static NJS_CNK_OBJECT rush_sky_land_draw_224 = {
#include "stg30/sky_land_tex/rush_sky_land_draw_224.inc"
};
static NJS_CNK_OBJECT rush_sky_land_draw_223 = {
#include "stg30/sky_land_tex/rush_sky_land_draw_223.inc"
};
static NJS_CNK_OBJECT rush_sky_land_draw_222 = {
#include "stg30/sky_land_tex/rush_sky_land_draw_222.inc"
};
static NJS_CNK_OBJECT rush_sky_land_draw_221 = {
#include "stg30/sky_land_tex/rush_sky_land_draw_221.inc"
};
static NJS_CNK_OBJECT rush_sky_land_draw_220 = {
#include "stg30/sky_land_tex/rush_sky_land_draw_220.inc"
};
static NJS_CNK_OBJECT rush_sky_land_draw_219 = {
#include "stg30/sky_land_tex/rush_sky_land_draw_219.inc"
};
static NJS_CNK_OBJECT rush_sky_land_draw_218 = {
#include "stg30/sky_land_tex/rush_sky_land_draw_218.inc"
};
static NJS_CNK_OBJECT rush_sky_land_draw_217 = {
#include "stg30/sky_land_tex/rush_sky_land_draw_217.inc"
};
static NJS_CNK_OBJECT rush_sky_land_draw_216 = {
#include "stg30/sky_land_tex/rush_sky_land_draw_216.inc"
};
static NJS_CNK_OBJECT rush_sky_land_draw_215 = {
#include "stg30/sky_land_tex/rush_sky_land_draw_215.inc"
};
static NJS_CNK_OBJECT rush_sky_land_draw_214 = {
#include "stg30/sky_land_tex/rush_sky_land_draw_214.inc"
};
static NJS_CNK_OBJECT rush_sky_land_draw_213 = {
#include "stg30/sky_land_tex/rush_sky_land_draw_213.inc"
};
static NJS_CNK_OBJECT rush_sky_land_draw_212 = {
#include "stg30/sky_land_tex/rush_sky_land_draw_212.inc"
};
static NJS_CNK_OBJECT rush_sky_land_draw_211 = {
#include "stg30/sky_land_tex/rush_sky_land_draw_211.inc"
};
static NJS_CNK_OBJECT rush_sky_land_draw_210 = {
#include "stg30/sky_land_tex/rush_sky_land_draw_210.inc"
};
static NJS_CNK_OBJECT rush_sky_land_draw_209 = {
#include "stg30/sky_land_tex/rush_sky_land_draw_209.inc"
};
static NJS_CNK_OBJECT rush_sky_land_draw_208 = {
#include "stg30/sky_land_tex/rush_sky_land_draw_208.inc"
};
static NJS_CNK_OBJECT rush_sky_land_draw_206 = {
#include "stg30/sky_land_tex/rush_sky_land_draw_206.inc"
};
static NJS_CNK_OBJECT rush_sky_land_draw_205 = {
#include "stg30/sky_land_tex/rush_sky_land_draw_205.inc"
};
static NJS_CNK_OBJECT rush_sky_land_draw_204 = {
#include "stg30/sky_land_tex/rush_sky_land_draw_204.inc"
};
static NJS_CNK_OBJECT rush_sky_land_draw_203 = {
#include "stg30/sky_land_tex/rush_sky_land_draw_203.inc"
};
static NJS_CNK_OBJECT rush_sky_land_draw_202 = {
#include "stg30/sky_land_tex/rush_sky_land_draw_202.inc"
};
static NJS_CNK_OBJECT rush_sky_land_draw_201 = {
#include "stg30/sky_land_tex/rush_sky_land_draw_201.inc"
};
static NJS_CNK_OBJECT rush_sky_land_draw_200 = {
#include "stg30/sky_land_tex/rush_sky_land_draw_200.inc"
};
static NJS_CNK_OBJECT rush_sky_land_draw_199 = {
#include "stg30/sky_land_tex/rush_sky_land_draw_199.inc"
};
static NJS_CNK_OBJECT rush_sky_land_draw_198 = {
#include "stg30/sky_land_tex/rush_sky_land_draw_198.inc"
};
static NJS_CNK_OBJECT rush_sky_land_draw_197 = {
#include "stg30/sky_land_tex/rush_sky_land_draw_197.inc"
};
static NJS_CNK_OBJECT rush_sky_land_draw_196 = {
#include "stg30/sky_land_tex/rush_sky_land_draw_196.inc"
};
static NJS_CNK_OBJECT rush_sky_land_draw_194 = {
#include "stg30/sky_land_tex/rush_sky_land_draw_194.inc"
};
static NJS_CNK_OBJECT rush_sky_land_draw_189 = {
#include "stg30/sky_land_tex/rush_sky_land_draw_189.inc"
};
static NJS_CNK_OBJECT rush_sky_land_draw_188 = {
#include "stg30/sky_land_tex/rush_sky_land_draw_188.inc"
};
static NJS_CNK_OBJECT rush_sky_land_draw_187 = {
#include "stg30/sky_land_tex/rush_sky_land_draw_187.inc"
};
static NJS_CNK_OBJECT rush_sky_land_draw_186 = {
#include "stg30/sky_land_tex/rush_sky_land_draw_186.inc"
};
static NJS_CNK_OBJECT rush_sky_land_draw_185 = {
#include "stg30/sky_land_tex/rush_sky_land_draw_185.inc"
};
static NJS_CNK_OBJECT rush_sky_land_draw_184 = {
#include "stg30/sky_land_tex/rush_sky_land_draw_184.inc"
};
static NJS_CNK_OBJECT rush_sky_land_draw_183 = {
#include "stg30/sky_land_tex/rush_sky_land_draw_183.inc"
};
static NJS_CNK_OBJECT rush_sky_land_draw_182 = {
#include "stg30/sky_land_tex/rush_sky_land_draw_182.inc"
};
static NJS_CNK_OBJECT rush_sky_land_draw_181 = {
#include "stg30/sky_land_tex/rush_sky_land_draw_181.inc"
};
static NJS_CNK_OBJECT rush_sky_land_draw_180 = {
#include "stg30/sky_land_tex/rush_sky_land_draw_180.inc"
};
static NJS_CNK_OBJECT rush_sky_land_draw_179 = {
#include "stg30/sky_land_tex/rush_sky_land_draw_179.inc"
};
static NJS_CNK_OBJECT rush_sky_land_draw_178 = {
#include "stg30/sky_land_tex/rush_sky_land_draw_178.inc"
};
static NJS_CNK_OBJECT rush_sky_land_draw_177 = {
#include "stg30/sky_land_tex/rush_sky_land_draw_177.inc"
};
static NJS_CNK_OBJECT rush_sky_land_draw_176 = {
#include "stg30/sky_land_tex/rush_sky_land_draw_176.inc"
};
static NJS_CNK_OBJECT rush_sky_land_draw_173 = {
#include "stg30/sky_land_tex/rush_sky_land_draw_173.inc"
};
static NJS_CNK_OBJECT rush_sky_land_draw_172 = {
#include "stg30/sky_land_tex/rush_sky_land_draw_172.inc"
};
static NJS_CNK_OBJECT rush_sky_land_draw_171 = {
#include "stg30/sky_land_tex/rush_sky_land_draw_171.inc"
};
static NJS_CNK_OBJECT rush_sky_land_draw_169 = {
#include "stg30/sky_land_tex/rush_sky_land_draw_169.inc"
};
static NJS_CNK_OBJECT rush_sky_land_draw_168 = {
#include "stg30/sky_land_tex/rush_sky_land_draw_168.inc"
};
static NJS_CNK_OBJECT rush_sky_land_draw_167 = {
#include "stg30/sky_land_tex/rush_sky_land_draw_167.inc"
};
static NJS_CNK_OBJECT rush_sky_land_draw_166 = {
#include "stg30/sky_land_tex/rush_sky_land_draw_166.inc"
};
static NJS_CNK_OBJECT rush_sky_land_draw_165 = {
#include "stg30/sky_land_tex/rush_sky_land_draw_165.inc"
};
static NJS_CNK_OBJECT rush_sky_land_draw_164 = {
#include "stg30/sky_land_tex/rush_sky_land_draw_164.inc"
};
static NJS_CNK_OBJECT rush_sky_land_draw_163 = {
#include "stg30/sky_land_tex/rush_sky_land_draw_163.inc"
};
static NJS_CNK_OBJECT rush_sky_land_draw_162 = {
#include "stg30/sky_land_tex/rush_sky_land_draw_162.inc"
};
static NJS_CNK_OBJECT rush_sky_land_draw_161 = {
#include "stg30/sky_land_tex/rush_sky_land_draw_161.inc"
};
static NJS_CNK_OBJECT rush_sky_land_draw_160 = {
#include "stg30/sky_land_tex/rush_sky_land_draw_160.inc"
};
static NJS_CNK_OBJECT rush_sky_land_draw_159 = {
#include "stg30/sky_land_tex/rush_sky_land_draw_159.inc"
};
static NJS_CNK_OBJECT rush_sky_land_draw_158 = {
#include "stg30/sky_land_tex/rush_sky_land_draw_158.inc"
};
static NJS_CNK_OBJECT rush_sky_land_draw_157 = {
#include "stg30/sky_land_tex/rush_sky_land_draw_157.inc"
};
static NJS_CNK_OBJECT rush_sky_land_draw_156 = {
#include "stg30/sky_land_tex/rush_sky_land_draw_156.inc"
};
static NJS_CNK_OBJECT rush_sky_land_draw_155 = {
#include "stg30/sky_land_tex/rush_sky_land_draw_155.inc"
};
static NJS_CNK_OBJECT rush_sky_land_draw_154 = {
#include "stg30/sky_land_tex/rush_sky_land_draw_154.inc"
};
static NJS_CNK_OBJECT rush_sky_land_draw_153 = {
#include "stg30/sky_land_tex/rush_sky_land_draw_153.inc"
};
static NJS_CNK_OBJECT rush_sky_land_draw_152 = {
#include "stg30/sky_land_tex/rush_sky_land_draw_152.inc"
};
static NJS_CNK_OBJECT rush_sky_land_draw_150 = {
#include "stg30/sky_land_tex/rush_sky_land_draw_150.inc"
};
static NJS_CNK_OBJECT rush_sky_land_draw_149 = {
#include "stg30/sky_land_tex/rush_sky_land_draw_149.inc"
};
static NJS_CNK_OBJECT rush_sky_land_draw_148 = {
#include "stg30/sky_land_tex/rush_sky_land_draw_148.inc"
};
static NJS_CNK_OBJECT rush_sky_land_draw_147 = {
#include "stg30/sky_land_tex/rush_sky_land_draw_147.inc"
};
static NJS_CNK_OBJECT rush_sky_land_draw_145 = {
#include "stg30/sky_land_tex/rush_sky_land_draw_145.inc"
};
static NJS_CNK_OBJECT rush_sky_land_draw_144 = {
#include "stg30/sky_land_tex/rush_sky_land_draw_144.inc"
};
static NJS_CNK_OBJECT rush_sky_land_draw_143 = {
#include "stg30/sky_land_tex/rush_sky_land_draw_143.inc"
};
static NJS_CNK_OBJECT rush_sky_land_draw_142 = {
#include "stg30/sky_land_tex/rush_sky_land_draw_142.inc"
};
static NJS_CNK_OBJECT rush_sky_land_draw_141 = {
#include "stg30/sky_land_tex/rush_sky_land_draw_141.inc"
};
static NJS_CNK_OBJECT rush_sky_land_draw_140 = {
#include "stg30/sky_land_tex/rush_sky_land_draw_140.inc"
};
static NJS_CNK_OBJECT rush_sky_land_draw_139 = {
#include "stg30/sky_land_tex/rush_sky_land_draw_139.inc"
};
static NJS_CNK_OBJECT rush_sky_land_draw_138 = {
#include "stg30/sky_land_tex/rush_sky_land_draw_138.inc"
};
static NJS_CNK_OBJECT rush_sky_land_draw_137 = {
#include "stg30/sky_land_tex/rush_sky_land_draw_137.inc"
};
static NJS_CNK_OBJECT rush_sky_land_draw_136 = {
#include "stg30/sky_land_tex/rush_sky_land_draw_136.inc"
};
static NJS_CNK_OBJECT rush_sky_land_draw_135 = {
#include "stg30/sky_land_tex/rush_sky_land_draw_135.inc"
};
static NJS_CNK_OBJECT rush_sky_land_draw_133 = {
#include "stg30/sky_land_tex/rush_sky_land_draw_133.inc"
};
static NJS_CNK_OBJECT rush_sky_land_draw_132 = {
#include "stg30/sky_land_tex/rush_sky_land_draw_132.inc"
};
static NJS_CNK_OBJECT rush_sky_land_draw_131 = {
#include "stg30/sky_land_tex/rush_sky_land_draw_131.inc"
};
static NJS_CNK_OBJECT rush_sky_land_draw_130 = {
#include "stg30/sky_land_tex/rush_sky_land_draw_130.inc"
};
static NJS_CNK_OBJECT rush_sky_land_draw_128 = {
#include "stg30/sky_land_tex/rush_sky_land_draw_128.inc"
};
static NJS_CNK_OBJECT rush_sky_land_draw_126 = {
#include "stg30/sky_land_tex/rush_sky_land_draw_126.inc"
};
static NJS_CNK_OBJECT rush_sky_land_draw_125 = {
#include "stg30/sky_land_tex/rush_sky_land_draw_125.inc"
};
static NJS_CNK_OBJECT rush_sky_land_draw_124 = {
#include "stg30/sky_land_tex/rush_sky_land_draw_124.inc"
};
static NJS_CNK_OBJECT rush_sky_land_draw_123 = {
#include "stg30/sky_land_tex/rush_sky_land_draw_123.inc"
};
static NJS_CNK_OBJECT rush_sky_land_draw_122 = {
#include "stg30/sky_land_tex/rush_sky_land_draw_122.inc"
};
static NJS_CNK_OBJECT rush_sky_land_draw_121 = {
#include "stg30/sky_land_tex/rush_sky_land_draw_121.inc"
};
static NJS_CNK_OBJECT rush_sky_land_draw_120 = {
#include "stg30/sky_land_tex/rush_sky_land_draw_120.inc"
};
static NJS_CNK_OBJECT rush_sky_land_draw_119 = {
#include "stg30/sky_land_tex/rush_sky_land_draw_119.inc"
};
static NJS_CNK_OBJECT rush_sky_land_draw_118 = {
#include "stg30/sky_land_tex/rush_sky_land_draw_118.inc"
};
static NJS_CNK_OBJECT rush_sky_land_draw_117 = {
#include "stg30/sky_land_tex/rush_sky_land_draw_117.inc"
};
static NJS_CNK_OBJECT rush_sky_land_draw_116 = {
#include "stg30/sky_land_tex/rush_sky_land_draw_116.inc"
};
static NJS_CNK_OBJECT rush_sky_land_draw_115 = {
#include "stg30/sky_land_tex/rush_sky_land_draw_115.inc"
};
static NJS_CNK_OBJECT rush_sky_land_draw_114 = {
#include "stg30/sky_land_tex/rush_sky_land_draw_114.inc"
};
static NJS_CNK_OBJECT rush_sky_land_draw_113 = {
#include "stg30/sky_land_tex/rush_sky_land_draw_113.inc"
};
static NJS_CNK_OBJECT rush_sky_land_draw_112 = {
#include "stg30/sky_land_tex/rush_sky_land_draw_112.inc"
};
static NJS_CNK_OBJECT rush_sky_land_draw_111 = {
#include "stg30/sky_land_tex/rush_sky_land_draw_111.inc"
};
static NJS_CNK_OBJECT rush_sky_land_draw_110 = {
#include "stg30/sky_land_tex/rush_sky_land_draw_110.inc"
};
static NJS_CNK_OBJECT rush_sky_land_draw_109 = {
#include "stg30/sky_land_tex/rush_sky_land_draw_109.inc"
};
static NJS_CNK_OBJECT rush_sky_land_draw_108 = {
#include "stg30/sky_land_tex/rush_sky_land_draw_108.inc"
};
static NJS_CNK_OBJECT rush_sky_land_draw_107 = {
#include "stg30/sky_land_tex/rush_sky_land_draw_107.inc"
};
static NJS_CNK_OBJECT rush_sky_land_draw_106 = {
#include "stg30/sky_land_tex/rush_sky_land_draw_106.inc"
};
static NJS_CNK_OBJECT rush_sky_land_draw_105 = {
#include "stg30/sky_land_tex/rush_sky_land_draw_105.inc"
};
static NJS_CNK_OBJECT rush_sky_land_draw_104 = {
#include "stg30/sky_land_tex/rush_sky_land_draw_104.inc"
};
static NJS_CNK_OBJECT rush_sky_land_draw_103 = {
#include "stg30/sky_land_tex/rush_sky_land_draw_103.inc"
};
static NJS_CNK_OBJECT rush_sky_land_draw_102 = {
#include "stg30/sky_land_tex/rush_sky_land_draw_102.inc"
};
static NJS_CNK_OBJECT rush_sky_land_draw_101 = {
#include "stg30/sky_land_tex/rush_sky_land_draw_101.inc"
};
static NJS_CNK_OBJECT rush_sky_land_draw_100 = {
#include "stg30/sky_land_tex/rush_sky_land_draw_100.inc"
};
static NJS_CNK_OBJECT rush_sky_land_draw_99 = {
#include "stg30/sky_land_tex/rush_sky_land_draw_99.inc"
};
static NJS_CNK_OBJECT rush_sky_land_draw_98 = {
#include "stg30/sky_land_tex/rush_sky_land_draw_98.inc"
};
static NJS_CNK_OBJECT rush_sky_land_draw_97 = {
#include "stg30/sky_land_tex/rush_sky_land_draw_97.inc"
};
static NJS_CNK_OBJECT rush_sky_land_draw_96 = {
#include "stg30/sky_land_tex/rush_sky_land_draw_96.inc"
};
static NJS_CNK_OBJECT rush_sky_land_draw_95 = {
#include "stg30/sky_land_tex/rush_sky_land_draw_95.inc"
};
static NJS_CNK_OBJECT rush_sky_land_draw_94 = {
#include "stg30/sky_land_tex/rush_sky_land_draw_94.inc"
};
static NJS_CNK_OBJECT rush_sky_land_draw_93 = {
#include "stg30/sky_land_tex/rush_sky_land_draw_93.inc"
};
static NJS_CNK_OBJECT rush_sky_land_draw_92 = {
#include "stg30/sky_land_tex/rush_sky_land_draw_92.inc"
};
static NJS_CNK_OBJECT rush_sky_land_draw_91 = {
#include "stg30/sky_land_tex/rush_sky_land_draw_91.inc"
};
static NJS_CNK_OBJECT rush_sky_land_draw_90 = {
#include "stg30/sky_land_tex/rush_sky_land_draw_90.inc"
};
static NJS_CNK_OBJECT rush_sky_land_draw_89 = {
#include "stg30/sky_land_tex/rush_sky_land_draw_89.inc"
};
static NJS_CNK_OBJECT rush_sky_land_draw_88 = {
#include "stg30/sky_land_tex/rush_sky_land_draw_88.inc"
};
static NJS_CNK_OBJECT rush_sky_land_draw_87 = {
#include "stg30/sky_land_tex/rush_sky_land_draw_87.inc"
};
static NJS_CNK_OBJECT rush_sky_land_draw_86 = {
#include "stg30/sky_land_tex/rush_sky_land_draw_86.inc"
};
static NJS_CNK_OBJECT rush_sky_land_draw_85 = {
#include "stg30/sky_land_tex/rush_sky_land_draw_85.inc"
};
static NJS_CNK_OBJECT rush_sky_land_draw_84 = {
#include "stg30/sky_land_tex/rush_sky_land_draw_84.inc"
};
static NJS_CNK_OBJECT rush_sky_land_draw_83 = {
#include "stg30/sky_land_tex/rush_sky_land_draw_83.inc"
};
static NJS_CNK_OBJECT rush_sky_land_draw_82 = {
#include "stg30/sky_land_tex/rush_sky_land_draw_82.inc"
};
static NJS_CNK_OBJECT rush_sky_land_draw_81 = {
#include "stg30/sky_land_tex/rush_sky_land_draw_81.inc"
};
static NJS_CNK_OBJECT rush_sky_land_draw_80 = {
#include "stg30/sky_land_tex/rush_sky_land_draw_80.inc"
};
static NJS_CNK_OBJECT rush_sky_land_draw_79 = {
#include "stg30/sky_land_tex/rush_sky_land_draw_79.inc"
};
static NJS_CNK_OBJECT rush_sky_land_draw_78 = {
#include "stg30/sky_land_tex/rush_sky_land_draw_78.inc"
};
static NJS_CNK_OBJECT rush_sky_land_draw_77 = {
#include "stg30/sky_land_tex/rush_sky_land_draw_77.inc"
};
static NJS_CNK_OBJECT rush_sky_land_draw_76 = {
#include "stg30/sky_land_tex/rush_sky_land_draw_76.inc"
};
static NJS_CNK_OBJECT rush_sky_land_draw_75 = {
#include "stg30/sky_land_tex/rush_sky_land_draw_75.inc"
};
static NJS_CNK_OBJECT rush_sky_land_draw_74 = {
#include "stg30/sky_land_tex/rush_sky_land_draw_74.inc"
};
static NJS_CNK_OBJECT rush_sky_land_draw_73 = {
#include "stg30/sky_land_tex/rush_sky_land_draw_73.inc"
};
static NJS_CNK_OBJECT rush_sky_land_draw_72 = {
#include "stg30/sky_land_tex/rush_sky_land_draw_72.inc"
};
static NJS_CNK_OBJECT rush_sky_land_draw_71 = {
#include "stg30/sky_land_tex/rush_sky_land_draw_71.inc"
};
static NJS_CNK_OBJECT rush_sky_land_draw_70 = {
#include "stg30/sky_land_tex/rush_sky_land_draw_70.inc"
};
static NJS_CNK_OBJECT rush_sky_land_draw_69 = {
#include "stg30/sky_land_tex/rush_sky_land_draw_69.inc"
};
static NJS_CNK_OBJECT rush_sky_land_draw_68 = {
#include "stg30/sky_land_tex/rush_sky_land_draw_68.inc"
};
static NJS_CNK_OBJECT rush_sky_land_draw_67 = {
#include "stg30/sky_land_tex/rush_sky_land_draw_67.inc"
};
static NJS_CNK_OBJECT rush_sky_land_draw_62 = {
#include "stg30/sky_land_tex/rush_sky_land_draw_62.inc"
};
static NJS_CNK_OBJECT rush_sky_land_draw_61 = {
#include "stg30/sky_land_tex/rush_sky_land_draw_61.inc"
};
static NJS_CNK_OBJECT rush_sky_land_draw_60 = {
#include "stg30/sky_land_tex/rush_sky_land_draw_60.inc"
};
static NJS_CNK_OBJECT rush_sky_land_draw_59 = {
#include "stg30/sky_land_tex/rush_sky_land_draw_59.inc"
};
static NJS_CNK_OBJECT rush_sky_land_draw_58 = {
#include "stg30/sky_land_tex/rush_sky_land_draw_58.inc"
};
static NJS_CNK_OBJECT rush_sky_land_draw_57 = {
#include "stg30/sky_land_tex/rush_sky_land_draw_57.inc"
};
static NJS_CNK_OBJECT rush_sky_land_draw_56 = {
#include "stg30/sky_land_tex/rush_sky_land_draw_56.inc"
};
static NJS_CNK_OBJECT rush_sky_land_draw_55 = {
#include "stg30/sky_land_tex/rush_sky_land_draw_55.inc"
};
static NJS_CNK_OBJECT rush_sky_land_draw_54 = {
#include "stg30/sky_land_tex/rush_sky_land_draw_54.inc"
};
static NJS_CNK_OBJECT rush_sky_land_draw_53 = {
#include "stg30/sky_land_tex/rush_sky_land_draw_53.inc"
};
static NJS_CNK_OBJECT rush_sky_land_draw_52 = {
#include "stg30/sky_land_tex/rush_sky_land_draw_52.inc"
};
static NJS_CNK_OBJECT rush_sky_land_draw_51 = {
#include "stg30/sky_land_tex/rush_sky_land_draw_51.inc"
};
static NJS_CNK_OBJECT rush_sky_land_draw_50 = {
#include "stg30/sky_land_tex/rush_sky_land_draw_50.inc"
};
static NJS_CNK_OBJECT rush_sky_land_draw_49 = {
#include "stg30/sky_land_tex/rush_sky_land_draw_49.inc"
};
static NJS_CNK_OBJECT rush_sky_land_draw_48 = {
#include "stg30/sky_land_tex/rush_sky_land_draw_48.inc"
};
static NJS_CNK_OBJECT rush_sky_land_draw_47 = {
#include "stg30/sky_land_tex/rush_sky_land_draw_47.inc"
};
static NJS_CNK_OBJECT rush_sky_land_draw_46 = {
#include "stg30/sky_land_tex/rush_sky_land_draw_46.inc"
};
static NJS_CNK_OBJECT rush_sky_land_draw_45 = {
#include "stg30/sky_land_tex/rush_sky_land_draw_45.inc"
};
static NJS_CNK_OBJECT rush_sky_land_draw_44 = {
#include "stg30/sky_land_tex/rush_sky_land_draw_44.inc"
};
static NJS_CNK_OBJECT rush_sky_land_draw_43 = {
#include "stg30/sky_land_tex/rush_sky_land_draw_43.inc"
};
static NJS_CNK_OBJECT rush_sky_land_draw_42 = {
#include "stg30/sky_land_tex/rush_sky_land_draw_42.inc"
};
static NJS_CNK_OBJECT rush_sky_land_draw_41 = {
#include "stg30/sky_land_tex/rush_sky_land_draw_41.inc"
};
static NJS_CNK_OBJECT rush_sky_land_draw_40 = {
#include "stg30/sky_land_tex/rush_sky_land_draw_40.inc"
};
static NJS_CNK_OBJECT rush_sky_land_draw_39 = {
#include "stg30/sky_land_tex/rush_sky_land_draw_39.inc"
};
static NJS_CNK_OBJECT rush_sky_land_draw_38 = {
#include "stg30/sky_land_tex/rush_sky_land_draw_38.inc"
};
static NJS_CNK_OBJECT rush_sky_land_draw_37 = {
#include "stg30/sky_land_tex/rush_sky_land_draw_37.inc"
};
static NJS_CNK_OBJECT rush_sky_land_draw_36 = {
#include "stg30/sky_land_tex/rush_sky_land_draw_36.inc"
};
static NJS_CNK_OBJECT rush_sky_land_draw_35 = {
#include "stg30/sky_land_tex/rush_sky_land_draw_35.inc"
};
static NJS_CNK_OBJECT rush_sky_land_draw_34 = {
#include "stg30/sky_land_tex/rush_sky_land_draw_34.inc"
};
static NJS_CNK_OBJECT rush_sky_land_draw_33 = {
#include "stg30/sky_land_tex/rush_sky_land_draw_33.inc"
};
static NJS_CNK_OBJECT rush_sky_land_draw_32 = {
#include "stg30/sky_land_tex/rush_sky_land_draw_32.inc"
};
static NJS_CNK_OBJECT rush_sky_land_draw_31 = {
#include "stg30/sky_land_tex/rush_sky_land_draw_31.inc"
};
static NJS_CNK_OBJECT rush_sky_land_draw_30 = {
#include "stg30/sky_land_tex/rush_sky_land_draw_30.inc"
};
static NJS_CNK_OBJECT rush_sky_land_draw_29 = {
#include "stg30/sky_land_tex/rush_sky_land_draw_29.inc"
};
static NJS_CNK_OBJECT rush_sky_land_draw_28 = {
#include "stg30/sky_land_tex/rush_sky_land_draw_28.inc"
};
static NJS_CNK_OBJECT rush_sky_land_draw_27 = {
#include "stg30/sky_land_tex/rush_sky_land_draw_27.inc"
};
static NJS_CNK_OBJECT rush_sky_land_draw_26 = {
#include "stg30/sky_land_tex/rush_sky_land_draw_26.inc"
};
static NJS_CNK_OBJECT rush_sky_land_draw_25 = {
#include "stg30/sky_land_tex/rush_sky_land_draw_25.inc"
};
static NJS_CNK_OBJECT rush_sky_land_draw_24 = {
#include "stg30/sky_land_tex/rush_sky_land_draw_24.inc"
};
static NJS_CNK_OBJECT rush_sky_land_draw_23 = {
#include "stg30/sky_land_tex/rush_sky_land_draw_23.inc"
};
static NJS_CNK_OBJECT rush_sky_land_draw_22 = {
#include "stg30/sky_land_tex/rush_sky_land_draw_22.inc"
};
static NJS_CNK_OBJECT rush_sky_land_draw_21 = {
#include "stg30/sky_land_tex/rush_sky_land_draw_21.inc"
};
static NJS_CNK_OBJECT rush_sky_land_draw_20 = {
#include "stg30/sky_land_tex/rush_sky_land_draw_20.inc"
};
static NJS_CNK_OBJECT rush_sky_land_draw_19 = {
#include "stg30/sky_land_tex/rush_sky_land_draw_19.inc"
};
static NJS_CNK_OBJECT rush_sky_land_draw_18 = {
#include "stg30/sky_land_tex/rush_sky_land_draw_18.inc"
};
static NJS_CNK_OBJECT rush_sky_land_draw_17 = {
#include "stg30/sky_land_tex/rush_sky_land_draw_17.inc"
};
static NJS_CNK_OBJECT rush_sky_land_draw_16 = {
#include "stg30/sky_land_tex/rush_sky_land_draw_16.inc"
};
static NJS_CNK_OBJECT rush_sky_land_draw_15 = {
#include "stg30/sky_land_tex/rush_sky_land_draw_15.inc"
};
static NJS_CNK_OBJECT rush_sky_land_draw_14 = {
#include "stg30/sky_land_tex/rush_sky_land_draw_14.inc"
};
static NJS_CNK_OBJECT rush_sky_land_draw_13 = {
#include "stg30/sky_land_tex/rush_sky_land_draw_13.inc"
};
static NJS_CNK_OBJECT rush_sky_land_draw_13_instance_1 = {
#include "stg30/sky_land_tex/rush_sky_land_draw_13_instance_1.inc"
};
static NJS_CNK_OBJECT rush_sky_land_draw_13_instance_2 = {
#include "stg30/sky_land_tex/rush_sky_land_draw_13_instance_2.inc"
};
static NJS_CNK_OBJECT rush_sky_land_draw_12 = {
#include "stg30/sky_land_tex/rush_sky_land_draw_12.inc"
};
static NJS_CNK_OBJECT rush_sky_land_draw_11 = {
#include "stg30/sky_land_tex/rush_sky_land_draw_11.inc"
};
static NJS_CNK_OBJECT rush_sky_land_draw_10 = {
#include "stg30/sky_land_tex/rush_sky_land_draw_10.inc"
};
static NJS_CNK_OBJECT rush_sky_land_draw_9 = {
#include "stg30/sky_land_tex/rush_sky_land_draw_9.inc"
};
static NJS_CNK_OBJECT rush_sky_land_draw_8 = {
#include "stg30/sky_land_tex/rush_sky_land_draw_8.inc"
};
static NJS_CNK_OBJECT rush_sky_land_draw_7 = {
#include "stg30/sky_land_tex/rush_sky_land_draw_7.inc"
};
static NJS_CNK_OBJECT rush_sky_land_draw_6 = {
#include "stg30/sky_land_tex/rush_sky_land_draw_6.inc"
};
static NJS_CNK_OBJECT rush_sky_land_draw_5 = {
#include "stg30/sky_land_tex/rush_sky_land_draw_5.inc"
};
static NJS_CNK_OBJECT rush_sky_land_draw_4 = {
#include "stg30/sky_land_tex/rush_sky_land_draw_4.inc"
};
static NJS_CNK_OBJECT rush_sky_land_draw_3 = {
#include "stg30/sky_land_tex/rush_sky_land_draw_3.inc"
};
static NJS_CNK_OBJECT rush_sky_land_draw_2 = {
#include "stg30/sky_land_tex/rush_sky_land_draw_2.inc"
};
static NJS_CNK_OBJECT rush_sky_land_draw_1 = {
#include "stg30/sky_land_tex/rush_sky_land_draw_1.inc"
};
static NJS_CNK_OBJECT rush_sky_land_draw_0 = {
#include "stg30/sky_land_tex/rush_sky_land_draw_0.inc"
};
