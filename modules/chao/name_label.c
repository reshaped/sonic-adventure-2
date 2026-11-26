/* a Chao's name, drawn in the world or on the screen. a name label is a
 * balloon that springs open above a Chao. it stays up while the player holds
 * the Chao, for HOLD_FRAMES + 2 frames at most, then closes */
#include <ninja.h>
#include <sg_xpt.h>
#ifdef NONMATCHING
/* memcpy, with which this side's draw copies the name */
#include <string.h>
#endif

#include "chao/garden.h"
#include "chao/name_quad.h"
#include "chao/ui.h"
#include "chao/work.h"
#include "player/player.h"
#include "system/task.h"

#define QUAD_VERTICES 4
#define LABEL_RGB 0x6699ff
/* the frame draws white, at the label's alpha */
#define WHITE_RGB 0xffffff
/* chao_common_texlist's font and balloon, and the font's global index */
#define FONT_TEXTURE 0
#define BALLOON_TEXTURE 1
#define FONT_GLOBAL_INDEX 0x29810
/* the font is 16 rows of 16 glyphs, a glyph for each byte value. a glyph's
 * row is its code's high four bits */
#define FONT_COLUMNS 16
#define FONT_ROW_SHIFT 4
#define GLYPH_UV (1.0F / FONT_COLUMNS)
/* a glyph's width and height on the screen at scale 1, in pixels. in the
 * world a glyph is 1 across */
#define SCREEN_GLYPH 16.0F

static NJS_TEXTURE_VTX glyph[QUAD_VERTICES] = {
#include "chao/name_label/glyph.inc"
};
static NJS_TEXTURE_VTX frame_left[QUAD_VERTICES] = {
#include "chao/name_label/frame_left.inc"
};
static NJS_TEXTURE_VTX frame_body[QUAD_VERTICES] = {
#include "chao/name_label/frame_body.inc"
};
static NJS_TEXTURE_VTX frame_right[QUAD_VERTICES] = {
#include "chao/name_label/frame_right.inc"
};
static NJS_TEXTURE_VTX frame_tail[QUAD_VERTICES] = {
#include "chao/name_label/frame_tail.inc"
};
static NJS_TEXTURE_VTX shadow_left[QUAD_VERTICES] = {
#include "chao/name_label/shadow_left.inc"
};
static NJS_TEXTURE_VTX shadow_body[QUAD_VERTICES] = {
#include "chao/name_label/shadow_body.inc"
};
static NJS_TEXTURE_VTX shadow_right[QUAD_VERTICES] = {
#include "chao/name_label/shadow_right.inc"
};
static NJS_TEXTURE_VTX shadow_tail[QUAD_VERTICES] = {
#include "chao/name_label/shadow_tail.inc"
};

#define COLOR_QUAD(quad)                                                       \
    {                                                                          \
        (quad)[0].col = color;                                                 \
        (quad)[1].col = color;                                                 \
        (quad)[2].col = color;                                                 \
        (quad)[3].col = color;                                                 \
    }
#define GLYPH_TILE(quad)                                                       \
    {                                                                          \
        COLOR_QUAD(quad);                                                      \
        tile_u = (code % FONT_COLUMNS) * GLYPH_UV;                             \
        tile_v = (code >> FONT_ROW_SHIFT) * GLYPH_UV;                          \
        (quad)[0].u = tile_u;                                                  \
        (quad)[0].v = tile_v;                                                  \
        (quad)[1].u = tile_u;                                                  \
        (quad)[1].v = tile_v + GLYPH_UV;                                       \
        (quad)[2].u = tile_u + GLYPH_UV;                                       \
        (quad)[2].v = tile_v;                                                  \
        (quad)[3].u = tile_u + GLYPH_UV;                                       \
        (quad)[3].v = tile_v + GLYPH_UV;                                       \
    }
static void draw_glyph(char character, Uint32 color)
{
    Uint8 code = character;
    Float tile_u;
    Float tile_v;
    GLYPH_TILE(glyph);
    njDrawTexture3DEx(glyph, QUAD_VERTICES, TRUE);
}
#define GLYPH_CORNER(i, x_pos, y_pos)                                          \
    {                                                                          \
        garden_screen_glyph[i].x = (x_pos);                                    \
        garden_screen_glyph[i].y = (y_pos);                                    \
        garden_screen_glyph[i].z = 0.8F;                                       \
    }
