/* blasts: three shells of fire about a point, two spheres and a flat ring.
 * the shells grow and fade. every blast but a tiny one throws out black smoke
 * as it grows */
#include <ninja.h>
#include <sg_xpt.h>
#include <shinobi.h>

#include "draw/lines.h"
#include "draw/texture_anim.h"
#include "system/port.h"
#include "shared/blast/text.inc"
#include "shared/blast.h"
#include "shared/black_smoke.h"
#include "system/game_state.h"
#include "system/task.h"
#include "system/texture.h"
#include "system/util.h"

#define BLAST_LEVEL 3
#define TEXTURE_LEVEL 0

#define BLAST_SHELLS 3
/* the draw turns TILTED_SHELL, the second sphere, about x by SHELL_TILT: -1/16
 * of a turn */
#define TILTED_SHELL 1
#define SHELL_TILT 0xf000
/* the ring, which stays at the blast's point where a raised kind lifts the
 * spheres */
#define RING_SHELL 2
/* how far a raised kind lifts the spheres, by its size */
#define LARGE_LIFT 10.0F
#define MEDIUM_LIFT 5.0F
#define SMALL_LIFT 3.0F

/* the draw clears DRAW_BITS_OFF in _nj_control_3d_flag_ and sets DRAW_BITS_ON.
 * each shell then takes its diffuse color from the constant material. the draw
 * makes that material white, at the shell's alpha */
#define DRAW_BITS_OFF                                                          \
    (NJD_CONTROL_3D_CONSTANT_TEXTURE_MATERIAL | NJD_CONTROL_3D_CONSTANT_ATTR)
#define DRAW_BITS_ON NJD_CONTROL_3D_CONSTANT_MATERIAL

enum blast_mode { MODE_GROWING, MODE_ENDED };

/* a blast ends once its timer passes its size's lifetime, in steps */
#define TINY_STEPS 45
#define SMALL_STEPS 60
#define MEDIUM_STEPS 60
#define LARGE_STEPS 180
#define HUGE_STEPS 180

/* a blast's record, which its task's extra points to */
struct blast {
    Sint8 mode; /* an enum blast_mode */
    Sint8 kind; /* an enum blast_kind */
    Sint16 timer; /* the steps the blast has taken */
    NJS_POINT3 pos;
    Float scale[BLAST_SHELLS];
    Float unk_1c[2];
    Float growth[BLAST_SHELLS]; /* what each step adds to scale */
    Float unk_30[2];
    Float alpha[BLAST_SHELLS];
    Float unk_44[2];
};

static NJS_TEXNAME blast_texnames[1] = {
#include "shared/blast/blast_texnames.inc"
};
NJS_TEXLIST blast_texlist = {
#include "shared/blast/blast_texlist.inc"
};
/* the first shell's model, a sphere of SHELL_0_POINTS points and radius
 * 3.5 */
#define SHELL_0_POINTS 32
static Sint16 blast_polygons_0[277] = {
#include "shared/blast/blast_polygons_0.inc"
};
static Sint32 blast_vertices_0[195] = {
#include "shared/blast/blast_vertices_0.inc"
};
static NJS_CNK_MODEL blast_model_0 = {
#include "shared/blast/blast_model_0.inc"
};
static NJS_CNK_OBJECT blast_object_0 = {
#include "shared/blast/blast_object_0.inc"
};
/* the second shell's model, a sphere of SHELL_1_POINTS points and radius 5 */
#define SHELL_1_POINTS 110
static Sint16 blast_polygons_1[829] = {
#include "shared/blast/blast_polygons_1.inc"
};
static Sint32 blast_vertices_1[663] = {
#include "shared/blast/blast_vertices_1.inc"
};
NJS_CNK_MODEL blast_model_1 = {
#include "shared/blast/blast_model_1.inc"
};
static NJS_CNK_OBJECT blast_object_1 = {
#include "shared/blast/blast_object_1.inc"
};
/* the third shell's model, a flat ring of SHELL_2_POINTS points and outer
 * radius 10.6 */
