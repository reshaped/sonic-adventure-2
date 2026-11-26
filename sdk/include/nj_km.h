/* the calls through which Ninja's own draws send polygons straight to
 * Kamui's lists */
#ifndef NJ_KM_H
#define NJ_KM_H
#include <sg_xpt.h>

/* a store to an address in SQ_AREA goes to a store queue. a prefetch of the
 * address writes the queue to memory. the queue lands at the address's
 * SQ_OFFSET bits, within a 64 MB area. the queue's register, QACR0 or QACR1,
 * names that area. the register's QACR_AREA bits hold bits 26 to 28 of the
 * area's address */
#define SQ_AREA 0xe0000000
#define SQ_OFFSET 0x03ffffff
/* @hack type: QACR0 and QACR1 as plain Uint32s, where volatile ones would
 * do */
#define QACR0 (*(Uint32 *)0xff000038)
#define QACR1 (*(Uint32 *)0xff00003c)
#define QACR_AREA 0x1c
#define QACR_AREA_SHIFT 24
/* the store queue address that writes to address */
#define SQ_ADDRESS(address) ((Uint32 *)(((address) & SQ_OFFSET) | SQ_AREA))
/* have both store queues write to the 64 MB area that holds address */
/* @hack spelling: both registers set in one chained assignment, where a
 * statement for each would do */
#define SQ_MAP(address)                                                        \
    (QACR0 = QACR1 = ((address) >> QACR_AREA_SHIFT) & QACR_AREA)
/* a store queue holds SQ_WORDS words, a 32-byte block */
#define SQ_WORDS 8

/* njGetkmPointer copies _nj_current_ctx_'s four PARAMBUFFER words: the
 * global, ISP, TSP and texture words that open a polygon's parameter. it
 * also copies the address each list of _nj_vertex_buffer_ writes next. the
 * calls below read and change the copy, not the context or the buffer.
 * njSetkmPointer writes the copy back. so a draw makes its calls between the
 * two. NJD_CONTROL_3D_USE_PUNCHTHROUGH swaps two lists' addresses in both
 * calls, so it must not change between them */
void njGetkmPointer(void);
void njSetkmPointer(void);

/* the copy njGetkmPointer takes: the context's four parameter words, and the
 * address each list writes next. trans holds the translucent list's address,
 * and punch the punch-through list's. under NJD_CONTROL_3D_USE_PUNCHTHROUGH,
 * trans and punch swap lists */
struct nj_parameterkm {
    Uint32 global;
    Uint32 isp;
    Uint32 tsp;
    Uint32 texture;
    Uint32 *opaque;
    Uint32 *trans;
    Uint32 *punch;
    Uint32 *opaque_modifier;
    Uint32 *trans_modifier;
};
extern struct nj_parameterkm _nj_parameterkm_;
/* the bits above SQ_OFFSET of the next address of the list the last starter
 * opened */
extern Uint32 _nj_sq_base_;
/* the field of _nj_parameterkm_ that holds the next address of the list the
 * last starter opened */
extern Uint32 **_nj_listtype_;
/* a polygon's parameter opens with four words: the global, ISP, TSP and
 * texture words. a vertex context keeps them as its four PARAMBUFFER words.
 * the global word holds the parameter's type from GLOBAL_PARA_SHIFT up, and
 * its list from GLOBAL_LIST_SHIFT up. GLOBAL_USER_CLIP takes effect only
 * under GLOBAL_GROUP_EN */
#define GLOBAL_PARA_SHIFT 29
#define GLOBAL_LIST 0x07000000
#define GLOBAL_LIST_SHIFT 24
#define GLOBAL_GROUP_EN 0x00800000
#define GLOBAL_USER_CLIP 0x00030000
/* the global word's low byte says what the polygon and its vertices hold.
 * GLOBAL_SHADOW lets modifier volumes change the polygon. GLOBAL_VOLUME gives
 * the polygon a second TSP word and texture word, which it takes inside a
 * modifier volume. on a modifier volume's parameter, GLOBAL_VOLUME marks the
 * volume's last triangle */
#define GLOBAL_OBJECT 0x000000ff
#define GLOBAL_SHADOW 0x00000080
#define GLOBAL_VOLUME 0x00000040
#define GLOBAL_TEXTURE 0x00000008 /* the polygon is textured */
#define GLOBAL_OFFSET 0x00000004 /* each vertex has an offset color */
#define GLOBAL_UV16 0x00000001 /* the vertices' u and v are 16 bits each */
/* the global word's list field for the translucent list */
#define GLOBAL_TRANS ((Uint32)KM_TRANS_POLYGON << GLOBAL_LIST_SHIFT)
/* the global words of a sprite and of a modifier volume, before the copy's
 * user clip joins them. GLOBAL_MODIFIER names the opaque modifier list.
 * njStartTransModifier sends it unchanged to the translucent modifier list */
