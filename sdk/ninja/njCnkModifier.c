/* the modifier volume chunk drawer. each NJD_CO_P3 chunk of a model's
 * polygon list becomes a modifier volume, which changes the polygons inside
 * it */
#include <machine.h>
#include <ninja.h>
#include <nj_cnk.h>
#include <nj_km.h>
#include <sg_xpt.h>

/* the member exports these functions, though no header of the SDK declares
 * them */
/* NOLINTBEGIN(misc-use-internal-linkage) */
/* draw model as njCnkModDrawModel does. the caller must call njGetkmPointer
 * before it, and njSetkmPointer after. it returns 1 where the model clip skips
 * the model, else 0 */
Sint32 _njCnkModDrawModel(NJS_CNK_MODEL *model);
/* draw object as njCnkModDrawObject does. the caller must make the same
 * calls around it */
void _njCnkModDrawObject(NJS_CNK_OBJECT *object);
/* NOLINTEND(misc-use-internal-linkage) */

static struct nj_cnk_mod_work work;

Sint32 _njCnkModDrawModel(NJS_CNK_MODEL *model)
{
    /* @hack redundant: register on vbuf, buf, plist, nbvertex, size and
     * near */
    register NJS_CNK_MOD_BUF *vbuf;
    register NJS_CNK_MOD_BUF *buf;
    Sint16 *vlist;
    register Sint16 *plist;
    Sint32 head;
    register Sint32 nbvertex;
    register Sint32 size;
    register Sint32 near;
    Sint16 chunk;
    Sint16 type;
    struct nj_cnk_mod_work *w;

    vbuf = (NJS_CNK_MOD_BUF *)_nj_vertex_buf_;
    vlist = (Sint16 *)model->vlist;
    if ((_nj_control_3d_flag_ & NJD_CONTROL_3D_MODEL_CLIP) &&
        njCnkModelClip(model)) {
        return 1;
    }
    head = *vlist++;
    /* @hack spelling: work's fields stored through w, where stores to the
     * fields themselves would do */
    w = &work;
    vlist++;
    buf = &vbuf[*vlist++];
    nbvertex = *vlist++;
    w->nbvertex = nbvertex;
    w->vertices = (NJS_POINT3 *)vlist;
    w->buf = buf;
    w->sq = SQ_ADDRESS((Uint32)buf);
    SQ_MAP((Uint32)buf);
    w->dist_x = _nj_screen_.dist * _nj_scr_aspect_x_;
    w->dist_y = _nj_screen_.dist * _nj_scr_aspect_y_;
    w->cx = _nj_screen_.cx;
    w->cy = _nj_screen_.cy;
    _nj_calc_vertex_ += nbvertex;
    /* @hack spelling: a switch of one case on head, where an if would do */
    switch (head) {
    case NJD_CV:
        /* @hack spelling: the count kept in near for the test, where the
         * test of the call's result would do */
        near = njCnkModVertex(&work);
        if (near >= nbvertex) {
            break;
        }
        plist = model->plist;
        while ((chunk = *plist++) != NJD_CE) {
            type = CNK_TYPE(chunk);
            if (type < NJD_MATOFF) {
                continue;
            }
            if (type < NJD_VOLOFF) {
                size = *plist++;
                plist += size;
                continue;
            }
            size = *plist++;
            nbvertex = *plist++;
            if (type == NJD_CO_P3) {
                njEndVertex(
                    njCnkModPolygon(plist, vbuf, nbvertex, njStartModifier()));
                if (_nj_control_3d_flag_ & NJD_CONTROL_3D_TRANS_MODIFIER) {
                    njEndVertex(njCnkModPolygon(plist, vbuf, nbvertex,
                                                njStartTransModifier()));
                }
            }
            /* size counts the chunk's halfwords after the size halfword.
             * plist has passed the first, which holds the triangle count */
            plist += size - 1;
            prefetch(plist);
        }
        break;
    }
    return 0;
}

void _njCnkModDrawObject(NJS_CNK_OBJECT *object)
{
    /* @hack redundant: register on flags and clipped */
    register Uint32 flags;
    register Sint32 clipped;

#ifdef NONMATCHING
    /* clipped starts at 0 in place of the caller's R13, as nj_cnk.h says */
    clipped = 0;
#endif
    do {
        flags = object->evalflags;
        if ((flags & CNK_EVAL_UNIT) == CNK_EVAL_UNIT) {
            if (!(flags & NJD_EVAL_HIDE)) {
                _njCnkModDrawModel(object->model);
            }
            if (!(flags & NJD_EVAL_BREAK)) {
                _njCnkModDrawObject(object->child);
            }
        } else {
#ifdef NONMATCHING
            /* This subtree needs a transform of its own. Its siblings can
             * still draw in the parent coordinate system. */
            if (!njPushMatrixEx()) {
                object = object->sibling;
                continue;
            }
#else
            njPushMatrixEx();
#endif
            if (!(flags & NJD_EVAL_UNIT_POS)) {
                njTranslateEx((NJS_VECTOR *)object->pos);
            }
            if (!(flags & NJD_EVAL_UNIT_ANG)) {
                /* @hack spelling: the rotation order passed in clipped,
                 * where flags & NJD_EVAL_ZXY_ANG passed in place would do */
                clipped = flags & NJD_EVAL_ZXY_ANG;
                njRotateEx(object->ang, clipped);
            }
            if (!(flags & NJD_EVAL_UNIT_SCL)) {
                njScaleEx((NJS_VECTOR *)object->scl);
            }
            if (!(flags & NJD_EVAL_HIDE)) {
                clipped = _njCnkModDrawModel(object->model);
            }
            if (!(flags & NJD_EVAL_BREAK)) {
                /* @bug uninitialized: tests clipped for a hidden node, as
                 * nj_cnk.h says (TST R13,R13 at 0x8c128d2c) */
                if (!(flags & NJD_EVAL_CLIP) || !clipped) {
                    _njCnkModDrawObject(object->child);
                }
            }
            njPopMatrixEx();
        }
        object = object->sibling;
    } while (object);
}

void njCnkModDrawModel(NJS_CNK_MODEL *model)
{
    njGetkmPointer();
    _njCnkModDrawModel(model);
    njSetkmPointer();
}

void njCnkModDrawObject(NJS_CNK_OBJECT *object)
{
    njGetkmPointer();
    prefetch(object);
    _njCnkModDrawObject(object);
    njSetkmPointer();
}