static void draw_screen_glyph(Float x, Float y, char character, Uint32 color,
                              Float scale)
{
    Float half = scale * (SCREEN_GLYPH * 0.5F);
    Uint8 code = character;
    Float tile_u;
    Float tile_v;
    GLYPH_CORNER(0, x - half, y - half);
    GLYPH_CORNER(1, x - half, y + half);
    GLYPH_CORNER(2, x + half, y - half);
    GLYPH_CORNER(3, x + half, y + half);
    GLYPH_TILE(garden_screen_glyph);
    njDrawTexture(garden_screen_glyph, QUAD_VERTICES, FONT_GLOBAL_INDEX, TRUE);
}
#define DRAW_OFFSET(part, x, y, z)                                             \
    {                                                                          \
        njPushMatrixEx();                                                      \
        njTranslate(NULL, x, y, z);                                            \
        njDrawTexture3DEx(part, QUAD_VERTICES, TRUE);                          \
        njPopMatrixEx();                                                       \
    }
static void draw_name_frame(int length, Uint32 color)
{
    Float half = length * 0.5F;
    COLOR_QUAD(shadow_left);
    COLOR_QUAD(shadow_right);
    COLOR_QUAD(shadow_body);
    COLOR_QUAD(shadow_tail);
    DRAW_OFFSET(shadow_tail, 0.0F, 1.0F, -0.015F);
    DRAW_OFFSET(shadow_left, -half, 0.0F, -0.005F);
    DRAW_OFFSET(shadow_right, half, 0.0F, -0.005F);
    njPushMatrixEx();
    njTranslate(NULL, 0.0F, 0.0F, -0.005F);
    njScale(NULL, length - 1, 1.0F, 1.0F);
    njDrawTexture3DEx(shadow_body, QUAD_VERTICES, TRUE);
    njPopMatrixEx();
    color |= WHITE_RGB;
    COLOR_QUAD(frame_left);
    COLOR_QUAD(frame_right);
    COLOR_QUAD(frame_body);
    COLOR_QUAD(frame_tail);
    DRAW_OFFSET(frame_tail, 0.0F, 1.0F, -0.01F);
    DRAW_OFFSET(frame_left, -half, 0.0F, 0.0F);
    DRAW_OFFSET(frame_right, half, 0.0F, 0.0F);
    njPushMatrixEx();
    njScale(NULL, length - 1, 1.0F, 1.0F);
    njDrawTexture3DEx(frame_body, QUAD_VERTICES, TRUE);
    njPopMatrixEx();
}
#define MEASURE_NAME()                                                         \
    while (*cursor++ != '\0') {                                                \
        length++;                                                              \
    }
#define DRAW_LETTERS()                                                         \
    while (*cursor != '\0') {                                                  \
        draw_glyph(*cursor++, color);                                          \
        njTranslate(NULL, 1.0F, 0.0F, 0.0F);                                   \
    }