#define SHELL_2_POINTS 32
static Sint16 blast_polygons_2[115] = {
#include "shared/blast/blast_polygons_2.inc"
};
static Sint32 blast_vertices_2[195] = {
#include "shared/blast/blast_vertices_2.inc"
};
static NJS_CNK_MODEL blast_model_2 = {
#include "shared/blast/blast_model_2.inc"
};
static NJS_CNK_OBJECT blast_object_2 = {
#include "shared/blast/blast_object_2.inc"
};
static Sint16 blast_texture_anim_uvs[578] = {
#include "shared/blast/blast_texture_anim_uvs.inc"
};
struct texture_anim blast_texture_anim = {
#include "shared/blast/blast_texture_anim.inc"
};

KEPT static NJS_CNK_MODEL *blast_models[BLAST_SHELLS] = {
#include "shared/blast/blast_models.inc"
};
/* the task that holds the textures, or null while none does. the initializer
 * places the pointer in the object's data, where each image has it, not in its
 * bss */
static struct task *blast_texture_task = NULL;

static struct task *blast_task_create(void (*step)(struct task *task))
{
    struct task *task;
    struct blast *blast;

    task = task_create(0, BLAST_LEVEL, step, "exec");
    if (task != NULL) {
        /* @hack spelling: the assignment inside the test, where a statement
         * of its own would do */
        if ((blast = syCalloc(1, sizeof(struct blast))) == NULL) {
            task_destroy(task);
            return NULL;
        }
        task->extra.p = blast;
    }
    return task;
}

static void blast_draw(struct task *task)
{
    struct blast *blast = task->extra.p;
    /* @hack spelling: scale, a local for the shell's scale, where
     * blast->scale[i] read in place would do */
    Float scale;
    int i;

    draw_control_save();
    draw_attr_save();
    draw_control_clear(DRAW_BITS_OFF);
    draw_control_set(DRAW_BITS_ON);
    njSetTexture(&blast_texlist);
    njPushMatrixEx();
    njTranslateEx(&blast->pos);
    texture_anim_apply(&blast_model_1, &blast_texture_anim, blast->timer);
    for (i = 0; i < BLAST_SHELLS; i++) {
        if (blast->scale[i] > 0.0F && blast->alpha[i] > 0.0F) {
            constant_material_set(blast->alpha[i], 1.0F, 1.0F, 1.0F);
            njPushMatrixEx();
            scale = blast->scale[i];
            if (i != RING_SHELL) {
                if (blast->kind == BLAST_LARGE_RAISED ||
                    blast->kind == BLAST_HUGE_RAISED) {
                    njTranslate(NULL, 0.0F, LARGE_LIFT, 0.0F);
                } else if (blast->kind == BLAST_MEDIUM_RAISED) {
                    njTranslate(NULL, 0.0F, MEDIUM_LIFT, 0.0F);
                } else if (blast->kind == BLAST_SMALL_RAISED ||
                           blast->kind == BLAST_TINY_RAISED) {
                    njTranslate(NULL, 0.0F, SMALL_LIFT, 0.0F);
                }
            }
            if (i == TILTED_SHELL) {
                njRotateX(NULL, SHELL_TILT);
            }
            njScale(NULL, scale, scale, scale);
            njCnkSimpleDrawModel(blast_models[i]);
            njPopMatrixEx();
        }
    }
    njPopMatrixEx();
    constant_material_set(0.0F, 0.0F, 0.0F, 0.0F);
    draw_control_restore();
    draw_attr_restore();
}

/* at the blast's first step, a size's step sets each shell's scale, growth
 * and alpha. a shell's growth then moves toward a floor each step, by up to
 * the timer times a rate. the growth adds to the shell's scale. once the timer
 * passes the shell's fade start, its alpha falls each step */
