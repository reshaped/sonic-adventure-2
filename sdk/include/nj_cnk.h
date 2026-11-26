/* what the chunk drawers' members share.
 *
 * a drawer's vertex walker, such as njCnkEasyVlist, reads a model's vertex
 * list. it hands each vertex chunk to a vertex function. that function lights
 * the chunk's vertices and projects them into the drawer's buffer. the
 * drawer's polygon walker, such as njCnkEasyPlist, then reads the polygon
 * list. it hands each strip chunk to a polygon function. that function sends
 * the strips to Kamui's lists, with their vertices from the buffer.
 *
 * the easy and the simple drawers' polygon walkers misread the chunk after a
 * bump texture's. each tests the next chunk's type for any bit of NJD_CM_BU,
 * where the type should equal NJD_CM_BU. so every material chunk's type
 * passes. the walker reads a passing chunk's first six halfwords after its
 * size as the texture's vectors. it then resumes 8 halfwords past the chunk's
 * start, whatever the chunk's size.
 *
 * a run is triangles in a row that a polygon function sends as a strip of
 * their own. a polygon function's Clip form is named for it with Clip after.
 * it serves a model with a vertex near the eye. it sends each run of whole
 * triangles, those its family finds far enough from the eye. it hands some of
 * the others to its near clip, which cuts the triangle where z is CNK_NEAR_Z.
 * the near clip sends the part beyond as a strip of 3 or 4 vertices */
#ifndef NJ_CNK_H
#define NJ_CNK_H
#include <ninja.h>
#include <sg_xpt.h>

/* a chunk's first halfword holds its type in the low byte and its flags in
 * the high byte. CNK_TYPE gives the type, and CNK_FLAGS the flags shifted
 * down. CNK_FLAGS_MASK masks the flags without shifting them */
#define CNK_TYPE(head) ((head) & 0xff)
#define CNK_FLAGS(head) (((head) >> 8) & 0xff)
#define CNK_FLAGS_MASK 0xff00
/* where flag, an NJD_FST_ flag, stands once a walker shifts a strip
 * chunk's flags down by NJD_FST_SHIFT */
#define CNK_FST(flag) ((flag) >> NJD_FST_SHIFT)
/* a vertex chunk's size halfword counts the 4-byte words after it, of
 * CNK_WORD_HALFWORDS halfwords each */
#define CNK_WORD_HALFWORDS 2
/* a material chunk's halfwords before its data: its head and its size */
#define CNK_MATERIAL_LEAD 2
/* the evalflags of a node that keeps its parent's matrix */
#define CNK_EVAL_UNIT                                                          \
    (NJD_EVAL_UNIT_POS | NJD_EVAL_UNIT_ANG | NJD_EVAL_UNIT_SCL)
/* the scale from a UVN or a UVH strip's u and v to texture coordinates */
#define UVN_SCALE (1.0F / 256.0F)
#define UVH_SCALE (1.0F / 1024.0F)
/* an Env function's texture coordinate: u is CNK_ENV_CENTER plus
 * CNK_ENV_SCALE times its entry's nx, and v the same of ny */
#define CNK_ENV_CENTER 0.5F
#define CNK_ENV_SCALE 0.5F
/* a bump vector's halfword is its float times this */
#define BUMP_SCALE 32767.0F
/* how far past strips, in halfwords, a polygon function prefetches its strip
 * chunk */
#define CNK_STRIP_AHEAD 3
/* a strip's vertices before its first triangle's last: v0 and v1. a strip of
 * len vertices holds len - CNK_STRIP_LEAD triangles */
#define CNK_STRIP_LEAD 2
/* the halfwords a textured strip gives each vertex after its index: its u,
 * then its v */
#define CNK_UV_HALFWORDS 2
/* the words of a 32-byte block. a sent vertex is one block, or two */
#define CNK_BLOCK_WORDS 8
/* the words of a 64-byte vertex */
#define CNK_VERTEX64_WORDS (2 * CNK_BLOCK_WORDS)
/* a sent vertex's texture coordinate takes CNK_UV_WORDS words from word
 * CNK_UV_WORD: u, then v. CNK_UV_END is the word after v */
#define CNK_UV_WORD 4
#define CNK_UV_WORDS 2
#define CNK_UV_END (CNK_UV_WORD + CNK_UV_WORDS)
/* the word of the simple drawer's 32-byte vertex that holds its spec */
#define CNK_SPEC_WORD 7

/* a vertex of the easy drawer's buffer. x and y place it on the screen. ooz
 * is 1 over its depth, and inten is the intensity of its light */
typedef struct {
    Float x;
    Float y;
    Float ooz;
    Float inten;
} NJS_CNK_EASY_BUF;

/* a vertex of the simple drawer's buffer. z is its depth, and x and y place
 * it on the screen. ooz is 1 over its depth. inten is its diffuse light.
 * where its vertex gives a color, inten holds a packed ARGB word's bits
 * instead. spec is its specular light. nx and ny are the x and y of the
 * vertex's normal, as the current matrix turns it. for a vertex with no normal,
 * they are its point's x and y, turned the same way */
typedef struct {
    Float z;
    Float x;
    Float y;
    Float ooz;
    Float inten;
    Float spec;
    Float nx;
    Float ny;
} NJS_CNK_SIMPLE_BUF;

/* a vertex of the easy multi drawer's buffer. z is its depth, and x and y
 * place it on the screen. ooz is 1 over its depth. a, r, g and b are the
 * color its lights give it */
typedef struct {
    Float z;
    Float x;
    Float y;
    Float ooz;
    Float a;
    Float r;
    Float g;
    Float b;
} NJS_CNK_EASYMULTI_BUF;

/* a vertex of the simple multi drawer's buffer. z is its depth, and sx and
 * sy place it on the screen. ooz is 1 over its depth. a, r, g and b are its
 * diffuse color. x and y are its point's, as the current matrix turns it.
 * nx, ny and nz are its normal, turned the same way. sr, sg and sb are its
 * specular color */
typedef struct {
    Float z;
    Float sx;
    Float sy;
    Float ooz;
    Float a;
    Float r;
    Float g;
    Float b;
    Float x;
    Float y;
    Float nx;
    Float ny;
    Float nz;
    Float sr;
    Float sg;
    Float sb;
} NJS_CNK_SIMPLEMULTI_BUF;
/* the words of NJS_CNK_SIMPLEMULTI_BUF's fields that a drawer steps a pointer
 * between, counted from z at word 0 */
#define CNK_SIMPLEMULTI_SX 1
#define CNK_SIMPLEMULTI_A 4
#define CNK_SIMPLEMULTI_X 8
#define CNK_SIMPLEMULTI_NX 10
#define CNK_SIMPLEMULTI_NY 11
#define CNK_SIMPLEMULTI_SR 13

/* a work: what a vertex walker gives a vertex function for one chunk.
 * nbvertex is how many vertices the chunk holds, and vertices points to the
 * first. buf is the buffer entry the first vertex goes to, and sq is
 * SQ_ADDRESS of buf. the walker has run SQ_MAP on buf first. dist_x and
 * dist_y are _nj_screen_.dist times _nj_scr_aspect_x_ and _nj_scr_aspect_y_.
 * (cx, cy) is the screen's center. the vertex functions' bodies, and
 * njCnkModVertex's, read a work's fields at fixed offsets. so no work's layout
 * can change.
 *
 * the easy and the simple drawers' works open with their drawer's light.
 * light is the direction it shines along. inten is its intensity, negated as
 * njCnkSetEasyLightIntensity and njCnkSetSimpleLightIntensity store it.
 * ambient is the light a vertex takes from no direction */
