#ifdef DREAMCAST
#include <ninja.h>

#ifdef NONMATCHING
#include <kamui.h>
#include <machine.h>
#include <nj_km.h>
#include <sg_xpt.h>

/* make dc builds the three draws from this C. it sends the vertices the
 * bodies under the #else send, and only x differs: under the port's 16:9 a
 * draw squeezes x (engine/dc/port.h). an untextured polygon that covers the
 * screen keeps it */

/* a vertex's 32 bytes as the strips send them. an untextured vertex leaves u
 * and v unwritten, and each leaves offset unwritten */
struct vertex {
    Uint32 pcw;
    Float x, y, z;
    Float u, v;
    Uint32 col;
    Uint32 offset;
};

Sint32 njSetkmTextureMemList(Uint32 n);

/* the control word of vertex i of count: the last one ends the strip */
#define VERTEX_PCW(i, count)                                                   \
    ((i) == (count) - 1 ? KM_VERTEXPARAM_ENDOFSTRIP : KM_VERTEXPARAM_NORMAL)

void njDrawPolygon(NJS_POLYGON_VTX *polygon, Int count, Int trans)
{
    Int squeeze =
        port_wide_squeezes() &&
        !port_wide_covers(&polygon->x, count, sizeof(*polygon) / sizeof(Float));
    struct vertex *sq;
    Int i;

    njGetkmPointer();
    if (trans) {
        njSetkmColorMode(NJD_COLOR_MODE_PACKED_NORMAL_TRANS);
        sq = (struct vertex *)njStartVertexNonTexTrans();
    } else {
        njSetkmColorMode(NJD_COLOR_MODE_PACKED_NORMAL);
        sq = (struct vertex *)njStartVertexNonTexOpaque();
    }
    for (i = 0; i < count; i++, sq++) {
        sq->pcw = VERTEX_PCW(i, count);
        sq->x = squeeze ? port_wide_x(polygon[i].x) : polygon[i].x;
        sq->y = polygon[i].y;
        sq->z = polygon[i].z;
        sq->col = polygon[i].col;
        prefetch(sq);
    }
    njEndVertex((Uint32 *)sq);
    njSetkmPointer();
}

/* draw polygon's count vertices as njDrawTexture and njDrawTextureMemList
 * do. the two differ only in set_texture, the call that makes texture tex
 * current */
static void texture_draw(NJS_TEXTURE_VTX *polygon, Int count, Int tex, Int flag,
                         Sint32 (*set_texture)(Uint32 tex))
{
    struct vertex *sq;
    Int i;

    njGetkmPointer();
    if (flag) {
        njSetkmColorMode(NJD_COLOR_MODE_PACKED_TEXTURE_TRANS);
        set_texture(tex);
        sq = (struct vertex *)njStartVertexTexTrans();
    } else {
        njSetkmColorMode(NJD_COLOR_MODE_PACKED_TEXTURE);
        set_texture(tex);
        sq = (struct vertex *)njStartVertexTexOpaque();
    }
    for (i = 0; i < count; i++, sq++) {
        sq->pcw = VERTEX_PCW(i, count);
        sq->x = port_wide_x(polygon[i].x);
        sq->y = polygon[i].y;
        sq->z = polygon[i].z;
        sq->u = polygon[i].u;
        sq->v = polygon[i].v;
        sq->col = polygon[i].col;
        prefetch(sq);
    }
    njEndVertex((Uint32 *)sq);
    njSetkmPointer();
}

void njDrawTexture(NJS_TEXTURE_VTX *polygon, Int count, Int tex, Int flag)
{
    texture_draw(polygon, count, tex, flag, njSetkmTextureNumG);
}

void njDrawTextureMemList(NJS_TEXTURE_VTX *polygon, Int count, Int tex,
                          Int flag)
{
    texture_draw(polygon, count, tex, flag, njSetkmTextureMemList);
}
#else
/* clang-format off */

/* each vertex goes out as a 32-byte block. the block holds
 * KM_VERTEXPARAM_NORMAL, or KM_VERTEXPARAM_ENDOFSTRIP for the last vertex,
 * then x, y and z. col is word 6, and words 4, 5 and 7 stay unwritten.
 * H'E0000000 is KM_VERTEXPARAM_NORMAL, and H'F0000000
 * KM_VERTEXPARAM_ENDOFSTRIP. H'00080002 is NJD_COLOR_MODE_PACKED_NORMAL, and
 * H'02100002 is NJD_COLOR_MODE_PACKED_NORMAL_TRANS */
