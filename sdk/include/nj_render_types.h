/* Ninja's angle, matrix, screen, camera, vertex buffer and texture types */
#ifndef NJ_RENDER_TYPES_H
#define NJ_RENDER_TYPES_H
#include <sg_xpt.h>

typedef Sint32 Angle; /* 65536 to a full turn */

/* a 4 by 4 matrix. it takes a point as the row (x, y, z, 1) on its left.
 * Ninja's current matrix lives in XMTRX, the FPU's back bank of registers */
#ifdef DREAMCAST
#ifdef NONMATCHING
/* an FMOV with FPSCR.SZ set moves 8 bytes, and faults off an 8-byte boundary.
 * SH4ZAM, this side's matrix library, moves a matrix that way. so this side's
 * matrix aligns to 8, where SHC's aligns to 4. -fpack-struct=4 caps a matrix
 * inside a structure at 4 again, as SHC does. a #pragma pack(8) around such a
 * structure lifts the cap, as ninja.h's does around NJS_LIGHT.
 * warn_if_not_aligned has GCC warn where a structure could leave a matrix off 8
 */
typedef Float NJS_MATRIX[16]
    __attribute__((aligned(8), warn_if_not_aligned(8)));
#else
typedef Float NJS_MATRIX[16];
#endif
#else
typedef Float NJS_MATRIX[16];
#endif
typedef struct {
    Float x, y;
} NJS_POINT2;
typedef struct {
    Float x, y, z;
} NJS_POINT3, NJS_VECTOR;

typedef struct {
    Float a, r, g, b;
} NJS_ARGB;

/* a screen, measured in pixels. dist is how far it lies from the eye, and
 * (cx, cy) is its center */
typedef struct {
    Float dist;
    Float w, h;
    Float cx, cy;
} NJS_SCREEN;

/* a camera at the place px to pz, looking along the direction vx to vz.
 * roll turns it about that direction. ang is its angle of view across the
 * screen's width. n_clip and f_clip place its near and far clip planes, as
 * negated distances. lx and ly are the vectors njPointCameraInterest
 * sets */
typedef struct {
    Float px, py, pz;
    Float vx, vy, vz;
    Angle roll;
    Angle ang;
    Float n_clip;
    Float f_clip;
    NJS_VECTOR lx, ly;
} NJS_CAMERA;

/* the buffer the chunk drawers' vertex functions write a model's vertices
 * to. each drawer takes it as an array of its own kind of vertex */
typedef struct nj_vertex_buf NJS_VERTEX_BUF;

/* a texture by its name. texaddr holds the global index njSetTextureName
 * gives it, until the texture loads and texaddr points to its
 * NJS_TEXMEMLIST */
typedef struct {
    void *filename;
    Uint32 attr;
    Uint32 texaddr;
} NJS_TEXNAME;
typedef struct {
    NJS_TEXNAME *textures;
    Uint32 nbTexture;
} NJS_TEXLIST;

/* a texture's surface in texture memory */
typedef struct {
    Uint32 Type;
    Uint32 BitDepth;
    Uint32 PixelFormat;
    Uint32 nWidth;
    Uint32 nHeight;
    Uint32 TextureSize;
    Uint32 fSurfaceFlags;
    Uint32 *pSurface;
    Uint32 *pVirtual;
    Uint32 *pPhysical;
} NJS_TEXSURFACE;
/* a PVR texture chunk's header: the texture's NJD_TEXFMT_ format, and its
 * width and height */
typedef struct {
    Uint32 nTextureType;
    Uint16 nWidth;
    Uint16 nHeight;
} NJS_PVRHEADERDATA;
/* where a texture's pixels lie, and its surface */
typedef struct {
    void *texaddr;
    NJS_TEXSURFACE texsurface;
} NJS_TEXINFO;
/* Ninja's record of a loaded texture */
typedef struct {
    Uint32 globalIndex;
    Uint32 bank;
    Uint32 tspparambuffer;
    Uint32 texparambuffer;
    Uint32 texaddr;
    NJS_TEXINFO texinfo;
    Uint16 count;
    /* the error code of the record's last load, negated, or 0 where the load
     * succeeded */
    Uint16 dummy;
} NJS_TEXMEMLIST;

/* a texture error. n and globalIndex name the texture at fault, and texerr
 * holds its NJD_TEXERR_ code. gdstat and gderr hold what gdFsGetStat and
 * gdFsGetErrStat said of the file */
typedef struct {
    Uint32 n;
    Uint32 globalIndex;
    Sint32 texerr;
    Sint32 gdstat;
    Sint32 gderr;
    Sint32 reserved0;
    Sint32 reserved1;
    Sint32 reserved2;
} NJS_TEXERRSTAT;

/* the bytes the texture path holds, its NUL among them */
#define TEXTURE_PATH_SIZE 256

/* a free record's globalIndex and bank */
#define TEXMEM_FREE 0xffffffff

#endif
