/* Ninja 3D library, as far as the sources use it */
#ifndef NINJA_H
#define NINJA_H
#include <nj_render_types.h> /* IWYU pragma: export */
#include <sg_xpt.h>

/* pi cut to six places, as NJM_RAD_ANG takes it */
#define NJD_PI 3.141592

/* the Angle of n radians */
#define NJM_RAD_ANG(n) ((Angle)(((n) * 65536.0) / (2 * NJD_PI)))
/* the Angle of n degrees */
#define NJM_DEG_ANG(n) ((Angle)(((n) * 65536.0) / 360.0))

/* a sphere: its center and radius */
typedef struct {
    NJS_POINT3 c;
    Float r;
} NJS_SPHERE;
/* 1 when the spheres overlap, else 0 */
Int njCollisionCheckSS(NJS_SPHERE *sphere1, NJS_SPHERE *sphere2);

/* a line through a point along a vector. the same record is a plane
 * through the point, normal to the vector */
typedef struct {
    Float px, py, pz;
    Float vx, vy, vz;
} NJS_LINE, NJS_PLANE;

/* a capsule: the centers of the spheres at its two ends, and their radius */
typedef struct {
    NJS_POINT3 c1;
    NJS_POINT3 c2;
    Float r;
} NJS_CAPSULE;
typedef struct {
    NJS_POINT3 v[8];
} NJS_BOX;
/* 1 where capsule meets box by the tests njCollisionCheckBC.c gives, else 0.
 * box's corners 0 to 3 must run around one face. corner i + 4 must share an
 * edge with corner i. (v[2] - v[0]) x (v[1] - v[0]) must point out of the
 * box */
Int njCollisionCheckBC(NJS_BOX *box, NJS_CAPSULE *capsule);
/* set *min and *max to the least and greatest x, y and z of hull's corners */
void njGetBoxMinMax(NJS_BOX *hull, NJS_POINT3 *min, NJS_POINT3 *max);
/* set *min and *max to the least and greatest x, y and z that hull's end
 * spheres reach */
void njGetCapsuleMinMax(NJS_CAPSULE *hull, NJS_POINT3 *min, NJS_POINT3 *max);
/* 1 when the boxes min_h1..max_h1 and min_h2..max_h2 overlap by more than 0
 * on every axis, else 0 */
Int njCheckBBox(NJS_POINT3 *min_h1, NJS_POINT3 *max_h1, NJS_POINT3 *min_h2,
                NJS_POINT3 *max_h2);

typedef struct {
    Sint16 u, v;
} NJS_TEX;
typedef struct {
    Uint8 b, g, r, a;
} NJS_BGRA;
/* a word that holds a color or a texture coordinate. as one number, a color
 * is a packed ARGB word */
typedef union {
    Uint32 color;
    NJS_TEX tex;
    NJS_BGRA argb;
} NJS_COLOR;
/* the shifts of a packed ARGB word's alpha, red and green bytes. its blue byte
 * is its lowest */
#define ARGB_ALPHA_SHIFT 24
#define ARGB_RED_SHIFT 16
#define ARGB_GREEN_SHIFT 8

typedef struct {
    NJS_COLOR diffuse;
    NJS_COLOR specular;
    Float exponent;
    Uint32 attr_texId;
    Uint32 attrflags;
} NJS_MATERIAL;
/* bits of a material's attrflags. NJD_SA_ sets the source's blend factor,
 * and NJD_DA_ the destination's. SRC is the source's alpha, and INV_SRC 1
 * less it. NJD_D_100 sets the mipmap D adjust to 1.0 */
#define NJD_SA_SRC 0x80000000
#define NJD_DA_INV_SRC 0x14000000
#define NJD_FLAG_IGNORE_LIGHT 0x02000000
#define NJD_FLAG_DOUBLE_SIDE 0x00800000
#define NJD_FLAG_IGNORE_SPECULAR 0x00080000
#define NJD_FLAG_USE_TEXTURE 0x00200000
#define NJD_FLAG_USE_ALPHA 0x00100000
#define NJD_FLAG_FLIP_U 0x00040000
#define NJD_FLAG_FLIP_V 0x00020000
#define NJD_FLAG_CLAMP_U 0x00010000
#define NJD_FLAG_CLAMP_V 0x00008000
#define NJD_FILTER_BILINEAR 0x00002000
#define NJD_D_100 0x00000400

/* a model's polygons of one type and material */
typedef struct {
    Uint16 type_matId;
    Uint16 nbMesh;
    Sint16 *meshes;
    Uint32 *attrs;
    NJS_VECTOR *normals;
    NJS_COLOR *vertcolor;
    NJS_COLOR *vertuv;
} NJS_MESHSET;
/* the kinds of mesh set, in type_matId's bits above the material's index. an
 * NJD_MESHSET_3 mesh is three indices, and an NJD_MESHSET_4 mesh four. an
 * NJD_MESHSET_N or NJD_MESHSET_TRIMESH mesh is a halfword that counts its
 * indices, then the indices. NJD_TRIMESH_END is a bit of a trimesh's count
 * halfword, above the count */
#define NJD_MESHSET_3 0x0000
#define NJD_MESHSET_4 0x4000
#define NJD_MESHSET_N 0x8000
#define NJD_MESHSET_TRIMESH 0xc000
#define NJD_MESHSET_MASK 0xc000
#define NJD_TRIMESH_END 0x8000

/* a basic model. center and r give the sphere that holds it */
typedef struct {
    NJS_POINT3 *points;
    NJS_VECTOR *normals;
    Sint32 nbPoint;
    NJS_MESHSET *meshsets;
    NJS_MATERIAL *mats;
    Uint16 nbMeshset;
    Uint16 nbMat;
    NJS_POINT3 center;
    Float r;
} NJS_MODEL;

/* a node of a model tree; pos, ang and scl place it in its parent */
typedef struct obj {
    Uint32 evalflags;
    NJS_MODEL *model;
    Float pos[3];
    Angle ang[3];
    Float scl[3];
    struct obj *child;
    struct obj *sibling;
} NJS_OBJECT;
/* bits of a node's evalflags. NJD_EVAL_UNIT_POS, NJD_EVAL_UNIT_ANG and
 * NJD_EVAL_UNIT_SCL leave pos, ang or scl out of the node's transform.
 * NJD_EVAL_HIDE hides the node's model, and NJD_EVAL_BREAK skips its
 * children. NJD_EVAL_ZXY_ANG turns the node about z, then x, then y */
#define NJD_EVAL_UNIT_POS 0x01
#define NJD_EVAL_UNIT_ANG 0x02
#define NJD_EVAL_UNIT_SCL 0x04
#define NJD_EVAL_HIDE 0x08
#define NJD_EVAL_BREAK 0x10
#define NJD_EVAL_ZXY_ANG 0x20
/* in a node's evalflags: a motion draw poses the node by its own pos, ang and
 * scl. the motion's mdata holds no entry for the node. its entries go in order
 * to the nodes without the flag */
#define NJD_EVAL_SKIP 0x40
/* in a node's evalflags: a draw that takes a shape motion moves none of the
 * node's vertices. the shape's mdata holds no entry for the node. its entries
 * go in order to the nodes without the flag */
#define NJD_EVAL_SHAPE_SKIP 0x80
/* in a node's evalflags: the chunk drawers' object draws skip the node's
 * children where they skip its model. under NJD_CONTROL_3D_MODEL_CLIP, they
 * skip a model that njCnkModelClip finds outside the view. a multi drawer also
 * skips a model whose vertex chunks it can't draw. a node with all three
 * NJD_EVAL_UNIT_ flags draws its children regardless. any other node with the
 * flag must not have NJD_EVAL_HIDE. the draws then test a value no model draw
 * set, as their sources' bug tags say */
#define NJD_EVAL_CLIP 0x100

/* a chunk model: its vertex and polygon chunk lists */
typedef struct {
    Sint32 *vlist;
    Sint16 *plist;
    NJS_POINT3 center;
    Float r;
} NJS_CNK_MODEL;

/* a chunk's type is the low byte of its first halfword. each kind of chunk
 * takes the types from its offset up to the next kind's. a bits chunk, below
 * NJD_TINYOFF, is one halfword, and a tiny chunk two. the material chunk
 * types start at NJD_MATOFF + 1 */
#define NJD_TINYOFF 8
#define NJD_MATOFF 16
#define NJD_VERTOFF 32
#define NJD_VOLOFF 56
#define NJD_STRIPOFF 64
/* the null chunk, a halfword the drawers pass over. it lets the chunk after
 * it start on a word */
#define NJD_CN 0
/* bits chunks. NJD_CB_BA sets a blend, from the NJD_FBS_ and NJD_FBD_ bits
 * above its type. NJD_CB_CP stores the rest of its polygon list, undrawn,
 * under the number in its flags. NJD_CB_DP draws the list stored under its
 * number */
#define NJD_CB_BA 1
#define NJD_CB_DA 2
#define NJD_CB_EXP 3
#define NJD_CB_CP 4
#define NJD_CB_DP 5
/* a blend's factors. NJD_FBS_ is the source's, in bits 11 to 13, and NJD_FBD_
 * the destination's, in bits 8 to 10. ONE is 1, SA the source's alpha, and
 * ISA 1 less it */
#define NJD_FBS_ONE 0x0800
#define NJD_FBD_ONE 0x0100
#define NJD_FBS_SA 0x2000
#define NJD_FBD_ISA 0x0500
/* the tiny chunk that sets a texture */
#define NJD_CT_TID 8
/* flags of an NJD_CT_TID chunk's first halfword. NJD_FFL_ mirrors the
 * texture along u or v, and NJD_FCL_ clamps it. NJD_FDA_MASK is the mipmap D
 * adjust's field, and NJD_FDA_100 and NJD_FDA_075 set it to 1.0 and 0.75 */
#define NJD_FFL_U 0x8000
#define NJD_FFL_V 0x4000
#define NJD_FCL_U 0x2000
#define NJD_FCL_V 0x1000
#define NJD_FDA_MASK 0x0f00
#define NJD_FDA_200 0x0800
#define NJD_FDA_100 0x0400
#define NJD_FDA_075 0x0300
#define NJD_FDA_025 0x0100
/* flags of an NJD_CT_TID chunk's second halfword, above the texture's index.
 * NJD_FFM_MASK is the filter mode's field, where NJD_FFM_BF filters
 * bilinearly. NJD_FSS super-samples the texture */
#define NJD_FFM_MASK 0xc000
#define NJD_FFM_BF 0x4000
#define NJD_FSS 0x2000
/* material chunks. each sets the colors its name gives: D a diffuse, A an
 * ambient and S a specular. NJD_CM_BU holds a bump texture's two vectors, as
 * six halfwords */
#define NJD_CM_D 17
#define NJD_CM_A 18
#define NJD_CM_DA 19
#define NJD_CM_S 20
#define NJD_CM_DS 21
#define NJD_CM_AS 22
#define NJD_CM_DAS 23
#define NJD_CM_BU 24
/* vertex chunks. a vertex is a point, then the parts its type's name gives
 * after CV. VN is a normal, and VNX a normal packed in a word. D8 is a packed
 * color, and UF a word of user flags. NF is a word whose low halfword indexes
 * the vertex buffer, and whose high halfword weights the vertex. S5 and S4 are
 * a diffuse and a specular color, a halfword each. IN is a diffuse and a
 * specular intensity, a halfword each. SH gives the point four floats, and
 * VN_SH gives the normal four as well */
#define NJD_CV_SH 32
#define NJD_CV_VN_SH 33
#define NJD_CV 34
#define NJD_CV_D8 35
#define NJD_CV_UF 36
#define NJD_CV_NF 37
#define NJD_CV_S5 38
#define NJD_CV_S4 39
#define NJD_CV_IN 40
#define NJD_CV_VN 41
#define NJD_CV_VN_D8 42
#define NJD_CV_VN_UF 43
#define NJD_CV_VN_NF 44
#define NJD_CV_VN_S5 45
#define NJD_CV_VN_S4 46
#define NJD_CV_VN_IN 47
#define NJD_CV_VNX 48
#define NJD_CV_VNX_D8 49
#define NJD_CV_VNX_UF 50
/* flags of a vertex chunk's first halfword. the chunk drawers count a
 * model's vertices, and those of them near the eye. a model whose every vertex
 * is near draws no strip. one with any near vertex draws by its polygon
 * functions' Clip forms. NJD_FV_CONT adds the chunk's vertices to the counts
 * so far. without it, the counts start again at the chunk.
 *
 * the NJD_CV_VN_NF chunks of a series weight the same vertices. the first
 * chunk takes NJD_FW_START, intermediate chunks take NJD_FW_MIDDLE, and the
 * last takes NJD_FW_END */
#define NJD_FV_CONT 0x8000
#define NJD_FW_START 0x0000
#define NJD_FW_MIDDLE 0x0100
#define NJD_FW_END 0x0200
/* a volume chunk of triangles, three vertex indices each, which
 * njCnkModDrawObject draws as a modifier volume */
#define NJD_CO_P3 56
/* strip chunks. NJD_CS's strips are indices alone. NJD_CS_UVN's give each
 * index a u and a v, in 256ths of the texture's size. NJD_CS_UVH's count u and
 * v in 1024ths */
#define NJD_CS 64
#define NJD_CS_UVN 65
#define NJD_CS_UVH 66
/* flags of a strip chunk's header, which lie above its type byte. the strip
 * ignores the light under IL, its specular part under IS, and its ambient
 * part under IA. UA puts the strip in the translucent list. DB marks the
 * strip double-sided. FL shades it flat, and ENV maps the environment onto
 * it */
#define NJD_FST_IL 0x00000100
#define NJD_FST_IS 0x00000200
#define NJD_FST_IA 0x00000400
#define NJD_FST_UA 0x00000800
#define NJD_FST_DB 0x00001000
#define NJD_FST_FL 0x00002000
#define NJD_FST_ENV 0x00004000
/* the NJD_FST_ flags lie from bit NJD_FST_SHIFT of a strip chunk's header up */
#define NJD_FST_SHIFT 8
/* the halfword after a strip chunk's size counts its strips below bit
 * NJD_UFO_SHIFT. the bits from NJD_UFO_SHIFT up count the user halfwords
 * after each vertex but a strip's first two */
#define NJD_UFO_SHIFT 14
/* the end of a chunk list */
#define NJD_CE 255

/* a node of a chunk model tree, laid out as NJS_OBJECT is */
typedef struct cnkobj {
    Uint32 evalflags;
    NJS_CNK_MODEL *model;
    Float pos[3];
    Angle ang[3];
    Float scl[3];
    struct cnkobj *child;
    struct cnkobj *sibling;
} NJS_CNK_OBJECT;

/* a motion of nbFrame frames. type holds the NJD_MTYPE_ bits of the factors
 * it moves. inp_fn holds n, the count of factors in each entry, and
 * NJD_MTYPE_MASK's bits. mdata holds an NJS_MDATAn entry for each node it
 * moves, in the order the motion draws walk them */
typedef struct {
    void *mdata;
    Uint32 nbFrame;
    Uint16 type;
    Uint16 inp_fn;
} NJS_MOTION;

/* An exported motion and the model tree it animates. A standalone motion can
 * leave the object null for the caller to supply. */
typedef struct {
    NJS_OBJECT *object;
    NJS_MOTION *motion;
} NJS_ACTION;
/* NJS_MOTION type bits: the factors the motion moves */
#define NJD_MTYPE_POS_0 0x1
#define NJD_MTYPE_ANG_1 0x2
#define NJD_MTYPE_SCL_2 0x4
#define NJD_MTYPE_VEC_3 0x8
#define NJD_MTYPE_VERT_4 0x10
#define NJD_MTYPE_NORM_5 0x20
#define NJD_MTYPE_TARGET_3 0x40
#define NJD_MTYPE_ROLL_6 0x80
#define NJD_MTYPE_ANGLE_7 0x100
/* NJS_MOTION type bits of a light's motion: its color, its intensity, and
 * its spot or point. a camera motion's entries may hold these factors too.
 * the camera calls pass over them */
#define NJD_MTYPE_RGB_8 0x200
#define NJD_MTYPE_INTENSITY_9 0x400
#define NJD_MTYPE_SPOT_10 0x800
#define NJD_MTYPE_POINT_10 0x1000
/* NJS_MOTION type bit: the motion turns its nodes by quaternions, where it
 * has no NJD_MTYPE_ANG_1 */
#define NJD_MTYPE_QUAT_1 0x2000
/* in an NJS_MOTION's inp_fn: any of these bits asks for the motion's keys to
 * be interpolated along a spline, not in a straight line */
#define NJD_MTYPE_MASK 0xc0

/* two motions, each at its own frame, for a draw that blends the first into
 * the second */
typedef struct {
    NJS_MOTION *motion[2];
    Float frame[2];
} NJS_MOTION_LINK;

/* a motion key that points to its data. an NJD_MTYPE_VERT_4 factor's keys
 * are of this kind, and so are an NJD_MTYPE_NORM_5 factor's */
typedef struct {
    Uint32 keyframe;
    void *key;
} NJS_MKEY_P;

/* a motion key of three floats. an NJD_MTYPE_POS_0 factor's key holds the
 * position at the frame keyframe */
typedef struct {
    Uint32 keyframe;
    Float key[3];
} NJS_MKEY_F;
/* a motion key of three angles. an NJD_MTYPE_ANG_1 factor's key holds the
 * turns about x, y and z at the frame keyframe */
typedef struct {
    Uint32 keyframe;
    Angle key[3];
} NJS_MKEY_A;
/* a motion key of a quaternion's four floats, in NJS_QUATERNION's order. an
 * NJD_MTYPE_QUAT_1 factor's key holds the turn at the frame keyframe */