/* @asm open: SHC's dead RTS follows a tail jump, not the body's own return */
#pragma inline_asm(njDrawPolygon)
void njDrawPolygon(NJS_POLYGON_VTX *polygon, Int count, Int trans)
{
        .IMPORT     _njEndVertex
        .IMPORT     _njGetkmPointer
        .IMPORT     _njSetkmColorMode
        .IMPORT     _njSetkmPointer
        .IMPORT     _njStartVertexNonTexOpaque
        .IMPORT     _njStartVertexNonTexTrans
        STS.L       PR,@-R15
        MOV.L       R4,@-R15
        MOV.L       R5,@-R15
        MOV.L       #_njGetkmPointer,R3
        TST         R6,R6
        BT          ?opaque
        JSR         @R3
        NOP
        MOV.L       #_njSetkmColorMode,R3
        MOV.L       #H'02100002,R4
        JSR         @R3
        NOP
        MOV.L       #_njStartVertexNonTexTrans,R3
        JSR         @R3
        NOP
        BRA         ?start
        NOP
?opaque:
        JSR         @R3
        NOP
        MOV.L       #_njSetkmColorMode,R3
        MOV.L       #H'00080002,R4
        JSR         @R3
        NOP
        MOV.L       #_njStartVertexNonTexOpaque,R3
        JSR         @R3
        NOP
?start:
        MOV.L       @R15+,R1
        MOV.L       @R15+,R2
        MOV.L       #H'E0000000,R3
        ADD         #28,R0
?next:
        MOV.L       @R2+,R4
        MOV.L       @R2+,R5
        MOV.L       @R2+,R6
        MOV.L       @R2+,R7
        PREF        @R2
        DT          R1
        BT          ?last
        MOV.L       R7,@-R0
        ADD         #-8,R0
        MOV.L       R6,@-R0
        MOV.L       R5,@-R0
        MOV.L       R4,@-R0
        MOV.L       R3,@-R0
        PREF        @R0
        BRA         ?next
        ADD         #60,R0
?last:
        MOV.L       #H'F0000000,R3
        MOV.L       R7,@-R0
        ADD         #-8,R0
        MOV.L       R6,@-R0
        MOV.L       R5,@-R0
        MOV.L       R4,@-R0
        MOV.L       R3,@-R0
        PREF        @R0
        MOV.L       #_njEndVertex,R3
        ADD         #32,R0
        JSR         @R3
        MOV         R0,R4
        MOV.L       #_njSetkmPointer,R3
        LDS.L       @R15+,PR
        JMP         @R3
        NOP
}

/* each vertex goes out as njDrawPolygon's do, with u and v as words 4 and 5.
 * word 7, the offset color, stays unwritten. H'0008000A is
 * NJD_COLOR_MODE_PACKED_TEXTURE, and H'0210000A is
 * NJD_COLOR_MODE_PACKED_TEXTURE_TRANS */
/* @asm open: SHC's dead RTS follows a tail jump, not the body's own return */
#pragma inline_asm(njDrawTexture)
void njDrawTexture(NJS_TEXTURE_VTX *polygon, Int count, Int tex, Int flag)
{
        .IMPORT     _njSetkmTextureNumG
        .IMPORT     _njStartVertexTexOpaque
        .IMPORT     _njStartVertexTexTrans
        MOV.L       R9,@-R15
        MOV.L       R8,@-R15
        STS.L       PR,@-R15
        MOV.L       R4,@-R15
        MOV.L       R5,@-R15
        MOV.L       R6,@-R15
        MOV.L       #_njGetkmPointer,R3
        TST         R7,R7
        BT          ?opaque
        JSR         @R3
        NOP
        MOV.L       #_njSetkmColorMode,R3
        MOV.L       #H'0210000A,R4
        JSR         @R3
        NOP
        MOV.L       #_njSetkmTextureNumG,R3
        JSR         @R3
        MOV.L       @R15+,R4
        MOV.L       #_njStartVertexTexTrans,R3
        JSR         @R3
        NOP
        BRA         ?start
        NOP
?opaque:
        JSR         @R3
        NOP
        MOV.L       #_njSetkmColorMode,R3
        MOV.L       #H'0008000A,R4
        JSR         @R3
        NOP
        MOV.L       #_njSetkmTextureNumG,R3
        JSR         @R3
        MOV.L       @R15+,R4
        MOV.L       #_njStartVertexTexOpaque,R3
        JSR         @R3
        NOP
?start:
        MOV.L       @R15+,R1
        MOV.L       @R15+,R2
        MOV.L       #H'E0000000,R3
        ADD         #28,R0
?next:
        MOV.L       @R2+,R4
        MOV.L       @R2+,R5
        MOV.L       @R2+,R6
        MOV.L       @R2+,R7
        MOV.L       @R2+,R8
        MOV.L       @R2+,R9
        PREF        @R2
        DT          R1
        BT          ?last
        MOV.L       R9,@-R0
        MOV.L       R8,@-R0
        MOV.L       R7,@-R0
        MOV.L       R6,@-R0
        MOV.L       R5,@-R0
        MOV.L       R4,@-R0
        MOV.L       R3,@-R0
        PREF        @R0
        BRA         ?next
        ADD         #60,R0
?last:
        MOV.L       #H'F0000000,R3
        MOV.L       R9,@-R0
        MOV.L       R8,@-R0
        MOV.L       R7,@-R0
        MOV.L       R6,@-R0
        MOV.L       R5,@-R0
        MOV.L       R4,@-R0
        MOV.L       R3,@-R0
        PREF        @R0
        MOV.L       #_njEndVertex,R3
        ADD         #32,R0
        JSR         @R3
        MOV         R0,R4
        MOV.L       #_njSetkmPointer,R3
        LDS.L       @R15+,PR
        MOV.L       @R15+,R8
        JMP         @R3
        MOV.L       @R15+,R9
}

