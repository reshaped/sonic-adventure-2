/* SHC intrinsic functions, as far as the sources use them */
#ifndef MACHINE_H
#define MACHINE_H

#ifdef DREAMCAST
#ifdef NONMATCHING
#include <sh4zam/shz_scalar.h>
#include <sh4zam/shz_xmtrx.h>

/* SHC builds each intrinsic in. this side defines each for GCC, as the
 * instructions SHC writes for it. under -m4-single-only, GCC's code takes
 * FPSCR's PR and SZ clear, and names neither bank of the FPU's registers.
 * set_fpscr, ld_ext, st_ext and fsca below rely on that */
#ifndef __SH4_SINGLE_ONLY__
#error "the non-matching code takes -m4-single-only"
#endif
#endif
#endif

/* set_imask and get_imask write and read SR's interrupt mask. IMASK_ALL masks
 * every interrupt, and 0 masks none. get_vbr returns VBR, the base address of
 * the exception and interrupt handlers */
#ifdef DREAMCAST
#ifdef NONMATCHING
/* SR's interrupt mask: SR_IMASK_BITS, from bit SR_IMASK_SHIFT */
#define SR_IMASK_SHIFT 4
#define SR_IMASK_BITS 0xf
/* set_imask, get_imask and get_vbr are always inline, as SHC writes them in
 * place. code that runs uncached through P2, as syCacheInit_P2_ does, can't
 * call out to P1. set_imask's LDC writes all of SR, T among it. no load or
 * store moves across the LDC, which opens or closes what the mask guards */
__attribute__((always_inline)) static inline void _builtin_set_imask(int mask)
{
    int sr;

    __asm__ volatile("stc sr,%0" : "=r"(sr));
    sr = (sr & ~(SR_IMASK_BITS << SR_IMASK_SHIFT)) |
         ((mask & SR_IMASK_BITS) << SR_IMASK_SHIFT);
    __asm__ volatile("ldc %0,sr" : : "r"(sr) : "t", "memory");
}

__attribute__((always_inline)) static inline int _builtin_get_imask(void)
{
    int sr;

    __asm__ volatile("stc sr,%0" : "=r"(sr));
    return (sr >> SR_IMASK_SHIFT) & SR_IMASK_BITS;
}

__attribute__((always_inline)) static inline void *_builtin_get_vbr(void)
{
    void *vbr;

    __asm__ volatile("stc vbr,%0" : "=r"(vbr));
    return vbr;
}
#endif
#endif
#define set_imask(mask) _builtin_set_imask(mask)
#define get_imask() _builtin_get_imask()
#define get_vbr() _builtin_get_vbr()
#define IMASK_ALL 15

/* set_fpscr and get_fpscr write and read FPSCR. its bit FPSCR_DN has the FPU
 * take a denormal operand as zero */
#ifdef DREAMCAST
#ifdef NONMATCHING
/* this side's set_fpscr keeps FPSCR's PR, SZ and FR as they are, where
 * SHC's writes them from cr. the two agree on PR and SZ where cr holds them
 * clear, as FPSCR_DN does. they also agree where cr is FPSCR as get_fpscr
 * read it. this side must keep FR, which tells which bank is XMTRX */
#define _builtin_set_fpscr __builtin_sh_set_fpscr
#define _builtin_get_fpscr __builtin_sh_get_fpscr
#endif
#endif
#define set_fpscr(cr) _builtin_set_fpscr(cr)
#define get_fpscr() _builtin_get_fpscr()
#define FPSCR_DN 0x40000
/* FPSCR's bit FPSCR_PR has the FPU work in double precision */
#define FPSCR_PR 0x80000

/* FSCA: store the sine of angle at sinv and its cosine at cosv, both to the
 * instruction's precision. angle counts 65536 to a full turn, and only its
 * low 16 bits are read. FSRRA: 1 over the square root of x, to the
 * instruction's precision */
#ifdef DREAMCAST
#ifdef NONMATCHING
/* FSCA writes its pair directly to FR0 and FR1. Binding both outputs here
 * saves the two register moves in SH4ZAM's scalar helper. */
static inline void _builtin_fsca(long angle, float *sinv, float *cosv)
{
    register float sine __asm__("fr0");
    register float cosine __asm__("fr1");

    __asm__("lds %2,fpul\n\tfsca fpul,dr0"
            : "=f"(sine), "=f"(cosine)
            : "r"(angle)
            : "fpul");
    *sinv = sine;
    *cosv = cosine;
}

#define _builtin_fsrra shz_inv_sqrtf_fsrra_sh4
#endif
#endif
#define fsca(angle, sinv, cosv) _builtin_fsca(angle, sinv, cosv)
#define fsrra(x) _builtin_fsrra(x)

/* Load the sixteen floats at mx into XMTRX. The row type guarantees only
 * float alignment. SH4ZAM uses paired transfers when mx is 8-byte aligned. */