static void blast_step_small(struct task *task)
{
    struct blast *blast = task->extra.p;
    NJS_VECTOR spd;

    if (blast->timer > SMALL_STEPS) {
        blast->mode = MODE_ENDED;
        return;
    }
    /* @hack spelling: a test of timer 0 for each shell, where one test for
     * all three shells would do */
    if (blast->timer == 0) {
        blast->scale[0] = 0.5F;
        blast->growth[0] = 0.3F;
        blast->alpha[0] = 0.7F;
    }
    blast->growth[0] =
        float_approach(blast->growth[0], 0.02F, blast->timer * 0.015F);
    blast->scale[0] += blast->growth[0];
    if (blast->timer > 5) {
        blast->alpha[0] -= 0.1F;
    }
    if (blast->timer == 0) {
        blast->scale[1] = 0.0F;
        blast->growth[1] = 0.2F;
        blast->alpha[1] = 0.6F;
    }
    blast->growth[1] =
        float_approach(blast->growth[1], 0.02F, blast->timer * 0.012F);
    blast->scale[1] += blast->growth[1];
    if (blast->timer > 20) {
        blast->alpha[1] -= 0.022F;
    }
    if (blast->timer == 0) {
        blast->scale[2] = 0.4F;
        blast->growth[2] = 0.25F;
        blast->alpha[2] = 0.7F;
    }
    blast->growth[2] =
        float_approach(blast->growth[2], 0.015F, blast->timer * 0.01F);
    blast->scale[2] += blast->growth[2];
    if (blast->timer > 0) {
        blast->alpha[2] -= 0.035F;
    }
    if (blast->timer == 6) {
        spd.x = 0.0F;
        spd.y = 0.1F;
        spd.z = 0.0F;
        black_smoke_rising_burst(&blast->pos, &spd, 3.0F, 3.5F, 3);
    } else if (blast->timer == 30) {
        spd.x = 0.0F;
        spd.y = 0.15F;
        spd.z = 0.0F;
        black_smoke_rising_burst(&blast->pos, &spd, 4.0F, 5.0F, 4);
    }
}

static void blast_step_medium(struct task *task)
{
    struct blast *blast = task->extra.p;
    NJS_VECTOR spd;

    if (blast->timer > MEDIUM_STEPS) {
        blast->mode = MODE_ENDED;
        return;
    }
    /* @hack spelling: a test of timer 0 for each shell, where one test for
     * all three shells would do */
    if (blast->timer == 0) {
        blast->scale[0] = 1.0F;
        blast->growth[0] = 0.4F;
        blast->alpha[0] = 0.7F;
    }
    blast->growth[0] =
        float_approach(blast->growth[0], 0.04F, blast->timer * 0.01F);
    blast->scale[0] += blast->growth[0];
    if (blast->timer > 5) {
        blast->alpha[0] -= 0.1F;
    }
    if (blast->timer == 0) {
        blast->scale[1] = 0.65F;
        blast->growth[1] = 0.28F;
        blast->alpha[1] = 0.6F;
    }
    blast->growth[1] =
        float_approach(blast->growth[1], 0.025F, blast->timer * 0.004F);
    blast->scale[1] += blast->growth[1];
    if (blast->timer > 20) {
        blast->alpha[1] -= 0.022F;
    }
    if (blast->timer == 0) {
        blast->scale[2] = 0.5F;
        blast->growth[2] = 0.25F;
        blast->alpha[2] = 0.7F;
    }
    blast->growth[2] =
        float_approach(blast->growth[2], 0.015F, blast->timer * 0.01F);
    blast->scale[2] += blast->growth[2];
    if (blast->timer > 0) {
        blast->alpha[2] -= 0.035F;
    }
    if (blast->timer == 30) {
        spd.x = 0.0F;
        spd.y = 0.2F;
        spd.z = 0.0F;
        black_smoke_rising_burst(&blast->pos, &spd, 14.0F, 5.5F, 9);
    }
}

static void blast_step_tiny(struct task *task)
{
    struct blast *blast = task->extra.p;

    if (blast->timer > TINY_STEPS) {
        blast->mode = MODE_ENDED;
        return;
    }
    /* @hack spelling: a test of timer 0 for each shell, where one test for
     * all three shells would do */
    if (blast->timer == 0) {
        blast->scale[0] = 0.5F;
        blast->growth[0] = 0.3F;
        blast->alpha[0] = 0.7F;
    }
    blast->growth[0] =
        float_approach(blast->growth[0], 0.02F, blast->timer * 0.015F);
    blast->scale[0] += blast->growth[0];
    if (blast->timer > 5) {
        blast->alpha[0] -= 0.1F;
    }
    if (blast->timer == 0) {
        blast->scale[1] = 0.0F;
        blast->growth[1] = 0.1F;
        blast->alpha[1] = 0.5F;
    }
    blast->growth[1] =
        float_approach(blast->growth[1], 0.015F, blast->timer * 0.005F);
    blast->scale[1] += blast->growth[1];
    if (blast->timer > 20) {
        blast->alpha[1] -= 0.025F;
    }
    if (blast->timer == 0) {
        blast->scale[2] = 0.4F;
        blast->growth[2] = 0.25F;
        blast->alpha[2] = 0.7F;
    }
    blast->growth[2] =
        float_approach(blast->growth[2], 0.015F, blast->timer * 0.01F);
    blast->scale[2] += blast->growth[2];
    if (blast->timer > 0) {
        blast->alpha[2] -= 0.035F;
    }
}

