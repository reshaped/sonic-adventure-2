/* the simple chunk drawer's vertex and strip tables, which nj_cnk.h
 * describes */
#include <nj_cnk.h>
#include <nj_km.h>
#include <sg_xpt.h>

NJS_CNK_SIMPLE_VERTEX njCnkSimpleVertexSHV, njCnkSimpleVertexSHVN;
NJS_CNK_SIMPLE_VERTEX njCnkSimpleVertexV, njCnkSimpleVertexVD8;
NJS_CNK_SIMPLE_VERTEX njCnkSimpleVertexV1, njCnkSimpleVertexVS5;
/* VI and VNI serve NJD_CV_IN and NJD_CV_VN_IN chunks, and V1 and VN1 serve
 * NJD_CV_UF and NJD_CV_VN_UF. the SDK's names differ by I and 1 */
/* NOLINTNEXTLINE(misc-confusable-identifiers) */
NJS_CNK_SIMPLE_VERTEX njCnkSimpleVertexVS4, njCnkSimpleVertexVI;
NJS_CNK_SIMPLE_VERTEX njCnkSimpleVertexVN, njCnkSimpleVertexVND8;
NJS_CNK_SIMPLE_VERTEX njCnkSimpleVertexVN1, njCnkSimpleVertexVNS5;
/* NOLINTNEXTLINE(misc-confusable-identifiers) */
NJS_CNK_SIMPLE_VERTEX njCnkSimpleVertexVNS4, njCnkSimpleVertexVNI;
NJS_CNK_SIMPLE_VERTEX njCnkSimpleVertexVNX, njCnkSimpleVertexVNXD8;
NJS_CNK_SIMPLE_VERTEX njCnkSimpleVertexVNX1;

NJS_CNK_SIMPLE_INTEN_POLYGON njCnkSimplePolygon, njCnkSimplePolygonClip;
NJS_CNK_SIMPLE_INTEN_POLYGON njCnkSimplePolygonDb, njCnkSimplePolygonDbClip;
NJS_CNK_SIMPLE_INTEN_POLYGON njCnkSimplePolygonEnv, njCnkSimplePolygonEnvClip;
NJS_CNK_SIMPLE_INTEN_POLYGON njCnkSimplePolygonEnvDb,
    njCnkSimplePolygonEnvDbClip;
NJS_CNK_SIMPLE_POLYGON njCnkSimplePolygonD8, njCnkSimplePolygonD8Clip;
NJS_CNK_SIMPLE_POLYGON njCnkSimplePolygonD8Db, njCnkSimplePolygonD8DbClip;
NJS_CNK_SIMPLE_POLYGON njCnkSimplePolygonD8Env, njCnkSimplePolygonD8EnvClip;
NJS_CNK_SIMPLE_POLYGON njCnkSimplePolygonD8EnvDb, njCnkSimplePolygonD8EnvDbClip;

NJS_CNK_SIMPLE_INTEN_TEXTURE njCnkSimpleTexture, njCnkSimpleTextureClip;
NJS_CNK_SIMPLE_INTEN_TEXTURE njCnkSimpleTextureDb, njCnkSimpleTextureDbClip;
NJS_CNK_SIMPLE_INTEN_TEXTURE njCnkSimpleTextureEnv, njCnkSimpleTextureEnvClip;
NJS_CNK_SIMPLE_INTEN_TEXTURE njCnkSimpleTextureEnvDb,
    njCnkSimpleTextureEnvDbClip;
NJS_CNK_SIMPLE_INTEN_TEXTURE njCnkSimpleTextureBump, njCnkSimpleTextureBumpClip;
NJS_CNK_SIMPLE_TEXTURE njCnkSimpleTextureD8, njCnkSimpleTextureD8Clip;
NJS_CNK_SIMPLE_TEXTURE njCnkSimpleTextureD8Db, njCnkSimpleTextureD8DbClip;
NJS_CNK_SIMPLE_TEXTURE njCnkSimpleTextureD8Env, njCnkSimpleTextureD8EnvClip;
NJS_CNK_SIMPLE_TEXTURE njCnkSimpleTextureD8EnvDb, njCnkSimpleTextureD8EnvDbClip;

