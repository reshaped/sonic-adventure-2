/* the chunk lights. njSetChunkLightQueue queues the lights that are on, and
 * njCalculateChunkLights lights a vertex of a chunk model by them */
#include <nj_global.h>
#include <ninja.h>
#include <sg_xpt.h>

/* the member exports these functions and data, though no header of the SDK
 * declares them */
/* NOLINTBEGIN(misc-use-internal-linkage) */
void njChunkDirectLight(NJS_LIGHT *light);
void njChunkDummyLight(NJS_LIGHT *light);
void njChunkPointLight(NJS_LIGHT *light);
void njChunkSpotLight(NJS_LIGHT *light);
void njChunkLightLoop(void);
void njCalculateChunkLights(NJS_ARGB *argb, NJS_POINT3 *pnt, NJS_VECTOR *nml,
                            Bool reverse);
void njGetChunkLightDistance(NJS_POINT3 *point, Float radius);
void njCalcChunkLightMaterials(void);
void njCheckChunkLightMaterials(Int flags);
void njSetChunkLightQueue(void);
void njResetChunkLightQueue(void);
/* while _nj_ignore_specular_flag_ is set, the directional and point lights
 * add no specular light. while _nj_ignore_ambient_flag_ is set, a vertex
 * takes no ambient light. njCheckChunkLightMaterials sets both */
Bool _nj_ignore_specular_flag_ = FALSE;
Bool _nj_ignore_ambient_flag_ = FALSE;
/* each queued light's calculator, the function that adds its light to a
 * vertex, indexed as _nj_light_queue_.slots */
void (*_nj_chunk_rgb_lt_func_[NJD_LIGHT_MAX])(NJS_LIGHT *light) = {NULL};
/* the ambient color njCnkSetAmbient sets */
NJS_ARGB _nj_chunk_light_ambient_ = {0.0F, 0.2F, 0.2F, 0.2F};
/* the colors, point and normal of the vertex njCalculateChunkLights lights,
 * where the calculators read them */
NJS_ARGB *_nj_ptr_chunk_rgb_lt_argb_;
NJS_POINT3 *_nj_ptr_chunk_rgb_lt_pnt_;
NJS_VECTOR *_nj_ptr_chunk_rgb_lt_nml_;
/* each queued light, indexed as _nj_light_queue_.slots */
NJS_LIGHT *_nj_ptr_chunk_rgb_lt_[NJD_LIGHT_MAX];
/* set where njGetChunkLightDistance finds that a queued light can't reach the
 * sphere it was last given. njChunkLightLoop skips such a light */
Bool _nj_light_clip_flag_[NJD_LIGHT_MAX];
/* the ambient light a vertex takes. njCalcChunkLightMaterials sets its red,
 * green and blue to _nj_chunk_light_ambient_'s, each times the ambient one of
 * _nj_light_material_ */
NJS_ARGB _nj_chunk_light_calc_ambient_;
/* NOLINTEND(misc-use-internal-linkage) */

/* clang-format off */
#ifdef DREAMCAST
/* value raised to the power that step stands for. steps 0 to 16 stand for
 * 0, 1/2, 1, 3/2, 2, 3, 4, 6, 8, 12, 16, 24, 32, 48, 64, 96 and 128. any
 * other step, a negative one too, stands for 128.
 *
 * the member doesn't export specular_power. it is external here because SHC
 * writes a static body only where C calls it, and only the bodies below call
 * this one */
/* @asm epilogue: dead-return, SHC's RTS at 0x8c11a5e0 follows the body's own
 * return at 0x8c11a5da */
#pragma inline_asm(specular_power)
Float specular_power(Float value, Int step)
{
        MOV         #17,R1
        CMP/HS      R1,R4
        BT          ?other
        SHLL        R4
        MOVA        ?steps,R0
        MOV.W       @(R0,R4),R0
        BRAF        R0
        NOP
?steps:
        .DATA.W     ?step0-?steps
        .DATA.W     ?step1-?steps
        .DATA.W     ?step2-?steps
        .DATA.W     ?step3-?steps
        .DATA.W     ?step4-?steps
        .DATA.W     ?step5-?steps
        .DATA.W     ?step6-?steps
        .DATA.W     ?step7-?steps
        .DATA.W     ?step8-?steps
        .DATA.W     ?step9-?steps
        .DATA.W     ?step10-?steps
        .DATA.W     ?step11-?steps
        .DATA.W     ?step12-?steps
        .DATA.W     ?step13-?steps
        .DATA.W     ?step14-?steps
        .DATA.W     ?step15-?steps
        .DATA.W     ?step16-?steps
?step0:
        RTS
        FLDI1       FR0
?step1:
        FMOV.S      FR4,FR0
        RTS
        FSQRT       FR0
?step3:
        FMOV.S      FR4,FR0
        FSQRT       FR4
        RTS
        FMUL        FR4,FR0
?step15:
        FMUL        FR4,FR4
?step13:
        FMUL        FR4,FR4
?step11:
        FMUL        FR4,FR4
?step9:
        FMUL        FR4,FR4
?step7:
        FMUL        FR4,FR4
?step5:
        FMOV.S      FR4,FR0
        FMUL        FR4,FR0
        RTS
        FMUL        FR4,FR0
?other:
?step16:
        FMUL        FR4,FR4
?step14:
        FMUL        FR4,FR4
?step12:
        FMUL        FR4,FR4
?step10:
        FMUL        FR4,FR4
?step8:
        FMUL        FR4,FR4
?step6:
        FMUL        FR4,FR4
?step4:
        FMUL        FR4,FR4
?step2:
        FMOV.S      FR4,FR0
        RTS
        NOP
}
#endif
/* clang-format on */

