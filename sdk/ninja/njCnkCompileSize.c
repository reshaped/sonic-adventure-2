/* the direct compile's size calls. each walks a model's chunks as
 * njCnkCompile.c's compile does. it counts the bytes the compile writes, and
 * writes nothing */
#include <nj_cnk.h>
#include <nj_direct.h>
#include <ninja.h>
#include <sg_xpt.h>

/* the halfwords of a vertex in a UVN or a UVH strip: its index, u and v */
#define UV_VERTEX_HALFWORDS 3

/* the member exports these functions, though no header of the SDK declares
 * them */
/* NOLINTBEGIN(misc-use-internal-linkage) */
/* the bytes that njCnkDirectTexture, njCnkDirectTextureD8 and
 * njCnkDirectTextureEnvD8 each write for plist and strips */
Uint32 njCnkDirectTextureSize(Sint16 *plist, Uint32 strips);
/* the bytes that njCnkDirectPolygon, njCnkDirectPolygonD8 and
 * njCnkDirectPolygonEnvD8 each write for plist and strips */
Uint32 njCnkDirectPolygonSize(Sint16 *plist, Uint32 strips);
/* the bytes both passes of njCnkDirectPlist write for plist. compile_d8 picks
 * the global records' size */
Uint32 njCnkDirectPlistSize(Sint16 *plist);
/* set this source's compile_d8 from vlist, as njCnkDirectVlist sets its
 * own */
void njCnkDirectVlistSize(Sint16 *vlist);
/* the bytes a compile of model writes, under the compile mode the caller
 * set. model must have a polygon list */
Uint32 _njCnkDirectModelCompileSize(NJS_CNK_MODEL *model);
/* add the bytes a compile of object writes to object_size, under the compile
 * mode the caller set */
void _njCnkDirectObjectCompileSize(NJS_CNK_OBJECT *object);
/* NOLINTEND(misc-use-internal-linkage) */

static Bool compile_d8;
static Uint32 object_size;

Uint32 njCnkDirectTextureSize(Sint16 *plist, Uint32 strips)
{
    Sint32 len;
    Sint32 count;
    Sint32 user_halfwords;

    count = 0;
    user_halfwords = (strips >> NJD_UFO_SHIFT) & STRIP_USER;
    strips &= STRIP_COUNT;
    do {
        len = *plist++;
        plist++;
        if (len < 0) {
            len = -len;
            if (_nj_direct_culling_mode_ != NJD_POLYGON_CULLINGSMALL) {
                count++;
            }
        }
        len -= CNK_STRIP_LEAD;
        count++;
        /* plist has passed the first vertex's index */
        plist += (CNK_STRIP_LEAD * UV_VERTEX_HALFWORDS) - 1;
        do {
            count++;
            plist += UV_VERTEX_HALFWORDS;
            plist += user_halfwords;
        } while (--len);
        count++;
    } while (--strips);
    return count * sizeof(NJS_DIRECT_VERTEX);
}

Uint32 njCnkDirectPolygonSize(Sint16 *plist, Uint32 strips)
{
    Sint32 len;
    Sint32 count;
    Sint32 user_halfwords;

    count = 0;
    user_halfwords = (strips >> NJD_UFO_SHIFT) & STRIP_USER;
    strips &= STRIP_COUNT;
    do {
        len = *plist++;
        plist++;
        if (len < 0) {
            len = -len;
            if (_nj_direct_culling_mode_ != NJD_POLYGON_CULLINGSMALL) {
                count++;
            }
        }
        len -= CNK_STRIP_LEAD;
        count++;
        plist++;
        do {
            count++;
            plist++;
            plist += user_halfwords;
        } while (--len);
        count++;
    } while (--strips);
    return count * sizeof(NJS_DIRECT_VERTEX);
}