typedef struct {
    Uint32 keyframe;
    Float key[4];
} NJS_MKEY_QUAT;
/* a motion key of one angle. a camera motion's NJD_MTYPE_ROLL_6 and
 * NJD_MTYPE_ANGLE_7 keys hold its roll and its angle of view */
typedef struct {
    Uint32 keyframe;
    Angle key;
} NJS_MKEY_A1;
/* an NJS_MDATAn is a node's entry in the mdata of a motion that moves n
 * factors. p[0] points to the keys of the lowest bit's factor, p[1] to the
 * next bit's, and so on. a p is null where the node has no keys of its
 * factor. nb[i] counts p[i]'s keys */
typedef struct {
    void *p[1];
    Uint32 nb[1];
} NJS_MDATA1;
typedef struct {
    void *p[2];
    Uint32 nb[2];
} NJS_MDATA2;
typedef struct {
    void *p[3];
    Uint32 nb[3];
} NJS_MDATA3;
typedef struct {
    void *p[4];
    Uint32 nb[4];
} NJS_MDATA4;
typedef struct {
    void *p[5];
    Uint32 nb[5];
} NJS_MDATA5;

/* a screen vertex of a polygon, for njDrawPolygon */
typedef struct {
    Float x, y, z;
    Uint32 col;
} NJS_POLYGON_VTX;

/* a screen vertex of a textured polygon, for njDrawTexture */
typedef struct {
    Float x, y, z;
    Float u, v;
    Uint32 col;
} NJS_TEXTURE_VTX;

/* a palette as njLoadPaletteFile reads it */
typedef struct {
    void *palette;
    Uint16 mode;
    Sint16 bank;
    Sint16 offset;
    Sint16 count;
} NJS_TEXPALETTE;

/* texerr's codes */
#define NJD_TEXERR_OTHER 1 /* an error of no other code */
#define NJD_TEXERR_FILEOPEN 2 /* a file that won't open or be sized */
#define NJD_TEXERR_HEADER 4 /* a texture header the loads can't take */
#define NJD_TEXERR_FILELOAD 5 /* a read that failed */
#define NJD_TEXERR_SURFACE 6 /* a surface that can't be made */
#define NJD_TEXERR_MAINMEMORY 7 /* a texture cache with no room */
#define NJD_TEXERR_TEXMEMLOAD 8 /* a copy to texture memory that failed */
#define NJD_TEXERR_GLOBALINDEX 9 /* a memory texture without a global index */

/* num points of two floats, with a color and a texture coordinate for
 * each */
typedef struct {
    NJS_POINT2 *p;
    NJS_COLOR *col;
    NJS_COLOR *tex;
    Uint32 num;
} NJS_POINT2COL;

/* the same for points of three floats */
typedef struct {
    NJS_POINT3 *p;
    NJS_COLOR *col;
    NJS_COLOR *tex;
    Uint32 num;
} NJS_POINT3COL;

/* the bits of a display mode. a mode is the sum of a signal, a scan and a
 * size, as the NJD_RESOLUTION_ modes are. NJD_DSP_HEIGHT_EXT stretches a PAL
 * display by an NJD_PALEXT_HEIGHT_RATIO_ value. NJD_DSP_ANTI renders each
 * frame twice as wide */
#define NJD_DSP_NTSC 0x0
#define NJD_DSP_VGA 0x1
#define NJD_DSP_PAL 0x2
#define NJD_DSP_NONINTER 0x00
#define NJD_DSP_INTER 0x04
#define NJD_DSP_PSINTER 0x08
#define NJD_DSP_WIDTH_640 0x010
#define NJD_DSP_HEIGHT_480 0x020
#define NJD_DSP_HEIGHT_EXT 0x040
#define NJD_DSP_640x480 0x030
#define NJD_DSP_640x480_EXT 0x070
#define NJD_DSP_ANTI 0x00010000
#define NJD_PALEXT_HEIGHT_RATIO_1_033 0x00000000
#define NJD_PALEXT_HEIGHT_RATIO_1_066 0x00020000
#define NJD_PALEXT_HEIGHT_RATIO_1_100 0x00040000
#define NJD_PALEXT_HEIGHT_RATIO_1_133 0x00060000
#define NJD_PALEXT_HEIGHT_RATIO_1_166 0x00080000
#define NJD_RESOLUTION_VGA (NJD_DSP_VGA | NJD_DSP_NONINTER | NJD_DSP_640x480)
#define NJD_RESOLUTION_640x480_NTSCNI                                          \
    (NJD_DSP_NTSC | NJD_DSP_PSINTER | NJD_DSP_640x480)
/* the frame buffer's formats */
#define NJD_FRAMEBUFFER_MODE_RGB565 0
#define NJD_FRAMEBUFFER_MODE_RGB555 1
#define NJD_FRAMEBUFFER_MODE_ARGB1555 3
#define NJD_FRAMEBUFFER_MODE_RGB888 4
#define NJD_FRAMEBUFFER_MODE_ARGB8888 5
/* set up the display, Kamui and Ninja. mode is a display mode, frame an
 * NJD_FRAMEBUFFER_MODE_ format, and count the vertical blanks to a frame. the
 * call fills the setup that njInitVertexBuffer then gives Kamui.
 *
 * _nj_screen_ takes its size from mode, and the rest of it resets. so do the
 * draw counts, the clips, the lights and the vertex contexts. so do
 * _nj_control_3d_flag_ and the motion callback, and context 0 becomes current.
 * a function that njSetVSyncFunction set stays on the vertical blank chain for
 * good. the call forgets its handle.
 *
 * where the system bus's revision is below 10, the call shows an orange
 * border. it then calls the code at the start of main memory, and returns only
 * if that code does */
void njInitSystem(Int mode, Int frame, Int count);
/* set the display up again, from arguments as njInitSystem takes them. the
 * call stops the display first, and leaves _nj_display_mode_ as it was.
 * unlike njInitSystem, it neither waits for a vertical blank nor starts
 * Kamui's device. it registers Ninja's render callbacks again, and resets
 * the draw counts, _nj_screen_'s center and the 2D clip. Kamui takes the
 * texture memory's size only at the next njInitVertexBuffer or
 * njChangeTextureMemorySize */
void njChangeSystem(Int mode, Int frame, Int count);
/* end the frame: render what it sent through kmRender, show it through
 * kmFlipFrameBuffer, and update the controllers. the call first waits for the
 * last render to end. it skips the wait until a render has ended since
 * njInitSystem or njChangeSystem. njSetEORFunction's function runs only where
 * the render ended within the wait's limit */
void njWaitVSync(void);
/* discard Kamui's vertex buffer, and free it through syFree unless
 * njSetVertexBuffer gave it. a buffer that njInitVertexBuffer took from
 * syMalloc before a njSetVertexBuffer call is never freed */
void njExitSystem(void);
/* call func at each vertical blank, in place of the function the last call
 * set */
void njSetVSyncFunction(void (*func)(void));
/* call func as each render ends, after Ninja's own work there. a null func
 * ends the calls */
void njSetEORCallBackFunction(void (*func)(void));
/* render the polygons the frame has sent, through kmRender */
void njRenderStart(void);
/* show the frame the last render drew, through kmFlipFrameBuffer */
void njFlipFrameBuffer(void);
/* call func where njWaitVSync finds the last render ended in time, before
 * it starts the next. a null func ends the calls */
void njSetEORFunction(void (*func)(void));
/* make func, with arg, Kamui's wait vsync callback */
void njSetWaitVSyncFunction(void (*func)(void *), void *arg);
/* make count the vertical blanks to a frame, as njInitSystem's count is */
void njSetWaitVsyncCount(Int count);
/* set the texture memory's size in bytes. njInitSystem and njChangeSystem
 * take it into the setup that njInitVertexBuffer gives Kamui, so call it
 * before them */
void njSetTextureMemorySize(Uint32 size);
/* set the texture memory's size to size bytes, as njSetTextureMemorySize
 * does. then give Kamui the setup again, and measure the free texture
 * memory */
void njChangeTextureMemorySize(Uint32 size);
/* make the size bytes at buffer the vertex buffer njInitVertexBuffer gives
 * Kamui, in place of one from syMalloc. Kamui takes it by its P2 address.
 * njInitVertexBuffer needs 4 bytes for each word its lists take, plus 88
 * words. where size is below that, it gives Kamui no buffer */
void njSetVertexBuffer(Uint32 *buffer, Sint32 size);
/* give Kamui its setup with a vertex buffer for the five lists, and measure
 * the free texture memory. op and om are the words of the opaque list and its
 * modifier list. tp and tm are those of the translucent list and its modifier
 * list, and pt the punch-through list's. each list takes its words' share of
 * the buffer, as a percent.
 *
 * at most one of op, tp and pt may be negative, which gives its list no
 * buffer. Kamui then runs with its 2V latency, and that list counts as 100
 * words toward the shares.
 *
 * the buffer is the one njSetVertexBuffer gave. without one, the call frees
 * the last buffer it took from syMalloc and takes another. the call masks
 * every interrupt while it waits for scan line 0 and gives Kamui the setup */
void njInitVertexBuffer(Sint32 op, Sint32 om, Sint32 tp, Sint32 tm, Sint32 pt);

/* stop showing the frame buffer, through kmStopDisplayFrameBuffer */
void njStopDisplay(void);
/* move the picture by xadjust pixels across and yadjust lines down */
void njAdjustDisplay(Sint32 xadjust, Sint32 yadjust);
/* show color, a packed color, as the display's border */
void njSetBorderColor(Uint32 color);
/* give the background plane's corners packed colors. the screen's top left
 * corner takes col1, its top right col2, and its bottom left col3 */
void njSetBackColor(Uint32 col1, Uint32 col2, Uint32 col3);
/* Ninja's version, 0x01100114 */
Uint32 njVersion(void);
/* a copy of a vertex context, as njGetSystemAttr takes it */
#define NJD_SYS_ATTR_SIZE 192
typedef Sint8 NJS_SYS_ATTR[NJD_SYS_ATTR_SIZE];
/* njGetSystemAttr copies the current vertex context to attr, and saves the
 * current texture and its bank. njSetSystemAttr copies attr to the current
 * context and makes it current. it also restores the texture and the bank
 * the last njGetSystemAttr saved */
void njGetSystemAttr(NJS_SYS_ATTR *attr);
void njSetSystemAttr(NJS_SYS_ATTR *attr);
/* how many lists Kamui's vertex buffer holds, Kamui's KMI_LISTS */
#define VERTEX_LISTS 5
/* what njGetSystemInfo reports. NativeLimit is the limit of Kamui's first
 * parameter bank, and NativeCurrent the last address Kamui reports for it.
 * NativePeak is the greatest NativeCurrent the calls have seen. each array
 * holds a size for each list, by list number. the vertex buffer sizes are
 * Kamui's. CurrentVertexBufSize holds the bytes each list has taken since
 * njWaitVSync or njFlipFrameBuffer last returned. the times are the last
 * vertex transfer's and the last render's, in microseconds */
typedef struct {
    Uint32 NativeLimit;
    Uint32 NativeCurrent;
    Uint32 NativePeak;
    Uint32 InternalVertexBufSize[VERTEX_LISTS];
    Uint32 MaxVertexBufSize[VERTEX_LISTS];
    Uint32 LastTATime;
    Uint32 LastRenderTime;
    Uint32 CurrentVertexBufSize[VERTEX_LISTS];
    Uint32 reserved[12];
} NJS_SYSTEMINFO;
/* fill info from Kamui's state. info->NativePeak must hold the last call's
 * peak, or 0 */
void njGetSystemInfo(NJS_SYSTEMINFO *info);
/* njUserClipping's modes: no clipping, and drawing only inside or only
 * outside its tiles */
#define NJD_CLIP_DISABLE 0
#define NJD_CLIP_INSIDE 2
#define NJD_CLIP_OUTSIDE 3
/* clip later draws by mode to the tiles from p[0] to p[1]. a tile is 32
 * pixels a side */
void njUserClipping(Int mode, NJS_POINT2 *p);

/* in a texture name's attr: the texture lies in memory. the name's filename
 * holds the texture's address, not a file's name */
#define NJD_TEXATTR_TYPE_MEMORY 0x40000000
/* in a texture name's attr: the name's texaddr holds the global index the
 * texture loads under */
#define NJD_TEXATTR_GLOBALINDEX 0x00800000
/* in a texture name's attr: an ABGR texture loads as ABGR_MM, with
 * mipmaps */
#define NJD_TEXATTR_AUTOMIPMAP 0x00400000
/* where a texture name's attr says its texture loads. it loads into texture
 * memory alone by default. NJD_TEXATTR_CACHE loads it into the texture cache
 * alone, and NJD_TEXATTR_BOTH into both */
#define NJD_TEXATTR_CACHE 0x80000000
#define NJD_TEXATTR_BOTH 0x20000000
#define NJD_TEXATTR_READAREA_MASK (NJD_TEXATTR_CACHE | NJD_TEXATTR_BOTH)
/* a name's NJD_TEXATTR_READAREA_MASK bits where its texture loads into
 * texture memory alone */
#define READAREA_TEXMEM 0
/* in a texture name's attr: the name's texaddr holds a palette bank and a
 * global index. the bank lies in its NJD_TEXBANK_MASK bits, and the index in
 * the bits below */
#define NJD_TEXATTR_PALGLOBALINDEX 0x01000000
#define NJD_TEXBANK_MASK 0xfc000000
#define NJD_TEXBANK_SHIFT 26
#define NJD_TEXPALGLOBAL_MASK 0x03ffffff
/* in a texture name's attr: the texture is the one the name before it
 * loaded. the name's texaddr holds the bank it takes */
#define NJD_TEXATTR_TEXCONTINUE 0x00010000
/* describe the pixels at tex in info. info->texaddr becomes tex, so info
 * points at the pixels rather than a copy of them. info->texsurface's nWidth
 * and nHeight become width and height. info->texsurface.Type becomes type <<
 * TEXSURFACE_FORMAT_SHIFT, where type is an NJD_TEXFMT_ format */
void njSetTextureInfo(NJS_TEXINFO *info, Uint16 *tex, Sint32 type, Sint32 width,
                      Sint32 height);
/* read the chunks of the PVR texture at pTexture, from its first chunk to
 * its PVRT chunk. pvrh takes that chunk's header. a GBIX chunk before it gives
 * the global index stored in *globalIndex, which is otherwise ~0. the call
 * returns the address of the texture's pixels. it returns NULL at a chunk of
 * any other name */
Uint32 *njLoadPvrHeader(NJS_PVRHEADERDATA *pvrh, Uint32 *globalIndex,
                        Uint32 *pTexture);
/* name a texture in texname: its filename becomes addr, its attr attr, and
 * its texaddr globalIndex. addr is the texture's address under
 * NJD_TEXATTR_TYPE_MEMORY, and a file's name otherwise */
void njSetTextureName(NJS_TEXNAME *texname, void *addr, Uint32 globalIndex,
                      Uint32 attr);
/* reset _nj_texerr_, load each texture texlist names in turn, and return 1.
 * it returns -1 for an empty list, or where a texture fails to load. that
 * texture's index then goes in _nj_texerr_.n, and the rest stay unloaded */
Sint32 njLoadTexture(NJS_TEXLIST *texlist);
/* release each texture texlist names, and return 1. where a texture fails to
 * release, the call returns -1 instead, and still goes on to the rest.
 * _nj_texerr_.n then holds the last failed texture's index */
Sint32 njReleaseTexture(NJS_TEXLIST *texlist);
/* load the texture of the first record with globalIndex again, as
 * njReLoadTextureLN loads one. texaddr and attr give the texture as a name's
 * filename and attr do. the record may be loaded or cached. where no record
 * has globalIndex, the call returns -1 with texerr NJD_TEXERR_OTHER.
 * otherwise it resets _nj_texerr_ and returns the load's result. on a
 * negative return, _nj_texerr_.globalIndex holds globalIndex */
Sint32 njReLoadTextureNumG(Uint32 globalIndex, void *texaddr, Uint32 attr,
                           Uint32 lod);
/* reset _nj_texerr_, and load texture n of the current list again. its
 * record loads as njReLoadTextureLN loads one. the call returns the load's
 * result. where that is negative, _nj_texerr_.n holds n */
Sint32 njReLoadTextureNum(Uint32 n, void *texaddr, Uint32 attr, Uint32 lod);
/* make texlist the list that later draws take their textures from, and
 * return 1. a null texlist returns -1 and changes nothing */
Sint32 njSetTexture(NJS_TEXLIST *texlist);
/* make texture n of the current list the texture later draws take, and
 * return 1. the portable build returns -1 and preserves the current material
 * if the list, slot or texture handle is unavailable. the original expects a
 * loaded slot. its texaddr points to the NJS_TEXMEMLIST that supplies the
 * current vertex context's texture parameters.
 * _nj_current_texture, _nj_current_texture_bank and _nj_texglobal_color_ take
 * the record's globalIndex, bank and format. a record whose globalIndex and
 * bank are current already changes nothing */
Sint32 njSetTextureNum(Uint32 n);
/* return _nj_current_texture: the global index of the current texture */
Sint32 njGetTextureNumG(void);
/* njTextureFilterMode's modes: the nearest texel, the four nearest, or a
 * trilinear filter's first pass */
#define NJD_TEXTUREFILTER_POINT_SAMPLE 0
#define NJD_TEXTUREFILTER_BILINEAR 1
#define NJD_TEXTUREFILTER_TRILINEAR 2
/* set the current vertex context's texture filter to mode, an
 * NJD_TEXTUREFILTER_ value. the call then sets the context as njZBufferMode
 * does */