/* clang-format off */
#ifdef DREAMCAST
/* the calculators. each adds one queued light's share to the colors of the
 * vertex njCalculateChunkLights lights. it reads three fields of the light's
 * ltcal that njCalcChunkLightMaterials sets: mdif, the diffuse color times
 * the material's; mspc, the specular color times the material's; and exp,
 * specular_power's step.
 *
 * a calculator works out the light's strength at the vertex, and a specular
 * value. it adds the strength times mdif to the diffuse color's red, green
 * and blue. the specular term is specular_power of the specular value and
 * exp. the calculator adds the term times mspc's red, green and blue to the
 * specular color's. it adds the term times mspc's alpha to the diffuse
 * color's alpha.
 *
 * njChunkDirectLight shines along ltcal.lvctr. its strength is the inner
 * product of the vertex's normal and -lvctr. its specular value is the
 * strength squared. a strength of 0 or less adds nothing */
/* @asm epilogue: dead-return, SHC's RTS at 0x8c11a694 follows the body's own
 * return at 0x8c11a676 and its pool */
#pragma inline_asm(njChunkDirectLight)
void njChunkDirectLight(NJS_LIGHT *light)
{
        MOV.L       #__nj_ptr_chunk_rgb_lt_nml_,R3
        MOV.W       #H'009C,R1
        MOV.L       @R3,R3
        ADD         R4,R1
        FMOV.S      @R1+,FR0
        FMOV.S      @R1+,FR1
        FMOV.S      @R1+,FR2
        FLDI0       FR3
        FMOV.S      @R3+,FR4
        FMOV.S      @R3+,FR5
        FMOV.S      @R3+,FR6
        FLDI0       FR7
        FIPR        FV4,FV0
        FLDI0       FR1
        FNEG        FR3
        FCMP/GT     FR1,FR3
        BF          ?done
        FMOV.S      FR3,FR0
        MOV.W       #H'0108,R2
        ADD         R4,R2
        FMOV.S      @R2+,FR4
        MOV.L       #__nj_ptr_chunk_rgb_lt_argb_,R1
        FMOV.S      @R2+,FR5
        MOV.L       @R1,R1
        ADD         #4,R1
        FMOV.S      @R2+,FR6
        FMOV.S      @R1+,FR7
        FMOV.S      @R1+,FR8
        FMOV.S      @R1+,FR9
        FMAC        FR0,FR4,FR7
        FMAC        FR0,FR5,FR8
        FMAC        FR0,FR6,FR9
        MOV.L       #__nj_ignore_specular_flag_,R3
        FMOV.S      FR9,@-R1
        MOV.L       @R3,R3
        FMOV.S      FR8,@-R1
        FMOV.S      FR7,@-R1
        TST         R3,R3
        BF          ?done
        FMUL        FR0,FR0
        STS.L       PR,@-R15
        MOV.L       #_specular_power,R3
        MOV.L       R4,@-R15
        MOV.W       #H'0088,R2
        ADD         R2,R4
        MOV.L       @R4,R4
        FMOV.S      FR0,FR4
        JSR         @R3
        NOP
        MOV.L       #__nj_ptr_chunk_rgb_lt_argb_,R1
        MOV.L       @R15+,R4
        MOV.L       @R1,R1
        MOV.W       #H'0114,R2
        LDS.L       @R15+,PR
        ADD         R4,R2
        MOV         R1,R3
        FMOV.S      @R2+,FR1
        FMOV.S      @R2+,FR2
        FMOV.S      @R2+,FR3
        ADD         #20,R1
        FMOV.S      @R2+,FR4
        FMOV.S      @R3+,FR5
        FMOV.S      @R1+,FR6
        FMOV.S      @R1+,FR7
        FMOV.S      @R1+,FR8
        FMAC        FR0,FR1,FR5
        FMAC        FR0,FR2,FR6
        FMAC        FR0,FR3,FR7
        FMAC        FR0,FR4,FR8
        FMOV.S      FR5,@-R3
        FMOV.S      FR8,@-R1
        FMOV.S      FR7,@-R1
        FMOV.S      FR6,@-R1
?done:
        RTS
        NOP
}
#endif
/* clang-format on */

/* the calculator njSetChunkLightQueue picks for a light it has no other
 * calculator for. it adds nothing */
void njChunkDummyLight(NJS_LIGHT *light)
{
}

/* clang-format off */
#ifdef DREAMCAST
/* njChunkPointLight shines from ltcal.lpnt. its strength is the inner product
 * of the vertex's normal and the unit vector toward lpnt. a vertex takes
 * nothing unless the strength is above 0. its distance from lpnt squared must
 * also be below ltcal.frr. where that square is above ltcal.nrr, the
 * strength is scaled by ltcal.odev - ltcal.idev * distance. the specular
 * value is the strength squared, before the scale. the scale then multiplies
 * the specular term too */
/* @asm operand: fipr-registers, FIPR FV0,FV4 at 0x8c11a6c4 takes the inner
 * product of the direction to the light and the normal */