/* njDrawTexture's body, but for njSetkmTextureMemList in place of
 * njSetkmTextureNumG */
/* @asm open: SHC's dead RTS follows a tail jump, not the body's own return */
#pragma inline_asm(njDrawTextureMemList)
void njDrawTextureMemList(NJS_TEXTURE_VTX *polygon, Int count, Int tex,
                          Int flag)
{
        .IMPORT     _njSetkmTextureMemList
        MOV.L       R9,@-R15
        MOV.L       R8,@-R15
        STS.L       PR,@-R15
        MOV.L       R4,@-R15
        MOV.L       R5,@-R15
        MOV.L       R6,@-R15
        MOV.L       #_njGetkmPointer,R3
        TST         R7,R7
        BT          ?opaque
        JSR         @R3
        NOP
        MOV.L       #_njSetkmColorMode,R3
        MOV.L       #H'0210000A,R4
        JSR         @R3
        NOP
        MOV.L       #_njSetkmTextureMemList,R3
        JSR         @R3
        MOV.L       @R15+,R4
        MOV.L       #_njStartVertexTexTrans,R3
        JSR         @R3
        NOP
        BRA         ?start
        NOP
?opaque:
        JSR         @R3
        NOP
        MOV.L       #_njSetkmColorMode,R3
        MOV.L       #H'0008000A,R4
        JSR         @R3
        NOP
        MOV.L       #_njSetkmTextureMemList,R3
        JSR         @R3
        MOV.L       @R15+,R4
        MOV.L       #_njStartVertexTexOpaque,R3
        JSR         @R3
        NOP
?start:
        MOV.L       @R15+,R1
        MOV.L       @R15+,R2
        MOV.L       #H'E0000000,R3
        ADD         #28,R0
?next:
        MOV.L       @R2+,R4
        MOV.L       @R2+,R5
        MOV.L       @R2+,R6
        MOV.L       @R2+,R7
        MOV.L       @R2+,R8
        MOV.L       @R2+,R9
        PREF        @R2
        DT          R1
        BT          ?last
        MOV.L       R9,@-R0
        MOV.L       R8,@-R0
        MOV.L       R7,@-R0
        MOV.L       R6,@-R0
        MOV.L       R5,@-R0
        MOV.L       R4,@-R0
        MOV.L       R3,@-R0
        PREF        @R0
        BRA         ?next
        ADD         #60,R0
?last:
        MOV.L       #H'F0000000,R3
        MOV.L       R9,@-R0
        MOV.L       R8,@-R0
        MOV.L       R7,@-R0
        MOV.L       R6,@-R0
        MOV.L       R5,@-R0
        MOV.L       R4,@-R0
        MOV.L       R3,@-R0
        PREF        @R0
        MOV.L       #_njEndVertex,R3
        ADD         #32,R0
        JSR         @R3
        MOV         R0,R4
        MOV.L       #_njSetkmPointer,R3
        LDS.L       @R15+,PR
        MOV.L       @R15+,R8
        JMP         @R3
        MOV.L       @R15+,R9
}
/* clang-format on */
#endif
#endif