struct nj_cnk_easy_work {
    NJS_VECTOR light;
    Float inten;
    Float ambient;
    Sint32 nbvertex;
    void *vertices;
    NJS_CNK_EASY_BUF *buf;
    Uint32 *sq;
    Float dist_x;
    Float dist_y;
    Float cx;
    Float cy;
    Uint32 reserved[3]; /* the last three words of the 64-byte SDK work */
};
struct nj_cnk_simple_work {
    NJS_VECTOR light;
    Float inten;
    Float ambient;
    Sint32 nbvertex;
    void *vertices;
    NJS_CNK_SIMPLE_BUF *buf;
    Uint32 *sq;
    Float dist_x;
    Float dist_y;
    Float cx;
    Float cy;
    Uint32 reserved[3]; /* the last three words of the 64-byte SDK work */
};
struct nj_cnk_easy_multi_work {
    Sint32 nbvertex;
    void *vertices;
    NJS_CNK_EASYMULTI_BUF *buf;
    Uint32 *sq;
    Float dist_x;
    Float dist_y;
    Float cx;
    Float cy;
};
struct nj_cnk_simple_multi_work {
    Sint32 nbvertex;
    void *vertices;
    NJS_CNK_SIMPLEMULTI_BUF *buf;
    Uint32 *sq;
    Float dist_x;
    Float dist_y;
    Float cx;
    Float cy;
};

/* a vertex of the modifier drawer's buffer. x and y place it on the screen.
 * ooz is 1 over its depth, and z is its depth */
typedef struct {
    Float x;
    Float y;
    Float ooz;
    Float z;
} NJS_CNK_MOD_BUF;
/* the work njCnkModDrawModel gives njCnkModVertex for a model's NJD_CV
 * chunk. vertices points to the chunk's first point */
struct nj_cnk_mod_work {
    Sint32 nbvertex;
    NJS_POINT3 *vertices;
    NJS_CNK_MOD_BUF *buf;
    Uint32 *sq;
    Float dist_x;
    Float dist_y;
    Float cx;
    Float cy;
};
/* project work->nbvertex points, 1 or more, from work->vertices on, to
 * entries of work->buf. the current matrix takes a point to (x, y, z). the
 * entry's z is z. its ooz is 1 / z, or 100000 where z is 1e-5 or less. its x
 * is cx + dist_x * x * ooz. its y is cy + dist_y * y * ooz. the function
 * stores the entries through the store queues, as the easy drawer's vertex
 * functions do. it returns how many of the points have a z of 1e-5 or
 * less */
Sint32 njCnkModVertex(struct nj_cnk_mod_work *work);
/* send the triangles of an NJD_CO_P3 chunk at sq, as the open modifier
 * volume's. tris points to the chunk's first triangle, which is three indices
 * in buf. nbtri is the chunk's halfword just before it. its bits below
 * NJD_UFO_SHIFT count the triangles, which must be 2 or more. its two bits
 * from NJD_UFO_SHIFT count the user halfwords after each triangle, which the
 * function skips.
 *
 * each triangle goes as 64 bytes. its first word is
 * KM_VERTEXPARAM_ENDOFSTRIP. words 1 to 9 are each vertex's x, y and ooz in
 * turn. words 10 to 15 keep whatever the store queue held. after the next to
 * last triangle, the function sends njEndModifier's parameter. the last
 * triangle then closes the volume. the function returns the store queue
 * address after the last triangle. it adds 3 for each triangle to
 * _nj_calc_polygon_ and to _nj_draw_polygon_ */
Uint32 *njCnkModPolygon(Sint16 *tris, NJS_CNK_MOD_BUF *buf, Uint32 nbtri,
                        Uint32 *sq);

/* the easy drawer's vertex functions, each for the vertex chunk types its
 * source names. each projects work->nbvertex vertices, 1 or more, from
 * work->vertices on, to entries of work->buf. each returns how many of the
 * vertices have a z of 1 or less.
 *
 * the current matrix takes a vertex's point to (x, y, z). the entry's ooz is
 * 1 / z. its x is cx + dist_x * x * ooz. its y is cy + dist_y * y * ooz.
 *
 * each function but njCnkEasyVertexVNNF stores the entries in order through
 * the store queues, from work->sq on. work->buf must lie on 32 bytes, and
 * each 32-byte block of it holds two entries. the function drops a block's
 * line from the operand cache, then has the store queue write the block. where
 * the count is odd, the last block's second entry takes whatever the store
 * queue held.
 *
 * a lit function is one whose vertices hold a normal, but for
 * njCnkEasyVertexVND8. the current matrix turns a lit vertex's normal. the
 * light's part is work->inten times the dot product of work->light and the
 * turned normal. a lit entry's inten is work->ambient plus the light's part,
 * or work->ambient where that part is below 0. njCnkEasyVertexSHVN instead
 * takes the greater of work->ambient and the light's part. an unlit
 * function's source says what its entries' inten holds */
Sint32 njCnkEasyVertexSHV(struct nj_cnk_easy_work *work);
Sint32 njCnkEasyVertexSHVN(struct nj_cnk_easy_work *work);
Sint32 njCnkEasyVertexV(struct nj_cnk_easy_work *work);
Sint32 njCnkEasyVertexV1(struct nj_cnk_easy_work *work);
Sint32 njCnkEasyVertexVD8(struct nj_cnk_easy_work *work);
/* the library names njCnkEasyVertexVI and njCnkEasyVertexV1, which differ
 * only in I and 1. clang-tidy flags such a pair as confusable */
/* NOLINTNEXTLINE(misc-confusable-identifiers) */
Sint32 njCnkEasyVertexVI(struct nj_cnk_easy_work *work);
Sint32 njCnkEasyVertexVN(struct nj_cnk_easy_work *work);
Sint32 njCnkEasyVertexVN1(struct nj_cnk_easy_work *work);
Sint32 njCnkEasyVertexVND8(struct nj_cnk_easy_work *work);
/* head is the chunk's first halfword, sign-extended, with NJD_FV_CONT
 * cleared. an NJD_CV_VN_NF vertex is a point, a normal and a word. the
 * word's low halfword, signed, indexes work->buf for the vertex's entry. its
 * high halfword is the vertex's weight, in 255ths. the function scales the
 * weight by the largest float below 1 / 255.
 *
 * the chunks of a series sum their weighted vertices into the same entries.
 * ninja.h's NJD_FW_ flags describe a series. CNK_FLAGS(head) says where the
 * chunk stands. at the first chunk's 0, the function sets each entry to its
 * weighted vertex. at a middle chunk's 1, it adds each weighted vertex to its
 * entry. in both cases it returns 0. at the last chunk's NJD_FW_END, or any
 * other flags, it adds them too. it then projects and lights each entry from
 * its sums as for any lit vertex. it counts as the other functions do.
 *
 * until the series' last chunk, an entry holds sums. its x, y and ooz hold
 * those of its vertices' x, y and z. its inten holds the sum of their light's
 * parts.
 *
 * the function writes work->buf in place, not through the store queues */
Sint32 njCnkEasyVertexVNNF(struct nj_cnk_easy_work *work, Sint32 head);
Sint32 njCnkEasyVertexVNX(struct nj_cnk_easy_work *work);
Sint32 njCnkEasyVertexVNX1(struct nj_cnk_easy_work *work);

/* the simple drawer's vertex functions, each declared in its own source with
 * the vertex chunk types it takes. each projects work->nbvertex vertices as the
 * easy drawer's vertex functions do. an entry's z is its vertex's z. each
 * returns how many of the vertices have a z of 1 or less. njCnkSimpleVertexVN
 * counts those with a z below 1 instead.
 *
 * each entry fills a 32-byte block of its own, and work->buf must lie on 32
 * bytes. each function but njCnkSimpleVertexVNNF stores the entries through the
 * store queues, as the easy functions do.
 *
 * a vertex's light's part is as the easy functions define it. an entry's spec
 * is the 17th power of that part where its vertex holds a normal, else 0.
 * njCnkSimpleVertexVND8 writes no spec, so spec keeps whatever the store queue
 * held. njCnkSimpleVertexVI sets spec from its vertex's word.
 *
 * a vertex's D8, S4, S5 or IN word makes a packed ARGB word. its entry's inten
 * holds that word's bits. each function's source says how it makes the word.
 * where a vertex holds a normal and no such word, its entry's inten is the
 * light's part. a part below 0 is kept. where a vertex holds neither, its
 * entry's inten is 1. no function reads work->ambient: the strip tables' inten
 * functions take a strip's ambient light instead.
 *
 * njCnkSimpleVertexVNNF takes NJD_CV_VN_NF chunks, and reads head and each
 * vertex's word as njCnkEasyVertexVNNF does. it sums each vertex's weighted
 * point and normal, which the current matrix turns first. it chooses among the
 * same three cases by CNK_FLAGS(head), and returns as njCnkEasyVertexVNNF does.
 * until the series' last chunk, an entry holds the sums, not the fields
 * NJS_CNK_SIMPLE_BUF names. the function writes work->buf in place, not through
 * the store queues */