static void blast_step_large(struct task *task)
{
    struct blast *blast = task->extra.p;
    NJS_VECTOR spd;

    if (blast->timer > LARGE_STEPS) {
        blast->mode = MODE_ENDED;
        return;
    }
    /* @hack spelling: a test of timer 0 for each shell, where one test for
     * all three shells would do */
    if (blast->timer == 0) {
        blast->scale[0] = 1.5F;
        blast->growth[0] = 0.5F;
        blast->alpha[0] = 0.7F;
    }
    blast->growth[0] =
        float_approach(blast->growth[0], 0.05F, blast->timer * 0.012F);
    blast->scale[0] += blast->growth[0];
    if (blast->timer > 10) {
        blast->alpha[0] -= 0.05F;
    }
    if (blast->timer == 0) {
        blast->scale[1] = 0.0F;
        blast->growth[1] = 0.5F;
        blast->alpha[1] = 0.7F;
    }
    blast->growth[1] =
        float_approach(blast->growth[1], 0.024F, blast->timer * 0.01F);
    blast->scale[1] += blast->growth[1];
    if (blast->timer > 30) {
        blast->alpha[1] -= 0.01F;
    }
    if (blast->timer == 0) {
        blast->scale[2] = 1.0F;
        blast->growth[2] = 0.4F;
        blast->alpha[2] = 0.7F;
    }
    blast->growth[2] =
        float_approach(blast->growth[2], 0.04F, blast->timer * 0.003F);
    blast->scale[2] += blast->growth[2];
    if (blast->timer > 0) {
        blast->alpha[2] -= 0.02F;
    }
    if (blast->timer == 10) {
        spd.x = 0.0F;
        spd.y = 0.15F;
        spd.z = 0.0F;
        black_smoke_rising_burst(&blast->pos, &spd, 10.0F, 8.0F, 3);
    } else if (blast->timer == 30) {
        spd.x = 0.0F;
        spd.y = 0.2F;
        spd.z = 0.0F;
        black_smoke_rising_burst(&blast->pos, &spd, 13.0F, 10.0F, 4);
    }
    /* @hack spelling: a test of its own, where else if would do */
    if (blast->timer == 50) {
        spd.x = 0.0F;
        spd.y = 0.25F;
        spd.z = 0.0F;
        black_smoke_rising_burst(&blast->pos, &spd, 14.0F, 12.0F, 5);
    }
}

static void blast_step_huge(struct task *task)
{
    struct blast *blast = task->extra.p;
    NJS_VECTOR spd;

    if (blast->timer > HUGE_STEPS) {
        blast->mode = MODE_ENDED;
        return;
    }
    /* @hack spelling: a test of timer 0 for each shell, where one test for
     * all three shells would do */
    if (blast->timer == 0) {
        blast->scale[0] = 1.5F;
        blast->growth[0] = 0.6F;
        blast->alpha[0] = 0.7F;
    }
    blast->growth[0] =
        float_approach(blast->growth[0], 0.05F, blast->timer * 0.012F);
    blast->scale[0] += blast->growth[0];
    if (blast->timer > 12) {
        blast->alpha[0] -= 0.05F;
    }
    if (blast->timer == 0) {
        blast->scale[1] = 0.0F;
        blast->growth[1] = 0.6F;
        blast->alpha[1] = 0.7F;
    }
    blast->growth[1] =
        float_approach(blast->growth[1], 0.04F, blast->timer * 0.0025F);
    blast->scale[1] += blast->growth[1];
    if (blast->timer > 30) {
        blast->alpha[1] -= 0.007F;
    }
    if (blast->timer == 0) {
        blast->scale[2] = 1.0F;
        blast->growth[2] = 0.5F;
        blast->alpha[2] = 0.7F;
    }
    blast->growth[2] =
        float_approach(blast->growth[2], 0.05F, blast->timer * 0.003F);
    blast->scale[2] += blast->growth[2];
    if (blast->timer > 0) {
        blast->alpha[2] -= 0.014F;
    }
    if (blast->timer == 10) {
        spd.x = 0.0F;
        spd.y = 0.15F;
        spd.z = 0.0F;
        black_smoke_rising_burst(&blast->pos, &spd, 11.0F, 10.0F, 3);
    } else if (blast->timer == 30) {
        spd.x = 0.0F;
        spd.y = 0.2F;
        spd.z = 0.0F;
        black_smoke_rising_burst(&blast->pos, &spd, 15.0F, 12.0F, 4);
    }
    /* @hack spelling: a test of its own, where else if would do */
    if (blast->timer == 50) {
        spd.x = 0.0F;
        spd.y = 0.25F;
        spd.z = 0.0F;
        black_smoke_rising_burst(&blast->pos, &spd, 18.0F, 14.0F, 5);
    }
    /* @hack spelling: a test of its own, where else if would do */
    if (blast->timer == 70) {
        spd.x = 0.0F;
        spd.y = 0.3F;
        spd.z = 0.0F;
        black_smoke_rising_burst(&blast->pos, &spd, 20.0F, 15.0F, 7);
    }
}