void njTextureFilterMode(Int mode);
/* njZBufferMode's modes, as Kamui's depth compares. ZBUFFER_OFF is
 * KM_ALWAYS: every polygon draws whatever its depth. ZBUFFER_ON is
 * KM_GREATEREQUAL: a polygon draws only where it lies no farther than what
 * the depth buffer holds */
#define ZBUFFER_OFF 0
#define ZBUFFER_ON 1
/* set the current vertex context's depth compare to mode, a ZBUFFER_ value.
 * another mode leaves the compare as it was. the call then sets the context.
 * kmProcessVertexRenderState packs it, and kmSetVertexRenderState makes
 * polygons draw with it */
void njZBufferMode(Int mode);
/* njTextureShadingMode's modes: how the texture and the polygon's color
 * combine */
#define NJD_TEX_SHADING_MODE_MODULATE 0
#define NJD_TEX_SHADING_MODE_DECALALPHA 1
#define NJD_TEX_SHADING_MODE_MODULATEALPHA 2
#define NJD_TEX_SHADING_MODE_DECAL 3
/* njTextureClampMode's modes clamp the texture along no axis, v, u or both.
 * njTextureFlipMode's mirror it along the same axes */
#define NJD_TEXTURECLAMP_NOCLAMP 0
#define NJD_TEXTURECLAMP_CLAMP_V 1
#define NJD_TEXTURECLAMP_CLAMP_U 2
#define NJD_TEXTURECLAMP_CLAMP_UV 3
#define NJD_TEXTUREFLIP_NOFLIP 0
#define NJD_TEXTUREFLIP_FLIP_V 1
#define NJD_TEXTUREFLIP_FLIP_U 2
#define NJD_TEXTUREFLIP_FLIP_UV 3
/* each call below sets one field of the current vertex context from its
 * argument. it then sets the context as njZBufferMode does. a value the call
 * doesn't take leaves the field as it was. njMipmapAdjust takes any level, the
 * mipmap D adjust in quarters. the others take:
 * - njTextureShadingMode, an NJD_TEX_SHADING_MODE_ value;
 * - njTextureClampMode, an NJD_TEXTURECLAMP_ value;
 * - njTextureFlipMode, an NJD_TEXTUREFLIP_ value;
 * - njPolygonCullingMode, KM_NOCULLING or a KM_CULL value of kamui.h;
 * - each other call, TRUE or FALSE, to turn its state on or off.
 * njSpecularMode's state is the offset color, and njAlphaMode's the vertices'
 * alpha. njIgnoreTextureAlphaMode's is ignoring the texture's alpha.
 * njColorClampMode's is the color clamp, and njSuperSampleMode's
 * super-sampling */
void njMipmapAdjust(Int level);
void njSpecularMode(Int mode);
void njAlphaMode(Int mode);
void njIgnoreTextureAlphaMode(Int mode);
void njColorClampMode(Int mode);
void njTextureShadingMode(Int mode);
void njSuperSampleMode(Int mode);
void njTextureClampMode(Int mode);
void njTextureFlipMode(Int mode);
void njPolygonCullingMode(Int mode);
/* set the color clamp's top to MaxARGB and its bottom to MinARGB, packed
 * colors */
void njSetColorClamp(Uint32 MaxARGB, Uint32 MinARGB);
/* set the cheap shadow's intensity to mode, through kmSetCheapShadowMode */
void njSetCheapShadowMode(Int mode);
/* give the hardware's culling register size, through kmSetCullingRegister */
void njPolygonCullingSize(Float size);
/* the bytes njSetPvmTextureList gives each name */
#define PVM_NAME_SIZE 28
/* point texlist's textures at texname. for each of the size names, name i's
 * filename becomes filename + i * PVM_NAME_SIZE. nbTexture is left as it
 * was */
void njSetPvmTextureList(NJS_TEXLIST *texlist, NJS_TEXNAME *texname,
                         char *filename, Sint32 size);
/* load the textures of a PVM archive into texlist's names, and return 1.
 * njLoadTexturePvmFile reads the archive from the file that njCheckFileName
 * names by filename and .PVM. njLoadTexturePvmMemory reads it at addr.
 *
 * the archive's PVMH chunk holds an entry for each texture, numbered 0 to the
 * count less 1. nbTexture takes the count, and name i takes entry i. the
 * entry's number goes in the low half of the name's attr. the entry's name,
 * where it has one, goes in the name's filename, and its global index in
 * texaddr. a name must not set both NJD_TEXATTR_CACHE and NJD_TEXATTR_BOTH. a
 * PVPL chunk ahead of the first texture loads the palette. each texture then
 * loads into the name its number gives, as njLoadTextureLN loads one.
 *
 * a texture loads under the global index its name's texaddr holds. a
 * palettized texture, or one whose name has NJD_TEXATTR_PALGLOBALINDEX, takes
 * only texaddr's NJD_TEXPALGLOBAL_MASK bits as the index. a palettized
 * texture's bank is texaddr's NJD_TEXBANK_MASK bits.
 *
 * each call first resets _nj_texerr_. it returns -1 where the archive opens
 * with no PVMH chunk, or where a texture fails to load. the textures before
 * the failure stay loaded, and _nj_texerr_.n holds the failed name's index.
 * njLoadTexturePvmFile also returns -1 where the file won't open or a read
 * fails. the bug tags in njLoadTexturePvmFile.c give the cases each call gets
 * wrong */
Sint32 njLoadTexturePvmMemory(Uint8 *addr, NJS_TEXLIST *texlist);
Sint32 njLoadTexturePvmFile(const char *filename, NJS_TEXLIST *texlist);
/* a texture format holds a color format in its NJD_TEXFMT_COLOR_MASK byte.
 * its NJD_TEXFMT_TYPE_MASK byte holds a texture type. a TWIDDLED
 * texture's texels lie in the order the hardware reads them. a RECTANGLE
 * texture's lie row after row, and its sides may differ. a type whose name
 * holds MM holds its mipmaps too. a VQ or SMALLVQ texture holds indices into
 * a codebook */
#define NJD_TEXFMT_ARGB_1555 0x00
#define NJD_TEXFMT_RGB_565 0x01
#define NJD_TEXFMT_ARGB_4444 0x02
#define NJD_TEXFMT_YUV_422 0x03
#define NJD_TEXFMT_BUMP 0x04
#define NJD_TEXFMT_ARGB_8888 0x06
#define NJD_TEXFMT_COLOR_MASK 0x00ff
#define NJD_TEXFMT_TWIDDLED 0x0100
#define NJD_TEXFMT_TWIDDLED_MM 0x0200
#define NJD_TEXFMT_VQ 0x0300
#define NJD_TEXFMT_VQ_MM 0x0400
#define NJD_TEXFMT_PALETTIZE4 0x0500
#define NJD_TEXFMT_PALETTIZE4_MM 0x0600
#define NJD_TEXFMT_PALETTIZE8 0x0700
#define NJD_TEXFMT_PALETTIZE8_MM 0x0800
#define NJD_TEXFMT_RECTANGLE 0x0900
#define NJD_TEXFMT_STRIDE 0x0b00
#define NJD_TEXFMT_TWIDDLED_RECTANGLE 0x0d00
#define NJD_TEXFMT_ABGR 0x0e00
#define NJD_TEXFMT_ABGR_MM 0x0f00
#define NJD_TEXFMT_SMALLVQ 0x1000
#define NJD_TEXFMT_SMALLVQ_MM 0x1100
#define NJD_TEXFMT_TWIDDLED_MM_DMA 0x1200
#define NJD_TEXFMT_TYPE_MASK 0xff00
/* NJS_TEXSURFACE's Type holds an NJD_TEXFMT_ format from this bit up */
#define TEXSURFACE_FORMAT_SHIFT 16

/* the index of the record with globalIndex among the n at list. it returns
 * -1 where none has it, or where list is null */
Sint32 njSearchTexList(NJS_TEXMEMLIST *list, Uint32 n, Uint32 globalIndex);
/* reset _nj_texerr_: n and globalIndex become ~0, and texerr, gdstat and
 * gderr 0 */
void njInitTextureErrStat(void);
/* return _nj_texerr_'s address */
NJS_TEXERRSTAT *njGetTextureErrStat(void);
/* make the n records at addr Ninja's texture records, as njInitTexMemList
 * does. the call also empties the texture path */
void njInitTexture(NJS_TEXMEMLIST *addr, Uint32 n);
/* make addr the buffer that the texture loads read files through. size is
 * its bytes, of which the loads use whole sectors of GDD_FS_SCTSIZE */
void njInitTextureBuffer(Sint8 *addr, Uint32 size);
/* make globalIndex the index from which a texture file without one of its
 * own takes one. a globalIndex of 0xfffffff0 or more counts as 0xffffffef.
 * such a file takes the highest index that no texture record holds. it
 * looks at or below the index the last such file took. so an index above that
 * one, once released, isn't taken again */
void njInitTextureGlobalIndex(Uint32 globalIndex);
/* release every texture as njReleaseTextureAll does, then every cached copy
 * as njReleaseCacheTextureAll does */
void njExitTexture(void);
/* make the size bytes at addr the texture cache, all one free block */
void njInitCacheTextureBuffer(Sint8 *addr, Uint32 size);
/* release texture n of the current list as njReleaseTextureLN releases a
 * record, and return its result. where that is -1, _nj_texerr_.n takes n */
Sint32 njReleaseTextureNum(Uint32 n);
/* free the texture memory of every record with TEXMEM_LOADED, and clear the
 * flag. a palettized record's surface is shared by every record of its
 * global index. those records lose TEXMEM_LOADED with it, or are freed
 * where the record isn't cached. a record left with no copy of its texture
 * is freed. _nj_current_texture becomes -1, so the next njSetTextureNum
 * sets its texture */
void njReleaseTextureAll(void);
/* free the cached copy of every record with TEXMEM_CACHED, and clear the
 * flag. the records of a palettized texture's global index share one copy.
 * each of them loses the flag with it. a record left with no copy of its
 * texture is freed */
void njReleaseCacheTextureAll(void);
/* njCalcTexture's flags */
#define NJD_TEXMEM_FREESIZE 0
#define NJD_TEXMEM_MAXBLOCK 1
#define NJD_TEXMEM_MAXSIZE 2
/* for NJD_TEXMEM_MAXSIZE, return _nj_tex_max_free_size. otherwise ask
 * kmGetFreeTextureMem: NJD_TEXMEM_FREESIZE returns the free texture memory,
 * and NJD_TEXMEM_MAXBLOCK, or any other flag, the largest free block */
Uint32 njCalcTexture(Uint32 flag);
/* make path the text that each texture file's name is appended to. path
 * must fit TEXTURE_PATH_SIZE bytes with its NUL */
void njSetTexturePath(const char *path);
/* gather the free texture memory into one block. then njSetTextureParam
 * sets the parameter words of each loaded record of the n at addr again. a
 * record with TEXMEM_SHARED_SURFACE first gives its pSurface to every
 * record of its global index */
void njGarbageTexture(NJS_TEXMEMLIST *addr, Uint32 n);
/* the bytes in texture memory of a texture of nTextureType, an NJD_TEXFMT_
 * format, and nWidth by nHeight texels. it returns 0 for a type it doesn't
 * know */
Uint32 njCalcTextureSize(Uint32 nWidth, Uint32 nHeight, Uint32 nTextureType);
/* return 1 once the last transfer to texture memory has ended, else -1 */
Sint32 njLoadTextureFinishDMA(void);
/* set tex's tspparambuffer and texparambuffer, the TSP and texture words it
 * draws with, from its surface. each side must be a power of two from 8 to
 * 1024. a palettized texture's texture word then takes tex's bank, as
 * njSetPaletteBankNum describes */
void njSetTextureParam(NJS_TEXMEMLIST *tex);
/* open filename, and start njTexFread's reads at its first byte. it returns
 * 0, or -2 with _nj_texerr_.texerr set to NJD_TEXERR_FILEOPEN if the file
 * can't be opened or sized. a file that can't be sized stays open */
Sint32 njTexFopen(const char *filename);
/* close the file njTexFopen opened */
void njTexFclose(void);
/* read size bytes into buf from the next byte to read, and move past them.
 * it returns 1. it returns -1 and sets _nj_texerr_ where size is 0 or the
 * read would pass the file's end. it does the same where a seek or a read
 * fails. bytes copied into buf before such a failure stay there. the next
 * byte to read is then the first byte not copied */
Sint32 njTexFread(void *buf, Uint32 size);
/* move the next byte to read, by type. GDD_SEEK_SET puts it at offset, and
 * GDD_SEEK_CUR moves it by offset. GDD_SEEK_END puts it offset back from the
 * file's end. another type leaves it */
void njTexFseek(Sint32 offset, Sint32 type);
/* the bits of a palette color: 32 for NJD_TEXFMT_ARGB_8888, and 16 for any
 * other mode */
#define NJD_TEXPALETTE_16BPP 16
#define NJD_TEXPALETTE_32BPP 32
/* set the palette bank a palettized texture draws with. bank counts banks of
 * 16 colors. a 4-bit texture takes bank's low six bits. an 8-bit texture's
 * banks are 256 colors, so it takes only bank's bits 4 and 5. the texture's
 * record keeps all of bank. a texture that isn't palettized is left alone.
 *
 * njSetPaletteBankNum sets texture n of the current list. texture n must be
 * loaded, and n isn't checked against the list's count. njSetPaletteBankNumG
 * sets Ninja's record with globalIndex. a globalIndex that no record has
 * changes nothing */
void njSetPaletteBankNum(Uint32 n, Sint32 bank);
void njSetPaletteBankNumG(Uint32 globalIndex, Sint32 bank);
/* set the palette's color format to mode, an NJD_TEXFMT_ value. a mode other
 * than NJD_TEXFMT_RGB_565, NJD_TEXFMT_ARGB_4444 or NJD_TEXFMT_ARGB_8888 sets
 * ARGB 1555 */
void njSetPaletteMode(Uint32 mode);
/* the mode njSetPaletteMode was last given, whatever format it set */
Uint32 njGetPaletteMode(void);
/* njSetPaletteData copies count colors from data into the palette, from
 * entry offset on. njGetPaletteData copies them from the palette into data.
 * while njSetPaletteMode last set NJD_TEXFMT_ARGB_8888, data holds a Uint32
 * for each color, a whole palette word. under any other mode data holds a
 * Uint16 for each color, the low half of a palette word. njSetPaletteData
 * then writes 0 to the word's high half. under NJD_TEXFMT_ARGB_8888 count
 * must be above 0: a count of 0 there copies 2^32 words */
void njSetPaletteData(Sint32 offset, Sint32 count, void *data);
void njGetPaletteData(Sint32 offset, Sint32 count, void *data);
/* read the PVPL file filename into pl, and return 1. the file's colors go to
 * pl->palette, which must have room for them all. they reach the palette only
 * when njSetPaletteData writes them there. pl takes the file's bank, offset and
 * count, and its mode's low byte. the file's bank becomes the current palette
 * bank before the colors are read. a failure to read the colors leaves the
 * file's bank current. the call returns -1 where the file can't be opened or
 * read, or isn't a PVPL file */
Sint32 njLoadPaletteFile(const char *filename, NJS_TEXPALETTE *pl);

/* the njMemCopy calls copy num units from src to dst. a num of 0 copies 2^32
 * units. njMemCopy's unit is a byte, njMemCopy2's a halfword and njMemCopy4's a
 * word. njMemCopy8's, njMemCopy16's and njMemCopy32's are blocks of that many
 * bytes, and njMemCopySQ's a block of 32. the block copies move pairs of
 * floats, so their src and dst must be multiples of 8.
 *
 * njMemCopy32 and njMemCopySQ need a dst that is a multiple of 32 too.
 * njMemCopy32 takes dst's cache lines without reading them. njMemCopySQ
 * copies through the store queues. it sets QACR0 and QACR1 once, to dst's
 * 64 MB area. so the blocks must all lie in that area. the two registers keep
 * it after the call */
void njMemCopy(void *dst, const void *src, Sint32 num);
void njMemCopy2(void *dst, const void *src, Sint32 num);
void njMemCopy4(void *dst, const void *src, Sint32 num);
void njMemCopy32(void *dst, const void *src, Sint32 num);
void njMemCopySQ(void *dst, const void *src, Sint32 num);
void njMemCopy16(void *dst, const void *src, Sint32 num);
void njMemCopy8(void *dst, const void *src, Sint32 num);

/* attributes of njDrawPolygon2D, njDrawLine3D and njDrawTriangle3D */
#define NJD_FILL 0x00000020
#define NJD_TRANSPARENT 0x00000040
#define NJD_USE_TEXTURE 0x80000000
/* draw a polygon of n corners on the screen at priority pri. corner i lies at
 * p->p[i], in the packed color p->col[i]. NJD_USE_TEXTURE textures the
 * polygon with the current vertex context's texture. corner i then takes
 * p->tex[i] too, whose u and v count 256 to the texture's side.
 * NJD_TRANSPARENT makes the polygon translucent.
 *
 * NJD_FILL fills the polygon, and n must then be from 1 to 32. an n of 1 or 2
 * fills nothing. the fill is one strip. its corners run 0, n - 1, 1, n - 2 and
 * so on. that order fills a convex polygon exactly.
 *
 * without NJD_FILL the polygon is outlined, its edges about a pixel wide. n
 * must then be 2 or more.
 *
 * a pri below -1 gives every corner the depth -1 / pri, as for a distance of
 * -pri. any other pri gives the depth 1 */
void njDrawPolygon2D(NJS_POINT2COL *p, Int n, Float pri, Uint32 attr);
/* a cell of a sprite, a part of a texture. it draws sx pixels across and sy
 * down about its point (cx, cy). texid is the texture's index in the
 * sprite's tlist. the part runs from (u1, v1) to (u2, v2), which count 256
 * to the texture's side. njDrawSprite2D never reads attr */