#pragma inline_asm(njChunkPointLight)
void njChunkPointLight(NJS_LIGHT *light)
{
        MOV.W       #H'0090,R2
        MOV.L       #__nj_ptr_chunk_rgb_lt_pnt_,R3
        ADD         R4,R2
        MOV.L       @R3,R3
        FMOV.S      @R2+,FR0
        FMOV.S      @R2+,FR1
        FMOV.S      @R2+,FR2
        FLDI0       FR3
        FMOV.S      @R3+,FR4
        FMOV.S      @R3+,FR5
        FMOV.S      @R3+,FR6
        FLDI0       FR7
        FSUB        FR4,FR0
        MOV.L       #__nj_ptr_chunk_rgb_lt_nml_,R2
        FSUB        FR5,FR1
        MOV.L       @R2,R2
        FSUB        FR6,FR2
        FMOV.S      @R2+,FR4
        FMOV.S      @R2+,FR5
        FMOV.S      @R2+,FR6
        FIPR        FV0,FV4
        FMOV.S      FR0,FR8
        FMOV.S      FR1,FR9
        FMOV.S      FR2,FR10
        FLDI0       FR11
        FIPR        FV0,FV8
        MOV.W       #H'006C,R0
        FCMP/GT     FR3,FR7
        FMOV.S      @(R0,R4),FR1
        BF          ?done
        FCMP/GT     FR11,FR1
        BF          ?done
        FMOV.S      FR11,FR0
        FSRRA       FR0
        FLDI1       FR3
        MOV.W       #H'0068,R0
        FMUL        FR0,FR7
        FMOV.S      @(R0,R4),FR2
        FCMP/GT     FR2,FR11
        BF          ?add
        MOV.W       #H'0078,R0
        FSQRT       FR11
        FMOV.S      @(R0,R4),FR0
        MOV.W       #H'007C,R0
        FNEG        FR0
        FMOV.S      @(R0,R4),FR3
        FMAC        FR0,FR11,FR3
?add:
        MOV.W       #H'0108,R2
        FMOV.S      FR7,FR0
        ADD         R4,R2
        FMUL        FR3,FR0
        FMOV.S      @R2+,FR4
        MOV.L       #__nj_ptr_chunk_rgb_lt_argb_,R3
        FMOV.S      @R2+,FR5
        MOV.L       @R3,R3
        FMOV.S      @R2+,FR6
        ADD         #4,R3
        FMOV.S      @R3+,FR8
        FMOV.S      @R3+,FR9
        FMAC        FR0,FR4,FR8
        FMOV.S      @R3+,FR10
        FMAC        FR0,FR5,FR9
        FMAC        FR0,FR6,FR10
        MOV.L       #__nj_ignore_specular_flag_,R5
        FMOV.S      FR10,@-R3
        MOV.L       @R5,R5
        FMOV.S      FR9,@-R3
        MOV         #0,R6
        FMOV.S      FR8,@-R3
        CMP/EQ      R6,R5
        BF          ?done
        STS.L       PR,@-R15
        FMUL        FR7,FR7
        MOV.L       R4,@-R15
        MOV.L       #_specular_power,R1
        FMOV.S      FR3,@-R15
        MOV.W       #H'0088,R0
        FMOV.S      FR7,FR4
        JSR         @R1
        MOV.L       @(R0,R4),R4
        FMOV.S      @R15+,FR3
        MOV.L       @R15+,R4
        FMUL        FR3,FR0
        MOV.W       #H'0114,R2
        LDS.L       @R15+,PR
        ADD         R4,R2
        MOV.L       #__nj_ptr_chunk_rgb_lt_argb_,R3
        FMOV.S      @R2+,FR1
        MOV.L       @R3,R3
        FMOV.S      @R2+,FR2
        MOV         R3,R5
        FMOV.S      @R2+,FR3
        FMOV.S      @R2+,FR4
        ADD         #20,R3
        FMOV.S      @R5+,FR5
        FMOV.S      @R3+,FR6
        FMAC        FR0,FR1,FR5
        FMOV.S      @R3+,FR7
        FMAC        FR0,FR2,FR6
        FMOV.S      @R3+,FR8
        FMAC        FR0,FR3,FR7
        FMAC        FR0,FR4,FR8
        FMOV.S      FR5,@-R5
        FMOV.S      FR8,@-R3
        FMOV.S      FR7,@-R3
        FMOV.S      FR6,@-R3
?done:
        RTS
        NOP
}
#endif
/* clang-format on */

/* clang-format off */
#ifdef DREAMCAST
/* njChunkSpotLight shines from ltcal.lpnt along ltcal.lvctr. a vertex's spot
 * cosine is lvctr's inner product with the unit vector from lpnt to the
 * vertex. the light reaches a vertex only where njChunkPointLight's would.
 * the spot cosine must also be above 0, and its square above ltcal.cose. the
 * strength is njChunkPointLight's, with its scale by distance.
 *
 * where the spot cosine's square is below ltcal.cosi, an angle scales the
 * strength too. the angle is acosf of the spot cosine, in Angle's units. the
 * scale is linear in the angle. ltcal.reserve's high halfword holds the angle
 * where the scale is 1. its low halfword holds the angle where the scale is 0.
 * where the two halfwords are equal, no angle scales the strength.
 *
 * the specular value is the spot cosine. the specular term is then multiplied
 * by the strength, after its scales */
/* @bug logic: takes the spot cosine's square as the specular value wherever
 * the angle scales the strength. the body saves the square from FR8
 * (FMOV.S FR8,@-R15 at 0x8c11a84c), and pops it into FR10, which held the
 * spot cosine (FMOV.S @R15+,FR10 at 0x8c11a858) */
/* @bug logic: adds a specular term while _nj_ignore_specular_flag_ is set.
 * njChunkDirectLight and njChunkPointLight test the flag (TST R3,R3 at
 * 0x8c11a62e, CMP/EQ R6,R5 at 0x8c11a726). this body calls specular_power
 * with no test (JSR @R1 at 0x8c11a89c) */
/* @asm operand: fipr-registers, FIPR FV0,FV4 at 0x8c11a7ca takes the inner
 * product of the direction from the light and its lvctr */
