/* the strip compilers of NJD_CS_UVN and NJD_CS_UVH chunks. each vertex of
 * their strips gives a u and a v after its index */
#include <kamui.h>
#include <nj_cnk.h>
#include <nj_direct.h>
#include <ninja.h>
#include <sg_xpt.h>

#define UV_BIAS (1.0F / 512.0F)

#define TAKE_UV(uv, plist, div)                                                \
    {                                                                          \
        (uv).f.u = ((Float)(*(plist)++) * (div)) + UV_BIAS;                    \
        (uv).f.v = ((Float)(*(plist)++) * (div)) + UV_BIAS;                    \
    }

void *njCnkDirectTexture(Sint16 *plist, NJS_DIRECT_BUF *vbuf, Uint32 strips,
                         void *ptr, Float div, Uint32 *vtx)
{
    NJS_DIRECT_VERTEX *out = ptr;
    NJS_DIRECT_BUF *v;
    Uint32 count = 0;
    Uint32 user_halfwords = (strips >> NJD_UFO_SHIFT) & STRIP_USER;
    Sint32 len;
    union direct_uv uv;
    union direct_uv_word packed;

    strips &= STRIP_COUNT;
    do {
        len = *plist++;
        v = &vbuf[*plist++];
        if (len < 0) {
            len = -len;
            if (_nj_direct_culling_mode_ != NJD_POLYGON_CULLINGSMALL) {
                PUT_POINT_NORMAL(out, v);
                /* @hack spelling: u and v taken and plist stepped back over
                 * them, where reading plist[0] and plist[1] in place would
                 * do */
                TAKE_UV(uv, plist, div);
                PACK_UV(packed, uv);
                plist -= CNK_UV_HALFWORDS;
                END_RECORD(out, packed, KM_VERTEXPARAM_NORMAL);
                count++;
            }
        }
        len -= CNK_STRIP_LEAD;
        PUT_POINT_NORMAL(out, v);
        TAKE_UV(uv, plist, div);
        PACK_UV(packed, uv);
        END_RECORD(out, packed, KM_VERTEXPARAM_NORMAL);
        count++;
        v = &vbuf[*plist++];
        TAKE_UV(uv, plist, div);
        PACK_UV(packed, uv);
        do {
            PUT_POINT_NORMAL(out, v);
            END_RECORD(out, packed, KM_VERTEXPARAM_NORMAL);
            count++;
            v = &vbuf[*plist++];
            TAKE_UV(uv, plist, div);
            PACK_UV(packed, uv);
            plist += user_halfwords;
        } while (--len);
        PUT_POINT_NORMAL(out, v);
        END_RECORD(out, packed, KM_VERTEXPARAM_ENDOFSTRIP);
        count++;
    } while (--strips);
    *vtx = count;
    return out;
}

void *njCnkDirectTextureD8(Sint16 *plist, NJS_DIRECT_BUF *vbuf, Uint32 strips,
                           void *ptr, Float div, Uint32 *vtx)
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
                /* @hack spelling: u and v taken and plist stepped back over
                 * them, where reading plist[0] and plist[1] in place would
                 * do */
                TAKE_UV(uv, plist, div);
                PACK_UV(packed, uv);
                plist -= CNK_UV_HALFWORDS;
                END_RECORD(out, packed, KM_VERTEXPARAM_NORMAL);
                count++;
            }
        }
        len -= CNK_STRIP_LEAD;
        PUT_POINT_COLOR_D8(out, v, a, r, g, b);
        TAKE_UV(uv, plist, div);
        PACK_UV(packed, uv);
        END_RECORD(out, packed, KM_VERTEXPARAM_NORMAL);
        count++;
        v = &vbuf[*plist++];
        TAKE_UV(uv, plist, div);
        PACK_UV(packed, uv);
        do {
            PUT_POINT_COLOR_D8(out, v, a, r, g, b);
            END_RECORD(out, packed, KM_VERTEXPARAM_NORMAL);
            count++;
            v = &vbuf[*plist++];
            TAKE_UV(uv, plist, div);
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

void *njCnkDirectTextureEnvD8(Sint16 *plist, NJS_DIRECT_BUF *vbuf,
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
        plist += CNK_UV_HALFWORDS;
        ENV_UV(uv, v);
        PACK_UV(packed, uv);
        END_RECORD(out, packed, KM_VERTEXPARAM_NORMAL);
        count++;
        v = &vbuf[*plist++];
        ENV_UV(uv, v);
        PACK_UV(packed, uv);
        plist += CNK_UV_HALFWORDS;
        do {
            PUT_POINT_COLOR_D8(out, v, a, r, g, b);
            END_RECORD(out, packed, KM_VERTEXPARAM_NORMAL);
            count++;
            v = &vbuf[*plist++];
            ENV_UV(uv, v);
            PACK_UV(packed, uv);
            plist += CNK_UV_HALFWORDS;
            plist += user_halfwords;
        } while (--len);
        PUT_POINT_COLOR_D8(out, v, a, r, g, b);
        END_RECORD(out, packed, KM_VERTEXPARAM_ENDOFSTRIP);
        count++;
    } while (--strips);
    *vtx = count;
    return out;
}
