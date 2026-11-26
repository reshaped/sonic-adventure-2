/* vertex_colors.h's functions: vlist_set_color in C, and the other three in
 * assembly */
#include <ninja.h>
#include <sg_xpt.h>
#include "draw/chunk.h"
#include "shared/vertex_colors.h"

#ifdef NONMATCHING
/* the plain form of the hacks tagged in the matching side. it sets nothing for
 * a list of no vertices, where the matching side's loop runs on past 0 */
#include <stddef.h>
#include <stdint.h>

Sint32 vlist_set_color(Sint32 *vlist, Uint32 color)
{
    Uint32 count = CHUNK_VERTEX_COUNT(vlist);
    uintptr_t address;
    Uint32 i;

    if (count == 0) {
        return 0;
    }
    address = (uintptr_t)(vlist + CHUNK_HEAD_WORDS);
    address += offsetof(struct chunk_vertex_d8, color);
    for (i = 0; i < count; i++) {
        *(Uint32 *)address = color;
        address += sizeof(struct chunk_vertex_d8);
    }
    return 0;
}
#else
/* @hack redundant: color_word, an identity the color passes through */
static Uint32 color_word(Uint32 color)
{
    return color;
}

Sint32 vlist_set_color(Sint32 *vlist, Uint32 color)
{
    /* @hack type: header, and address below, volatile, where plain pointers
     * would do */
    volatile Sint32 *header = vlist;
    Uint32 remaining;
    volatile Uint32 *address;
    Uint32 packed;

    ++header;
    remaining = (Uint32)*header++ >> CHUNK_HIGH_SHIFT;
    /* a D8 vertex's color follows its point. SHC's headers have no
     * offsetof */
    address = (volatile Uint32 *)((Uint32)header + sizeof(NJS_POINT3));
    packed = color_word(color);
    do {
        *address = packed;
        address += CHUNK_D8_WORDS;
    } while (--remaining);
    return 0;
}
#endif

#ifdef DREAMCAST
/* clang-format off */
/* @asm operand: integer-constant, MOV.L #NJD_CE,R6 at 0x8c53476a loads 255,
 * which CMP/EQ then reads as an integer */
#pragma asm
        .SECTION    P,CODE,ALIGN=32
; the color's byte offset in an NJD_CV_D8 vertex
VX_COLOR:       .EQU    12
; the chunk type that ends a chunk list
NJD_CE:         .EQU    255
; the mask of the high byte of a product of two bytes
PRODUCT_HIGH:   .EQU    255 << 8

        .EXPORT     _vlist_copy_shaded

; Sint32 vlist_copy_shaded(Sint32 *dst, Sint32 *vlist, Uint32 color,
;                          Uint32 mask)
; copy the chunk at vlist to dst with each vertex color shaded by color,
; then copy the word after the chunk, and return 0. mask is unused
_vlist_copy_shaded:
        MOV.L       R14,@-R15
        MOV.L       R13,@-R15
        MOV.L       R12,@-R15
        PREF        @R5
        EXTU.B      R6,R7                   ; color bytes, low to high:
        SHLR8       R6                      ; R7, R3, R2 and R1
        EXTU.B      R6,R3
        SHLR8       R6
        EXTU.B      R6,R2
        SHLR8       R6
        EXTU.B      R6,R1
        MOV.L       #PRODUCT_HIGH,R13
        MOV.L       @R5+,R0
        MOV.L       R0,@R4
        MOV.L       @R5+,R0
        MOV.L       R0,@(4,R4)
        ADD         #8,R4
        SHLR16      R0
        MOV         R0,R14                  ; the count
?copy:
        MOV.L       @R5+,R0                 ; the point
        MOV.L       R0,@R4
        MOV.L       @R5+,R0
        MOV.L       R0,@(4,R4)
        MOV.L       @R5+,R0
        MOV.L       R0,@(8,R4)
        ADD         #VX_COLOR,R4
        MOV.L       @R5+,R12                ; the color
        PREF        @R5
        EXTU.B      R12,R6
        MULU.W      R6,R7
        STS         MACL,R6
        SHLR8       R6
        EXTU.B      R6,R0
        SHLR8       R12
        EXTU.B      R12,R6
        MULU.W      R6,R3
        STS         MACL,R6
        AND         R13,R6
        OR          R6,R0
        SHLR8       R12
        EXTU.B      R12,R6
        MULU.W      R6,R2
        STS         MACL,R6
        AND         R13,R6
        SHLL8       R6
        OR          R6,R0
        SHLR8       R12
        MULU.W      R12,R1
        STS         MACL,R6
        AND         R13,R6
        SHLL16      R6
        OR          R6,R0
        MOV.L       R0,@R4
        DT          R14
        BF/S        ?copy
        ADD         #4,R4
        MOV.L       @R5+,R0
        MOV.L       R0,@R4
        MOV.L       #NJD_CE,R6
