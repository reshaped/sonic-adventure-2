/* Each simple multi clip sends nothing for a facing triangle that isn't whole
 * and has no corner beyond CNK_NEAR_Z. one with a corner beyond goes to the
 * near clip, which sends the part beyond. */
#include <assert.h>
#include <nj_cnk.h>
#include <nj_global.h>
#include <ninja.h>

NJS_SCREEN _nj_screen_;
Float _nj_scr_aspect_x_;
Float _nj_scr_aspect_y_;
Uint32 _nj_control_3d_flag_;
Bool _nj_flat_model_flag_;
Float _nj_uv_scroll_u_;
Float _nj_uv_scroll_v_;
Int _nj_calc_polygon_;
Int _nj_draw_polygon_;

/* a strip of one triangle: its length, then each corner's index. a Texture
 * clip's strip gives a u and a v after each index */
static Sint16 plain[] = {CNK_TRI_CORNERS, 0, 1, 2};
static Sint16 textured[] = {CNK_TRI_CORNERS, 0, 0, 0, 1, 0, 0, 2, 0, 0};

static Uint32 *polygon(NJS_CNK_SIMPLEMULTI_BUF *entries, Uint32 *sq)
{
    return njCnkSimpleMultiPolygonClip(plain, entries, 1, sq, 1.0F, 1.0F, 1.0F,
                                       1.0F, 0.0F, 0.0F, 0.0F);
}
static Uint32 *polygon_env(NJS_CNK_SIMPLEMULTI_BUF *entries, Uint32 *sq)
{
    return njCnkSimpleMultiPolygonEnvClip(plain, entries, 1, sq, 1.0F, 0.0F,
                                          0.0F, 0.0F);
}
static Uint32 *polygon_diff_env(NJS_CNK_SIMPLEMULTI_BUF *entries, Uint32 *sq)
{
    return njCnkSimpleMultiPolygonDiffEnvClip(plain, entries, 1, sq, 1.0F, 1.0F,
                                              1.0F, 1.0F, 0.0F, 0.0F, 0.0F);
}
static Uint32 *texture(NJS_CNK_SIMPLEMULTI_BUF *entries, Uint32 *sq)
{
    return njCnkSimpleMultiTextureClip(textured, entries, 1, sq, 1.0F, 1.0F,
                                       0.0F, 0.0F, 0.0F);
}
static Uint32 *texture_diff(NJS_CNK_SIMPLEMULTI_BUF *entries, Uint32 *sq)
{
    return njCnkSimpleMultiTextureDiffClip(textured, entries, 1, sq, 1.0F, 1.0F,
                                           1.0F, 1.0F, 1.0F, 0.0F, 0.0F, 0.0F);
}

/* a clip, and the words of each vertex it sends */
static const struct {
    Uint32 *(*clip)(NJS_CNK_SIMPLEMULTI_BUF *entries, Uint32 *sq);
    Int words;
} clips[] = {
    {polygon, CNK_BLOCK_WORDS},
    {polygon_env, CNK_VERTEX64_WORDS},
    {polygon_diff_env, CNK_VERTEX64_WORDS},
    {texture, CNK_VERTEX64_WORDS},
    {texture_diff, CNK_VERTEX64_WORDS},
};

static Uint32 sq[CNK_CLIP_VERTICES * CNK_VERTEX64_WORDS];

/* the vertices clip k sends for one triangle. its first corner lies at z
 * near, and the other two at z 0.5 */
static Int sent(Int k, Float near)
{
    NJS_CNK_SIMPLEMULTI_BUF entries[CNK_TRI_CORNERS] = {{0}};

    entries[0].z = near;
    entries[1].y = 1.0F;
    entries[1].z = 0.5F;
    entries[2].x = 1.0F;
    entries[2].z = 0.5F;
    /* the triangle faces the viewer */
    assert(CNK_POINT_WINDING(&entries[0], &entries[1], &entries[2]) < 0.0F);
    return (Int)(clips[k].clip(entries, sq) - sq) / clips[k].words;
}

int main(void)
{
    Int k;

    for (k = 0; k < (Int)(sizeof(clips) / sizeof(clips[0])); k++) {
        assert(sent(k, CNK_NEAR_Z) == 0);
        /* the strip holds the first corner and a cut on each of its edges */
        assert(sent(k, 1.5F) == CNK_TRI_CORNERS);
    }
    return 0;
}