#ifdef DREAMCAST
#ifdef NONMATCHING
/* this side loads through SH4ZAM's unaligned load, which takes any
 * float-aligned mx */
static inline void _builtin_ld_ext(float (*mx)[4])
{
    shz_xmtrx_load_unaligned_4x4((float *)mx);
}
#endif
#endif
#define ld_ext(mx) _builtin_ld_ext(mx)

/* Store XMTRX at mx with the same alignment contract as ld_ext. */
#ifdef DREAMCAST
#ifdef NONMATCHING
/* this side stores through SH4ZAM's unaligned store, as ld_ext loads */
static inline void _builtin_st_ext(float (*mx)[4])
{
    shz_xmtrx_store_unaligned_4x4((float *)mx);
}
#endif
#endif
#define st_ext(mx) _builtin_st_ext(mx)

/* FTRV: set vec2 to XMTRX times vec1, each a vector of four floats. part i of
 * vec2 is the inner product of vec1 with XMTRX's floats i, i + 4, i + 8 and
 * i + 12, to the instruction's precision. vec2 may be vec1 */
#ifdef DREAMCAST
#ifdef NONMATCHING
/* FTRV reads and writes FV0, FR0 to FR3, which the four register variables
 * name. the asm is volatile so that it keeps its place among ld_ext's and
 * st_ext's. ld_ext sets XMTRX, and st_ext reads it */
static inline void _builtin_ftrv(float *vec1, float *vec2)
{
    register float x __asm__("fr0") = vec1[0];
    register float y __asm__("fr1") = vec1[1];
    register float z __asm__("fr2") = vec1[2];
    register float w __asm__("fr3") = vec1[3];

    __asm__ volatile("ftrv xmtrx,fv0" : "+f"(x), "+f"(y), "+f"(z), "+f"(w));
    vec2[0] = x;
    vec2[1] = y;
    vec2[2] = z;
    vec2[3] = w;
}
#endif
#endif
#define ftrv(vec1, vec2) _builtin_ftrv(vec1, vec2)

/* TAS.B: in one indivisible operation, set bit 7 of the byte at addr and
 * return whether the byte was 0 */
#ifdef DREAMCAST
#ifdef NONMATCHING
/* GCC writes no TAS.B for C, so this side's asm does. its memory clobber keeps
 * loads and stores on their side of it */
static inline int _builtin_tas(char *addr)
{
    int was_zero;

    __asm__ volatile("tas.b @%1\n\tmovt %0"
                     : "=r"(was_zero)
                     : "r"(addr)
                     : "t", "memory");
    return was_zero;
}
#endif
#endif
#define tas(addr) _builtin_tas(addr)

/* PREF: prefetch the cache line at p. at a store queue address, write that
 * store queue to memory */
#ifdef DREAMCAST
#ifdef NONMATCHING
/* PREF at a store queue address writes out the stores before it, so no load
 * or store moves across it. GCC's __builtin_prefetch lets them move */
static inline void _builtin_prefetch(void *p)
{
    __asm__ volatile("pref @%0" : : "r"(p) : "memory");
}
#endif
#endif
#define prefetch(p) _builtin_prefetch(p)

/* TRAPA: raise a trap exception with vector, a constant from 0 to 255. the
 * handler at VBR + 0x100 finds vector times 4 in TRA */
#ifdef DREAMCAST
#ifdef NONMATCHING
/* this side's asm takes vector as TRAPA's immediate. its memory clobber keeps
 * loads and stores on their side of the trap */
#define _builtin_trapa(vector)                                                 \
    __asm__ volatile("trapa %0" : : "i"(vector) : "memory")
#endif
#endif
#define trapa(vector) _builtin_trapa(vector)

/* the string intrinsics, which SHC compiles to calls to the C runtime's
 * _slow_strcpy and _slow_strcmp1 */
#ifdef DREAMCAST
#ifdef NONMATCHING
/* _slow_strcpy takes s1 in R0 and s2 in R1, outside C's calling convention.
 * it returns s1 in R0 and changes R1 and T alone. the asm moves s1 and s2
 * into R0 and R1 itself */
void _slow_strcpy(void);
int _slow_strcmp1(const char *s1, const char *s2);
int _quick_strcmp1(const char *s1, const char *s2);

static inline char *_builtin_strcpy(char *s1, const char *s2)
{
    char *to;

    __asm__ volatile("mov %2,r1\n\tjsr @%3\n\tmov %1,r0\n\tmov r0,%0"
                     : "=r"(to)
                     : "r"(s1), "r"(s2), "r"(_slow_strcpy)
                     : "r0", "r1", "pr", "t", "memory");
    return to;
}

#define _builtin_strcmp _slow_strcmp1
#endif
#endif

#endif