#pragma inline_asm(njChunkSpotLight)
void njChunkSpotLight(NJS_LIGHT *light)
{
        .IMPORT     _acosf
        STS.L       PR,@-R15
        MOV.W       #H'0090,R2
        MOV.L       #__nj_ptr_chunk_rgb_lt_pnt_,R3
        ADD         R4,R2
        MOV.L       @R3,R3
        FMOV.S      @R2+,FR4
        FMOV.S      @R2+,FR5
        FMOV.S      @R2+,FR6
        FLDI0       FR7
        FMOV.S      @R3+,FR0
        FMOV.S      @R3+,FR1
        FSUB        FR4,FR0
        FMOV.S      @R3+,FR2
        FLDI0       FR3
        MOV.W       #H'009C,R2
        FSUB        FR5,FR1
        ADD         R4,R2
        FSUB        FR6,FR2
        FMOV.S      @R2+,FR4
        FMOV.S      @R2+,FR5
        MOV.L       #__nj_ptr_chunk_rgb_lt_nml_,R3
        FMOV.S      @R2+,FR6
        MOV.L       @R3,R3
        FIPR        FV0,FV4
        FMOV.S      @R3+,FR8
        FMOV.S      @R3+,FR9
        FMOV.S      @R3+,FR10
        FLDI0       FR11
        FIPR        FV0,FV8
        FMOV.S      FR7,FR10
        FNEG        FR11
        FMOV.S      FR0,FR4
        FMOV.S      FR1,FR5
        FMOV.S      FR2,FR6
        FLDI0       FR7
        FIPR        FV0,FV4
        MOV.W       #H'006C,R0
        FCMP/GT     FR3,FR11
        BF          ?done
        FCMP/GT     FR3,FR10
        BF          ?done
        FMOV.S      @(R0,R4),FR9
        FCMP/GT     FR7,FR9
        BF          ?done
        FMOV.S      FR7,FR9
        FSRRA       FR7
        MOV.W       #H'0074,R0
        FMOV.S      @(R0,R4),FR6
        FMUL        FR7,FR10
        FMOV.S      FR10,FR8
        FMUL        FR8,FR8
        FCMP/GT     FR6,FR8
        BF          ?done
        MOV.W       #H'0068,R0
        FMUL        FR7,FR11
        FMOV.S      @(R0,R4),FR5
        FLDI1       FR6
        FCMP/GT     FR5,FR9
        BF          ?cone
        MOV.W       #H'0078,R0
        FSQRT       FR9
        FMOV.S      @(R0,R4),FR0
        MOV.W       #H'007C,R0
        FNEG        FR0
        FMOV.S      @(R0,R4),FR6
        FMAC        FR0,FR9,FR6
?cone:
        MOV.W       #H'0070,R0
        FMUL        FR6,FR11
        FMOV.S      @(R0,R4),FR5
        FCMP/GT     FR8,FR5
        BF/S        ?add
        FMOV.S      FR11,FR0
        MOV.W       #H'008C,R0
        FMOV.S      FR3,FR4
        MOV.L       @(R0,R4),R3
        EXTU.W      R3,R2
        SHLR16      R3
        MOV         R2,R5
        SUB         R3,R2
        LDS         R2,FPUL
        FLDI0       FR1
        FLOAT       FPUL,FR9
        FCMP/EQ     FR9,FR1
        BT          ?add
        MOV.L       #_acosf,R3
        MOV.L       R4,@-R15
        FMOV.S      FR9,@-R15
        MOV.L       R5,@-R15
        FMOV.S      FR8,@-R15
        FMOV.S      FR10,FR4
        JSR         @R3
        FMOV.S      FR0,@-R15
        FMOV.S      @R15+,FR11
        MOVA        ?to_angle,R0
        FMOV.S      @R15+,FR10
        FMOV.S      @R0,FR9
        MOV.L       @R15+,R5
        FMUL        FR9,FR0
        FMOV.S      @R15+,FR9
        MOV.L       @R15+,R4
        LDS         R5,FPUL
        FLOAT       FPUL,FR8
        FSUB        FR0,FR8
        FDIV        FR9,FR8
        FMUL        FR8,FR11
        FMOV.S      FR11,FR0
?add:
        MOV.W       #H'0108,R2
        MOV.L       R4,@-R15
        MOV.L       #__nj_ptr_chunk_rgb_lt_argb_,R3
        FMOV.S      FR0,@-R15
        ADD         R4,R2
        MOV.L       @R3,R3
        FMOV.S      @R2+,FR1
        ADD         #4,R3
        FMOV.S      @R2+,FR2
        FMOV.S      @R2+,FR3
        FMOV.S      @R3+,FR4
        FMOV.S      @R3+,FR5
        FMOV.S      @R3+,FR6
        FMAC        FR0,FR3,FR6
        FMAC        FR0,FR2,FR5
        FMAC        FR0,FR1,FR4
        MOV.L       #_specular_power,R1
        FMOV.S      FR6,@-R3
        FMOV.S      FR5,@-R3
        MOV.W       #H'0088,R0
        FMOV.S      FR4,@-R3
        MOV.L       @(R0,R4),R4
        JSR         @R1
        FMOV.S      FR10,FR4
        FMOV.S      @R15+,FR1
        MOV.W       #H'0114,R2
        FMUL        FR1,FR0
        MOV.L       @R15+,R4
        MOV.L       #__nj_ptr_chunk_rgb_lt_argb_,R3
        ADD         R4,R2
        MOV.L       @R3,R3
        FMOV.S      @R2+,FR1
        MOV         R3,R5
        FMOV.S      @R2+,FR2
        ADD         #20,R5
        FMOV.S      @R2+,FR3
        FMOV.S      @R2+,FR4
        FMOV.S      @R3+,FR5
        FMOV.S      @R5+,FR6
        FMAC        FR0,FR1,FR5
        FMOV.S      @R5+,FR7
        FMAC        FR0,FR2,FR6
        FMOV.S      @R5+,FR8
        FMAC        FR0,FR3,FR7
        FMOV.S      FR5,@-R3
        FMAC        FR0,FR4,FR8
        FMOV.S      FR8,@-R5
        FMOV.S      FR7,@-R5
        FMOV.S      FR6,@-R5
?done:
        LDS.L       @R15+,PR
        RTS
        NOP
?to_angle:
        .FDATA.S    F'10430.381
}
#endif
/* clang-format on */