struct nj_cnk_simple_vertex _njCnkVertexFuncTableS[] = {
    {njCnkSimpleVertexSHV, FALSE},
    {njCnkSimpleVertexSHVN, TRUE},
    {njCnkSimpleVertexV, TRUE},
    {njCnkSimpleVertexVD8, FALSE},
    {njCnkSimpleVertexV1, TRUE},
    {njCnkSimpleVertexV1, TRUE},
    {njCnkSimpleVertexVS5, FALSE},
    {njCnkSimpleVertexVS4, FALSE},
    {njCnkSimpleVertexVI, FALSE},
    {njCnkSimpleVertexVN, TRUE},
    {njCnkSimpleVertexVND8, FALSE},
    {njCnkSimpleVertexVN1, TRUE},
    /* njCnkSimpleVertexVNNF takes the chunk's head as well. so the walker
     * calls it by name, and reads only this row's inten */
    /* NOLINTNEXTLINE(clang-diagnostic-cast-function-type-mismatch) */
    {(NJS_CNK_SIMPLE_VERTEX *)njCnkSimpleVertexVNNF, TRUE},
    {njCnkSimpleVertexVNS5, FALSE},
    {njCnkSimpleVertexVNS4, FALSE},
    {njCnkSimpleVertexVNI, FALSE},
    {njCnkSimpleVertexVNX, TRUE},
    {njCnkSimpleVertexVNXD8, FALSE},
    {njCnkSimpleVertexVNX1, TRUE}};

/* a row of the strip tables. env is GLOBAL_TEXTURE in the groups with
 * NJD_FST_ENV, and 0 elsewhere. environment mapping gives even an NJD_CS
 * strip a u and v. specular, GLOBAL_OFFSET or 0, goes into each textured
 * intensity mode */
#define STRIP_ROW(packed, inten, env, specular, polygon, d8, texture, tex_d8)  \
    {(inten) | ((env) ? GLOBAL_TEXTURE | (specular) : 0),                      \
     (packed) | (env),                                                         \
     polygon,                                                                  \
     d8,                                                                       \
     (inten) | GLOBAL_TEXTURE | (specular),                                    \
     (packed) | GLOBAL_TEXTURE,                                                \
     texture,                                                                  \
     tex_d8}

/* a row's index is its strip chunk's NJD_FST_ flags, as CNK_FST shifts them.
 * IL, IS and IA are the index's low three bits, and UA is the next. the
 * walker applies IL and IA through the color and ambient it passes. so only
 * IS changes a row, by dropping the specular offset */
#define LIGHT_FLAGS(highlight, plain)                                          \
    highlight, highlight, plain, plain, highlight, highlight, plain, plain

#define STRIP_LIGHTS(packed, inten, env, polygon, d8, texture, tex_d8)         \
    LIGHT_FLAGS(                                                               \
        STRIP_ROW(packed, inten, env, GLOBAL_OFFSET, polygon, d8, texture,     \
                  tex_d8),                                                     \
        STRIP_ROW(packed, inten, env, 0, polygon, d8, texture, tex_d8))

/* a group is sixteen rows: eight opaque, then eight with UA set, which take
 * the _TRANS modes. DB, FL and ENV, the three bits above UA, pick one of a
 * table's eight groups */
#define STRIP_GROUP(packed, inten, env, polygon, d8, texture, tex_d8)          \
    STRIP_LIGHTS(NJD_COLOR_MODE_##packed, NJD_COLOR_MODE_##inten, env,         \
                 polygon, d8, texture, tex_d8),                                \
        STRIP_LIGHTS(NJD_COLOR_MODE_##packed##_TRANS,                          \
                     NJD_COLOR_MODE_##inten##_TRANS, env, polygon, d8,         \
                     texture, tex_d8)

