/* STG23.PRS's land, texture lists, chunk models and Basic models, from the
 * disc. its D lies between hidden_base.c and object_archives.c */
#include <ninja.h>
#include <sg_xpt.h>

#include "collision/world.h"
#include "stg23/dat_8c545aec.h"
#include "stg23/hidden_base.h"

/* the data up to the last that a pointer names ahead of its definition, as
 * tentative definitions in the disc's order: GCC places each datum at its
 * first definition */
static struct land_entry entries[634];
struct land_table hidden_base_land;
static NJS_TEXNAME textures[30];
NJS_TEXLIST hidden_base_land_texlist;
static Sint16 draw_469_polygons[113];
static Sint32 draw_469_vertices[83];
static NJS_CNK_MODEL draw_469_model;
static NJS_CNK_OBJECT draw_469;
static Sint16 draw_465_polygons[167];
static Sint32 draw_465_vertices[171];
static NJS_CNK_MODEL draw_465_model;
static NJS_CNK_OBJECT draw_465;
static Sint16 draw_464_polygons[1113];
static Sint32 draw_464_vertices[827];
static NJS_CNK_MODEL draw_464_model;
static NJS_CNK_OBJECT draw_464;
static Sint16 draw_462_polygons[213];
static Sint32 draw_462_vertices[127];
static NJS_CNK_MODEL draw_462_model;
static NJS_CNK_OBJECT draw_463;
static Sint16 draw_460_polygons[679];
static Sint32 draw_460_vertices[435];
static NJS_CNK_MODEL draw_460_model;
static NJS_CNK_OBJECT draw_461;
static Sint16 draw_458_polygons[213];
static Sint32 draw_458_vertices[123];
static NJS_CNK_MODEL draw_458_model;
static NJS_CNK_OBJECT draw_459;
static Sint16 draw_456_polygons[659];
static Sint32 draw_456_vertices[403];
static NJS_CNK_MODEL draw_456_model;
static NJS_CNK_OBJECT draw_457;
static Sint16 draw_455_polygons[259];
static Sint32 draw_455_vertices[211];
static NJS_CNK_MODEL draw_455_model;
static NJS_CNK_OBJECT draw_455;
static Sint16 draw_453_polygons[195];
static Sint32 draw_453_vertices[103];
static NJS_CNK_MODEL draw_453_model;
static NJS_CNK_OBJECT draw_454;
static Sint16 draw_452_polygons[1019];
static Sint32 draw_452_vertices[887];
static NJS_CNK_MODEL draw_452_model;
static NJS_CNK_OBJECT draw_452;
static Sint16 draw_451_polygons[65];
static Sint32 draw_451_vertices[67];
static NJS_CNK_MODEL draw_451_model;
static NJS_CNK_OBJECT draw_451;
static Sint16 draw_450_polygons[425];
static Sint32 draw_450_vertices[383];
static NJS_CNK_MODEL draw_450_model;
static NJS_CNK_OBJECT draw_450;
static Sint16 draw_449_polygons[633];
static Sint32 draw_449_vertices[599];
static NJS_CNK_MODEL draw_449_model;
static NJS_CNK_OBJECT draw_449;
static Sint16 draw_447_polygons[213];
static Sint32 draw_447_vertices[143];
static NJS_CNK_MODEL draw_447_model;
static NJS_CNK_OBJECT draw_448;
static Sint16 draw_444_polygons[213];
static Sint32 draw_444_vertices[143];
static NJS_CNK_MODEL draw_444_model;
static NJS_CNK_OBJECT draw_446;
static Sint16 draw_441_polygons[25];
static Sint32 draw_441_vertices[19];
static NJS_CNK_MODEL draw_441_model;
static NJS_CNK_OBJECT draw_443;
static Sint16 draw_440_polygons[221];
static Sint32 draw_440_vertices[267];
static NJS_CNK_MODEL draw_440_model;
static NJS_CNK_OBJECT draw_440;
static Sint16 draw_439_polygons[135];
static Sint32 draw_439_vertices[155];
static NJS_CNK_MODEL draw_439_model;
static NJS_CNK_OBJECT draw_439;
static Sint16 draw_438_polygons[41];
static Sint32 draw_438_vertices[39];
static NJS_CNK_MODEL draw_438_model;
static NJS_CNK_OBJECT draw_438;
static Sint16 draw_437_polygons[143];
static Sint32 draw_437_vertices[163];
static NJS_CNK_MODEL draw_437_model;
static NJS_CNK_OBJECT draw_437;
static Sint16 draw_436_polygons[83];
static Sint32 draw_436_vertices[63];
static NJS_CNK_MODEL draw_436_model;
static NJS_CNK_OBJECT draw_436;
static Sint16 draw_434_polygons[307];
static Sint32 draw_434_vertices[227];
static NJS_CNK_MODEL draw_434_model;
static NJS_CNK_OBJECT draw_435;
static Sint16 draw_433_polygons[159];
static Sint32 draw_433_vertices[131];
static NJS_CNK_MODEL draw_433_model;
static NJS_CNK_OBJECT draw_433;
static Sint16 draw_432_polygons[105];
static Sint32 draw_432_vertices[87];
static NJS_CNK_MODEL draw_432_model;
static NJS_CNK_OBJECT draw_432;
static Sint16 draw_431_polygons[95];
static Sint32 draw_431_vertices[99];
static NJS_CNK_MODEL draw_431_model;
static NJS_CNK_OBJECT draw_431;
static Sint16 draw_430_polygons[35];
static Sint32 draw_430_vertices[27];
static NJS_CNK_MODEL draw_430_model;
static NJS_CNK_OBJECT draw_430;
static Sint16 draw_429_polygons[81];
static Sint32 draw_429_vertices[79];
static NJS_CNK_MODEL draw_429_model;
static NJS_CNK_OBJECT draw_429;
static Sint16 draw_428_polygons[25];
static Sint32 draw_428_vertices[19];
static NJS_CNK_MODEL draw_428_model;
static NJS_CNK_OBJECT draw_428;
static Sint16 draw_427_polygons[89];
static Sint32 draw_427_vertices[67];
static NJS_CNK_MODEL draw_427_model;
static NJS_CNK_OBJECT draw_427;
static Sint16 draw_424_polygons[79];
static Sint32 draw_424_vertices[51];
static NJS_CNK_MODEL draw_424_model;
static NJS_CNK_OBJECT draw_426;
static Sint16 draw_421_polygons[79];
static Sint32 draw_421_vertices[51];
static NJS_CNK_MODEL draw_421_model;
static NJS_CNK_OBJECT draw_423;
static Sint16 draw_420_polygons[51];
static Sint32 draw_420_vertices[51];
static NJS_CNK_MODEL draw_420_model;
static NJS_CNK_OBJECT draw_420;
static Sint16 draw_419_polygons[51];
static Sint32 draw_419_vertices[51];
static NJS_CNK_MODEL draw_419_model;
static NJS_CNK_OBJECT draw_419;
static Sint16 draw_411_polygons[63];
static Sint32 draw_411_vertices[51];
static NJS_CNK_MODEL draw_411_model;
static NJS_CNK_OBJECT draw_418;
static Sint16 draw_407_polygons[195];
static Sint32 draw_407_vertices[103];
static NJS_CNK_MODEL draw_407_model;
static NJS_CNK_OBJECT draw_410;
static Sint16 draw_406_polygons[195];
static Sint32 draw_406_vertices[103];
static NJS_CNK_MODEL draw_406_model;
static NJS_CNK_OBJECT draw_406;
static Sint16 draw_363_polygons[203];
static Sint32 draw_363_vertices[191];
static NJS_CNK_MODEL draw_363_model;
static NJS_CNK_OBJECT draw_363;
static Sint16 draw_362_polygons[89];
static Sint32 draw_362_vertices[83];
static NJS_CNK_MODEL draw_362_model;
static NJS_CNK_OBJECT draw_362;
static Sint16 draw_361_polygons[307];
static Sint32 draw_361_vertices[223];
static NJS_CNK_MODEL draw_361_model;
static NJS_CNK_OBJECT draw_361;
static Sint16 draw_359_polygons[183];
static Sint32 draw_359_vertices[147];
static NJS_CNK_MODEL draw_359_model;
static NJS_CNK_OBJECT draw_360;
static Sint16 draw_358_polygons[153];
static Sint32 draw_358_vertices[123];
static NJS_CNK_MODEL draw_358_model;
static NJS_CNK_OBJECT draw_358;
static Sint16 draw_357_polygons[185];
static Sint32 draw_357_vertices[183];
static NJS_CNK_MODEL draw_357_model;
static NJS_CNK_OBJECT draw_357;
static Sint16 draw_356_polygons[83];
static Sint32 draw_356_vertices[75];
static NJS_CNK_MODEL draw_356_model;
static NJS_CNK_OBJECT draw_356;
static Sint16 draw_355_polygons[83];
static Sint32 draw_355_vertices[83];
static NJS_CNK_MODEL draw_355_model;
static NJS_CNK_OBJECT draw_355;
static Sint16 draw_354_polygons[83];
static Sint32 draw_354_vertices[75];
static NJS_CNK_MODEL draw_354_model;
static NJS_CNK_OBJECT draw_354;
static Sint16 draw_353_polygons[179];
static Sint32 draw_353_vertices[151];
static NJS_CNK_MODEL draw_353_model;
static NJS_CNK_OBJECT draw_353;
static Sint16 draw_352_polygons[347];
static Sint32 draw_352_vertices[227];
static NJS_CNK_MODEL draw_352_model;
static NJS_CNK_OBJECT draw_352;
static Sint16 draw_351_polygons[229];
static Sint32 draw_351_vertices[175];
static NJS_CNK_MODEL draw_351_model;
static NJS_CNK_OBJECT draw_351;
static Sint16 draw_350_polygons[169];
static Sint32 draw_350_vertices[115];
static NJS_CNK_MODEL draw_350_model;
static NJS_CNK_OBJECT draw_350;
static Sint16 draw_305_polygons[199];
static Sint32 draw_305_vertices[163];
static NJS_CNK_MODEL draw_305_model;
static NJS_CNK_OBJECT draw_306;
static Sint16 draw_304_polygons[25];
static Sint32 draw_304_vertices[19];
static NJS_CNK_MODEL draw_304_model;
static NJS_CNK_OBJECT draw_304;
static Sint16 draw_303_polygons[65];
static Sint32 draw_303_vertices[67];
static NJS_CNK_MODEL draw_303_model;
static NJS_CNK_OBJECT draw_303;
static Sint16 draw_302_polygons[75];
static Sint32 draw_302_vertices[43];
static NJS_CNK_MODEL draw_302_model;
static NJS_CNK_OBJECT draw_302;
static Sint16 draw_301_polygons[399];
static Sint32 draw_301_vertices[359];
static NJS_CNK_MODEL draw_301_model;
static NJS_CNK_OBJECT draw_301;
static Sint16 draw_285_polygons[1053];
static Sint32 draw_285_vertices[743];
static NJS_CNK_MODEL draw_285_model;
static NJS_CNK_OBJECT draw_286;
static Sint16 draw_283_polygons[193];
static Sint32 draw_283_vertices[103];
static NJS_CNK_MODEL draw_283_model;
static NJS_CNK_OBJECT draw_284;
static Sint16 draw_281_polygons[771];
static Sint32 draw_281_vertices[455];
static NJS_CNK_MODEL draw_281_model;
static NJS_CNK_OBJECT draw_282;
static Sint16 draw_280_polygons[341];
static Sint32 draw_280_vertices[319];
static NJS_CNK_MODEL draw_280_model;
static NJS_CNK_OBJECT draw_280;
static Sint16 draw_279_polygons[89];
static Sint32 draw_279_vertices[83];
static NJS_CNK_MODEL draw_279_model;
static NJS_CNK_OBJECT draw_279;
static Sint16 draw_278_polygons[89];
static Sint32 draw_278_vertices[83];
static NJS_CNK_MODEL draw_278_model;
static NJS_CNK_OBJECT draw_278;
static Sint16 draw_277_polygons[95];
static Sint32 draw_277_vertices[91];
static NJS_CNK_MODEL draw_277_model;
static NJS_CNK_OBJECT draw_277;
static Sint16 draw_276_polygons[91];
static Sint32 draw_276_vertices[99];
static NJS_CNK_MODEL draw_276_model;
static NJS_CNK_OBJECT draw_276;
static Sint16 draw_275_polygons[107];
static Sint32 draw_275_vertices[99];
static NJS_CNK_MODEL draw_275_model;
static NJS_CNK_OBJECT draw_275;
static Sint16 draw_274_polygons[93];
static Sint32 draw_274_vertices[83];
static NJS_CNK_MODEL draw_274_model;
static NJS_CNK_OBJECT draw_274;
static Sint16 draw_273_polygons[47];
static Sint32 draw_273_vertices[47];
static NJS_CNK_MODEL draw_273_model;
static NJS_CNK_OBJECT draw_273;
static Sint16 draw_272_polygons[281];
static Sint32 draw_272_vertices[303];
static NJS_CNK_MODEL draw_272_model;
static NJS_CNK_OBJECT draw_272;
static Sint16 draw_271_polygons[149];
static Sint32 draw_271_vertices[135];
static NJS_CNK_MODEL draw_271_model;
static NJS_CNK_OBJECT draw_271;
static Sint16 draw_270_polygons[157];
static Sint32 draw_270_vertices[139];
static NJS_CNK_MODEL draw_270_model;
static NJS_CNK_OBJECT draw_270;
static Sint16 draw_269_polygons[79];
static Sint32 draw_269_vertices[83];
static NJS_CNK_MODEL draw_269_model;
static NJS_CNK_OBJECT draw_269;
static Sint16 draw_268_polygons[231];
static Sint32 draw_268_vertices[215];
static NJS_CNK_MODEL draw_268_model;
static NJS_CNK_OBJECT draw_268;
static Sint16 draw_267_polygons[103];
static Sint32 draw_267_vertices[115];
static NJS_CNK_MODEL draw_267_model;
static NJS_CNK_OBJECT draw_267;
static Sint16 draw_263_polygons[141];
static Sint32 draw_263_vertices[103];
static NJS_CNK_MODEL draw_263_model;
static NJS_CNK_OBJECT draw_266;
static Sint16 draw_260_polygons[135];
static Sint32 draw_260_vertices[107];
static NJS_CNK_MODEL draw_260_model;
static NJS_CNK_OBJECT draw_261;
static Sint16 draw_254_polygons[141];
static Sint32 draw_254_vertices[103];
static NJS_CNK_MODEL draw_254_model;
static NJS_CNK_OBJECT draw_254;
static Sint16 draw_253_polygons[121];
static Sint32 draw_253_vertices[91];
static NJS_CNK_MODEL draw_253_model;
static NJS_CNK_OBJECT draw_253;
static Sint16 draw_251_polygons[159];
static Sint32 draw_251_vertices[127];
static NJS_CNK_MODEL draw_251_model;
static NJS_CNK_OBJECT draw_251;
static Sint16 draw_249_polygons[95];
static Sint32 draw_249_vertices[79];
static NJS_CNK_MODEL draw_249_model;
static NJS_CNK_OBJECT draw_250;
static Sint16 draw_248_polygons[71];
static Sint32 draw_248_vertices[55];
static NJS_CNK_MODEL draw_248_model;
static NJS_CNK_OBJECT draw_248;
static Sint16 draw_247_polygons[927];
static Sint32 draw_247_vertices[795];
static NJS_CNK_MODEL draw_247_model;
static NJS_CNK_OBJECT draw_247;
static Sint16 draw_246_polygons[141];
static Sint32 draw_246_vertices[111];
static NJS_CNK_MODEL draw_246_model;
static NJS_CNK_OBJECT draw_246;
static Sint16 draw_245_polygons[149];
static Sint32 draw_245_vertices[135];
static NJS_CNK_MODEL draw_245_model;
static NJS_CNK_OBJECT draw_245;
static Sint16 draw_244_polygons[215];
static Sint32 draw_244_vertices[199];
static NJS_CNK_MODEL draw_244_model;
static NJS_CNK_OBJECT draw_244;
static Sint16 draw_241_polygons[55];
static Sint32 draw_241_vertices[51];
static NJS_CNK_MODEL draw_241_model;
static NJS_CNK_OBJECT draw_243;
static Sint16 draw_239_polygons[81];
static Sint32 draw_239_vertices[67];
static NJS_CNK_MODEL draw_239_model;
static NJS_CNK_OBJECT draw_240;
static Sint16 draw_238_polygons[107];
static Sint32 draw_238_vertices[83];
static NJS_CNK_MODEL draw_238_model;
static NJS_CNK_OBJECT draw_238;
static Sint16 draw_237_polygons[95];
static Sint32 draw_237_vertices[71];
static NJS_CNK_MODEL draw_237_model;
static NJS_CNK_OBJECT draw_237;
static Sint16 draw_236_polygons[291];
static Sint32 draw_236_vertices[247];
static NJS_CNK_MODEL draw_236_model;
static NJS_CNK_OBJECT draw_236;
static Sint16 draw_221_polygons[163];
static Sint32 draw_221_vertices[119];
static NJS_CNK_MODEL draw_221_model;
static NJS_CNK_OBJECT draw_235;
static Sint16 draw_220_polygons[31];
static Sint32 draw_220_vertices[27];
static NJS_CNK_MODEL draw_220_model;
static NJS_CNK_OBJECT draw_220;
static Sint16 draw_218_polygons[107];
static Sint32 draw_218_vertices[95];
static NJS_CNK_MODEL draw_218_model;
static NJS_CNK_OBJECT draw_219;
static Sint16 draw_213_polygons[25];
static Sint32 draw_213_vertices[19];
static NJS_CNK_MODEL draw_213_model;
static NJS_CNK_OBJECT draw_217;
static Sint16 draw_212_polygons[175];
static Sint32 draw_212_vertices[151];
static NJS_CNK_MODEL draw_212_model;
static NJS_CNK_OBJECT draw_212;
static Sint16 draw_211_polygons[39];
static Sint32 draw_211_vertices[35];
static NJS_CNK_MODEL draw_211_model;
static NJS_CNK_OBJECT draw_211;
static Sint16 draw_210_polygons[547];
static Sint32 draw_210_vertices[347];
static NJS_CNK_MODEL draw_210_model;
static NJS_CNK_OBJECT draw_210;
static Sint16 draw_209_polygons[399];
static Sint32 draw_209_vertices[443];
static NJS_CNK_MODEL draw_209_model;
static NJS_CNK_OBJECT draw_209;
static Sint16 draw_208_polygons[231];
static Sint32 draw_208_vertices[267];
static NJS_CNK_MODEL draw_208_model;
static NJS_CNK_OBJECT draw_208;
static Sint16 draw_207_polygons[169];
static Sint32 draw_207_vertices[171];
static NJS_CNK_MODEL draw_207_model;
static NJS_CNK_OBJECT draw_207;
static Sint16 draw_200_polygons[169];
static Sint32 draw_200_vertices[187];
static NJS_CNK_MODEL draw_200_model;
static NJS_CNK_OBJECT draw_201;
static Sint16 draw_199_polygons[91];
static Sint32 draw_199_vertices[99];
static NJS_CNK_MODEL draw_199_model;
static NJS_CNK_OBJECT draw_199;
static Sint16 draw_198_polygons[91];
static Sint32 draw_198_vertices[99];
static NJS_CNK_MODEL draw_198_model;
static NJS_CNK_OBJECT draw_198;
static Sint16 draw_193_polygons[161];
static Sint32 draw_193_vertices[171];
static NJS_CNK_MODEL draw_193_model;
static NJS_CNK_OBJECT draw_194;
static Sint16 draw_181_polygons[167];
static Sint32 draw_181_vertices[115];
static NJS_CNK_MODEL draw_181_model;
static NJS_CNK_OBJECT draw_181;
static Sint16 draw_180_polygons[69];
static Sint32 draw_180_vertices[67];
static NJS_CNK_MODEL draw_180_model;
static NJS_CNK_OBJECT draw_180;
static Sint16 draw_179_polygons[157];
static Sint32 draw_179_vertices[155];
static NJS_CNK_MODEL draw_179_model;
static NJS_CNK_OBJECT draw_179;
static Sint16 draw_178_polygons[157];
static Sint32 draw_178_vertices[155];
static NJS_CNK_MODEL draw_178_model;
static NJS_CNK_OBJECT draw_178;
static Sint16 draw_174_polygons[51];
static Sint32 draw_174_vertices[47];
static NJS_CNK_MODEL draw_174_model;
static NJS_CNK_OBJECT draw_177;
static Sint16 draw_173_polygons[51];
static Sint32 draw_173_vertices[43];
static NJS_CNK_MODEL draw_173_model;
static NJS_CNK_OBJECT draw_173;
static Sint16 draw_37_polygons[87];
static Sint32 draw_37_vertices[75];
static NJS_CNK_MODEL draw_37_model;
static NJS_CNK_OBJECT draw_169;
static Sint16 draw_157_polygons[63];
static Sint32 draw_157_vertices[51];
static NJS_CNK_MODEL draw_157_model;
static NJS_CNK_OBJECT draw_167;
static Sint16 draw_156_polygons[551];
static Sint32 draw_156_vertices[503];
static NJS_CNK_MODEL draw_156_model;
static NJS_CNK_OBJECT draw_156;
static Sint16 draw_155_polygons[745];
static Sint32 draw_155_vertices[687];
static NJS_CNK_MODEL draw_155_model;
static NJS_CNK_OBJECT draw_155;
static Sint16 draw_154_polygons[337];
static Sint32 draw_154_vertices[403];
static NJS_CNK_MODEL draw_154_model;
static NJS_CNK_OBJECT draw_154;
static Sint16 draw_153_polygons[113];
static Sint32 draw_153_vertices[99];
static NJS_CNK_MODEL draw_153_model;
static NJS_CNK_OBJECT draw_153;
static Sint16 draw_152_polygons[1629];
static Sint32 draw_152_vertices[1555];
static NJS_CNK_MODEL draw_152_model;
static NJS_CNK_OBJECT draw_152;
static Sint16 draw_150_polygons[1611];
static Sint32 draw_150_vertices[1351];
static NJS_CNK_MODEL draw_150_model;
static NJS_CNK_OBJECT draw_151;
static Sint16 draw_147_polygons[883];
static Sint32 draw_147_vertices[831];
static NJS_CNK_MODEL draw_147_model;
static NJS_CNK_OBJECT draw_149;
static Sint16 draw_146_polygons[139];
static Sint32 draw_146_vertices[99];
static NJS_CNK_MODEL draw_146_model;
static NJS_CNK_OBJECT draw_146;
static Sint16 draw_145_polygons[43];
static Sint32 draw_145_vertices[35];
static NJS_CNK_MODEL draw_145_model;
static NJS_CNK_OBJECT draw_145;
static Sint16 draw_143_polygons[697];
static Sint32 draw_143_vertices[651];
static NJS_CNK_MODEL draw_143_model;
static NJS_CNK_OBJECT draw_143;
static Sint16 draw_142_polygons[299];
static Sint32 draw_142_vertices[219];
static NJS_CNK_MODEL draw_142_model;
static NJS_CNK_OBJECT draw_142;
static Sint16 draw_139_polygons[367];
static Sint32 draw_139_vertices[275];
static NJS_CNK_MODEL draw_139_model;
static NJS_CNK_OBJECT draw_140;
static Sint16 draw_137_polygons[93];
static Sint32 draw_137_vertices[59];
static NJS_CNK_MODEL draw_137_model;
static NJS_CNK_OBJECT draw_138;
static Sint16 draw_136_polygons[101];
static Sint32 draw_136_vertices[79];
static NJS_CNK_MODEL draw_136_model;
static NJS_CNK_OBJECT draw_136;
static Sint16 draw_135_polygons[63];
static Sint32 draw_135_vertices[51];
static NJS_CNK_MODEL draw_135_model;
static NJS_CNK_OBJECT draw_135;
static Sint16 draw_133_polygons[63];
static Sint32 draw_133_vertices[51];
static NJS_CNK_MODEL draw_133_model;
static NJS_CNK_OBJECT draw_134;
static Sint16 draw_132_polygons[451];
static Sint32 draw_132_vertices[351];
static NJS_CNK_MODEL draw_132_model;
static NJS_CNK_OBJECT draw_132;
static Sint16 draw_131_polygons[677];
static Sint32 draw_131_vertices[603];
static NJS_CNK_MODEL draw_131_model;
static NJS_CNK_OBJECT draw_131;
static Sint16 draw_130_polygons[603];
static Sint32 draw_130_vertices[531];
static NJS_CNK_MODEL draw_130_model;
static NJS_CNK_OBJECT draw_130;
static Sint16 draw_129_polygons[779];
static Sint32 draw_129_vertices[567];
static NJS_CNK_MODEL draw_129_model;
static NJS_CNK_OBJECT draw_129;
static Sint16 draw_128_polygons[115];
static Sint32 draw_128_vertices[107];
static NJS_CNK_MODEL draw_128_model;
static NJS_CNK_OBJECT draw_128;
static Sint16 draw_127_polygons[81];
static Sint32 draw_127_vertices[55];
static NJS_CNK_MODEL draw_127_model;
static NJS_CNK_OBJECT draw_127;
static Sint16 draw_126_polygons[39];
static Sint32 draw_126_vertices[35];
static NJS_CNK_MODEL draw_126_model;
static NJS_CNK_OBJECT draw_126;
static Sint16 draw_125_polygons[247];
static Sint32 draw_125_vertices[199];
static NJS_CNK_MODEL draw_125_model;
static NJS_CNK_OBJECT draw_125;
static Sint16 draw_123_polygons[93];
static Sint32 draw_123_vertices[91];
static NJS_CNK_MODEL draw_123_model;
static NJS_CNK_OBJECT draw_124;
static Sint16 draw_122_polygons[39];
static Sint32 draw_122_vertices[35];
static NJS_CNK_MODEL draw_122_model;
static NJS_CNK_OBJECT draw_122;
static Sint16 draw_121_polygons[505];
static Sint32 draw_121_vertices[407];
static NJS_CNK_MODEL draw_121_model;
static NJS_CNK_OBJECT draw_121;
static Sint16 draw_120_polygons[193];
static Sint32 draw_120_vertices[103];
static NJS_CNK_MODEL draw_120_model;
static NJS_CNK_OBJECT draw_120;
static Sint16 draw_119_polygons[141];
static Sint32 draw_119_vertices[111];
static NJS_CNK_MODEL draw_119_model;
static NJS_CNK_OBJECT draw_119;
static Sint16 draw_118_polygons[179];
static Sint32 draw_118_vertices[151];
static NJS_CNK_MODEL draw_118_model;
static NJS_CNK_OBJECT draw_118;
static Sint16 draw_117_polygons[281];
static Sint32 draw_117_vertices[215];
static NJS_CNK_MODEL draw_117_model;
static NJS_CNK_OBJECT draw_117;
static Sint16 draw_116_polygons[263];
static Sint32 draw_116_vertices[175];
static NJS_CNK_MODEL draw_116_model;
static NJS_CNK_OBJECT draw_116;
static Sint16 draw_115_polygons[381];
static Sint32 draw_115_vertices[267];
static NJS_CNK_MODEL draw_115_model;
static NJS_CNK_OBJECT draw_115;
static Sint16 draw_113_polygons[857];
static Sint32 draw_113_vertices[799];
static NJS_CNK_MODEL draw_113_model;
static NJS_CNK_OBJECT draw_114;
static Sint16 draw_112_polygons[113];
static Sint32 draw_112_vertices[91];
static NJS_CNK_MODEL draw_112_model;
static NJS_CNK_OBJECT draw_112;
static Sint16 draw_111_polygons[141];
static Sint32 draw_111_vertices[111];
static NJS_CNK_MODEL draw_111_model;
static NJS_CNK_OBJECT draw_111;
static Sint16 draw_110_polygons[257];
static Sint32 draw_110_vertices[175];
static NJS_CNK_MODEL draw_110_model;
static NJS_CNK_OBJECT draw_110;
static Sint16 draw_109_polygons[187];
static Sint32 draw_109_vertices[147];
static NJS_CNK_MODEL draw_109_model;
static NJS_CNK_OBJECT draw_109;
static Sint16 draw_108_polygons[141];
static Sint32 draw_108_vertices[111];
static NJS_CNK_MODEL draw_108_model;
static NJS_CNK_OBJECT draw_108;
static Sint16 draw_107_polygons[141];
static Sint32 draw_107_vertices[111];
static NJS_CNK_MODEL draw_107_model;
static NJS_CNK_OBJECT draw_107;
static Sint16 draw_106_polygons[141];
static Sint32 draw_106_vertices[111];
static NJS_CNK_MODEL draw_106_model;
static NJS_CNK_OBJECT draw_106;
static Sint16 draw_105_polygons[141];
static Sint32 draw_105_vertices[111];
static NJS_CNK_MODEL draw_105_model;
static NJS_CNK_OBJECT draw_105;
static Sint16 draw_104_polygons[141];
static Sint32 draw_104_vertices[111];
static NJS_CNK_MODEL draw_104_model;
static NJS_CNK_OBJECT draw_104;
static Sint16 draw_103_polygons[141];
static Sint32 draw_103_vertices[107];
static NJS_CNK_MODEL draw_103_model;
static NJS_CNK_OBJECT draw_103;
static Sint16 draw_102_polygons[465];
static Sint32 draw_102_vertices[395];
static NJS_CNK_MODEL draw_102_model;
static NJS_CNK_OBJECT draw_102;
static Sint16 draw_100_polygons[51];
static Sint32 draw_100_vertices[35];
static NJS_CNK_MODEL draw_100_model;
static NJS_CNK_OBJECT draw_101;
static Sint16 draw_99_polygons[51];
static Sint32 draw_99_vertices[35];
static NJS_CNK_MODEL draw_99_model;
static NJS_CNK_OBJECT draw_99;
static Sint16 draw_92_polygons[51];
static Sint32 draw_92_vertices[35];
static NJS_CNK_MODEL draw_92_model;
static NJS_CNK_OBJECT draw_98;
static Sint16 draw_78_polygons[785];
static Sint32 draw_78_vertices[471];
static NJS_CNK_MODEL draw_78_model;
static NJS_CNK_OBJECT draw_80;
static Sint16 draw_76_polygons[447];
static Sint32 draw_76_vertices[459];
static NJS_CNK_MODEL draw_76_model;
static NJS_CNK_OBJECT draw_77;
static Sint16 draw_75_polygons[237];
static Sint32 draw_75_vertices[143];
static NJS_CNK_MODEL draw_75_model;
static NJS_CNK_OBJECT draw_75;
static Sint16 draw_74_polygons[81];
static Sint32 draw_74_vertices[79];
static NJS_CNK_MODEL draw_74_model;
static NJS_CNK_OBJECT draw_74;
static Sint16 draw_73_polygons[203];
static Sint32 draw_73_vertices[211];
static NJS_CNK_MODEL draw_73_model;
static NJS_CNK_OBJECT draw_73;
static Sint16 draw_70_polygons[1261];
static Sint32 draw_70_vertices[1003];
static NJS_CNK_MODEL draw_70_model;
static NJS_CNK_OBJECT draw_72;
static Sint16 draw_69_polygons[731];
static Sint32 draw_69_vertices[551];
static NJS_CNK_MODEL draw_69_model;
static NJS_CNK_OBJECT draw_69;
static Sint16 draw_68_polygons[161];
static Sint32 draw_68_vertices[95];
static NJS_CNK_MODEL draw_68_model;
static NJS_CNK_OBJECT draw_68;
static Sint16 draw_67_polygons[359];
static Sint32 draw_67_vertices[271];
static NJS_CNK_MODEL draw_67_model;
static NJS_CNK_OBJECT draw_67;
static Sint16 draw_66_polygons[175];
static Sint32 draw_66_vertices[131];
static NJS_CNK_MODEL draw_66_model;
static NJS_CNK_OBJECT draw_66;
static Sint16 draw_65_polygons[269];
static Sint32 draw_65_vertices[223];
static NJS_CNK_MODEL draw_65_model;
static NJS_CNK_OBJECT draw_65;
static Sint16 draw_39_polygons[101];
static Sint32 draw_39_vertices[83];
static NJS_CNK_MODEL draw_39_model;
static NJS_CNK_OBJECT draw_50;
static Sint16 draw_36_polygons[89];
static Sint32 draw_36_vertices[67];
static NJS_CNK_MODEL draw_36_model;
static NJS_CNK_OBJECT draw_36;
static Sint16 draw_35_polygons[201];
static Sint32 draw_35_vertices[99];
static NJS_CNK_MODEL draw_35_model;
static NJS_CNK_OBJECT draw_35;
static Sint16 draw_34_polygons[463];
static Sint32 draw_34_vertices[211];
static NJS_CNK_MODEL draw_34_model;
static NJS_CNK_OBJECT draw_34;
static Sint16 draw_32_polygons[83];
static Sint32 draw_32_vertices[79];
static NJS_CNK_MODEL draw_32_model;
static NJS_CNK_OBJECT draw_33;
static Sint16 draw_30_polygons[141];
static Sint32 draw_30_vertices[99];
static NJS_CNK_MODEL draw_30_model;
static NJS_CNK_OBJECT draw_31;
static Sint16 draw_29_polygons[853];
static Sint32 draw_29_vertices[775];
static NJS_CNK_MODEL draw_29_model;
static NJS_CNK_OBJECT draw_29;
static Sint16 draw_28_polygons[115];
static Sint32 draw_28_vertices[115];
static NJS_CNK_MODEL draw_28_model;
static NJS_CNK_OBJECT draw_28;
static Sint16 draw_26_polygons[185];
static Sint32 draw_26_vertices[179];
static NJS_CNK_MODEL draw_26_model;
static NJS_CNK_OBJECT draw_27;
static Sint16 draw_25_polygons[113];
static Sint32 draw_25_vertices[99];
static NJS_CNK_MODEL draw_25_model;
static NJS_CNK_OBJECT draw_25;
static Sint16 draw_24_polygons[101];
static Sint32 draw_24_vertices[83];
static NJS_CNK_MODEL draw_24_model;
static NJS_CNK_OBJECT draw_24;
static Sint16 draw_23_polygons[83];
static Sint32 draw_23_vertices[83];
static NJS_CNK_MODEL draw_23_model;
static NJS_CNK_OBJECT draw_23;
static Sint16 draw_22_polygons[141];
static Sint32 draw_22_vertices[111];
static NJS_CNK_MODEL draw_22_model;
static NJS_CNK_OBJECT draw_22;
static Sint16 draw_21_polygons[141];
static Sint32 draw_21_vertices[111];
static NJS_CNK_MODEL draw_21_model;
static NJS_CNK_OBJECT draw_21;
static Sint16 draw_20_polygons[141];
static Sint32 draw_20_vertices[111];
static NJS_CNK_MODEL draw_20_model;
static NJS_CNK_OBJECT draw_20;
static Sint16 draw_18_polygons[141];
static Sint32 draw_18_vertices[115];
static NJS_CNK_MODEL draw_18_model;
static NJS_CNK_OBJECT draw_19;
static Sint16 draw_17_polygons[155];
static Sint32 draw_17_vertices[79];
static NJS_CNK_MODEL draw_17_model;
static NJS_CNK_OBJECT draw_17;
static Sint16 draw_8_polygons[449];
static Sint32 draw_8_vertices[443];
static NJS_CNK_MODEL draw_8_model;
static NJS_CNK_OBJECT draw_16;
static Sint16 draw_0_polygons[139];
static Sint32 draw_0_vertices[87];
static NJS_CNK_MODEL draw_0_model;
static NJS_CNK_OBJECT draw_7;
static NJS_MATERIAL collision_0_materials[1];
static Sint16 collision_0_faces[59];
static NJS_MESHSET collision_0_meshsets[1];
static NJS_POINT3 collision_0_points[28];
static NJS_MODEL collision_0_model;
static NJS_OBJECT collision_0;
static NJS_MATERIAL collision_2_materials[1];
static Sint16 collision_2_faces[15];
static NJS_MESHSET collision_2_meshsets[1];
static NJS_POINT3 collision_2_points[12];
static NJS_MODEL collision_2_model;
static NJS_OBJECT collision_2;
static NJS_MATERIAL collision_1_materials[1];
static Sint16 collision_1_faces[27];
static NJS_MESHSET collision_1_meshsets[1];
static NJS_POINT3 collision_1_points[12];
static NJS_MODEL collision_1_model;
static NJS_OBJECT collision_4;
static NJS_MATERIAL collision_5_materials[1];
static Sint16 collision_5_faces[5];
static NJS_MESHSET collision_5_meshsets[1];
static NJS_POINT3 collision_5_points[4];
static NJS_MODEL collision_5_model;
static NJS_OBJECT collision_5;
static NJS_MATERIAL collision_6_materials[1];
static Sint16 collision_6_faces[11];
static NJS_MESHSET collision_6_meshsets[1];
static NJS_POINT3 collision_6_points[8];
static NJS_MODEL collision_6_model;
static NJS_OBJECT collision_6;
static NJS_MATERIAL collision_7_materials[1];
static Sint16 collision_7_faces[42];
static NJS_MESHSET collision_7_meshsets[1];
static NJS_POINT3 collision_7_points[19];
static NJS_MODEL collision_7_model;
static NJS_OBJECT collision_7;
static NJS_MATERIAL collision_8_materials[1];
static Sint16 collision_8_faces[71];
static NJS_MESHSET collision_8_meshsets[1];
static NJS_POINT3 collision_8_points[30];
static NJS_MODEL collision_8_model;
static NJS_OBJECT collision_8;
static NJS_MATERIAL collision_9_materials[1];
static Sint16 collision_9_faces[97];
static NJS_MESHSET collision_9_meshsets[1];
static NJS_POINT3 collision_9_points[44];
static NJS_MODEL collision_9_model;
static NJS_OBJECT collision_9;
static NJS_MATERIAL collision_10_materials[1];
static Sint16 collision_10_faces[30];
static NJS_MESHSET collision_10_meshsets[1];
static NJS_POINT3 collision_10_points[16];
static NJS_MODEL collision_10_model;
static NJS_OBJECT collision_10;
static NJS_MATERIAL collision_11_materials[1];
static Sint16 collision_11_faces[35];
static NJS_MESHSET collision_11_meshsets[1];
static NJS_POINT3 collision_11_points[19];
static NJS_MODEL collision_11_model;
static NJS_OBJECT collision_11;
static NJS_MATERIAL collision_12_materials[1];
static Sint16 collision_12_faces[18];
static NJS_MESHSET collision_12_meshsets[1];
static NJS_POINT3 collision_12_points[16];
static NJS_MODEL collision_12_model;
static NJS_OBJECT collision_12;
static NJS_MATERIAL collision_13_materials[1];
static Sint16 collision_13_faces[50];
static NJS_MESHSET collision_13_meshsets[1];
static NJS_POINT3 collision_13_points[27];
static NJS_MODEL collision_13_model;
static NJS_OBJECT collision_13;
static NJS_MATERIAL collision_14_materials[1];
static Sint16 collision_14_faces[7];
static NJS_MESHSET collision_14_meshsets[1];
static NJS_POINT3 collision_14_points[6];
static NJS_MODEL collision_14_model;
static NJS_OBJECT collision_14;
static NJS_MATERIAL collision_15_materials[1];
static Sint16 collision_15_faces[18];
static NJS_MESHSET collision_15_meshsets[1];
static NJS_POINT3 collision_15_points[12];
static NJS_MODEL collision_15_model;
static NJS_OBJECT collision_15;
static NJS_MATERIAL collision_16_materials[1];
static Sint16 collision_16_faces[18];
static NJS_MESHSET collision_16_meshsets[1];
static NJS_POINT3 collision_16_points[9];
static NJS_MODEL collision_16_model;
static NJS_OBJECT collision_16;
static NJS_MATERIAL collision_17_materials[1];
static Sint16 collision_17_faces[19];
static NJS_MESHSET collision_17_meshsets[1];
static NJS_POINT3 collision_17_points[8];
static NJS_MODEL collision_17_model;
static NJS_OBJECT collision_17;
static NJS_MATERIAL collision_18_materials[1];
static Sint16 collision_18_faces[15];
static NJS_MESHSET collision_18_meshsets[1];
static NJS_POINT3 collision_18_points[9];
static NJS_MODEL collision_18_model;
static NJS_OBJECT collision_18;
static NJS_MATERIAL collision_19_materials[1];
static Sint16 collision_19_faces[19];
static NJS_MESHSET collision_19_meshsets[1];
static NJS_POINT3 collision_19_points[8];
static NJS_MODEL collision_19_model;
static NJS_OBJECT collision_19;
static NJS_MATERIAL collision_20_materials[1];
static Sint16 collision_20_faces[19];
static NJS_MESHSET collision_20_meshsets[1];
static NJS_POINT3 collision_20_points[8];
static NJS_MODEL collision_20_model;
static NJS_OBJECT collision_20;
static NJS_MATERIAL collision_21_materials[1];
static Sint16 collision_21_faces[24];
static NJS_MESHSET collision_21_meshsets[1];
static NJS_POINT3 collision_21_points[14];
static NJS_MODEL collision_21_model;
static NJS_OBJECT collision_21;
static NJS_MATERIAL collision_22_materials[1];
static Sint16 collision_22_faces[19];
static NJS_MESHSET collision_22_meshsets[1];
static NJS_POINT3 collision_22_points[11];
static NJS_MODEL collision_22_model;
static NJS_OBJECT collision_22;
static NJS_MATERIAL collision_23_materials[1];
static Sint16 collision_23_faces[21];
static NJS_MESHSET collision_23_meshsets[1];
static NJS_POINT3 collision_23_points[12];
static NJS_MODEL collision_23_model;
static NJS_OBJECT collision_23;
static NJS_MATERIAL collision_24_materials[1];
static Sint16 collision_24_faces[50];
static NJS_MESHSET collision_24_meshsets[1];
static NJS_POINT3 collision_24_points[23];
static NJS_MODEL collision_24_model;
static NJS_OBJECT collision_24;
static NJS_MATERIAL collision_25_materials[1];
static Sint16 collision_25_faces[14];
static NJS_MESHSET collision_25_meshsets[1];
static NJS_POINT3 collision_25_points[8];
static NJS_MODEL collision_25_model;
static NJS_OBJECT collision_25;
static NJS_MATERIAL collision_26_materials[1];
static Sint16 collision_26_faces[16];
static NJS_MESHSET collision_26_meshsets[1];
static NJS_POINT3 collision_26_points[8];
static NJS_MODEL collision_26_model;
static NJS_OBJECT collision_26;
static NJS_MATERIAL collision_27_materials[1];
static Sint16 collision_27_faces[30];
static NJS_MESHSET collision_27_meshsets[1];
static NJS_POINT3 collision_27_points[12];
static NJS_MODEL collision_27_model;
static NJS_OBJECT collision_27;
static NJS_MATERIAL collision_28_materials[1];
static Sint16 collision_28_faces[25];
static NJS_MESHSET collision_28_meshsets[1];
static NJS_POINT3 collision_28_points[12];
static NJS_MODEL collision_28_model;
static NJS_OBJECT collision_28;
static NJS_MATERIAL collision_29_materials[1];
static Sint16 collision_29_faces[17];
static NJS_MESHSET collision_29_meshsets[1];
static NJS_POINT3 collision_29_points[8];
static NJS_MODEL collision_29_model;
static NJS_OBJECT collision_29;
static NJS_MATERIAL collision_34_materials[1];
static Sint16 collision_34_faces[30];
static NJS_MESHSET collision_34_meshsets[1];
static NJS_POINT3 collision_34_points[16];
static NJS_MODEL collision_34_model;
static NJS_OBJECT collision_34;
static NJS_MATERIAL collision_44_materials[1];
static Sint16 collision_44_faces[201];
static NJS_MESHSET collision_44_meshsets[1];
static NJS_POINT3 collision_44_points[77];
static NJS_MODEL collision_44_model;
static NJS_OBJECT collision_44;
static NJS_MATERIAL collision_49_materials[1];
static Sint16 collision_49_faces[100];
static NJS_MESHSET collision_49_meshsets[1];
static NJS_POINT3 collision_49_points[40];
static NJS_MODEL collision_49_model;
static NJS_OBJECT collision_49;
static NJS_MATERIAL collision_54_materials[1];
static Sint16 collision_54_faces[21];
static NJS_MESHSET collision_54_meshsets[1];
static NJS_POINT3 collision_54_points[14];
static NJS_MODEL collision_54_model;
static NJS_OBJECT collision_54;
static NJS_MATERIAL collision_55_materials[1];
static Sint16 collision_55_faces[63];
static NJS_MESHSET collision_55_meshsets[1];
static NJS_POINT3 collision_55_points[41];
static NJS_MODEL collision_55_model;
static NJS_OBJECT collision_55;
static NJS_MATERIAL collision_56_materials[1];
static Sint16 collision_56_faces[17];
static NJS_MESHSET collision_56_meshsets[1];
static NJS_POINT3 collision_56_points[8];
static NJS_MODEL collision_56_model;
static NJS_OBJECT collision_56;
static NJS_MATERIAL collision_57_materials[1];
static Sint16 collision_57_faces[11];
static NJS_MESHSET collision_57_meshsets[1];
static NJS_POINT3 collision_57_points[8];
static NJS_MODEL collision_57_model;
static NJS_OBJECT collision_57;
static NJS_MATERIAL collision_63_materials[1];
static Sint16 collision_63_faces[18];
static NJS_MESHSET collision_63_meshsets[1];
static NJS_POINT3 collision_63_points[12];
static NJS_MODEL collision_63_model;
static NJS_OBJECT collision_63;
static NJS_MATERIAL collision_64_materials[1];
static Sint16 collision_64_faces[11];
static NJS_MESHSET collision_64_meshsets[1];
static NJS_POINT3 collision_64_points[8];
static NJS_MODEL collision_64_model;
static NJS_OBJECT collision_64;
static NJS_MATERIAL collision_67_materials[1];
static Sint16 collision_67_faces[11];
static NJS_MESHSET collision_67_meshsets[1];
static NJS_POINT3 collision_67_points[8];
static NJS_MODEL collision_67_model;
static NJS_OBJECT collision_67;
static NJS_MATERIAL collision_69_materials[1];
static Sint16 collision_69_faces[133];
static NJS_MESHSET collision_69_meshsets[1];
static NJS_POINT3 collision_69_points[62];
static NJS_MODEL collision_69_model;
static NJS_OBJECT collision_69;
static NJS_MATERIAL collision_70_materials[1];
static Sint16 collision_70_faces[16];
static NJS_MESHSET collision_70_meshsets[1];
static NJS_POINT3 collision_70_points[8];
static NJS_MODEL collision_70_model;
static NJS_OBJECT collision_70;
static NJS_MATERIAL collision_71_materials[1];
static Sint16 collision_71_faces[16];
static NJS_MESHSET collision_71_meshsets[1];
static NJS_POINT3 collision_71_points[8];
static NJS_MODEL collision_71_model;
static NJS_OBJECT collision_71;
static NJS_MATERIAL collision_72_materials[1];
static Sint16 collision_72_faces[16];
static NJS_MESHSET collision_72_meshsets[1];
static NJS_POINT3 collision_72_points[8];
static NJS_MODEL collision_72_model;
static NJS_OBJECT collision_72;
static NJS_MATERIAL collision_73_materials[1];
static Sint16 collision_73_faces[16];
static NJS_MESHSET collision_73_meshsets[1];
static NJS_POINT3 collision_73_points[8];
static NJS_MODEL collision_73_model;
static NJS_OBJECT collision_73;
static NJS_MATERIAL collision_74_materials[1];
static Sint16 collision_74_faces[12];
static NJS_MESHSET collision_74_meshsets[1];
static NJS_POINT3 collision_74_points[8];
static NJS_MODEL collision_74_model;
static NJS_OBJECT collision_74;
static NJS_MATERIAL collision_75_materials[1];
static Sint16 collision_75_faces[15];
static NJS_MESHSET collision_75_meshsets[1];
static NJS_POINT3 collision_75_points[8];
static NJS_MODEL collision_75_model;
static NJS_OBJECT collision_75;
static NJS_MATERIAL collision_76_materials[1];
static Sint16 collision_76_faces[32];
static NJS_MESHSET collision_76_meshsets[1];
static NJS_POINT3 collision_76_points[12];
static NJS_MODEL collision_76_model;
static NJS_OBJECT collision_76;
static NJS_MATERIAL collision_77_materials[1];
static Sint16 collision_77_faces[31];
static NJS_MESHSET collision_77_meshsets[1];
static NJS_POINT3 collision_77_points[12];
static NJS_MODEL collision_77_model;
static NJS_OBJECT collision_77;
static NJS_MATERIAL collision_88_materials[1];
static Sint16 collision_88_faces[14];
static NJS_MESHSET collision_88_meshsets[1];
static NJS_POINT3 collision_88_points[8];
static NJS_MODEL collision_88_model;
static NJS_OBJECT collision_88;
static NJS_MATERIAL collision_96_materials[1];
static Sint16 collision_96_faces[109];
static NJS_MESHSET collision_96_meshsets[1];
static NJS_POINT3 collision_96_points[52];
static NJS_MODEL collision_96_model;
static NJS_OBJECT collision_96;
static NJS_MATERIAL collision_97_materials[1];
static Sint16 collision_97_faces[26];
static NJS_MESHSET collision_97_meshsets[1];
static NJS_POINT3 collision_97_points[12];
static NJS_MODEL collision_97_model;
static NJS_OBJECT collision_97;
static NJS_MATERIAL collision_98_materials[1];
static Sint16 collision_98_faces[41];
static NJS_MESHSET collision_98_meshsets[1];
static NJS_POINT3 collision_98_points[16];
static NJS_MODEL collision_98_model;
static NJS_OBJECT collision_98;
static NJS_MATERIAL collision_100_materials[1];
static Sint16 collision_100_faces[31];
static NJS_MESHSET collision_100_meshsets[1];
static NJS_POINT3 collision_100_points[12];
static NJS_MODEL collision_100_model;
static NJS_OBJECT collision_100;
static NJS_MATERIAL collision_102_materials[1];
static Sint16 collision_102_faces[44];
static NJS_MESHSET collision_102_meshsets[1];
static NJS_POINT3 collision_102_points[32];
static NJS_MODEL collision_102_model;
static NJS_OBJECT collision_102;
static NJS_MATERIAL collision_103_materials[1];
static Sint16 collision_103_faces[87];
static NJS_MESHSET collision_103_meshsets[1];
static NJS_POINT3 collision_103_points[34];
static NJS_MODEL collision_103_model;
static NJS_OBJECT collision_103;
static NJS_MATERIAL collision_106_materials[1];
static Sint16 collision_106_faces[107];
static NJS_MESHSET collision_106_meshsets[1];
static NJS_POINT3 collision_106_points[40];
static NJS_MODEL collision_106_model;
static NJS_OBJECT collision_106;
static NJS_MATERIAL collision_107_materials[1];
static Sint16 collision_107_faces[31];
static NJS_MESHSET collision_107_meshsets[1];
static NJS_POINT3 collision_107_points[12];
static NJS_MODEL collision_107_model;
static NJS_OBJECT collision_107;
static NJS_MATERIAL collision_108_materials[1];
static Sint16 collision_108_faces[39];
static NJS_MESHSET collision_108_meshsets[1];
static NJS_POINT3 collision_108_points[16];
static NJS_MODEL collision_108_model;
static NJS_OBJECT collision_108;
static NJS_MATERIAL collision_109_materials[1];
static Sint16 collision_109_faces[16];
static NJS_MESHSET collision_109_meshsets[1];
static NJS_POINT3 collision_109_points[8];
static NJS_MODEL collision_109_model;
static NJS_OBJECT collision_109;
static NJS_MATERIAL collision_110_materials[1];
static Sint16 collision_110_faces[27];
static NJS_MESHSET collision_110_meshsets[1];
static NJS_POINT3 collision_110_points[12];
static NJS_MODEL collision_110_model;
static NJS_OBJECT collision_110;
static NJS_MATERIAL collision_111_materials[1];
static Sint16 collision_111_faces[16];
static NJS_MESHSET collision_111_meshsets[1];
static NJS_POINT3 collision_111_points[8];
static NJS_MODEL collision_111_model;
static NJS_OBJECT collision_111;
static NJS_MATERIAL collision_112_materials[1];
static Sint16 collision_112_faces[16];
static NJS_MESHSET collision_112_meshsets[1];
static NJS_POINT3 collision_112_points[8];
static NJS_MODEL collision_112_model;
static NJS_OBJECT collision_112;
static NJS_MATERIAL collision_113_materials[1];
static Sint16 collision_113_faces[16];
static NJS_MESHSET collision_113_meshsets[1];
static NJS_POINT3 collision_113_points[8];
static NJS_MODEL collision_113_model;
static NJS_OBJECT collision_113;
static NJS_MATERIAL collision_116_materials[1];
static Sint16 collision_116_faces[16];
static NJS_MESHSET collision_116_meshsets[1];
static NJS_POINT3 collision_116_points[8];
static NJS_MODEL collision_116_model;
static NJS_OBJECT collision_116;
static NJS_MATERIAL collision_117_materials[1];
static Sint16 collision_117_faces[31];
static NJS_MESHSET collision_117_meshsets[1];
static NJS_POINT3 collision_117_points[15];
static NJS_MODEL collision_117_model;
static NJS_OBJECT collision_117;
static NJS_MATERIAL collision_118_materials[1];
static Sint16 collision_118_faces[67];
static NJS_MESHSET collision_118_meshsets[1];
static NJS_POINT3 collision_118_points[27];
static NJS_MODEL collision_118_model;
static NJS_OBJECT collision_118;
static NJS_MATERIAL collision_119_materials[1];
static Sint16 collision_119_faces[91];
static NJS_MESHSET collision_119_meshsets[1];
static NJS_POINT3 collision_119_points[31];
static NJS_MODEL collision_119_model;
static NJS_OBJECT collision_119;
static NJS_MATERIAL collision_120_materials[1];
static Sint16 collision_120_faces[116];
static NJS_MESHSET collision_120_meshsets[1];
static NJS_POINT3 collision_120_points[51];
static NJS_MODEL collision_120_model;
static NJS_OBJECT collision_120;
static NJS_MATERIAL collision_121_materials[1];
static Sint16 collision_121_faces[159];
static NJS_MESHSET collision_121_meshsets[1];
static NJS_POINT3 collision_121_points[74];
static NJS_MODEL collision_121_model;
static NJS_OBJECT collision_121;
static NJS_MATERIAL collision_123_materials[1];
static Sint16 collision_123_faces[178];
static NJS_MESHSET collision_123_meshsets[1];
static NJS_POINT3 collision_123_points[82];
static NJS_MODEL collision_123_model;
static NJS_OBJECT collision_123;
static NJS_MATERIAL collision_128_materials[1];
static Sint16 collision_128_faces[21];
static NJS_MESHSET collision_128_meshsets[1];
static NJS_POINT3 collision_128_points[12];
static NJS_MODEL collision_128_model;
static NJS_OBJECT collision_128;
static NJS_MATERIAL collision_129_materials[1];
static Sint16 collision_129_faces[16];
static NJS_MESHSET collision_129_meshsets[1];
static NJS_POINT3 collision_129_points[8];
static NJS_MODEL collision_129_model;
static NJS_OBJECT collision_129;
static NJS_MATERIAL collision_130_materials[1];
static Sint16 collision_130_faces[25];
static NJS_MESHSET collision_130_meshsets[1];
static NJS_POINT3 collision_130_points[12];
static NJS_MODEL collision_130_model;
static NJS_OBJECT collision_130;
static NJS_MATERIAL collision_131_materials[1];
static Sint16 collision_131_faces[35];
static NJS_MESHSET collision_131_meshsets[1];
static NJS_POINT3 collision_131_points[18];
static NJS_MODEL collision_131_model;
static NJS_OBJECT collision_131;
static NJS_MATERIAL collision_132_materials[1];
static Sint16 collision_132_faces[23];
static NJS_MESHSET collision_132_meshsets[1];
static NJS_POINT3 collision_132_points[13];
static NJS_MODEL collision_132_model;
static NJS_OBJECT collision_132;
static NJS_MATERIAL collision_133_materials[1];
static Sint16 collision_133_faces[23];
static NJS_MESHSET collision_133_meshsets[1];
static NJS_POINT3 collision_133_points[12];
static NJS_MODEL collision_133_model;
static NJS_OBJECT collision_133;
static NJS_MATERIAL collision_134_materials[1];
static Sint16 collision_134_faces[41];
static NJS_MESHSET collision_134_meshsets[1];
static NJS_POINT3 collision_134_points[21];
static NJS_MODEL collision_134_model;
static NJS_OBJECT collision_134;
static NJS_MATERIAL collision_135_materials[1];
static Sint16 collision_135_faces[48];
static NJS_MESHSET collision_135_meshsets[1];
static NJS_POINT3 collision_135_points[16];
static NJS_MODEL collision_135_model;
static NJS_OBJECT collision_135;
static NJS_MATERIAL collision_140_materials[1];
static Sint16 collision_140_faces[79];
static NJS_MESHSET collision_140_meshsets[1];
static NJS_POINT3 collision_140_points[45];
static NJS_MODEL collision_140_model;
static NJS_OBJECT collision_140;
static NJS_MATERIAL collision_142_materials[1];
static Sint16 collision_142_faces[63];
static NJS_MESHSET collision_142_meshsets[1];
static NJS_POINT3 collision_142_points[24];
static NJS_MODEL collision_142_model;
static NJS_OBJECT collision_142;
static NJS_MATERIAL collision_143_materials[1];
static Sint16 collision_143_faces[146];
static NJS_MESHSET collision_143_meshsets[1];
static NJS_POINT3 collision_143_points[63];
static NJS_MODEL collision_143_model;
static NJS_OBJECT collision_143;
static NJS_MATERIAL collision_144_materials[1];
static Sint16 collision_144_faces[89];
static NJS_MESHSET collision_144_meshsets[1];
static NJS_POINT3 collision_144_points[50];
static NJS_MODEL collision_144_model;
static NJS_OBJECT collision_144;
static NJS_MATERIAL collision_146_materials[1];
static Sint16 collision_146_faces[184];
static NJS_MESHSET collision_146_meshsets[1];
static NJS_POINT3 collision_146_points[58];
static NJS_MODEL collision_146_model;
static NJS_OBJECT collision_146;
static NJS_MATERIAL collision_147_materials[1];
static Sint16 collision_147_faces[298];
static NJS_MESHSET collision_147_meshsets[1];
static NJS_POINT3 collision_147_points[96];
static NJS_MODEL collision_147_model;
static NJS_OBJECT collision_147;
static NJS_MATERIAL collision_148_materials[1];
static Sint16 collision_148_faces[221];
static NJS_MESHSET collision_148_meshsets[1];
static NJS_POINT3 collision_148_points[75];
static NJS_MODEL collision_148_model;
static NJS_OBJECT collision_148;
static NJS_MATERIAL collision_149_materials[1];
static Sint16 collision_149_faces[30];
static NJS_MESHSET collision_149_meshsets[1];
static NJS_POINT3 collision_149_points[11];
static NJS_MODEL collision_149_model;
static NJS_OBJECT collision_149;
static NJS_MATERIAL collision_150_materials[1];
static Sint16 collision_150_faces[10];
static NJS_MESHSET collision_150_meshsets[1];
static NJS_POINT3 collision_150_points[5];
static NJS_MODEL collision_150_model;
static NJS_OBJECT collision_150;
static NJS_MATERIAL collision_151_materials[1];
static Sint16 collision_151_faces[21];
static NJS_MESHSET collision_151_meshsets[1];
static NJS_POINT3 collision_151_points[11];
static NJS_MODEL collision_151_model;
static NJS_OBJECT collision_151;
static NJS_MATERIAL collision_152_materials[1];
static Sint16 collision_152_faces[19];
static NJS_MESHSET collision_152_meshsets[1];
static NJS_POINT3 collision_152_points[15];
static NJS_MODEL collision_152_model;
static NJS_OBJECT collision_152;
static NJS_MATERIAL collision_153_materials[1];
static Sint16 collision_153_faces[27];
static NJS_MESHSET collision_153_meshsets[1];
static NJS_POINT3 collision_153_points[16];
static NJS_MODEL collision_153_model;
static NJS_OBJECT collision_153;
static NJS_MATERIAL collision_154_materials[1];
static Sint16 collision_154_faces[76];
static NJS_MESHSET collision_154_meshsets[1];
static NJS_POINT3 collision_154_points[26];
static NJS_MODEL collision_154_model;
static NJS_OBJECT collision_154;
static NJS_MATERIAL collision_155_materials[1];
static Sint16 collision_155_faces[19];
static NJS_MESHSET collision_155_meshsets[1];
static NJS_POINT3 collision_155_points[8];
static NJS_MODEL collision_155_model;
static NJS_OBJECT collision_155;
static NJS_MATERIAL collision_156_materials[1];
static Sint16 collision_156_faces[11];
static NJS_MESHSET collision_156_meshsets[1];
static NJS_POINT3 collision_156_points[8];
static NJS_MODEL collision_156_model;
static NJS_OBJECT collision_156;
static NJS_MATERIAL collision_158_materials[1];
static Sint16 collision_158_faces[116];
static NJS_MESHSET collision_158_meshsets[1];
static NJS_POINT3 collision_158_points[45];
static NJS_MODEL collision_158_model;
static NJS_OBJECT collision_158;
static NJS_MATERIAL collision_159_materials[1];
static Sint16 collision_159_faces[57];
static NJS_MESHSET collision_159_meshsets[1];
static NJS_POINT3 collision_159_points[16];
static NJS_MODEL collision_159_model;
static NJS_OBJECT collision_159;
static NJS_MATERIAL collision_160_materials[1];
static Sint16 collision_160_faces[91];
static NJS_MESHSET collision_160_meshsets[1];
static NJS_POINT3 collision_160_points[42];
static NJS_MODEL collision_160_model;
static NJS_OBJECT collision_160;
static NJS_MATERIAL collision_161_materials[1];
static Sint16 collision_161_faces[25];
static NJS_MESHSET collision_161_meshsets[1];
static NJS_POINT3 collision_161_points[15];
static NJS_MODEL collision_161_model;
static NJS_OBJECT collision_161;
static NJS_MATERIAL collision_162_materials[1];
static Sint16 collision_162_faces[41];
static NJS_MESHSET collision_162_meshsets[1];
static NJS_POINT3 collision_162_points[18];
static NJS_MODEL collision_162_model;
static NJS_OBJECT collision_162;
static NJS_MATERIAL collision_163_materials[1];
static Sint16 collision_163_faces[27];
static NJS_MESHSET collision_163_meshsets[1];
static NJS_POINT3 collision_163_points[16];
static NJS_MODEL collision_163_model;
static NJS_OBJECT collision_163;
static NJS_CNK_OBJECT draw_468;
static NJS_CNK_OBJECT draw_467;
static NJS_CNK_OBJECT draw_466;
static NJS_CNK_OBJECT draw_462;
static NJS_CNK_OBJECT draw_460;
static NJS_CNK_OBJECT draw_458;
static NJS_CNK_OBJECT draw_456;
static NJS_CNK_OBJECT draw_453;
static NJS_CNK_OBJECT draw_447;
static NJS_CNK_OBJECT draw_445;
static NJS_CNK_OBJECT draw_444;
static NJS_CNK_OBJECT draw_442;
static NJS_CNK_OBJECT draw_441;
static NJS_CNK_OBJECT draw_434;
static NJS_CNK_OBJECT draw_425;
static NJS_CNK_OBJECT draw_424;
static NJS_CNK_OBJECT draw_422;
static NJS_CNK_OBJECT draw_421;
static NJS_CNK_OBJECT draw_417;
static NJS_CNK_OBJECT draw_416;
static NJS_CNK_OBJECT draw_415;
static NJS_CNK_OBJECT draw_414;
static NJS_CNK_OBJECT draw_413;
static NJS_CNK_OBJECT draw_412;
static NJS_CNK_OBJECT draw_411;
static NJS_CNK_OBJECT draw_409;
static NJS_CNK_OBJECT draw_408;
static NJS_CNK_OBJECT draw_407;
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
static NJS_CNK_OBJECT draw_391;
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
static NJS_CNK_OBJECT draw_372;
static NJS_CNK_OBJECT draw_371;
static NJS_CNK_OBJECT draw_370;
static NJS_CNK_OBJECT draw_369;
static NJS_CNK_OBJECT draw_368;
static NJS_CNK_OBJECT draw_367;
static NJS_CNK_OBJECT draw_366;
static NJS_CNK_OBJECT draw_365;
static NJS_CNK_OBJECT draw_364;
static NJS_CNK_OBJECT draw_359;
static NJS_CNK_OBJECT draw_349;
static NJS_CNK_OBJECT draw_348;
static NJS_CNK_OBJECT draw_347;
static NJS_CNK_OBJECT draw_346;
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
static NJS_CNK_OBJECT draw_332;
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
static NJS_CNK_OBJECT draw_305;
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
static NJS_CNK_OBJECT draw_285;
static NJS_CNK_OBJECT draw_283;
static NJS_CNK_OBJECT draw_281;
static NJS_CNK_OBJECT draw_265;
static NJS_CNK_OBJECT draw_264;
static NJS_CNK_OBJECT draw_263;
static NJS_CNK_OBJECT draw_262;
static NJS_CNK_OBJECT draw_260;
static NJS_CNK_OBJECT draw_259;
static NJS_CNK_OBJECT draw_258;
static NJS_CNK_OBJECT draw_257;
static NJS_CNK_OBJECT draw_256;
static NJS_CNK_OBJECT draw_255;
static NJS_CNK_OBJECT draw_252;
static NJS_CNK_OBJECT draw_249;
static NJS_CNK_OBJECT draw_242;
static NJS_CNK_OBJECT draw_241;
static NJS_CNK_OBJECT draw_239;
static NJS_CNK_OBJECT draw_234;
static NJS_CNK_OBJECT draw_233;
static NJS_CNK_OBJECT draw_232;
static NJS_CNK_OBJECT draw_231;
static NJS_CNK_OBJECT draw_230;
static NJS_CNK_OBJECT draw_229;
static NJS_CNK_OBJECT draw_228;
static NJS_CNK_OBJECT draw_227;
static NJS_CNK_OBJECT draw_226;
static NJS_CNK_OBJECT draw_225;
static NJS_CNK_OBJECT draw_224;
static NJS_CNK_OBJECT draw_223;
static NJS_CNK_OBJECT draw_222;
static NJS_CNK_OBJECT draw_221;
static NJS_CNK_OBJECT draw_218;
static NJS_CNK_OBJECT draw_216;
static NJS_CNK_OBJECT draw_215;
static NJS_CNK_OBJECT draw_214;
static NJS_CNK_OBJECT draw_213;
static NJS_CNK_OBJECT draw_206;
static NJS_CNK_OBJECT draw_205;
static NJS_CNK_OBJECT draw_204;
static NJS_CNK_OBJECT draw_203;
static NJS_CNK_OBJECT draw_202;
static NJS_CNK_OBJECT draw_200;
static NJS_CNK_OBJECT draw_197;
static NJS_CNK_OBJECT draw_196;
static NJS_CNK_OBJECT draw_195;
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
static NJS_CNK_OBJECT draw_176;
static NJS_CNK_OBJECT draw_175;
static NJS_CNK_OBJECT draw_174;
static NJS_CNK_OBJECT draw_172;
static NJS_CNK_OBJECT draw_171;
static NJS_CNK_OBJECT draw_170;
static NJS_CNK_OBJECT draw_168;
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
static NJS_CNK_OBJECT draw_150;
static NJS_CNK_OBJECT draw_148;
static NJS_CNK_OBJECT draw_147;
static NJS_CNK_OBJECT draw_144;
static NJS_CNK_OBJECT draw_141;
static NJS_CNK_OBJECT draw_139;
static NJS_CNK_OBJECT draw_137;
static NJS_CNK_OBJECT draw_133;
static NJS_CNK_OBJECT draw_123;
static NJS_CNK_OBJECT draw_113;
static NJS_CNK_OBJECT draw_100;
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
static NJS_CNK_OBJECT draw_79;
static NJS_CNK_OBJECT draw_78;
static NJS_CNK_OBJECT draw_76;
static NJS_CNK_OBJECT draw_71;
static NJS_CNK_OBJECT draw_70;
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
static NJS_CNK_OBJECT draw_32;
static NJS_CNK_OBJECT draw_30;
static NJS_CNK_OBJECT draw_26;
static NJS_CNK_OBJECT draw_18;
static NJS_CNK_OBJECT draw_15;
static NJS_CNK_OBJECT draw_14;
static NJS_CNK_OBJECT draw_13;
static NJS_CNK_OBJECT draw_12;
static NJS_CNK_OBJECT draw_11;
static NJS_CNK_OBJECT draw_10;
static NJS_CNK_OBJECT draw_9;
static NJS_CNK_OBJECT draw_8;
static NJS_CNK_OBJECT draw_6;
static NJS_CNK_OBJECT draw_5;
static NJS_CNK_OBJECT draw_4;
static NJS_CNK_OBJECT draw_3;
static NJS_CNK_OBJECT draw_2;
static NJS_CNK_OBJECT draw_1;
static NJS_CNK_OBJECT draw_0;
static NJS_OBJECT collision_1;
static NJS_OBJECT collision_3;
static NJS_OBJECT collision_30;
static NJS_OBJECT collision_31;
static NJS_OBJECT collision_32;
static NJS_OBJECT collision_33;
static NJS_OBJECT collision_35;
static NJS_OBJECT collision_36;
static NJS_OBJECT collision_37;
static NJS_OBJECT collision_38;
static NJS_OBJECT collision_39;
static NJS_OBJECT collision_40;
static NJS_OBJECT collision_41;
static NJS_OBJECT collision_42;
static NJS_OBJECT collision_43;
static NJS_OBJECT collision_45;
static NJS_OBJECT collision_46;
static NJS_OBJECT collision_47;
static NJS_OBJECT collision_48;
static NJS_OBJECT collision_50;
static NJS_OBJECT collision_51;
static NJS_OBJECT collision_52;
static NJS_OBJECT collision_53;
static NJS_OBJECT collision_58;
static NJS_OBJECT collision_59;
static NJS_OBJECT collision_60;
static NJS_OBJECT collision_61;
static NJS_OBJECT collision_62;
static NJS_OBJECT collision_65;
static NJS_OBJECT collision_66;
static NJS_OBJECT collision_68;
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
static NJS_OBJECT collision_89;
static NJS_OBJECT collision_90;
static NJS_OBJECT collision_91;
static NJS_OBJECT collision_92;
static NJS_OBJECT collision_93;
static NJS_OBJECT collision_94;
static NJS_OBJECT collision_95;
static NJS_OBJECT collision_99;
static NJS_OBJECT collision_101;
static NJS_OBJECT collision_104;
static NJS_OBJECT collision_105;
static NJS_OBJECT collision_114;
static NJS_OBJECT collision_115;
static NJS_OBJECT collision_122;
static NJS_OBJECT collision_124;
static NJS_OBJECT collision_125;
static NJS_OBJECT collision_126;
static NJS_OBJECT collision_127;
static NJS_OBJECT collision_136;
static NJS_OBJECT collision_137;
static NJS_OBJECT collision_138;
static NJS_OBJECT collision_139;
static NJS_OBJECT collision_141;
static NJS_OBJECT collision_145;
static NJS_OBJECT collision_157;