void garden_draw_world_name(NJS_POINT3 *position, const char *name,
                            Uint32 color, Float scale)
{
    int length = 0;
    const char *cursor = name;
    Float left;
    NJS_POINT3 camera;
    MEASURE_NAME();
    left = -((length - 1) * 0.5F);
    cursor = name;
    njTextureFilterMode(NJD_TEXTUREFILTER_POINT_SAMPLE);
    njSetTexture(&chao_common_texlist);
    njSetTextureNum(FONT_TEXTURE);
    njCalcPoint(NULL, position, &camera);
    njPushMatrix(&_nj_unit_matrix_);
    njTranslateV(NULL, &camera);
    njScale(NULL, scale, scale * 1.1F, 1.0F);
    njTranslate(NULL, left, 0.0F, -2.0F);
    DRAW_LETTERS();
    njPopMatrixEx();
    njTextureFilterMode(NJD_TEXTUREFILTER_BILINEAR);
}
void garden_draw_name(NJS_POINT2 *position, const char *name, Uint32 color,
                      Float scale, int center)
{
    int length = 0;
    const char *cursor = name;
    int i;
    Float advance = scale * SCREEN_GLYPH;
    Float left;
    /* @hack redundant: unused, a point that only sizes the frame */
    NJS_POINT3 unused; /* NOLINT(clang-diagnostic-unused-variable) */
    MEASURE_NAME();
    cursor = name;
    if (center) {
        left = -((length - 1) * (SCREEN_GLYPH * 0.5F));
        left *= scale;
    } else {
        left = 0.0F;
    }
    njTextureFilterMode(NJD_TEXTUREFILTER_POINT_SAMPLE);
    njSetTexture(&chao_common_texlist);
    njSetTextureNum(FONT_TEXTURE);
    for (i = 0; *cursor != '\0'; i++) {
        draw_screen_glyph(position->x + left + (i * advance), position->y,
                          *cursor++, color, scale);
    }
    njTextureFilterMode(NJD_TEXTUREFILTER_BILINEAR);
}
static void draw_name_balloon(NJS_POINT3 *position, const char *name,
                              Uint32 color, Float scale)
{
    int length = 0;
    const char *cursor = name;
    Float left;
    NJS_POINT3 camera;
    MEASURE_NAME();
    if (length == 0) {
        return;
    }
    left = -((length - 1) * 0.5F);
    cursor = name;
    njTextureFilterMode(NJD_TEXTUREFILTER_POINT_SAMPLE);
    njSetTexture(&chao_common_texlist);
    njCalcPoint(NULL, position, &camera);
    njPushMatrix(&_nj_unit_matrix_);
    njTranslateV(NULL, &camera);
    njScale(NULL, scale, scale, 1.0F);
    njTranslate(NULL, 0.0F, 0.0F, -2.0F);
    njSetTextureNum(BALLOON_TEXTURE);
    draw_name_frame(length, color);
    njTranslate(NULL, left, 0.0F, -0.01F);
    njSetTextureNum(FONT_TEXTURE);
    DRAW_LETTERS();
    njPopMatrixEx();
    njTextureFilterMode(NJD_TEXTUREFILTER_BILINEAR);
}
/* the label's work. a spring opens the balloon. the balloon's scale and its
 * height above the Chao follow the spring's length */
#define BALLOON_SCALE(work) ((work)->scl.x)
#define SPRING_LENGTH(work) ((work)->scl.y)
#define SPRING_SPEED(work) ((work)->scl.z)
#define BALLOON_HEIGHT(work) ((work)->pos.y)
#define LABEL_ALPHA(work) ((work)->timer)
#define BALLOON_BASE 3.0F
#define HOLD_FRAMES 150
/* the alpha a fading label loses each frame, and below which it is gone */
#define FADE_STEP 8
#define FADED 5

/* work->mode in name_label_step */
enum label_mode {
    LABEL_START,
    LABEL_OPEN,
    LABEL_HOLD,
    LABEL_CLOSE,
    LABEL_FALL
};
#define FADE_LABEL()                                                           \
    {                                                                          \
        LABEL_ALPHA(work) -= FADE_STEP;                                        \
        if (LABEL_ALPHA(work) < FADED) {                                       \
            LABEL_ALPHA(work) = 0;                                             \
            task_kill(label);                                                  \
        }                                                                      \
    }
