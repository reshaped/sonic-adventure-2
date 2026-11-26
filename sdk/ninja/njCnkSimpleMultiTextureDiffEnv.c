#include <kamui.h>
#include <machine.h>
#include <ninja.h>
#include <nj_cnk.h>
#include <nj_direct.h>
#include <nj_global.h>
#include <sg_xpt.h>

/* send the entry p points to as a vertex at sq, with param as its control word.
 * sq is left past the vertex */
#define SEND_VERTEX(param)                                                     \
    {                                                                          \
        /* @hack redundant: register on the macro's locals */                  \
        register Float sx;                                                     \
        register Float sy;                                                     \
        register Float ooz;                                                    \
        register Float u;                                                      \
        register Float v;                                                      \
        register Float ca;                                                     \
        register Float cr;                                                     \
        register Float cg;                                                     \
        register Float cb;                                                     \
        register Float sr;                                                     \
        register Float sg;                                                     \
        register Float sb;                                                     \
        /* @hack spelling: the entry's fields read as p steps along it, where  \
         * reads of entry's fields would do */                                 \
        p++;                                                                   \
        sx = *p++;                                                             \
        sy = *p++;                                                             \
        ooz = *p++;                                                            \
        p += CNK_SIMPLEMULTI_NX - CNK_SIMPLEMULTI_A;                           \
        u = uoff + (*p++ * CNK_ENV_SCALE);                                     \
        v = voff + (*p * CNK_ENV_SCALE);                                       \
        /* @hack spelling: the words stored as sq steps down through each      \
         * block, where indexed stores from sq would do */                     \
        sq += CNK_UV_END;                                                      \
        *(Float *)--sq = v;                                                    \
        *(Float *)--sq = u;                                                    \
        *(Float *)--sq = ooz;                                                  \
        *(Float *)--sq = sy;                                                   \
        *(Float *)--sq = sx;                                                   \
        *--sq = (param);                                                       \
        prefetch(sq);                                                          \
        p -= CNK_SIMPLEMULTI_NY - CNK_SIMPLEMULTI_A;                           \
        ca = a * *p++;                                                         \
        cr = (r * *p++) + ar;                                                  \
        cg = (g * *p++) + ag;                                                  \
        cb = (b * *p++) + ab;                                                  \
        p += CNK_SIMPLEMULTI_SR - CNK_SIMPLEMULTI_X;                           \
        sr = *p++;                                                             \
        sg = *p++;                                                             \
        sb = *p;                                                               \
        sq += CNK_VERTEX64_WORDS;                                              \
        *(Float *)--sq = sb;                                                   \
        *(Float *)--sq = sg;                                                   \
        *(Float *)--sq = sr;                                                   \
        sq--;                                                                  \
        *(Float *)--sq = cb;                                                   \
        *(Float *)--sq = cg;                                                   \
        *(Float *)--sq = cr;                                                   \
        *(Float *)--sq = ca;                                                   \
        prefetch(sq);                                                          \
        sq += CNK_BLOCK_WORDS;                                                 \
    }

Uint32 *njCnkSimpleMultiTextureDiffEnv(Sint16 *strips,
                                       NJS_CNK_SIMPLEMULTI_BUF *buf,
                                       Uint32 nbstrip, Uint32 *sq, Float a,
                                       Float r, Float g, Float b, Float ar,
                                       Float ag, Float ab)
{
    /* @hack redundant: register on every local */
    register NJS_CNK_SIMPLEMULTI_BUF *v0;
    register NJS_CNK_SIMPLEMULTI_BUF *v1;
    register NJS_CNK_SIMPLEMULTI_BUF *v2;
    register Sint32 len;
    register Sint32 user_halfwords;
    register Sint32 sent;
    register Sint32 drawn;
    register Sint32 calculated;
    register Float uoff;
    register Float voff;
    register Float model_side;
    register Float side;
    register Float facing;
    register Float *p;

    prefetch(strips + CNK_STRIP_AHEAD);
    user_halfwords = (nbstrip >> NJD_UFO_SHIFT) & STRIP_USER;
    nbstrip &= STRIP_COUNT;
    if (_nj_control_3d_flag_ & NJD_CONTROL_3D_ENV_UV_SCROLL) {
        uoff = _nj_uv_scroll_u_ + CNK_ENV_CENTER;
        voff = _nj_uv_scroll_v_ + CNK_ENV_CENTER;
    } else {
        uoff = CNK_ENV_CENTER;
        voff = CNK_ENV_CENTER;
    }
    /* @hack spelling: calculated and drawn zeroed in one chained assignment,
     * where a statement for each would do */
    calculated = drawn = 0;
    model_side = 1.0F;
    if (_nj_control_3d_flag_ & NJD_CONTROL_3D_MIRROR_MODEL) {
        model_side = -1.0F;
    }
    do {
        len = *strips++;
        v0 = &buf[*strips++];
        prefetch(v0);
        side = model_side;
        if (len < 0) {
            len = -len;
            side = -side;
        }
        sent = 0;
        strips += CNK_UV_HALFWORDS;
        calculated += len;
        len -= CNK_STRIP_LEAD;
        v1 = &buf[*strips++];
        prefetch(v1);
        strips += CNK_UV_HALFWORDS;
        do {
            v2 = &buf[*strips++];
            prefetch(v2);
            facing = (((v1->sx - v0->sx) * (v2->sy - v1->sy)) -
                      ((v1->sy - v0->sy) * (v2->sx - v1->sx))) *
                     side;
            strips += CNK_UV_HALFWORDS;
            if (facing < 0.0F) {
                if (sent == 0) {
                    /* @hack spelling: each caller of SEND_VERTEX points p at
                     * the entry's z and prefetches its second block, as p +
                     * CNK_BLOCK_WORDS for v0 and v2 and &v1->x for v1, where
                     * SEND_VERTEX taking the entry would do */
                    p = &v0->z;
                    prefetch(p + CNK_BLOCK_WORDS);
                    SEND_VERTEX(KM_VERTEXPARAM_NORMAL)
                    sent++;
                }
                p = &v1->z;
                prefetch(&v1->x);
                SEND_VERTEX(KM_VERTEXPARAM_NORMAL)
                sent++;
            } else if (sent != 0) {
                p = &v1->z;
                prefetch(&v1->x);
                SEND_VERTEX(KM_VERTEXPARAM_ENDOFSTRIP)
                sent++;
                drawn += sent;
                sent = 0;
            }
            strips += user_halfwords;
            prefetch(strips + CNK_STRIP_AHEAD);
            side = -side;
            v0 = v1;
            v1 = v2;
        } while (--len);
        if (sent != 0) {
            p = &v2->z;
            prefetch(p + CNK_BLOCK_WORDS);
            SEND_VERTEX(KM_VERTEXPARAM_ENDOFSTRIP)
            sent++;
            drawn += sent;
        }
    } while (--nbstrip);
    _nj_draw_polygon_ += drawn;
    _nj_calc_polygon_ += calculated;
    return sq;
}