static void blast_destroy(struct task *task)
{
}

static void blast_step(struct task *task)
{
    struct blast *blast = task->extra.p;

    if (game_state.time_stopped != 0) {
        return;
    }
    switch (blast->mode) {
    case MODE_GROWING:
        /* @hack spelling: the cases out of blast_kind's order, where its
         * order would do */
        /* @hack spelling: a case of its own for each kind, where one case for
         * both kinds of a size would do */
        switch (blast->kind) {
        case BLAST_LARGE:
            blast_step_large(task);
            break;
        case BLAST_LARGE_RAISED:
            blast_step_large(task);
        /* @hack redundant: a label nothing jumps to */
        large_raised_stepped: /* NOLINT(clang-diagnostic-unused-label) */
            break;
        case BLAST_HUGE_RAISED:
            blast_step_huge(task);
            break;
        case BLAST_HUGE:
            blast_step_huge(task);
        /* @hack redundant: a label nothing jumps to */
        huge_stepped: /* NOLINT(clang-diagnostic-unused-label) */
            break;
        case BLAST_SMALL:
            blast_step_small(task);
            break;
        case BLAST_SMALL_RAISED:
            blast_step_small(task);
        /* @hack redundant: a label nothing jumps to */
        small_raised_stepped: /* NOLINT(clang-diagnostic-unused-label) */
            break;
        case BLAST_MEDIUM:
            blast_step_medium(task);
            break;
        case BLAST_MEDIUM_RAISED:
            blast_step_medium(task);
        /* @hack redundant: a label nothing jumps to */
        medium_raised_stepped: /* NOLINT(clang-diagnostic-unused-label) */
            break;
        case BLAST_TINY:
            blast_step_tiny(task);
            break;
        /* @hack spelling: the last case without a break, where a break would
         * do */
        case BLAST_TINY_RAISED:
            blast_step_tiny(task);
        }
        break;
    /* @hack redundant: the case of an ended blast beside default, which takes
     * it anyway */
    case MODE_ENDED:
    default:
        task_kill(task);
        break;
    }
    blast->timer++;
}

/* each size's step sets the first shell again at the blast's first step. only
 * a kind outside blast_kind keeps what this sets */
static void blast_init(struct task *task)
{
    struct blast *blast = task->extra.p;

    /* @hack spelling: alpha set before scale and growth, not in the members'
     * order */
    blast->alpha[0] = 1.0F;
    blast->scale[0] = 0.0F;
    blast->growth[0] = 0.34F;
    /* @hack spelling: the destructor set before the draw, not in the
     * members' order */
    task->destructor = blast_destroy;
    task->draw = blast_draw;
}

static void blast_texture_destroy(struct task *task)
{
    njReleaseTexture(&blast_texlist);
    blast_texture_task = NULL;
}

static void blast_texture_step(struct task *task)
{
}

void blast_load_texture(void)
{
    if (blast_texture_task == NULL) {
        texture_load_pvm("E_EXPTEX", &blast_texlist);
        blast_texture_task =
            task_create(0, TEXTURE_LEVEL, blast_texture_step, "ManTex");
        blast_texture_task->destructor = blast_texture_destroy;
    }
}

void blast_create(enum blast_kind kind, NJS_POINT3 *pos)
{
    struct task *task;
    struct blast *blast;

    task = blast_task_create(blast_step);
    if (task != NULL) {
        blast = task->extra.p;
        blast->kind = kind;
        blast->pos = *pos;
        blast_init(task);
        blast_load_texture();
    }
}
