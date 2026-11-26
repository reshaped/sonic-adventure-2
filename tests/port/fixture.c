/* what engine/dc's port.c, pace.c, readout.c and wide.c reach outside
 * themselves,
 * for tests/test_port.py: Ninja's screen and the draws the port calls, the
 * first pad's record, and the game's frame state. each stub keeps what it
 * was last called with */
#include <kamui.h>
#include <math.h>
#include <ninja.h>
#include <nj_cnk.h>
#include <nj_direct_draw.h>
#include <nj_global.h>
#include <nj_km.h>
#include <rnext.h>
#include <sg_xpt.h>
#include <shinobi.h>
#include <string.h>

#include "engine/dc/port.h"
#include "system/frame.h"
#include "system/game_state.h"

#undef njDrawSprite2D
#undef njProjectScreen

NJS_SCREEN _nj_screen_ = {500.0F, 640.0F, 480.0F, 320.0F, 240.0F};
Float _nj_scr_aspect_x_ = 1.0F;
Float _nj_scr_aspect_y_ = 1.0F;
struct nj_cnk_easy_work _nj_vertex_easy;
struct nj_cnk_simple_work _nj_vertex_simple;
struct direct_lighting direct_light;
struct game_state game_state;
int frame_interval = 1;
int frame_step_count = 1;

PDS_PERIPHERAL fixture_pad;
NJS_SPRITE fixture_sprite;
Float fixture_projected_aspect;
int fixture_lights;
char fixture_prints[16][40];
Uint32 fixture_print_colors[16];
int fixture_print_count;
int fixture_bands;
Int fixture_band_cover;
/* the fog mode each band drew under, and the vertex context that holds the
 * game's */
int fixture_band_fog;
static struct tagKMVERTEXCONTEXT fixture_context;
struct tagKMVERTEXCONTEXT *_nj_current_ctx_ = &fixture_context;

const PDS_PERIPHERAL *pdGetPeripheral(Uint32 port)
{
    return &fixture_pad;
}

void njDrawSprite2D(NJS_SPRITE *sp, Int n, Float pri, Uint32 attr)
{
    fixture_sprite = *sp;
}

void njProjectScreen(NJS_MATRIX *m, NJS_POINT3 *p3, NJS_POINT2 *p2)
{
    fixture_projected_aspect = _nj_scr_aspect_x_;
    p2->x = _nj_screen_.cx + (p3->x * _nj_scr_aspect_x_);
    p2->y = _nj_screen_.cy + p3->y;
}

void njCnkSetEasyLight(Float x, Float y, Float z)
{
    fixture_lights++;
}

void njCnkSetSimpleLight(Float x, Float y, Float z)
{
    fixture_lights++;
}

void njSetDirectLight(Float x, Float y, Float z)
{
    fixture_lights++;
}

void njSetDirectClip(void)
{
    fixture_lights++;
}

Float fixture_band_x[8];

void njDrawPolygon2D(NJS_POINT2COL *p, Int n, Float pri, Uint32 attr)
{
    if (fixture_bands < 4) {
        fixture_band_x[2 * fixture_bands] = p->p[0].x;
        fixture_band_x[(2 * fixture_bands) + 1] = p->p[1].x;
    }
    fixture_bands++;
    fixture_band_cover = port_wide_cover;
    fixture_band_fog = fixture_context.FogMode;
}

void njFogEnable(void)
{
    fixture_context.FogMode = KM_FOGTABLE;
}

void njFogDisable(void)
{
    fixture_context.FogMode = KM_NOFOG;
}

/* the fog mode the game's draws left, a KM_ value, and the mode that stands */
void fixture_set_fog(int mode)
{
    fixture_context.FogMode = mode;
}

int fixture_fog(void)
{
    return fixture_context.FogMode;
}

void readout_print(Int loc, const char *s, Uint16 size, Uint32 c)
{
    if (fixture_print_count < 16) {
        strncpy(fixture_prints[fixture_print_count], s, 39);
        fixture_print_colors[fixture_print_count] = c;
        fixture_print_count++;
    }
}

/* the display the game runs under, a DISPLAY_ value of game_state.h */
void fixture_set_display(int display)
{
    game_state.display = (Sint8)display;
}

/* draw the readout and the note, keeping only this draw's prints */
void fixture_draw(void)
{
    fixture_print_count = 0;
    fixture_bands = 0;
    memset(fixture_prints, 0, sizeof(fixture_prints));
    readout_draw();
}

