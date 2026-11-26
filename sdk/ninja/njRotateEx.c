#ifdef DREAMCAST
#include <ninja.h>

#ifdef NONMATCHING
/* this side keeps the console's bits. ninja_zam.h's helper makes SH4ZAM's
 * turns in the body's order, and skips the same ones. each is the same FSCA and
 * two FTRVs on the same rows */
#undef njRotateEx
void njRotateEx(Angle *ang, Sint32 lv)
{
    zam_njRotateEx(ang, lv);
}
#else
/* clang-format off */
/* XMTRX holds the current matrix. a turn about one axis differs from the unit
 * matrix in two rows. FTRV multiplies each of those rows by XMTRX, and the
 * products replace the same rows of XMTRX. so each turn goes before the
 * matrix, and a point takes the body's last turn first. a turn by 0 is
 * skipped */
/* @asm operand: ftrv-registers, FTRV XMTRX,FV4 at 0x8c1189dc multiplies the
 * second row of the turn about z */
#pragma inline_asm(njRotateEx)
void njRotateEx(Angle *ang, Sint32 lv)
{
        MOV.L       @R4+,R1
        MOV.L       @R4+,R2
        CMP/PL      R5
        MOV.L       @R4+,R3
        BT          ?about_y
?about_z:
        TST         R3,R3
        BT/S        ?after_z
        LDS         R3,FPUL
        FSCA        FPUL,DR0
        FLDI0       FR3
        FMOV.S      FR0,FR2
        FMOV.S      FR1,FR0
        FMOV.S      FR2,FR1
        FLDI0       FR2
        FMOV.S      FR0,FR5
        FMOV.S      FR1,FR4
        FTRV        XMTRX,FV0
        FNEG        FR4
        FLDI0       FR6
        FLDI0       FR7
        FTRV        XMTRX,FV4
        FSCHG
        FMOV        DR0,XD0
        FMOV        DR2,XD2
        FMOV        DR4,XD4
        FMOV        DR6,XD6
        FSCHG
?after_z:
        CMP/PL      R5
        BT          ?done
?about_y:
        TST         R2,R2
        BT/S        ?about_x
        LDS         R2,FPUL
        FSCA        FPUL,DR0
        FLDI0       FR3
        FMOV.S      FR0,FR2
        FMOV.S      FR1,FR0
        FLDI0       FR1
        FMOV.S      FR0,FR6
        FMOV.S      FR2,FR4
        FNEG        FR2
        FTRV        XMTRX,FV0
        FLDI0       FR5
        FLDI0       FR7
        FTRV        XMTRX,FV4
        FSCHG
        FMOV        DR0,XD0
        FMOV        DR2,XD2
        FMOV        DR4,XD8
        FMOV        DR6,XD10
        FSCHG
?about_x:
        TST         R1,R1
        BT/S        ?after_x
        LDS         R1,FPUL
        FSCA        FPUL,DR0
        FLDI0       FR3
        FMOV.S      FR0,FR2
        FLDI0       FR0
        FMOV.S      FR2,FR5
        FMOV.S      FR1,FR6
        FNEG        FR5
        FTRV        XMTRX,FV0
        FLDI0       FR4
        FLDI0       FR7
        FTRV        XMTRX,FV4
        FSCHG
        FMOV        DR0,XD4
        FMOV        DR2,XD6
        FMOV        DR4,XD8
        FMOV        DR6,XD10
        FSCHG
?after_x:
        CMP/PL      R5
        BT          ?about_z
?done:
        RTS
        NOP
}
/* clang-format on */
#endif
#endif