Sint32 njCnkSimpleVertexVNNF(struct nj_cnk_simple_work *work, Sint32 head);

/* the easy drawer's polygon functions. each sends the strips of one strip
 * chunk as vertices of the open polygon. a function stores them through the
 * store queues, from sq on. sq is the address that one of nj_km.h's
 * njStartVertex calls returned. strips points to the chunk's first strip.
 * nbstrip is the chunk's halfword just before the first strip.
 *
 * nbstrip's bits below NJD_UFO_SHIFT count the strips, which must be 1 or
 * more. its two bits from NJD_UFO_SHIFT count the user halfwords after each
 * vertex from a strip's third on. the functions skip them.
 *
 * a strip is a length, then that many vertices. the length must be 3 or
 * more. a negative length marks a reversed winding, and the functions take
 * its magnitude. a vertex is its index in buf, then any halfwords its format
 * adds.
 *
 * each vertex goes as 32 bytes. its first word is KM_VERTEXPARAM_NORMAL, or
 * KM_VERTEXPARAM_ENDOFSTRIP for the last of a strip. a function returns the
 * store queue address after its last vertex, which njEndVertex takes. it
 * adds each strip's length to _nj_calc_polygon_. it adds the count of
 * vertices it sends to _nj_draw_polygon_.
 *
 * njCnkEasyPolygon sends an entry's x, y and ooz as a vertex's words 1 to 3.
 * it sends the entry's inten as word 6. words 4 and 5 keep whatever the store
 * queue held.
 * word 7, which the vertex doesn't use, holds KM_VERTEXPARAM_ENDOFSTRIP */
Uint32 *njCnkEasyPolygon(Sint16 *strips, NJS_CNK_EASY_BUF *buf, Uint32 nbstrip,
                         Uint32 *sq);
/* njCnkEasyPolygon for a textured strip. a vertex of its strips gives a u
 * and a v after the index. the function multiplies each by uvscale for the
 * vertex's texture coordinate. it sends the two as words 4 and 5. word 6, the
 * base intensity, is the entry's inten. word 7, the offset intensity, is that
 * inten less 1. so the offset carries any part of inten above 1 */
Uint32 *njCnkEasyTexture(Sint16 *strips, NJS_CNK_EASY_BUF *buf, Uint32 nbstrip,
                         Uint32 *sq, Float uvscale);
/* njCnkEasyTexture for a bump-mapped strip. every vertex goes with the same
 * base and offset colors, not its entry's inten. the base color is
 * _nj_cnk_easy_bump_lt_color_. the offset color is what njCnkEasyBumpLight
 * gives for the easy light's direction. the function first turns that
 * direction into the bump texture's space. it does so through the current
 * matrix and the texture's vectors */
Uint32 *njCnkEasyTextureBump(Sint16 *strips, NJS_CNK_EASY_BUF *buf,
                             Uint32 nbstrip, Uint32 *sq, Float uvscale);
/* the Clip forms of the three functions above. each serves a model with a
 * vertex whose z is 1 or less. a triangle is whole where each of its vertices
 * has a z of 1 or more. njCnkEasyTextureBumpClip needs each z above 1. a Clip
 * form hands its near clip each other triangle with a vertex whose z is above
 * 1. it drops the rest. a Clip form sends a vertex as its function does.
 * njCnkEasyPolygonClip differs at word 7, which it leaves as the store queue
 * held it */
Uint32 *njCnkEasyPolygonClip(Sint16 *strips, NJS_CNK_EASY_BUF *buf,
                             Uint32 nbstrip, Uint32 *sq);
Uint32 *njCnkEasyTextureClip(Sint16 *strips, NJS_CNK_EASY_BUF *buf,
                             Uint32 nbstrip, Uint32 *sq, Float uvscale);
Uint32 *njCnkEasyTextureBumpClip(Sint16 *strips, NJS_CNK_EASY_BUF *buf,
                                 Uint32 nbstrip, Uint32 *sq, Float uvscale);
/* njCnkEasyPolygonClip's near clip, for the triangle v0, v1, v2. it sends
 * each vertex of its strip at sq as njCnkEasyPolygonClip sends one.
 *
 * the near clip works each corner's point back from its entry's ooz, x and
 * y. the point's z is 1 / ooz. its x is (x - cx) * z * (1 / dist_x). its y
 * is (y - cy) * z * (1 / dist_y). a vertex on the cut lies where an edge
 * crosses a z of 1. the near clip interpolates the points and the intens to
 * that place. the vertex's ooz is 1. its x and y project the point as the
 * easy drawer's vertex functions do, at that ooz. where _nj_cnk_easy_vtype_
 * is NJD_CV_D8, each inten holds a packed ARGB word's bits. the near clip
 * then interpolates each byte of the word.
 *
 * while _nj_flat_model_flag_ is set, the strip's vertices after the first two
 * take v2's inten. the first two go with their inten words unset.
 *
 * the near clip returns the store queue address after its strip. under
 * NJD_CONTROL_3D_DISABLE_NEAR_CLIP it sends nothing and returns sq. it adds
 * the count of vertices it sends to _nj_draw_polygon_. it requires a vertex
 * whose z is above 1. given none, it still sends four vertices, but adds 0 to
 * _nj_draw_polygon_. it sets their control words, and no other word but the
 * intens flat shading writes */
Uint32 *njCnkEasyPolygonNearClip(NJS_CNK_EASY_BUF *v0, NJS_CNK_EASY_BUF *v1,
                                 NJS_CNK_EASY_BUF *v2, Uint32 *sq);
/* njCnkEasyTextureClip's near clip. it works as njCnkEasyPolygonNearClip
 * does, but for a vertex's words and for flat shading. each vertex goes as
 * njCnkEasyTextureClip sends one. (tu0, tv0) is v0's texture coordinate, and
 * the same for v1 and v2. a vertex on the cut takes the texture coordinate
 * interpolated to its place.
 *
 * while _nj_flat_model_flag_ is set, the strip's vertices after the first two
 * take v2's inten. the first two keep their own */
Uint32 *njCnkEasyTextureNearClip(NJS_CNK_EASY_BUF *v0, NJS_CNK_EASY_BUF *v1,
                                 NJS_CNK_EASY_BUF *v2, Uint32 *sq, Float tu0,
                                 Float tv0, Float tu1, Float tv1, Float tu2,
                                 Float tv2);
/* njCnkEasyTextureBumpClip's near clip. it works as njCnkEasyPolygonNearClip
 * does, but for a vertex's words: they hold no inten. each vertex goes as
 * njCnkEasyTextureBumpClip sends one. (tu0, tv0) is v0's texture coordinate,
 * and the same for v1 and v2. a vertex on the cut takes the texture
 * coordinate interpolated to its place. every vertex takes the base and offset
 * colors that only njCnkEasyTextureBumpClip sets. so only that function may
 * call the near clip */
Uint32 *njCnkEasyTextureBumpNearClip(NJS_CNK_EASY_BUF *v0, NJS_CNK_EASY_BUF *v1,
                                     NJS_CNK_EASY_BUF *v2, Uint32 *sq,
                                     Float tu0, Float tv0, Float tu1, Float tv1,
                                     Float tu2, Float tv2);
/* the offset color of a bump-mapped vertex lit along v, a direction in the
 * bump texture's space. the color's top byte is 0x80, and its bits 16 to 23
 * are 0.
 *
 * bits 8 to 15 hold 255 times the cosine of v's angle above the xz plane.
 * where v's y has its sign bit set, the function takes y as 1e-5, just above
 * the plane.
 *
 * bits 0 to 7 hold v's heading about the y axis, in 256ths of a turn. the
 * heading is 0 along +z, 64 along -x, 128 along -z and 192 along +x. a
 * heading from 192 to 255 comes out one less */