/* the buttons held on the first pad, which fixture_serve gives the port as
 * the peripheral server fills its record */
Uint32 fixture_held;

void fixture_serve(void)
{
    fixture_pad.old = fixture_pad.on;
    fixture_pad.on = fixture_held;
    fixture_pad.press = fixture_pad.on & ~fixture_pad.old;
    fixture_pad.l = fixture_held & PDD_DGT_TL ? 255 : 0;
    fixture_pad.r = fixture_held & PDD_DGT_TR ? 255 : 0;
}

/* the two timers, which a test moves, and what the split's picture did */
Uint32 fixture_count;
int fixture_timer_runs = 1;
int fixture_flips;
int fixture_step_draws;
int fixture_drawn_stopped;
unsigned long _rnext;
Bool frame_render_off;
NJS_ARGB _nj_constant_material_;
const NJS_ARGB sprite_white = {1.0F, 1.0F, 1.0F, 1.0F};
Uint32 fixture_strip[64];
Uint32 fixture_color_mode;
NJS_POINT2 _nj_2d_clip_[2] = {{0.0F, 0.0F}, {639.0F, 479.0F}};
Float _nj_near_clip_ = 1.0F;
Float _nj_far_clip_ = 60000.0F;
NJS_MATRIX fixture_stack[2];
NJS_MATRIX *_nj_base_matrix_ptr_ = &fixture_stack[0];
NJS_MATRIX *_nj_current_matrix_ptr_ = &fixture_stack[0];
NJS_MATRIX _nj_view_matrix_;

/* a frame of blanks blanks that no loop offers the split: the blanks, its
 * end, and the next frame's start */
void fixture_frame(int blanks)
{
    int i;

    for (i = 0; i < blanks; i++) {
        port_blank();
    }
    frame_end(fixture_count, blanks - frame_interval);
    fixture_serve();
    frame_begin();
}

Uint32 syTmrGetCount(void)
{
    return fixture_count;
}

Uint32 syTmrDiffCount(Uint32 count1, Uint32 count2)
{
    return count2 - count1;
}

void syTmrGenStop(void)
{
    fixture_timer_runs = 0;
}

void syTmrGenStart(void)
{
    fixture_timer_runs = 1;
}

void njRenderStart(void)
{
}

void njFlipFrameBuffer(void)
{
    fixture_flips++;
}

/* a step's draw, which moves the view and draws at random, as a camera's
 * draw and a task's may */
void fixture_step_draw(void)
{
    fixture_step_draws++;
    fixture_drawn_stopped = !fixture_timer_runs;
    _rnext += 12345;
    _nj_screen_.dist += 100.0F;
    _nj_screen_.cx = 480.0F;
    _nj_2d_clip_[0].x = 320.0F;
    _nj_far_clip_ = 3000.0F;
    _nj_current_matrix_ptr_ = &fixture_stack[1];
    fixture_stack[0][12] += 1.0F;
    _nj_view_matrix_[12] += 1.0F;
}

/* a held frame of two steps that run first and second of syTmrGetCount's
 * counts, with a draw of draw after them, and its end after blanks blanks */
void fixture_held_frame(Uint32 first, Uint32 second, Uint32 draw, int blanks)
{
    frame_interval = 2;
    frame_step_count = 2;
    fixture_count += first;
    present_step(1, fixture_step_draw);
    fixture_count += second;
    present_step(2, fixture_step_draw);
    fixture_count += draw;
    game_state.frame_time = (Float)(first + second + draw) * 1000.0F / 13034.0F;
    fixture_frame(blanks);
}

void njGetkmPointer(void)
{
}

void njSetkmPointer(void)
{
}

Sint32 njSetTexture(NJS_TEXLIST *list)
{
    return 0;
}

Sint32 njSetkmTextureNum(Uint32 n)
{
    return 0;
}

void njSetkmColorMode(Uint32 mode)
{
    fixture_color_mode = mode;
}

Uint32 *njStartVertexTexOpaqueIntens(Float *face)
{
    return fixture_strip;
}

Uint32 *njStartVertexTexTransIntens(Float *face)
{
    return fixture_strip;
}

void njEndVertex(const Uint32 *sq)
{
}

Float njSin(Angle angle)
{
    return (Float)sin(angle * 3.14159265358979323846 / 0x8000);
}

Float njCos(Angle angle)
{
    return (Float)cos(angle * 3.14159265358979323846 / 0x8000);
}