?copy_end:
        CMP/EQ      R6,R0                   ; NJD_CE, or loop for ever
        BF          ?copy_end
        MOV.L       @R15+,R12
        MOV.L       @R15+,R13
        MOV.L       @R15+,R14
        RTS
        MOV         #0,R0
#pragma endasm

/* @asm operand: integer-constant, MOV.L #NJD_CE,R6 at 0x8c5347e0 loads 255,
 * which CMP/EQ then reads as an integer */
#pragma asm
        .SECTION    P,CODE,ALIGN=32
        .EXPORT     _vlist_shade

; Sint32 vlist_shade(Sint32 *vlist, Uint32 color)
; shade each vertex color of the chunk at vlist by color, and return 0
_vlist_shade:
        MOV.L       R14,@-R15
        MOV.L       R12,@-R15
        PREF        @R4
        EXTU.B      R5,R7                   ; color bytes, low to high:
        SHLR8       R5                      ; R7, R3, R2 and R1
        EXTU.B      R5,R3
        SHLR8       R5
        EXTU.B      R5,R2
        SHLR8       R5
        EXTU.B      R5,R1
        MOV.L       #PRODUCT_HIGH,R5
        ADD         #4,R4
        MOV.L       @R4+,R0
        SHLR16      R0
        MOV         R0,R14                  ; the count
?shade:
        ADD         #VX_COLOR,R4
        MOV.L       @R4,R12                 ; the color
        EXTU.B      R12,R6
        MULU.W      R6,R7
        STS         MACL,R6
        SHLR8       R6
        EXTU.B      R6,R0
        SHLR8       R12
        EXTU.B      R12,R6
        MULU.W      R6,R3
        STS         MACL,R6
        AND         R5,R6
        OR          R6,R0
        SHLR8       R12
        EXTU.B      R12,R6
        MULU.W      R6,R2
        STS         MACL,R6
        AND         R5,R6
        SHLL8       R6
        OR          R6,R0
        SHLR8       R12
        MULU.W      R12,R1
        STS         MACL,R6
        AND         R5,R6
        SHLL16      R6
        OR          R6,R0
        MOV.L       R0,@R4
        DT          R14
        BF/S        ?shade
        ADD         #4,R4
        MOV.L       @R4+,R0
        MOV.L       #NJD_CE,R6
?shade_end:
        CMP/EQ      R6,R0                   ; NJD_CE, or loop for ever
        BF          ?shade_end
        MOV.L       @R15+,R12
        MOV.L       @R15+,R14
        RTS
        MOV         #0,R0
#pragma endasm

/* @asm open: no inspect rule reads any of its instructions */
#pragma asm
        .SECTION    P,CODE,ALIGN=32
        .EXPORT     _point_midpoint

; void point_midpoint(NJS_POINT3 *from, NJS_POINT3 *to, NJS_POINT3 *mid)
; set mid to the point halfway between from and to
_point_midpoint:
        FMOV.S      @R4+,FR0
        MOVA        ?half,R0
        FMOV.S      @R5+,FR3
        FMOV.S      @R0,FR10
        FMOV.S      @R4+,FR1
        FADD        FR3,FR0
        FMOV.S      @R5+,FR4
        FMUL        FR10,FR0
        FMOV.S      @R4+,FR2
        FADD        FR4,FR1
        FMOV.S      @R5+,FR5
        FMUL        FR10,FR1
        FADD        FR5,FR2
        FMOV.S      FR0,@R6                 ; x
        FMUL        FR10,FR2
        ADD         #8,R6
        FMOV.S      FR2,@R6                 ; z
        RTS
        FMOV.S      FR1,@-R6                ; y
        .ALIGN      4
?half:
        .FDATA.S    F'0.5                   ; the midpoint's scale
#pragma endasm
/* clang-format on */
#endif
