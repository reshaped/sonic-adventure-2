/* A direct object compile that writes no head leaves alone the head an
 * earlier compile wrote last, and writes through no head before any. */
#include <assert.h>
#include <kamui.h>
#include <nj_cnk.h>
#include <nj_cnk_set_material.h>
#include <nj_direct.h>
#include <nj_km.h>
#include <ninja.h>
#include <stdlib.h>

Uint32 _nj_constant_attr_and_ = ~0U;
Uint32 _nj_constant_attr_or_;
Uint32 _nj_control_3d_flag_;
struct nj_parameterkm _nj_parameterkm_;
NJS_VERTEX_BUF *_nj_vertex_buf_;

void njGetkmPointer(void)
{
}
void njSetkmPointer(void)
{
}
Bool njPushMatrixEx(void)
{
    return TRUE;
}
Bool njPopMatrixEx(void)
{
    return TRUE;
}
void njUnitMatrix(NJS_MATRIX *m)
{
}
void njTranslateEx(NJS_VECTOR *v)
{
}
void njRotateEx(Angle *ang, Sint32 lv)
{
}
void njScaleEx(NJS_VECTOR *v)
{
}
void njCalcPoint(NJS_MATRIX *m, NJS_POINT3 *ps, NJS_POINT3 *pd)
{
    *pd = *ps;
}

/* the model below has no vertex list and no polygon list, so the compile
 * reaches none of these */
void njCnkDirectVertexV(Float *vlist, NJS_DIRECT_BUF *buf, Int num)
{
    abort();
}
void njCnkDirectVertexVN(Float *vlist, NJS_DIRECT_BUF *buf, Int num)
{
    abort();
}
void njCnkDirectVertexVD8(Float *vlist, NJS_DIRECT_BUF *buf, Int num)
{
    abort();
}
void njCnkDirectVertexVND8(Float *vlist, NJS_DIRECT_BUF *buf, Int num)
{
    abort();
}
void *njCnkDirectPolygon(Sint16 *plist, NJS_DIRECT_BUF *vbuf, Uint32 strips,
                         void *ptr, Uint32 *vtx)
{
    abort();
}
void *njCnkDirectPolygonD8(Sint16 *plist, NJS_DIRECT_BUF *vbuf, Uint32 strips,
                           void *ptr, Uint32 *vtx)
{
    abort();
}
void *njCnkDirectPolygonEnvD8(Sint16 *plist, NJS_DIRECT_BUF *vbuf,
                              Uint32 strips, void *ptr, Uint32 *vtx)
{
    abort();
}
void *njCnkDirectTexture(Sint16 *plist, NJS_DIRECT_BUF *vbuf, Uint32 strips,
                         void *ptr, Float div, Uint32 *vtx)
{
    abort();
}
void *njCnkDirectTextureD8(Sint16 *plist, NJS_DIRECT_BUF *vbuf, Uint32 strips,
                           void *ptr, Float div, Uint32 *vtx)
{
    abort();
}
void *njCnkDirectTextureEnvD8(Sint16 *plist, NJS_DIRECT_BUF *vbuf,
                              Uint32 strips, void *ptr, Uint32 *vtx)
{
    abort();
}
void njCnkSetMaterial(Uint32 head, Uint32 *colors, Float *diffuse,
                      Float *ambient, Float *specular)
{
    abort();
}
void njSetkmCnkBlendMode(Uint32 head)
{
    abort();
}
Sint32 njSetkmCnkTextureNum(Uint32 head, Uint32 texid)
{
    abort();
}
void njSetkmColorMode(Uint32 mode)
{
    abort();
}

static NJS_CNK_MODEL model;
static NJS_CNK_OBJECT shown = {CNK_EVAL_UNIT | NJD_EVAL_BREAK, &model};
static NJS_CNK_OBJECT hidden = {CNK_EVAL_UNIT | NJD_EVAL_BREAK | NJD_EVAL_HIDE,
                                &model};
/* a compile of shown writes one head, and a compile of hidden none */
static NJS_DIRECT_HEAD earlier;
static NJS_DIRECT_HEAD later;

int main(void)
{
    /* before any compile, no head was written last */
    assert(njCnkDirectObjectCompile(&hidden, &later) == &later);
    assert(njCnkDirectObjectCompile(&shown, &earlier) == &earlier + 1);
    assert(earlier.next == NULL);
    earlier.next = &earlier;
    assert(njCnkDirectObjectCompile(&hidden, &later) == &later);
    assert(njCnkDirectObjectCompileLight(&hidden, &later, NULL) == &later);
    assert(earlier.next == &earlier);
    return 0;
}