/* clang-format off */
#ifdef DREAMCAST
/* run each queued light's calculator on the light, from the last queued to
 * the first. a light whose _nj_light_clip_flag_ is set is skipped */
/* @asm epilogue: dead-return, SHC's RTS at 0x8c11a968 follows the body's own
 * return at 0x8c11a964 */
#pragma inline_asm(njChunkLightLoop)
void njChunkLightLoop(void)
{
        MOV.W       #H'0080,R0
        MOV.L       R8,@-R15
        STS.L       PR,@-R15
        MOV.L       #__nj_light_queue_,R3
        MOV.L       R9,@-R15
        MOV.L       @(R0,R3),R8
        MOV.L       R10,@-R15
        MOV.L       #__nj_ptr_chunk_rgb_lt_,R9
        MOV.L       R11,@-R15
        MOV.L       #__nj_light_clip_flag_,R10
        MOV.L       #__nj_chunk_rgb_lt_func_,R11
?next:
        TST         R8,R8
        BT          ?done
        ADD         #-1,R8
        MOV         R10,R6
        MOV         R8,R3
        SHLL2       R3
        ADD         R3,R6
        MOV         #0,R2
        MOV.L       @R6,R6
        CMP/EQ      R2,R6
        BF          ?next
        MOV         R11,R7
        ADD         R3,R7
        ADD         R9,R3
        MOV.L       @R7,R2
        JSR         @R2
        MOV.L       @R3,R4
        BRA         ?next
        NOP
?done:
        MOV.L       @R15+,R11
        MOV.L       @R15+,R10
        MOV.L       @R15+,R9
        LDS.L       @R15+,PR
        RTS
        MOV.L       @R15+,R8
}
#endif
/* clang-format on */

/* clang-format off */
#ifdef DREAMCAST
/* light the vertex at pnt, whose normal is nml, by the queued lights. argb[0]
 * gets the vertex's diffuse color, and argb[1] its specular color. with
 * reverse set, the vertex is lit from behind, as if its normal were -nml.
 *
 * the diffuse color starts as _nj_chunk_light_calc_ambient_ with an alpha of
 * 0, or as 0 while _nj_ignore_ambient_flag_ is set. the specular color's red,
 * green and blue start as 0, and its alpha keeps its value. after the lights,
 * the call adds _nj_light_material_'s diffuse alpha to the diffuse alpha.
 *
 * while _nj_depth_enable_ is set, the diffuse alpha fades where pnt->z is
 * above _nj_depth_near_. below _nj_depth_far_, it is multiplied by
 * 1 - _nj_depth_range_ * (pnt->z - _nj_depth_near_). at _nj_depth_far_ or
 * above, it becomes 0 */
/* @asm epilogue: dead-return, SHC's RTS at 0x8c11aa64 follows the body's own
 * return at 0x8c11aa60 */