Uint32 njCnkDirectPlistSize(Sint16 *plist)
{
    Sint16 head;
    Sint16 type;
    Uint32 size;
    Uint32 strips;
    Uint32 total;
    Uint32 highlight; /* NOLINT(clang-diagnostic-unused-but-set-variable) */

    total = 0;
    while ((head = *plist++) != NJD_CE) {
        type = CNK_TYPE(head);
        if (type < NJD_TINYOFF) {
            /* @hack redundant: a switch on the bits chunks that does
             * nothing for any of them */
            switch (type) {
            case NJD_CB_BA:
                break;
            case NJD_CB_DA:
                break;
            case NJD_CB_EXP:
                break;
            }
        } else if (type < NJD_MATOFF) {
            plist++;
        } else if (type < NJD_STRIPOFF) {
            size = *plist++;
            plist += size;
        } else {
            size = (Uint16)*plist++;
            strips = *plist++;
            /* @hack redundant: head masked and shifted, and highlight set
             * from it, though nothing reads highlight */
            if (_nj_control_3d_flag_ & NJD_CONTROL_3D_CNK_CONSTANT_ATTR) {
                head &= _nj_constant_attr_and_;
                head |= _nj_constant_attr_or_;
            }
            head >>= NJD_FST_SHIFT;
            /* @hack spelling: a switch for each format, where one switch
             * that adds the format's record size would do */
            if (!compile_d8) {
                highlight = NJD_ARC1_HIGHLIGHT;
                if (head & CNK_FST(NJD_FST_IS)) {
                    highlight = 0;
                }
                switch (type) {
                case NJD_CS_UVN:
                case NJD_CS_UVH:
                    total += sizeof(NJS_DIRECT_GLOBAL);
                    total += njCnkDirectTextureSize(plist, strips);
                    break;
                case NJD_CS:
                    total += sizeof(NJS_DIRECT_GLOBAL);
                    total += njCnkDirectPolygonSize(plist, strips);
                    break;
                }
            } else {
                switch (type) {
                case NJD_CS_UVN:
                case NJD_CS_UVH:
                    total += sizeof(NJS_DIRECT_GLOBAL_D8);
                    total += njCnkDirectTextureSize(plist, strips);
                    break;
                case NJD_CS:
                    total += sizeof(NJS_DIRECT_GLOBAL_D8);
                    total += njCnkDirectPolygonSize(plist, strips);
                    break;
                }
            }
            /* size counts the chunk's halfwords after the size halfword.
             * plist has passed the first, which holds strips */
            plist += size - 1;
        }
    }
    return total;
}

/* @hack redundant: register on vlist */
void njCnkDirectVlistSize(register Sint16 *vlist)
{
    Int head;
    /* @hack redundant: register on size */
    register Int size;

    while ((head = *vlist++) != NJD_CE) {
        /* size counts the chunk's 4-byte words after the size halfword. the
         * walker steps over the first, which holds offset and count */
        size = (Uint16)*vlist++;
        vlist += CNK_WORD_HALFWORDS;
        switch (CNK_TYPE(head)) {
        case NJD_CV_VN:
        case NJD_CV:
            compile_d8 = FALSE;
            break;
        case NJD_CV_D8:
        case NJD_CV_VN_D8:
            compile_d8 = TRUE;
            break;
        }
        if (_nj_direct_compile_mode_ == DIRECT_COMPILE_LIGHT) {
            compile_d8 = TRUE;
        }
        vlist += (size * CNK_WORD_HALFWORDS) - CNK_WORD_HALFWORDS;
    }
}

Uint32 _njCnkDirectModelCompileSize(NJS_CNK_MODEL *model)
{
    Uint32 size;
    Sint32 *vlist;
    Sint16 *plist;

    vlist = model->vlist;
    if (vlist != NULL) {
        njCnkDirectVlistSize((Sint16 *)vlist);
    }
    plist = model->plist;
#ifdef NONMATCHING
    /* this side repairs the bug tagged below: it counts a model without a
     * polygon list as its head alone, which is what the compile writes for
     * it */
    size = sizeof(NJS_DIRECT_HEAD);
#endif
    /* @bug uninitialized: returns size unset for a model without a polygon
     * list, though the compile writes that model's head (BT at 0x8c12620e
     * skips the store to size, and MOV R5,R0 at 0x8c12621e returns it) */
    /* NOLINTBEGIN(clang-diagnostic-sometimes-uninitialized) */
    if (plist != NULL) {
        size = njCnkDirectPlistSize(plist) + sizeof(NJS_DIRECT_HEAD);
    }
    /* NOLINTEND(clang-diagnostic-sometimes-uninitialized) */
    return size;
}

void _njCnkDirectObjectCompileSize(NJS_CNK_OBJECT *object)
{
    /* @hack redundant: register on flags */
    register Uint32 flags;

    do {
        flags = object->evalflags;
        if (!(flags & NJD_EVAL_HIDE)) {
            object_size += _njCnkDirectModelCompileSize(object->model);
        }
        if (!(flags & NJD_EVAL_BREAK)) {
            _njCnkDirectObjectCompileSize(object->child);
        }
        object = object->sibling;
    } while (object != NULL);
}

Uint32 njCnkDirectModelCompileSize(NJS_CNK_MODEL *model)
{
    _nj_direct_compile_mode_ = DIRECT_COMPILE_PLAIN;
    return _njCnkDirectModelCompileSize(model);
}

Uint32 njCnkDirectObjectCompileSize(NJS_CNK_OBJECT *object)
{
    _nj_direct_compile_mode_ = DIRECT_COMPILE_PLAIN;
    object_size = 0;
    _njCnkDirectObjectCompileSize(object);
    return object_size;
}

Uint32 njCnkDirectModelCompileLightSize(NJS_CNK_MODEL *model)
{
    _nj_direct_compile_mode_ = DIRECT_COMPILE_LIGHT;
    return _njCnkDirectModelCompileSize(model);
}

Uint32 njCnkDirectObjectCompileLightSize(NJS_CNK_OBJECT *object)
{
    _nj_direct_compile_mode_ = DIRECT_COMPILE_LIGHT;
    object_size = 0;
    _njCnkDirectObjectCompileSize(object);
    return object_size;
}