#define GLOBAL_SPRITE                                                          \
    (((Uint32)KM_PARAM_SPRITE << GLOBAL_PARA_SHIFT) | GLOBAL_GROUP_EN)
#define GLOBAL_MODIFIER                                                        \
    (((Uint32)KM_PARAM_MODIFIERVOLUME << GLOBAL_PARA_SHIFT) |                  \
     ((Uint32)KM_OPAQUE_MODIFIER << GLOBAL_LIST_SHIFT))
/* ISP_CULL masks the ISP word's culling mode, the field that the
 * NJD_POLYGON_CULLING values set. ISP_Z_WRITE_DISABLE keeps the polygon's
 * depth out of the depth buffer */
#define ISP_CULL 0x18000000
#define ISP_Z_WRITE_DISABLE 0x04000000
/* the TSP word's blend factors: TSP_SRC's for the source, and TSP_DST's for
 * the destination. each is one of the BLEND_ numbers */
#define TSP_SRC 0xe0000000
#define TSP_DST 0x1c000000
#define TSP_BLEND (TSP_SRC | TSP_DST)
#define TSP_SRC_SHIFT 29
#define TSP_DST_SHIFT 26
#define BLEND_ZERO 0
#define BLEND_ONE 1
/* BLEND_OTHER_COLOR is the destination's color in TSP_SRC, and the source's
 * in TSP_DST */
#define BLEND_OTHER_COLOR 2
#define BLEND_INV_OTHER_COLOR 3
#define BLEND_SRC_ALPHA 4
#define BLEND_INV_SRC_ALPHA 5
#define BLEND_DST_ALPHA 6
#define BLEND_INV_DST_ALPHA 7
/* the TSP word's other fields */
#define TSP_USE_ALPHA 0x00100000 /* blend by the vertices' alpha */
#define TSP_IGNORE_TEX_ALPHA 0x00080000 /* take the texture as opaque */
#define TSP_FLIP 0x00060000 /* mirror the texture along u, and along v */
#define TSP_CLAMP 0x00018000 /* clamp it along u, and along v */
#define TSP_FILTER 0x00006000
#define TSP_SUPER_SAMPLE 0x00001000
#define TSP_MIPMAP 0x00000f00 /* the mipmap D adjust */
#define TSP_SHADING 0x000000c0 /* how the texture and the color combine */
#define TSP_SIZE 0x0000003f /* the texture's width and height */
/* tsp as an Opaque starter sends it: the source at one and the destination
 * at zero, ignoring alpha */
#define TSP_OPAQUE(tsp)                                                        \
    (((tsp) & ~(TSP_BLEND | TSP_USE_ALPHA)) |                                  \
     ((Uint32)BLEND_ONE << TSP_SRC_SHIFT) |                                    \
     ((Uint32)BLEND_ZERO << TSP_DST_SHIFT) | TSP_IGNORE_TEX_ALPHA)

/* the bits of a global word and of a TSP word that a color mode holds. a
 * color mode holds each at its place in its word. as a mask, GLOBAL_TRANS is
 * the one bit of the list field that picks the translucent list */
#define COLOR_MODE_GLOBAL (GLOBAL_TRANS | GLOBAL_OBJECT)
#define COLOR_MODE_TSP (TSP_USE_ALPHA | TSP_IGNORE_TEX_ALPHA)
/* make mode, a color mode, the color mode of _nj_parameterkm_ and of the
 * current context. the call copies mode's COLOR_MODE_GLOBAL bits into
 * _nj_parameterkm_'s global word, and its COLOR_MODE_TSP bits into the TSP
 * word. it leaves the words' other bits.
 *
 * the call sets the global word's GLOBAL_SHADOW while NJD_CONTROL_3D_SHADOW is
 * set. it also sets the bit while NJD_CONTROL_3D_SHADOW_OPAQUE is set and mode
 * lacks NJD_ARC1_TRANS. it clears the bit otherwise */
void njSetkmColorMode(Uint32 mode);

/* a starter opens a polygon in one list, through the store queues. it returns
 * the store queue address after the polygon's parameter. each starter
 * maps both store queues, so only one polygon can be open at a time.
 *
 * the caller stores the polygon's vertices from that address on. once it
 * fills a 32-byte block, it prefetches the block's address, which sends the
 * block to the list. it passes the address after the last vertex to
 * njEndVertex.
 *
 * a starter's parameter opens with _nj_parameterkm_'s global, ISP and TSP
 * words. a Tex starter sends its texture word after them, and a NonTex
 * starter leaves that word unwritten. an Opaque starter sends to the opaque
 * list, with the TSP word as TSP_OPAQUE makes it. a Trans starter sends to
 * _nj_parameterkm_'s trans list, with the TSP word unchanged */