#pragma inline_asm(njCalculateChunkLights)
void njCalculateChunkLights(NJS_ARGB *argb, NJS_POINT3 *pnt, NJS_VECTOR *nml,
                            Bool reverse)
{
        .IMPORT     __nj_light_material_
        .IMPORT     __nj_depth_enable_
        .IMPORT     __nj_depth_near_
        .IMPORT     __nj_depth_far_
        .IMPORT     __nj_depth_range_
        STS.L       PR,@-R15
        MOV.L       #__nj_ptr_chunk_rgb_lt_nml_,R1
        ADD         #-12,R15
        MOV.L       R6,@R1
        MOV.L       #__nj_ptr_chunk_rgb_lt_pnt_,R2
        MOV.L       R5,@R2
        MOV.L       #__nj_ptr_chunk_rgb_lt_argb_,R3
        MOV.L       @R2,R2
        MOV.L       R4,@R3
        FLDI0       FR1
        MOV.L       @R3,R3
        ADD         #32,R3
        FMOV.S      FR1,@-R3
        FMOV.S      FR1,@-R3
        FMOV.S      FR1,@-R3
        MOV.L       #__nj_ignore_ambient_flag_,R0
        ADD         #-4,R3
        MOV.L       @R0,R0
        CMP/EQ      #0,R0
        BT          ?ambient
        FMOV.S      FR1,@-R3
        FMOV.S      FR1,@-R3
        FMOV.S      FR1,@-R3
        BRA         ?reverse
        FMOV.S      FR1,@-R3
?ambient:
        MOV.L       #__nj_chunk_light_calc_ambient_,R2
        ADD         #4,R2
        FMOV.S      @R2+,FR2
        FMOV.S      @R2+,FR3
        FMOV.S      @R2+,FR4
        FMOV.S      FR4,@-R3
        FMOV.S      FR3,@-R3
        FMOV.S      FR2,@-R3
        FMOV.S      FR1,@-R3
?reverse:
        TST         R7,R7
        BT          ?lights
        MOV.L       #__nj_ptr_chunk_rgb_lt_nml_,R2
        MOV.L       @R2,R3
        FMOV.S      @R3+,FR1
        FMOV.S      @R3+,FR2
        FMOV.S      @R3+,FR3
        FNEG        FR1
        MOV         #4,R0
        FNEG        FR2
        FMOV.S      FR1,@R15
        FMOV.S      FR2,@(R0,R15)
        FNEG        FR3
        ADD         #4,R0
        FMOV.S      FR3,@(R0,R15)
        MOV.L       R15,@R2
?lights:
        MOV.L       #_njChunkLightLoop,R2
        JSR         @R2
        NOP
        MOV.L       #__nj_light_material_,R3
        MOV.W       #H'0010,R0
        MOV.L       #__nj_ptr_chunk_rgb_lt_argb_,R2
        FMOV.S      @(R0,R3),FR1
        MOV.L       @R2,R2
        MOV.L       #__nj_depth_enable_,R3
        FMOV.S      @R2+,FR2
        MOV.L       @R3,R3
        FADD        FR2,FR1
        TST         R3,R3
        FMOV.S      FR1,@-R2
        BT          ?done
        MOV.L       #__nj_ptr_chunk_rgb_lt_pnt_,R2
        MOV.L       @R2,R2
        MOV.L       #__nj_depth_near_,R1
        ADD         #8,R2
        FMOV.S      @R1,FR2
        FMOV.S      @R2,FR1
        FCMP/GT     FR2,FR1
        MOV.L       #__nj_depth_far_,R2
        BF          ?done
        FMOV.S      @R2,FR3
        MOV.L       #__nj_ptr_chunk_rgb_lt_argb_,R3
        FCMP/GT     FR1,FR3
        MOV.L       @R3,R3
        BF/S        ?far
        FMOV.S      @R3+,FR4
        MOV.L       #__nj_depth_range_,R1
        FSUB        FR2,FR1
        FMOV.S      @R1,FR0
        FLDI1       FR6
        FNEG        FR1
        FMAC        FR0,FR1,FR6
        BRA         ?alpha
        FMUL        FR6,FR4
?far:
        FLDI0       FR4
?alpha:
        FMOV.S      FR4,@-R3
?done:
        ADD         #12,R15
        LDS.L       @R15+,PR
        RTS
        NOP
}
#endif
/* clang-format on */

/* set _nj_light_clip_flag_ where a queued light can't reach the sphere of
 * radius about point. clear it for every other queued light. only a light
 * whose attr.lsrc holds NJD_SPOT_LIGHT or NJD_SPEC_POINT can fail to reach
 * it. it fails where attr.frang + radius is no more than its ltcal.lpnt's
 * distance from point */
void njGetChunkLightDistance(NJS_POINT3 *point, Float radius)
{
    Int i;
    NJS_LIGHT *light;
    Float d;
    Float distance2;
    Float distance;

    i = _nj_light_queue_.count;
    while (i != 0) {
        i--;
        light = _nj_light_control_.entries[_nj_light_queue_.slots[i]];
        if (light->attr.lsrc & (NJD_SPOT_LIGHT | NJD_SPEC_POINT)) {
            d = light->ltcal.lpnt.x - point->x;
            distance2 = d * d;
            d = light->ltcal.lpnt.y - point->y;
            distance2 += d * d;
            d = light->ltcal.lpnt.z - point->z;
            distance2 += d * d;
            distance = njSqrt(distance2);
            if (light->attr.frang + radius <= distance) {
                _nj_light_clip_flag_[i] = TRUE;
            } else {
                _nj_light_clip_flag_[i] = FALSE;
            }
        } else {
            _nj_light_clip_flag_[i] = FALSE;
        }
    }
}

/* clang-format off */
#ifdef DREAMCAST
/* @asm epilogue: dead-return, SHC's RTS at 0x8c11ab2c follows the body's own
 * return at 0x8c11ab24 and its pool */
#pragma inline_asm(njCnkSetAmbient)
void njCnkSetAmbient(Float r, Float g, Float b)
{
        MOV.L       #__nj_chunk_light_ambient_,R1
        ADD         #16,R1
        FMOV.S      FR6,@-R1
        FMOV.S      FR5,@-R1
        RTS
        FMOV.S      FR4,@-R1
}
#endif
/* clang-format on */

/* clang-format off */
#ifdef DREAMCAST
/* multiply the ambient color and each queued light's colors by
 * _nj_light_material_'s, red, green and blue alone. the products go to the
 * fields the calculators read. _nj_chunk_light_calc_ambient_ becomes
 * _nj_chunk_light_ambient_ times the material's ambient color. each queued
 * light's ltcal.mdif becomes its ltcal.dif times the material's diffuse
 * color. its ltcal.mspc becomes its ltcal.spc times the material's specular
 * color, and takes spc's alpha. the other alphas keep their values.
 *
 * each queued light's ltcal.exp takes the material's exponent, truncated to
 * an Int, as specular_power's step. where attr.lsrc holds NJD_SPOT_LIGHT,
 * NJD_SPEC_DIR or NJD_SPEC_POINT, ltcal.rate becomes
 * 1 / specular_power(ltcal.cosi, ltcal.exp) */
/* @asm epilogue: dead-return, SHC's RTS at 0x8c11ac40 follows the body's own
 * return at 0x8c11ac3a */