static struct land_entry entries[634] = {
#include "stg23/terrain/entries.inc"
};
struct land_table hidden_base_land = {
#include "stg23/terrain/hidden_base_land.inc"
};
static NJS_TEXNAME textures[30] = {
#include "stg23/terrain/textures.inc"
};
NJS_TEXLIST hidden_base_land_texlist = {
#include "stg23/terrain/hidden_base_land_texlist.inc"
};
static Sint16 draw_469_polygons[113] = {
#include "stg23/terrain/draw_469_polygons.inc"
};
static Sint32 draw_469_vertices[83] = {
#include "stg23/terrain/draw_469_vertices.inc"
};
static NJS_CNK_MODEL draw_469_model = {
#include "stg23/terrain/draw_469_model.inc"
};
static NJS_CNK_OBJECT draw_469 = {
#include "stg23/terrain/draw_469.inc"
};
static Sint16 draw_465_polygons[167] = {
#include "stg23/terrain/draw_465_polygons.inc"
};
static Sint32 draw_465_vertices[171] = {
#include "stg23/terrain/draw_465_vertices.inc"
};
static NJS_CNK_MODEL draw_465_model = {
#include "stg23/terrain/draw_465_model.inc"
};
static NJS_CNK_OBJECT draw_465 = {
#include "stg23/terrain/draw_465.inc"
};
static Sint16 draw_464_polygons[1113] = {
#include "stg23/terrain/draw_464_polygons.inc"
};
static Sint32 draw_464_vertices[827] = {
#include "stg23/terrain/draw_464_vertices.inc"
};
static NJS_CNK_MODEL draw_464_model = {
#include "stg23/terrain/draw_464_model.inc"
};
static NJS_CNK_OBJECT draw_464 = {
#include "stg23/terrain/draw_464.inc"
};
static Sint16 draw_462_polygons[213] = {
#include "stg23/terrain/draw_462_polygons.inc"
};
static Sint32 draw_462_vertices[127] = {
#include "stg23/terrain/draw_462_vertices.inc"
};
static NJS_CNK_MODEL draw_462_model = {
#include "stg23/terrain/draw_462_model.inc"
};
static NJS_CNK_OBJECT draw_463 = {
#include "stg23/terrain/draw_463.inc"
};
static Sint16 draw_460_polygons[679] = {
#include "stg23/terrain/draw_460_polygons.inc"
};
static Sint32 draw_460_vertices[435] = {
#include "stg23/terrain/draw_460_vertices.inc"
};
static NJS_CNK_MODEL draw_460_model = {
#include "stg23/terrain/draw_460_model.inc"
};
static NJS_CNK_OBJECT draw_461 = {
#include "stg23/terrain/draw_461.inc"
};
static Sint16 draw_458_polygons[213] = {
#include "stg23/terrain/draw_458_polygons.inc"
};
static Sint32 draw_458_vertices[123] = {
#include "stg23/terrain/draw_458_vertices.inc"
};
static NJS_CNK_MODEL draw_458_model = {
#include "stg23/terrain/draw_458_model.inc"
};
static NJS_CNK_OBJECT draw_459 = {
#include "stg23/terrain/draw_459.inc"
};
static Sint16 draw_456_polygons[659] = {
#include "stg23/terrain/draw_456_polygons.inc"
};
static Sint32 draw_456_vertices[403] = {
#include "stg23/terrain/draw_456_vertices.inc"
};
static NJS_CNK_MODEL draw_456_model = {
#include "stg23/terrain/draw_456_model.inc"
};
static NJS_CNK_OBJECT draw_457 = {
#include "stg23/terrain/draw_457.inc"
};
static Sint16 draw_455_polygons[259] = {
#include "stg23/terrain/draw_455_polygons.inc"
};
static Sint32 draw_455_vertices[211] = {
#include "stg23/terrain/draw_455_vertices.inc"
};
static NJS_CNK_MODEL draw_455_model = {
#include "stg23/terrain/draw_455_model.inc"
};
static NJS_CNK_OBJECT draw_455 = {
#include "stg23/terrain/draw_455.inc"
};
static Sint16 draw_453_polygons[195] = {
#include "stg23/terrain/draw_453_polygons.inc"
};
static Sint32 draw_453_vertices[103] = {
#include "stg23/terrain/draw_453_vertices.inc"
};
static NJS_CNK_MODEL draw_453_model = {
#include "stg23/terrain/draw_453_model.inc"
};
static NJS_CNK_OBJECT draw_454 = {
#include "stg23/terrain/draw_454.inc"
};
static Sint16 draw_452_polygons[1019] = {
#include "stg23/terrain/draw_452_polygons.inc"
};
static Sint32 draw_452_vertices[887] = {
#include "stg23/terrain/draw_452_vertices.inc"
};
static NJS_CNK_MODEL draw_452_model = {
#include "stg23/terrain/draw_452_model.inc"
};
static NJS_CNK_OBJECT draw_452 = {
#include "stg23/terrain/draw_452.inc"
};
static Sint16 draw_451_polygons[65] = {
#include "stg23/terrain/draw_451_polygons.inc"
};
static Sint32 draw_451_vertices[67] = {
#include "stg23/terrain/draw_451_vertices.inc"
};
static NJS_CNK_MODEL draw_451_model = {
#include "stg23/terrain/draw_451_model.inc"
};
static NJS_CNK_OBJECT draw_451 = {
#include "stg23/terrain/draw_451.inc"
};
static Sint16 draw_450_polygons[425] = {
#include "stg23/terrain/draw_450_polygons.inc"
};
static Sint32 draw_450_vertices[383] = {
#include "stg23/terrain/draw_450_vertices.inc"
};
static NJS_CNK_MODEL draw_450_model = {
#include "stg23/terrain/draw_450_model.inc"
};
static NJS_CNK_OBJECT draw_450 = {
#include "stg23/terrain/draw_450.inc"
};
static Sint16 draw_449_polygons[633] = {
#include "stg23/terrain/draw_449_polygons.inc"
};
static Sint32 draw_449_vertices[599] = {
#include "stg23/terrain/draw_449_vertices.inc"
};
static NJS_CNK_MODEL draw_449_model = {
#include "stg23/terrain/draw_449_model.inc"
};
static NJS_CNK_OBJECT draw_449 = {
#include "stg23/terrain/draw_449.inc"
};
static Sint16 draw_447_polygons[213] = {
#include "stg23/terrain/draw_447_polygons.inc"
};
static Sint32 draw_447_vertices[143] = {
#include "stg23/terrain/draw_447_vertices.inc"
};
static NJS_CNK_MODEL draw_447_model = {
#include "stg23/terrain/draw_447_model.inc"
};
static NJS_CNK_OBJECT draw_448 = {
#include "stg23/terrain/draw_448.inc"
};
static Sint16 draw_444_polygons[213] = {
#include "stg23/terrain/draw_444_polygons.inc"
};
static Sint32 draw_444_vertices[143] = {
#include "stg23/terrain/draw_444_vertices.inc"
};
static NJS_CNK_MODEL draw_444_model = {
#include "stg23/terrain/draw_444_model.inc"
};
static NJS_CNK_OBJECT draw_446 = {
#include "stg23/terrain/draw_446.inc"
};
static Sint16 draw_441_polygons[25] = {
#include "stg23/terrain/draw_441_polygons.inc"
};
static Sint32 draw_441_vertices[19] = {
#include "stg23/terrain/draw_441_vertices.inc"
};
static NJS_CNK_MODEL draw_441_model = {
#include "stg23/terrain/draw_441_model.inc"
};
static NJS_CNK_OBJECT draw_443 = {
#include "stg23/terrain/draw_443.inc"
};
static Sint16 draw_440_polygons[221] = {
#include "stg23/terrain/draw_440_polygons.inc"
};
static Sint32 draw_440_vertices[267] = {
#include "stg23/terrain/draw_440_vertices.inc"
};
static NJS_CNK_MODEL draw_440_model = {
#include "stg23/terrain/draw_440_model.inc"
};
static NJS_CNK_OBJECT draw_440 = {
#include "stg23/terrain/draw_440.inc"
};
static Sint16 draw_439_polygons[135] = {
#include "stg23/terrain/draw_439_polygons.inc"
};
static Sint32 draw_439_vertices[155] = {
#include "stg23/terrain/draw_439_vertices.inc"
};
static NJS_CNK_MODEL draw_439_model = {
#include "stg23/terrain/draw_439_model.inc"
};
static NJS_CNK_OBJECT draw_439 = {
#include "stg23/terrain/draw_439.inc"
};
static Sint16 draw_438_polygons[41] = {
#include "stg23/terrain/draw_438_polygons.inc"
};
static Sint32 draw_438_vertices[39] = {
#include "stg23/terrain/draw_438_vertices.inc"
};
static NJS_CNK_MODEL draw_438_model = {
#include "stg23/terrain/draw_438_model.inc"
};
static NJS_CNK_OBJECT draw_438 = {
#include "stg23/terrain/draw_438.inc"
};
static Sint16 draw_437_polygons[143] = {
#include "stg23/terrain/draw_437_polygons.inc"
};
static Sint32 draw_437_vertices[163] = {
#include "stg23/terrain/draw_437_vertices.inc"
};
static NJS_CNK_MODEL draw_437_model = {
#include "stg23/terrain/draw_437_model.inc"
};
static NJS_CNK_OBJECT draw_437 = {
#include "stg23/terrain/draw_437.inc"
};
static Sint16 draw_436_polygons[83] = {
#include "stg23/terrain/draw_436_polygons.inc"
};
static Sint32 draw_436_vertices[63] = {
#include "stg23/terrain/draw_436_vertices.inc"
};
static NJS_CNK_MODEL draw_436_model = {
#include "stg23/terrain/draw_436_model.inc"
};
static NJS_CNK_OBJECT draw_436 = {
#include "stg23/terrain/draw_436.inc"
};
static Sint16 draw_434_polygons[307] = {
#include "stg23/terrain/draw_434_polygons.inc"
};
static Sint32 draw_434_vertices[227] = {
#include "stg23/terrain/draw_434_vertices.inc"
};
static NJS_CNK_MODEL draw_434_model = {
#include "stg23/terrain/draw_434_model.inc"
};
static NJS_CNK_OBJECT draw_435 = {
#include "stg23/terrain/draw_435.inc"
};
static Sint16 draw_433_polygons[159] = {
#include "stg23/terrain/draw_433_polygons.inc"
};
static Sint32 draw_433_vertices[131] = {
#include "stg23/terrain/draw_433_vertices.inc"
};
static NJS_CNK_MODEL draw_433_model = {
#include "stg23/terrain/draw_433_model.inc"
};
static NJS_CNK_OBJECT draw_433 = {
#include "stg23/terrain/draw_433.inc"
};
static Sint16 draw_432_polygons[105] = {
#include "stg23/terrain/draw_432_polygons.inc"
};
static Sint32 draw_432_vertices[87] = {
#include "stg23/terrain/draw_432_vertices.inc"
};
static NJS_CNK_MODEL draw_432_model = {
#include "stg23/terrain/draw_432_model.inc"
};
static NJS_CNK_OBJECT draw_432 = {
#include "stg23/terrain/draw_432.inc"
};
static Sint16 draw_431_polygons[95] = {
#include "stg23/terrain/draw_431_polygons.inc"
};
static Sint32 draw_431_vertices[99] = {
#include "stg23/terrain/draw_431_vertices.inc"
};
static NJS_CNK_MODEL draw_431_model = {
#include "stg23/terrain/draw_431_model.inc"
};
static NJS_CNK_OBJECT draw_431 = {
#include "stg23/terrain/draw_431.inc"
};
static Sint16 draw_430_polygons[35] = {
#include "stg23/terrain/draw_430_polygons.inc"
};
static Sint32 draw_430_vertices[27] = {
#include "stg23/terrain/draw_430_vertices.inc"
};
static NJS_CNK_MODEL draw_430_model = {
#include "stg23/terrain/draw_430_model.inc"
};
static NJS_CNK_OBJECT draw_430 = {
#include "stg23/terrain/draw_430.inc"
};
static Sint16 draw_429_polygons[81] = {
#include "stg23/terrain/draw_429_polygons.inc"
};
static Sint32 draw_429_vertices[79] = {
#include "stg23/terrain/draw_429_vertices.inc"
};
static NJS_CNK_MODEL draw_429_model = {
#include "stg23/terrain/draw_429_model.inc"
};
static NJS_CNK_OBJECT draw_429 = {
#include "stg23/terrain/draw_429.inc"
};
static Sint16 draw_428_polygons[25] = {
#include "stg23/terrain/draw_428_polygons.inc"
};
static Sint32 draw_428_vertices[19] = {
#include "stg23/terrain/draw_428_vertices.inc"
};
static NJS_CNK_MODEL draw_428_model = {
#include "stg23/terrain/draw_428_model.inc"
};
static NJS_CNK_OBJECT draw_428 = {
#include "stg23/terrain/draw_428.inc"
};
static Sint16 draw_427_polygons[89] = {
#include "stg23/terrain/draw_427_polygons.inc"
};
static Sint32 draw_427_vertices[67] = {
#include "stg23/terrain/draw_427_vertices.inc"
};
static NJS_CNK_MODEL draw_427_model = {
#include "stg23/terrain/draw_427_model.inc"
};
static NJS_CNK_OBJECT draw_427 = {
#include "stg23/terrain/draw_427.inc"
};
static Sint16 draw_424_polygons[79] = {
#include "stg23/terrain/draw_424_polygons.inc"
};
static Sint32 draw_424_vertices[51] = {
#include "stg23/terrain/draw_424_vertices.inc"
};
static NJS_CNK_MODEL draw_424_model = {
#include "stg23/terrain/draw_424_model.inc"
};
static NJS_CNK_OBJECT draw_426 = {
#include "stg23/terrain/draw_426.inc"
};
static Sint16 draw_421_polygons[79] = {
#include "stg23/terrain/draw_421_polygons.inc"
};
static Sint32 draw_421_vertices[51] = {
#include "stg23/terrain/draw_421_vertices.inc"
};
static NJS_CNK_MODEL draw_421_model = {
#include "stg23/terrain/draw_421_model.inc"
};
static NJS_CNK_OBJECT draw_423 = {
#include "stg23/terrain/draw_423.inc"
};
static Sint16 draw_420_polygons[51] = {
#include "stg23/terrain/draw_420_polygons.inc"
};
static Sint32 draw_420_vertices[51] = {
#include "stg23/terrain/draw_420_vertices.inc"
};
static NJS_CNK_MODEL draw_420_model = {
#include "stg23/terrain/draw_420_model.inc"
};
static NJS_CNK_OBJECT draw_420 = {
#include "stg23/terrain/draw_420.inc"
};
static Sint16 draw_419_polygons[51] = {
#include "stg23/terrain/draw_419_polygons.inc"
};
static Sint32 draw_419_vertices[51] = {
#include "stg23/terrain/draw_419_vertices.inc"
};
static NJS_CNK_MODEL draw_419_model = {
#include "stg23/terrain/draw_419_model.inc"
};
static NJS_CNK_OBJECT draw_419 = {
#include "stg23/terrain/draw_419.inc"
};
static Sint16 draw_411_polygons[63] = {
#include "stg23/terrain/draw_411_polygons.inc"
};
static Sint32 draw_411_vertices[51] = {
#include "stg23/terrain/draw_411_vertices.inc"
};
static NJS_CNK_MODEL draw_411_model = {
#include "stg23/terrain/draw_411_model.inc"
};
static NJS_CNK_OBJECT draw_418 = {
#include "stg23/terrain/draw_418.inc"
};
static Sint16 draw_407_polygons[195] = {
#include "stg23/terrain/draw_407_polygons.inc"
};
static Sint32 draw_407_vertices[103] = {
#include "stg23/terrain/draw_407_vertices.inc"
};
static NJS_CNK_MODEL draw_407_model = {
#include "stg23/terrain/draw_407_model.inc"
};
static NJS_CNK_OBJECT draw_410 = {
#include "stg23/terrain/draw_410.inc"
};
static Sint16 draw_406_polygons[195] = {
#include "stg23/terrain/draw_406_polygons.inc"
};
static Sint32 draw_406_vertices[103] = {
#include "stg23/terrain/draw_406_vertices.inc"
};
static NJS_CNK_MODEL draw_406_model = {
#include "stg23/terrain/draw_406_model.inc"
};
static NJS_CNK_OBJECT draw_406 = {
#include "stg23/terrain/draw_406.inc"
};
static Sint16 draw_363_polygons[203] = {
#include "stg23/terrain/draw_363_polygons.inc"
};
static Sint32 draw_363_vertices[191] = {
#include "stg23/terrain/draw_363_vertices.inc"
};
static NJS_CNK_MODEL draw_363_model = {
#include "stg23/terrain/draw_363_model.inc"
};
static NJS_CNK_OBJECT draw_363 = {
#include "stg23/terrain/draw_363.inc"
};
static Sint16 draw_362_polygons[89] = {
#include "stg23/terrain/draw_362_polygons.inc"
};
static Sint32 draw_362_vertices[83] = {
#include "stg23/terrain/draw_362_vertices.inc"
};
static NJS_CNK_MODEL draw_362_model = {
#include "stg23/terrain/draw_362_model.inc"
};
static NJS_CNK_OBJECT draw_362 = {
#include "stg23/terrain/draw_362.inc"
};
static Sint16 draw_361_polygons[307] = {
#include "stg23/terrain/draw_361_polygons.inc"
};
static Sint32 draw_361_vertices[223] = {
#include "stg23/terrain/draw_361_vertices.inc"
};
static NJS_CNK_MODEL draw_361_model = {
#include "stg23/terrain/draw_361_model.inc"
};
static NJS_CNK_OBJECT draw_361 = {
#include "stg23/terrain/draw_361.inc"
};
static Sint16 draw_359_polygons[183] = {
#include "stg23/terrain/draw_359_polygons.inc"
};
static Sint32 draw_359_vertices[147] = {
#include "stg23/terrain/draw_359_vertices.inc"
};
static NJS_CNK_MODEL draw_359_model = {
#include "stg23/terrain/draw_359_model.inc"
};
static NJS_CNK_OBJECT draw_360 = {
#include "stg23/terrain/draw_360.inc"
};
static Sint16 draw_358_polygons[153] = {
#include "stg23/terrain/draw_358_polygons.inc"
};
static Sint32 draw_358_vertices[123] = {
#include "stg23/terrain/draw_358_vertices.inc"
};
static NJS_CNK_MODEL draw_358_model = {
#include "stg23/terrain/draw_358_model.inc"
};
static NJS_CNK_OBJECT draw_358 = {
#include "stg23/terrain/draw_358.inc"
};
static Sint16 draw_357_polygons[185] = {
#include "stg23/terrain/draw_357_polygons.inc"
};
static Sint32 draw_357_vertices[183] = {
#include "stg23/terrain/draw_357_vertices.inc"
};
static NJS_CNK_MODEL draw_357_model = {
#include "stg23/terrain/draw_357_model.inc"
};
static NJS_CNK_OBJECT draw_357 = {
#include "stg23/terrain/draw_357.inc"
};
static Sint16 draw_356_polygons[83] = {
#include "stg23/terrain/draw_356_polygons.inc"
};
static Sint32 draw_356_vertices[75] = {
#include "stg23/terrain/draw_356_vertices.inc"
};
static NJS_CNK_MODEL draw_356_model = {
#include "stg23/terrain/draw_356_model.inc"
};
static NJS_CNK_OBJECT draw_356 = {
#include "stg23/terrain/draw_356.inc"
};
static Sint16 draw_355_polygons[83] = {
#include "stg23/terrain/draw_355_polygons.inc"
};
static Sint32 draw_355_vertices[83] = {
#include "stg23/terrain/draw_355_vertices.inc"
};
static NJS_CNK_MODEL draw_355_model = {
#include "stg23/terrain/draw_355_model.inc"
};
static NJS_CNK_OBJECT draw_355 = {
#include "stg23/terrain/draw_355.inc"
};
static Sint16 draw_354_polygons[83] = {
#include "stg23/terrain/draw_354_polygons.inc"
};
static Sint32 draw_354_vertices[75] = {
#include "stg23/terrain/draw_354_vertices.inc"
};
static NJS_CNK_MODEL draw_354_model = {
#include "stg23/terrain/draw_354_model.inc"
};
static NJS_CNK_OBJECT draw_354 = {
#include "stg23/terrain/draw_354.inc"
};
static Sint16 draw_353_polygons[179] = {
#include "stg23/terrain/draw_353_polygons.inc"
};
static Sint32 draw_353_vertices[151] = {
#include "stg23/terrain/draw_353_vertices.inc"
};
static NJS_CNK_MODEL draw_353_model = {
#include "stg23/terrain/draw_353_model.inc"
};
static NJS_CNK_OBJECT draw_353 = {
#include "stg23/terrain/draw_353.inc"
};
static Sint16 draw_352_polygons[347] = {
#include "stg23/terrain/draw_352_polygons.inc"
};
static Sint32 draw_352_vertices[227] = {
#include "stg23/terrain/draw_352_vertices.inc"
};
static NJS_CNK_MODEL draw_352_model = {
#include "stg23/terrain/draw_352_model.inc"
};
static NJS_CNK_OBJECT draw_352 = {
#include "stg23/terrain/draw_352.inc"
};
static Sint16 draw_351_polygons[229] = {
#include "stg23/terrain/draw_351_polygons.inc"
};
static Sint32 draw_351_vertices[175] = {
#include "stg23/terrain/draw_351_vertices.inc"
};
static NJS_CNK_MODEL draw_351_model = {
#include "stg23/terrain/draw_351_model.inc"
};
static NJS_CNK_OBJECT draw_351 = {
#include "stg23/terrain/draw_351.inc"
};
static Sint16 draw_350_polygons[169] = {
#include "stg23/terrain/draw_350_polygons.inc"
};
static Sint32 draw_350_vertices[115] = {
#include "stg23/terrain/draw_350_vertices.inc"
};
static NJS_CNK_MODEL draw_350_model = {
#include "stg23/terrain/draw_350_model.inc"
};
static NJS_CNK_OBJECT draw_350 = {
#include "stg23/terrain/draw_350.inc"
};
static Sint16 draw_305_polygons[199] = {
#include "stg23/terrain/draw_305_polygons.inc"
};
static Sint32 draw_305_vertices[163] = {
#include "stg23/terrain/draw_305_vertices.inc"
};
static NJS_CNK_MODEL draw_305_model = {
#include "stg23/terrain/draw_305_model.inc"
};
static NJS_CNK_OBJECT draw_306 = {
#include "stg23/terrain/draw_306.inc"
};
static Sint16 draw_304_polygons[25] = {
#include "stg23/terrain/draw_304_polygons.inc"
};
static Sint32 draw_304_vertices[19] = {
#include "stg23/terrain/draw_304_vertices.inc"
};
static NJS_CNK_MODEL draw_304_model = {
#include "stg23/terrain/draw_304_model.inc"
};
static NJS_CNK_OBJECT draw_304 = {
#include "stg23/terrain/draw_304.inc"
};
static Sint16 draw_303_polygons[65] = {
#include "stg23/terrain/draw_303_polygons.inc"
};
static Sint32 draw_303_vertices[67] = {
#include "stg23/terrain/draw_303_vertices.inc"
};
static NJS_CNK_MODEL draw_303_model = {
#include "stg23/terrain/draw_303_model.inc"
};
static NJS_CNK_OBJECT draw_303 = {
#include "stg23/terrain/draw_303.inc"
};
static Sint16 draw_302_polygons[75] = {
#include "stg23/terrain/draw_302_polygons.inc"
};
static Sint32 draw_302_vertices[43] = {
#include "stg23/terrain/draw_302_vertices.inc"
};
static NJS_CNK_MODEL draw_302_model = {
#include "stg23/terrain/draw_302_model.inc"
};
static NJS_CNK_OBJECT draw_302 = {
#include "stg23/terrain/draw_302.inc"
};
static Sint16 draw_301_polygons[399] = {
#include "stg23/terrain/draw_301_polygons.inc"
};
static Sint32 draw_301_vertices[359] = {
#include "stg23/terrain/draw_301_vertices.inc"
};
static NJS_CNK_MODEL draw_301_model = {
#include "stg23/terrain/draw_301_model.inc"
};
static NJS_CNK_OBJECT draw_301 = {
#include "stg23/terrain/draw_301.inc"
};
static Sint16 draw_285_polygons[1053] = {
#include "stg23/terrain/draw_285_polygons.inc"
};
static Sint32 draw_285_vertices[743] = {
#include "stg23/terrain/draw_285_vertices.inc"
};
static NJS_CNK_MODEL draw_285_model = {
#include "stg23/terrain/draw_285_model.inc"
};
static NJS_CNK_OBJECT draw_286 = {
#include "stg23/terrain/draw_286.inc"
};
static Sint16 draw_283_polygons[193] = {
#include "stg23/terrain/draw_283_polygons.inc"
};
static Sint32 draw_283_vertices[103] = {
#include "stg23/terrain/draw_283_vertices.inc"
};
static NJS_CNK_MODEL draw_283_model = {
#include "stg23/terrain/draw_283_model.inc"
};
static NJS_CNK_OBJECT draw_284 = {
#include "stg23/terrain/draw_284.inc"
};
static Sint16 draw_281_polygons[771] = {
#include "stg23/terrain/draw_281_polygons.inc"
};
static Sint32 draw_281_vertices[455] = {
#include "stg23/terrain/draw_281_vertices.inc"
};
static NJS_CNK_MODEL draw_281_model = {
#include "stg23/terrain/draw_281_model.inc"
};
static NJS_CNK_OBJECT draw_282 = {
#include "stg23/terrain/draw_282.inc"
};
static Sint16 draw_280_polygons[341] = {
#include "stg23/terrain/draw_280_polygons.inc"
};
static Sint32 draw_280_vertices[319] = {
#include "stg23/terrain/draw_280_vertices.inc"
};
static NJS_CNK_MODEL draw_280_model = {
#include "stg23/terrain/draw_280_model.inc"
};
static NJS_CNK_OBJECT draw_280 = {
#include "stg23/terrain/draw_280.inc"
};
static Sint16 draw_279_polygons[89] = {
#include "stg23/terrain/draw_279_polygons.inc"
};
static Sint32 draw_279_vertices[83] = {
#include "stg23/terrain/draw_279_vertices.inc"
};
static NJS_CNK_MODEL draw_279_model = {
#include "stg23/terrain/draw_279_model.inc"
};
static NJS_CNK_OBJECT draw_279 = {
#include "stg23/terrain/draw_279.inc"
};
static Sint16 draw_278_polygons[89] = {
#include "stg23/terrain/draw_278_polygons.inc"
};
static Sint32 draw_278_vertices[83] = {
#include "stg23/terrain/draw_278_vertices.inc"
};
static NJS_CNK_MODEL draw_278_model = {
#include "stg23/terrain/draw_278_model.inc"
};
static NJS_CNK_OBJECT draw_278 = {
#include "stg23/terrain/draw_278.inc"
};
static Sint16 draw_277_polygons[95] = {
#include "stg23/terrain/draw_277_polygons.inc"
};
static Sint32 draw_277_vertices[91] = {
#include "stg23/terrain/draw_277_vertices.inc"
};
static NJS_CNK_MODEL draw_277_model = {
#include "stg23/terrain/draw_277_model.inc"
};
static NJS_CNK_OBJECT draw_277 = {
#include "stg23/terrain/draw_277.inc"
};
static Sint16 draw_276_polygons[91] = {
#include "stg23/terrain/draw_276_polygons.inc"
};
static Sint32 draw_276_vertices[99] = {
#include "stg23/terrain/draw_276_vertices.inc"
};
static NJS_CNK_MODEL draw_276_model = {
#include "stg23/terrain/draw_276_model.inc"
};
static NJS_CNK_OBJECT draw_276 = {
#include "stg23/terrain/draw_276.inc"
};
static Sint16 draw_275_polygons[107] = {
#include "stg23/terrain/draw_275_polygons.inc"
};
static Sint32 draw_275_vertices[99] = {
#include "stg23/terrain/draw_275_vertices.inc"
};
static NJS_CNK_MODEL draw_275_model = {
#include "stg23/terrain/draw_275_model.inc"
};
static NJS_CNK_OBJECT draw_275 = {
#include "stg23/terrain/draw_275.inc"
};
static Sint16 draw_274_polygons[93] = {
#include "stg23/terrain/draw_274_polygons.inc"
};
static Sint32 draw_274_vertices[83] = {
#include "stg23/terrain/draw_274_vertices.inc"
};
static NJS_CNK_MODEL draw_274_model = {
#include "stg23/terrain/draw_274_model.inc"
};
static NJS_CNK_OBJECT draw_274 = {
#include "stg23/terrain/draw_274.inc"
};
static Sint16 draw_273_polygons[47] = {
#include "stg23/terrain/draw_273_polygons.inc"
};
static Sint32 draw_273_vertices[47] = {
#include "stg23/terrain/draw_273_vertices.inc"
};
static NJS_CNK_MODEL draw_273_model = {
#include "stg23/terrain/draw_273_model.inc"
};
static NJS_CNK_OBJECT draw_273 = {
#include "stg23/terrain/draw_273.inc"
};
static Sint16 draw_272_polygons[281] = {
#include "stg23/terrain/draw_272_polygons.inc"
};
static Sint32 draw_272_vertices[303] = {
#include "stg23/terrain/draw_272_vertices.inc"
};
static NJS_CNK_MODEL draw_272_model = {
#include "stg23/terrain/draw_272_model.inc"
};
static NJS_CNK_OBJECT draw_272 = {
#include "stg23/terrain/draw_272.inc"
};
static Sint16 draw_271_polygons[149] = {
#include "stg23/terrain/draw_271_polygons.inc"
};
static Sint32 draw_271_vertices[135] = {
#include "stg23/terrain/draw_271_vertices.inc"
};
static NJS_CNK_MODEL draw_271_model = {
#include "stg23/terrain/draw_271_model.inc"
};
static NJS_CNK_OBJECT draw_271 = {
#include "stg23/terrain/draw_271.inc"
};
static Sint16 draw_270_polygons[157] = {
#include "stg23/terrain/draw_270_polygons.inc"
};
static Sint32 draw_270_vertices[139] = {
#include "stg23/terrain/draw_270_vertices.inc"
};
static NJS_CNK_MODEL draw_270_model = {
#include "stg23/terrain/draw_270_model.inc"
};
static NJS_CNK_OBJECT draw_270 = {
#include "stg23/terrain/draw_270.inc"
};
static Sint16 draw_269_polygons[79] = {
#include "stg23/terrain/draw_269_polygons.inc"
};
static Sint32 draw_269_vertices[83] = {
#include "stg23/terrain/draw_269_vertices.inc"
};
static NJS_CNK_MODEL draw_269_model = {
#include "stg23/terrain/draw_269_model.inc"
};
static NJS_CNK_OBJECT draw_269 = {
#include "stg23/terrain/draw_269.inc"
};
static Sint16 draw_268_polygons[231] = {
#include "stg23/terrain/draw_268_polygons.inc"
};
static Sint32 draw_268_vertices[215] = {
#include "stg23/terrain/draw_268_vertices.inc"
};
static NJS_CNK_MODEL draw_268_model = {
#include "stg23/terrain/draw_268_model.inc"
};
static NJS_CNK_OBJECT draw_268 = {
#include "stg23/terrain/draw_268.inc"
};
static Sint16 draw_267_polygons[103] = {
#include "stg23/terrain/draw_267_polygons.inc"
};
static Sint32 draw_267_vertices[115] = {
#include "stg23/terrain/draw_267_vertices.inc"
};
static NJS_CNK_MODEL draw_267_model = {
#include "stg23/terrain/draw_267_model.inc"
};
static NJS_CNK_OBJECT draw_267 = {
#include "stg23/terrain/draw_267.inc"
};
static Sint16 draw_263_polygons[141] = {
#include "stg23/terrain/draw_263_polygons.inc"
};
static Sint32 draw_263_vertices[103] = {
#include "stg23/terrain/draw_263_vertices.inc"
};
static NJS_CNK_MODEL draw_263_model = {
#include "stg23/terrain/draw_263_model.inc"
};
static NJS_CNK_OBJECT draw_266 = {
#include "stg23/terrain/draw_266.inc"
};
static Sint16 draw_260_polygons[135] = {
#include "stg23/terrain/draw_260_polygons.inc"
};
static Sint32 draw_260_vertices[107] = {
#include "stg23/terrain/draw_260_vertices.inc"
};
static NJS_CNK_MODEL draw_260_model = {
#include "stg23/terrain/draw_260_model.inc"
};
static NJS_CNK_OBJECT draw_261 = {
#include "stg23/terrain/draw_261.inc"
};
static Sint16 draw_254_polygons[141] = {
#include "stg23/terrain/draw_254_polygons.inc"
};
static Sint32 draw_254_vertices[103] = {
#include "stg23/terrain/draw_254_vertices.inc"
};
static NJS_CNK_MODEL draw_254_model = {
#include "stg23/terrain/draw_254_model.inc"
};
static NJS_CNK_OBJECT draw_254 = {
#include "stg23/terrain/draw_254.inc"
};
static Sint16 draw_253_polygons[121] = {
#include "stg23/terrain/draw_253_polygons.inc"
};
static Sint32 draw_253_vertices[91] = {
#include "stg23/terrain/draw_253_vertices.inc"
};
static NJS_CNK_MODEL draw_253_model = {
#include "stg23/terrain/draw_253_model.inc"
};
static NJS_CNK_OBJECT draw_253 = {
#include "stg23/terrain/draw_253.inc"
};
static Sint16 draw_251_polygons[159] = {
#include "stg23/terrain/draw_251_polygons.inc"
};
static Sint32 draw_251_vertices[127] = {
#include "stg23/terrain/draw_251_vertices.inc"
};
static NJS_CNK_MODEL draw_251_model = {
#include "stg23/terrain/draw_251_model.inc"
};
static NJS_CNK_OBJECT draw_251 = {
#include "stg23/terrain/draw_251.inc"
};
static Sint16 draw_249_polygons[95] = {
#include "stg23/terrain/draw_249_polygons.inc"
};
static Sint32 draw_249_vertices[79] = {
#include "stg23/terrain/draw_249_vertices.inc"
};
static NJS_CNK_MODEL draw_249_model = {
#include "stg23/terrain/draw_249_model.inc"
};
static NJS_CNK_OBJECT draw_250 = {
#include "stg23/terrain/draw_250.inc"
};
static Sint16 draw_248_polygons[71] = {
#include "stg23/terrain/draw_248_polygons.inc"
};
static Sint32 draw_248_vertices[55] = {
#include "stg23/terrain/draw_248_vertices.inc"
};
static NJS_CNK_MODEL draw_248_model = {
#include "stg23/terrain/draw_248_model.inc"
};
static NJS_CNK_OBJECT draw_248 = {
#include "stg23/terrain/draw_248.inc"
};
static Sint16 draw_247_polygons[927] = {
#include "stg23/terrain/draw_247_polygons.inc"
};
static Sint32 draw_247_vertices[795] = {
#include "stg23/terrain/draw_247_vertices.inc"
};
static NJS_CNK_MODEL draw_247_model = {
#include "stg23/terrain/draw_247_model.inc"
};
static NJS_CNK_OBJECT draw_247 = {
#include "stg23/terrain/draw_247.inc"
};
static Sint16 draw_246_polygons[141] = {
#include "stg23/terrain/draw_246_polygons.inc"
};
static Sint32 draw_246_vertices[111] = {
#include "stg23/terrain/draw_246_vertices.inc"
};
static NJS_CNK_MODEL draw_246_model = {
#include "stg23/terrain/draw_246_model.inc"
};
static NJS_CNK_OBJECT draw_246 = {
#include "stg23/terrain/draw_246.inc"
};
static Sint16 draw_245_polygons[149] = {
#include "stg23/terrain/draw_245_polygons.inc"
};
static Sint32 draw_245_vertices[135] = {
#include "stg23/terrain/draw_245_vertices.inc"
};
static NJS_CNK_MODEL draw_245_model = {
#include "stg23/terrain/draw_245_model.inc"
};
static NJS_CNK_OBJECT draw_245 = {
#include "stg23/terrain/draw_245.inc"
};
static Sint16 draw_244_polygons[215] = {
#include "stg23/terrain/draw_244_polygons.inc"
};
static Sint32 draw_244_vertices[199] = {
#include "stg23/terrain/draw_244_vertices.inc"
};
static NJS_CNK_MODEL draw_244_model = {
#include "stg23/terrain/draw_244_model.inc"
};
static NJS_CNK_OBJECT draw_244 = {
#include "stg23/terrain/draw_244.inc"
};
static Sint16 draw_241_polygons[55] = {
#include "stg23/terrain/draw_241_polygons.inc"
};
static Sint32 draw_241_vertices[51] = {
#include "stg23/terrain/draw_241_vertices.inc"
};
static NJS_CNK_MODEL draw_241_model = {
#include "stg23/terrain/draw_241_model.inc"
};
static NJS_CNK_OBJECT draw_243 = {
#include "stg23/terrain/draw_243.inc"
};
static Sint16 draw_239_polygons[81] = {
#include "stg23/terrain/draw_239_polygons.inc"
};
static Sint32 draw_239_vertices[67] = {
#include "stg23/terrain/draw_239_vertices.inc"
};
static NJS_CNK_MODEL draw_239_model = {
#include "stg23/terrain/draw_239_model.inc"
};
static NJS_CNK_OBJECT draw_240 = {
#include "stg23/terrain/draw_240.inc"
};
static Sint16 draw_238_polygons[107] = {
#include "stg23/terrain/draw_238_polygons.inc"
};
static Sint32 draw_238_vertices[83] = {
#include "stg23/terrain/draw_238_vertices.inc"
};
static NJS_CNK_MODEL draw_238_model = {
#include "stg23/terrain/draw_238_model.inc"
};
static NJS_CNK_OBJECT draw_238 = {
#include "stg23/terrain/draw_238.inc"
};
static Sint16 draw_237_polygons[95] = {
#include "stg23/terrain/draw_237_polygons.inc"
};
static Sint32 draw_237_vertices[71] = {
#include "stg23/terrain/draw_237_vertices.inc"
};
static NJS_CNK_MODEL draw_237_model = {
#include "stg23/terrain/draw_237_model.inc"
};
static NJS_CNK_OBJECT draw_237 = {
#include "stg23/terrain/draw_237.inc"
};
static Sint16 draw_236_polygons[291] = {
#include "stg23/terrain/draw_236_polygons.inc"
};
static Sint32 draw_236_vertices[247] = {
#include "stg23/terrain/draw_236_vertices.inc"
};
static NJS_CNK_MODEL draw_236_model = {
#include "stg23/terrain/draw_236_model.inc"
};
static NJS_CNK_OBJECT draw_236 = {
#include "stg23/terrain/draw_236.inc"
};
static Sint16 draw_221_polygons[163] = {
#include "stg23/terrain/draw_221_polygons.inc"
};
static Sint32 draw_221_vertices[119] = {
#include "stg23/terrain/draw_221_vertices.inc"
};
static NJS_CNK_MODEL draw_221_model = {
#include "stg23/terrain/draw_221_model.inc"
};
static NJS_CNK_OBJECT draw_235 = {
#include "stg23/terrain/draw_235.inc"
};
static Sint16 draw_220_polygons[31] = {
#include "stg23/terrain/draw_220_polygons.inc"
};
static Sint32 draw_220_vertices[27] = {
#include "stg23/terrain/draw_220_vertices.inc"
};
static NJS_CNK_MODEL draw_220_model = {
#include "stg23/terrain/draw_220_model.inc"
};
static NJS_CNK_OBJECT draw_220 = {
#include "stg23/terrain/draw_220.inc"
};
static Sint16 draw_218_polygons[107] = {
#include "stg23/terrain/draw_218_polygons.inc"
};
static Sint32 draw_218_vertices[95] = {
#include "stg23/terrain/draw_218_vertices.inc"
};
static NJS_CNK_MODEL draw_218_model = {
#include "stg23/terrain/draw_218_model.inc"
};
static NJS_CNK_OBJECT draw_219 = {
#include "stg23/terrain/draw_219.inc"
};
static Sint16 draw_213_polygons[25] = {
#include "stg23/terrain/draw_213_polygons.inc"
};
static Sint32 draw_213_vertices[19] = {
#include "stg23/terrain/draw_213_vertices.inc"
};
static NJS_CNK_MODEL draw_213_model = {
#include "stg23/terrain/draw_213_model.inc"
};
static NJS_CNK_OBJECT draw_217 = {
#include "stg23/terrain/draw_217.inc"
};
static Sint16 draw_212_polygons[175] = {
#include "stg23/terrain/draw_212_polygons.inc"
};
static Sint32 draw_212_vertices[151] = {
#include "stg23/terrain/draw_212_vertices.inc"
};
static NJS_CNK_MODEL draw_212_model = {
#include "stg23/terrain/draw_212_model.inc"
};
static NJS_CNK_OBJECT draw_212 = {
#include "stg23/terrain/draw_212.inc"
};
static Sint16 draw_211_polygons[39] = {
#include "stg23/terrain/draw_211_polygons.inc"
};
static Sint32 draw_211_vertices[35] = {
#include "stg23/terrain/draw_211_vertices.inc"
};
static NJS_CNK_MODEL draw_211_model = {
#include "stg23/terrain/draw_211_model.inc"
};
static NJS_CNK_OBJECT draw_211 = {
#include "stg23/terrain/draw_211.inc"
};
static Sint16 draw_210_polygons[547] = {
#include "stg23/terrain/draw_210_polygons.inc"
};
static Sint32 draw_210_vertices[347] = {
#include "stg23/terrain/draw_210_vertices.inc"
};
static NJS_CNK_MODEL draw_210_model = {
#include "stg23/terrain/draw_210_model.inc"
};
static NJS_CNK_OBJECT draw_210 = {
#include "stg23/terrain/draw_210.inc"
};
static Sint16 draw_209_polygons[399] = {
#include "stg23/terrain/draw_209_polygons.inc"
};
static Sint32 draw_209_vertices[443] = {
#include "stg23/terrain/draw_209_vertices.inc"
};
static NJS_CNK_MODEL draw_209_model = {
#include "stg23/terrain/draw_209_model.inc"
};
static NJS_CNK_OBJECT draw_209 = {
#include "stg23/terrain/draw_209.inc"
};
static Sint16 draw_208_polygons[231] = {
#include "stg23/terrain/draw_208_polygons.inc"
};
static Sint32 draw_208_vertices[267] = {
#include "stg23/terrain/draw_208_vertices.inc"
};
static NJS_CNK_MODEL draw_208_model = {
#include "stg23/terrain/draw_208_model.inc"
};
static NJS_CNK_OBJECT draw_208 = {
#include "stg23/terrain/draw_208.inc"
};
static Sint16 draw_207_polygons[169] = {
#include "stg23/terrain/draw_207_polygons.inc"
};
static Sint32 draw_207_vertices[171] = {
#include "stg23/terrain/draw_207_vertices.inc"
};
static NJS_CNK_MODEL draw_207_model = {
#include "stg23/terrain/draw_207_model.inc"
};
static NJS_CNK_OBJECT draw_207 = {
#include "stg23/terrain/draw_207.inc"
};
static Sint16 draw_200_polygons[169] = {
#include "stg23/terrain/draw_200_polygons.inc"
};
static Sint32 draw_200_vertices[187] = {
#include "stg23/terrain/draw_200_vertices.inc"
};
static NJS_CNK_MODEL draw_200_model = {
#include "stg23/terrain/draw_200_model.inc"
};
static NJS_CNK_OBJECT draw_201 = {
#include "stg23/terrain/draw_201.inc"
};
static Sint16 draw_199_polygons[91] = {
#include "stg23/terrain/draw_199_polygons.inc"
};
static Sint32 draw_199_vertices[99] = {
#include "stg23/terrain/draw_199_vertices.inc"
};
static NJS_CNK_MODEL draw_199_model = {
#include "stg23/terrain/draw_199_model.inc"
};
static NJS_CNK_OBJECT draw_199 = {
#include "stg23/terrain/draw_199.inc"
};
static Sint16 draw_198_polygons[91] = {
#include "stg23/terrain/draw_198_polygons.inc"
};
static Sint32 draw_198_vertices[99] = {
#include "stg23/terrain/draw_198_vertices.inc"
};
static NJS_CNK_MODEL draw_198_model = {
#include "stg23/terrain/draw_198_model.inc"
};
static NJS_CNK_OBJECT draw_198 = {
#include "stg23/terrain/draw_198.inc"
};
static Sint16 draw_193_polygons[161] = {
#include "stg23/terrain/draw_193_polygons.inc"
};
static Sint32 draw_193_vertices[171] = {
#include "stg23/terrain/draw_193_vertices.inc"
};
static NJS_CNK_MODEL draw_193_model = {
#include "stg23/terrain/draw_193_model.inc"
};
static NJS_CNK_OBJECT draw_194 = {
#include "stg23/terrain/draw_194.inc"
};
static Sint16 draw_181_polygons[167] = {
#include "stg23/terrain/draw_181_polygons.inc"
};
static Sint32 draw_181_vertices[115] = {
#include "stg23/terrain/draw_181_vertices.inc"
};
static NJS_CNK_MODEL draw_181_model = {
#include "stg23/terrain/draw_181_model.inc"
};
static NJS_CNK_OBJECT draw_181 = {
#include "stg23/terrain/draw_181.inc"
};
static Sint16 draw_180_polygons[69] = {
#include "stg23/terrain/draw_180_polygons.inc"
};
static Sint32 draw_180_vertices[67] = {
#include "stg23/terrain/draw_180_vertices.inc"
};
static NJS_CNK_MODEL draw_180_model = {
#include "stg23/terrain/draw_180_model.inc"
};
static NJS_CNK_OBJECT draw_180 = {
#include "stg23/terrain/draw_180.inc"
};
static Sint16 draw_179_polygons[157] = {
#include "stg23/terrain/draw_179_polygons.inc"
};
static Sint32 draw_179_vertices[155] = {
#include "stg23/terrain/draw_179_vertices.inc"
};
static NJS_CNK_MODEL draw_179_model = {
#include "stg23/terrain/draw_179_model.inc"
};
static NJS_CNK_OBJECT draw_179 = {
#include "stg23/terrain/draw_179.inc"
};
static Sint16 draw_178_polygons[157] = {
#include "stg23/terrain/draw_178_polygons.inc"
};
static Sint32 draw_178_vertices[155] = {
#include "stg23/terrain/draw_178_vertices.inc"
};
static NJS_CNK_MODEL draw_178_model = {
#include "stg23/terrain/draw_178_model.inc"
};
static NJS_CNK_OBJECT draw_178 = {
#include "stg23/terrain/draw_178.inc"
};
static Sint16 draw_174_polygons[51] = {
#include "stg23/terrain/draw_174_polygons.inc"
};
static Sint32 draw_174_vertices[47] = {
#include "stg23/terrain/draw_174_vertices.inc"
};
static NJS_CNK_MODEL draw_174_model = {
#include "stg23/terrain/draw_174_model.inc"
};
static NJS_CNK_OBJECT draw_177 = {
#include "stg23/terrain/draw_177.inc"
};
static Sint16 draw_173_polygons[51] = {
#include "stg23/terrain/draw_173_polygons.inc"
};
static Sint32 draw_173_vertices[43] = {
#include "stg23/terrain/draw_173_vertices.inc"
};
static NJS_CNK_MODEL draw_173_model = {
#include "stg23/terrain/draw_173_model.inc"
};
static NJS_CNK_OBJECT draw_173 = {
#include "stg23/terrain/draw_173.inc"
};
static Sint16 draw_37_polygons[87] = {
#include "stg23/terrain/draw_37_polygons.inc"
};
static Sint32 draw_37_vertices[75] = {
#include "stg23/terrain/draw_37_vertices.inc"
};
static NJS_CNK_MODEL draw_37_model = {
#include "stg23/terrain/draw_37_model.inc"
};
static NJS_CNK_OBJECT draw_169 = {
#include "stg23/terrain/draw_169.inc"
};
static Sint16 draw_157_polygons[63] = {
#include "stg23/terrain/draw_157_polygons.inc"
};
static Sint32 draw_157_vertices[51] = {
#include "stg23/terrain/draw_157_vertices.inc"
};
static NJS_CNK_MODEL draw_157_model = {
#include "stg23/terrain/draw_157_model.inc"
};
static NJS_CNK_OBJECT draw_167 = {
#include "stg23/terrain/draw_167.inc"
};
static Sint16 draw_156_polygons[551] = {
#include "stg23/terrain/draw_156_polygons.inc"
};
static Sint32 draw_156_vertices[503] = {
#include "stg23/terrain/draw_156_vertices.inc"
};
static NJS_CNK_MODEL draw_156_model = {
#include "stg23/terrain/draw_156_model.inc"
};
static NJS_CNK_OBJECT draw_156 = {
#include "stg23/terrain/draw_156.inc"
};
static Sint16 draw_155_polygons[745] = {
#include "stg23/terrain/draw_155_polygons.inc"
};
static Sint32 draw_155_vertices[687] = {
#include "stg23/terrain/draw_155_vertices.inc"
};
static NJS_CNK_MODEL draw_155_model = {
#include "stg23/terrain/draw_155_model.inc"
};
static NJS_CNK_OBJECT draw_155 = {
#include "stg23/terrain/draw_155.inc"
};
static Sint16 draw_154_polygons[337] = {
#include "stg23/terrain/draw_154_polygons.inc"
};
static Sint32 draw_154_vertices[403] = {
#include "stg23/terrain/draw_154_vertices.inc"
};
static NJS_CNK_MODEL draw_154_model = {
#include "stg23/terrain/draw_154_model.inc"
};
static NJS_CNK_OBJECT draw_154 = {
#include "stg23/terrain/draw_154.inc"
};
static Sint16 draw_153_polygons[113] = {
#include "stg23/terrain/draw_153_polygons.inc"
};
static Sint32 draw_153_vertices[99] = {
#include "stg23/terrain/draw_153_vertices.inc"
};
static NJS_CNK_MODEL draw_153_model = {
#include "stg23/terrain/draw_153_model.inc"
};
static NJS_CNK_OBJECT draw_153 = {
#include "stg23/terrain/draw_153.inc"
};
static Sint16 draw_152_polygons[1629] = {
#include "stg23/terrain/draw_152_polygons.inc"
};
static Sint32 draw_152_vertices[1555] = {
#include "stg23/terrain/draw_152_vertices.inc"
};
static NJS_CNK_MODEL draw_152_model = {
#include "stg23/terrain/draw_152_model.inc"
};
static NJS_CNK_OBJECT draw_152 = {
#include "stg23/terrain/draw_152.inc"
};
static Sint16 draw_150_polygons[1611] = {
#include "stg23/terrain/draw_150_polygons.inc"
};
static Sint32 draw_150_vertices[1351] = {
#include "stg23/terrain/draw_150_vertices.inc"
};
static NJS_CNK_MODEL draw_150_model = {
#include "stg23/terrain/draw_150_model.inc"
};
static NJS_CNK_OBJECT draw_151 = {
#include "stg23/terrain/draw_151.inc"
};
static Sint16 draw_147_polygons[883] = {
#include "stg23/terrain/draw_147_polygons.inc"
};
static Sint32 draw_147_vertices[831] = {
#include "stg23/terrain/draw_147_vertices.inc"
};
static NJS_CNK_MODEL draw_147_model = {
#include "stg23/terrain/draw_147_model.inc"
};
static NJS_CNK_OBJECT draw_149 = {
#include "stg23/terrain/draw_149.inc"
};
static Sint16 draw_146_polygons[139] = {
#include "stg23/terrain/draw_146_polygons.inc"
};
static Sint32 draw_146_vertices[99] = {
#include "stg23/terrain/draw_146_vertices.inc"
};
static NJS_CNK_MODEL draw_146_model = {
#include "stg23/terrain/draw_146_model.inc"
};
static NJS_CNK_OBJECT draw_146 = {
#include "stg23/terrain/draw_146.inc"
};
static Sint16 draw_145_polygons[43] = {
#include "stg23/terrain/draw_145_polygons.inc"
};
static Sint32 draw_145_vertices[35] = {
#include "stg23/terrain/draw_145_vertices.inc"
};
static NJS_CNK_MODEL draw_145_model = {
#include "stg23/terrain/draw_145_model.inc"
};
static NJS_CNK_OBJECT draw_145 = {
#include "stg23/terrain/draw_145.inc"
};
static Sint16 draw_143_polygons[697] = {
#include "stg23/terrain/draw_143_polygons.inc"
};
static Sint32 draw_143_vertices[651] = {
#include "stg23/terrain/draw_143_vertices.inc"
};
static NJS_CNK_MODEL draw_143_model = {
#include "stg23/terrain/draw_143_model.inc"
};
static NJS_CNK_OBJECT draw_143 = {
#include "stg23/terrain/draw_143.inc"
};
static Sint16 draw_142_polygons[299] = {
#include "stg23/terrain/draw_142_polygons.inc"
};
static Sint32 draw_142_vertices[219] = {
#include "stg23/terrain/draw_142_vertices.inc"
};
static NJS_CNK_MODEL draw_142_model = {
#include "stg23/terrain/draw_142_model.inc"
};
static NJS_CNK_OBJECT draw_142 = {
#include "stg23/terrain/draw_142.inc"
};
static Sint16 draw_139_polygons[367] = {
#include "stg23/terrain/draw_139_polygons.inc"
};
static Sint32 draw_139_vertices[275] = {
#include "stg23/terrain/draw_139_vertices.inc"
};
static NJS_CNK_MODEL draw_139_model = {
#include "stg23/terrain/draw_139_model.inc"
};
static NJS_CNK_OBJECT draw_140 = {
#include "stg23/terrain/draw_140.inc"
};
static Sint16 draw_137_polygons[93] = {
#include "stg23/terrain/draw_137_polygons.inc"
};
static Sint32 draw_137_vertices[59] = {
#include "stg23/terrain/draw_137_vertices.inc"
};
static NJS_CNK_MODEL draw_137_model = {
#include "stg23/terrain/draw_137_model.inc"
};
static NJS_CNK_OBJECT draw_138 = {
#include "stg23/terrain/draw_138.inc"
};
static Sint16 draw_136_polygons[101] = {
#include "stg23/terrain/draw_136_polygons.inc"
};
static Sint32 draw_136_vertices[79] = {
#include "stg23/terrain/draw_136_vertices.inc"
};
static NJS_CNK_MODEL draw_136_model = {
#include "stg23/terrain/draw_136_model.inc"
};
static NJS_CNK_OBJECT draw_136 = {
#include "stg23/terrain/draw_136.inc"
};
static Sint16 draw_135_polygons[63] = {
#include "stg23/terrain/draw_135_polygons.inc"
};
static Sint32 draw_135_vertices[51] = {
#include "stg23/terrain/draw_135_vertices.inc"
};
static NJS_CNK_MODEL draw_135_model = {
#include "stg23/terrain/draw_135_model.inc"
};
static NJS_CNK_OBJECT draw_135 = {
#include "stg23/terrain/draw_135.inc"
};
static Sint16 draw_133_polygons[63] = {
#include "stg23/terrain/draw_133_polygons.inc"
};
static Sint32 draw_133_vertices[51] = {
#include "stg23/terrain/draw_133_vertices.inc"
};
static NJS_CNK_MODEL draw_133_model = {
#include "stg23/terrain/draw_133_model.inc"
};
static NJS_CNK_OBJECT draw_134 = {
#include "stg23/terrain/draw_134.inc"
};
static Sint16 draw_132_polygons[451] = {
#include "stg23/terrain/draw_132_polygons.inc"
};
static Sint32 draw_132_vertices[351] = {
#include "stg23/terrain/draw_132_vertices.inc"
};
static NJS_CNK_MODEL draw_132_model = {
#include "stg23/terrain/draw_132_model.inc"
};
static NJS_CNK_OBJECT draw_132 = {
#include "stg23/terrain/draw_132.inc"
};
static Sint16 draw_131_polygons[677] = {
#include "stg23/terrain/draw_131_polygons.inc"
};
static Sint32 draw_131_vertices[603] = {
#include "stg23/terrain/draw_131_vertices.inc"
};
static NJS_CNK_MODEL draw_131_model = {
#include "stg23/terrain/draw_131_model.inc"
};
static NJS_CNK_OBJECT draw_131 = {
#include "stg23/terrain/draw_131.inc"
};
static Sint16 draw_130_polygons[603] = {
#include "stg23/terrain/draw_130_polygons.inc"
};
static Sint32 draw_130_vertices[531] = {
#include "stg23/terrain/draw_130_vertices.inc"
};
static NJS_CNK_MODEL draw_130_model = {
#include "stg23/terrain/draw_130_model.inc"
};
static NJS_CNK_OBJECT draw_130 = {
#include "stg23/terrain/draw_130.inc"
};
static Sint16 draw_129_polygons[779] = {
#include "stg23/terrain/draw_129_polygons.inc"
};
static Sint32 draw_129_vertices[567] = {
#include "stg23/terrain/draw_129_vertices.inc"
};
static NJS_CNK_MODEL draw_129_model = {
#include "stg23/terrain/draw_129_model.inc"
};
static NJS_CNK_OBJECT draw_129 = {
#include "stg23/terrain/draw_129.inc"
};
static Sint16 draw_128_polygons[115] = {
#include "stg23/terrain/draw_128_polygons.inc"
};
static Sint32 draw_128_vertices[107] = {
#include "stg23/terrain/draw_128_vertices.inc"
};
static NJS_CNK_MODEL draw_128_model = {
#include "stg23/terrain/draw_128_model.inc"
};
static NJS_CNK_OBJECT draw_128 = {
#include "stg23/terrain/draw_128.inc"
};
static Sint16 draw_127_polygons[81] = {
#include "stg23/terrain/draw_127_polygons.inc"
};
static Sint32 draw_127_vertices[55] = {
#include "stg23/terrain/draw_127_vertices.inc"
};
static NJS_CNK_MODEL draw_127_model = {
#include "stg23/terrain/draw_127_model.inc"
};
static NJS_CNK_OBJECT draw_127 = {
#include "stg23/terrain/draw_127.inc"
};
static Sint16 draw_126_polygons[39] = {
#include "stg23/terrain/draw_126_polygons.inc"
};
static Sint32 draw_126_vertices[35] = {
#include "stg23/terrain/draw_126_vertices.inc"
};
static NJS_CNK_MODEL draw_126_model = {
#include "stg23/terrain/draw_126_model.inc"
};
static NJS_CNK_OBJECT draw_126 = {
#include "stg23/terrain/draw_126.inc"
};
static Sint16 draw_125_polygons[247] = {
#include "stg23/terrain/draw_125_polygons.inc"
};
static Sint32 draw_125_vertices[199] = {
#include "stg23/terrain/draw_125_vertices.inc"
};
static NJS_CNK_MODEL draw_125_model = {
#include "stg23/terrain/draw_125_model.inc"
};
static NJS_CNK_OBJECT draw_125 = {
#include "stg23/terrain/draw_125.inc"
};
static Sint16 draw_123_polygons[93] = {
#include "stg23/terrain/draw_123_polygons.inc"
};
static Sint32 draw_123_vertices[91] = {
#include "stg23/terrain/draw_123_vertices.inc"
};
static NJS_CNK_MODEL draw_123_model = {
#include "stg23/terrain/draw_123_model.inc"
};
static NJS_CNK_OBJECT draw_124 = {
#include "stg23/terrain/draw_124.inc"
};
static Sint16 draw_122_polygons[39] = {
#include "stg23/terrain/draw_122_polygons.inc"
};
static Sint32 draw_122_vertices[35] = {
#include "stg23/terrain/draw_122_vertices.inc"
};
static NJS_CNK_MODEL draw_122_model = {
#include "stg23/terrain/draw_122_model.inc"
};
static NJS_CNK_OBJECT draw_122 = {
#include "stg23/terrain/draw_122.inc"
};
static Sint16 draw_121_polygons[505] = {
#include "stg23/terrain/draw_121_polygons.inc"
};
static Sint32 draw_121_vertices[407] = {
#include "stg23/terrain/draw_121_vertices.inc"
};
static NJS_CNK_MODEL draw_121_model = {
#include "stg23/terrain/draw_121_model.inc"
};
static NJS_CNK_OBJECT draw_121 = {
#include "stg23/terrain/draw_121.inc"
};
static Sint16 draw_120_polygons[193] = {
#include "stg23/terrain/draw_120_polygons.inc"
};
static Sint32 draw_120_vertices[103] = {
#include "stg23/terrain/draw_120_vertices.inc"
};
static NJS_CNK_MODEL draw_120_model = {
#include "stg23/terrain/draw_120_model.inc"
};
static NJS_CNK_OBJECT draw_120 = {
#include "stg23/terrain/draw_120.inc"
};
static Sint16 draw_119_polygons[141] = {
#include "stg23/terrain/draw_119_polygons.inc"
};
static Sint32 draw_119_vertices[111] = {
#include "stg23/terrain/draw_119_vertices.inc"
};
static NJS_CNK_MODEL draw_119_model = {
#include "stg23/terrain/draw_119_model.inc"
};
static NJS_CNK_OBJECT draw_119 = {
#include "stg23/terrain/draw_119.inc"
};
static Sint16 draw_118_polygons[179] = {
#include "stg23/terrain/draw_118_polygons.inc"
};
static Sint32 draw_118_vertices[151] = {
#include "stg23/terrain/draw_118_vertices.inc"
};
static NJS_CNK_MODEL draw_118_model = {
#include "stg23/terrain/draw_118_model.inc"
};
static NJS_CNK_OBJECT draw_118 = {
#include "stg23/terrain/draw_118.inc"
};
static Sint16 draw_117_polygons[281] = {
#include "stg23/terrain/draw_117_polygons.inc"
};
static Sint32 draw_117_vertices[215] = {
#include "stg23/terrain/draw_117_vertices.inc"
};
static NJS_CNK_MODEL draw_117_model = {
#include "stg23/terrain/draw_117_model.inc"
};
static NJS_CNK_OBJECT draw_117 = {
#include "stg23/terrain/draw_117.inc"
};
static Sint16 draw_116_polygons[263] = {
#include "stg23/terrain/draw_116_polygons.inc"
};
static Sint32 draw_116_vertices[175] = {
#include "stg23/terrain/draw_116_vertices.inc"
};
static NJS_CNK_MODEL draw_116_model = {
#include "stg23/terrain/draw_116_model.inc"
};
static NJS_CNK_OBJECT draw_116 = {
#include "stg23/terrain/draw_116.inc"
};
static Sint16 draw_115_polygons[381] = {
#include "stg23/terrain/draw_115_polygons.inc"
};
static Sint32 draw_115_vertices[267] = {
#include "stg23/terrain/draw_115_vertices.inc"
};
static NJS_CNK_MODEL draw_115_model = {
#include "stg23/terrain/draw_115_model.inc"
};
static NJS_CNK_OBJECT draw_115 = {
#include "stg23/terrain/draw_115.inc"
};
static Sint16 draw_113_polygons[857] = {
#include "stg23/terrain/draw_113_polygons.inc"
};
static Sint32 draw_113_vertices[799] = {
#include "stg23/terrain/draw_113_vertices.inc"
};
static NJS_CNK_MODEL draw_113_model = {
#include "stg23/terrain/draw_113_model.inc"
};
static NJS_CNK_OBJECT draw_114 = {
#include "stg23/terrain/draw_114.inc"
};
static Sint16 draw_112_polygons[113] = {
#include "stg23/terrain/draw_112_polygons.inc"
};
static Sint32 draw_112_vertices[91] = {
#include "stg23/terrain/draw_112_vertices.inc"
};
static NJS_CNK_MODEL draw_112_model = {
#include "stg23/terrain/draw_112_model.inc"
};
static NJS_CNK_OBJECT draw_112 = {
#include "stg23/terrain/draw_112.inc"
};
static Sint16 draw_111_polygons[141] = {
#include "stg23/terrain/draw_111_polygons.inc"
};
static Sint32 draw_111_vertices[111] = {
#include "stg23/terrain/draw_111_vertices.inc"
};
static NJS_CNK_MODEL draw_111_model = {
#include "stg23/terrain/draw_111_model.inc"
};
static NJS_CNK_OBJECT draw_111 = {
#include "stg23/terrain/draw_111.inc"
};
static Sint16 draw_110_polygons[257] = {
#include "stg23/terrain/draw_110_polygons.inc"
};
static Sint32 draw_110_vertices[175] = {
#include "stg23/terrain/draw_110_vertices.inc"
};
static NJS_CNK_MODEL draw_110_model = {
#include "stg23/terrain/draw_110_model.inc"
};
static NJS_CNK_OBJECT draw_110 = {
#include "stg23/terrain/draw_110.inc"
};
static Sint16 draw_109_polygons[187] = {
#include "stg23/terrain/draw_109_polygons.inc"
};
static Sint32 draw_109_vertices[147] = {
#include "stg23/terrain/draw_109_vertices.inc"
};
static NJS_CNK_MODEL draw_109_model = {
#include "stg23/terrain/draw_109_model.inc"
};
static NJS_CNK_OBJECT draw_109 = {
#include "stg23/terrain/draw_109.inc"
};
static Sint16 draw_108_polygons[141] = {
#include "stg23/terrain/draw_108_polygons.inc"
};
static Sint32 draw_108_vertices[111] = {
#include "stg23/terrain/draw_108_vertices.inc"
};
static NJS_CNK_MODEL draw_108_model = {
#include "stg23/terrain/draw_108_model.inc"
};
static NJS_CNK_OBJECT draw_108 = {
#include "stg23/terrain/draw_108.inc"
};
static Sint16 draw_107_polygons[141] = {
#include "stg23/terrain/draw_107_polygons.inc"
};
static Sint32 draw_107_vertices[111] = {
#include "stg23/terrain/draw_107_vertices.inc"
};
static NJS_CNK_MODEL draw_107_model = {
#include "stg23/terrain/draw_107_model.inc"
};
static NJS_CNK_OBJECT draw_107 = {
#include "stg23/terrain/draw_107.inc"
};
static Sint16 draw_106_polygons[141] = {
#include "stg23/terrain/draw_106_polygons.inc"
};
static Sint32 draw_106_vertices[111] = {
#include "stg23/terrain/draw_106_vertices.inc"
};
static NJS_CNK_MODEL draw_106_model = {
#include "stg23/terrain/draw_106_model.inc"
};
static NJS_CNK_OBJECT draw_106 = {
#include "stg23/terrain/draw_106.inc"
};
static Sint16 draw_105_polygons[141] = {
#include "stg23/terrain/draw_105_polygons.inc"
};
static Sint32 draw_105_vertices[111] = {
#include "stg23/terrain/draw_105_vertices.inc"
};
static NJS_CNK_MODEL draw_105_model = {
#include "stg23/terrain/draw_105_model.inc"
};
static NJS_CNK_OBJECT draw_105 = {
#include "stg23/terrain/draw_105.inc"
};
static Sint16 draw_104_polygons[141] = {
#include "stg23/terrain/draw_104_polygons.inc"
};
static Sint32 draw_104_vertices[111] = {
#include "stg23/terrain/draw_104_vertices.inc"
};
static NJS_CNK_MODEL draw_104_model = {
#include "stg23/terrain/draw_104_model.inc"
};
static NJS_CNK_OBJECT draw_104 = {
#include "stg23/terrain/draw_104.inc"
};
static Sint16 draw_103_polygons[141] = {
#include "stg23/terrain/draw_103_polygons.inc"
};
static Sint32 draw_103_vertices[107] = {
#include "stg23/terrain/draw_103_vertices.inc"
};
static NJS_CNK_MODEL draw_103_model = {
#include "stg23/terrain/draw_103_model.inc"
};
static NJS_CNK_OBJECT draw_103 = {
#include "stg23/terrain/draw_103.inc"
};
static Sint16 draw_102_polygons[465] = {
#include "stg23/terrain/draw_102_polygons.inc"
};
static Sint32 draw_102_vertices[395] = {
#include "stg23/terrain/draw_102_vertices.inc"
};
static NJS_CNK_MODEL draw_102_model = {
#include "stg23/terrain/draw_102_model.inc"
};
static NJS_CNK_OBJECT draw_102 = {
#include "stg23/terrain/draw_102.inc"
};
static Sint16 draw_100_polygons[51] = {
#include "stg23/terrain/draw_100_polygons.inc"
};
static Sint32 draw_100_vertices[35] = {
#include "stg23/terrain/draw_100_vertices.inc"
};
static NJS_CNK_MODEL draw_100_model = {
#include "stg23/terrain/draw_100_model.inc"
};
static NJS_CNK_OBJECT draw_101 = {
#include "stg23/terrain/draw_101.inc"
};
static Sint16 draw_99_polygons[51] = {
#include "stg23/terrain/draw_99_polygons.inc"
};
static Sint32 draw_99_vertices[35] = {
#include "stg23/terrain/draw_99_vertices.inc"
};
static NJS_CNK_MODEL draw_99_model = {
#include "stg23/terrain/draw_99_model.inc"
};
static NJS_CNK_OBJECT draw_99 = {
#include "stg23/terrain/draw_99.inc"
};
static Sint16 draw_92_polygons[51] = {
#include "stg23/terrain/draw_92_polygons.inc"
};
static Sint32 draw_92_vertices[35] = {
#include "stg23/terrain/draw_92_vertices.inc"
};
static NJS_CNK_MODEL draw_92_model = {
#include "stg23/terrain/draw_92_model.inc"
};
static NJS_CNK_OBJECT draw_98 = {
#include "stg23/terrain/draw_98.inc"
};
static Sint16 draw_78_polygons[785] = {
#include "stg23/terrain/draw_78_polygons.inc"
};
static Sint32 draw_78_vertices[471] = {
#include "stg23/terrain/draw_78_vertices.inc"
};
static NJS_CNK_MODEL draw_78_model = {
#include "stg23/terrain/draw_78_model.inc"
};
static NJS_CNK_OBJECT draw_80 = {
#include "stg23/terrain/draw_80.inc"
};
static Sint16 draw_76_polygons[447] = {
#include "stg23/terrain/draw_76_polygons.inc"
};
static Sint32 draw_76_vertices[459] = {
#include "stg23/terrain/draw_76_vertices.inc"
};
static NJS_CNK_MODEL draw_76_model = {
#include "stg23/terrain/draw_76_model.inc"
};
static NJS_CNK_OBJECT draw_77 = {
#include "stg23/terrain/draw_77.inc"
};
static Sint16 draw_75_polygons[237] = {
#include "stg23/terrain/draw_75_polygons.inc"
};
static Sint32 draw_75_vertices[143] = {
#include "stg23/terrain/draw_75_vertices.inc"
};
static NJS_CNK_MODEL draw_75_model = {
#include "stg23/terrain/draw_75_model.inc"
};
static NJS_CNK_OBJECT draw_75 = {
#include "stg23/terrain/draw_75.inc"
};
static Sint16 draw_74_polygons[81] = {
#include "stg23/terrain/draw_74_polygons.inc"
};
static Sint32 draw_74_vertices[79] = {
#include "stg23/terrain/draw_74_vertices.inc"
};
static NJS_CNK_MODEL draw_74_model = {
#include "stg23/terrain/draw_74_model.inc"
};
static NJS_CNK_OBJECT draw_74 = {
#include "stg23/terrain/draw_74.inc"
};
static Sint16 draw_73_polygons[203] = {
#include "stg23/terrain/draw_73_polygons.inc"
};
static Sint32 draw_73_vertices[211] = {
#include "stg23/terrain/draw_73_vertices.inc"
};
static NJS_CNK_MODEL draw_73_model = {
#include "stg23/terrain/draw_73_model.inc"
};
static NJS_CNK_OBJECT draw_73 = {
#include "stg23/terrain/draw_73.inc"
};
static Sint16 draw_70_polygons[1261] = {
#include "stg23/terrain/draw_70_polygons.inc"
};
static Sint32 draw_70_vertices[1003] = {
#include "stg23/terrain/draw_70_vertices.inc"
};
static NJS_CNK_MODEL draw_70_model = {
#include "stg23/terrain/draw_70_model.inc"
};
static NJS_CNK_OBJECT draw_72 = {
#include "stg23/terrain/draw_72.inc"
};
static Sint16 draw_69_polygons[731] = {
#include "stg23/terrain/draw_69_polygons.inc"
};
static Sint32 draw_69_vertices[551] = {
#include "stg23/terrain/draw_69_vertices.inc"
};
static NJS_CNK_MODEL draw_69_model = {
#include "stg23/terrain/draw_69_model.inc"
};
static NJS_CNK_OBJECT draw_69 = {
#include "stg23/terrain/draw_69.inc"
};
static Sint16 draw_68_polygons[161] = {
#include "stg23/terrain/draw_68_polygons.inc"
};
static Sint32 draw_68_vertices[95] = {
#include "stg23/terrain/draw_68_vertices.inc"
};
static NJS_CNK_MODEL draw_68_model = {
#include "stg23/terrain/draw_68_model.inc"
};
static NJS_CNK_OBJECT draw_68 = {
#include "stg23/terrain/draw_68.inc"
};
static Sint16 draw_67_polygons[359] = {
#include "stg23/terrain/draw_67_polygons.inc"
};
static Sint32 draw_67_vertices[271] = {
#include "stg23/terrain/draw_67_vertices.inc"
};
static NJS_CNK_MODEL draw_67_model = {
#include "stg23/terrain/draw_67_model.inc"
};
static NJS_CNK_OBJECT draw_67 = {
#include "stg23/terrain/draw_67.inc"
};
static Sint16 draw_66_polygons[175] = {
#include "stg23/terrain/draw_66_polygons.inc"
};
static Sint32 draw_66_vertices[131] = {
#include "stg23/terrain/draw_66_vertices.inc"
};
static NJS_CNK_MODEL draw_66_model = {
#include "stg23/terrain/draw_66_model.inc"
};
static NJS_CNK_OBJECT draw_66 = {
#include "stg23/terrain/draw_66.inc"
};
static Sint16 draw_65_polygons[269] = {
#include "stg23/terrain/draw_65_polygons.inc"
};
static Sint32 draw_65_vertices[223] = {
#include "stg23/terrain/draw_65_vertices.inc"
};
static NJS_CNK_MODEL draw_65_model = {
#include "stg23/terrain/draw_65_model.inc"
};
static NJS_CNK_OBJECT draw_65 = {
#include "stg23/terrain/draw_65.inc"
};
static Sint16 draw_39_polygons[101] = {
#include "stg23/terrain/draw_39_polygons.inc"
};
static Sint32 draw_39_vertices[83] = {
#include "stg23/terrain/draw_39_vertices.inc"
};
static NJS_CNK_MODEL draw_39_model = {
#include "stg23/terrain/draw_39_model.inc"
};
static NJS_CNK_OBJECT draw_50 = {
#include "stg23/terrain/draw_50.inc"
};
static Sint16 draw_36_polygons[89] = {
#include "stg23/terrain/draw_36_polygons.inc"
};
static Sint32 draw_36_vertices[67] = {
#include "stg23/terrain/draw_36_vertices.inc"
};
static NJS_CNK_MODEL draw_36_model = {
#include "stg23/terrain/draw_36_model.inc"
};
static NJS_CNK_OBJECT draw_36 = {
#include "stg23/terrain/draw_36.inc"
};
static Sint16 draw_35_polygons[201] = {
#include "stg23/terrain/draw_35_polygons.inc"
};
static Sint32 draw_35_vertices[99] = {
#include "stg23/terrain/draw_35_vertices.inc"
};
static NJS_CNK_MODEL draw_35_model = {
#include "stg23/terrain/draw_35_model.inc"
};
static NJS_CNK_OBJECT draw_35 = {
#include "stg23/terrain/draw_35.inc"
};
static Sint16 draw_34_polygons[463] = {
#include "stg23/terrain/draw_34_polygons.inc"
};
static Sint32 draw_34_vertices[211] = {
#include "stg23/terrain/draw_34_vertices.inc"
};
static NJS_CNK_MODEL draw_34_model = {
#include "stg23/terrain/draw_34_model.inc"
};
static NJS_CNK_OBJECT draw_34 = {
#include "stg23/terrain/draw_34.inc"
};
static Sint16 draw_32_polygons[83] = {
#include "stg23/terrain/draw_32_polygons.inc"
};
static Sint32 draw_32_vertices[79] = {
#include "stg23/terrain/draw_32_vertices.inc"
};
static NJS_CNK_MODEL draw_32_model = {
#include "stg23/terrain/draw_32_model.inc"
};
static NJS_CNK_OBJECT draw_33 = {
#include "stg23/terrain/draw_33.inc"
};
static Sint16 draw_30_polygons[141] = {
#include "stg23/terrain/draw_30_polygons.inc"
};
static Sint32 draw_30_vertices[99] = {
#include "stg23/terrain/draw_30_vertices.inc"
};
static NJS_CNK_MODEL draw_30_model = {
#include "stg23/terrain/draw_30_model.inc"
};
static NJS_CNK_OBJECT draw_31 = {
#include "stg23/terrain/draw_31.inc"
};
static Sint16 draw_29_polygons[853] = {
#include "stg23/terrain/draw_29_polygons.inc"
};
static Sint32 draw_29_vertices[775] = {
#include "stg23/terrain/draw_29_vertices.inc"
};
static NJS_CNK_MODEL draw_29_model = {
#include "stg23/terrain/draw_29_model.inc"
};
static NJS_CNK_OBJECT draw_29 = {
#include "stg23/terrain/draw_29.inc"
};
static Sint16 draw_28_polygons[115] = {
#include "stg23/terrain/draw_28_polygons.inc"
};
static Sint32 draw_28_vertices[115] = {
#include "stg23/terrain/draw_28_vertices.inc"
};
static NJS_CNK_MODEL draw_28_model = {
#include "stg23/terrain/draw_28_model.inc"
};
static NJS_CNK_OBJECT draw_28 = {
#include "stg23/terrain/draw_28.inc"
};
static Sint16 draw_26_polygons[185] = {
#include "stg23/terrain/draw_26_polygons.inc"
};
static Sint32 draw_26_vertices[179] = {
#include "stg23/terrain/draw_26_vertices.inc"
};
static NJS_CNK_MODEL draw_26_model = {
#include "stg23/terrain/draw_26_model.inc"
};
static NJS_CNK_OBJECT draw_27 = {
#include "stg23/terrain/draw_27.inc"
};
static Sint16 draw_25_polygons[113] = {
#include "stg23/terrain/draw_25_polygons.inc"
};
static Sint32 draw_25_vertices[99] = {
#include "stg23/terrain/draw_25_vertices.inc"
};
static NJS_CNK_MODEL draw_25_model = {
#include "stg23/terrain/draw_25_model.inc"
};
static NJS_CNK_OBJECT draw_25 = {
#include "stg23/terrain/draw_25.inc"
};
static Sint16 draw_24_polygons[101] = {
#include "stg23/terrain/draw_24_polygons.inc"
};
static Sint32 draw_24_vertices[83] = {
#include "stg23/terrain/draw_24_vertices.inc"
};
static NJS_CNK_MODEL draw_24_model = {
#include "stg23/terrain/draw_24_model.inc"
};
static NJS_CNK_OBJECT draw_24 = {
#include "stg23/terrain/draw_24.inc"
};
static Sint16 draw_23_polygons[83] = {
#include "stg23/terrain/draw_23_polygons.inc"
};
static Sint32 draw_23_vertices[83] = {
#include "stg23/terrain/draw_23_vertices.inc"
};
static NJS_CNK_MODEL draw_23_model = {
#include "stg23/terrain/draw_23_model.inc"
};
static NJS_CNK_OBJECT draw_23 = {
#include "stg23/terrain/draw_23.inc"
};
static Sint16 draw_22_polygons[141] = {
#include "stg23/terrain/draw_22_polygons.inc"
};
static Sint32 draw_22_vertices[111] = {
#include "stg23/terrain/draw_22_vertices.inc"
};
static NJS_CNK_MODEL draw_22_model = {
#include "stg23/terrain/draw_22_model.inc"
};
static NJS_CNK_OBJECT draw_22 = {
#include "stg23/terrain/draw_22.inc"
};
static Sint16 draw_21_polygons[141] = {
#include "stg23/terrain/draw_21_polygons.inc"
};
static Sint32 draw_21_vertices[111] = {
#include "stg23/terrain/draw_21_vertices.inc"
};
static NJS_CNK_MODEL draw_21_model = {
#include "stg23/terrain/draw_21_model.inc"
};
static NJS_CNK_OBJECT draw_21 = {
#include "stg23/terrain/draw_21.inc"
};
static Sint16 draw_20_polygons[141] = {
#include "stg23/terrain/draw_20_polygons.inc"
};
static Sint32 draw_20_vertices[111] = {
#include "stg23/terrain/draw_20_vertices.inc"
};
static NJS_CNK_MODEL draw_20_model = {
#include "stg23/terrain/draw_20_model.inc"
};
static NJS_CNK_OBJECT draw_20 = {
#include "stg23/terrain/draw_20.inc"
};
static Sint16 draw_18_polygons[141] = {
#include "stg23/terrain/draw_18_polygons.inc"
};
static Sint32 draw_18_vertices[115] = {
#include "stg23/terrain/draw_18_vertices.inc"
};
static NJS_CNK_MODEL draw_18_model = {
#include "stg23/terrain/draw_18_model.inc"
};
static NJS_CNK_OBJECT draw_19 = {
#include "stg23/terrain/draw_19.inc"
};
static Sint16 draw_17_polygons[155] = {
#include "stg23/terrain/draw_17_polygons.inc"
};
static Sint32 draw_17_vertices[79] = {
#include "stg23/terrain/draw_17_vertices.inc"
};
static NJS_CNK_MODEL draw_17_model = {
#include "stg23/terrain/draw_17_model.inc"
};
static NJS_CNK_OBJECT draw_17 = {
#include "stg23/terrain/draw_17.inc"
};
static Sint16 draw_8_polygons[449] = {
#include "stg23/terrain/draw_8_polygons.inc"
};
static Sint32 draw_8_vertices[443] = {
#include "stg23/terrain/draw_8_vertices.inc"
};
static NJS_CNK_MODEL draw_8_model = {
#include "stg23/terrain/draw_8_model.inc"
};
static NJS_CNK_OBJECT draw_16 = {
#include "stg23/terrain/draw_16.inc"
};
static Sint16 draw_0_polygons[139] = {
#include "stg23/terrain/draw_0_polygons.inc"
};
static Sint32 draw_0_vertices[87] = {
#include "stg23/terrain/draw_0_vertices.inc"
};
static NJS_CNK_MODEL draw_0_model = {
#include "stg23/terrain/draw_0_model.inc"
};
static NJS_CNK_OBJECT draw_7 = {
#include "stg23/terrain/draw_7.inc"
};
static NJS_MATERIAL collision_0_materials[1] = {
#include "stg23/terrain/collision_0_materials.inc"
};
static Sint16 collision_0_faces[59] = {
#include "stg23/terrain/collision_0_faces.inc"
};
static NJS_MESHSET collision_0_meshsets[1] = {
#include "stg23/terrain/collision_0_meshsets.inc"
};
static NJS_POINT3 collision_0_points[28] = {
#include "stg23/terrain/collision_0_points.inc"
};
static NJS_MODEL collision_0_model = {
#include "stg23/terrain/collision_0_model.inc"
};
static NJS_OBJECT collision_0 = {
#include "stg23/terrain/collision_0.inc"
};
static NJS_MATERIAL collision_2_materials[1] = {
#include "stg23/terrain/collision_2_materials.inc"
};
static Sint16 collision_2_faces[15] = {
#include "stg23/terrain/collision_2_faces.inc"
};
static NJS_MESHSET collision_2_meshsets[1] = {
#include "stg23/terrain/collision_2_meshsets.inc"
};
static NJS_POINT3 collision_2_points[12] = {
#include "stg23/terrain/collision_2_points.inc"
};
static NJS_MODEL collision_2_model = {
#include "stg23/terrain/collision_2_model.inc"
};
static NJS_OBJECT collision_2 = {
#include "stg23/terrain/collision_2.inc"
};
static NJS_MATERIAL collision_1_materials[1] = {
#include "stg23/terrain/collision_1_materials.inc"
};
static Sint16 collision_1_faces[27] = {
#include "stg23/terrain/collision_1_faces.inc"
};
static NJS_MESHSET collision_1_meshsets[1] = {
#include "stg23/terrain/collision_1_meshsets.inc"
};
static NJS_POINT3 collision_1_points[12] = {
#include "stg23/terrain/collision_1_points.inc"
};
static NJS_MODEL collision_1_model = {
#include "stg23/terrain/collision_1_model.inc"
};
static NJS_OBJECT collision_4 = {
#include "stg23/terrain/collision_4.inc"
};
static NJS_MATERIAL collision_5_materials[1] = {
#include "stg23/terrain/collision_5_materials.inc"
};
static Sint16 collision_5_faces[5] = {
#include "stg23/terrain/collision_5_faces.inc"
};
static NJS_MESHSET collision_5_meshsets[1] = {
#include "stg23/terrain/collision_5_meshsets.inc"
};
static NJS_POINT3 collision_5_points[4] = {
#include "stg23/terrain/collision_5_points.inc"
};
static NJS_MODEL collision_5_model = {
#include "stg23/terrain/collision_5_model.inc"
};
static NJS_OBJECT collision_5 = {
#include "stg23/terrain/collision_5.inc"
};
static NJS_MATERIAL collision_6_materials[1] = {
#include "stg23/terrain/collision_6_materials.inc"
};
static Sint16 collision_6_faces[11] = {
#include "stg23/terrain/collision_6_faces.inc"
};
static NJS_MESHSET collision_6_meshsets[1] = {
#include "stg23/terrain/collision_6_meshsets.inc"
};
static NJS_POINT3 collision_6_points[8] = {
#include "stg23/terrain/collision_6_points.inc"
};
static NJS_MODEL collision_6_model = {
#include "stg23/terrain/collision_6_model.inc"
};
static NJS_OBJECT collision_6 = {
#include "stg23/terrain/collision_6.inc"
};
static NJS_MATERIAL collision_7_materials[1] = {
#include "stg23/terrain/collision_7_materials.inc"
};
static Sint16 collision_7_faces[42] = {
#include "stg23/terrain/collision_7_faces.inc"
};
static NJS_MESHSET collision_7_meshsets[1] = {
#include "stg23/terrain/collision_7_meshsets.inc"
};
static NJS_POINT3 collision_7_points[19] = {
#include "stg23/terrain/collision_7_points.inc"
};
static NJS_MODEL collision_7_model = {
#include "stg23/terrain/collision_7_model.inc"
};
static NJS_OBJECT collision_7 = {
#include "stg23/terrain/collision_7.inc"
};
static NJS_MATERIAL collision_8_materials[1] = {
#include "stg23/terrain/collision_8_materials.inc"
};
static Sint16 collision_8_faces[71] = {
#include "stg23/terrain/collision_8_faces.inc"
};
static NJS_MESHSET collision_8_meshsets[1] = {
#include "stg23/terrain/collision_8_meshsets.inc"
};
static NJS_POINT3 collision_8_points[30] = {
#include "stg23/terrain/collision_8_points.inc"
};
static NJS_MODEL collision_8_model = {
#include "stg23/terrain/collision_8_model.inc"
};
static NJS_OBJECT collision_8 = {
#include "stg23/terrain/collision_8.inc"
};
static NJS_MATERIAL collision_9_materials[1] = {
#include "stg23/terrain/collision_9_materials.inc"
};
static Sint16 collision_9_faces[97] = {
#include "stg23/terrain/collision_9_faces.inc"
};
static NJS_MESHSET collision_9_meshsets[1] = {
#include "stg23/terrain/collision_9_meshsets.inc"
};
static NJS_POINT3 collision_9_points[44] = {
#include "stg23/terrain/collision_9_points.inc"
};
static NJS_MODEL collision_9_model = {
#include "stg23/terrain/collision_9_model.inc"
};
static NJS_OBJECT collision_9 = {
#include "stg23/terrain/collision_9.inc"
};
static NJS_MATERIAL collision_10_materials[1] = {
#include "stg23/terrain/collision_10_materials.inc"
};
static Sint16 collision_10_faces[30] = {
#include "stg23/terrain/collision_10_faces.inc"
};
static NJS_MESHSET collision_10_meshsets[1] = {
#include "stg23/terrain/collision_10_meshsets.inc"
};
static NJS_POINT3 collision_10_points[16] = {
#include "stg23/terrain/collision_10_points.inc"
};
static NJS_MODEL collision_10_model = {
#include "stg23/terrain/collision_10_model.inc"
};
static NJS_OBJECT collision_10 = {
#include "stg23/terrain/collision_10.inc"
};
static NJS_MATERIAL collision_11_materials[1] = {
#include "stg23/terrain/collision_11_materials.inc"
};
static Sint16 collision_11_faces[35] = {
#include "stg23/terrain/collision_11_faces.inc"
};
static NJS_MESHSET collision_11_meshsets[1] = {
#include "stg23/terrain/collision_11_meshsets.inc"
};
static NJS_POINT3 collision_11_points[19] = {
#include "stg23/terrain/collision_11_points.inc"
};
static NJS_MODEL collision_11_model = {
#include "stg23/terrain/collision_11_model.inc"
};
static NJS_OBJECT collision_11 = {
#include "stg23/terrain/collision_11.inc"
};
static NJS_MATERIAL collision_12_materials[1] = {
#include "stg23/terrain/collision_12_materials.inc"
};
static Sint16 collision_12_faces[18] = {
#include "stg23/terrain/collision_12_faces.inc"
};
static NJS_MESHSET collision_12_meshsets[1] = {
#include "stg23/terrain/collision_12_meshsets.inc"
};
static NJS_POINT3 collision_12_points[16] = {
#include "stg23/terrain/collision_12_points.inc"
};
static NJS_MODEL collision_12_model = {
#include "stg23/terrain/collision_12_model.inc"
};
static NJS_OBJECT collision_12 = {
#include "stg23/terrain/collision_12.inc"
};
static NJS_MATERIAL collision_13_materials[1] = {
#include "stg23/terrain/collision_13_materials.inc"
};
static Sint16 collision_13_faces[50] = {
#include "stg23/terrain/collision_13_faces.inc"
};
static NJS_MESHSET collision_13_meshsets[1] = {
#include "stg23/terrain/collision_13_meshsets.inc"
};
static NJS_POINT3 collision_13_points[27] = {
#include "stg23/terrain/collision_13_points.inc"
};
static NJS_MODEL collision_13_model = {
#include "stg23/terrain/collision_13_model.inc"
};
static NJS_OBJECT collision_13 = {
#include "stg23/terrain/collision_13.inc"
};
static NJS_MATERIAL collision_14_materials[1] = {
#include "stg23/terrain/collision_14_materials.inc"
};
static Sint16 collision_14_faces[7] = {
#include "stg23/terrain/collision_14_faces.inc"
};
static NJS_MESHSET collision_14_meshsets[1] = {
#include "stg23/terrain/collision_14_meshsets.inc"
};
static NJS_POINT3 collision_14_points[6] = {
#include "stg23/terrain/collision_14_points.inc"
};
static NJS_MODEL collision_14_model = {
#include "stg23/terrain/collision_14_model.inc"
};
static NJS_OBJECT collision_14 = {
#include "stg23/terrain/collision_14.inc"
};
static NJS_MATERIAL collision_15_materials[1] = {
#include "stg23/terrain/collision_15_materials.inc"
};
static Sint16 collision_15_faces[18] = {
#include "stg23/terrain/collision_15_faces.inc"
};
static NJS_MESHSET collision_15_meshsets[1] = {
#include "stg23/terrain/collision_15_meshsets.inc"
};
static NJS_POINT3 collision_15_points[12] = {
#include "stg23/terrain/collision_15_points.inc"
};
static NJS_MODEL collision_15_model = {
#include "stg23/terrain/collision_15_model.inc"
};
static NJS_OBJECT collision_15 = {
#include "stg23/terrain/collision_15.inc"
};
static NJS_MATERIAL collision_16_materials[1] = {
#include "stg23/terrain/collision_16_materials.inc"
};
static Sint16 collision_16_faces[18] = {
#include "stg23/terrain/collision_16_faces.inc"
};
static NJS_MESHSET collision_16_meshsets[1] = {
#include "stg23/terrain/collision_16_meshsets.inc"
};
static NJS_POINT3 collision_16_points[9] = {
#include "stg23/terrain/collision_16_points.inc"
};
static NJS_MODEL collision_16_model = {
#include "stg23/terrain/collision_16_model.inc"
};
static NJS_OBJECT collision_16 = {
#include "stg23/terrain/collision_16.inc"
};
static NJS_MATERIAL collision_17_materials[1] = {
#include "stg23/terrain/collision_17_materials.inc"
};
static Sint16 collision_17_faces[19] = {
#include "stg23/terrain/collision_17_faces.inc"
};
static NJS_MESHSET collision_17_meshsets[1] = {
#include "stg23/terrain/collision_17_meshsets.inc"
};
static NJS_POINT3 collision_17_points[8] = {
#include "stg23/terrain/collision_17_points.inc"
};
static NJS_MODEL collision_17_model = {
#include "stg23/terrain/collision_17_model.inc"
};
static NJS_OBJECT collision_17 = {
#include "stg23/terrain/collision_17.inc"
};
static NJS_MATERIAL collision_18_materials[1] = {
#include "stg23/terrain/collision_18_materials.inc"
};
static Sint16 collision_18_faces[15] = {
#include "stg23/terrain/collision_18_faces.inc"
};
static NJS_MESHSET collision_18_meshsets[1] = {
#include "stg23/terrain/collision_18_meshsets.inc"
};
static NJS_POINT3 collision_18_points[9] = {
#include "stg23/terrain/collision_18_points.inc"
};
static NJS_MODEL collision_18_model = {
#include "stg23/terrain/collision_18_model.inc"
};
static NJS_OBJECT collision_18 = {
#include "stg23/terrain/collision_18.inc"
};
static NJS_MATERIAL collision_19_materials[1] = {
#include "stg23/terrain/collision_19_materials.inc"
};
static Sint16 collision_19_faces[19] = {
#include "stg23/terrain/collision_19_faces.inc"
};
static NJS_MESHSET collision_19_meshsets[1] = {
#include "stg23/terrain/collision_19_meshsets.inc"
};
static NJS_POINT3 collision_19_points[8] = {
#include "stg23/terrain/collision_19_points.inc"
};
static NJS_MODEL collision_19_model = {
#include "stg23/terrain/collision_19_model.inc"
};
static NJS_OBJECT collision_19 = {
#include "stg23/terrain/collision_19.inc"
};
static NJS_MATERIAL collision_20_materials[1] = {
#include "stg23/terrain/collision_20_materials.inc"
};
static Sint16 collision_20_faces[19] = {
#include "stg23/terrain/collision_20_faces.inc"
};
static NJS_MESHSET collision_20_meshsets[1] = {
#include "stg23/terrain/collision_20_meshsets.inc"
};
static NJS_POINT3 collision_20_points[8] = {
#include "stg23/terrain/collision_20_points.inc"
};
static NJS_MODEL collision_20_model = {
#include "stg23/terrain/collision_20_model.inc"
};
static NJS_OBJECT collision_20 = {
#include "stg23/terrain/collision_20.inc"
};
static NJS_MATERIAL collision_21_materials[1] = {
#include "stg23/terrain/collision_21_materials.inc"
};
static Sint16 collision_21_faces[24] = {
#include "stg23/terrain/collision_21_faces.inc"
};
static NJS_MESHSET collision_21_meshsets[1] = {
#include "stg23/terrain/collision_21_meshsets.inc"
};
static NJS_POINT3 collision_21_points[14] = {
#include "stg23/terrain/collision_21_points.inc"
};
static NJS_MODEL collision_21_model = {
#include "stg23/terrain/collision_21_model.inc"
};
static NJS_OBJECT collision_21 = {
#include "stg23/terrain/collision_21.inc"
};
static NJS_MATERIAL collision_22_materials[1] = {
#include "stg23/terrain/collision_22_materials.inc"
};
static Sint16 collision_22_faces[19] = {
#include "stg23/terrain/collision_22_faces.inc"
};
static NJS_MESHSET collision_22_meshsets[1] = {
#include "stg23/terrain/collision_22_meshsets.inc"
};
static NJS_POINT3 collision_22_points[11] = {
#include "stg23/terrain/collision_22_points.inc"
};
static NJS_MODEL collision_22_model = {
#include "stg23/terrain/collision_22_model.inc"
};
static NJS_OBJECT collision_22 = {
#include "stg23/terrain/collision_22.inc"
};
static NJS_MATERIAL collision_23_materials[1] = {
#include "stg23/terrain/collision_23_materials.inc"
};
static Sint16 collision_23_faces[21] = {
#include "stg23/terrain/collision_23_faces.inc"
};
static NJS_MESHSET collision_23_meshsets[1] = {
#include "stg23/terrain/collision_23_meshsets.inc"
};
static NJS_POINT3 collision_23_points[12] = {
#include "stg23/terrain/collision_23_points.inc"
};
static NJS_MODEL collision_23_model = {
#include "stg23/terrain/collision_23_model.inc"
};
static NJS_OBJECT collision_23 = {
#include "stg23/terrain/collision_23.inc"
};
static NJS_MATERIAL collision_24_materials[1] = {
#include "stg23/terrain/collision_24_materials.inc"
};
static Sint16 collision_24_faces[50] = {
#include "stg23/terrain/collision_24_faces.inc"
};
static NJS_MESHSET collision_24_meshsets[1] = {
#include "stg23/terrain/collision_24_meshsets.inc"
};
static NJS_POINT3 collision_24_points[23] = {
#include "stg23/terrain/collision_24_points.inc"
};
static NJS_MODEL collision_24_model = {
#include "stg23/terrain/collision_24_model.inc"
};
static NJS_OBJECT collision_24 = {
#include "stg23/terrain/collision_24.inc"
};
static NJS_MATERIAL collision_25_materials[1] = {
#include "stg23/terrain/collision_25_materials.inc"
};
static Sint16 collision_25_faces[14] = {
#include "stg23/terrain/collision_25_faces.inc"
};
static NJS_MESHSET collision_25_meshsets[1] = {
#include "stg23/terrain/collision_25_meshsets.inc"
};
static NJS_POINT3 collision_25_points[8] = {
#include "stg23/terrain/collision_25_points.inc"
};
static NJS_MODEL collision_25_model = {
#include "stg23/terrain/collision_25_model.inc"
};
static NJS_OBJECT collision_25 = {
#include "stg23/terrain/collision_25.inc"
};
static NJS_MATERIAL collision_26_materials[1] = {
#include "stg23/terrain/collision_26_materials.inc"
};
static Sint16 collision_26_faces[16] = {
#include "stg23/terrain/collision_26_faces.inc"
};
static NJS_MESHSET collision_26_meshsets[1] = {
#include "stg23/terrain/collision_26_meshsets.inc"
};
static NJS_POINT3 collision_26_points[8] = {
#include "stg23/terrain/collision_26_points.inc"
};
static NJS_MODEL collision_26_model = {
#include "stg23/terrain/collision_26_model.inc"
};
static NJS_OBJECT collision_26 = {
#include "stg23/terrain/collision_26.inc"
};
static NJS_MATERIAL collision_27_materials[1] = {
#include "stg23/terrain/collision_27_materials.inc"
};
static Sint16 collision_27_faces[30] = {
#include "stg23/terrain/collision_27_faces.inc"
};
static NJS_MESHSET collision_27_meshsets[1] = {
#include "stg23/terrain/collision_27_meshsets.inc"
};
static NJS_POINT3 collision_27_points[12] = {
#include "stg23/terrain/collision_27_points.inc"
};
static NJS_MODEL collision_27_model = {
#include "stg23/terrain/collision_27_model.inc"
};
static NJS_OBJECT collision_27 = {
#include "stg23/terrain/collision_27.inc"
};
static NJS_MATERIAL collision_28_materials[1] = {
#include "stg23/terrain/collision_28_materials.inc"
};
static Sint16 collision_28_faces[25] = {
#include "stg23/terrain/collision_28_faces.inc"
};
static NJS_MESHSET collision_28_meshsets[1] = {
#include "stg23/terrain/collision_28_meshsets.inc"
};
static NJS_POINT3 collision_28_points[12] = {
#include "stg23/terrain/collision_28_points.inc"
};
static NJS_MODEL collision_28_model = {
#include "stg23/terrain/collision_28_model.inc"
};
static NJS_OBJECT collision_28 = {
#include "stg23/terrain/collision_28.inc"
};
static NJS_MATERIAL collision_29_materials[1] = {
#include "stg23/terrain/collision_29_materials.inc"
};
static Sint16 collision_29_faces[17] = {
#include "stg23/terrain/collision_29_faces.inc"
};
static NJS_MESHSET collision_29_meshsets[1] = {
#include "stg23/terrain/collision_29_meshsets.inc"
};
static NJS_POINT3 collision_29_points[8] = {
#include "stg23/terrain/collision_29_points.inc"
};
static NJS_MODEL collision_29_model = {
#include "stg23/terrain/collision_29_model.inc"
};
static NJS_OBJECT collision_29 = {
#include "stg23/terrain/collision_29.inc"
};
static NJS_MATERIAL collision_34_materials[1] = {
#include "stg23/terrain/collision_34_materials.inc"
};
static Sint16 collision_34_faces[30] = {
#include "stg23/terrain/collision_34_faces.inc"
};
static NJS_MESHSET collision_34_meshsets[1] = {
#include "stg23/terrain/collision_34_meshsets.inc"
};
static NJS_POINT3 collision_34_points[16] = {
#include "stg23/terrain/collision_34_points.inc"
};
static NJS_MODEL collision_34_model = {
#include "stg23/terrain/collision_34_model.inc"
};
static NJS_OBJECT collision_34 = {
#include "stg23/terrain/collision_34.inc"
};
static NJS_MATERIAL collision_44_materials[1] = {
#include "stg23/terrain/collision_44_materials.inc"
};
static Sint16 collision_44_faces[201] = {
#include "stg23/terrain/collision_44_faces.inc"
};
static NJS_MESHSET collision_44_meshsets[1] = {
#include "stg23/terrain/collision_44_meshsets.inc"
};
static NJS_POINT3 collision_44_points[77] = {
#include "stg23/terrain/collision_44_points.inc"
};
static NJS_MODEL collision_44_model = {
#include "stg23/terrain/collision_44_model.inc"
};
static NJS_OBJECT collision_44 = {
#include "stg23/terrain/collision_44.inc"
};
static NJS_MATERIAL collision_49_materials[1] = {
#include "stg23/terrain/collision_49_materials.inc"
};
static Sint16 collision_49_faces[100] = {
#include "stg23/terrain/collision_49_faces.inc"
};
static NJS_MESHSET collision_49_meshsets[1] = {
#include "stg23/terrain/collision_49_meshsets.inc"
};
static NJS_POINT3 collision_49_points[40] = {
#include "stg23/terrain/collision_49_points.inc"
};
static NJS_MODEL collision_49_model = {
#include "stg23/terrain/collision_49_model.inc"
};
static NJS_OBJECT collision_49 = {
#include "stg23/terrain/collision_49.inc"
};
static NJS_MATERIAL collision_54_materials[1] = {
#include "stg23/terrain/collision_54_materials.inc"
};
static Sint16 collision_54_faces[21] = {
#include "stg23/terrain/collision_54_faces.inc"
};
static NJS_MESHSET collision_54_meshsets[1] = {
#include "stg23/terrain/collision_54_meshsets.inc"
};
static NJS_POINT3 collision_54_points[14] = {
#include "stg23/terrain/collision_54_points.inc"
};
static NJS_MODEL collision_54_model = {
#include "stg23/terrain/collision_54_model.inc"
};
static NJS_OBJECT collision_54 = {
#include "stg23/terrain/collision_54.inc"
};
static NJS_MATERIAL collision_55_materials[1] = {
#include "stg23/terrain/collision_55_materials.inc"
};
static Sint16 collision_55_faces[63] = {
#include "stg23/terrain/collision_55_faces.inc"
};
static NJS_MESHSET collision_55_meshsets[1] = {
#include "stg23/terrain/collision_55_meshsets.inc"
};
static NJS_POINT3 collision_55_points[41] = {
#include "stg23/terrain/collision_55_points.inc"
};
static NJS_MODEL collision_55_model = {
#include "stg23/terrain/collision_55_model.inc"
};
static NJS_OBJECT collision_55 = {
#include "stg23/terrain/collision_55.inc"
};
static NJS_MATERIAL collision_56_materials[1] = {
#include "stg23/terrain/collision_56_materials.inc"
};
static Sint16 collision_56_faces[17] = {
#include "stg23/terrain/collision_56_faces.inc"
};
static NJS_MESHSET collision_56_meshsets[1] = {
#include "stg23/terrain/collision_56_meshsets.inc"
};
static NJS_POINT3 collision_56_points[8] = {
#include "stg23/terrain/collision_56_points.inc"
};
static NJS_MODEL collision_56_model = {
#include "stg23/terrain/collision_56_model.inc"
};
static NJS_OBJECT collision_56 = {
#include "stg23/terrain/collision_56.inc"
};
static NJS_MATERIAL collision_57_materials[1] = {
#include "stg23/terrain/collision_57_materials.inc"
};
static Sint16 collision_57_faces[11] = {
#include "stg23/terrain/collision_57_faces.inc"
};
static NJS_MESHSET collision_57_meshsets[1] = {
#include "stg23/terrain/collision_57_meshsets.inc"
};
static NJS_POINT3 collision_57_points[8] = {
#include "stg23/terrain/collision_57_points.inc"
};
static NJS_MODEL collision_57_model = {
#include "stg23/terrain/collision_57_model.inc"
};
static NJS_OBJECT collision_57 = {
#include "stg23/terrain/collision_57.inc"
};
static NJS_MATERIAL collision_63_materials[1] = {
#include "stg23/terrain/collision_63_materials.inc"
};
static Sint16 collision_63_faces[18] = {
#include "stg23/terrain/collision_63_faces.inc"
};
static NJS_MESHSET collision_63_meshsets[1] = {
#include "stg23/terrain/collision_63_meshsets.inc"
};
static NJS_POINT3 collision_63_points[12] = {
#include "stg23/terrain/collision_63_points.inc"
};
static NJS_MODEL collision_63_model = {
#include "stg23/terrain/collision_63_model.inc"
};
static NJS_OBJECT collision_63 = {
#include "stg23/terrain/collision_63.inc"
};
static NJS_MATERIAL collision_64_materials[1] = {
#include "stg23/terrain/collision_64_materials.inc"
};
static Sint16 collision_64_faces[11] = {
#include "stg23/terrain/collision_64_faces.inc"
};
static NJS_MESHSET collision_64_meshsets[1] = {
#include "stg23/terrain/collision_64_meshsets.inc"
};
static NJS_POINT3 collision_64_points[8] = {
#include "stg23/terrain/collision_64_points.inc"
};
static NJS_MODEL collision_64_model = {
#include "stg23/terrain/collision_64_model.inc"
};
static NJS_OBJECT collision_64 = {
#include "stg23/terrain/collision_64.inc"
};
static NJS_MATERIAL collision_67_materials[1] = {
#include "stg23/terrain/collision_67_materials.inc"
};
static Sint16 collision_67_faces[11] = {
#include "stg23/terrain/collision_67_faces.inc"
};
static NJS_MESHSET collision_67_meshsets[1] = {
#include "stg23/terrain/collision_67_meshsets.inc"
};
static NJS_POINT3 collision_67_points[8] = {
#include "stg23/terrain/collision_67_points.inc"
};
static NJS_MODEL collision_67_model = {
#include "stg23/terrain/collision_67_model.inc"
};
static NJS_OBJECT collision_67 = {
#include "stg23/terrain/collision_67.inc"
};
static NJS_MATERIAL collision_69_materials[1] = {
#include "stg23/terrain/collision_69_materials.inc"
};
static Sint16 collision_69_faces[133] = {
#include "stg23/terrain/collision_69_faces.inc"
};
static NJS_MESHSET collision_69_meshsets[1] = {
#include "stg23/terrain/collision_69_meshsets.inc"
};
static NJS_POINT3 collision_69_points[62] = {
#include "stg23/terrain/collision_69_points.inc"
};
static NJS_MODEL collision_69_model = {
#include "stg23/terrain/collision_69_model.inc"
};
static NJS_OBJECT collision_69 = {
#include "stg23/terrain/collision_69.inc"
};
static NJS_MATERIAL collision_70_materials[1] = {
#include "stg23/terrain/collision_70_materials.inc"
};
static Sint16 collision_70_faces[16] = {
#include "stg23/terrain/collision_70_faces.inc"
};
static NJS_MESHSET collision_70_meshsets[1] = {
#include "stg23/terrain/collision_70_meshsets.inc"
};
static NJS_POINT3 collision_70_points[8] = {
#include "stg23/terrain/collision_70_points.inc"
};
static NJS_MODEL collision_70_model = {
#include "stg23/terrain/collision_70_model.inc"
};
static NJS_OBJECT collision_70 = {
#include "stg23/terrain/collision_70.inc"
};
static NJS_MATERIAL collision_71_materials[1] = {
#include "stg23/terrain/collision_71_materials.inc"
};
static Sint16 collision_71_faces[16] = {
#include "stg23/terrain/collision_71_faces.inc"
};
static NJS_MESHSET collision_71_meshsets[1] = {
#include "stg23/terrain/collision_71_meshsets.inc"
};
static NJS_POINT3 collision_71_points[8] = {
#include "stg23/terrain/collision_71_points.inc"
};
static NJS_MODEL collision_71_model = {
#include "stg23/terrain/collision_71_model.inc"
};
static NJS_OBJECT collision_71 = {
#include "stg23/terrain/collision_71.inc"
};
static NJS_MATERIAL collision_72_materials[1] = {
#include "stg23/terrain/collision_72_materials.inc"
};
static Sint16 collision_72_faces[16] = {
#include "stg23/terrain/collision_72_faces.inc"
};
static NJS_MESHSET collision_72_meshsets[1] = {
#include "stg23/terrain/collision_72_meshsets.inc"
};
static NJS_POINT3 collision_72_points[8] = {
#include "stg23/terrain/collision_72_points.inc"
};
static NJS_MODEL collision_72_model = {
#include "stg23/terrain/collision_72_model.inc"
};
static NJS_OBJECT collision_72 = {
#include "stg23/terrain/collision_72.inc"
};
static NJS_MATERIAL collision_73_materials[1] = {
#include "stg23/terrain/collision_73_materials.inc"
};
static Sint16 collision_73_faces[16] = {
#include "stg23/terrain/collision_73_faces.inc"
};
static NJS_MESHSET collision_73_meshsets[1] = {
#include "stg23/terrain/collision_73_meshsets.inc"
};
static NJS_POINT3 collision_73_points[8] = {
#include "stg23/terrain/collision_73_points.inc"
};
static NJS_MODEL collision_73_model = {
#include "stg23/terrain/collision_73_model.inc"
};
static NJS_OBJECT collision_73 = {
#include "stg23/terrain/collision_73.inc"
};
static NJS_MATERIAL collision_74_materials[1] = {
#include "stg23/terrain/collision_74_materials.inc"
};
static Sint16 collision_74_faces[12] = {
#include "stg23/terrain/collision_74_faces.inc"
};
static NJS_MESHSET collision_74_meshsets[1] = {
#include "stg23/terrain/collision_74_meshsets.inc"
};
static NJS_POINT3 collision_74_points[8] = {
#include "stg23/terrain/collision_74_points.inc"
};
static NJS_MODEL collision_74_model = {
#include "stg23/terrain/collision_74_model.inc"
};
static NJS_OBJECT collision_74 = {
#include "stg23/terrain/collision_74.inc"
};
static NJS_MATERIAL collision_75_materials[1] = {
#include "stg23/terrain/collision_75_materials.inc"
};
static Sint16 collision_75_faces[15] = {
#include "stg23/terrain/collision_75_faces.inc"
};
static NJS_MESHSET collision_75_meshsets[1] = {
#include "stg23/terrain/collision_75_meshsets.inc"
};
static NJS_POINT3 collision_75_points[8] = {
#include "stg23/terrain/collision_75_points.inc"
};
static NJS_MODEL collision_75_model = {
#include "stg23/terrain/collision_75_model.inc"
};
static NJS_OBJECT collision_75 = {
#include "stg23/terrain/collision_75.inc"
};
static NJS_MATERIAL collision_76_materials[1] = {
#include "stg23/terrain/collision_76_materials.inc"
};
static Sint16 collision_76_faces[32] = {
#include "stg23/terrain/collision_76_faces.inc"
};
static NJS_MESHSET collision_76_meshsets[1] = {
#include "stg23/terrain/collision_76_meshsets.inc"
};
static NJS_POINT3 collision_76_points[12] = {
#include "stg23/terrain/collision_76_points.inc"
};
static NJS_MODEL collision_76_model = {
#include "stg23/terrain/collision_76_model.inc"
};
static NJS_OBJECT collision_76 = {
#include "stg23/terrain/collision_76.inc"
};
static NJS_MATERIAL collision_77_materials[1] = {
#include "stg23/terrain/collision_77_materials.inc"
};
static Sint16 collision_77_faces[31] = {
#include "stg23/terrain/collision_77_faces.inc"
};
static NJS_MESHSET collision_77_meshsets[1] = {
#include "stg23/terrain/collision_77_meshsets.inc"
};
static NJS_POINT3 collision_77_points[12] = {
#include "stg23/terrain/collision_77_points.inc"
};
static NJS_MODEL collision_77_model = {
#include "stg23/terrain/collision_77_model.inc"
};
static NJS_OBJECT collision_77 = {
#include "stg23/terrain/collision_77.inc"
};
static NJS_MATERIAL collision_88_materials[1] = {
#include "stg23/terrain/collision_88_materials.inc"
};
static Sint16 collision_88_faces[14] = {
#include "stg23/terrain/collision_88_faces.inc"
};
static NJS_MESHSET collision_88_meshsets[1] = {
#include "stg23/terrain/collision_88_meshsets.inc"
};
static NJS_POINT3 collision_88_points[8] = {
#include "stg23/terrain/collision_88_points.inc"
};
static NJS_MODEL collision_88_model = {
#include "stg23/terrain/collision_88_model.inc"
};
static NJS_OBJECT collision_88 = {
#include "stg23/terrain/collision_88.inc"
};
static NJS_MATERIAL collision_96_materials[1] = {
#include "stg23/terrain/collision_96_materials.inc"
};
static Sint16 collision_96_faces[109] = {
#include "stg23/terrain/collision_96_faces.inc"
};
static NJS_MESHSET collision_96_meshsets[1] = {
#include "stg23/terrain/collision_96_meshsets.inc"
};
static NJS_POINT3 collision_96_points[52] = {
#include "stg23/terrain/collision_96_points.inc"
};
static NJS_MODEL collision_96_model = {
#include "stg23/terrain/collision_96_model.inc"
};
static NJS_OBJECT collision_96 = {
#include "stg23/terrain/collision_96.inc"
};
static NJS_MATERIAL collision_97_materials[1] = {
#include "stg23/terrain/collision_97_materials.inc"
};
static Sint16 collision_97_faces[26] = {
#include "stg23/terrain/collision_97_faces.inc"
};
static NJS_MESHSET collision_97_meshsets[1] = {
#include "stg23/terrain/collision_97_meshsets.inc"
};
static NJS_POINT3 collision_97_points[12] = {
#include "stg23/terrain/collision_97_points.inc"
};
static NJS_MODEL collision_97_model = {
#include "stg23/terrain/collision_97_model.inc"
};
static NJS_OBJECT collision_97 = {
#include "stg23/terrain/collision_97.inc"
};
static NJS_MATERIAL collision_98_materials[1] = {
#include "stg23/terrain/collision_98_materials.inc"
};
static Sint16 collision_98_faces[41] = {
#include "stg23/terrain/collision_98_faces.inc"
};
static NJS_MESHSET collision_98_meshsets[1] = {
#include "stg23/terrain/collision_98_meshsets.inc"
};
static NJS_POINT3 collision_98_points[16] = {
#include "stg23/terrain/collision_98_points.inc"
};
static NJS_MODEL collision_98_model = {
#include "stg23/terrain/collision_98_model.inc"
};
static NJS_OBJECT collision_98 = {
#include "stg23/terrain/collision_98.inc"
};
static NJS_MATERIAL collision_100_materials[1] = {
#include "stg23/terrain/collision_100_materials.inc"
};
static Sint16 collision_100_faces[31] = {
#include "stg23/terrain/collision_100_faces.inc"
};
static NJS_MESHSET collision_100_meshsets[1] = {
#include "stg23/terrain/collision_100_meshsets.inc"
};
static NJS_POINT3 collision_100_points[12] = {
#include "stg23/terrain/collision_100_points.inc"
};
static NJS_MODEL collision_100_model = {
#include "stg23/terrain/collision_100_model.inc"
};
static NJS_OBJECT collision_100 = {
#include "stg23/terrain/collision_100.inc"
};
static NJS_MATERIAL collision_102_materials[1] = {
#include "stg23/terrain/collision_102_materials.inc"
};
static Sint16 collision_102_faces[44] = {
#include "stg23/terrain/collision_102_faces.inc"
};
static NJS_MESHSET collision_102_meshsets[1] = {
#include "stg23/terrain/collision_102_meshsets.inc"
};
static NJS_POINT3 collision_102_points[32] = {
#include "stg23/terrain/collision_102_points.inc"
};
static NJS_MODEL collision_102_model = {
#include "stg23/terrain/collision_102_model.inc"
};
static NJS_OBJECT collision_102 = {
#include "stg23/terrain/collision_102.inc"
};
static NJS_MATERIAL collision_103_materials[1] = {
#include "stg23/terrain/collision_103_materials.inc"
};
static Sint16 collision_103_faces[87] = {
#include "stg23/terrain/collision_103_faces.inc"
};
static NJS_MESHSET collision_103_meshsets[1] = {
#include "stg23/terrain/collision_103_meshsets.inc"
};
static NJS_POINT3 collision_103_points[34] = {
#include "stg23/terrain/collision_103_points.inc"
};
static NJS_MODEL collision_103_model = {
#include "stg23/terrain/collision_103_model.inc"
};
static NJS_OBJECT collision_103 = {
#include "stg23/terrain/collision_103.inc"
};
static NJS_MATERIAL collision_106_materials[1] = {
#include "stg23/terrain/collision_106_materials.inc"
};
static Sint16 collision_106_faces[107] = {
#include "stg23/terrain/collision_106_faces.inc"
};
static NJS_MESHSET collision_106_meshsets[1] = {
#include "stg23/terrain/collision_106_meshsets.inc"
};
static NJS_POINT3 collision_106_points[40] = {
#include "stg23/terrain/collision_106_points.inc"
};
static NJS_MODEL collision_106_model = {
#include "stg23/terrain/collision_106_model.inc"
};
static NJS_OBJECT collision_106 = {
#include "stg23/terrain/collision_106.inc"
};
static NJS_MATERIAL collision_107_materials[1] = {
#include "stg23/terrain/collision_107_materials.inc"
};
static Sint16 collision_107_faces[31] = {
#include "stg23/terrain/collision_107_faces.inc"
};
static NJS_MESHSET collision_107_meshsets[1] = {
#include "stg23/terrain/collision_107_meshsets.inc"
};
static NJS_POINT3 collision_107_points[12] = {
#include "stg23/terrain/collision_107_points.inc"
};
static NJS_MODEL collision_107_model = {
#include "stg23/terrain/collision_107_model.inc"
};
static NJS_OBJECT collision_107 = {
#include "stg23/terrain/collision_107.inc"
};
static NJS_MATERIAL collision_108_materials[1] = {
#include "stg23/terrain/collision_108_materials.inc"
};
static Sint16 collision_108_faces[39] = {
#include "stg23/terrain/collision_108_faces.inc"
};
static NJS_MESHSET collision_108_meshsets[1] = {
#include "stg23/terrain/collision_108_meshsets.inc"
};
static NJS_POINT3 collision_108_points[16] = {
#include "stg23/terrain/collision_108_points.inc"
};
static NJS_MODEL collision_108_model = {
#include "stg23/terrain/collision_108_model.inc"
};
static NJS_OBJECT collision_108 = {
#include "stg23/terrain/collision_108.inc"
};
static NJS_MATERIAL collision_109_materials[1] = {
#include "stg23/terrain/collision_109_materials.inc"
};
static Sint16 collision_109_faces[16] = {
#include "stg23/terrain/collision_109_faces.inc"
};
static NJS_MESHSET collision_109_meshsets[1] = {
#include "stg23/terrain/collision_109_meshsets.inc"
};
static NJS_POINT3 collision_109_points[8] = {
#include "stg23/terrain/collision_109_points.inc"
};
static NJS_MODEL collision_109_model = {
#include "stg23/terrain/collision_109_model.inc"
};
static NJS_OBJECT collision_109 = {
#include "stg23/terrain/collision_109.inc"
};
static NJS_MATERIAL collision_110_materials[1] = {
#include "stg23/terrain/collision_110_materials.inc"
};
static Sint16 collision_110_faces[27] = {
#include "stg23/terrain/collision_110_faces.inc"
};
static NJS_MESHSET collision_110_meshsets[1] = {
#include "stg23/terrain/collision_110_meshsets.inc"
};
static NJS_POINT3 collision_110_points[12] = {
#include "stg23/terrain/collision_110_points.inc"
};
static NJS_MODEL collision_110_model = {
#include "stg23/terrain/collision_110_model.inc"
};
static NJS_OBJECT collision_110 = {
#include "stg23/terrain/collision_110.inc"
};
static NJS_MATERIAL collision_111_materials[1] = {
#include "stg23/terrain/collision_111_materials.inc"
};
static Sint16 collision_111_faces[16] = {
#include "stg23/terrain/collision_111_faces.inc"
};
static NJS_MESHSET collision_111_meshsets[1] = {
#include "stg23/terrain/collision_111_meshsets.inc"
};
static NJS_POINT3 collision_111_points[8] = {
#include "stg23/terrain/collision_111_points.inc"
};
static NJS_MODEL collision_111_model = {
#include "stg23/terrain/collision_111_model.inc"
};
static NJS_OBJECT collision_111 = {
#include "stg23/terrain/collision_111.inc"
};
static NJS_MATERIAL collision_112_materials[1] = {
#include "stg23/terrain/collision_112_materials.inc"
};
static Sint16 collision_112_faces[16] = {
#include "stg23/terrain/collision_112_faces.inc"
};
static NJS_MESHSET collision_112_meshsets[1] = {
#include "stg23/terrain/collision_112_meshsets.inc"
};
static NJS_POINT3 collision_112_points[8] = {
#include "stg23/terrain/collision_112_points.inc"
};
static NJS_MODEL collision_112_model = {
#include "stg23/terrain/collision_112_model.inc"
};
static NJS_OBJECT collision_112 = {
#include "stg23/terrain/collision_112.inc"
};
static NJS_MATERIAL collision_113_materials[1] = {
#include "stg23/terrain/collision_113_materials.inc"
};
static Sint16 collision_113_faces[16] = {
#include "stg23/terrain/collision_113_faces.inc"
};
static NJS_MESHSET collision_113_meshsets[1] = {
#include "stg23/terrain/collision_113_meshsets.inc"
};
static NJS_POINT3 collision_113_points[8] = {
#include "stg23/terrain/collision_113_points.inc"
};
static NJS_MODEL collision_113_model = {
#include "stg23/terrain/collision_113_model.inc"
};
static NJS_OBJECT collision_113 = {
#include "stg23/terrain/collision_113.inc"
};
static NJS_MATERIAL collision_116_materials[1] = {
#include "stg23/terrain/collision_116_materials.inc"
};
static Sint16 collision_116_faces[16] = {
#include "stg23/terrain/collision_116_faces.inc"
};
static NJS_MESHSET collision_116_meshsets[1] = {
#include "stg23/terrain/collision_116_meshsets.inc"
};
static NJS_POINT3 collision_116_points[8] = {
#include "stg23/terrain/collision_116_points.inc"
};
static NJS_MODEL collision_116_model = {
#include "stg23/terrain/collision_116_model.inc"
};
static NJS_OBJECT collision_116 = {
#include "stg23/terrain/collision_116.inc"
};
static NJS_MATERIAL collision_117_materials[1] = {
#include "stg23/terrain/collision_117_materials.inc"
};
static Sint16 collision_117_faces[31] = {
#include "stg23/terrain/collision_117_faces.inc"
};
static NJS_MESHSET collision_117_meshsets[1] = {
#include "stg23/terrain/collision_117_meshsets.inc"
};
static NJS_POINT3 collision_117_points[15] = {
#include "stg23/terrain/collision_117_points.inc"
};
static NJS_MODEL collision_117_model = {
#include "stg23/terrain/collision_117_model.inc"
};
static NJS_OBJECT collision_117 = {
#include "stg23/terrain/collision_117.inc"
};
static NJS_MATERIAL collision_118_materials[1] = {
#include "stg23/terrain/collision_118_materials.inc"
};
static Sint16 collision_118_faces[67] = {
#include "stg23/terrain/collision_118_faces.inc"
};
static NJS_MESHSET collision_118_meshsets[1] = {
#include "stg23/terrain/collision_118_meshsets.inc"
};
static NJS_POINT3 collision_118_points[27] = {
#include "stg23/terrain/collision_118_points.inc"
};
static NJS_MODEL collision_118_model = {
#include "stg23/terrain/collision_118_model.inc"
};
static NJS_OBJECT collision_118 = {
#include "stg23/terrain/collision_118.inc"
};
static NJS_MATERIAL collision_119_materials[1] = {
#include "stg23/terrain/collision_119_materials.inc"
};
static Sint16 collision_119_faces[91] = {
#include "stg23/terrain/collision_119_faces.inc"
};
static NJS_MESHSET collision_119_meshsets[1] = {
#include "stg23/terrain/collision_119_meshsets.inc"
};
static NJS_POINT3 collision_119_points[31] = {
#include "stg23/terrain/collision_119_points.inc"
};
static NJS_MODEL collision_119_model = {
#include "stg23/terrain/collision_119_model.inc"
};
static NJS_OBJECT collision_119 = {
#include "stg23/terrain/collision_119.inc"
};
static NJS_MATERIAL collision_120_materials[1] = {
#include "stg23/terrain/collision_120_materials.inc"
};
static Sint16 collision_120_faces[116] = {
#include "stg23/terrain/collision_120_faces.inc"
};
static NJS_MESHSET collision_120_meshsets[1] = {
#include "stg23/terrain/collision_120_meshsets.inc"
};
static NJS_POINT3 collision_120_points[51] = {
#include "stg23/terrain/collision_120_points.inc"
};
static NJS_MODEL collision_120_model = {
#include "stg23/terrain/collision_120_model.inc"
};
static NJS_OBJECT collision_120 = {
#include "stg23/terrain/collision_120.inc"
};
static NJS_MATERIAL collision_121_materials[1] = {
#include "stg23/terrain/collision_121_materials.inc"
};
static Sint16 collision_121_faces[159] = {
#include "stg23/terrain/collision_121_faces.inc"
};
static NJS_MESHSET collision_121_meshsets[1] = {
#include "stg23/terrain/collision_121_meshsets.inc"
};
static NJS_POINT3 collision_121_points[74] = {
#include "stg23/terrain/collision_121_points.inc"
};
static NJS_MODEL collision_121_model = {
#include "stg23/terrain/collision_121_model.inc"
};
static NJS_OBJECT collision_121 = {
#include "stg23/terrain/collision_121.inc"
};
static NJS_MATERIAL collision_123_materials[1] = {
#include "stg23/terrain/collision_123_materials.inc"
};
static Sint16 collision_123_faces[178] = {
#include "stg23/terrain/collision_123_faces.inc"
};
static NJS_MESHSET collision_123_meshsets[1] = {
#include "stg23/terrain/collision_123_meshsets.inc"
};
static NJS_POINT3 collision_123_points[82] = {
#include "stg23/terrain/collision_123_points.inc"
};
static NJS_MODEL collision_123_model = {
#include "stg23/terrain/collision_123_model.inc"
};
static NJS_OBJECT collision_123 = {
#include "stg23/terrain/collision_123.inc"
};
static NJS_MATERIAL collision_128_materials[1] = {
#include "stg23/terrain/collision_128_materials.inc"
};
static Sint16 collision_128_faces[21] = {
#include "stg23/terrain/collision_128_faces.inc"
};
static NJS_MESHSET collision_128_meshsets[1] = {
#include "stg23/terrain/collision_128_meshsets.inc"
};
static NJS_POINT3 collision_128_points[12] = {
#include "stg23/terrain/collision_128_points.inc"
};
static NJS_MODEL collision_128_model = {
#include "stg23/terrain/collision_128_model.inc"
};
static NJS_OBJECT collision_128 = {
#include "stg23/terrain/collision_128.inc"
};
static NJS_MATERIAL collision_129_materials[1] = {
#include "stg23/terrain/collision_129_materials.inc"
};
static Sint16 collision_129_faces[16] = {
#include "stg23/terrain/collision_129_faces.inc"
};
static NJS_MESHSET collision_129_meshsets[1] = {
#include "stg23/terrain/collision_129_meshsets.inc"
};
static NJS_POINT3 collision_129_points[8] = {
#include "stg23/terrain/collision_129_points.inc"
};
static NJS_MODEL collision_129_model = {
#include "stg23/terrain/collision_129_model.inc"
};
static NJS_OBJECT collision_129 = {
#include "stg23/terrain/collision_129.inc"
};
static NJS_MATERIAL collision_130_materials[1] = {
#include "stg23/terrain/collision_130_materials.inc"
};
static Sint16 collision_130_faces[25] = {
#include "stg23/terrain/collision_130_faces.inc"
};
static NJS_MESHSET collision_130_meshsets[1] = {
#include "stg23/terrain/collision_130_meshsets.inc"
};
static NJS_POINT3 collision_130_points[12] = {
#include "stg23/terrain/collision_130_points.inc"
};
static NJS_MODEL collision_130_model = {
#include "stg23/terrain/collision_130_model.inc"
};
static NJS_OBJECT collision_130 = {
#include "stg23/terrain/collision_130.inc"
};
static NJS_MATERIAL collision_131_materials[1] = {
#include "stg23/terrain/collision_131_materials.inc"
};
static Sint16 collision_131_faces[35] = {
#include "stg23/terrain/collision_131_faces.inc"
};
static NJS_MESHSET collision_131_meshsets[1] = {
#include "stg23/terrain/collision_131_meshsets.inc"
};
static NJS_POINT3 collision_131_points[18] = {
#include "stg23/terrain/collision_131_points.inc"
};
static NJS_MODEL collision_131_model = {
#include "stg23/terrain/collision_131_model.inc"
};
static NJS_OBJECT collision_131 = {
#include "stg23/terrain/collision_131.inc"
};
static NJS_MATERIAL collision_132_materials[1] = {
#include "stg23/terrain/collision_132_materials.inc"
};
static Sint16 collision_132_faces[23] = {
#include "stg23/terrain/collision_132_faces.inc"
};
static NJS_MESHSET collision_132_meshsets[1] = {
#include "stg23/terrain/collision_132_meshsets.inc"
};
static NJS_POINT3 collision_132_points[13] = {
#include "stg23/terrain/collision_132_points.inc"
};
static NJS_MODEL collision_132_model = {
#include "stg23/terrain/collision_132_model.inc"
};
static NJS_OBJECT collision_132 = {
#include "stg23/terrain/collision_132.inc"
};
static NJS_MATERIAL collision_133_materials[1] = {
#include "stg23/terrain/collision_133_materials.inc"
};
static Sint16 collision_133_faces[23] = {
#include "stg23/terrain/collision_133_faces.inc"
};
static NJS_MESHSET collision_133_meshsets[1] = {
#include "stg23/terrain/collision_133_meshsets.inc"
};
static NJS_POINT3 collision_133_points[12] = {
#include "stg23/terrain/collision_133_points.inc"
};
static NJS_MODEL collision_133_model = {
#include "stg23/terrain/collision_133_model.inc"
};
static NJS_OBJECT collision_133 = {
#include "stg23/terrain/collision_133.inc"
};
static NJS_MATERIAL collision_134_materials[1] = {
#include "stg23/terrain/collision_134_materials.inc"
};
static Sint16 collision_134_faces[41] = {
#include "stg23/terrain/collision_134_faces.inc"
};
static NJS_MESHSET collision_134_meshsets[1] = {
#include "stg23/terrain/collision_134_meshsets.inc"
};
static NJS_POINT3 collision_134_points[21] = {
#include "stg23/terrain/collision_134_points.inc"
};
static NJS_MODEL collision_134_model = {
#include "stg23/terrain/collision_134_model.inc"
};
static NJS_OBJECT collision_134 = {
#include "stg23/terrain/collision_134.inc"
};
static NJS_MATERIAL collision_135_materials[1] = {
#include "stg23/terrain/collision_135_materials.inc"
};
static Sint16 collision_135_faces[48] = {
#include "stg23/terrain/collision_135_faces.inc"
};
static NJS_MESHSET collision_135_meshsets[1] = {
#include "stg23/terrain/collision_135_meshsets.inc"
};
static NJS_POINT3 collision_135_points[16] = {
#include "stg23/terrain/collision_135_points.inc"
};
static NJS_MODEL collision_135_model = {
#include "stg23/terrain/collision_135_model.inc"
};
static NJS_OBJECT collision_135 = {
#include "stg23/terrain/collision_135.inc"
};
static NJS_MATERIAL collision_140_materials[1] = {
#include "stg23/terrain/collision_140_materials.inc"
};
static Sint16 collision_140_faces[79] = {
#include "stg23/terrain/collision_140_faces.inc"
};
static NJS_MESHSET collision_140_meshsets[1] = {
#include "stg23/terrain/collision_140_meshsets.inc"
};
static NJS_POINT3 collision_140_points[45] = {
#include "stg23/terrain/collision_140_points.inc"
};
static NJS_MODEL collision_140_model = {
#include "stg23/terrain/collision_140_model.inc"
};
static NJS_OBJECT collision_140 = {
#include "stg23/terrain/collision_140.inc"
};
static NJS_MATERIAL collision_142_materials[1] = {
#include "stg23/terrain/collision_142_materials.inc"
};
static Sint16 collision_142_faces[63] = {
#include "stg23/terrain/collision_142_faces.inc"
};
static NJS_MESHSET collision_142_meshsets[1] = {
#include "stg23/terrain/collision_142_meshsets.inc"
};
static NJS_POINT3 collision_142_points[24] = {
#include "stg23/terrain/collision_142_points.inc"
};
static NJS_MODEL collision_142_model = {
#include "stg23/terrain/collision_142_model.inc"
};
static NJS_OBJECT collision_142 = {
#include "stg23/terrain/collision_142.inc"
};
static NJS_MATERIAL collision_143_materials[1] = {
#include "stg23/terrain/collision_143_materials.inc"
};
static Sint16 collision_143_faces[146] = {
#include "stg23/terrain/collision_143_faces.inc"
};
static NJS_MESHSET collision_143_meshsets[1] = {
#include "stg23/terrain/collision_143_meshsets.inc"
};
static NJS_POINT3 collision_143_points[63] = {
#include "stg23/terrain/collision_143_points.inc"
};
static NJS_MODEL collision_143_model = {
#include "stg23/terrain/collision_143_model.inc"
};
static NJS_OBJECT collision_143 = {
#include "stg23/terrain/collision_143.inc"
};
static NJS_MATERIAL collision_144_materials[1] = {
#include "stg23/terrain/collision_144_materials.inc"
};
static Sint16 collision_144_faces[89] = {
#include "stg23/terrain/collision_144_faces.inc"
};
static NJS_MESHSET collision_144_meshsets[1] = {
#include "stg23/terrain/collision_144_meshsets.inc"
};
static NJS_POINT3 collision_144_points[50] = {
#include "stg23/terrain/collision_144_points.inc"
};
static NJS_MODEL collision_144_model = {
#include "stg23/terrain/collision_144_model.inc"
};
static NJS_OBJECT collision_144 = {
#include "stg23/terrain/collision_144.inc"
};
static NJS_MATERIAL collision_146_materials[1] = {
#include "stg23/terrain/collision_146_materials.inc"
};
static Sint16 collision_146_faces[184] = {
#include "stg23/terrain/collision_146_faces.inc"
};
static NJS_MESHSET collision_146_meshsets[1] = {
#include "stg23/terrain/collision_146_meshsets.inc"
};
static NJS_POINT3 collision_146_points[58] = {
#include "stg23/terrain/collision_146_points.inc"
};
static NJS_MODEL collision_146_model = {
#include "stg23/terrain/collision_146_model.inc"
};
static NJS_OBJECT collision_146 = {
#include "stg23/terrain/collision_146.inc"
};
static NJS_MATERIAL collision_147_materials[1] = {
#include "stg23/terrain/collision_147_materials.inc"
};
static Sint16 collision_147_faces[298] = {
#include "stg23/terrain/collision_147_faces.inc"
};
static NJS_MESHSET collision_147_meshsets[1] = {
#include "stg23/terrain/collision_147_meshsets.inc"
};
static NJS_POINT3 collision_147_points[96] = {
#include "stg23/terrain/collision_147_points.inc"
};
static NJS_MODEL collision_147_model = {
#include "stg23/terrain/collision_147_model.inc"
};
static NJS_OBJECT collision_147 = {
#include "stg23/terrain/collision_147.inc"
};
static NJS_MATERIAL collision_148_materials[1] = {
#include "stg23/terrain/collision_148_materials.inc"
};
static Sint16 collision_148_faces[221] = {
#include "stg23/terrain/collision_148_faces.inc"
};
static NJS_MESHSET collision_148_meshsets[1] = {
#include "stg23/terrain/collision_148_meshsets.inc"
};
static NJS_POINT3 collision_148_points[75] = {
#include "stg23/terrain/collision_148_points.inc"
};
static NJS_MODEL collision_148_model = {
#include "stg23/terrain/collision_148_model.inc"
};
static NJS_OBJECT collision_148 = {
#include "stg23/terrain/collision_148.inc"
};
static NJS_MATERIAL collision_149_materials[1] = {
#include "stg23/terrain/collision_149_materials.inc"
};
static Sint16 collision_149_faces[30] = {
#include "stg23/terrain/collision_149_faces.inc"
};
static NJS_MESHSET collision_149_meshsets[1] = {
#include "stg23/terrain/collision_149_meshsets.inc"
};
static NJS_POINT3 collision_149_points[11] = {
#include "stg23/terrain/collision_149_points.inc"
};
static NJS_MODEL collision_149_model = {
#include "stg23/terrain/collision_149_model.inc"
};
static NJS_OBJECT collision_149 = {
#include "stg23/terrain/collision_149.inc"
};
static NJS_MATERIAL collision_150_materials[1] = {
#include "stg23/terrain/collision_150_materials.inc"
};
static Sint16 collision_150_faces[10] = {
#include "stg23/terrain/collision_150_faces.inc"
};
static NJS_MESHSET collision_150_meshsets[1] = {
#include "stg23/terrain/collision_150_meshsets.inc"
};
static NJS_POINT3 collision_150_points[5] = {
#include "stg23/terrain/collision_150_points.inc"
};
static NJS_MODEL collision_150_model = {
#include "stg23/terrain/collision_150_model.inc"
};
static NJS_OBJECT collision_150 = {
#include "stg23/terrain/collision_150.inc"
};
static NJS_MATERIAL collision_151_materials[1] = {
#include "stg23/terrain/collision_151_materials.inc"
};
static Sint16 collision_151_faces[21] = {
#include "stg23/terrain/collision_151_faces.inc"
};
static NJS_MESHSET collision_151_meshsets[1] = {
#include "stg23/terrain/collision_151_meshsets.inc"
};
static NJS_POINT3 collision_151_points[11] = {
#include "stg23/terrain/collision_151_points.inc"
};
static NJS_MODEL collision_151_model = {
#include "stg23/terrain/collision_151_model.inc"
};
static NJS_OBJECT collision_151 = {
#include "stg23/terrain/collision_151.inc"
};
static NJS_MATERIAL collision_152_materials[1] = {
#include "stg23/terrain/collision_152_materials.inc"
};
static Sint16 collision_152_faces[19] = {
#include "stg23/terrain/collision_152_faces.inc"
};
static NJS_MESHSET collision_152_meshsets[1] = {
#include "stg23/terrain/collision_152_meshsets.inc"
};
static NJS_POINT3 collision_152_points[15] = {
#include "stg23/terrain/collision_152_points.inc"
};
static NJS_MODEL collision_152_model = {
#include "stg23/terrain/collision_152_model.inc"
};
static NJS_OBJECT collision_152 = {
#include "stg23/terrain/collision_152.inc"
};
static NJS_MATERIAL collision_153_materials[1] = {
#include "stg23/terrain/collision_153_materials.inc"
};
static Sint16 collision_153_faces[27] = {
#include "stg23/terrain/collision_153_faces.inc"
};
static NJS_MESHSET collision_153_meshsets[1] = {
#include "stg23/terrain/collision_153_meshsets.inc"
};
static NJS_POINT3 collision_153_points[16] = {
#include "stg23/terrain/collision_153_points.inc"
};
static NJS_MODEL collision_153_model = {
#include "stg23/terrain/collision_153_model.inc"
};
static NJS_OBJECT collision_153 = {
#include "stg23/terrain/collision_153.inc"
};
static NJS_MATERIAL collision_154_materials[1] = {
#include "stg23/terrain/collision_154_materials.inc"
};
static Sint16 collision_154_faces[76] = {
#include "stg23/terrain/collision_154_faces.inc"
};
static NJS_MESHSET collision_154_meshsets[1] = {
#include "stg23/terrain/collision_154_meshsets.inc"
};
static NJS_POINT3 collision_154_points[26] = {
#include "stg23/terrain/collision_154_points.inc"
};
static NJS_MODEL collision_154_model = {
#include "stg23/terrain/collision_154_model.inc"
};
static NJS_OBJECT collision_154 = {
#include "stg23/terrain/collision_154.inc"
};
static NJS_MATERIAL collision_155_materials[1] = {
#include "stg23/terrain/collision_155_materials.inc"
};
static Sint16 collision_155_faces[19] = {
#include "stg23/terrain/collision_155_faces.inc"
};
static NJS_MESHSET collision_155_meshsets[1] = {
#include "stg23/terrain/collision_155_meshsets.inc"
};
static NJS_POINT3 collision_155_points[8] = {
#include "stg23/terrain/collision_155_points.inc"
};
static NJS_MODEL collision_155_model = {
#include "stg23/terrain/collision_155_model.inc"
};
static NJS_OBJECT collision_155 = {
#include "stg23/terrain/collision_155.inc"
};
static NJS_MATERIAL collision_156_materials[1] = {
#include "stg23/terrain/collision_156_materials.inc"
};
static Sint16 collision_156_faces[11] = {
#include "stg23/terrain/collision_156_faces.inc"
};
static NJS_MESHSET collision_156_meshsets[1] = {
#include "stg23/terrain/collision_156_meshsets.inc"
};
static NJS_POINT3 collision_156_points[8] = {
#include "stg23/terrain/collision_156_points.inc"
};
static NJS_MODEL collision_156_model = {
#include "stg23/terrain/collision_156_model.inc"
};
static NJS_OBJECT collision_156 = {
#include "stg23/terrain/collision_156.inc"
};
static NJS_MATERIAL collision_158_materials[1] = {
#include "stg23/terrain/collision_158_materials.inc"
};
static Sint16 collision_158_faces[116] = {
#include "stg23/terrain/collision_158_faces.inc"
};
static NJS_MESHSET collision_158_meshsets[1] = {
#include "stg23/terrain/collision_158_meshsets.inc"
};
static NJS_POINT3 collision_158_points[45] = {
#include "stg23/terrain/collision_158_points.inc"
};
static NJS_MODEL collision_158_model = {
#include "stg23/terrain/collision_158_model.inc"
};
static NJS_OBJECT collision_158 = {
#include "stg23/terrain/collision_158.inc"
};
static NJS_MATERIAL collision_159_materials[1] = {
#include "stg23/terrain/collision_159_materials.inc"
};
static Sint16 collision_159_faces[57] = {
#include "stg23/terrain/collision_159_faces.inc"
};
static NJS_MESHSET collision_159_meshsets[1] = {
#include "stg23/terrain/collision_159_meshsets.inc"
};
static NJS_POINT3 collision_159_points[16] = {
#include "stg23/terrain/collision_159_points.inc"
};
static NJS_MODEL collision_159_model = {
#include "stg23/terrain/collision_159_model.inc"
};
static NJS_OBJECT collision_159 = {
#include "stg23/terrain/collision_159.inc"
};
static NJS_MATERIAL collision_160_materials[1] = {
#include "stg23/terrain/collision_160_materials.inc"
};
static Sint16 collision_160_faces[91] = {
#include "stg23/terrain/collision_160_faces.inc"
};
static NJS_MESHSET collision_160_meshsets[1] = {
#include "stg23/terrain/collision_160_meshsets.inc"
};
static NJS_POINT3 collision_160_points[42] = {
#include "stg23/terrain/collision_160_points.inc"
};
static NJS_MODEL collision_160_model = {
#include "stg23/terrain/collision_160_model.inc"
};
static NJS_OBJECT collision_160 = {
#include "stg23/terrain/collision_160.inc"
};
static NJS_MATERIAL collision_161_materials[1] = {
#include "stg23/terrain/collision_161_materials.inc"
};
static Sint16 collision_161_faces[25] = {
#include "stg23/terrain/collision_161_faces.inc"
};
static NJS_MESHSET collision_161_meshsets[1] = {
#include "stg23/terrain/collision_161_meshsets.inc"
};
static NJS_POINT3 collision_161_points[15] = {
#include "stg23/terrain/collision_161_points.inc"
};
static NJS_MODEL collision_161_model = {
#include "stg23/terrain/collision_161_model.inc"
};
static NJS_OBJECT collision_161 = {
#include "stg23/terrain/collision_161.inc"
};
static NJS_MATERIAL collision_162_materials[1] = {
#include "stg23/terrain/collision_162_materials.inc"
};
static Sint16 collision_162_faces[41] = {
#include "stg23/terrain/collision_162_faces.inc"
};
static NJS_MESHSET collision_162_meshsets[1] = {
#include "stg23/terrain/collision_162_meshsets.inc"
};
static NJS_POINT3 collision_162_points[18] = {
#include "stg23/terrain/collision_162_points.inc"
};
static NJS_MODEL collision_162_model = {
#include "stg23/terrain/collision_162_model.inc"
};
static NJS_OBJECT collision_162 = {
#include "stg23/terrain/collision_162.inc"
};
static NJS_MATERIAL collision_163_materials[1] = {
#include "stg23/terrain/collision_163_materials.inc"
};
static Sint16 collision_163_faces[27] = {
#include "stg23/terrain/collision_163_faces.inc"
};
static NJS_MESHSET collision_163_meshsets[1] = {
#include "stg23/terrain/collision_163_meshsets.inc"
};
static NJS_POINT3 collision_163_points[16] = {
#include "stg23/terrain/collision_163_points.inc"
};
static NJS_MODEL collision_163_model = {
#include "stg23/terrain/collision_163_model.inc"
};
static NJS_OBJECT collision_163 = {
#include "stg23/terrain/collision_163.inc"
};
static NJS_CNK_OBJECT draw_468 = {
#include "stg23/terrain/draw_468.inc"
};
static NJS_CNK_OBJECT draw_467 = {
#include "stg23/terrain/draw_467.inc"
};
static NJS_CNK_OBJECT draw_466 = {
#include "stg23/terrain/draw_466.inc"
};
static NJS_CNK_OBJECT draw_462 = {
#include "stg23/terrain/draw_462.inc"
};
static NJS_CNK_OBJECT draw_460 = {
#include "stg23/terrain/draw_460.inc"
};
static NJS_CNK_OBJECT draw_458 = {
#include "stg23/terrain/draw_458.inc"
};
static NJS_CNK_OBJECT draw_456 = {
#include "stg23/terrain/draw_456.inc"
};
static NJS_CNK_OBJECT draw_453 = {
#include "stg23/terrain/draw_453.inc"
};
static NJS_CNK_OBJECT draw_447 = {
#include "stg23/terrain/draw_447.inc"
};
static NJS_CNK_OBJECT draw_445 = {
#include "stg23/terrain/draw_445.inc"
};
static NJS_CNK_OBJECT draw_444 = {
#include "stg23/terrain/draw_444.inc"
};
static NJS_CNK_OBJECT draw_442 = {
#include "stg23/terrain/draw_442.inc"
};
static NJS_CNK_OBJECT draw_441 = {
#include "stg23/terrain/draw_441.inc"
};
static NJS_CNK_OBJECT draw_434 = {
#include "stg23/terrain/draw_434.inc"
};
static NJS_CNK_OBJECT draw_425 = {
#include "stg23/terrain/draw_425.inc"
};
static NJS_CNK_OBJECT draw_424 = {
#include "stg23/terrain/draw_424.inc"
};
static NJS_CNK_OBJECT draw_422 = {
#include "stg23/terrain/draw_422.inc"
};
static NJS_CNK_OBJECT draw_421 = {
#include "stg23/terrain/draw_421.inc"
};
static NJS_CNK_OBJECT draw_417 = {
#include "stg23/terrain/draw_417.inc"
};
static NJS_CNK_OBJECT draw_416 = {
#include "stg23/terrain/draw_416.inc"
};
static NJS_CNK_OBJECT draw_415 = {
#include "stg23/terrain/draw_415.inc"
};
static NJS_CNK_OBJECT draw_414 = {
#include "stg23/terrain/draw_414.inc"
};
static NJS_CNK_OBJECT draw_413 = {
#include "stg23/terrain/draw_413.inc"
};
static NJS_CNK_OBJECT draw_412 = {
#include "stg23/terrain/draw_412.inc"
};
static NJS_CNK_OBJECT draw_411 = {
#include "stg23/terrain/draw_411.inc"
};
static NJS_CNK_OBJECT draw_409 = {
#include "stg23/terrain/draw_409.inc"
};
static NJS_CNK_OBJECT draw_408 = {
#include "stg23/terrain/draw_408.inc"
};
static NJS_CNK_OBJECT draw_407 = {
#include "stg23/terrain/draw_407.inc"
};
static NJS_CNK_OBJECT draw_405 = {
#include "stg23/terrain/draw_405.inc"
};
static NJS_CNK_OBJECT draw_404 = {
#include "stg23/terrain/draw_404.inc"
};
static NJS_CNK_OBJECT draw_403 = {
#include "stg23/terrain/draw_403.inc"
};
static NJS_CNK_OBJECT draw_402 = {
#include "stg23/terrain/draw_402.inc"
};
static NJS_CNK_OBJECT draw_401 = {
#include "stg23/terrain/draw_401.inc"
};
static NJS_CNK_OBJECT draw_400 = {
#include "stg23/terrain/draw_400.inc"
};
static NJS_CNK_OBJECT draw_399 = {
#include "stg23/terrain/draw_399.inc"
};
static NJS_CNK_OBJECT draw_398 = {
#include "stg23/terrain/draw_398.inc"
};
static NJS_CNK_OBJECT draw_397 = {
#include "stg23/terrain/draw_397.inc"
};
static NJS_CNK_OBJECT draw_396 = {
#include "stg23/terrain/draw_396.inc"
};
static NJS_CNK_OBJECT draw_395 = {
#include "stg23/terrain/draw_395.inc"
};
static NJS_CNK_OBJECT draw_394 = {
#include "stg23/terrain/draw_394.inc"
};
static NJS_CNK_OBJECT draw_393 = {
#include "stg23/terrain/draw_393.inc"
};
static NJS_CNK_OBJECT draw_392 = {
#include "stg23/terrain/draw_392.inc"
};
static NJS_CNK_OBJECT draw_391 = {
#include "stg23/terrain/draw_391.inc"
};
static NJS_CNK_OBJECT draw_390 = {
#include "stg23/terrain/draw_390.inc"
};
static NJS_CNK_OBJECT draw_389 = {
#include "stg23/terrain/draw_389.inc"
};
static NJS_CNK_OBJECT draw_388 = {
#include "stg23/terrain/draw_388.inc"
};
static NJS_CNK_OBJECT draw_387 = {
#include "stg23/terrain/draw_387.inc"
};
static NJS_CNK_OBJECT draw_386 = {
#include "stg23/terrain/draw_386.inc"
};
static NJS_CNK_OBJECT draw_385 = {
#include "stg23/terrain/draw_385.inc"
};
static NJS_CNK_OBJECT draw_384 = {
#include "stg23/terrain/draw_384.inc"
};
static NJS_CNK_OBJECT draw_383 = {
#include "stg23/terrain/draw_383.inc"
};
static NJS_CNK_OBJECT draw_382 = {
#include "stg23/terrain/draw_382.inc"
};
static NJS_CNK_OBJECT draw_381 = {
#include "stg23/terrain/draw_381.inc"
};
static NJS_CNK_OBJECT draw_380 = {
#include "stg23/terrain/draw_380.inc"
};
static NJS_CNK_OBJECT draw_379 = {
#include "stg23/terrain/draw_379.inc"
};
static NJS_CNK_OBJECT draw_378 = {
#include "stg23/terrain/draw_378.inc"
};
static NJS_CNK_OBJECT draw_377 = {
#include "stg23/terrain/draw_377.inc"
};
static NJS_CNK_OBJECT draw_376 = {
#include "stg23/terrain/draw_376.inc"
};
static NJS_CNK_OBJECT draw_375 = {
#include "stg23/terrain/draw_375.inc"
};
static NJS_CNK_OBJECT draw_374 = {
#include "stg23/terrain/draw_374.inc"
};
static NJS_CNK_OBJECT draw_373 = {
#include "stg23/terrain/draw_373.inc"
};
static NJS_CNK_OBJECT draw_372 = {
#include "stg23/terrain/draw_372.inc"
};
static NJS_CNK_OBJECT draw_371 = {
#include "stg23/terrain/draw_371.inc"
};
static NJS_CNK_OBJECT draw_370 = {
#include "stg23/terrain/draw_370.inc"
};
static NJS_CNK_OBJECT draw_369 = {
#include "stg23/terrain/draw_369.inc"
};
static NJS_CNK_OBJECT draw_368 = {
#include "stg23/terrain/draw_368.inc"
};
static NJS_CNK_OBJECT draw_367 = {
#include "stg23/terrain/draw_367.inc"
};
static NJS_CNK_OBJECT draw_366 = {
#include "stg23/terrain/draw_366.inc"
};
static NJS_CNK_OBJECT draw_365 = {
#include "stg23/terrain/draw_365.inc"
};
static NJS_CNK_OBJECT draw_364 = {
#include "stg23/terrain/draw_364.inc"
};
static NJS_CNK_OBJECT draw_359 = {
#include "stg23/terrain/draw_359.inc"
};
static NJS_CNK_OBJECT draw_349 = {
#include "stg23/terrain/draw_349.inc"
};
static NJS_CNK_OBJECT draw_348 = {
#include "stg23/terrain/draw_348.inc"
};
static NJS_CNK_OBJECT draw_347 = {
#include "stg23/terrain/draw_347.inc"
};
static NJS_CNK_OBJECT draw_346 = {
#include "stg23/terrain/draw_346.inc"
};
static NJS_CNK_OBJECT draw_345 = {
#include "stg23/terrain/draw_345.inc"
};
static NJS_CNK_OBJECT draw_344 = {
#include "stg23/terrain/draw_344.inc"
};
static NJS_CNK_OBJECT draw_343 = {
#include "stg23/terrain/draw_343.inc"
};
static NJS_CNK_OBJECT draw_342 = {
#include "stg23/terrain/draw_342.inc"
};
static NJS_CNK_OBJECT draw_341 = {
#include "stg23/terrain/draw_341.inc"
};
static NJS_CNK_OBJECT draw_340 = {
#include "stg23/terrain/draw_340.inc"
};
static NJS_CNK_OBJECT draw_339 = {
#include "stg23/terrain/draw_339.inc"
};
static NJS_CNK_OBJECT draw_338 = {
#include "stg23/terrain/draw_338.inc"
};
static NJS_CNK_OBJECT draw_337 = {
#include "stg23/terrain/draw_337.inc"
};
static NJS_CNK_OBJECT draw_336 = {
#include "stg23/terrain/draw_336.inc"
};
static NJS_CNK_OBJECT draw_335 = {
#include "stg23/terrain/draw_335.inc"
};
static NJS_CNK_OBJECT draw_334 = {
#include "stg23/terrain/draw_334.inc"
};
static NJS_CNK_OBJECT draw_333 = {
#include "stg23/terrain/draw_333.inc"
};
static NJS_CNK_OBJECT draw_332 = {
#include "stg23/terrain/draw_332.inc"
};
static NJS_CNK_OBJECT draw_331 = {
#include "stg23/terrain/draw_331.inc"
};
static NJS_CNK_OBJECT draw_330 = {
#include "stg23/terrain/draw_330.inc"
};
static NJS_CNK_OBJECT draw_329 = {
#include "stg23/terrain/draw_329.inc"
};
static NJS_CNK_OBJECT draw_328 = {
#include "stg23/terrain/draw_328.inc"
};
static NJS_CNK_OBJECT draw_327 = {
#include "stg23/terrain/draw_327.inc"
};
static NJS_CNK_OBJECT draw_326 = {
#include "stg23/terrain/draw_326.inc"
};
static NJS_CNK_OBJECT draw_325 = {
#include "stg23/terrain/draw_325.inc"
};
static NJS_CNK_OBJECT draw_324 = {
#include "stg23/terrain/draw_324.inc"
};
static NJS_CNK_OBJECT draw_323 = {
#include "stg23/terrain/draw_323.inc"
};
static NJS_CNK_OBJECT draw_322 = {
#include "stg23/terrain/draw_322.inc"
};
static NJS_CNK_OBJECT draw_321 = {
#include "stg23/terrain/draw_321.inc"
};
static NJS_CNK_OBJECT draw_320 = {
#include "stg23/terrain/draw_320.inc"
};
static NJS_CNK_OBJECT draw_319 = {
#include "stg23/terrain/draw_319.inc"
};
static NJS_CNK_OBJECT draw_318 = {
#include "stg23/terrain/draw_318.inc"
};
static NJS_CNK_OBJECT draw_317 = {
#include "stg23/terrain/draw_317.inc"
};
static NJS_CNK_OBJECT draw_316 = {
#include "stg23/terrain/draw_316.inc"
};
static NJS_CNK_OBJECT draw_315 = {
#include "stg23/terrain/draw_315.inc"
};
static NJS_CNK_OBJECT draw_314 = {
#include "stg23/terrain/draw_314.inc"
};
static NJS_CNK_OBJECT draw_313 = {
#include "stg23/terrain/draw_313.inc"
};
static NJS_CNK_OBJECT draw_312 = {
#include "stg23/terrain/draw_312.inc"
};
static NJS_CNK_OBJECT draw_311 = {
#include "stg23/terrain/draw_311.inc"
};
static NJS_CNK_OBJECT draw_310 = {
#include "stg23/terrain/draw_310.inc"
};
static NJS_CNK_OBJECT draw_309 = {
#include "stg23/terrain/draw_309.inc"
};
static NJS_CNK_OBJECT draw_308 = {
#include "stg23/terrain/draw_308.inc"
};
static NJS_CNK_OBJECT draw_307 = {
#include "stg23/terrain/draw_307.inc"
};
static NJS_CNK_OBJECT draw_305 = {
#include "stg23/terrain/draw_305.inc"
};
static NJS_CNK_OBJECT draw_300 = {
#include "stg23/terrain/draw_300.inc"
};
static NJS_CNK_OBJECT draw_299 = {
#include "stg23/terrain/draw_299.inc"
};
static NJS_CNK_OBJECT draw_298 = {
#include "stg23/terrain/draw_298.inc"
};
static NJS_CNK_OBJECT draw_297 = {
#include "stg23/terrain/draw_297.inc"
};
static NJS_CNK_OBJECT draw_296 = {
#include "stg23/terrain/draw_296.inc"
};
static NJS_CNK_OBJECT draw_295 = {
#include "stg23/terrain/draw_295.inc"
};
static NJS_CNK_OBJECT draw_294 = {
#include "stg23/terrain/draw_294.inc"
};
static NJS_CNK_OBJECT draw_293 = {
#include "stg23/terrain/draw_293.inc"
};
static NJS_CNK_OBJECT draw_292 = {
#include "stg23/terrain/draw_292.inc"
};
static NJS_CNK_OBJECT draw_291 = {
#include "stg23/terrain/draw_291.inc"
};
static NJS_CNK_OBJECT draw_290 = {
#include "stg23/terrain/draw_290.inc"
};
static NJS_CNK_OBJECT draw_289 = {
#include "stg23/terrain/draw_289.inc"
};
static NJS_CNK_OBJECT draw_288 = {
#include "stg23/terrain/draw_288.inc"
};
static NJS_CNK_OBJECT draw_287 = {
#include "stg23/terrain/draw_287.inc"
};
static NJS_CNK_OBJECT draw_285 = {
#include "stg23/terrain/draw_285.inc"
};
static NJS_CNK_OBJECT draw_283 = {
#include "stg23/terrain/draw_283.inc"
};
static NJS_CNK_OBJECT draw_281 = {
#include "stg23/terrain/draw_281.inc"
};
static NJS_CNK_OBJECT draw_265 = {
#include "stg23/terrain/draw_265.inc"
};
static NJS_CNK_OBJECT draw_264 = {
#include "stg23/terrain/draw_264.inc"
};
static NJS_CNK_OBJECT draw_263 = {
#include "stg23/terrain/draw_263.inc"
};
static NJS_CNK_OBJECT draw_262 = {
#include "stg23/terrain/draw_262.inc"
};
static NJS_CNK_OBJECT draw_260 = {
#include "stg23/terrain/draw_260.inc"
};
static NJS_CNK_OBJECT draw_259 = {
#include "stg23/terrain/draw_259.inc"
};
static NJS_CNK_OBJECT draw_258 = {
#include "stg23/terrain/draw_258.inc"
};
static NJS_CNK_OBJECT draw_257 = {
#include "stg23/terrain/draw_257.inc"
};
static NJS_CNK_OBJECT draw_256 = {
#include "stg23/terrain/draw_256.inc"
};
static NJS_CNK_OBJECT draw_255 = {
#include "stg23/terrain/draw_255.inc"
};
static NJS_CNK_OBJECT draw_252 = {
#include "stg23/terrain/draw_252.inc"
};
static NJS_CNK_OBJECT draw_249 = {
#include "stg23/terrain/draw_249.inc"
};
static NJS_CNK_OBJECT draw_242 = {
#include "stg23/terrain/draw_242.inc"
};
static NJS_CNK_OBJECT draw_241 = {
#include "stg23/terrain/draw_241.inc"
};
static NJS_CNK_OBJECT draw_239 = {
#include "stg23/terrain/draw_239.inc"
};
static NJS_CNK_OBJECT draw_234 = {
#include "stg23/terrain/draw_234.inc"
};
static NJS_CNK_OBJECT draw_233 = {
#include "stg23/terrain/draw_233.inc"
};
static NJS_CNK_OBJECT draw_232 = {
#include "stg23/terrain/draw_232.inc"
};
static NJS_CNK_OBJECT draw_231 = {
#include "stg23/terrain/draw_231.inc"
};
static NJS_CNK_OBJECT draw_230 = {
#include "stg23/terrain/draw_230.inc"
};
static NJS_CNK_OBJECT draw_229 = {
#include "stg23/terrain/draw_229.inc"
};
static NJS_CNK_OBJECT draw_228 = {
#include "stg23/terrain/draw_228.inc"
};
static NJS_CNK_OBJECT draw_227 = {
#include "stg23/terrain/draw_227.inc"
};
static NJS_CNK_OBJECT draw_226 = {
#include "stg23/terrain/draw_226.inc"
};
static NJS_CNK_OBJECT draw_225 = {
#include "stg23/terrain/draw_225.inc"
};
static NJS_CNK_OBJECT draw_224 = {
#include "stg23/terrain/draw_224.inc"
};
static NJS_CNK_OBJECT draw_223 = {
#include "stg23/terrain/draw_223.inc"
};
static NJS_CNK_OBJECT draw_222 = {
#include "stg23/terrain/draw_222.inc"
};
static NJS_CNK_OBJECT draw_221 = {
#include "stg23/terrain/draw_221.inc"
};
static NJS_CNK_OBJECT draw_218 = {
#include "stg23/terrain/draw_218.inc"
};
static NJS_CNK_OBJECT draw_216 = {
#include "stg23/terrain/draw_216.inc"
};
static NJS_CNK_OBJECT draw_215 = {
#include "stg23/terrain/draw_215.inc"
};
static NJS_CNK_OBJECT draw_214 = {
#include "stg23/terrain/draw_214.inc"
};
static NJS_CNK_OBJECT draw_213 = {
#include "stg23/terrain/draw_213.inc"
};
static NJS_CNK_OBJECT draw_206 = {
#include "stg23/terrain/draw_206.inc"
};
static NJS_CNK_OBJECT draw_205 = {
#include "stg23/terrain/draw_205.inc"
};
static NJS_CNK_OBJECT draw_204 = {
#include "stg23/terrain/draw_204.inc"
};
static NJS_CNK_OBJECT draw_203 = {
#include "stg23/terrain/draw_203.inc"
};
static NJS_CNK_OBJECT draw_202 = {
#include "stg23/terrain/draw_202.inc"
};
static NJS_CNK_OBJECT draw_200 = {
#include "stg23/terrain/draw_200.inc"
};
static NJS_CNK_OBJECT draw_197 = {
#include "stg23/terrain/draw_197.inc"
};
static NJS_CNK_OBJECT draw_196 = {
#include "stg23/terrain/draw_196.inc"
};
static NJS_CNK_OBJECT draw_195 = {
#include "stg23/terrain/draw_195.inc"
};
static NJS_CNK_OBJECT draw_193 = {
#include "stg23/terrain/draw_193.inc"
};
static NJS_CNK_OBJECT draw_192 = {
#include "stg23/terrain/draw_192.inc"
};
static NJS_CNK_OBJECT draw_191 = {
#include "stg23/terrain/draw_191.inc"
};
static NJS_CNK_OBJECT draw_190 = {
#include "stg23/terrain/draw_190.inc"
};
static NJS_CNK_OBJECT draw_189 = {
#include "stg23/terrain/draw_189.inc"
};
static NJS_CNK_OBJECT draw_188 = {
#include "stg23/terrain/draw_188.inc"
};
static NJS_CNK_OBJECT draw_187 = {
#include "stg23/terrain/draw_187.inc"
};
static NJS_CNK_OBJECT draw_186 = {
#include "stg23/terrain/draw_186.inc"
};
static NJS_CNK_OBJECT draw_185 = {
#include "stg23/terrain/draw_185.inc"
};
static NJS_CNK_OBJECT draw_184 = {
#include "stg23/terrain/draw_184.inc"
};
static NJS_CNK_OBJECT draw_183 = {
#include "stg23/terrain/draw_183.inc"
};
static NJS_CNK_OBJECT draw_182 = {
#include "stg23/terrain/draw_182.inc"
};
static NJS_CNK_OBJECT draw_176 = {
#include "stg23/terrain/draw_176.inc"
};
static NJS_CNK_OBJECT draw_175 = {
#include "stg23/terrain/draw_175.inc"
};
static NJS_CNK_OBJECT draw_174 = {
#include "stg23/terrain/draw_174.inc"
};
static NJS_CNK_OBJECT draw_172 = {
#include "stg23/terrain/draw_172.inc"
};
static NJS_CNK_OBJECT draw_171 = {
#include "stg23/terrain/draw_171.inc"
};
static NJS_CNK_OBJECT draw_170 = {
#include "stg23/terrain/draw_170.inc"
};
static NJS_CNK_OBJECT draw_168 = {
#include "stg23/terrain/draw_168.inc"
};
static NJS_CNK_OBJECT draw_166 = {
#include "stg23/terrain/draw_166.inc"
};
static NJS_CNK_OBJECT draw_165 = {
#include "stg23/terrain/draw_165.inc"
};
static NJS_CNK_OBJECT draw_164 = {
#include "stg23/terrain/draw_164.inc"
};
static NJS_CNK_OBJECT draw_163 = {
#include "stg23/terrain/draw_163.inc"
};
static NJS_CNK_OBJECT draw_162 = {
#include "stg23/terrain/draw_162.inc"
};
static NJS_CNK_OBJECT draw_161 = {
#include "stg23/terrain/draw_161.inc"
};
static NJS_CNK_OBJECT draw_160 = {
#include "stg23/terrain/draw_160.inc"
};
static NJS_CNK_OBJECT draw_159 = {
#include "stg23/terrain/draw_159.inc"
};
static NJS_CNK_OBJECT draw_158 = {
#include "stg23/terrain/draw_158.inc"
};
static NJS_CNK_OBJECT draw_157 = {
#include "stg23/terrain/draw_157.inc"
};
static NJS_CNK_OBJECT draw_150 = {
#include "stg23/terrain/draw_150.inc"
};
static NJS_CNK_OBJECT draw_148 = {
#include "stg23/terrain/draw_148.inc"
};
static NJS_CNK_OBJECT draw_147 = {
#include "stg23/terrain/draw_147.inc"
};
static NJS_CNK_OBJECT draw_144 = {
#include "stg23/terrain/draw_144.inc"
};
static NJS_CNK_OBJECT draw_141 = {
#include "stg23/terrain/draw_141.inc"
};
static NJS_CNK_OBJECT draw_139 = {
#include "stg23/terrain/draw_139.inc"
};
static NJS_CNK_OBJECT draw_137 = {
#include "stg23/terrain/draw_137.inc"
};
static NJS_CNK_OBJECT draw_133 = {
#include "stg23/terrain/draw_133.inc"
};
static NJS_CNK_OBJECT draw_123 = {
#include "stg23/terrain/draw_123.inc"
};
static NJS_CNK_OBJECT draw_113 = {
#include "stg23/terrain/draw_113.inc"
};
static NJS_CNK_OBJECT draw_100 = {
#include "stg23/terrain/draw_100.inc"
};
static NJS_CNK_OBJECT draw_97 = {
#include "stg23/terrain/draw_97.inc"
};
static NJS_CNK_OBJECT draw_96 = {
#include "stg23/terrain/draw_96.inc"
};
static NJS_CNK_OBJECT draw_95 = {
#include "stg23/terrain/draw_95.inc"
};
static NJS_CNK_OBJECT draw_94 = {
#include "stg23/terrain/draw_94.inc"
};
static NJS_CNK_OBJECT draw_93 = {
#include "stg23/terrain/draw_93.inc"
};
static NJS_CNK_OBJECT draw_92 = {
#include "stg23/terrain/draw_92.inc"
};
static NJS_CNK_OBJECT draw_91 = {
#include "stg23/terrain/draw_91.inc"
};
static NJS_CNK_OBJECT draw_90 = {
#include "stg23/terrain/draw_90.inc"
};
static NJS_CNK_OBJECT draw_89 = {
#include "stg23/terrain/draw_89.inc"
};
static NJS_CNK_OBJECT draw_88 = {
#include "stg23/terrain/draw_88.inc"
};
static NJS_CNK_OBJECT draw_87 = {
#include "stg23/terrain/draw_87.inc"
};
static NJS_CNK_OBJECT draw_86 = {
#include "stg23/terrain/draw_86.inc"
};
static NJS_CNK_OBJECT draw_85 = {
#include "stg23/terrain/draw_85.inc"
};
static NJS_CNK_OBJECT draw_84 = {
#include "stg23/terrain/draw_84.inc"
};
static NJS_CNK_OBJECT draw_83 = {
#include "stg23/terrain/draw_83.inc"
};
static NJS_CNK_OBJECT draw_82 = {
#include "stg23/terrain/draw_82.inc"
};
static NJS_CNK_OBJECT draw_81 = {
#include "stg23/terrain/draw_81.inc"
};
static NJS_CNK_OBJECT draw_79 = {
#include "stg23/terrain/draw_79.inc"
};
static NJS_CNK_OBJECT draw_78 = {
#include "stg23/terrain/draw_78.inc"
};
static NJS_CNK_OBJECT draw_76 = {
#include "stg23/terrain/draw_76.inc"
};
static NJS_CNK_OBJECT draw_71 = {
#include "stg23/terrain/draw_71.inc"
};
static NJS_CNK_OBJECT draw_70 = {
#include "stg23/terrain/draw_70.inc"
};
static NJS_CNK_OBJECT draw_64 = {
#include "stg23/terrain/draw_64.inc"
};
static NJS_CNK_OBJECT draw_63 = {
#include "stg23/terrain/draw_63.inc"
};
static NJS_CNK_OBJECT draw_62 = {
#include "stg23/terrain/draw_62.inc"
};
static NJS_CNK_OBJECT draw_61 = {
#include "stg23/terrain/draw_61.inc"
};
static NJS_CNK_OBJECT draw_60 = {
#include "stg23/terrain/draw_60.inc"
};
static NJS_CNK_OBJECT draw_59 = {
#include "stg23/terrain/draw_59.inc"
};
static NJS_CNK_OBJECT draw_58 = {
#include "stg23/terrain/draw_58.inc"
};
static NJS_CNK_OBJECT draw_57 = {
#include "stg23/terrain/draw_57.inc"
};
static NJS_CNK_OBJECT draw_56 = {
#include "stg23/terrain/draw_56.inc"
};
static NJS_CNK_OBJECT draw_55 = {
#include "stg23/terrain/draw_55.inc"
};
static NJS_CNK_OBJECT draw_54 = {
#include "stg23/terrain/draw_54.inc"
};
static NJS_CNK_OBJECT draw_53 = {
#include "stg23/terrain/draw_53.inc"
};
static NJS_CNK_OBJECT draw_52 = {
#include "stg23/terrain/draw_52.inc"
};
static NJS_CNK_OBJECT draw_51 = {
#include "stg23/terrain/draw_51.inc"
};
static NJS_CNK_OBJECT draw_49 = {
#include "stg23/terrain/draw_49.inc"
};
static NJS_CNK_OBJECT draw_48 = {
#include "stg23/terrain/draw_48.inc"
};
static NJS_CNK_OBJECT draw_47 = {
#include "stg23/terrain/draw_47.inc"
};
static NJS_CNK_OBJECT draw_46 = {
#include "stg23/terrain/draw_46.inc"
};
static NJS_CNK_OBJECT draw_45 = {
#include "stg23/terrain/draw_45.inc"
};
static NJS_CNK_OBJECT draw_44 = {
#include "stg23/terrain/draw_44.inc"
};
static NJS_CNK_OBJECT draw_43 = {
#include "stg23/terrain/draw_43.inc"
};
static NJS_CNK_OBJECT draw_42 = {
#include "stg23/terrain/draw_42.inc"
};
static NJS_CNK_OBJECT draw_41 = {
#include "stg23/terrain/draw_41.inc"
};
static NJS_CNK_OBJECT draw_40 = {
#include "stg23/terrain/draw_40.inc"
};
static NJS_CNK_OBJECT draw_39 = {
#include "stg23/terrain/draw_39.inc"
};
static NJS_CNK_OBJECT draw_38 = {
#include "stg23/terrain/draw_38.inc"
};
static NJS_CNK_OBJECT draw_37 = {
#include "stg23/terrain/draw_37.inc"
};
static NJS_CNK_OBJECT draw_32 = {
#include "stg23/terrain/draw_32.inc"
};
static NJS_CNK_OBJECT draw_30 = {
#include "stg23/terrain/draw_30.inc"
};
static NJS_CNK_OBJECT draw_26 = {
#include "stg23/terrain/draw_26.inc"
};
static NJS_CNK_OBJECT draw_18 = {
#include "stg23/terrain/draw_18.inc"
};
static NJS_CNK_OBJECT draw_15 = {
#include "stg23/terrain/draw_15.inc"
};
static NJS_CNK_OBJECT draw_14 = {
#include "stg23/terrain/draw_14.inc"
};
static NJS_CNK_OBJECT draw_13 = {
#include "stg23/terrain/draw_13.inc"
};
static NJS_CNK_OBJECT draw_12 = {
#include "stg23/terrain/draw_12.inc"
};
static NJS_CNK_OBJECT draw_11 = {
#include "stg23/terrain/draw_11.inc"
};
static NJS_CNK_OBJECT draw_10 = {
#include "stg23/terrain/draw_10.inc"
};
static NJS_CNK_OBJECT draw_9 = {
#include "stg23/terrain/draw_9.inc"
};
static NJS_CNK_OBJECT draw_8 = {
#include "stg23/terrain/draw_8.inc"
};
static NJS_CNK_OBJECT draw_6 = {
#include "stg23/terrain/draw_6.inc"
};
static NJS_CNK_OBJECT draw_5 = {
#include "stg23/terrain/draw_5.inc"
};
static NJS_CNK_OBJECT draw_4 = {
#include "stg23/terrain/draw_4.inc"
};
static NJS_CNK_OBJECT draw_3 = {
#include "stg23/terrain/draw_3.inc"
};
static NJS_CNK_OBJECT draw_2 = {
#include "stg23/terrain/draw_2.inc"
};
static NJS_CNK_OBJECT draw_1 = {
#include "stg23/terrain/draw_1.inc"
};
static NJS_CNK_OBJECT draw_0 = {
#include "stg23/terrain/draw_0.inc"
};
static NJS_OBJECT collision_1 = {
#include "stg23/terrain/collision_1.inc"
};
static NJS_OBJECT collision_3 = {
#include "stg23/terrain/collision_3.inc"
};
static NJS_OBJECT collision_30 = {
#include "stg23/terrain/collision_30.inc"
};
static NJS_OBJECT collision_31 = {
#include "stg23/terrain/collision_31.inc"
};
static NJS_OBJECT collision_32 = {
#include "stg23/terrain/collision_32.inc"
};
static NJS_OBJECT collision_33 = {
#include "stg23/terrain/collision_33.inc"
};
static NJS_OBJECT collision_35 = {
#include "stg23/terrain/collision_35.inc"
};
static NJS_OBJECT collision_36 = {
#include "stg23/terrain/collision_36.inc"
};
static NJS_OBJECT collision_37 = {
#include "stg23/terrain/collision_37.inc"
};
static NJS_OBJECT collision_38 = {
#include "stg23/terrain/collision_38.inc"
};
static NJS_OBJECT collision_39 = {
#include "stg23/terrain/collision_39.inc"
};
static NJS_OBJECT collision_40 = {
#include "stg23/terrain/collision_40.inc"
};
static NJS_OBJECT collision_41 = {
#include "stg23/terrain/collision_41.inc"
};
static NJS_OBJECT collision_42 = {
#include "stg23/terrain/collision_42.inc"
};
static NJS_OBJECT collision_43 = {
#include "stg23/terrain/collision_43.inc"
};
static NJS_OBJECT collision_45 = {
#include "stg23/terrain/collision_45.inc"
};
static NJS_OBJECT collision_46 = {
#include "stg23/terrain/collision_46.inc"
};
static NJS_OBJECT collision_47 = {
#include "stg23/terrain/collision_47.inc"
};
static NJS_OBJECT collision_48 = {
#include "stg23/terrain/collision_48.inc"
};
static NJS_OBJECT collision_50 = {
#include "stg23/terrain/collision_50.inc"
};
static NJS_OBJECT collision_51 = {
#include "stg23/terrain/collision_51.inc"
};
static NJS_OBJECT collision_52 = {
#include "stg23/terrain/collision_52.inc"
};
static NJS_OBJECT collision_53 = {
#include "stg23/terrain/collision_53.inc"
};
static NJS_OBJECT collision_58 = {
#include "stg23/terrain/collision_58.inc"
};
static NJS_OBJECT collision_59 = {
#include "stg23/terrain/collision_59.inc"
};
static NJS_OBJECT collision_60 = {
#include "stg23/terrain/collision_60.inc"
};
static NJS_OBJECT collision_61 = {
#include "stg23/terrain/collision_61.inc"
};
static NJS_OBJECT collision_62 = {
#include "stg23/terrain/collision_62.inc"
};
static NJS_OBJECT collision_65 = {
#include "stg23/terrain/collision_65.inc"
};
static NJS_OBJECT collision_66 = {
#include "stg23/terrain/collision_66.inc"
};
static NJS_OBJECT collision_68 = {
#include "stg23/terrain/collision_68.inc"
};
static NJS_OBJECT collision_78 = {
#include "stg23/terrain/collision_78.inc"
};
static NJS_OBJECT collision_79 = {
#include "stg23/terrain/collision_79.inc"
};
static NJS_OBJECT collision_80 = {
#include "stg23/terrain/collision_80.inc"
};
static NJS_OBJECT collision_81 = {
#include "stg23/terrain/collision_81.inc"
};
static NJS_OBJECT collision_82 = {
#include "stg23/terrain/collision_82.inc"
};
static NJS_OBJECT collision_83 = {
#include "stg23/terrain/collision_83.inc"
};
static NJS_OBJECT collision_84 = {
#include "stg23/terrain/collision_84.inc"
};
static NJS_OBJECT collision_85 = {
#include "stg23/terrain/collision_85.inc"
};
static NJS_OBJECT collision_86 = {
#include "stg23/terrain/collision_86.inc"
};
static NJS_OBJECT collision_87 = {
#include "stg23/terrain/collision_87.inc"
};
static NJS_OBJECT collision_89 = {
#include "stg23/terrain/collision_89.inc"
};
static NJS_OBJECT collision_90 = {
#include "stg23/terrain/collision_90.inc"
};
static NJS_OBJECT collision_91 = {
#include "stg23/terrain/collision_91.inc"
};
static NJS_OBJECT collision_92 = {
#include "stg23/terrain/collision_92.inc"
};
static NJS_OBJECT collision_93 = {
#include "stg23/terrain/collision_93.inc"
};
static NJS_OBJECT collision_94 = {
#include "stg23/terrain/collision_94.inc"
};
static NJS_OBJECT collision_95 = {
#include "stg23/terrain/collision_95.inc"
};
static NJS_OBJECT collision_99 = {
#include "stg23/terrain/collision_99.inc"
};
static NJS_OBJECT collision_101 = {
#include "stg23/terrain/collision_101.inc"
};
static NJS_OBJECT collision_104 = {
#include "stg23/terrain/collision_104.inc"
};
static NJS_OBJECT collision_105 = {
#include "stg23/terrain/collision_105.inc"
};
static NJS_OBJECT collision_114 = {
#include "stg23/terrain/collision_114.inc"
};
static NJS_OBJECT collision_115 = {
#include "stg23/terrain/collision_115.inc"
};
static NJS_OBJECT collision_122 = {
#include "stg23/terrain/collision_122.inc"
};
static NJS_OBJECT collision_124 = {
#include "stg23/terrain/collision_124.inc"
};
static NJS_OBJECT collision_125 = {
#include "stg23/terrain/collision_125.inc"
};
static NJS_OBJECT collision_126 = {
#include "stg23/terrain/collision_126.inc"
};
static NJS_OBJECT collision_127 = {
#include "stg23/terrain/collision_127.inc"
};
static NJS_OBJECT collision_136 = {
#include "stg23/terrain/collision_136.inc"
};
static NJS_OBJECT collision_137 = {
#include "stg23/terrain/collision_137.inc"
};
static NJS_OBJECT collision_138 = {
#include "stg23/terrain/collision_138.inc"
};
static NJS_OBJECT collision_139 = {
#include "stg23/terrain/collision_139.inc"
};
static NJS_OBJECT collision_141 = {
#include "stg23/terrain/collision_141.inc"
};
static NJS_OBJECT collision_145 = {
#include "stg23/terrain/collision_145.inc"
};
static NJS_OBJECT collision_157 = {
#include "stg23/terrain/collision_157.inc"
};