typedef struct {
    Sint16 sx, sy;
    Sint16 cx, cy;
    Sint16 u1, v1;
    Sint16 u2, v2;
    Sint16 texid;
    Sint16 attr;
} NJS_TEXANIM;
/* a sprite: where its cells draw, their scale, their turn, and the cells.
 * njDrawSprite2D never reads p.z, and reads ang only under NJD_SPRITE_ANGLE */
typedef struct {
    NJS_POINT3 p;
    Float sx, sy;
    Angle ang;
    NJS_TEXLIST *tlist;
    NJS_TEXANIM *tanim;
} NJS_SPRITE;
/* attributes of njDrawSprite2D */
#define NJD_SPRITE_COLOR 0x00000002
#define NJD_SPRITE_ALPHA 0x00000020
/* draw cell n of sp's tanim on the screen at priority pri. the cell's point
 * (cx, cy) lands on (p.x, p.y). the cell scales by sx across and sy down.
 * the call makes sp's tlist the current texture list. NJD_SPRITE_ALPHA makes
 * the sprite translucent. NJD_SPRITE_COLOR gives it the constant material's
 * color. without it the sprite is white, of alpha 1. pri gives the depth as
 * for njDrawPolygon2D */
void njDrawSprite2D(NJS_SPRITE *sp, Int n, Float pri, Uint32 attr);
/* an attribute of njDrawSprite3D */
#define NJD_SPRITE_SCALE 0x00000010
/* attributes of the sprite draws. NJD_SPRITE_ANGLE turns the cell by sp's
 * ang. NJD_SPRITE_HFLIP mirrors it left for right, and NJD_SPRITE_VFLIP top
 * for bottom */
#define NJD_SPRITE_ANGLE 0x00000001
#define NJD_SPRITE_HFLIP 0x00000004
#define NJD_SPRITE_VFLIP 0x00000008
/* draw cell n of sp's tanim at p, through the current matrix. the call makes
 * sp's tlist the current texture list. NJD_SPRITE_ALPHA and NJD_SPRITE_COLOR
 * act as for njDrawSprite2D. this header gives the draw only under
 * NJD_SPRITE_SCALE. the cell's point (cx, cy) then lands where p projects.
 * sx and sy scale the cell across and down. the scaled cell is the sprite's
 * size in pixels at a z equal to the screen's distance. a farther sprite is
 * smaller in proportion. the screen's aspect then stretches it. a p whose z
 * lies below _nj_near_clip_ or above _nj_far_clip_ draws nothing */
void njDrawSprite3D(NJS_SPRITE *sp, Int n, Uint32 attr);
/* do nothing: the sprite draws keep no state to set up */
void njInitSprite(void);
/* color modes. a mode says how a polygon's vertices give their colors. it
 * also says which list the polygon goes in, the opaque or the translucent. its
 * NJD_ARC1_TRANS bits put it in the translucent list. NJD_ARC1_HIGHLIGHT adds
 * an offset color, a specular highlight. NJD_ARC1_UV16_FLOAT gives each
 * vertex a u and a v of 16 bits each */
#define NJD_ARC1_TRANS 0x02100000
#define NJD_ARC1_HIGHLIGHT 0x00000004
#define NJD_ARC1_UV16_FLOAT 0x00000001
/* the color modes, by their names' parts. a PACKED mode's vertex holds a
 * packed color, and an INTENSITY mode's an intensity of the polygon's color.
 * any other mode's vertex holds its color as four floats. NORMAL leaves the
 * polygon untextured, and TEXTURE textures it. FLAT shades each polygon flat,
 * HIGHLIGHT has NJD_ARC1_HIGHLIGHT, and TRANS has NJD_ARC1_TRANS */
#define NJD_COLOR_MODE_PACKED_NORMAL 0x00080002
#define NJD_COLOR_MODE_PACKED_TEXTURE 0x0008000a
#define NJD_COLOR_MODE_PACKED_NORMAL_TRANS 0x02100002
#define NJD_COLOR_MODE_PACKED_TEXTURE_TRANS 0x0210000a
#define NJD_COLOR_MODE_FLAT_PACKED_NORMAL 0x00080000
#define NJD_COLOR_MODE_FLAT_PACKED_TEXTURE 0x00080008
#define NJD_COLOR_MODE_HIGHLIGHT_FLAT_PACKED_TEXTURE 0x0008000c
#define NJD_COLOR_MODE_FLAT_NORMAL 0x00080010
#define NJD_COLOR_MODE_NORMAL 0x00080012
#define NJD_COLOR_MODE_FLAT_TEXTURE 0x00080018
#define NJD_COLOR_MODE_TEXTURE 0x0008001a
#define NJD_COLOR_MODE_HIGHLIGHT_FLAT_TEXTURE 0x0008001c
#define NJD_COLOR_MODE_HIGHLIGHT_TEXTURE 0x0008001e
#define NJD_COLOR_MODE_FLAT_INTENSITY_NORMAL 0x00080020
#define NJD_COLOR_MODE_INTENSITY_NORMAL 0x00080022
#define NJD_COLOR_MODE_FLAT_INTENSITY_TEXTURE 0x00080028
#define NJD_COLOR_MODE_INTENSITY_TEXTURE 0x0008002a
#define NJD_COLOR_MODE_HIGHLIGHT_FLAT_INTENSITY_TEXTURE 0x0008002c
#define NJD_COLOR_MODE_HIGHLIGHT_INTENSITY_TEXTURE 0x0008002e
#define NJD_COLOR_MODE_FLAT_PACKED_NORMAL_TRANS 0x02100000
#define NJD_COLOR_MODE_FLAT_PACKED_TEXTURE_TRANS 0x02100008
#define NJD_COLOR_MODE_HIGHLIGHT_FLAT_PACKED_TEXTURE_TRANS 0x0210000c
#define NJD_COLOR_MODE_FLAT_NORMAL_TRANS 0x02100010
#define NJD_COLOR_MODE_NORMAL_TRANS 0x02100012
#define NJD_COLOR_MODE_FLAT_TEXTURE_TRANS 0x02100018
#define NJD_COLOR_MODE_TEXTURE_TRANS 0x0210001a
#define NJD_COLOR_MODE_HIGHLIGHT_FLAT_TEXTURE_TRANS 0x0210001c
#define NJD_COLOR_MODE_HIGHLIGHT_TEXTURE_TRANS 0x0210001e
#define NJD_COLOR_MODE_FLAT_INTENSITY_NORMAL_TRANS 0x02100020
#define NJD_COLOR_MODE_INTENSITY_NORMAL_TRANS 0x02100022
#define NJD_COLOR_MODE_FLAT_INTENSITY_TEXTURE_TRANS 0x02100028
#define NJD_COLOR_MODE_INTENSITY_TEXTURE_TRANS 0x0210002a
#define NJD_COLOR_MODE_HIGHLIGHT_FLAT_INTENSITY_TEXTURE_TRANS 0x0210002c
#define NJD_COLOR_MODE_HIGHLIGHT_INTENSITY_TEXTURE_TRANS 0x0210002e

/* a quad on the screen for njDrawQuadTextureEx: the parallelogram with a
 * corner at (x, y) and edges along (vx1, vy1) and (vx2, vy2). each corner
 * takes z as its depth, 1 over its distance from the camera. the corner at
 * (x, y) shows the texture at (u, v). the texture moves by (vu1, vv1) along
 * the first edge, and by (vu2, vv2) along the second */
typedef struct {
    Float x, y, z;
    Float vx1, vy1;
    Float vx2, vy2;
    Uint32 dmy;
    Float u, v;
    Float vu1, vv1;
    Float vu2, vv2;
    Uint32 dmy2[2];
} NJS_QUAD_TEXTURE_EX;
/* start a run of quads with njGetkmPointer. a nonzero trans puts them in the
 * translucent list, and 0 in the opaque list */
void njQuadTextureStart(Sint32 trans);
/* end the run with njSetkmPointer. the list takes the run's quads only then,
 * so a run left unended draws nothing */
void njQuadTextureEnd(void);
/* give the run's later quads texture texid of the current texture list,
 * colored by col */
void njSetQuadTexture(Sint32 texid, Uint32 col);
/* the same with the texture of _nj_texmemlist whose globalIndex is gid. some
 * texture of _nj_texmemlist must have it */
void njSetQuadTextureG(Sint32 gid, Uint32 col);
/* color the run's later quads by col. they keep the current texture */
void njSetQuadTextureColor(Uint32 col);
/* send q as a quad of the run */
void njDrawQuadTextureEx(NJS_QUAD_TEXTURE_EX *q);
/* a quad on the screen for njDrawQuadTexture: the rectangle with corners at
 * (x1, y1) and (x2, y2). the corner at (x1, y1) shows the texture at (u1, v1).
 * the corner at (x2, y2) shows it at (u2, v2) */
typedef struct {
    Float x1, y1, x2, y2;
    Float u1, v1, u2, v2;
} NJS_QUAD_TEXTURE;
/* send q as a quad of the run. each corner takes z as its depth, 1 over its
 * distance from the camera. each u and v keeps only its float's high 16
 * bits */
void njDrawQuadTexture(NJS_QUAD_TEXTURE *q, Float z);

/* a screen position in characters, for the njPrint calls: x in the high half
 * and y in the low */
#define NJM_LOCATION(x, y) ((x) << 16 | (y))
/* the x and the y that NJM_LOCATION packs into loc */
#define NJM_LOCATION_X(loc) ((Sint16)((loc) >> 16))
#define NJM_LOCATION_Y(loc) ((Sint16)(loc))
/* the debug print. njInitPrint starts it, and njExitPrint ends it.
 *
 * texture is the font: size texels a side, in attr, an NJD_TEXFMT_ format. a
 * null texture takes the built-in font, 128 texels a side, and leaves size
 * and attr unread. a font holds 16 rows of 16 characters. a character's code
 * gives its row in its high four bits, and its column in its low four.
 *
 * njInitPrint loads the font into texture memory. it sets _nj_context_[4] up
 * for the print's sprites, and packs that context's parameter words. it makes
 * the print's color 0xffbfbfbf and its size 8. it reads no status from Kamui,
 * so a font that Kamui fails to load goes unnoticed. njExitPrint frees the
 * font's texture memory */
void njInitPrint(Uint32 *texture, Uint32 size, Uint32 attr);
void njExitPrint(void);
/* make size the print's size, the side in pixels of each later character */
void njPrintSize(Uint16 size);
/* make c, a packed ARGB color, the color later njPrint calls write in */
void njPrintColor(Uint32 c);
/* the calls below write text at loc, on a grid of cells each the print's size
 * a side. the top left corner of cell NJM_LOCATION(x, y) lies x sizes
 * across and y sizes down. each character fills a cell: a sprite of
 * the print's color, in the translucent list. the first character takes loc's
 * cell, and each next one the cell to the right.
 *
 * each call sends its sprites through the store queues, which it maps to the
 * translucent list. it then moves _nj_vertex_buffer_'s next address for that
 * list past them, leaving njGetkmPointer's copy as it was.
 * _nj_current_texture becomes -2, since the sprites draw with the font.
 *
 * njPrintC writes the string s, which must be shorter than 32768
 * characters */
void njPrintC(Int loc, const char *s);
/* write val in decimal, in digit places. digit must be at most 10. a
 * negative val takes a minus sign before its magnitude, and must not be the
 * least Int. a magnitude too long for digit places shows its low digits.
 * where digit is 2 or more, the top place then shows 0 instead */
void njPrintD(Int loc, Int val, Int digit);
/* write val's low digit bits as 0s and 1s, the highest first. digit must be
 * at most 32 */
void njPrintB(Int loc, Uint32 val, Int digit);
/* write val's low digit hexadecimal digits, the highest first. digit must be
 * at most 8 */
void njPrintH(Int loc, Uint32 val, Int digit);
/* write val in decimal, with digit places before the point and five after
 * it. zeros pad a shorter integral part on the left, and a negative val takes
 * a minus sign first. digit must lie from the integral part's count of digits
 * to 13. outside that range, the call stores outside its 20-byte buffer on
 * the stack */
void njPrintF(Int loc, Float val, Int digit);
/* write, as njPrintC does, the text vsprintf makes of fmt and the arguments
 * after it. the text must fit 128 bytes with its NUL, unchecked */
void njPrint(Int loc, const char *fmt, ...);

/* push a matrix: the current one again for a null m, or a copy of m. it
 * returns TRUE, or FALSE, pushing nothing, when the stack is full */
Bool njPushMatrix(NJS_MATRIX *m);
/* Pop n matrices, keeping at least the base. The GCC port returns FALSE
 * without changing the stack or XMTRX if n reaches or exceeds its depth.
 * Popping zero keeps the live transform. The original SDK only rejects
 * an exact-depth pop, and reloads the current slot for a zero count. */
Bool njPopMatrix(Uint32 n);
/* push a copy of the current matrix. it returns TRUE, or FALSE, pushing
 * nothing, when the stack is full */
Bool njPushMatrixEx(void);
/* pop one matrix. it returns TRUE, or FALSE, popping nothing, when the stack
 * holds only its base */
Bool njPopMatrixEx(void);
/* copy _nj_unit_matrix_ into m, or into the current matrix for a null m */
void njUnitMatrix(NJS_MATRIX *m);
/* invert m in place, or the current matrix for a null m, and return TRUE.
 * where the determinant is 0, every element becomes 3.4e38 instead, and the
 * call returns FALSE */
Bool njInvertMatrix(NJS_MATRIX *m);
/* the determinant of m, or of the current matrix for a null m */
Float njDetMatrix(NJS_MATRIX *m);
/* copy the current matrix to m */
void njGetMatrix(NJS_MATRIX *m);
/* copy ms to md, or make ms the current matrix for a null md */
void njSetMatrix(NJS_MATRIX *md, NJS_MATRIX *ms);
/* put a translation by (x, y, z) before m. a null m stands for the current
 * matrix. a point takes the translation first, then m as it was */
void njTranslate(NJS_MATRIX *m, Float x, Float y, Float z);
/* njTranslate by v's x, y and z */
void njTranslateV(NJS_MATRIX *m, NJS_VECTOR *v);
/* njTranslateV on the current matrix */
void njTranslateEx(NJS_VECTOR *v);
/* put a turn by ang about x, which takes y toward z, before m. a null m
 * stands for the current matrix. a point takes the turn first, then m as it
 * was */
void njRotateX(NJS_MATRIX *m, Angle ang);
/* the same about y, which takes z toward x */
void njRotateY(NJS_MATRIX *m, Angle ang);
/* the same about z, which takes x toward y */
void njRotateZ(NJS_MATRIX *m, Angle ang);
/* put a turn by ang about the axis along v before m, as njRotateX puts its
 * turn. v must be a unit vector, unchecked: the call takes v's parts as the
 * sines and cosines of the turns that bring v onto an axis */
void njRotate(NJS_MATRIX *m, NJS_VECTOR *v, Angle ang);
/* a quaternion: re + im[0] i + im[1] j + im[2] k */
typedef struct {
    Float re;
    Float im[3];
} NJS_QUATERNION;
/* put the turn of the unit quaternion qua before the current matrix, as
 * njRotateEx puts its turns. the turn is by 2 acos(re), about im over
 * sqrt(1 - re * re). that axis has unit length only where qua does. where
 * re * re is 1 or more, the turn is none */
void njQuaternionEx(NJS_QUATERNION *qua);
/* make m Ninja's matrix stack. m must hold n matrices, and n be at least 1.
 * m must start on a 32-byte boundary, as njPushMatrix and njPushMatrixEx
 * require. the stack starts with one matrix, the current one: a scale of -1
 * in x and y. while flag isn't 0, njCalcVector makes each vector it writes
 * unit length. The GCC port leaves the current stack and transform intact
 * on a null or misaligned base, nonpositive capacity or an unaddressable
 * extent. The caller supplies storage for all n matrices. */
void njInitMatrix(NJS_MATRIX *m, Sint32 n, Int flag);
/* put a turn before the current matrix. a point takes the turn first, then
 * the matrix as it was. the turn is by ang[0] about x, then by ang[1] about
 * y, then by ang[2] about z. with lv above 0 the order is z, then x, then y */
void njRotateEx(Angle *ang, Sint32 lv);
/* put a turn before m, or before the current matrix for a null m. the turn
 * is by angz about z, then by angx about x, then by angy about y */
void njRotateZXY(NJS_MATRIX *m, Angle angx, Angle angy, Angle angz);
/* put a scale by v's x, y and z before the current matrix, as njRotateEx
 * puts its turn */
void njScaleEx(NJS_VECTOR *v);
/* put the reflection through the plane pl before m, or before the current
 * matrix for a null m. a point is reflected first, then takes m as it was.
 * pl's normal, vx to vz, must be unit length */
void njMirror(NJS_MATRIX *m, NJS_PLANE *pl);
/* put a scale by sx, sy and sz before m. a null m stands for the current
 * matrix. a point takes the scale first, then m as it was */
void njScale(NJS_MATRIX *m, Float sx, Float sy, Float sz);
/* njScale by v's x, y and z */
void njScaleV(NJS_MATRIX *m, NJS_VECTOR *v);
/* njMultiMatrix puts ms before md, or before the current matrix for a null
 * md. a point takes ms first, then md as it was. md may be ms */
void njMultiMatrix(NJS_MATRIX *md, NJS_MATRIX *ms);
/* transform the num points at ps by m into pd: pd[i] = (ps[i], 1) times m. a
 * null m stands for the current matrix. a num of 0 or less transforms none. pd
 * may be ps. with a null m the call also reads ps[num], the point after the
 * last, which must be readable */