#pragma inline_asm(njCalcChunkLightMaterials)
void njCalcChunkLightMaterials(void)
{
        MOV.L       R8,@-R15
        MOV.L       #__nj_light_queue_,R1
        MOV.L       #__nj_light_material_,R2
        MOV.L       #__nj_chunk_light_ambient_,R4
        MOV.W       #H'0080,R0
        MOV.L       @(R0,R1),R8
        ADD         #4,R4
        ADD         #4,R2
        FMOV.S      @R4+,FR1
        FMOV.S      @R2+,FR4
        FMOV.S      @R4+,FR2
        FMUL        FR4,FR1
        FMOV.S      @R2+,FR5
        FMOV.S      @R4+,FR3
        FMUL        FR5,FR2
        FMOV.S      @R2+,FR6
        MOV.L       #__nj_chunk_light_calc_ambient_,R5
        FMUL        FR6,FR3
        ADD         #16,R5
        FMOV.S      FR3,@-R5
        FMOV.S      FR2,@-R5
        FMOV.S      FR1,@-R5
        MOV.L       R9,@-R15
        TST         R8,R8
        BT          ?done
        MOV.L       #__nj_light_control_,R6
        ADD         #-16,R2
?light:
        ADD         #-1,R8
        MOV         R1,R3
        MOV         R8,R7
        SHLL2       R7
        ADD         R7,R3
        MOV.L       @R3,R9
        MOV.W       #H'0014,R0
        SHLL2       R9
        ADD         R6,R9
        MOV         R2,R3
        MOV.L       @R9,R9
        ADD         R0,R3
        FMOV.S      @R3+,FR1
        FMOV.S      @R3+,FR2
        FMOV.S      @R3+,FR3
        MOV.W       #H'00D8,R0
        ADD         R9,R0
        FMOV.S      @R0+,FR4
        FMOV.S      @R0+,FR5
        FMOV.S      @R0+,FR6
        FMUL        FR4,FR1
        FMUL        FR5,FR2
        FMUL        FR6,FR3
        MOV.W       #H'0110,R0
        ADD         R9,R0
        FMOV.S      FR3,@R0
        FMOV.S      FR2,@-R0
        FMOV.S      FR1,@-R0
        MOV.W       #H'00E4,R7
        ADD         R9,R7
        MOV.W       #H'0120,R6
        FMOV.S      @R7+,FR1
        ADD         R9,R6
        FMOV.S      @R7+,FR2
        FMOV.S      @R7+,FR3
        FMOV.S      @R7+,FR4
        MOV.W       #H'0024,R0
        ADD         R2,R0
        FMOV.S      @R0+,FR5
        FMOV.S      @R0+,FR6
        FMOV.S      @R0+,FR7
        FMUL        FR5,FR2
        FMUL        FR6,FR3
        FMUL        FR7,FR4
        FMOV.S      FR4,@R6
        FMOV.S      FR3,@-R6
        FMOV.S      FR2,@-R6
        FMOV.S      FR1,@-R6
        MOV.W       #H'0030,R0
        FMOV.S      @(R0,R2),FR5
        FTRC        FR5,FPUL
        MOV.W       #H'0088,R0
        STS         FPUL,R7
        MOV.L       R7,@(R0,R9)
        MOV         #8,R1
        MOV.W       #H'0124,R0
        MOV         #32,R2
        MOV.L       @(R0,R9),R7
        OR          R2,R1
        MOV         #16,R3
        OR          R3,R1
        TST         R7,R1
        BT          ?next
        MOV.W       #H'0070,R0
        MOV.L       #_specular_power,R2
        FMOV.S      @(R0,R9),FR4
        STS.L       PR,@-R15
        MOV.W       #H'0088,R0
        JSR         @R2
        MOV.L       @(R0,R9),R4
        FLDI1       FR1
        LDS.L       @R15+,PR
        FDIV        FR0,FR1
        MOV.W       #H'0080,R0
        FMOV.S      FR1,@(R0,R9)
?next:
        MOV.L       #__nj_light_queue_,R1
        MOV.L       #__nj_light_material_,R2
        MOV.L       #__nj_light_control_,R6
        TST         R8,R8
        BT          ?done
        BRA         ?light
        NOP
?done:
        MOV.L       @R15+,R9
        RTS
        MOV.L       @R15+,R8
}
#endif
/* clang-format on */

/* clang-format off */
#ifdef DREAMCAST
/* set _nj_ignore_specular_flag_ where flags holds NJD_FST_IS >> 8, and
 * _nj_ignore_ambient_flag_ where it holds NJD_FST_IA >> 8. each flag is
 * cleared otherwise */
/* @asm epilogue: dead-return, SHC's RTS at 0x8c11ac74 follows the body's own
 * return at 0x8c11ac68 and its pool */
#pragma inline_asm(njCheckChunkLightMaterials)
void njCheckChunkLightMaterials(Int flags)
{
        MOV         #0,R3
        MOV.L       #__nj_ignore_specular_flag_,R1
        MOV.L       #__nj_ignore_ambient_flag_,R2
        MOV.L       R3,@R1
        MOV         #2,R5
        MOV.L       R3,@R2
        AND         R4,R5
        MOV         #4,R6
        TST         R5,R5
        MOV         #1,R3
        BT          ?ambient
        MOV.L       #__nj_ignore_specular_flag_,R1
        MOV.L       R3,@R1
?ambient:
        AND         R4,R6
        TST         R6,R6
        BT          ?done
        MOV.L       #__nj_ignore_ambient_flag_,R1
        MOV.L       R3,@R1
?done:
        RTS
        NOP
}
#endif
/* clang-format on */