Uint32 *njStartVertexNonTexOpaque(void);
Uint32 *njStartVertexNonTexTrans(void);
Uint32 *njStartVertexTexOpaque(void);
Uint32 *njStartVertexTexTrans(void);
/* the floats of a color in NJS_ARGB's order: a, r, g and b */
#define COLOR_FLOATS 4
/* a starter whose name ends in Intens or IntensH also sends face, the
 * polygon's color. face is COLOR_FLOATS floats: alpha, red, green and blue.
 * an Intens starter's parameter is 32 bytes, with face as words 4 to 7. an
 * IntensH starter's parameter is 64 bytes, with face as words 8 to 11. words
 * 12 to 15 of that parameter are offset, the polygon's offset color */
Uint32 *njStartVertexNonTexOpaqueIntens(Float *face);
Uint32 *njStartVertexNonTexTransIntens(Float *face);
Uint32 *njStartVertexTexOpaqueIntens(Float *face);
Uint32 *njStartVertexTexTransIntens(Float *face);
Uint32 *njStartVertexTexOpaqueIntensH(Float *face, Float *offset);
Uint32 *njStartVertexTexTransIntensH(Float *face, Float *offset);
/* the words of a sprite's parameter from its first through its color */
#define SPRITE_WORDS 5
/* njStartVertexTexSpriteOpaque and njStartVertexTexSpriteTrans open a
 * textured sprite as the Tex starters open a polygon. the global word they
 * send takes only its user clip from the copy. the TSP word that
 * njStartVertexTexSpriteTrans sends also takes TSP_USE_ALPHA, less
 * TSP_IGNORE_TEX_ALPHA. color is the sprite's packed color */
Uint32 *njStartVertexTexSpriteOpaque(Uint32 color);
Uint32 *njStartVertexTexSpriteTrans(Uint32 color);
/* njStartVertexNonTexSpriteOpaque and njStartVertexNonTexSpriteTrans open an
 * untextured sprite in the opaque or the trans list, as the Tex sprite
 * starters open a textured one, and leave the texture word unwritten. the
 * trans starter's global word names the opaque list, not the trans list */
Uint32 *njStartVertexNonTexSpriteOpaque(Uint32 color);
Uint32 *njStartVertexNonTexSpriteTrans(Uint32 color);
/* njStartModifier opens a modifier volume in the opaque modifier list, as a
 * starter opens a polygon. njStartTransModifier opens one in the translucent
 * modifier list. the caller then sends the volume's triangles, 64 bytes
 * each. njEndModifier sends a parameter at sq that makes the triangle after
 * it the volume's last. it returns the store queue address after the
 * parameter. the volume stays open until njEndVertex */
Uint32 *njStartModifier(void);
Uint32 *njStartTransModifier(void);
Uint32 *njEndModifier(Uint32 *sq);
/* close the open polygon. sq is the store queue address after its last
 * vertex. the copy's next address for the polygon's list becomes the memory
 * address that sq stands for */
void njEndVertex(const Uint32 *sq);

/* make texture n of _nj_current_texlist the texture polygons draw with. a
 * texture whose globalIndex and bank are current already changes nothing.
 * the call returns _nj_texglobal_color_, the current texture's format */
Sint32 njSetkmTextureNum(Uint32 n);
/* njSetkmTextureNum for the record of _nj_texmemlist with globalIndex. a
 * record must have it: the call doesn't test what njSearchTexList returns */
Sint32 njSetkmTextureNumG(Uint32 globalIndex);

/* give the copy's TSP word the blend that head holds. head is a chunk's
 * first halfword, with NJD_FBS_ and NJD_FBD_ bits. while
 * NJD_CONTROL_3D_CNK_BLEND_MODE is set, the blend is _nj_cnk_blend_mode_'s
 * instead */
void njSetkmCnkBlendMode(Uint32 head);
/* set the texture an NJD_CT_TID chunk names. head and texid are the chunk's
 * two halfwords. texid's low 13 bits index _nj_current_texlist. where that
 * texture isn't current, it becomes current. the copy then takes its texture
 * word, and the size bits of its TSP word.
 *
 * whether or not the texture changed, the copy's TSP word takes head's
 * flip, clamp and D adjust bits. the word also takes texid's NJD_FSS. the
 * word's filter becomes bilinear where texid has NJD_FFM_BF, and point
 * sampled otherwise. the call returns _nj_texglobal_color_ */
Sint32 njSetkmCnkTextureNum(Uint32 head, Uint32 texid);

#endif