void njCalcPoints(NJS_MATRIX *m, NJS_POINT3 *ps, NJS_POINT3 *pd, Int num);
/* the screen Ninja projects onto */
extern NJS_SCREEN _nj_screen_;
/* copy s's dist, cx and cy into _nj_screen_. w and h are left as they were */
void njSetScreen(NJS_SCREEN *s);
/* set _nj_screen_.dist to _nj_screen_.w / (2 * tan(ang / 2)). at that
 * distance the screen's width spans the angle ang. ang / 2 is ang shifted
 * right by 1. _nj_scr_aspect_x_ plays no part */
void njSetPerspective(Angle ang);
/* set the near clip to the depth n and the far clip to f, each held to
 * -65535..-1. _nj_near_clip_ gets -n, and _nj_far_clip_ gets -f */
void njClipZ(Float n, Float f);
/* the factor njProjectScreen scales a projected x by */
extern Float _nj_scr_aspect_x_;
/* the factor njProjectScreen scales a projected y by */
extern Float _nj_scr_aspect_y_;
/* project p3, transformed by m, onto the screen, into p2. a null m stands for
 * the current matrix. with (x, y, z) the transformed point, p2->x =
 * _nj_screen_.cx + _nj_screen_.dist * x / z * _nj_scr_aspect_x_. p2->y is the
 * same with cy, y and _nj_scr_aspect_y_. z must not be 0, unchecked */
void njProjectScreen(NJS_MATRIX *m, NJS_POINT3 *p3, NJS_POINT2 *p2);
/* add vs to vd */
void njAddVector(NJS_VECTOR *vd, NJS_VECTOR *vs);
/* subtract vs from vd */
void njSubVector(NJS_VECTOR *vd, NJS_VECTOR *vs);
/* the dot product of v1 and v2 */
Float njInnerProduct(NJS_VECTOR *v1, NJS_VECTOR *v2);
/* set ov to v1 x v2, and return ov's length. ov may be v1 or v2: both are
 * read whole before ov is written */
Float njOuterProduct(NJS_VECTOR *v1, NJS_VECTOR *v2, NJS_VECTOR *ov);
/* the fractional part of n, as modff gives it */
Float njFraction(Float n);
/* n rounded away from zero: ceilf(|n|), negated for n below 0 */
Float njRoundUp(Float n);
/* n rounded toward zero: floorf(|n|), negated for n below 0 */
Float njRoundOff(Float n);
/* set every field of c. px, py and pz become 0, and (vx, vy, vz) becomes
 * (0, 0, -1). roll becomes 0, and ang 0x2aaa, a sixth of a turn rounded down.
 * n_clip becomes -1 and f_clip -60000. lx becomes (1, 0, 0) and ly
 * (0, 1, 0) */
void njInitCamera(NJS_CAMERA *c);
/* the current matrix's slot on the matrix stack. the slot holds the matrix
 * only once a push writes it there */
extern NJS_MATRIX *_nj_current_matrix_ptr_;
/* the matrix stack's first slot */
extern NJS_MATRIX *_nj_base_matrix_ptr_;
/* the near clip distance: a camera's n_clip, negated */
extern Float _nj_near_clip_;
/* the far clip distance: a camera's f_clip, negated */
extern Float _nj_far_clip_;
/* how many matrices the stack holds now, the current one among them */
extern Sint32 _nj_stack_size_;
/* njSetCamera's copy of the stack's first matrix */
extern NJS_MATRIX _nj_view_matrix_;
/* njSetCamera's and njPointCameraInterest's bound on a direction's squared
 * length in x and z. at or under it, the direction gets no turn about y */
#define NO_YAW_LENGTH2_MAX 1.0e-6F
/* look through c from now on. the view takes c's place (px, py, pz) to the
 * origin. it then turns about y and about x to take c's direction
 * (vx, vy, vz) onto +z. last it makes njRotateZ's turn by -roll. the direction
 * must not be 0, which isn't tested.
 *
 * the view doesn't turn about y where vx * vx + vz * vz is
 * NO_YAW_LENGTH2_MAX or less. that bound doesn't scale with the direction's
 * length. the view then takes the direction onto +z only where vx is 0 and vz
 * isn't negative.
 *
 * the matrix stack is left with one matrix, the current one. that matrix is
 * the view, then a scale of -1 in x and y. _nj_view_matrix_ gets a copy of it.
 * _nj_screen_.dist becomes _nj_screen_.w / _nj_scr_aspect_x_ /
 * (2 * njTan(ang / 2)). _nj_near_clip_ becomes -n_clip, and _nj_far_clip_
 * -f_clip. lx and ly aren't read */
void njSetCamera(NJS_CAMERA *c);
/* store ang as c's angle of view. it takes effect at the next njSetCamera */
void njSetCameraAngle(NJS_CAMERA *c, Angle ang);
/* aim c at the point (x, y, z), which must not be c's place: that isn't
 * tested. c's direction (vx, vy, vz) becomes the unit vector toward the
 * point, to FSRRA's precision.
 *
 * njSetCamera's view of c turns two unit vectors onto -x and +y. lx and ly
 * become those vectors, but for one term each of lx.x and ly.x. that term takes
 * the wrong sign. each such term holds the direction's x times its y. so lx and
 * ly come out right where either is 0. the bug tag in njPointCamereInterest.c
 * gives the defect */
void njPointCameraInterest(NJS_CAMERA *c, Float x, Float y, Float z);
/* roll c by ang about its direction (vx, vy, vz). the direction must be a
 * unit vector, as njRotate's axis must be. c->roll grows by ang. lx and ly each
 * turn by ang about the direction. each is then scaled to unit length, to
 * FSRRA's precision. the turn is built in _nj_camera_matrix_, where
 * njSetCamera builds its view */
void njRollCameraInterest(NJS_CAMERA *c, Angle ang);
/* a camera and the motion that moves it */
typedef struct {
    NJS_CAMERA *camera;
    NJS_MOTION *motion;
} NJS_CACTION;
/* look through camera as motion poses it at frame, by njSetCamera. the motion
 * poses _nj_motion_camera_, a copy of camera, and camera stays as it was.
 * nothing happens where motion is null, or where frame isn't below its
 * nbFrame. njCameraAction and njCameraActionOld take caction's camera and
 * motion.
 *
 * the motion's one entry holds its factors in the order of their type bits.
 * inp_fn counts the factors. njCameraMotion and njCameraMotionOld take 1 to
 * 4, and the Action calls 1 to 5. at any other count the call returns
 * before njSetCamera, with the copy already reset to camera.
 *
 * the position sets the copy's px, py and pz, along a spline where inp_fn
 * asks for one. the target aims the copy at its point, by
 * njPointCameraInterest. a motion without a target may give a direction
 * instead. it aims the copy at the copy's place plus the direction. the roll
 * sets the copy's roll, by njRollCameraInterest from a roll of 0. the Old calls
 * roll from the copy's own roll instead. the angle of view sets the copy's by
 * njSetCameraAngle. each factor but the position takes the straight line
 * between keys. a factor without keys leaves the copy as it was.
 *
 * the call passes over the factors of NJD_MTYPE_ANG_1, NJD_MTYPE_SCL_2,
 * NJD_MTYPE_RGB_8 and NJD_MTYPE_INTENSITY_9. it passes over one factor for
 * NJD_MTYPE_SPOT_10, NJD_MTYPE_POINT_10 or both. it counts no factor for any
 * other bit, nor for NJD_MTYPE_VEC_3 beside NJD_MTYPE_TARGET_3. so an entry
 * must hold no factor for such a bit */
void njCameraAction(NJS_CACTION *caction, Float frame);
void njCameraMotion(NJS_CAMERA *camera, NJS_MOTION *motion, Float frame);
void njCameraActionOld(NJS_CACTION *caction, Float frame);
void njCameraMotionOld(NJS_CAMERA *camera, NJS_MOTION *motion, Float frame);
/* the places njGetCameraMotion writes a frame of a camera motion to */
typedef struct {
    Float *pos;
    Float *vect;
    Angle *roll;
    Angle *ang;
} NJS_CMOTION_DATA;
/* write the camera that motion gives at frame through data's pointers. only
 * the factors the motion moves are written. nothing is written where motion
 * is null, or where frame isn't below motion's nbFrame */
void njGetCameraMotion(NJS_CAMERA *camera, NJS_MOTION *motion,
                       NJS_CMOTION_DATA *data, Float frame);
/* write as njGetCameraMotion writes, from caction's camera and motion */
void njGetCameraAction(NJS_CACTION *caction, NJS_CMOTION_DATA *data,
                       Float frame);
/* njSin returns n's sine, and njCos its cosine, to FSCA's precision */
Float njSin(Angle n);
Float njCos(Angle n);
/* the square root of n, or -sqrt(-n) for a negative n */
Float njSqrt(Float n);
/* 1 over njSqrt(n), to FSRRA's precision */
Float njInvertSqrt(Float n);
/* n's tangent, to the precision of FSCA's sine over its cosine */
Float njTan(Angle n);
/* SHC's sqrtf and fabsf intrinsics, whose names have one leading underscore */
#ifdef DREAMCAST
#ifdef NONMATCHING
/* GCC spells them __builtin_sqrtf and __builtin_fabsf */
#define _builtin_sqrtf __builtin_sqrtf
#define _builtin_fabsf __builtin_fabsf
#endif
#endif
/* n's absolute value */
#define njAbs(n) ((Float)_builtin_fabsf((Float)(n)))
/* v's length */
Float njScalor(NJS_VECTOR *v);
/* v's length, squared */
Float njScalor2(NJS_VECTOR *v);
/* scale v to length 1, and return the length it had, both to FSRRA's
 * precision. v must not be zero: its length isn't tested */
Float njUnitVector(NJS_VECTOR *v);
/* turn vs by m, or by the current matrix for a null m, into vd. m's
 * translation is left out. while njInitMatrix's flag isn't 0, vd is then
 * scaled to unit length, to FSRRA's precision */
void njCalcVector(NJS_MATRIX *m, NJS_VECTOR *vs, NJS_VECTOR *vd);
/* put a turn before m, or before the current matrix for a null m. the turn
 * is by angx about x, then by angy about y, then by angz about z */
void njRotateXYZ(NJS_MATRIX *m, Angle angx, Angle angy, Angle angz);
/* transform ps by m, or by the current matrix for a null m, translation and
 * all, into pd */
void njCalcPoint(NJS_MATRIX *m, NJS_POINT3 *ps, NJS_POINT3 *pd);
/* copy m's translation, or the current matrix's for a null m, into p */
void njGetTranslation(NJS_MATRIX *m, NJS_POINT3 *p);
/* how far p1 is from p2, or 0 when the distance squared is under 0.025 */
Float njDistanceP2P(NJS_POINT3 *p1, NJS_POINT3 *p2);
/* how far p is from the line l, which runs both ways through its point, or 0
 * when the distance squared is under 0.025. where cp isn't null, it gets the
 * point of l nearest p. l's vector must not be 0 */
Float njDistanceP2L(NJS_POINT3 *p, NJS_LINE *l, NJS_POINT3 *cp);
/* how far l is from pl. l counts as parallel to pl where the cosine between
 * their vectors has a magnitude below 0.025. neither vector may
 * be 0.
 *
 * for a parallel l the call returns how far l's point (px, py, pz) lies from
 * pl. cp then gets the point of pl nearest l's point. for any other l the
 * call returns 0, and cp gets the point where l crosses pl. cp may be null */
Float njDistanceL2PL(NJS_LINE *l, NJS_PLANE *pl, NJS_POINT3 *cp);
/* njIsParalellL2PL says whether l counts as parallel to pl, as
 * njDistanceL2PL counts it. njIsParalellPL2PL says whether pl1 counts as
 * parallel to pl2. it does where the cosine between their vectors has a
 * magnitude above 0.975. no vector may be 0. both names keep the library's
 * spelling */
Bool njIsParalellL2PL(NJS_LINE *l, NJS_PLANE *pl);
Bool njIsParalellPL2PL(NJS_PLANE *pl1, NJS_PLANE *pl2);
/* how far p is from pl, to FSRRA's precision. cp gets the point of pl nearest
 * p. cp may be null. pl's vector must not be 0 */
Float njDistanceP2PL(NJS_POINT3 *p, NJS_PLANE *pl, NJS_POINT3 *cp);
/* how far pl1 is from pl2. for planes that count as parallel it returns how
 * far pl1's point is from pl2, and sets l's six floats to 0. for any others
 * it returns 0, and l gets the line where they cross. l's vector is pl1's
 * vector cross pl2's, and l's point is the line's nearest to pl2's point. l
 * may be null, and must be neither pl1 nor pl2. neither plane's vector may
 * be 0 */
Float njDistancePL2PL(NJS_PLANE *pl1, NJS_PLANE *pl2, NJS_LINE *l);
/* set v to (p[1] - p[0]) x (p[2] - p[0]), a vector normal to their plane.
 * v isn't scaled to unit length, and must not be one of the three points */
void njGetPlaneNormal(NJS_POINT3 *p, NJS_VECTOR *v);

/* a spline's attributes. the spline calls read only fparam[0], which is
 * njCardinalSpline's tension */
typedef struct {
    Int keys;
    Int *iparam;
    Float *fparam;
} NJS_SPLINE;

/* put in odata the point at frame, from 0 to 1, on the curve that four
 * points shape. idata holds the points, three floats each, and odata may be
 * one of them. the curve runs from the second point to the third. its tangent
 * at the second is s times the third less the first. its tangent at the third
 * is s times the fourth less the second. s is (1 - tension) / 2, where tension
 * is attr->fparam[0] */
void njCardinalSpline(Float *idata, Float *odata, NJS_SPLINE *attr,
                      Float frame);
/* njCardinalSpline's curve with s at 1 / 2. the call doesn't read attr */
void njOverhauserSpline(Float *idata, Float *odata, NJS_SPLINE *attr,
                        Float frame);
/* put in odata the point at frame on the Bezier curve that the four points
 * at idata shape. frame runs from 0 to 1. a point is three floats, and odata
 * may be one of the four. the curve runs from the first point to the fourth.
 * the second and third are its control points. the call doesn't read attr */
void njCubicBezierSpline(Float *idata, Float *odata, NJS_SPLINE *attr,
                         Float frame);

/* the lights of the direct, easy chunk and simple chunk drawers.
 * njSetDirectLight, njCnkSetEasyLight and njCnkSetSimpleLight point their
 * drawer's light along (x, y, z). each also copies the projection to its
 * drawer: _nj_screen_'s dist times each aspect, and its cx and cy. the drawer
 * keeps that copy until the next such call. the LightIntensity calls set a
 * light's intensity and its drawer's ambient level. the LightColor calls set a
 * light's color */
void njSetDirectLight(Float x, Float y, Float z);
void njSetDirectLightIntensity(Float inten, Float ambient);
void njSetDirectLightColor(Float r, Float g, Float b);
void njCnkSetEasyLight(Float x, Float y, Float z);
void njCnkSetEasyLightIntensity(Float inten, Float ambient);
void njCnkSetEasyLightColor(Float r, Float g, Float b);
void njCnkSetSimpleLight(Float x, Float y, Float z);
void njCnkSetSimpleLightIntensity(Float inten, Float ambient);
void njCnkSetSimpleLightColor(Float r, Float g, Float b);

/* how many lights Ninja keeps at most */
#define NJD_LIGHT_MAX 32
/* bits of a light's kind, which its attr.lsrc holds */
#define NJD_SPOT_LIGHT 0x08
#define NJD_SPEC_DIR 0x10
#define NJD_SPEC_POINT 0x20
/* the values worked out from a light for drawing. lpnt and lvctr hold its
 * place and direction. the rest hold its colors, and the ranges and cones
 * that fade it */
typedef struct {
    Float ratten;
    Float ipd;
    Float nrr;
    Float frr;
    Float cosi;
    Float cose;
    Float idev;
    Float odev;
    Float rate;
    Float intns;
    Int exp;
    Int reserve;
    NJS_POINT3 lpnt;
    NJS_VECTOR lvctr;
    NJS_VECTOR lmvctr;
    NJS_ARGB atten;
    NJS_ARGB amb;
    NJS_ARGB dif;
    NJS_ARGB spc;
    NJS_ARGB mamb;
    NJS_ARGB mdif;
    NJS_ARGB mspc;
} NJS_LIGHT_CAL;
/* a light's settings. lsrc holds the bits of its kind. the rest hold its
 * intensities, ranges, angles and color */
typedef struct {
    Int lsrc;
    Float iamb;
    Float idif;
    Float ispc;
    Float nrang;
    Float frang;
    void *func;
    Angle iang;
    Angle oang;
    NJS_ARGB argb;
} NJS_LIGHT_ATTR;
/* a light: its matrix, and the place and direction the matrix moves. stat
 * is NJD_LIGHT_ON while the light is on */
#define NJD_LIGHT_ON 1
#ifdef DREAMCAST
#ifdef NONMATCHING
/* pack(8) lifts -fpack-struct=4's cap, so NJS_MATRIX's aligned(8) aligns the
 * structure, and the matrix at its start, to 8. no other member aligns past
 * 4, and the size is a multiple of 8. so no offset or size changes */
#pragma pack(push, 8)
#endif
#endif
typedef struct {
    NJS_MATRIX mtrx;
    NJS_POINT3 pnt;
    NJS_VECTOR vctr;
    Int stat;
    Int reserve;
    NJS_LIGHT_CAL ltcal;
    NJS_LIGHT_ATTR attr;
} NJS_LIGHT;
#ifdef DREAMCAST
#ifdef NONMATCHING
/* this side ends the pack(8) above, and checks that the matrix lies on the
 * 8-byte boundary that pack means it to */