Uint32 njCnkEasyBumpLight(NJS_VECTOR *v);
/* the same body as njCnkEasyBumpLight, under the name the simple drawer's bump
 * functions call */
Uint32 njCnkSimpleBumpLight(NJS_VECTOR *v);

/* the simple drawer's polygon functions. each is declared in its own source,
 * with the strips it serves and the words it sends. each sends a strip chunk as
 * the easy drawer's polygon functions do, but for what follows.
 *
 * a triangle is three vertices in a row of a strip: v0, v1 and v2. its winding
 * is the cross product of its edges v1 - v0 and v2 - v1. the edges use the
 * entries' x and y alone. its side is 1 for a strip's first triangle, or -1
 * where the strip's length is negative. each next triangle negates the side.
 * NJD_CONTROL_3D_MIRROR_MODEL negates every side. a triangle's facing is its
 * winding times its side. the triangle faces the viewer where its facing is
 * below 0.
 *
 * a function for strips without NJD_FST_DB drops each triangle that faces away,
 * which ends the run.
 *
 * an inten function serves vertices lit by an intensity, and takes ambient, the
 * strip's ambient light. it sends each vertex's lit inten. that is its entry's
 * inten plus ambient, or ambient where that inten is below 0. an inten function
 * for NJD_FST_DB strips ends a run where the facing changes sign. in a run that
 * faces away, it lights each vertex from its entry's inten and spec negated.
 *
 * an Env function sends CNK_ENV_CENTER plus CNK_ENV_SCALE times an entry's nx
 * and ny as u and v.
 * NJD_CONTROL_3D_ENV_UV_SCROLL offsets them.
 *
 * in a Clip form, a triangle is whole where each vertex's z is above 1. for an
 * inten function, each z need only be 1 or more. njCnkSimplePolygonEnvClip
 * still needs each z above 1 while NJD_CONTROL_3D_ENV_UV_SCROLL is set. a Clip
 * form drops each triangle that isn't whole, which ends the run. it hands its
 * near clip each dropped triangle with a z above 1. njCnkSimplePolygonDbClip
 * also hands it one whose v2 has a z of exactly 1. njCnkSimplePolygonEnvDbClip
 * does so too while NJD_CONTROL_3D_ENV_UV_SCROLL is clear. the near clip sends
 * the part with z above 1 as a strip of 3 or 4 vertices.
 *
 * a Clip form whose function drops or negates by facing passes the triangle's
 * side to the near clip. the near clip then drops or negates as that function
 * does. it finds the facing from the triangle's points, not the entries' x and
 * y. an inten function's Clip form passes ambient too, and its near clip sends
 * each vertex's lit inten */

/* the near clips of four of the simple drawer's Clip forms. a near clip cuts
 * the triangle v0, v1, v2 where z is 1. it sends the part with z above 1 at sq,
 * as a strip of 3 or 4 vertices. it works each corner's point back from its
 * entry's x, y and z. side is the triangle's side, as the Clip form passes it.
 *
 * njCnkSimplePolygonNearClip serves njCnkSimplePolygonClip, and drops a
 * triangle that faces away. njCnkSimplePolygonDbNearClip serves
 * njCnkSimplePolygonDbClip. where the triangle faces away, it negates each
 * entry's inten first. each of the two sends a vertex as its Clip form does,
 * but writes no spec. spec's word, the vertex's last, keeps whatever the store
 * queue held.
 *
 * njCnkSimplePolygonD8NearClip serves njCnkSimplePolygonD8Clip, and drops a
 * triangle that faces away. njCnkSimplePolygonD8DbNearClip serves
 * njCnkSimplePolygonD8DbClip. it takes no side, and drops nothing. each of the
 * two sends a vertex as njCnkSimplePolygonD8 does.
 *
 * a vertex on the cut lies where an edge crosses a z of 1. a near clip
 * interpolates the points and colors of the edge's ends to that place. the
 * vertex's ooz is 1. its x and y project the point as the easy drawer's vertex
 * functions do, at that ooz. njCnkSimplePolygonNearClip and
 * njCnkSimplePolygonDbNearClip interpolate the lit inten. the two D8 near clips
 * interpolate each byte of the packed ARGB word that inten holds.
 *
 * a near clip returns the store queue address after its strip, or sq where it
 * sends nothing. it sends nothing under NJD_CONTROL_3D_DISABLE_NEAR_CLIP. it
 * adds the count of vertices it sends to _nj_draw_polygon_.
 *
 * one of v0, v1 and v2 must have a z above 1. where none does, a near clip that
 * keeps the triangle still sends four vertices. it sets each vertex's first
 * word alone, and adds 0 to _nj_draw_polygon_. njCnkSimplePolygonDbNearClip and
 * njCnkSimplePolygonD8DbNearClip keep every triangle. the other two keep one
 * that faces the viewer */
Uint32 *njCnkSimplePolygonNearClip(NJS_CNK_SIMPLE_BUF *v0,
                                   NJS_CNK_SIMPLE_BUF *v1,
                                   NJS_CNK_SIMPLE_BUF *v2, Uint32 *sq,
                                   Float ambient, Float side);
Uint32 *njCnkSimplePolygonDbNearClip(NJS_CNK_SIMPLE_BUF *v0,
                                     NJS_CNK_SIMPLE_BUF *v1,
                                     NJS_CNK_SIMPLE_BUF *v2, Uint32 *sq,
                                     Float ambient, Float side);
Uint32 *njCnkSimplePolygonD8NearClip(NJS_CNK_SIMPLE_BUF *v0,
                                     NJS_CNK_SIMPLE_BUF *v1,
                                     NJS_CNK_SIMPLE_BUF *v2, Uint32 *sq,
                                     Float side);
Uint32 *njCnkSimplePolygonD8DbNearClip(NJS_CNK_SIMPLE_BUF *v0,
                                       NJS_CNK_SIMPLE_BUF *v1,
                                       NJS_CNK_SIMPLE_BUF *v2, Uint32 *sq);

/* the simple drawer's texture functions, each declared in its own source with
 * the strips it serves. each sends a strip chunk as the polygon function of its
 * form does. so njCnkSimpleTextureDbClip sends one as njCnkSimplePolygonDbClip
 * does. njCnkSimpleTextureBump has no polygon form, and its source says what it
 * sends. the paragraphs below say where a texture function differs.
 *
 * a vertex of a texture function's strips gives a u and a v after its index.
 * each is a signed halfword. the function multiplies each by uvscale for the
 * vertex's texture coordinate. an Env function skips the two halfwords. its
 * texture coordinate is the polygon Env functions' u and v.
 *
 * a sent vertex's words 1 to 3 are its entry's x, y and ooz. words 4 and 5 are
 * its texture coordinate. an inten function sends the lit inten as word 6, and
 * the entry's spec as word 7. in a run that faces away, an inten function for
 * NJD_FST_DB strips negates both first. a D8 function sends its entry's inten
 * and spec as they are. njCnkSimpleTextureBump sends its base and offset colors
 * as words 6 and 7.
 *
 * a Clip form's near clip is the texture near clip below for its form. a Clip
 * form other than an Env one passes it each vertex's texture coordinate. a
 * polygon Env function's Clip form takes the texture Env near clip of its form.
 * the two forms send the same vertices */