static void name_label_step(struct task *label)
{
    struct task_work *work = label->work;
    switch (work->mode) {
    case LABEL_START:
        BALLOON_HEIGHT(work) = BALLOON_BASE;
        BALLOON_SCALE(work) = 0.0F;
        SPRING_SPEED(work) = 0.0F;
        LABEL_ALPHA(work) = 0;
        work->mode = LABEL_OPEN;
        /* fall through */
    case LABEL_OPEN:
        SPRING_SPEED(work) += (1.0F - SPRING_LENGTH(work)) * 0.4F;
        SPRING_SPEED(work) *= 0.85F;
        if (SPRING_SPEED(work) > 0.1F) {
            SPRING_SPEED(work) = 0.1F;
        }
        SPRING_LENGTH(work) += SPRING_SPEED(work);
        if ((SPRING_SPEED(work) * SPRING_SPEED(work)) < 0.00001F &&
            SPRING_LENGTH(work) > 0.999F && SPRING_LENGTH(work) < 1.001F) {
            SPRING_LENGTH(work) = 1.0F;
            work->tick = 0;
            work->mode = LABEL_HOLD;
        }
        LABEL_ALPHA(work) += 15;
        if (LABEL_ALPHA(work) > 255) {
            LABEL_ALPHA(work) = 255;
        }
        BALLOON_SCALE(work) = SPRING_LENGTH(work) * 1.25F;
        BALLOON_HEIGHT(work) = (SPRING_LENGTH(work) * 2.5F) + BALLOON_BASE;
        break;
    case LABEL_HOLD:
        if (work->tick++ > HOLD_FRAMES) {
            work->mode = LABEL_CLOSE;
        }
        if (!(label->parent->work->flags & WORK_HELD_BY_PLAYER)) {
            work->mode = LABEL_CLOSE;
        }
        break;
    case LABEL_CLOSE:
        FADE_LABEL();
        BALLOON_SCALE(work) -= 0.06F;
        if (BALLOON_SCALE(work) < 0.8F) {
            work->mode = LABEL_FALL;
        }
        BALLOON_HEIGHT(work) -= 0.05F;
        break;
    case LABEL_FALL:
        FADE_LABEL();
        BALLOON_SCALE(work) -= 0.06F;
        if (BALLOON_SCALE(work) < 0.0F) {
            task_kill(label);
        }
        BALLOON_HEIGHT(work) -= 0.15F;
        if (BALLOON_HEIGHT(work) < 0.0F) {
            task_kill(label);
        }
        break;
    }
}
static void name_label_destroy(struct task *label)
{
}
static void name_label_draw(struct task *label)
{
    struct task_work *work = label->work;
    struct task *parent = label->parent;
    struct chao_work *owner;
    Uint32 color;
    NJS_POINT3 position;
#ifdef NONMATCHING
    /* the name and a terminating zero, which this side's draw copies. the
     * profile's name may fill its field */
    char name[sizeof(owner->profile.parameters.name) + 1];
#else
    Float *from;
#endif
    if (!parent) {
        return;
    }
    owner = (struct chao_work *)parent->work;
#ifdef NONMATCHING
    /* the plain form the hack tag names */
    position = owner->base.pos;
    position.y += BALLOON_HEIGHT(work);
#else
    /* @hack spelling: the position copied a float at a time through from,
     * where a copy of the point would do */
    from = &owner->base.pos.x;
    position.x = *from++;
    position.y = (*from++) + BALLOON_HEIGHT(work);
    position.z = *from;
#endif
    if (!(label->parent->work->flags & WORK_HELD_BY_PLAYER)) {
        position.y += 1.0F;
    }
    color = ((Uint32)LABEL_ALPHA(work) << 24) | LABEL_RGB;
#ifdef NONMATCHING
    /* this side repairs the bug tagged in the matching side. a name can fill
     * the profile's eight bytes with no terminator. this side draws a copy that
     * ends in a zero */
    memcpy(name, owner->profile.parameters.name,
           sizeof(owner->profile.parameters.name));
    name[sizeof(owner->profile.parameters.name)] = '\0';
    draw_name_balloon(&position, name, color, BALLOON_SCALE(work));
#else
    /* @bug memory: draw_name_balloon reads past a name that fills all eight
     * bytes, up to the first 0 after it (0x0c5129a2, 0x0c512a3e).
     * chao_import_legacy writes such a name from a record whose eight codes
     * all have glyphs (0x0c50ca18, 0x0c50ca22) */
    draw_name_balloon(&position, owner->profile.parameters.name, color,
                      BALLOON_SCALE(work));
#endif
}
void garden_create_name_label(struct task *chao)
{
    struct task *label = task_create_child(TASK_WORK, name_label_step, chao);
#ifdef NONMATCHING
    /* this side makes no label where task_create_child fails. the matching side
     * writes through the null */
    if (!label) {
        return;
    }
#endif
    label->draw = name_label_draw;
    label->destructor = name_label_destroy;
}