#pragma pack(pop)
_Static_assert(__builtin_offsetof(NJS_LIGHT, mtrx) % 8 == 0 &&
                   _Alignof(NJS_LIGHT) % 8 == 0,
               "an NJS_LIGHT's matrix lies off the 8-byte boundary");
#endif
#endif
/* the material the lights shine on: its ambient, diffuse and specular
 * colors, and its specular exponent */
typedef struct {
    NJS_ARGB ambient;
    NJS_ARGB diffuse;
    NJS_ARGB specular;
    Float exponent;
} NJS_LIGHT_MATERIAL;
/* set the chunk lights' ambient color to (r, g, b). its alpha is left as it
 * was */
void njCnkSetAmbient(Float r, Float g, Float b);

/* njColorBlendingMode's targets: the source's blend factor and the
 * destination's */
#define NJD_SOURCE_COLOR 0
#define NJD_DESTINATION_COLOR 1
/* njColorBlendingMode's factors. BOTHINVALPHA and BOTHSRCALPHA set the other
 * target's factor too */
#define NJD_COLOR_BLENDING_BOTHINVALPHA 0
#define NJD_COLOR_BLENDING_BOTHSRCALPHA 1
#define NJD_COLOR_BLENDING_DESTALPHA 2
#define NJD_COLOR_BLENDING_DESTCOLOR 3
#define NJD_COLOR_BLENDING_INVDESTALPHA 4
#define NJD_COLOR_BLENDING_INVDESTCOLOR 5
#define NJD_COLOR_BLENDING_INVSRCALPHA 6
#define NJD_COLOR_BLENDING_INVSRCCOLOR 7
#define NJD_COLOR_BLENDING_SRCALPHA 8
#define NJD_COLOR_BLENDING_SRCCOLOR 9
#define NJD_COLOR_BLENDING_ONE 10
#define NJD_COLOR_BLENDING_ZERO 11
/* set the current vertex context's blend factor for target to mode, then set
 * the context as njZBufferMode does */
void njColorBlendingMode(Int target, Int mode);
/* set the constant material from material, four floats in NJS_ARGB's
 * order */
void njSetConstantMaterial(NJS_ARGB *material);
/* the constant material, which njSetConstantMaterial sets */
extern NJS_ARGB _nj_constant_material_;
/* the NJD_CONTROL_3D_ bits that Ninja's draws test */
extern Uint32 _nj_control_3d_flag_;
/* make flag the NJD_CONTROL_3D_ bits */
void njControl3D(Uint32 flag);
/* a bit that no draw tests: setting it changes nothing */
#define NJD_CONTROL_3D_CONSTANT_ATTR 0x00000004
#define NJD_CONTROL_3D_ENABLE_ALPHA 0x00000008
/* under this bit the chunk drawers and njCnkDirectObjectCompile take a
 * material chunk's diffuse color from the constant material. its ambient and
 * specular colors stay the chunk's */
#define NJD_CONTROL_3D_CONSTANT_MATERIAL 0x00000010
/* under this bit, where NJD_CONTROL_3D_CONSTANT_MATERIAL is clear, a
 * material chunk's diffuse color is offset. each of its four parts becomes
 * the chunk's byte over 255, plus the constant material's part */
#define NJD_CONTROL_3D_OFFSET_MATERIAL 0x00000020
/* the depth queue. under this bit a multi chunk drawer checks each model
 * against _nj_depth_near_. a model that reaches past it goes to the
 * DepthQueue vertex functions, and its strips draw translucent */
#define NJD_CONTROL_3D_DEPTH_QUEUE 0x00000040
/* the depth past which the depth queue fades a vertex's alpha */
extern Float _nj_depth_near_;
/* set the depth queue's range. _nj_depth_near_ becomes -dnear, and
 * _nj_depth_far_ -dfar. the DepthQueue vertex functions fade a vertex as its z
 * grows from _nj_depth_near_ to _nj_depth_far_. so a caller passes each
 * distance negated, which puts dfar below dnear. _nj_depth_range_ becomes
 * 1 / (_nj_depth_far_ - _nj_depth_near_), so dnear must differ from dfar */
void njSetDepthQueue(Float dnear, Float dfar);
/* TRUE while the depth queue fades the model being drawn */
extern Int _nj_depth_enable_;
/* under this bit the chunk drawers skip a model that njCnkModelClip finds
 * outside the view. njCnkModDrawModel and njCnkModDrawObject test every
 * model. the other drawers draw a model whose r is 0 or less without the
 * test */
#define NJD_CONTROL_3D_MODEL_CLIP 0x00000100
/* under this bit a strip's color takes no red, green or blue from its
 * material's diffuse. the simple chunk drawer tests it for textured strips.
 * the simple multi drawer tests it for textured and environment mapped
 * strips */
#define NJD_CONTROL_3D_CONSTANT_TEXTURE_MATERIAL 0x00000200
/* under this bit, modifier volumes change opaque and translucent polygons
 * alike. njSetkmColorMode then gives every color mode the shadow bit, and so
 * do the sprite starters of nj_km.h */
#define NJD_CONTROL_3D_SHADOW 0x00000400
/* under this bit the chunk drawers and njCnkDirectObjectCompile pass each
 * strip chunk's header through njSetConstantAttr's masks */
#define NJD_CONTROL_3D_CNK_CONSTANT_ATTR 0x00000800
/* under this bit, njSetkmColorMode lets modifier volumes change the polygons
 * of a color mode without NJD_ARC1_TRANS */
#define NJD_CONTROL_3D_SHADOW_OPAQUE 0x00001000
/* under this bit, a volume that njCnkModDrawObject draws changes translucent
 * polygons as well as opaque ones */
#define NJD_CONTROL_3D_TRANS_MODIFIER 0x00002000
/* under this bit, Ninja's draws send their translucent polygons to the
 * punch-through list. they send their punch-through polygons to the
 * translucent list */
#define NJD_CONTROL_3D_USE_PUNCHTHROUGH 0x00004000
/* under this bit the chunk drawers blend as njSetCnkBlendMode says */
#define NJD_CONTROL_3D_CNK_BLEND_MODE 0x00008000
/* under this bit the easy chunk drawer's near clips send nothing. the drawer
 * then drops each triangle that crosses a z of 1 */
#define NJD_CONTROL_3D_DISABLE_NEAR_CLIP 0x00010000
/* under this bit the simple drawer's Env polygon functions add
 * _nj_uv_scroll_u_ to each vertex's u. they add _nj_uv_scroll_v_ to its v */
#define NJD_CONTROL_3D_ENV_UV_SCROLL 0x00020000
/* under this bit the simple drawer's polygon functions take each triangle's
 * facing negated */
#define NJD_CONTROL_3D_MIRROR_MODEL 0x80000000
/* set the masks of NJD_CONTROL_3D_CNK_CONSTANT_ATTR. a strip chunk's header
 * is the halfword of its type and flags. it is ANDed with and_attr, then ORed
 * with or_attr. njInit3D sets the masks to ~0 and 0, which leave the header
 * as it is */
void njSetConstantAttr(Uint32 and_attr, Uint32 or_attr);
/* the mask njSetConstantAttr ANDs a strip chunk's header with */
extern Uint32 _nj_constant_attr_and_;
/* the mask njSetConstantAttr ORs a strip chunk's header with */
extern Uint32 _nj_constant_attr_or_;
/* set the blend of NJD_CONTROL_3D_CNK_BLEND_MODE, which takes the place of the
 * blend a model's chunks give. attr holds it in NJD_FBS_ and NJD_FBD_ bits,
 * as those chunks do */
void njSetCnkBlendMode(Uint32 attr);
/* the blend njSetCnkBlendMode sets */
extern Uint32 _nj_cnk_blend_mode_;
/* how the 3D line and triangle draws take their points. under NJD_DRAW_FAN
 * the lines or triangles share point 0. under NJD_DRAW_CONNECTED each
 * shares the points of the one before */
#define NJD_DRAW_FAN 0x00000001
#define NJD_DRAW_CONNECTED 0x00000002
/* draw n lines from p's points and colors, through the current matrix.
 * attr's NJD_TRANSPARENT makes them translucent, and its NJD_USE_TEXTURE
 * textures them with the current vertex context's texture. line i joins:
 * - points i and i + 1, under NJD_DRAW_CONNECTED;
 * - else points 0 and i + 1, under NJD_DRAW_FAN;
 * - else points 2i and 2i + 1.
 * a line with an end beyond the far clip draws nothing. so does every line
 * of a fan whose point 0 lies there. each line goes as njTri_Wired sends
 * the outline of two vertices: two quads, one each way along it. where
 * one end lies at or nearer than the near clip, both quads run from that end.
 * the same quad then goes twice */
void njDrawLine3D(NJS_POINT3COL *p, Int n, Uint32 attr);
/* draw n triangles from p's points and colors, through the current matrix.
 * attr's NJD_TRANSPARENT and NJD_USE_TEXTURE act as for njDrawLine3D. attr's
 * NJD_FILL fills each triangle. without it each goes as its outline, as
 * njTri_Wired sends one. triangle i takes:
 * - points i to i + 2, under NJD_DRAW_CONNECTED;
 * - else point 0 and points 2i + 1 and 2i + 2, under NJD_DRAW_FAN;
 * - else points 3i to 3i + 2.
 * a triangle with a point beyond the far clip draws nothing. so does every
 * triangle of a fan whose point 0 lies there. under NJD_FILL a fan
 * checks only points 2i + 1 and 2i + 2 against the near clip. where point 0
 * alone lies at or nearer than it, triangle i goes uncut */
void njDrawTriangle3D(NJS_POINT3COL *p, Int n, Uint32 attr);
/* draw a polygon of n points through the current matrix. NJD_FILL fills
 * the polygon. NJD_USE_TEXTURE and NJD_TRANSPARENT select its texture
 * and blending as for njDrawTriangle3D. */
void njDrawPolygon3D(NJS_POINT3COL *p, Int n, Uint32 attr);
/* draw polygon's count vertices, at least 1, as one strip of triangles,
 * translucent when trans is nonzero */
void njDrawPolygon(NJS_POLYGON_VTX *polygon, Int count, Int trans);
/* start strips of triangles in the world. a trans of 1 puts them in the
 * translucent list, and any other in the opaque list */
void njDrawPolygon3DExStart(Int trans);
/* draw a polygon of n points through the current matrix. NJD_FILL fills
 * the polygon. NJD_USE_TEXTURE and NJD_TRANSPARENT select its texture
 * and blending as for njDrawTriangle3D. */
void njDrawPolygon3D(NJS_POINT3COL *p, Int n, Uint32 attr);
/* draw polygon's count vertices as one strip, each vertex a point the current
 * matrix places. it draws nothing where a vertex lies past the far clip. it
 * clips the strip at the near clip where a vertex lies at that clip or
 * nearer */
void njDrawPolygon3DExSetData(NJS_POLYGON_VTX *polygon, Int count);
/* end the strips njDrawPolygon3DExStart started */
void njDrawPolygon3DExEnd(void);
/* draw a polygon of n points through the current matrix. NJD_FILL fills
 * the polygon. NJD_USE_TEXTURE and NJD_TRANSPARENT select its texture
 * and blending as for njDrawTriangle3D. */
void njDrawPolygon3D(NJS_POINT3COL *p, Int n, Uint32 attr);
/* draw polygon's count vertices, at least 1, as one strip of triangles
 * textured with the texture whose global index is tex, translucent when flag
 * is nonzero */
void njDrawTexture(NJS_TEXTURE_VTX *polygon, Int count, Int tex, Int flag);
/* njDrawTexture with the texture of record tex of _nj_texmemlist */
void njDrawTextureMemList(NJS_TEXTURE_VTX *polygon, Int count, Int tex,
                          Int flag);
/* draw p's count vertices as one strip of triangles, translucent when trans
 * is nonzero. the vertices are points in the current matrix's space, which
 * the call takes to the screen. it draws nothing where a vertex lies past the
 * far clip. it clips the strip at the near clip where a vertex lies at that
 * clip or nearer */
void njDrawPolygon3DEx(NJS_POLYGON_VTX *p, Int count, Int trans);
/* draw as njDrawPolygon3DEx does, with the strip textured. its texture is
 * the one the current vertex context holds, as njSetTextureNum sets it */
void njDrawTexture3DEx(NJS_TEXTURE_VTX *p, Int count, Int trans);
/* start textured strips of triangles in the world, with the texture
 * njSetTextureNum set. a trans of 1 puts them in the translucent list, and
 * any other in the opaque list */
void njDrawTexture3DExStart(Int trans);
/* draw p's count vertices as one strip, each vertex a point the current
 * matrix places. it draws nothing where a vertex lies past the far clip. it
 * clips the strip at the near clip where a vertex lies at that clip or
 * nearer */
void njDrawTexture3DExSetData(NJS_TEXTURE_VTX *p, Int count);
/* end the strips njDrawTexture3DExStart started */
void njDrawTexture3DExEnd(void);
/* njPtclPolygonStart starts a run of untextured squares that face the
 * screen, in the opaque list. njPtclPolygonEnd ends the run. col, a packed
 * ARGB color, is every square's color. the run keeps one polygon open until
 * its end. no other draw may run between the two calls */
void njPtclPolygonStart(Uint32 col);
void njPtclPolygonStartTrans(Uint32 color);
/* Start screen-width lines with a half-width in pixels and packed ARGB color.
 * A strip joins adjacent points. A list takes disjoint pairs. Both use the
 * vertex work buffer and the current transform. */
void njDrawLineExStartOpaque(Float half_width, Uint32 color);
void njDrawLineExStartTrans(Float half_width, Uint32 color);
void njDrawLineExEnd(void);
void njDrawLine3DExSetStrip(NJS_POINT3 *points, Sint32 count);
void njDrawLine3DExSetList(NJS_POINT3 *points, Sint32 count);
void njPtclPolygonEnd(void);
/* draw a square of the run at each of the n points at p, n at least 1. the
 * current matrix and the screen project each point as njProjectScreen does,
 * and the square's center goes there. its half side is h on the screen,
 * whatever the point's depth. a point with z below _nj_near_clip_ or above
 * _nj_far_clip_ gets no square. the call also reads p[n], the point after the
 * last, which must be readable */
void njPtclDrawPolygon(NJS_POINT3 *p, Sint32 n, Float h);
/* draw as njPtclDrawPolygon does, for n points that lie 24 bytes apart from
 * p. the call also reads the point after the last */
void njPtclDrawPolygon2(NJS_POINT3 *p, Sint32 n, Float h);
/* njPtclSpriteStart starts a run of sprites, and njPtclSpriteEnd ends it. a
 * sprite is a rectangle textured with texture texid of the current texture
 * list. col, a packed ARGB color, is every sprite's color. a flag of 0 puts
 * them in the opaque list, and any other in the translucent. the run keeps one
 * polygon open until its end. no other draw may run between the two
 * calls */
void njPtclSpriteStart(Sint32 texid, Uint32 col, Sint32 flag);
void njPtclSpriteEnd(void);
/* draw a sprite of the run at each of the n points at p, n at least 1. the
 * current matrix and the screen project each point as njProjectScreen does,
 * and the sprite's center goes there. w and h are half the sprite's width and
 * height at a z equal to the screen's distance. a farther point's sprite is
 * smaller in proportion. a point with z below _nj_near_clip_ or above
 * _nj_far_clip_ gets no sprite. the call also reads p[n], the point after the
 * last, which must be readable */
void njPtclDrawSprite(NJS_POINT3 *p, Sint32 n, Float w, Float h);
/* njFogEnable sets the current vertex context's fog mode to the fog table.
 * njFogDisable sets it to no fog. each then sets the context as njZBufferMode
 * does */
void njFogEnable(void);
void njFogDisable(void);
/* how many floats a fog table holds, Kamui's KM_FOG_ENTRY */
#define FOG_TABLE_SIZE 128
/* a fog table: the fog at each of FOG_TABLE_SIZE depths, from 0 to 1 */
typedef Float NJS_FOG_TABLE[FOG_TABLE_SIZE];
/* set the fog table's color to c, a KMPACKEDARGB as one word */
void njSetFogColor(Uint32 c);
/* set the fog table from fog, as kmSetFogTable does */
void njSetFogTable(NJS_FOG_TABLE fog);
/* a fog density word is a mantissa times a power of 2. bits 8 to 15, which
 * FOG_DENSITY_MANTISSA masks, are the mantissa in 128ths, so 0x80 is 1. bits 0
 * to 7 are the power, a signed byte */
#define FOG_DENSITY_MANTISSA 0xff00
/* the fog density word whose value is 65536: a mantissa of 1, and a power of
 * 16 */
#define FOG_DENSITY_65536 0x8010
/* set the fog density to density, a fog density word */
void njSetFogDensity(Uint32 density);
/* the fog density word for -f, cut to eight significant bits. f must not be
 * above 0. f below -65536 gives FOG_DENSITY_65536 */
Uint32 njGenerateFogDensity(Float f);
/* fill fog, a fog table, and set the fog density. near is the depth -n, and
 * far the depth -f. near must be below far, or every entry gets 0.
 *
 * entry i stands for the depth far / s. s is (16 + (i & 15)) / 16, times 2
 * to the power i >> 4. s runs from 1 at entry 0 to 248 at entry 127.
 *
 * where near is above far / 256, the fog is a straight ramp in depth. a far
 * of 1 takes the ramp too, whatever near is. the ramp is 1 at far, and 0 from
 * the first entry no deeper than near on. near must then be far / 248 or more.
 * for any other near and far, each entry gets its depth over far.
 *
 * the density becomes the fog density word for far, which needs far to be 1
 * or more. a smaller far sets every bit of the word above its low byte. a
 * near above 65535 sets FOG_DENSITY_65536 instead, whatever far is */