/* the near clips of the texture functions' Clip forms. a near clip cuts the
 * triangle v0, v1, v2 where z is 1. it sends the part with z above 1 at sq, as
 * a strip of 3 or 4 vertices. it works each corner's point back from its
 * entry's x, y and z. a near clip serves the Clip form its name gives, as
 * njCnkSimpleTextureDbNearClip serves njCnkSimpleTextureDbClip. it sends a
 * vertex as that form does.
 *
 * (tu0, tv0) is v0's texture coordinate, and the same for v1 and v2. an Env
 * near clip takes none, and finds each as the polygon Env functions find u and
 * v. it offsets them while NJD_CONTROL_3D_ENV_UV_SCROLL is set. it does so even
 * for a form that never offsets its own vertices, as
 * _njCnkSimpleTextureEnvClip. ambient is the strip's ambient light, which every
 * near clip but a D8 one takes. side is the triangle's side.
 *
 * a near clip that takes side drops a triangle that faces away. it finds the
 * facing from the triangle's points, not the entries' x and y.
 * njCnkSimpleTextureDbNearClip and njCnkSimpleTextureEnvDbNearClip keep such a
 * triangle instead. they light each vertex from its entry's inten and spec
 * negated. the D8 near clips for NJD_FST_DB strips take no side, and drop
 * nothing.
 *
 * a vertex on the cut lies where an edge crosses a z of 1. a near clip
 * interpolates the edge's ends to that place. it interpolates their points,
 * texture coordinates and the inten and spec they would be sent with. in a D8
 * near clip, inten and spec hold packed ARGB words. it interpolates each byte,
 * and drops the byte's fraction. the vertex's ooz is 1. its x and y project the
 * point as the easy drawer's vertex functions do, at that ooz.
 *
 * a near clip returns and counts as the polygon near clips above do. it
 * requires a z above 1 as they do. the four near clips for NJD_FST_DB strips
 * keep every triangle. the others keep one that faces the viewer */
Uint32 *njCnkSimpleTextureNearClip(NJS_CNK_SIMPLE_BUF *v0,
                                   NJS_CNK_SIMPLE_BUF *v1,
                                   NJS_CNK_SIMPLE_BUF *v2, Uint32 *sq,
                                   Float tu0, Float tv0, Float tu1, Float tv1,
                                   Float tu2, Float tv2, Float ambient,
                                   Float side);
Uint32 *njCnkSimpleTextureDbNearClip(NJS_CNK_SIMPLE_BUF *v0,
                                     NJS_CNK_SIMPLE_BUF *v1,
                                     NJS_CNK_SIMPLE_BUF *v2, Uint32 *sq,
                                     Float tu0, Float tv0, Float tu1, Float tv1,
                                     Float tu2, Float tv2, Float ambient,
                                     Float side);
Uint32 *njCnkSimpleTextureEnvNearClip(NJS_CNK_SIMPLE_BUF *v0,
                                      NJS_CNK_SIMPLE_BUF *v1,
                                      NJS_CNK_SIMPLE_BUF *v2, Uint32 *sq,
                                      Float ambient, Float side);
Uint32 *njCnkSimpleTextureEnvDbNearClip(NJS_CNK_SIMPLE_BUF *v0,
                                        NJS_CNK_SIMPLE_BUF *v1,
                                        NJS_CNK_SIMPLE_BUF *v2, Uint32 *sq,
                                        Float ambient, Float side);
Uint32 *njCnkSimpleTextureD8NearClip(NJS_CNK_SIMPLE_BUF *v0,
                                     NJS_CNK_SIMPLE_BUF *v1,
                                     NJS_CNK_SIMPLE_BUF *v2, Uint32 *sq,
                                     Float tu0, Float tv0, Float tu1, Float tv1,
                                     Float tu2, Float tv2, Float side);
Uint32 *njCnkSimpleTextureD8DbNearClip(NJS_CNK_SIMPLE_BUF *v0,
                                       NJS_CNK_SIMPLE_BUF *v1,
                                       NJS_CNK_SIMPLE_BUF *v2, Uint32 *sq,
                                       Float tu0, Float tv0, Float tu1,
                                       Float tv1, Float tu2, Float tv2);
Uint32 *njCnkSimpleTextureD8EnvNearClip(NJS_CNK_SIMPLE_BUF *v0,
                                        NJS_CNK_SIMPLE_BUF *v1,
                                        NJS_CNK_SIMPLE_BUF *v2, Uint32 *sq,
                                        Float side);
Uint32 *njCnkSimpleTextureD8EnvDbNearClip(NJS_CNK_SIMPLE_BUF *v0,
                                          NJS_CNK_SIMPLE_BUF *v1,
                                          NJS_CNK_SIMPLE_BUF *v2, Uint32 *sq);

/* draw model as njCnkEasyDrawModel and njCnkSimpleDrawModel draw it,
 * between calls of njGetkmPointer and njSetkmPointer that the caller makes.
 * each returns -1 where the model clip skips the model, else 0 */
Sint32 _njCnkEasyDrawModel(NJS_CNK_MODEL *model);
Sint32 _njCnkSimpleDrawModel(NJS_CNK_MODEL *model);
/* draw model as njCnkEasyMultiDrawModel and njCnkSimpleMultiDrawModel draw
 * it. the caller makes the calls of njGetkmPointer, the drawer's InitVertex
 * call and njSetkmPointer around it. each returns -1 where the model clip
 * skips the model. each also returns -1 where the drawer's vertex walker
 * meets a vertex chunk it can't draw. the call then draws no strip. each
 * returns 0 otherwise */
Sint32 _njCnkEasyMultiDrawModel(NJS_CNK_MODEL *model);
Sint32 _njCnkSimpleMultiDrawModel(NJS_CNK_MODEL *model);
/* _njCnkEasyDrawObject, _njCnkSimpleDrawObject, their Multi forms and
 * _njCnkModDrawObject walk an object's nodes. each keeps in clipped what a
 * node's draw returns, where the node doesn't keep its parent's matrix. under
 * NJD_EVAL_CLIP, the walker skips the node's children where clipped isn't 0.
 * a node under NJD_EVAL_HIDE draws no model, so its test reads a clipped that
 * no draw set. without NJD_EVAL_UNIT_ANG, clipped then holds the node's
 * rotation order, and NJD_EVAL_ZXY_ANG skips the node's children. with
 * NJD_EVAL_UNIT_ANG, clipped holds an earlier sibling's value, or the caller's
 * R13. the GCC side starts clipped at 0, since C can't read the caller's R13 */

/* copy the projection to the easy multi or the simple multi drawer's work.
 * the copy is _nj_screen_'s dist times each aspect, and its cx and cy */
void njCnkInitEasyMultiVertex(void);
void njCnkInitSimpleMultiVertex(void);

/* whether model's bounding sphere lies outside the view: TRUE where it
 * does, else FALSE. the current matrix takes the sphere's center to
 * (x, y, z). r is model->r, which the matrix doesn't scale.
 *
 * the sphere lies outside where z is above _nj_far_clip_ + r. it lies
 * outside where z + r is at most _nj_near_clip_. it also lies outside where
 * a square of side 2r about (x, y) projects wholly outside _nj_2d_clip_. each
 * corner projects as njProjectScreen projects a point, at the depth z + r.
 * _nj_2d_clip_ is four floats: the least x and y on the screen, then the
 * greatest */
Bool njCnkModelClip(NJS_CNK_MODEL *model);

/* The strip functions write through the store queue and return its next
 * address. Intensity functions also take the strip's ambient light. */
typedef Uint32 *NJS_CNK_SIMPLE_POLYGON(Sint16 *strips, NJS_CNK_SIMPLE_BUF *buf,
                                       Uint32 nbstrip, Uint32 *sq);
typedef Uint32 *NJS_CNK_SIMPLE_INTEN_POLYGON(Sint16 *strips,
                                             NJS_CNK_SIMPLE_BUF *buf,
                                             Uint32 nbstrip, Uint32 *sq,
                                             Float ambient);
typedef Uint32 *NJS_CNK_SIMPLE_TEXTURE(Sint16 *strips, NJS_CNK_SIMPLE_BUF *buf,
                                       Uint32 nbstrip, Uint32 *sq,
                                       Float uvscale);
typedef Uint32 *NJS_CNK_SIMPLE_INTEN_TEXTURE(Sint16 *strips,
                                             NJS_CNK_SIMPLE_BUF *buf,
                                             Uint32 nbstrip, Uint32 *sq,
                                             Float ambient, Float uvscale);
typedef Sint32 NJS_CNK_SIMPLE_VERTEX(struct nj_cnk_simple_work *work);

/* a row of the simple drawer's strip tables, for one value of a strip
 * chunk's flags. its first four fields serve NJD_CS strips, which hold no u
 * or v, environment mapped ones among them. its last four serve NJD_CS_UVN
 * and NJD_CS_UVH strips. an inten field serves vertices lit by an
 * intensity, and the others vertices that hold their color. a mode field is
 * the color mode the walker sets for the strips. an inten function also
 * takes ambient, the strip's ambient light */
