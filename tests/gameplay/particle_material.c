/* Exercise both particle render paths against a recording Ninja boundary. */
#include <assert.h>
#include <stdlib.h>

#include <ninja.h>
#include <shinobi.h>

#include "draw/lines.h"
#include "effect/particle.h"
#include "system/angle.h"
#include "system/game_state.h"
#include "system/task.h"
#include "system/util.h"

struct game_state game_state;
NJS_TEXLIST *_nj_current_texlist;
NJS_SCREEN _nj_screen_;
Float _nj_scr_aspect_x_ = 1.0F;
Float _nj_scr_aspect_y_ = 1.0F;

static struct task core;
static int binds, starts, selections, draws, ends;
static Sint32 selected[8];
static NJS_TEXLIST *last_bound;

void *syMalloc(Uint32 size)
{
    return malloc(size);
}
void syFree(void *p)
{
    free(p);
}
struct task *task_create(Uint16 flags, int level, void (*init)(struct task *),
                         const char *name)
{
    init(&core);
    return &core;
}
void task_destroy(struct task *task)
{
    task->destructor(task);
}
void njCalcPoint(NJS_MATRIX *m, NJS_POINT3 *from, NJS_POINT3 *to)
{
    *to = *from;
}
void njCalcVector(NJS_MATRIX *m, NJS_VECTOR *from, NJS_VECTOR *to)
{
    *to = *from;
}
Float njScalor(NJS_VECTOR *v)
{
    return 1.0F;
}
Float njSin(Angle angle)
{
    return 0.0F;
}
Float njCos(Angle angle)
{
    return 1.0F;
}
void draw_control_save(void)
{
}
void draw_control_restore(void)
{
}
void draw_control_set(Uint32 flags)
{
}
void draw_control_clear(Uint32 flags)
{
}
void constant_material_set(Float a, Float r, Float g, Float b)
{
}
void njFogEnable(void)
{
}
void njFogDisable(void)
{
}
void njColorBlendingMode(Int target, Int mode)
{
}
Sint32 njSetTexture(NJS_TEXLIST *texlist)
{
    ++binds;
    last_bound = _nj_current_texlist = texlist;
    return 0;
}
void njQuadTextureStart(Sint32 trans)
{
    assert(starts == ends);
    ++starts;
}
void njSetQuadTexture(Sint32 frame, Uint32 color)
{
    assert(starts == ends + 1 && _nj_current_texlist != NULL);
    assert(frame >= 0 && (Uint32)frame < _nj_current_texlist->nbTexture);
    assert(_nj_current_texlist->textures[frame].texaddr != 0);
    assert(selections < (int)(sizeof(selected) / sizeof(selected[0])));
    selected[selections++] = frame;
}
void njDrawQuadTextureEx(NJS_QUAD_TEXTURE_EX *quad)
{
    assert(starts == ends + 1 && quad->z == 1.0F);
    ++draws;
}
void njQuadTextureEnd(void)
{
    assert(starts == ends + 1);
    ++ends;
}

static void reset_draw(void)
{
    starts = selections = draws = ends = binds = 0;
    last_bound = NULL;
}

static void draw_one(Sint32 frame)
{
    NJS_POINT3 pos = {0, 0, 1};
    particle_draw_quad(&pos, frame, QUARTER_TURN, 0, 1, 1, 0xffffffff, 0, 0, 0);
}

static void spawn(struct particle_group *group, Float frame)
{
    struct particle *p = particle_create(group);
    assert(p != NULL);
    p->frame = frame;
    p->pos.z = 1.0F;
}

int main(void)
{
    NJS_TEXNAME textures[3] = {{0}};
    NJS_TEXLIST bank = {textures, 3};
    struct particle_group live = {0};
#ifdef NONMATCHING
    struct particle_group absent = {0};
    NJS_TEXLIST missing = {NULL, 3};
    volatile Float zero = 0.0F;
#endif

    _nj_screen_.dist = 1.0F;
    textures[0].texaddr = 1;
    textures[2].texaddr = 2;
#ifdef NONMATCHING
    draw_one(0);
    _nj_current_texlist = &missing;
    draw_one(0);
    _nj_current_texlist = &bank;
    bank.nbTexture = 0;
    draw_one(0);
    bank.nbTexture = 3;
    draw_one(-1);
    draw_one(3);
    draw_one(1);
    assert(starts == 0 && selections == 0 && draws == 0 && ends == 0);
#endif
    _nj_current_texlist = &bank;
    draw_one(0);
    assert(binds == 0 && starts == 1 && selections == 1 && selected[0] == 0 &&
           draws == 1 && ends == 1);

    reset_draw();
    particle_core_start();
    live.texlist = &bank;
    spawn(&live, -0.5F);
    spawn(&live, 0.75F);
    spawn(&live, 2.5F);
#ifdef NONMATCHING
    spawn(&live, -1.0F);
    spawn(&live, 1.0F);
    spawn(&live, 3.0F);
    spawn(&live, 2147483648.0F);
    spawn(&live, zero / zero);
    spawn(&absent, 0);
#endif
    core.draw(&core);
    assert(binds == 1 && last_bound == &bank && starts == 1 && ends == 1 &&
           selections == 2 && selected[0] == 2 && selected[1] == 0 &&
           draws == 3);

#ifdef NONMATCHING
    reset_draw();
    absent.texlist = &missing;
    bank.nbTexture = 0;
    core.draw(&core);
    assert(binds == 0 && starts == 0 && selections == 0 && draws == 0 &&
           ends == 0);
    bank.nbTexture = 3;
    textures[1].texaddr = 3;
    core.draw(&core);
    assert(binds == 1 && last_bound == &bank && starts == 1 && ends == 1 &&
           selections == 3 && selected[0] == 1 && selected[1] == 2 &&
           selected[2] == 0 && draws == 4);
    reset_draw();
    draw_one(1);
    assert(binds == 0 && starts == 1 && selections == 1 && selected[0] == 1 &&
           draws == 1 && ends == 1);
#endif
    particle_core_stop();
    assert(live.particles == NULL);
    return 0;
}