void njGenerateFogTable3(NJS_FOG_TABLE fog, Float n, Float f);
extern NJS_VERTEX_BUF *_nj_vertex_buf_;
/* start Ninja's 3D draws. _nj_vertex_buf_ becomes vbuf, and _nj_vertex_num_
 * vn, its count of vertices. the call clears every NJD_CONTROL_3D_ bit, and
 * sets njSetConstantAttr's masks to ~0 and 0. the constant material's a
 * becomes 255, and its r, g and b 0 */
void njInit3D(NJS_VERTEX_BUF *vbuf, Int vn);
/* a running count of the vertices in the vertex chunks the chunk drawers
 * reach */
extern Int _nj_calc_vertex_;
/* a running count of strip vertices. the easy drawer's polygon functions
 * add the length of each strip they read */
extern Int _nj_calc_polygon_;
/* a running count of sent vertices. the easy drawer's polygon functions add
 * how many vertices they send */
extern Int _nj_draw_polygon_;
/* draw object, its children and its siblings, with the simple chunk drawer.
 * each node's model must be one njCnkSimpleDrawModel can draw, unless
 * NJD_EVAL_HIDE is set. its child must not be null unless NJD_EVAL_BREAK is
 * set */
void njCnkSimpleDrawObject(NJS_CNK_OBJECT *object);
/* draw object, its children and its siblings, with the easy chunk drawer.
 * each node's model must not be null unless NJD_EVAL_HIDE is set. its child
 * must not be null unless NJD_EVAL_BREAK is set */
void njCnkEasyDrawObject(NJS_CNK_OBJECT *object);
/* draw object, its children and its siblings, as modifier volumes */
void njCnkModDrawObject(NJS_CNK_OBJECT *object);
/* draw model as a modifier volume, as njCnkModDrawObject draws each node's.
 * neither of model's lists may be null.
 *
 * the call projects the vertex list's first chunk alone. it does so only
 * where that chunk's first halfword is NJD_CV, with no flags. it draws the
 * polygon list only where some projected point has a z above 1e-5. it sends
 * each NJD_CO_P3 chunk as a modifier volume, and skips every other chunk. it
 * skips each chunk below NJD_MATOFF as one halfword. so the list must hold no
 * tiny chunk, which takes two */
void njCnkModDrawModel(NJS_CNK_MODEL *model);
/* draw model with the easy chunk drawer */
void njCnkEasyDrawModel(NJS_CNK_MODEL *model);
/* draw model with the simple chunk drawer. no strip chunk may set the top
 * bit of its halfword, which no NJD_FST_ flag uses. the drawer's tables have
 * no row for it */
void njCnkSimpleDrawModel(NJS_CNK_MODEL *model);
/* draw model with the easy multi chunk drawer, which lights each vertex by
 * several lights. it takes only NJD_CV_VN, NJD_CV_VN_NF and NJD_CV_VN_D8
 * vertex chunks. it draws no strip of a model with any other kind */
void njCnkEasyMultiDrawModel(NJS_CNK_MODEL *model);
/* draw object, its children and its siblings, with the easy multi chunk
 * drawer. each node's model must not be null unless NJD_EVAL_HIDE is set.
 * its child must not be null unless NJD_EVAL_BREAK is set */
void njCnkEasyMultiDrawObject(NJS_CNK_OBJECT *object);
/* njCnkEasyMultiDrawModel for the simple multi chunk drawer */
void njCnkSimpleMultiDrawModel(NJS_CNK_MODEL *model);
/* njCnkEasyMultiDrawObject for the simple multi chunk drawer */
void njCnkSimpleMultiDrawObject(NJS_CNK_OBJECT *object);
/* the easy multi chunk drawer's lights, numbered 1 to 6. each is on or off.
 * the drawer reads them from light 1 up to the last light, which
 * njCnkSetEasyMultiLight sets. a call that takes light needs it from 1 to 6.
 * njCnkSetEasyMultiLightSwitch also takes 0, for all six.
 *
 * njCnkSetEasyMultiLight takes a num from 0 to 6. it turns lights 1 to num
 * on, and makes light num the last. light 1 then shines along its vector, and
 * lights 2 to num from their points. a num of 0 makes light 1, which is off,
 * the last. njCnkSetEasyMultiLightSwitch turns light on where flag is nonzero,
 * and off where it is 0. it leaves the last light as it was.
 *
 * njCnkSetEasyMultiAmbient sets the lights' ambient color. a strip's ambient
 * light is that color times its material's ambient, or 0 under NJD_FST_IA.
 * njCnkSetEasyMultiLightColor sets light's color.
 *
 * njCnkSetEasyMultiLightVectorEx makes light shine along (vx, vy, vz), and
 * njCnkSetEasyMultiLightVector does so for light 1.
 * njCnkSetEasyMultiLightPoint makes light shine from (px, py, pz).
 * njCnkSetEasyMultiLightRange sets a point light's ranges. the light gives
 * nothing at frange or further. past nrange its part falls off as nrange
 * squared over the distance squared.
 *
 * njCnkSetEasyMultiLightMatrices takes each light that is on into the view's
 * space, by the current matrix. it stops at the last light. it takes a point
 * light's point as njCalcPoint does, and another light's vector as njCalcVector
 * does. the drawer reads a light's vector or point only as that call left it */
void njCnkSetEasyMultiLight(Int num);
void njCnkSetEasyMultiLightSwitch(Int light, Int flag);
void njCnkSetEasyMultiAmbient(Float ar, Float ag, Float ab);
void njCnkSetEasyMultiLightColor(Int light, Float lr, Float lg, Float lb);
void njCnkSetEasyMultiLightVector(Float vx, Float vy, Float vz);
void njCnkSetEasyMultiLightVectorEx(Int light, Float vx, Float vy, Float vz);
void njCnkSetEasyMultiLightPoint(Int light, Float px, Float py, Float pz);
void njCnkSetEasyMultiLightRange(Int light, Float nrange, Float frange);
void njCnkSetEasyMultiLightMatrices(void);
/* the simple multi chunk drawer's lights. each call works as its easy
 * namesake above, on the simple multi drawer's lights */
void njCnkSetSimpleMultiLight(Int num);
void njCnkSetSimpleMultiLightSwitch(Int light, Int flag);
void njCnkSetSimpleMultiAmbient(Float ar, Float ag, Float ab);
void njCnkSetSimpleMultiLightColor(Int light, Float lr, Float lg, Float lb);
void njCnkSetSimpleMultiLightVector(Float vx, Float vy, Float vz);
void njCnkSetSimpleMultiLightVectorEx(Int light, Float vx, Float vy, Float vz);
void njCnkSetSimpleMultiLightPoint(Int light, Float px, Float py, Float pz);
void njCnkSetSimpleMultiLightRange(Int light, Float nrange, Float frange);
void njCnkSetSimpleMultiLightMatrices(void);
/* draw object as motion poses it at frame, with the easy chunk drawer.
 * nothing is drawn where frame isn't below motion's nbFrame. a null motion
 * poses each node by its pos, ang and scl.
 *
 * the call reads none of NJD_EVAL_UNIT_POS, NJD_EVAL_UNIT_ANG,
 * NJD_EVAL_UNIT_SCL and NJD_EVAL_BREAK. each node without NJD_EVAL_SKIP turns
 * in the order that object's own NJD_EVAL_ZXY_ANG gives, whatever its own
 * flag says. a motion that turns by quaternions is the exception. positions
 * and scales are interpolated in a straight line, whatever motion's inp_fn
 * says */
void njCnkEasyDrawMotion(NJS_CNK_OBJECT *object, NJS_MOTION *motion,
                         Float frame);
/* make buf the buffer the shape motion draws move vertices in. for chunk
 * models it must hold the largest vertex chunk a shape motion moves. a shape
 * link draw needs room for three such chunks. two hold its shapes' chunks, and
 * the third their blend. for a basic model it must hold the model's nbPoint
 * vectors. where the motion moves both points and normals, it needs twice
 * that: the points, then the normals */
void njInitShape(Float *buf);
/* draw object with the easy chunk drawer, posed by motion at frame. shape
 * moves the vertices of object's models at the same frame. nothing is drawn
 * where frame isn't below motion's nbFrame, nor where it isn't below shape's. a
 * null motion or shape skips its test */
void njCnkEasyDrawShapeMotion(NJS_CNK_OBJECT *object, NJS_MOTION *motion,
                              NJS_MOTION *shape, Float frame);
/* have the chunk motion draws, njCnkEasyDrawMotion and njCnkSimpleDrawMotion
 * among them, call func with each node they walk. each call comes once the
 * node's matrix is current, whether or not the node's model then draws. func
 * stays set until the next call. a null func calls nothing */
void njCnkSetMotionCallback(void (*func)(NJS_CNK_OBJECT *object));
/* make func the callback of the basic models' motion draws, in place of the
 * last one set. it is njCnkSetMotionCallback's counterpart for NJS_OBJECT
 * trees */
void njSetMotionCallback(void (*func)(NJS_OBJECT *object));
/* the function njSetMotionCallback last set, or null. njInitSystem sets it
 * null */
extern void (*_nj_motion_callback_)(NJS_OBJECT *object);
/* draw as njCnkEasyDrawMotion does, with the simple chunk drawer. where
 * motion's inp_fn has a bit of NJD_MTYPE_MASK, the call interpolates
 * positions and scales along a spline */
void njCnkSimpleDrawMotion(NJS_CNK_OBJECT *object, NJS_MOTION *motion,
                           Float frame);
/* draw as njCnkSimpleDrawMotion does, while shape moves the vertices of
 * object's models at frame. frame must be below shape's nbFrame as well, or
 * nothing is drawn. a null shape moves no vertex. how the call interpolates
 * shape depends on shape's NJD_MTYPE_MASK bits */
void njCnkSimpleDrawShapeMotion(NJS_CNK_OBJECT *object, NJS_MOTION *motion,
                                NJS_MOTION *shape, Float frame);
/* draw as njCnkSimpleDrawMotion, njCnkSimpleDrawShapeMotion,
 * njCnkEasyDrawMotion and njCnkEasyDrawShapeMotion draw, but for each
 * angle's change between two keys. a change above half a turn loses a turn.
 * one below minus half a turn gains a turn */
void njCnkSimpleDrawMotionOld(NJS_CNK_OBJECT *object, NJS_MOTION *motion,
                              Float frame);
void njCnkSimpleDrawShapeMotionOld(NJS_CNK_OBJECT *object, NJS_MOTION *motion,
                                   NJS_MOTION *shape, Float frame);
void njCnkEasyDrawMotionOld(NJS_CNK_OBJECT *object, NJS_MOTION *motion,
                            Float frame);
void njCnkEasyDrawShapeMotionOld(NJS_CNK_OBJECT *object, NJS_MOTION *motion,
                                 NJS_MOTION *shape, Float frame);
/* draw as njCnkEasyDrawMotion and njCnkEasyDrawShapeMotion draw, with the
 * easy multi chunk drawer. njCnkSimpleMultiDrawMotion and
 * njCnkSimpleMultiDrawShapeMotion draw the same with the simple multi
 * drawer. each draw takes every factor and shape in a straight line,
 * whatever inp_fn says. the Old forms change each angle as
 * njCnkSimpleDrawMotionOld does */
void njCnkEasyMultiDrawMotion(NJS_CNK_OBJECT *object, NJS_MOTION *motion,
                              Float frame);
void njCnkEasyMultiDrawShapeMotion(NJS_CNK_OBJECT *object, NJS_MOTION *motion,
                                   NJS_MOTION *shape, Float frame);
void njCnkSimpleMultiDrawMotion(NJS_CNK_OBJECT *object, NJS_MOTION *motion,
                                Float frame);
void njCnkSimpleMultiDrawShapeMotion(NJS_CNK_OBJECT *object, NJS_MOTION *motion,
                                     NJS_MOTION *shape, Float frame);
void njCnkEasyMultiDrawMotionOld(NJS_CNK_OBJECT *object, NJS_MOTION *motion,
                                 Float frame);
void njCnkEasyMultiDrawShapeMotionOld(NJS_CNK_OBJECT *object,
                                      NJS_MOTION *motion, NJS_MOTION *shape,
                                      Float frame);
void njCnkSimpleMultiDrawMotionOld(NJS_CNK_OBJECT *object, NJS_MOTION *motion,
                                   Float frame);
void njCnkSimpleMultiDrawShapeMotionOld(NJS_CNK_OBJECT *object,
                                        NJS_MOTION *motion, NJS_MOTION *shape,
                                        Float frame);
/* draw object with the simple chunk drawer, posed between motion_link's two
 * motions, each at its own frame. rate 0 gives the first's pose, and 1 the
 * second's. the exception is a link where one motion turns by quaternions
 * and the other doesn't. the other's angles then go through njAng2Quat, which
 * turns them in another order than njRotateEx. so the rate that picks that
 * motion need not give its pose under njCnkSimpleDrawMotion.
 *
 * nothing is drawn where rate is above 1, or where a frame isn't below its
 * motion's nbFrame. a null motion poses each node by its pos, ang and scl,
 * and its frame isn't tested */
void njCnkSimpleDrawMotionLink(NJS_CNK_OBJECT *object,
                               NJS_MOTION_LINK *motion_link, Float rate);
/* draw as njCnkSimpleDrawMotionLink does, while shape_link moves the
 * vertices of object's models between its two shapes at the same rate */
void njCnkSimpleDrawShapeMotionLink(NJS_CNK_OBJECT *object,
                                    NJS_MOTION_LINK *motion_link,
                                    NJS_MOTION_LINK *shape_link, Float rate);
/* draw object with the easy chunk drawer, posed between motion_link's two
 * motions as njCnkSimpleDrawMotionLink poses it. the easy draw interpolates in
 * a straight line, whatever the motions' inp_fn says. nothing is drawn where
 * rate is above 1, or where a frame isn't below its motion's nbFrame */
void njCnkEasyDrawMotionLink(NJS_CNK_OBJECT *object,
                             NJS_MOTION_LINK *motion_link, Float rate);
/* draw as njCnkEasyDrawMotionLink does. shape_link moves the vertices of
 * object's models between its two shapes at the same rate */
void njCnkEasyDrawShapeMotionLink(NJS_CNK_OBJECT *object,
                                  NJS_MOTION_LINK *motion_link,
                                  NJS_MOTION_LINK *shape_link, Float rate);
/* njCnkSimpleDrawMotionLinkOld and njCnkSimpleDrawShapeMotionLinkOld draw
 * as the calls without Old do. njCnkEasyDrawMotionLinkOld and
 * njCnkEasyDrawShapeMotionLinkOld first move each angle a turn toward the one
 * it blends with. they do so where the two lie over half a turn apart. a
 * key's angle blends with the previous key's, and the second motion's with the
 * first's. so angles less than a turn and a half apart blend the shorter way */
void njCnkSimpleDrawMotionLinkOld(NJS_CNK_OBJECT *object,
                                  NJS_MOTION_LINK *motion_link, Float rate);
void njCnkSimpleDrawShapeMotionLinkOld(NJS_CNK_OBJECT *object,
                                       NJS_MOTION_LINK *motion_link,
                                       NJS_MOTION_LINK *shape_link, Float rate);
void njCnkEasyDrawMotionLinkOld(NJS_CNK_OBJECT *object,
                                NJS_MOTION_LINK *motion_link, Float rate);
void njCnkEasyDrawShapeMotionLinkOld(NJS_CNK_OBJECT *object,
                                     NJS_MOTION_LINK *motion_link,
                                     NJS_MOTION_LINK *shape_link, Float rate);
/* start a walk through motion's nodes at frame, from the first node. the call
 * changes nothing where frame isn't below motion's nbFrame. a null motion
 * moves no node */
void njSetCnkCurrentMotion(NJS_MOTION *motion, Float frame);
/* pos gets the position of the walk's node, three floats. spline_flag must be
 * 0, to interpolate between keys in a straight line, or 1, along a spline.
 * the motion must move positions, and the node must have keys for them:
 * neither is tested */
void njGetCnkMotionTranslate(Float *pos, Int spline_flag);
/* ang gets the angles of the walk's node, and scl its scale, as
 * njGetCnkMotionTranslate gets the position. each angle takes the straight
 * line, whatever spline_flag is. qua gets the node's quaternion, which takes
 * the straight line too. the angles or the quaternion are the factor after
 * the position, where the motion's type moves positions. the scale is the
 * factor after the position and the turn. the motion must move the factor,
 * and the node must have keys for it: neither is tested */
void njGetCnkMotionRotate(Angle *ang, Int spline_flag);
void njGetCnkMotionQuaternion(NJS_QUATERNION *qua, Int spline_flag);
void njGetCnkMotionScale(Float *scl, Int spline_flag);
/* pose object by the walk's node, then move the walk on to the next node.
 * the call puts the node's translation, turn and scale before the current
 * matrix. a point so takes the scale first. object's own pos, ang or scl
 * stands in for a factor the motion doesn't move. it stands in too where the
 * node has no keys for the factor. last, a non-null m goes before the current
 * matrix too. spline_flag is as njGetCnkMotionTranslate takes it.
 *
 * njCnkMotionTransform turns about x, then y, then z.
 * njCnkMotionTransformZXY turns about z, then x, then y. each needs a motion
 * with NJD_MTYPE_ANG_1 or without NJD_MTYPE_QUAT_1.
 * njCnkMotionTransformQuaternion turns by the motion's quaternions, through
 * njQuaternionEx, and needs a motion without NJD_MTYPE_ANG_1.
 * njSetCnkCurrentMotion sets a form's turn only for a motion that suits it */