struct nj_cnk_simple_strip {
    Uint32 inten_mode;
    Uint32 mode;
    NJS_CNK_SIMPLE_INTEN_POLYGON *inten_polygon;
    NJS_CNK_SIMPLE_POLYGON *polygon;
    Uint32 inten_texture_mode;
    Uint32 texture_mode;
    NJS_CNK_SIMPLE_INTEN_TEXTURE *inten_texture;
    NJS_CNK_SIMPLE_TEXTURE *texture;
};
/* a row of the simple drawer's vertex table, for one vertex chunk type.
 * func is the type's vertex function. inten says whether func lights each
 * vertex by an intensity */
struct nj_cnk_simple_vertex {
    NJS_CNK_SIMPLE_VERTEX *func;
    Bool inten;
};

/* A vertex chunk's type less NJD_VERTOFF indexes the vertex table. The
 * strip tables have one row for each combination of the seven NJD_FST_ flags.
 * The Clip tables serve a model with a vertex near the eye. */
extern struct nj_cnk_simple_vertex _njCnkVertexFuncTableS[];
extern struct nj_cnk_simple_strip _njCnkFunctionS[];
extern struct nj_cnk_simple_strip _njCnkFunctionClipS[];
/* @hack type: the bump functions as arrays, called through their first
 * entries, where a function pointer each would do */
extern NJS_CNK_SIMPLE_INTEN_TEXTURE *_njCnkBumpFunctionS[];
extern NJS_CNK_SIMPLE_INTEN_TEXTURE *_njCnkBumpFunctionClipS[];

/* the easy multi drawer's vertex functions, each for the vertex chunk type its
 * name gives. each lights and projects work->nbvertex vertices, 1 or more, from
 * work->vertices on, to entries of work->buf. a DepthQueue function serves
 * while _nj_depth_enable_ is set.
 *
 * the current matrix takes a vertex's point to p, and its normal to n. the
 * entry's z is p's z. its ooz, x and y are as for the easy drawer's vertex
 * functions. each function returns how many of the vertices have a z below 1.
 *
 * the lights are numbered 1 to 6, as ninja.h numbers them. light n is the 8
 * floats from _nj_cnk_easy_multilight_buffer_dst_ + 8(n - 1). the first three
 * are a vector v, which njCnkSetEasyMultiLightMatrices puts in the view's
 * space. the next two are the squares of the light's far and near ranges. the
 * last three are its color's r, g and b.
 *
 * the first word of _nj_cnk_easy_multilight_buffer_mdst_ gives each light three
 * bits. light n's on bit is bit 3(n - 1). its point bit is the next, and its
 * last bit the one after. a function reads the lights in turn from light 1. it
 * stops after the first whose last bit is set.
 *
 * the entry's r, g and b sum the parts of the lights whose on bit is set. no
 * sum is clamped. a light without its point bit shines along v. its part is its
 * color times -(v . n), and nothing where that is 0 or less. a point light lies
 * at v. let d be v - p. its part is nothing unless d . n is above 0 and d . d
 * is below the far range's square. its part is then its color times
 * (d . n) / |d|. where d . d is above the near range's square, the part is also
 * scaled by that square over d . d. n is taken as it stands, not normalized.
 *
 * the entry's a is 1. an NJD_CV_VN_D8 vertex holds a packed ARGB word after its
 * normal. njCnkEasyMultiVertexVND8 scales each sum by the word's byte for it
 * over 128. the entry's a is the word's alpha byte over 128.
 *
 * a DepthQueue function scales a by the fade at the entry's z. the fade is 1 at
 * a z of _nj_depth_near_ or less, and 0 at _nj_depth_far_ or more. between them
 * it is 1 - (z - _nj_depth_near_) * _nj_depth_range_.
 *
 * each entry fills a 32-byte block of its own, and work->buf must lie on 32
 * bytes. the VN and VND8 functions store each entry through the store queues,
 * from work->sq on. each drops the entry's line from the operand cache, then
 * has the store queue write the entry.
 *
 * the VNNF functions take NJD_CV_VN_NF chunks, and read head and each vertex's
 * word as njCnkEasyVertexVNNF does. the weight scales p and n. they choose
 * among the same three cases by CNK_FLAGS(head), and return as
 * njCnkEasyVertexVNNF does. at the series' last chunk, they light and project
 * each entry from its sums. until then, the entry's words 0 to 2 hold p's sum,
 * and words 4 to 6 n's. the VNNF functions write work->buf in place, not
 * through the store queues */
Sint32 njCnkEasyMultiVertexVN(struct nj_cnk_easy_multi_work *work);
Sint32 njCnkEasyMultiVertexVND8(struct nj_cnk_easy_multi_work *work);
Sint32 njCnkEasyMultiVertexVNNF(struct nj_cnk_easy_multi_work *work,
                                Sint32 head);
Sint32 njCnkEasyMultiVertexVNDepthQueue(struct nj_cnk_easy_multi_work *work);
Sint32 njCnkEasyMultiVertexVND8DepthQueue(struct nj_cnk_easy_multi_work *work);
Sint32 njCnkEasyMultiVertexVNNFDepthQueue(struct nj_cnk_easy_multi_work *work,
                                          Sint32 head);

/* the easy multi drawer's polygon functions. each sends the strips of one strip
 * chunk as the easy drawer's polygon functions do. it returns the same address,
 * and adds to the same counts. only the words of a vertex differ.
 *
 * a vertex's color is its entry's, lit by the parameters. its a is a times the
 * entry's a. its r is the entry's r times r, plus ar. its g and b are the
 * entry's times g and b, plus ag and ab. a Texture function takes no r, g or b.
 * it adds ar, ag and ab to the entry's own.
 *
 * the Polygon functions send each vertex as 32 bytes. its words 1 to 3 are the
 * entry's x, y and ooz. words 4 to 7 are the color's a, r, g and b.
 *
 * the Texture functions send each vertex as 64 bytes. its words 1 to 3 are the
 * entry's x, y and ooz. words 4 and 5 are its u and v from the strip, each
 * times uvscale. words 6 and 7 keep whatever the store queue held. words 8 to
 * 11 are the color, as the base color. words 12 to 15 are the offset color. its
 * a is the same, and its r, g and b are each 1 less. the hardware holds each
 * channel of a color to 0..1. so the base color gives each of r, g and b up to
 * 1. the offset color gives what lies above.
 *
 * a Clip form serves a model with a vertex whose z is below 1. a triangle is
 * whole where each of its vertices has a z of 1 or more. the Clip form's near
 * clip takes each other triangle with a vertex whose z is 1 or more. the form
 * drops the rest */
Uint32 *njCnkEasyMultiPolygon(Sint16 *strips, NJS_CNK_EASYMULTI_BUF *buf,
                              Sint32 nbstrip, Uint32 *sq, Float a, Float r,
                              Float g, Float b, Float ar, Float ag, Float ab);
Uint32 *njCnkEasyMultiPolygonClip(Sint16 *strips, NJS_CNK_EASYMULTI_BUF *buf,
                                  Sint32 nbstrip, Uint32 *sq, Float a, Float r,
                                  Float g, Float b, Float ar, Float ag,
                                  Float ab);
Uint32 *njCnkEasyMultiTexture(Sint16 *strips, NJS_CNK_EASYMULTI_BUF *buf,
                              Sint32 nbstrip, Uint32 *sq, Float a, Float ar,
                              Float ag, Float ab, Float uvscale);
Uint32 *njCnkEasyMultiTextureClip(Sint16 *strips, NJS_CNK_EASYMULTI_BUF *buf,
                                  Sint32 nbstrip, Uint32 *sq, Float a, Float ar,
                                  Float ag, Float ab, Float uvscale);