struct nj_cnk_simple_strip _njCnkFunctionS[] = {
    STRIP_GROUP(PACKED_NORMAL, INTENSITY_NORMAL, 0, njCnkSimplePolygon,
                njCnkSimplePolygonD8, njCnkSimpleTexture, njCnkSimpleTextureD8),
    STRIP_GROUP(PACKED_NORMAL, INTENSITY_NORMAL, 0, njCnkSimplePolygonDb,
                njCnkSimplePolygonD8Db, njCnkSimpleTextureDb,
                njCnkSimpleTextureD8Db),
    STRIP_GROUP(FLAT_PACKED_NORMAL, FLAT_INTENSITY_NORMAL, 0,
                njCnkSimplePolygon, njCnkSimplePolygonD8, njCnkSimpleTexture,
                njCnkSimpleTextureD8),
    STRIP_GROUP(FLAT_PACKED_NORMAL, FLAT_INTENSITY_NORMAL, 0,
                njCnkSimplePolygonDb, njCnkSimplePolygonD8Db,
                njCnkSimpleTextureDb, njCnkSimpleTextureD8Db),
    STRIP_GROUP(PACKED_NORMAL, INTENSITY_NORMAL, GLOBAL_TEXTURE,
                njCnkSimplePolygonEnv, njCnkSimplePolygonD8Env,
                njCnkSimpleTextureEnv, njCnkSimpleTextureD8Env),
    STRIP_GROUP(PACKED_NORMAL, INTENSITY_NORMAL, GLOBAL_TEXTURE,
                njCnkSimplePolygonEnvDb, njCnkSimplePolygonD8EnvDb,
                njCnkSimpleTextureEnvDb, njCnkSimpleTextureD8EnvDb),
    STRIP_GROUP(FLAT_PACKED_NORMAL, FLAT_INTENSITY_NORMAL, GLOBAL_TEXTURE,
                njCnkSimplePolygonEnv, njCnkSimplePolygonD8Env,
                njCnkSimpleTextureEnv, njCnkSimpleTextureD8Env),
    STRIP_GROUP(FLAT_PACKED_NORMAL, FLAT_INTENSITY_NORMAL, GLOBAL_TEXTURE,
                njCnkSimplePolygonEnvDb, njCnkSimplePolygonD8EnvDb,
                njCnkSimpleTextureEnvDb, njCnkSimpleTextureD8EnvDb)};

struct nj_cnk_simple_strip _njCnkFunctionClipS[] = {
    STRIP_GROUP(PACKED_NORMAL, INTENSITY_NORMAL, 0, njCnkSimplePolygonClip,
                njCnkSimplePolygonD8Clip, njCnkSimpleTextureClip,
                njCnkSimpleTextureD8Clip),
    STRIP_GROUP(PACKED_NORMAL, INTENSITY_NORMAL, 0, njCnkSimplePolygonDbClip,
                njCnkSimplePolygonD8DbClip, njCnkSimpleTextureDbClip,
                njCnkSimpleTextureD8DbClip),
    STRIP_GROUP(FLAT_PACKED_NORMAL, FLAT_INTENSITY_NORMAL, 0,
                njCnkSimplePolygonClip, njCnkSimplePolygonD8Clip,
                njCnkSimpleTextureClip, njCnkSimpleTextureD8Clip),
    STRIP_GROUP(FLAT_PACKED_NORMAL, FLAT_INTENSITY_NORMAL, 0,
                njCnkSimplePolygonDbClip, njCnkSimplePolygonD8DbClip,
                njCnkSimpleTextureDbClip, njCnkSimpleTextureD8DbClip),
    STRIP_GROUP(PACKED_NORMAL, INTENSITY_NORMAL, GLOBAL_TEXTURE,
                njCnkSimplePolygonEnvClip, njCnkSimplePolygonD8EnvClip,
                njCnkSimpleTextureEnvClip, njCnkSimpleTextureD8EnvClip),
    STRIP_GROUP(PACKED_NORMAL, INTENSITY_NORMAL, GLOBAL_TEXTURE,
                njCnkSimplePolygonEnvDbClip, njCnkSimplePolygonD8EnvDbClip,
                njCnkSimpleTextureEnvDbClip, njCnkSimpleTextureD8EnvDbClip),
    STRIP_GROUP(FLAT_PACKED_NORMAL, FLAT_INTENSITY_NORMAL, GLOBAL_TEXTURE,
                njCnkSimplePolygonEnvClip, njCnkSimplePolygonD8EnvClip,
                njCnkSimpleTextureEnvClip, njCnkSimpleTextureD8EnvClip),
    STRIP_GROUP(FLAT_PACKED_NORMAL, FLAT_INTENSITY_NORMAL, GLOBAL_TEXTURE,
                njCnkSimplePolygonEnvDbClip, njCnkSimplePolygonD8EnvDbClip,
                njCnkSimpleTextureEnvDbClip, njCnkSimpleTextureD8EnvDbClip)};

NJS_CNK_SIMPLE_INTEN_TEXTURE *_njCnkBumpFunctionS[] = {njCnkSimpleTextureBump};
NJS_CNK_SIMPLE_INTEN_TEXTURE *_njCnkBumpFunctionClipS[] = {
    njCnkSimpleTextureBumpClip};