void njCnkMotionTransform(NJS_MATRIX *m, NJS_CNK_OBJECT *object,
                          Int spline_flag);
void njCnkMotionTransformZXY(NJS_MATRIX *m, NJS_CNK_OBJECT *object,
                             Int spline_flag);
void njCnkMotionTransformQuaternion(NJS_MATRIX *m, NJS_CNK_OBJECT *object,
                                    Int spline_flag);
/* njCnkMotionTranslate puts the walk's node's translation before the current
 * matrix, as njCnkMotionTransform does. njCnkMotionRotateXYZ puts its turn
 * about x, then y, then z. njCnkMotionRotateZXY puts its turn about z, then
 * x, then y. njCnkMotionQuaternion puts its quaternion's turn, and
 * njCnkMotionScale its scale. each turn needs the motion that
 * njCnkMotionTransform's form of the same turn needs. the walk stays at the
 * node. each call keeps m for njSetCnkNextMotionNode */
void njCnkMotionTranslate(NJS_MATRIX *m, NJS_CNK_OBJECT *object,
                          Int spline_flag);
void njCnkMotionRotateXYZ(NJS_MATRIX *m, NJS_CNK_OBJECT *object,
                          Int spline_flag);
void njCnkMotionRotateZXY(NJS_MATRIX *m, NJS_CNK_OBJECT *object,
                          Int spline_flag);
void njCnkMotionQuaternion(NJS_MATRIX *m, NJS_CNK_OBJECT *object,
                           Int spline_flag);
void njCnkMotionScale(NJS_MATRIX *m, NJS_CNK_OBJECT *object, Int spline_flag);
/* move the walk on to the next node. where the last call kept a non-null m,
 * put m before the current matrix */
void njSetCnkNextMotionNode(void);
/* read the whole file fname into a new block from syMalloc, and return the
 * block. it holds the file's sectors, GDD_FS_SCTSIZE bytes each, and 8 bytes
 * more. those keep njReadBinary's read of a chunk's name and size after the
 * last chunk in the block. the call returns null where the file can't be
 * opened or sized. it
 * returns null where the heap can't give the block, or where the read
 * fails. it closes the file either way, and a failed read frees the
 * block */
Sint8 *njOpenBinary(const char *fname);
/* free wbuff, a block njOpenBinary returned */
void njCloseBinary(Sint8 *wbuff);
/* the names njReadBinary gives a chunk of chunk models' objects, and a chunk
 * of basic models' */
#define iff_NJCM 0x4e4a434d
#define iff_NJBM 0x4e4a424d
/* copy the data of the chunk at wbuff + *fpos to a block from syMalloc, and
 * return the copy. a chunk is a name, its data's size in bytes, then its
 * data. *ptype gets the name.
 *
 * a POF0 chunk that follows lists the places in the copy that hold pointers.
 * the call adds the copy's address to each of those that isn't 0. *fpos then
 * lies past the POF0 chunk. where the next chunk isn't a POF0 chunk, the copy
 * stays as read. *fpos then lies 4 bytes into that next chunk, past its name.
 *
 * the name must be NJIN, NJCM, NJBM, NJTL, NJLI, NJCA, NMDM, NLIM, NCAM or
 * NSSM. for any other name, *ptype gets '****'. the call then returns null.
 * it also returns null where the heap can't give the copy. either null return
 * leaves *fpos 8 bytes on, past the name and the size */
void *njReadBinary(Sint8 *wbuff, Uint32 *fpos, Uint32 *ptype);
/* compile object, its children and its siblings into buf for
 * njDirectDrawObject. the call returns the address after the last compiled
 * model. buf must hold the bytes that njCnkDirectObjectCompileSize gives
 * under the same culling mode. object must be as that call requires.
 *
 * the compile skips the model of each node with NJD_EVAL_HIDE, and the
 * children of each node with NJD_EVAL_BREAK. each node's points go through
 * its own transform and its parents', from object down. the current matrix
 * plays no part, and is left as it was. the matrix stack needs room for one
 * more matrix than object's tree has levels.
 *
 * the compile takes NJD_CV, NJD_CV_VN, NJD_CV_D8 and NJD_CV_VN_D8 vertex
 * chunks. it takes NJD_CS, NJD_CS_UVN and NJD_CS_UVH strip chunks. it passes
 * over the other vertex and strip chunks. a vertex chunk's vertices go to
 * _nj_vertex_buf_ as NJS_DIRECT_BUF entries. they start at the chunk's index
 * offset plus those of the chunks before it. _nj_vertex_buf_ must hold them
 * all. a model with none of the four vertex chunks indexes the entries already
 * there. its records take the formats of the model compiled before it.
 *
 * the compile applies the constant material, njSetConstantAttr's masks and
 * njSetCnkBlendMode's blend where their NJD_CONTROL_3D_ bits are set. it
 * applies NJD_CONTROL_3D_SHADOW and NJD_CONTROL_3D_SHADOW_OPAQUE too. a later
 * change to any of these doesn't reach the compiled model. the blend reaches
 * the translucent strips alone. each texture a texture chunk names must be
 * loaded in the current texture list. the current vertex context keeps the
 * color mode, texture and blend that the compile set last.
 *
 * at least one model must compile */
void *njCnkDirectObjectCompile(NJS_CNK_OBJECT *object, void *buf);
/* compile model into buf as njCnkDirectObjectCompile compiles a node's
 * model, and return the address after it. model's points stay in its own
 * space. buf must hold the bytes that njCnkDirectModelCompileSize gives under
 * the same culling mode */
void *njCnkDirectModelCompile(NJS_CNK_MODEL *model, void *buf);
/* each size call returns the bytes its compile writes for object or model.
 * njCnkDirectObjectCompileSize and njCnkDirectModelCompileSize count for
 * njCnkDirectObjectCompile and njCnkDirectModelCompile. the two LightSize
 * calls count for the lit compiles. each node of object without
 * NJD_EVAL_HIDE must have a model, and each without NJD_EVAL_BREAK a child.
 * each model counted must have a polygon list */
Uint32 njCnkDirectObjectCompileSize(NJS_CNK_OBJECT *object);
Uint32 njCnkDirectModelCompileSize(NJS_CNK_MODEL *model);
Uint32 njCnkDirectModelCompileLightSize(NJS_CNK_MODEL *model);
Uint32 njCnkDirectObjectCompileLightSize(NJS_CNK_OBJECT *object);
/* a vertex of a chunk model as the compile transforms it, for the strips to
 * index. it holds the vertex's point through the current matrix, its normal
 * and its packed ARGB color */
typedef struct {
    Float px, py, pz;
    Float nx, ny, nz;
    Uint32 col;
    Uint32 unk_1c;
} NJS_DIRECT_BUF;
/* a compiled vertex of the plain formats. a model's strips take these where its
 * last vertex chunk is NJD_CV or NJD_CV_VN, outside a lit compile. uv's high
 * half is u's float cut to its high halfword. uv's low half is v's, cut
 * likewise. param is KM_VERTEXPARAM_NORMAL, or KM_VERTEXPARAM_ENDOFSTRIP for a
 * strip's last vertex */
typedef struct {
    Float px, py, pz;
    Float nz, nx, ny;
    Uint32 uv;
    Uint32 param;
} NJS_DIRECT_VERTEX;
/* a compiled vertex of the D8 formats, which carry colors where the plain
 * formats carry a normal. a model's strips take these where its last vertex
 * chunk is NJD_CV_D8 or NJD_CV_VN_D8. in a lit compile, they take these
 * wherever the model has a vertex chunk. one holds 1.0 in every record. base
 * and off are the packed ARGB base and offset colors. param and uv are as in
 * NJS_DIRECT_VERTEX */
typedef struct {
    Float px, py, pz, one;
    Uint32 base, off;
    Uint32 param;
    Uint32 uv;
} NJS_DIRECT_VERTEX_D8;
/* the light of a lit compile. (x, y, z) is the direction the light travels.
 * inten and ambient are the light's intensity and ambient level. r, g and b
 * are its color */
typedef struct {
    Float x, y, z;
    Float inten, ambient;
    Float r, g, b;
} NJS_DIRECT_COMPILE_LIGHT;
/* compile model as njCnkDirectModelCompile does, lit by light. buf must hold
 * the bytes that njCnkDirectModelCompileLightSize gives under the same
 * culling mode. the records take the D8 formats wherever model has a vertex
 * chunk. a model without one takes the formats of the model compiled before
 * it. those may be the plain formats. the vertex and strip compilers below
 * say how a lit compile colors a vertex */
void *njCnkDirectModelCompileLight(NJS_CNK_MODEL *model, void *buf,
                                   NJS_DIRECT_COMPILE_LIGHT *light);
/* compile object as njCnkDirectObjectCompile does, lit by light as
 * njCnkDirectModelCompileLight lights a model. buf must hold the bytes that
 * njCnkDirectObjectCompileLightSize gives under the same culling mode */
void *njCnkDirectObjectCompileLight(NJS_CNK_OBJECT *object, void *buf,
                                    NJS_DIRECT_COMPILE_LIGHT *light);
/* the vertex compilers, njCnkDirectVertexVN and the three below it. each
 * writes num vertices of one vertex chunk to buf, num 1 or more. vlist points
 * to the chunk's first vertex, a run of floats. each point goes through the
 * current matrix.
 *
 * njCnkDirectVertexVN takes an NJD_CV_VN chunk. each normal goes through the
 * current matrix, and is then scaled to unit length. outside a lit compile,
 * each color is opaque white.
 *
 * in a lit compile, the call colors each vertex by the compile's
 * NJS_DIRECT_COMPILE_LIGHT. let d be the dot product of the vertex's normal
 * and the light's (x, y, z). the intensity is ambient less inten times d,
 * held to ambient..1. r, g and b are each held
 * to 0..1. the color's red byte is 255 times the intensity times r,
 * truncated. its green and blue bytes take g and b alike, and its alpha byte
 * is 255 */
void njCnkDirectVertexVN(Float *vlist, NJS_DIRECT_BUF *buf, Int num);
/* njCnkDirectVertexV takes an NJD_CV chunk. each normal is (0, 0, 1), and
 * each color opaque white */
void njCnkDirectVertexV(Float *vlist, NJS_DIRECT_BUF *buf, Int num);
/* njCnkDirectVertexVD8 takes an NJD_CV_D8 chunk. each normal is (0, 0, 1),
 * and each color the vertex's own */
void njCnkDirectVertexVD8(Float *vlist, NJS_DIRECT_BUF *buf, Int num);
/* njCnkDirectVertexVND8 takes an NJD_CV_VN_D8 chunk. each normal goes as
 * njCnkDirectVertexVN's does, and each color is the vertex's own */
void njCnkDirectVertexVND8(Float *vlist, NJS_DIRECT_BUF *buf, Int num);
/* the strip compilers, njCnkDirectPolygon and the five below it. each
 * compiles the strips of one strip chunk into ptr, one record for each vertex
 * it sends. it returns the address after its last record. *vtx gets the
 * count of records.
 *
 * plist points to the chunk's first strip, and strips is the halfword before
 * it, which NJD_UFO_SHIFT describes. the chunk must hold one strip or more. a
 * strip is a length, 3 or more, then that many vertices. a vertex is its
 * index in vbuf, then any halfwords its format adds.
 *
 * a negative length marks a strip that winds against a positive one. under
 * KM_CULLCCW or KM_CULLCW, as njCnkDirectCullingMode last took, such a strip
 * sends its first vertex twice. that reverses the winding the hardware sees.
 * under any other mode the polygons are culled by size alone, and the strip
 * sends that vertex once.
 *
 * a D8 record's base color is its vertex's color, and its offset color 0. in
 * a lit compile, the base color's alpha is 255 times the diffuse alpha, held
 * to 0..255. that diffuse is the last one a material chunk gave in any
 * compile. it is opaque white before the first.
 *
 * njCnkDirectPolygon compiles an NJD_CS chunk to NJS_DIRECT_VERTEX records,
 * each its vertex's point and normal, with uv left unwritten */
void *njCnkDirectPolygon(Sint16 *plist, NJS_DIRECT_BUF *vbuf, Uint32 strips,
                         void *ptr, Uint32 *vtx);
/* compile an NJD_CS chunk to NJS_DIRECT_VERTEX_D8 records, uv unwritten. in a
 * lit compile, the diffuse r, g and b scale the base color's matching bytes.
 * each scaled byte is held to 0..255 */
void *njCnkDirectPolygonD8(Sint16 *plist, NJS_DIRECT_BUF *vbuf, Uint32 strips,
                           void *ptr, Uint32 *vtx);
/* compile an NJD_CS chunk to NJS_DIRECT_VERTEX_D8 records for an
 * environment map. each uv comes from its vertex's normal. the normal's x
 * from -1 to 1 takes u from 0 to 1. its y from -1 to 1 takes v from 1 to 0 */
void *njCnkDirectPolygonEnvD8(Sint16 *plist, NJS_DIRECT_BUF *vbuf,
                              Uint32 strips, void *ptr, Uint32 *vtx);
/* compile an NJD_CS_UVN or NJD_CS_UVH chunk to NJS_DIRECT_VERTEX records,
 * each its vertex's point and normal. each vertex's index is followed by its
 * u and v, a halfword each. the record's uv packs u * div + 1/512 and
 * v * div + 1/512. both count in units of the texture's size. the compile
 * passes div as 1/256 for NJD_CS_UVN and 1/1024 for NJD_CS_UVH */
void *njCnkDirectTexture(Sint16 *plist, NJS_DIRECT_BUF *vbuf, Uint32 strips,
                         void *ptr, Float div, Uint32 *vtx);
/* compile as njCnkDirectTexture does, to NJS_DIRECT_VERTEX_D8 records */
void *njCnkDirectTextureD8(Sint16 *plist, NJS_DIRECT_BUF *vbuf, Uint32 strips,
                           void *ptr, Float div, Uint32 *vtx);
/* compile an NJD_CS_UVN or NJD_CS_UVH chunk to NJS_DIRECT_VERTEX_D8 records
 * for an environment map. the u and v after each vertex's index are passed
 * over. each record's uv comes from its vertex's normal, as in
 * njCnkDirectPolygonEnvD8 */
void *njCnkDirectTextureEnvD8(Sint16 *plist, NJS_DIRECT_BUF *vbuf,
                              Uint32 strips, void *ptr, Uint32 *vtx);
/* the culling modes as the hardware's ISP word holds them.
 * NJD_POLYGON_CULLINGSMALL culls a polygon by its size alone.
 * NJD_POLYGON_CULLINGACW culls the polygons that wind counterclockwise, and
 * NJD_POLYGON_CULLINGCW those that wind clockwise */
#define NJD_POLYGON_CULLINGSMALL 0x08000000
#define NJD_POLYGON_CULLINGACW 0x10000000
#define NJD_POLYGON_CULLINGCW 0x18000000
/* set the culling that later compiles write into each strip chunk's global
 * record. mode KM_CULLCCW sets NJD_POLYGON_CULLINGACW, and KM_CULLCW sets
 * NJD_POLYGON_CULLINGCW. any other mode sets NJD_POLYGON_CULLINGSMALL. under
 * NJD_POLYGON_CULLINGACW or NJD_POLYGON_CULLINGCW, each strip of reversed
 * winding compiles to one more record. the size calls count that record too.
 * so a size call's count holds only for a compile under the mode it was made
 * under */
void njCnkDirectCullingMode(Uint32 mode);
/* copy to the direct drawer what njDirectDrawObject clips by: the screen,
 * _nj_far_clip_, and the 2D clip rectangle. njUserClipping sets that
 * rectangle to its tiles under NJD_CLIP_INSIDE, and to the whole screen
 * otherwise. the near plane is the depth 1, whatever _nj_near_clip_ is. the
 * drawer keeps the copies until the next call. a later change to any of these
 * reaches it only through that call */
void njSetDirectClip(void);
/* a compiled model: one node's model as njCnkDirectObjectCompile wrote it,
 * which links to the next node's */
typedef struct direct_head NJS_DIRECT_HEAD;
/* draw model and each compiled model linked after it, with the direct drawer.
 * they go through the current matrix, clipped as the last njSetDirectClip
 * set. the far plane, the near plane and the 2D clip rectangle's edges bound
 * the clip. a compiled model whose bounding sphere lies wholly beyond one of
 * them is skipped. one whose sphere crosses the near plane is clipped there */
void njDirectDrawObject(NJS_DIRECT_HEAD *model);

/* the unit matrix, which njUnitMatrix copies */
extern NJS_MATRIX _nj_unit_matrix_;
/* the texture list that njSetTexture last set */
extern NJS_TEXLIST *_nj_current_texlist;
/* Ninja's records of loaded textures */
extern NJS_TEXMEMLIST *_nj_texmemlist;
/* how many records _nj_texmemlist holds */
extern Uint32 _nj_texmemsize;
/* _nj_current_texture is the current texture's global index, and
 * _nj_current_texture_bank its record's bank */
extern Uint32 _nj_current_texture;
extern Uint32 _nj_current_texture_bank;
/* the format of the texture last set to draw with, an NJD_TEXFMT_ value */
extern Sint32 _nj_texglobal_color_;
/* the last texture error */
extern NJS_TEXERRSTAT _nj_texerr_;
/* the Kamui vertex context Ninja draws with */
extern struct tagKMVERTEXCONTEXT *_nj_current_ctx_;

#ifdef DREAMCAST
#ifdef NONMATCHING
/* GCC's inline helpers on SH4ZAM. they come last, after every prototype they
 * take the place of */
#include <ninja_zam.h>
/* the port's features, and the macros that route two of the draws above
 * through its 16:9. the path runs from this directory, since a compile may
 * take no include directory but this one */
#include "../../engine/dc/port.h"
#endif
#endif

#endif