/* the near clips of njCnkEasyMultiPolygonClip and njCnkEasyMultiTextureClip. a
 * near clip cuts the triangle v0, v1, v2 where z is 1. it sends the part with z
 * above 1 at sq, as a strip of 3 or 4 vertices. each vertex goes as the Clip
 * form sends one, lit by the same parameters. njCnkEasyMultiTextureNearClip
 * leaves each vertex's word 12, the offset color's a, unset. (tu0, tv0) is v0's
 * texture coordinate, and the same for v1 and v2.
 *
 * a near clip works each corner's point back from its entry. the point's x is
 * (x - cx) * z * (1 / dist_x). its y is (y - cy) * z * (1 / dist_y). its z is
 * the entry's z. the near clip finds each reciprocal once. a vertex on the cut
 * lies where an edge crosses a z of 1. the near clip interpolates the points,
 * the entries' colors and the texture coordinates to that place. the vertex's
 * ooz is 1. its x and y project the point as the easy drawer's vertex functions
 * do, at that ooz.
 *
 * while _nj_flat_model_flag_ is set, a near clip colors only the strip's
 * vertices after the first two. they take v2's color. the first two vertices go
 * with their color words unset.
 *
 * a near clip returns the store queue address after its strip. under
 * NJD_CONTROL_3D_DISABLE_NEAR_CLIP it sends nothing and returns sq. it adds the
 * count of vertices it sends to _nj_draw_polygon_. one of v0, v1 and v2 must
 * have a z above 1. given none, it sends four vertices with unset words, but
 * adds 0 to _nj_draw_polygon_ */
Uint32 *njCnkEasyMultiPolygonNearClip(NJS_CNK_EASYMULTI_BUF *v0,
                                      NJS_CNK_EASYMULTI_BUF *v1,
                                      NJS_CNK_EASYMULTI_BUF *v2, Uint32 *sq,
                                      Float a, Float r, Float g, Float b,
                                      Float ar, Float ag, Float ab);
Uint32 *njCnkEasyMultiTextureNearClip(NJS_CNK_EASYMULTI_BUF *v0,
                                      NJS_CNK_EASYMULTI_BUF *v1,
                                      NJS_CNK_EASYMULTI_BUF *v2, Uint32 *sq,
                                      Float tu0, Float tv0, Float tu1,
                                      Float tv1, Float tu2, Float tv2, Float a,
                                      Float ar, Float ag, Float ab);

/* the simple multi drawer's vertex functions. each works as the easy multi
 * function with its suffix does, but for what follows. the lights are those of
 * _nj_cnk_simple_multilight_buffer_mdst_ and
 * _nj_cnk_simple_multilight_buffer_dst_, laid out as the easy multi drawer's
 * are.
 *
 * each entry is 64 bytes: two blocks of 32. its sx and sy are what the easy
 * multi entry calls x and y. its x and y are p's, and its nx, ny and nz are
 * n's. its sr, sg and sb are its r, g and b, each less 1. the VN and VND8
 * functions store each block as the easy multi functions store an entry. a VNNF
 * function keeps its sums in the entry's first 32 bytes, as the easy multi
 * functions do */
Sint32 njCnkSimpleMultiVertexVN(struct nj_cnk_simple_multi_work *work);
Sint32 njCnkSimpleMultiVertexVND8(struct nj_cnk_simple_multi_work *work);
Sint32 njCnkSimpleMultiVertexVNNF(struct nj_cnk_simple_multi_work *work,
                                  Sint32 head);
Sint32
njCnkSimpleMultiVertexVNDepthQueue(struct nj_cnk_simple_multi_work *work);
Sint32
njCnkSimpleMultiVertexVND8DepthQueue(struct nj_cnk_simple_multi_work *work);
/* njCnkSimpleMultiVertexVNNFDepthQueue's sr, sg and sb are its r, g and b,
 * each less the fade rather than 1 */
Sint32
njCnkSimpleMultiVertexVNNFDepthQueue(struct nj_cnk_simple_multi_work *work,
                                     Sint32 head);

/* the simple multi drawer's polygon functions, for a model with no vertex whose
 * z is below 1. each sends a strip chunk as its Clip form below does, but takes
 * every triangle as whole. so each sends each run of triangles that face the
 * viewer as a strip of its own. it hands none to a near clip.
 *
 * njCnkSimpleMultiTexture also sends the entry's nz as a vertex's word 12. so
 * do njCnkSimpleMultiPolygonEnv and njCnkSimpleMultiTextureEnv while
 * NJD_CONTROL_3D_ENV_UV_SCROLL is clear. while it is clear, the two also fail
 * to skip the user halfwords after a strip's last triangle. where nbstrip
 * counts user halfwords, they read the next strip from them */
Uint32 *njCnkSimpleMultiPolygon(Sint16 *strips, NJS_CNK_SIMPLEMULTI_BUF *buf,
                                Uint32 nbstrip, Uint32 *sq, Float a, Float r,
                                Float g, Float b, Float ar, Float ag, Float ab);
Uint32 *njCnkSimpleMultiPolygonEnv(Sint16 *strips, NJS_CNK_SIMPLEMULTI_BUF *buf,
                                   Uint32 nbstrip, Uint32 *sq, Float a,
                                   Float ar, Float ag, Float ab);
Uint32 *njCnkSimpleMultiPolygonDiffEnv(Sint16 *strips,
                                       NJS_CNK_SIMPLEMULTI_BUF *buf,
                                       Uint32 nbstrip, Uint32 *sq, Float a,
                                       Float r, Float g, Float b, Float ar,
                                       Float ag, Float ab);
Uint32 *njCnkSimpleMultiTexture(Sint16 *strips, NJS_CNK_SIMPLEMULTI_BUF *buf,
                                Uint32 nbstrip, Uint32 *sq, Float uvscale,
                                Float a, Float ar, Float ag, Float ab);
Uint32 *njCnkSimpleMultiTextureDiff(Sint16 *strips,
                                    NJS_CNK_SIMPLEMULTI_BUF *buf,
                                    Uint32 nbstrip, Uint32 *sq, Float uvscale,
                                    Float a, Float r, Float g, Float b,
                                    Float ar, Float ag, Float ab);
Uint32 *njCnkSimpleMultiTextureEnv(Sint16 *strips, NJS_CNK_SIMPLEMULTI_BUF *buf,
                                   Uint32 nbstrip, Uint32 *sq, Float a,
                                   Float ar, Float ag, Float ab);
Uint32 *njCnkSimpleMultiTextureDiffEnv(Sint16 *strips,
                                       NJS_CNK_SIMPLEMULTI_BUF *buf,
                                       Uint32 nbstrip, Uint32 *sq, Float a,
                                       Float r, Float g, Float b, Float ar,
                                       Float ag, Float ab);

/* the Clip forms of the simple multi drawer's polygon functions. each serves a
 * model with a vertex whose z is below 1. each sends a strip chunk as the easy
 * drawer's polygon functions do, but for what follows.
 *
 * a triangle's side and facing are as the simple drawer's polygon functions
 * define them. the winding uses the entries' sx and sy. a triangle is whole
 * where each vertex's z is above 1. a function sends each run of whole
 * triangles that face the viewer as a strip of its own. a triangle that faces
 * away or isn't whole ends the run.
 *
 * a function hands its near clip each triangle that isn't whole but passes two
 * tests. the triangle must have a vertex whose z is 1 or more. its points must
 * face the viewer: CNK_POINT_WINDING times its side must be below 0.
 *
 * njCnkSimpleMultiPolygonClip sends each vertex as 32 bytes. its words 1 to 3
 * are the entry's sx, sy and ooz. words 4 to 7 are the vertex's color: its a,
 * r, g and b. the other functions send each vertex as 64 bytes. their words 1
 * to 3 are the same. words 4 and 5 hold a texture coordinate, and words 8 to 11
 * the color. words 13 to 15 hold the entry's sr, sg and sb. words 6, 7 and 12
 * keep whatever the store queue held.
 *
 * a vertex's color is its entry's, lit by the parameters. the parameter a
 * scales the entry's a. ar, ag and ab add to the entry's r, g and b. where a
 * function also takes r, g and b, it first scales the entry's by them.
 *
 * an Env function's texture coordinate is CNK_ENV_CENTER plus CNK_ENV_SCALE
 * times the entry's nx and ny.
 * NJD_CONTROL_3D_ENV_UV_SCROLL adds _nj_uv_scroll_u_ and _nj_uv_scroll_v_ to
 * it. a Texture function's strips give a u and a v after each index.
 * njCnkSimpleMultiTextureClip and njCnkSimpleMultiTextureDiffClip multiply each
 * by uvscale for the texture coordinate. njCnkSimpleMultiTextureEnvClip and
 * njCnkSimpleMultiTextureDiffEnvClip skip them */
