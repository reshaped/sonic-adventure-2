/* the strip compilers of NJD_CS chunks, whose vertices are indices alone */
#include <kamui.h>
#include <nj_cnk.h>
#include <nj_cnk_set_material.h>
#include <nj_direct.h>
#include <ninja.h>
#include <sg_xpt.h>

#define PUT_VERTEX(out, v, vertex_param)                                       \
    {                                                                          \
        PUT_POINT_NORMAL(out, v);                                              \
        (out)->param = (vertex_param);                                         \
        (out)++;                                                               \
    }

/* a, r, g and b are the caller's Float locals, which the macro overwrites */
#define PUT_VERTEX_D8(out, v, vertex_param, a, r, g, b)                        \
    {                                                                          \
        PUT_POINT_D8(out, v);                                                  \
        if (_nj_direct_compile_mode_ == DIRECT_COMPILE_LIGHT) {                \
            (a) = _nj_direct_col[ARGB_A] * COLOR_MAX;                          \
            (r) = _nj_direct_col[ARGB_R] * RED_BYTE((v)->col);                 \
            (g) = _nj_direct_col[ARGB_G] * GREEN_BYTE((v)->col);               \
            (b) = _nj_direct_col[ARGB_B] * BLUE_BYTE((v)->col);                \
            PUT_BASE(out, a, r, g, b);                                         \
        } else {                                                               \
            (out)->base = (v)->col;                                            \
        }                                                                      \
        (out)->off = 0;                                                        \
        (out)->param = (vertex_param);                                         \
        (out)++;                                                               \
    }

void *njCnkDirectPolygon(Sint16 *plist, NJS_DIRECT_BUF *vbuf, Uint32 strips,
                         void *ptr, Uint32 *vtx)
{
    NJS_DIRECT_VERTEX *out = ptr;
    NJS_DIRECT_BUF *v;
    Uint32 count = 0;
    Uint32 user_halfwords = (strips >> NJD_UFO_SHIFT) & STRIP_USER;
    Sint32 len;

    strips &= STRIP_COUNT;
    do {
        len = *plist++;
        v = &vbuf[*plist++];
        if (len < 0) {
            len = -len;
            if (_nj_direct_culling_mode_ != NJD_POLYGON_CULLINGSMALL) {
                PUT_VERTEX(out, v, KM_VERTEXPARAM_NORMAL);
                count++;
            }
        }
        len -= CNK_STRIP_LEAD;
        PUT_VERTEX(out, v, KM_VERTEXPARAM_NORMAL);
        count++;
        v = &vbuf[*plist++];
        do {
            PUT_VERTEX(out, v, KM_VERTEXPARAM_NORMAL);
            count++;
            v = &vbuf[*plist++];
            plist += user_halfwords;
        } while (--len);
        PUT_VERTEX(out, v, KM_VERTEXPARAM_ENDOFSTRIP);
        count++;
    } while (--strips);
    *vtx = count;
    return out;
}

void *njCnkDirectPolygonD8(Sint16 *plist, NJS_DIRECT_BUF *vbuf, Uint32 strips,
                           void *ptr, Uint32 *vtx)
{
    NJS_DIRECT_VERTEX_D8 *out = ptr;
    NJS_DIRECT_BUF *v;
    Uint32 count = 0;
    Uint32 user_halfwords = (strips >> NJD_UFO_SHIFT) & STRIP_USER;
    Sint32 len;
    Float a;
    Float r;
    Float g;
    Float b;

    strips &= STRIP_COUNT;
    do {
        len = *plist++;
        v = &vbuf[*plist++];
        if (len < 0) {
            len = -len;
            if (_nj_direct_culling_mode_ != NJD_POLYGON_CULLINGSMALL) {
                PUT_VERTEX_D8(out, v, KM_VERTEXPARAM_NORMAL, a, r, g, b);
                count++;
            }
        }
        len -= CNK_STRIP_LEAD;
        PUT_VERTEX_D8(out, v, KM_VERTEXPARAM_NORMAL, a, r, g, b);
        count++;
        v = &vbuf[*plist++];
        do {
            PUT_VERTEX_D8(out, v, KM_VERTEXPARAM_NORMAL, a, r, g, b);
            count++;
            v = &vbuf[*plist++];
            plist += user_halfwords;
        } while (--len);
        PUT_VERTEX_D8(out, v, KM_VERTEXPARAM_ENDOFSTRIP, a, r, g, b);
        count++;
    } while (--strips);
    *vtx = count;
    return out;
}

void *njCnkDirectPolygonEnvD8(Sint16 *plist, NJS_DIRECT_BUF *vbuf,
                              Uint32 strips, void *ptr, Uint32 *vtx)
{
    NJS_DIRECT_VERTEX_D8 *out = ptr;
    NJS_DIRECT_BUF *v;
    Uint32 count = 0;
    Uint32 user_halfwords = (strips >> NJD_UFO_SHIFT) & STRIP_USER;
    Sint32 len;
    Float a;
    Float r;
    Float g;
    Float b;
    union direct_uv uv;
    union direct_uv_word packed;

    strips &= STRIP_COUNT;
    do {
        len = *plist++;
        v = &vbuf[*plist++];
        if (len < 0) {
            len = -len;
            if (_nj_direct_culling_mode_ != NJD_POLYGON_CULLINGSMALL) {
                PUT_POINT_COLOR_D8(out, v, a, r, g, b);
                ENV_UV(uv, v);
                PACK_UV(packed, uv);
                END_RECORD(out, packed, KM_VERTEXPARAM_NORMAL);
                count++;
            }
        }
        len -= CNK_STRIP_LEAD;
        PUT_POINT_COLOR_D8(out, v, a, r, g, b);
        ENV_UV(uv, v);
        PACK_UV(packed, uv);
        END_RECORD(out, packed, KM_VERTEXPARAM_NORMAL);
        count++;
        v = &vbuf[*plist++];
        ENV_UV(uv, v);
        PACK_UV(packed, uv);
        do {
            PUT_POINT_COLOR_D8(out, v, a, r, g, b);
            END_RECORD(out, packed, KM_VERTEXPARAM_NORMAL);
            count++;
            v = &vbuf[*plist++];
            ENV_UV(uv, v);
            PACK_UV(packed, uv);
            plist += user_halfwords;
        } while (--len);
        PUT_POINT_COLOR_D8(out, v, a, r, g, b);
        END_RECORD(out, packed, KM_VERTEXPARAM_ENDOFSTRIP);
        count++;
    } while (--strips);
    *vtx = count;
    return out;
}