/* clang-format off */
#ifdef DREAMCAST
/* queue each light of _nj_light_control_ whose stat is NJD_LIGHT_ON, after
 * the lights queued already. then set every queued light's
 * _nj_ptr_chunk_rgb_lt_ entry, and pick its calculator. its ltcal.lpnt and
 * ltcal.lvctr take its pnt and vctr, moved by its mtrx and then by
 * _nj_view_matrix_. the call does nothing where _nj_light_control_ holds no
 * light.
 *
 * an attr.lsrc equal to NJD_SPEC_DIR picks njChunkDirectLight, one equal to
 * NJD_SPEC_POINT njChunkPointLight, and one equal to NJD_SPOT_LIGHT
 * njChunkSpotLight. any other value picks njChunkDummyLight, even one that
 * holds one of those bits */
/* @asm operand: integer-constant, MOV.L at 0x8c11ad84 loads the kind
 * NJD_SPEC_POINT, 32, from the pool */
#pragma inline_asm(njSetChunkLightQueue)
void njSetChunkLightQueue(void)
{
        .IMPORT     __nj_view_matrix_
        .IMPORT     __nj_tmp_local_mtrx_
        .IMPORT     _njMultiMatrix
        .IMPORT     _njCalcPoint
        .IMPORT     _njCalcVector
        MOV.W       #H'0080,R0
        MOV.L       #__nj_light_control_,R3
        MOV         #0,R7
        MOV.L       @(R0,R3),R1
        MOV.L       R8,@-R15
        CMP/GT      R7,R1
        MOV         #0,R7
        MOV.L       R9,@-R15
        MOV         #1,R2
        STS.L       PR,@-R15
        MOV.L       #__nj_light_queue_,R4
        BT          ?scan
        BRA         ?done
        NOP
?scan:
        CMP/GE      R1,R7
        BT          ?set_up
        MOV         R7,R6
        MOV.W       #H'0058,R0
        SHLL2       R6
        ADD         R3,R6
        MOV.L       @R6,R5
        ADD         R0,R5
        MOV.L       @R5,R5
        CMP/EQ      R5,R2
        BF          ?scan_next
        MOV.W       #H'0080,R0
        MOV.L       @(R0,R4),R5
        MOV         R5,R0
        SHLL2       R0
        ADD         #1,R5
        MOV.L       R7,@(R0,R4)
        MOV.W       #H'0080,R0
        MOV.L       R5,@(R0,R4)
?scan_next:
        BRA         ?scan
        ADD         #1,R7
?set_up:
        MOV.W       #H'0080,R0
        MOV.L       #__nj_light_queue_,R1
        MOV.L       @(R0,R1),R8
?light:
        TST         R8,R8
        BT          ?done
        ADD         #-1,R8
        MOV.L       #__nj_light_queue_,R1
        MOV         R8,R7
        MOV.W       #H'0000,R0
        SHLL2       R7
        MOV.L       #__nj_light_control_,R2
        ADD         R7,R0
        MOV.L       @(R0,R1),R6
        MOV.W       #H'0000,R0
        SHLL2       R6
        MOV.L       #__nj_ptr_chunk_rgb_lt_,R3
        ADD         R6,R0
        MOV.L       @(R0,R2),R9
        ADD         R7,R3
        MOV.L       R9,@R3
        MOV.L       #__nj_view_matrix_,R1
        MOV.L       #__nj_tmp_local_mtrx_,R4
        MOV         #16,R3
?copy:
        MOV.L       @R1+,R0
        DT          R3
        MOV.L       R0,@R4
        BF/S        ?copy
        ADD         #4,R4
        MOV.L       #_njMultiMatrix,R2
        MOV.W       #H'0000,R0
        MOV.L       #__nj_tmp_local_mtrx_,R4
        MOV         R9,R5
        JSR         @R2
        ADD         R0,R5
        MOV.W       #H'0040,R0
        MOV.L       #_njCalcPoint,R2
        MOV         R9,R5
        MOV.W       #H'0090,R3
        ADD         R0,R5
        MOV.L       #__nj_tmp_local_mtrx_,R4
        MOV         R9,R6
        JSR         @R2
        ADD         R3,R6
        MOV.W       #H'004C,R0
        MOV.L       #_njCalcVector,R2
        MOV         R9,R5
        MOV.W       #H'009C,R3
        ADD         R0,R5
        MOV         R9,R6
        MOV.L       #__nj_tmp_local_mtrx_,R4
        JSR         @R2
        ADD         R3,R6
        MOV.W       #H'0124,R0
        MOV         R8,R6
        MOV.L       @(R0,R9),R1
        MOV         #16,R2
        MOV.L       #__nj_chunk_rgb_lt_func_,R7
        CMP/EQ      R1,R2
        SHLL2       R6
        BF/S        ?point
        ADD         R6,R7
        MOV.L       #_njChunkDirectLight,R3
        BRA         ?next
        MOV.L       R3,@R7
?point:
        MOV.L       #H'00000020,R2
        CMP/EQ      R1,R2
        BF          ?spot
        MOV.L       #_njChunkPointLight,R3
        BRA         ?next
        MOV.L       R3,@R7
?spot:
        MOV.L       #H'00000008,R2
        CMP/EQ      R1,R2
        BF          ?dummy
        MOV.L       #_njChunkSpotLight,R3
        BRA         ?next
        MOV.L       R3,@R7
?dummy:
        MOV.L       #_njChunkDummyLight,R3
        MOV.L       R3,@R7
?next:
        BRA         ?light
        NOP
?done:
        LDS.L       @R15+,PR
        MOV.L       @R15+,R9
        RTS
        MOV.L       @R15+,R8
}
#endif
/* clang-format on */

void njResetChunkLightQueue(void)
{
    _nj_light_queue_.count = 0;
}