Uint32 *njCnkSimpleMultiPolygonClip(Sint16 *strips,
                                    NJS_CNK_SIMPLEMULTI_BUF *buf,
                                    Uint32 nbstrip, Uint32 *sq, Float a,
                                    Float r, Float g, Float b, Float ar,
                                    Float ag, Float ab);
Uint32 *njCnkSimpleMultiPolygonEnvClip(Sint16 *strips,
                                       NJS_CNK_SIMPLEMULTI_BUF *buf,
                                       Uint32 nbstrip, Uint32 *sq, Float a,
                                       Float ar, Float ag, Float ab);
Uint32 *njCnkSimpleMultiPolygonDiffEnvClip(Sint16 *strips,
                                           NJS_CNK_SIMPLEMULTI_BUF *buf,
                                           Uint32 nbstrip, Uint32 *sq, Float a,
                                           Float r, Float g, Float b, Float ar,
                                           Float ag, Float ab);
Uint32 *njCnkSimpleMultiTextureClip(Sint16 *strips,
                                    NJS_CNK_SIMPLEMULTI_BUF *buf,
                                    Uint32 nbstrip, Uint32 *sq, Float uvscale,
                                    Float a, Float ar, Float ag, Float ab);
Uint32 *njCnkSimpleMultiTextureDiffClip(Sint16 *strips,
                                        NJS_CNK_SIMPLEMULTI_BUF *buf,
                                        Uint32 nbstrip, Uint32 *sq,
                                        Float uvscale, Float a, Float r,
                                        Float g, Float b, Float ar, Float ag,
                                        Float ab);
Uint32 *njCnkSimpleMultiTextureEnvClip(Sint16 *strips,
                                       NJS_CNK_SIMPLEMULTI_BUF *buf,
                                       Uint32 nbstrip, Uint32 *sq, Float a,
                                       Float ar, Float ag, Float ab);
Uint32 *njCnkSimpleMultiTextureDiffEnvClip(Sint16 *strips,
                                           NJS_CNK_SIMPLEMULTI_BUF *buf,
                                           Uint32 nbstrip, Uint32 *sq, Float a,
                                           Float r, Float g, Float b, Float ar,
                                           Float ag, Float ab);
/* a triangle's winding, found from the points of entries v0, v1 and v2. it
 * uses each entry's x, y and z, not its sx and sy. it is the triple product
 * v0 . (v1 x v2) */
#define CNK_POINT_WINDING(v0, v1, v2)                                          \
    (((v2)->x * (((v1)->z * (v0)->y) - ((v1)->y * (v0)->z))) +                 \
     ((v2)->y * (((v1)->x * (v0)->z) - ((v1)->z * (v0)->x))) +                 \
     ((v2)->z * (((v1)->y * (v0)->x) - ((v1)->x * (v0)->y))))
/* the z of the near plane, where a near clip cuts a triangle */
#define CNK_NEAR_Z 1.0F
/* the ooz of a vertex on the near plane */
#define CNK_NEAR_OOZ (1.0F / CNK_NEAR_Z)
/* the corners of the triangle a near clip cuts, and the most vertices of the
 * strip it sends */
#define CNK_TRI_CORNERS 3
#define CNK_CLIP_VERTICES 4

/* the near clips of six of the Clip functions above. a near clip cuts the
 * triangle v0, v1, v2 where z is 1. it sends the part with z above 1 at sq, as
 * a strip of 3 or 4 vertices. each vertex goes as the Clip functions it serves
 * send one.
 *
 * njCnkSimpleMultiTextureEnvNearClip serves njCnkSimpleMultiPolygonEnvClip and
 * njCnkSimpleMultiTextureEnvClip. njCnkSimpleMultiTextureDiffEnvNearClip serves
 * njCnkSimpleMultiPolygonDiffEnvClip and njCnkSimpleMultiTextureDiffEnvClip.
 * njCnkSimpleMultiTextureNearClip serves njCnkSimpleMultiTextureClip, and
 * njCnkSimpleMultiTextureDiffNearClip serves njCnkSimpleMultiTextureDiffClip.
 * (tu0, tv0) is v0's texture coordinate, and the same for v1 and v2.
 *
 * a vertex on the cut lies where an edge crosses a z of 1. a near clip
 * interpolates the values of the edge's ends to that place. those values are x
 * and y, the color with sr, sg and sb, and the texture coordinate. the vertex's
 * ooz is 1. its sx and sy project x and y as the easy drawer's vertex functions
 * do, at that ooz.
 *
 * while _nj_flat_model_flag_ is set, a near clip colors only the strip's
 * vertices after the first two. they take v2's color, sr, sg and sb. flat
 * shading takes a triangle's color from its last vertex, so each triangle shows
 * v2's. the first two vertices go with those words unset. under flat shading,
 * njCnkSimpleMultiTextureDiffEnvNearClip and
 * njCnkSimpleMultiTextureDiffNearClip wrongly add ab, not ag, to the strip's
 * last vertex's g.
 *
 * a near clip returns the store queue address after its strip. under
 * NJD_CONTROL_3D_DISABLE_NEAR_CLIP it sends nothing and returns sq. it adds the
 * count of vertices it sends to _nj_draw_polygon_. it requires a vertex whose z
 * is above 1. given none, it sends four vertices but adds 0 to
 * _nj_draw_polygon_. it never sets their sx, sy, ooz or texture coordinate, nor
 * any color but those flat shading writes */
Uint32 *njCnkSimpleMultiTextureEnvNearClip(NJS_CNK_SIMPLEMULTI_BUF *v0,
                                           NJS_CNK_SIMPLEMULTI_BUF *v1,
                                           NJS_CNK_SIMPLEMULTI_BUF *v2,
                                           Uint32 *sq, Float a, Float ar,
                                           Float ag, Float ab);
Uint32 *njCnkSimpleMultiTextureDiffEnvNearClip(NJS_CNK_SIMPLEMULTI_BUF *v0,
                                               NJS_CNK_SIMPLEMULTI_BUF *v1,
                                               NJS_CNK_SIMPLEMULTI_BUF *v2,
                                               Uint32 *sq, Float a, Float r,
                                               Float g, Float b, Float ar,
                                               Float ag, Float ab);
Uint32 *njCnkSimpleMultiTextureNearClip(NJS_CNK_SIMPLEMULTI_BUF *v0,
                                        NJS_CNK_SIMPLEMULTI_BUF *v1,
                                        NJS_CNK_SIMPLEMULTI_BUF *v2, Uint32 *sq,
                                        Float tu0, Float tv0, Float tu1,
                                        Float tv1, Float tu2, Float tv2,
                                        Float a, Float ar, Float ag, Float ab);
Uint32 *njCnkSimpleMultiTextureDiffNearClip(
    NJS_CNK_SIMPLEMULTI_BUF *v0, NJS_CNK_SIMPLEMULTI_BUF *v1,
    NJS_CNK_SIMPLEMULTI_BUF *v2, Uint32 *sq, Float tu0, Float tv0, Float tu1,
    Float tv1, Float tu2, Float tv2, Float a, Float r, Float g, Float b,
    Float ar, Float ag, Float ab);
/* njCnkSimpleMultiPolygonClip's near clip. it keeps the contract above,
 * but sends each vertex as njCnkSimpleMultiPolygonClip sends one. the
 * vertex's 32 bytes hold no texture coordinate and no sr, sg or sb */
Uint32 *njCnkSimpleMultiPolygonNearClip(NJS_CNK_SIMPLEMULTI_BUF *v0,
                                        NJS_CNK_SIMPLEMULTI_BUF *v1,
                                        NJS_CNK_SIMPLEMULTI_BUF *v2, Uint32 *sq,
                                        Float a, Float r, Float g, Float b,
                                        Float ar, Float ag, Float ab);

#endif
