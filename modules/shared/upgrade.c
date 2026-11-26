/* the upgrade stand, LEVUPDAI in a stage's object list. a player who touches
 * it takes the upgrade it holds */
#include <ninja.h>
#include <sg_xpt.h>
#include <shinobi.h>
#include <stdlib.h>

#include "camera/camera.h"
#include "camera/modes.h"
#include "camera/stack.h"
#include "collision/collision.h"
#include "draw/chunk.h"
#include "draw/color.h"
#include "draw/lines.h"
#include "draw/texture_anim.h"
#include "effect/particle.h"
#include "hud/hud.h"
#include "hud/message_window.h"
#include "object/set.h"
#include "player/player.h"
#include "shared/dust.h"
#include "shared/messenger.h"
#include "shared/upgrade.h"
#include "shared/upgrade_models.h"
#include "sound/adx.h"
#include "sound/sound.h"
#include "stage/stage.h"
#include "system/angle.h"
#include "system/flags.h"
#include "system/game_state.h"
#include "system/pad.h"
#include "system/task.h"
#include "system/util.h"

/* the stand's modes, which its work's mode holds */
enum upgrade_mode {
    MODE_HOLDING, /* the upgrade waits on the stand for a player's touch */
    /* a player takes the upgrade. the take lasts until the upgrade's window
     * closes, or for one step where no window opens */
    MODE_TAKING,
    MODE_TAKEN /* the upgrade is gone, taken here or before the stand started */
};
/* the stand's upgrade, as an index into upgrade_kinds */
#define KIND_INDEX(work) ((work)->submode)
/* TRUE where the take gave player 0 PLAYER_INVINCIBLE, which player 0 lacked
 * until then. the take's end then clears PLAYER_INVINCIBLE and
 * MADE_INVINCIBLE */
#define MADE_INVINCIBLE(work) ((work)->tick)

/* the count of upgrade_kinds, one for each upgrade a stand can hold */
#define UPGRADE_KINDS 28
#define UPGRADE_SHAPES 1

/* an upgrade a stand can hold */
struct upgrade_kind {
    Sint32 character; /* the PLAYER_ character it is for */
    const char *name; /* its name, as [SONIC]SHOES */
    Uint32 bit; /* its bit in a player's upgrades */
    Sint32 number; /* its enum upgrade_number, which numbers its game flag */
    Sint32 unk_10;
    /* the upgrade's own draw and step, which the stand calls while it holds
     * the upgrade. draw starts at the stand's place and turn, and the matrix
     * it leaves is dropped. either may be null */
    void (*draw)(struct task *task);
    void (*step)(struct task *task);
    NJS_CNK_OBJECT *object; /* the upgrade's model, or null */
    Sint32 unk_20;
};

/* a stand's record, which its task's motion points to */
struct upgrade_state {
    Uint8 unk_00[8];
    struct upgrade_kind *kind;
    /* the texture list of kind's character, from player 0 or else player 1.
     * it is null while neither player is that character. each step then looks
     * again */
    NJS_TEXLIST *texlist;
    /* the upgrade's message window, or null. the window sets it to null as it
     * closes */
    struct task *message;
};

#define STATE(task) ((struct upgrade_state *)(task)->motion.p)

/* upgrade_melody_texlist's textures: the melody quad's frames eff_melody00 to
 * eff_melody03, then the particles eff_melody04 and eff_melody05. the notes
 * eff_onpu0 and eff_onpu1 follow */
#define MELODY_FRAMES 4
#define NOTES 2
#define MELODY_TEXTURES 8
/* upgrade_sparkle_texlist's textures: the frames of the sparkle's middle quad,
 * thhikari01 to thhikari08, then the grains thtsubu01 to thtsubu08 */
#define SPARKLE_FRAMES 8
#define FIRST_GRAIN 8
#define GRAIN_FRAMES 8
#define SPARKLE_TEXTURES 16

/* a quad shows each frame for FRAME_STEPS steps */
#define FRAME_STEPS 3
/* a quad turns QUAD_TURN Angle units a step on the screen. an upgrade's
 * model turns MODEL_TURN Angle units a step about y */
#define QUAD_TURN 128
#define MODEL_TURN 384

/* a particle's quad has its second edge a quarter turn plus up to EDGE_TURN
 * degrees from its first. a particle spins up to SPIN degrees a step either
 * way */
#define EDGE_TURN 20.0F
#define SPIN 1.5F
/* a particle past its last frame stays on it. its alpha then falls
 * FADE_STEP a step, out of 255. it ends there once its alpha is below
 * FADE_END */
#define FADE_STEP 2
#define FADE_END 10

/* the melody's quad bobs MELODY_BOB above and below melody_height over the
 * stand, at MELODY_BOB_RATE Angle units a step. a particle of
 * melody_04_group starts where the quad is, moved one step of its speed. a
 * particle of melody_05_group, or a note, starts on a bob of its own. that
 * bob runs WIDE_BOB above and below melody_height, at WIDE_BOB_RATE Angle
 * units a step */
#define MELODY_BOB 1.5F
#define MELODY_BOB_RATE 256
#define WIDE_BOB 3.0F
#define WIDE_BOB_RATE 128

/* a melody particle's upward speed is its kind's rise plus a random part.
 * that part spans melody_spread times the kind's share. it is centered on 0,
 * but melody_04_group's starts MELODY_04_DIP of its span below 0 */
#define MELODY_05_RISE 0.01F
#define NOTE_RISE 0.015F
#define MELODY_04_SHARE 0.5F
#define MELODY_05_SHARE 0.1F
#define NOTE_SHARE 0.5F
#define MELODY_04_DIP 0.2F
/* a note takes speeds in x and z within NOTE_SPREAD / 2 of 0, at random */
#define NOTE_SPREAD 0.35F
/* a note's texture is a random whole number below NOTE_PICK, modulo NOTES */
#define NOTE_PICK 100.0F
/* a note's alpha, as a share of 1, lies from NOTE_ALPHA_LOW up to
 * NOTE_ALPHA_LOW + NOTE_ALPHA_SPAN, at random */
#define NOTE_ALPHA_SPAN 0.3F
#define NOTE_ALPHA_LOW 0.7F
/* a color's channels each run from 0 to CHANNEL_MAX, CHANNEL_BITS wide.
 * COLOR_UNIT turns a channel into a share of 1 */
#define CHANNEL_MAX 255.0F
#define CHANNEL_BITS 8
#define COLOR_UNIT (1.0F / 255.0F)

/* the sparkle's middle quad bobs SPARKLE_BOB above and below SPARKLE_HEIGHT
 * over the stand. it bobs at SPARKLE_BOB_RATE Angle units a step */
#define SPARKLE_HEIGHT 20.0F
#define SPARKLE_BOB 1.0F
#define SPARKLE_BOB_RATE 224
/* the most quads that can orbit the sparkle */
#define MOST_ORBITS 20
/* quad i negates its angle about x where i has ORBIT_MIRROR_X set. it
 * negates its angle about y where i has ORBIT_MIRROR_Y set */
#define ORBIT_MIRROR_X 0x1
#define ORBIT_MIRROR_Y 0x2
/* an orbiting quad's color pulses between ORBIT_DIM and ORBIT_BRIGHT, at
 * ORBIT_PULSE_RATE Angle units a step. quad i's pulse leads quad 0's by
 * i * ORBIT_PULSE_PHASE */
#define ORBIT_DIM 0x50ffffff
#define ORBIT_BRIGHT 0xffffffff
#define ORBIT_PULSE_RATE 1616
#define ORBIT_PULSE_PHASE (FULL_TURN / 20)

#define SOUND_TAKE 0x1015
#define SOUND_TAKE_DURATION 30
#define SOUND_TAKE_PRIORITY 1
/* message_window_open's time that holds each page until player_pads[slot]
 * presses A */
#define UNTIL_A(slot) (-1 - (slot))
/* the take's puff of dust */
#define PUFF_HEIGHT 10.0F
#define PUFF_RISE 0.5F
#define PUFF_SCALE 10.0F
/* the animation the take has player 0 play */
#define TAKE_ANIM 86
/* during the take, camera 0 circles player 0 from scl.y + CAMERA_MARGIN
 * away. it circles at CAMERA_TURN Angle units a step. it looks down
 * CAMERA_TILT Angle units */
#define CAMERA_MARGIN 20.0F
#define CAMERA_TILT 0x800
#define CAMERA_TURN 0x80
/* a taken stand's timer counts the steps since the take ended, up to
 * TAKEN_STEPS */
#define TAKEN_STEPS 1000

/* the column's scale in x and in z, where a draw scales it. COLUMN_X is
 * negative, so each njScale by it mirrors the column in x */
#define COLUMN_X (-0.7F)
#define COLUMN_Z 0.72F
/* a draw that scales the column scales it in y by a pulse. the pulse runs
 * COLUMN_PULSE above and below COLUMN_HEIGHT, at COLUMN_PULSE_RATE Angle
 * units a step. the second draw takes column_scale of that pulse */
#define COLUMN_HEIGHT 1.7F
#define COLUMN_PULSE 0.2F
#define COLUMN_PULSE_RATE 384
/* once the take ends, each draw's scale in y falls with the timer, to 0 at
 * COLUMN_SHRINK_STEPS. past that step it turns negative, which mirrors the
 * column in y. the column stops drawing at COLUMN_STEPS */
#define COLUMN_SHRINK_STEPS 30
#define COLUMN_STEPS 35
/* the column's second draw turns COLUMN_TURN Angle units a step */
#define COLUMN_TURN 128
/* during the take, the column's glow pulses between glow_from and glow_to at
 * GLOW_RATE Angle units a step */
#define GLOW_RATE 768
/* under the glow, each strip chunk keeps its type, and takes GLOW_ATTR for
 * its only flag */
#define GLOW_ATTR NJD_FST_UA

static int upgrade_particle_step(struct particle_group *group,
                                 struct particle *particle);
static void upgrade_melody_draw(struct task *task);
static void upgrade_melody_step(struct task *task);
static void upgrade_sparkle_draw(struct task *task);
static void upgrade_sparkle_step(struct task *task);
static void upgrade_rubberunit_draw(struct task *task);
static void upgrade_rubberunit_step(struct task *task);
static void upgrade_shoes_draw(struct task *task);
static void upgrade_shoes_step(struct task *task);
static void upgrade_model_draw(struct task *task);
static void upgrade_model_step(struct task *task);
static void upgrade_destroy(struct task *task);
static void upgrade_step(struct task *task);
static void upgrade_draw(struct task *task);

/* the SET editor's labels, which no code reads. each size rounds its label
 * up to whole words */
static const char editor_angle_x[12] = "<- ANGLE X";
static const char editor_angle_y[12] = "<- ANGLE Y";
static const char editor_angle_z[12] = "<- ANGLE Z";
static const char editor_kind[8] = "<- TYPE";
static const char editor_camera_distance[16] = "<- CAMERA DIST";

/* the jingle that replaces the music during the take */
static const char take_jingle[16] = "ITEM_GET.ADX";

/* the stand's three models, its base, floor and light column, each with
 * its own textures */
static const char stand_base_filename[16] = {
#include "shared/upgrade/stand_base_filename.inc"
};

static const char stand_floor_filename[16] = {
#include "shared/upgrade/stand_floor_filename.inc"
};

static const char stand_column_filename[13] = {
#include "shared/upgrade/stand_column_filename.inc"
};

static NJS_TEXNAME stand_base_texname[1] = {
#include "shared/upgrade/stand_base_texname.inc"
};

static NJS_TEXLIST stand_base_texlist = {
#include "shared/upgrade/stand_base_texlist.inc"
};

static Sint16 stand_base_polygons[663] = {
#include "shared/upgrade/stand_base_polygons.inc"
};

static Sint32 stand_base_vertices[483] = {
#include "shared/upgrade/stand_base_vertices.inc"
};

static NJS_CNK_MODEL stand_base_model = {
#include "shared/upgrade/stand_base_model.inc"
};

static NJS_CNK_OBJECT stand_base_object = {
#include "shared/upgrade/stand_base_object.inc"
};

static NJS_TEXNAME stand_floor_texname[1] = {
#include "shared/upgrade/stand_floor_texname.inc"
};

static NJS_TEXLIST stand_floor_texlist = {
#include "shared/upgrade/stand_floor_texlist.inc"
};

static Sint16 stand_floor_polygons[25] = {
#include "shared/upgrade/stand_floor_polygons.inc"
};

static Sint32 stand_floor_vertices[27] = {
#include "shared/upgrade/stand_floor_vertices.inc"
};

static NJS_CNK_MODEL stand_floor_model = {
#include "shared/upgrade/stand_floor_model.inc"
};

static NJS_CNK_OBJECT stand_floor_object = {
#include "shared/upgrade/stand_floor_object.inc"
};

static NJS_TEXNAME stand_column_texname[1] = {
#include "shared/upgrade/stand_column_texname.inc"
};

static NJS_TEXLIST stand_column_texlist = {
#include "shared/upgrade/stand_column_texlist.inc"
};

static Sint16 stand_column_polygons[109] = {
#include "shared/upgrade/stand_column_polygons.inc"
};

static Sint32 stand_column_vertices[183] = {
#include "shared/upgrade/stand_column_vertices.inc"
};
/* @hack type: stand_column_model an array of one, where a single model would
 * do */
static NJS_CNK_MODEL stand_column_model[1] = {
#include "shared/upgrade/stand_column_model.inc"
};

static NJS_CNK_OBJECT stand_column_object = {
#include "shared/upgrade/stand_column_object.inc"
};

/* the column's scroll, one entry of one key. the entry's strip has its
 * length at short 11 of stand_column_polygons. the key gives the strip's 32
 * vertices a u and a v each */
static Sint16 stand_column_uvs[68] = {
#include "shared/upgrade/stand_column_uvs.inc"
};

static struct texture_anim stand_column_anim = {
#include "shared/upgrade/stand_column_anim.inc"
};

/* the data down to the next #pragma section lies in section UPGRADE. the
 * image places that section after the upgrades' models */
#pragma section UPGRADE
static const char effect_filenames[312] = {
#include "shared/upgrade/effect_filenames.inc"
};

static const char equipment_names[516] = {
#include "shared/upgrade/equipment_names.inc"
};

static NJS_TEXNAME upgrade_melody_texnames[MELODY_TEXTURES] = {
#include "shared/upgrade/upgrade_melody_texnames.inc"
};
NJS_TEXLIST upgrade_melody_texlist = {
#include "shared/upgrade/upgrade_melody_texlist.inc"
};

static NJS_TEXNAME upgrade_sparkle_texnames[SPARKLE_TEXTURES] = {
#include "shared/upgrade/upgrade_sparkle_texnames.inc"
};
NJS_TEXLIST upgrade_sparkle_texlist = {
#include "shared/upgrade/upgrade_sparkle_texlist.inc"
};

NJS_TEXLIST *upgrade_texlists[4] = {
#include "shared/upgrade/upgrade_texlists.inc"
};

/* the stand's shape, a cylinder that stands 30 high on its pos */
/* clang-format off */
static struct collision_shape upgrade_shapes[UPGRADE_SHAPES] = {
    {0, SHAPE_CYLINDER, PUSH_FIRM(PUSH_ALL_BUT_PICKUPS), 0,
     SHAPE_NO_OTHER_HITS | SHAPE_UNK_2000 | SHAPE_IGNORE_ENEMIES |
         SHAPE_IGNORE_PICKUPS | SHAPE_IGNORE_UNK_9,
     {0.0F, 15.0F, 0.0F}, 15.0F, 15.0F, 0.0F, 0, {0, 0, 0}},
};
/* clang-format on */

static struct particle_group melody_04_group = {
#include "shared/upgrade/melody_04_group.inc"
};

static struct particle_group grain_group = {
#include "shared/upgrade/grain_group.inc"
};

static struct particle_group melody_05_group = {
#include "shared/upgrade/melody_05_group.inc"
};
/* one group for each note's texture */
static struct particle_group note_groups[NOTES] = {
#include "shared/upgrade/note_groups.inc"
};

static Float melody_half_size = 3.5F;
static Uint32 melody_color = 0xffffffff;

static Float melody_height = 20.0F;
/* melody_04_group's and melody_05_group's particles take random speeds in x
 * and z, within melody_spread / 2 of 0 */
static Float melody_spread = 0.2F;
/* the scales the melody's particles start at, by kind */
static Float melody_04_scale = 0.5F;
static Float melody_05_scale = 0.7F;
static Float note_scale = 2.5F;
/* the rise of melody_04_group's particles */
static Float melody_04_rise = 0.025F;
/* the melody's periods, in steps. the melody makes a particle of each kind
 * where game_state.steps is a multiple of the kind's period */
static Uint32 melody_04_period = 15;
static Uint32 melody_05_period = 40;
static Uint32 note_period = 50;

/* sparkle_half_size and sparkle_color are the middle quad's */
static Float sparkle_half_size = 3.5F;
static Float orbit_half_size = 1.0F;
static Uint32 sparkle_color = 0xffffffff;
/* orbit_count quads, at most MOST_ORBITS, orbit the point SPARKLE_HEIGHT
 * over the stand at orbit_radius. quad i's angle about x starts at
 * i * orbit_phase_x. it turns orbit_rate_x + i * orbit_rate_x_step Angle
 * units a step. its angle about y does the same with orbit_phase_y,
 * orbit_rate_y and orbit_rate_y_step */
static Float orbit_radius = 6.0F;
static Sint32 orbit_count = 10;
static Angle orbit_rate_x = 200;
static Angle orbit_rate_y = 300;
static Angle orbit_rate_x_step = 0;
static Angle orbit_rate_y_step = 0;
static Angle orbit_phase_x = FULL_TURN / 10;
static Angle orbit_phase_y = FULL_TURN / 10;

/* six values that nothing reads */
static Float dat_upgrade_0 = 0.14F;
static Float dat_upgrade_1 = 0.1F;
static Float dat_upgrade_2 = 1.5F;
static Float dat_upgrade_3 = 0.03F;
static Sint32 dat_upgrade_4 = 2;
static Sint32 dat_upgrade_5 = 30;

/* the stands' upgrades, in the order a stand's scl.x picks them. an
 * upgrade's window shows the text messenger_upgrade_texts holds at the same
 * index */
static struct upgrade_kind upgrade_kinds[UPGRADE_KINDS] = {
#include "shared/upgrade/upgrade_kinds.inc"
};

/* the take sound's volume */
static Sint32 take_volume = -10;
/* the column's second draw scales its height by column_scale */
static Float column_scale = 0.61F;

/* the colors the column's glow pulses between */
static Uint32 glow_from = 0xffffffe0;
static Uint32 glow_to = 0xffa0e0ff;

#pragma section

/* make a grain at pos, of size scale, moving at spd. it returns the grain,
 * or null where the heap can't give one. nothing calls it, and no header
 * declares it. it isn't static, since SHC leaves out a static function that
 * nothing calls */
/* NOLINTBEGIN(misc-use-internal-linkage) */
struct particle *upgrade_grain_create(NJS_POINT3 *pos, NJS_VECTOR *spd,
                                      Float scale)
{
    struct particle *grain;

    grain = particle_create(&grain_group);
    if (grain != NULL) {
        grain->pos = *pos;
        grain->spd = *spd;
        grain->scale = scale;
        grain->ang = (Angle)RANDOM_TURN();
        grain->edge_angle =
            (Angle)((Float)rand() *
                    (EDGE_TURN * ANGLE_PER_DEGREE * RAND_SCALE)) +
            QUARTER_TURN;
        grain->ang_spd = RANDOM_ANGLE(SPIN);
    }
    return grain;
}
/* NOLINTEND(misc-use-internal-linkage) */

/* make a particle of melody_04_group as upgrade_grain_create makes a grain */
static struct particle *upgrade_melody_04_create(NJS_POINT3 *pos,
                                                 NJS_VECTOR *spd, Float scale)
{
    struct particle *particle;

    particle = particle_create(&melody_04_group);
    if (particle != NULL) {
        particle->pos = *pos;
        particle->spd = *spd;
        particle->scale = scale;
        particle->ang = (Angle)RANDOM_TURN();
        particle->edge_angle =
            (Angle)((Float)rand() *
                    (EDGE_TURN * ANGLE_PER_DEGREE * RAND_SCALE)) +
            QUARTER_TURN;
        particle->ang_spd = RANDOM_ANGLE(SPIN);
    }
    return particle;
}

/* make a particle of melody_05_group as upgrade_grain_create makes a grain */
static struct particle *upgrade_melody_05_create(NJS_POINT3 *pos,
                                                 NJS_VECTOR *spd, Float scale)
{
    struct particle *particle;

    particle = particle_create(&melody_05_group);
    if (particle != NULL) {
        particle->pos = *pos;
        particle->spd = *spd;
        particle->scale = scale;
        particle->ang = (Angle)RANDOM_TURN();
        particle->edge_angle =
            (Angle)((Float)rand() *
                    (EDGE_TURN * ANGLE_PER_DEGREE * RAND_SCALE)) +
            QUARTER_TURN;
        particle->ang_spd = RANDOM_ANGLE(SPIN);
    }
    return particle;
}

/* make a note of note_groups[note] as upgrade_grain_create makes a grain,
 * but with no spin */
static struct particle *upgrade_note_create(NJS_POINT3 *pos, NJS_VECTOR *spd,
                                            Float scale, int note)
{
    struct particle *particle;

    particle = particle_create(&note_groups[note]);
    if (particle != NULL) {
        particle->pos = *pos;
        particle->spd = *spd;
        particle->scale = scale;
        particle->ang = (Angle)RANDOM_TURN();
        particle->edge_angle =
            (Angle)((Float)rand() *
                    (EDGE_TURN * ANGLE_PER_DEGREE * RAND_SCALE)) +
            QUARTER_TURN;
        particle->ang_spd = 0;
    }
    return particle;
}

/* particle_move, but a particle that would pass its last frame stays on it
 * and fades. a fading particle ends once its alpha is below FADE_END, or its
 * scale below 0. every group here steps by it */
static int upgrade_particle_step(struct particle_group *group,
                                 struct particle *particle)
{
    Float frame;
    Uint32 alpha;

    particle->ang += particle->ang_spd;
    particle->scale += group->scale_step;
    particle->pos.x += particle->spd.x;
    particle->pos.y += particle->spd.y;
    particle->pos.z += particle->spd.z;
    particle->spd.x *= group->friction;
    particle->spd.y = (particle->spd.y * group->friction) + group->gravity;
    particle->spd.z *= group->friction;
    frame = particle->frame + group->frame_step;
    if ((Uint32)(Sint16)frame >= group->frames) {
        alpha = (Uint8)(particle->color >> ALPHA_SHIFT);
        if (alpha < FADE_END) {
            return 0;
        }
        if (particle->scale < 0.0F) {
            return 0;
        }
        alpha -= FADE_STEP;
        particle->color =
            (particle->color & ~COLOR_ALPHA) | (alpha << ALPHA_SHIFT);
    } else {
        particle->frame = frame;
    }
    /* @bug uninitialized: returns no value where the particle lives on.
     * particle_step_all frees a particle whose step returns 0, by the
     * TST R0,R0 at 0x8c036e84. the RTS at 0x332 of the object's code returns
     * frame's offset in the particle, which MOV #24,R0 at 0x2f0 set. a fading
     * particle returns scale's offset instead, which MOV #4,R0 at 0x310 set.
     * either is nonzero, so the particle lives */
#ifdef NONMATCHING
    /* this side keeps the tag's result: it returns 1, nonzero as either offset
     * is */
    return 1;
#endif
} /* NOLINT(clang-diagnostic-return-type) */

/* draw the melody: a quad of eff_melody00 to eff_melody03 in turn, bobbing
 * over the stand. it draws nothing until upgrade_melody_texlist loads */
static void upgrade_melody_draw(struct task *task)
{
    NJS_POINT3 pos;

    if (upgrade_melody_texlist.textures->texaddr == 0) {
        return;
    }
    pos.x = 0.0F;
    pos.y = melody_height +
            (njSin(game_state.steps * MELODY_BOB_RATE) * MELODY_BOB);
    pos.z = 0.0F;
    njColorBlendingMode(NJD_SOURCE_COLOR, NJD_COLOR_BLENDING_SRCALPHA);
    njColorBlendingMode(NJD_DESTINATION_COLOR, NJD_COLOR_BLENDING_ONE);
    njFogDisable();
    njSetTexture(&upgrade_melody_texlist);
    particle_draw_quad(&pos, (game_state.steps / FRAME_STEPS) % MELODY_FRAMES,
                       QUARTER_TURN, game_state.steps * QUAD_TURN,
                       melody_half_size, melody_half_size, melody_color,
                       PARTICLE_BLEND_ADD, 0.0F, 0.0F);
    njFogEnable();
    njColorBlendingMode(NJD_SOURCE_COLOR, NJD_COLOR_BLENDING_SRCALPHA);
    njColorBlendingMode(NJD_DESTINATION_COLOR, NJD_COLOR_BLENDING_INVSRCALPHA);
}

/* a random color for a note. its red, green and blue each lie from 0 to
 * 254. its alpha lies from 178 to 254 */
static Uint32 upgrade_note_color(void)
{
    Float red;
    Float green;
    Float blue;
    Float alpha;
    Uint32 color;

    red = (Float)rand() * RAND_SCALE;
    green = (Float)rand() * RAND_SCALE;
    blue = (Float)rand() * RAND_SCALE;
    alpha = ((Float)rand() * (NOTE_ALPHA_SPAN * RAND_SCALE)) + NOTE_ALPHA_LOW;
    /* @hack type: the channels converted to Sint32, where Uint32, color's
     * type, would do */
    color = (Sint32)(alpha * CHANNEL_MAX);
    color = (color << CHANNEL_BITS) | (Sint32)(red * CHANNEL_MAX);
    color = (color << CHANNEL_BITS) | (Sint32)(green * CHANNEL_MAX);
    color = (color << CHANNEL_BITS) | (Sint32)(blue * CHANNEL_MAX);
    return color;
}

/* @hack spelling: an inline function that walks a vector as Floats, where
 * its members written out would do */
#pragma inline(point_add)

static void point_add(Float *vec, Float *point)
{
    *point++ += *vec++;
    *point++ += *vec++;
    *point += *vec;
}

static void upgrade_melody_step(struct task *task)
{
    struct task_work *work = task->work;
    int note;
    NJS_POINT3 pos;
    NJS_VECTOR spd;
    struct particle *particle;

    if (upgrade_melody_texlist.textures->texaddr == 0) {
        return;
    }
    if (game_state.steps % melody_04_period == 0) {
        pos.x = work->pos.x;
        pos.y = work->pos.y + melody_height +
                (njSin(game_state.steps * MELODY_BOB_RATE) * MELODY_BOB);
        pos.z = work->pos.z;
        spd.x = (((Float)rand() * RAND_SCALE) - 0.5F) * melody_spread;
        spd.y = ((((Float)rand() * RAND_SCALE) - MELODY_04_DIP) *
                 melody_spread * MELODY_04_SHARE) +
                melody_04_rise;
        spd.z = (((Float)rand() * RAND_SCALE) - 0.5F) * melody_spread;
        point_add(&spd.x, &pos.x);
        upgrade_melody_04_create(&pos, &spd, melody_04_scale);
    }
    if (game_state.steps % melody_05_period == 0) {
        pos.x = work->pos.x;
        pos.y = work->pos.y + melody_height +
                (njSin(game_state.steps * WIDE_BOB_RATE) * WIDE_BOB);
        pos.z = work->pos.z;
        spd.x = (((Float)rand() * RAND_SCALE) - 0.5F) * melody_spread;
        spd.y = ((((Float)rand() * RAND_SCALE) - 0.5F) * melody_spread *
                 MELODY_05_SHARE) +
                MELODY_05_RISE;
        spd.z = (((Float)rand() * RAND_SCALE) - 0.5F) * melody_spread;
        upgrade_melody_05_create(&pos, &spd, melody_05_scale);
    }
    if (game_state.steps % note_period == 0) {
        note = (Sint32)((Float)rand() * (NOTE_PICK * RAND_SCALE)) % NOTES;
        pos.x = work->pos.x;
        pos.y = work->pos.y + melody_height +
                (njSin(game_state.steps * WIDE_BOB_RATE) * WIDE_BOB);
        pos.z = work->pos.z;
        spd.x = (((Float)rand() * RAND_SCALE) - 0.5F) * NOTE_SPREAD;
        spd.y = ((((Float)rand() * RAND_SCALE) - 0.5F) * melody_spread *
                 NOTE_SHARE) +
                NOTE_RISE;
        spd.z = (((Float)rand() * RAND_SCALE) - 0.5F) * NOTE_SPREAD;
        particle = upgrade_note_create(&pos, &spd, note_scale, note);
        if (particle != NULL) {
            particle->color = upgrade_note_color();
        }
    }
}

/* draw the sparkle: a quad of thhikari01 to thhikari08 in turn, bobbing over
 * the stand. orbit_count quads of the grains' textures orbit it. it draws
 * nothing until upgrade_sparkle_texlist loads */
static void upgrade_sparkle_draw(struct task *task)
{
    NJS_POINT3 center;
    NJS_POINT3 offset;
    NJS_POINT3 points[MOST_ORBITS];
    Uint32 colors[MOST_ORBITS];
    int i;
    Angle ang_x;
    Angle ang_y;
    Angle phase;
    Float half;
    Float mix;

    if (upgrade_sparkle_texlist.textures->texaddr == 0) {
        return;
    }
    center.x = 0.0F;
    /* @hack redundant: the bob's height of 1, where the sine added alone
     * would do */
    center.y = (njSin(game_state.steps * SPARKLE_BOB_RATE) * SPARKLE_BOB) +
               SPARKLE_HEIGHT;
    center.z = 0.0F;
    njColorBlendingMode(NJD_SOURCE_COLOR, NJD_COLOR_BLENDING_SRCALPHA);
    njColorBlendingMode(NJD_DESTINATION_COLOR, NJD_COLOR_BLENDING_ONE);
    njFogDisable();
    njSetTexture(&upgrade_sparkle_texlist);
    particle_draw_quad(
        &center, (game_state.steps / FRAME_STEPS) % SPARKLE_FRAMES,
        QUARTER_TURN, game_state.steps * QUAD_TURN, sparkle_half_size,
        sparkle_half_size, sparkle_color, PARTICLE_BLEND_ADD, 0.0F, 0.0F);
    offset.x = 0.0F;
    offset.y = 0.0F;
    offset.z = orbit_radius;
    njPushMatrixEx();
    /* @hack spelling: half, a local for the 0.5 the pulse scales by, set in
     * the loop's start after i, where the literal in place would do */
    for (i = 0, half = 0.5F; i < orbit_count; i++) {
        /* @hack spelling: phase, a local that holds i * ORBIT_PULSE_PHASE,
         * where the product in place would do */
        phase = i * ORBIT_PULSE_PHASE;
        mix = (njSin(phase + (game_state.steps * ORBIT_PULSE_RATE)) + 1.0F) *
              half;
        colors[i] = color_blend(ORBIT_DIM, ORBIT_BRIGHT, mix);
        ang_x = (game_state.steps * (orbit_rate_x + (i * orbit_rate_x_step))) +
                (i * orbit_phase_x);
        ang_y = (game_state.steps * (orbit_rate_y + (i * orbit_rate_y_step))) +
                (i * orbit_phase_y);
        if (i & ORBIT_MIRROR_X) {
            ang_x = -ang_x;
        }
        if (i & ORBIT_MIRROR_Y) {
            ang_y = -ang_y;
        }
        njUnitMatrix(NULL);
        njRotateX(NULL, ang_x);
        njRotateY(NULL, ang_y);
        njCalcPoint(NULL, &offset, &points[i]);
        points[i].y += SPARKLE_HEIGHT;
    }
    njPopMatrixEx();
    for (i = 0; i < orbit_count; i++) {
        particle_draw_quad(
            &points[i],
            (((game_state.steps / FRAME_STEPS) + i) % GRAIN_FRAMES) +
                FIRST_GRAIN,
            QUARTER_TURN, game_state.steps * QUAD_TURN, orbit_half_size,
            orbit_half_size, colors[i], PARTICLE_BLEND_ADD, 0.0F, 0.0F);
    }
    njFogEnable();
    njColorBlendingMode(NJD_SOURCE_COLOR, NJD_COLOR_BLENDING_SRCALPHA);
    njColorBlendingMode(NJD_DESTINATION_COLOR, NJD_COLOR_BLENDING_INVSRCALPHA);
}

/* the sparkle's step, which does nothing */
static void upgrade_sparkle_step(struct task *task)
{
    /* @hack redundant: 24 bytes of locals that nothing reads */
    Uint8 unused[24];

    /* @hack redundant: a test of the texture, whose two ways do nothing */
    if (upgrade_sparkle_texlist.textures->texaddr == 0) {
        return;
    }
}

/* the draws and steps of the upgrades with a model. the three draws below
 * are the same code, and so are the three steps */
static void upgrade_rubberunit_draw(struct task *task)
{
    NJS_TEXLIST *texlist;

    texlist = STATE(task)->texlist;
    if (texlist != NULL) {
        njRotateY(NULL, game_state.steps * MODEL_TURN);
        njSetTexture(texlist);
        njCnkEasyDrawObject(STATE(task)->kind->object);
    }
}

static void upgrade_rubberunit_step(struct task *task)
{
}

static void upgrade_shoes_draw(struct task *task)
{
    NJS_TEXLIST *texlist;

    texlist = STATE(task)->texlist;
    if (texlist != NULL) {
        njRotateY(NULL, game_state.steps * MODEL_TURN);
        njSetTexture(texlist);
        njCnkEasyDrawObject(STATE(task)->kind->object);
    }
}

static void upgrade_shoes_step(struct task *task)
{
}

static void upgrade_model_draw(struct task *task)
{
    NJS_TEXLIST *texlist;

    texlist = STATE(task)->texlist;
    if (texlist != NULL) {
        njRotateY(NULL, game_state.steps * MODEL_TURN);
        njSetTexture(texlist);
        njCnkEasyDrawObject(STATE(task)->kind->object);
    }
}

static void upgrade_model_step(struct task *task)
{
}

/* the texture list of the player in slot, where that player is character.
 * only Sonic, Shadow, Knuckles, Rouge and the mechs have one here. it returns
 * null otherwise */
static NJS_TEXLIST *upgrade_player_texlist(int slot, int character)
{
    if (player_character(slot) == character) {
        switch (character) {
        case PLAYER_SONIC:
        case PLAYER_SHADOW:
            return PLAYER_RECORD(slot)->by_character.sonic.texlist;
        case PLAYER_EGGMAN_MECH:
        case PLAYER_TAILS_MECH:
            return PLAYER_RECORD(slot)->by_character.mech.texlist;
        case PLAYER_KNUCKLES:
        case PLAYER_ROUGE:
            return PLAYER_RECORD(slot)->by_character.hunter.texlist;
        }
    }
    return NULL;
}

void upgrade_init(struct task *task)
{
    struct task_work *work = task->work;

    /* @hack redundant: a block no path reaches, which reads the SET editor's
     * labels */
    if (0) {
        (void)editor_angle_x;
        (void)editor_angle_y;
        (void)editor_angle_z;
        (void)editor_kind;
        (void)editor_camera_distance;
    }
    if (placement_try_leave(task)) {
        return;
    }
    task->motion.p = syCalloc(1, sizeof(struct upgrade_state));
    if (task->motion.p == NULL) {
        return;
    }
    /* @hack spelling: draw set before step, not in the members' order */
    task->draw = upgrade_draw;
    task->step = upgrade_step;
    task->destructor = upgrade_destroy;
    work->mode = MODE_HOLDING;
    KIND_INDEX(work) = (Uint8)work->scl.x % UPGRADE_KINDS;
    STATE(task)->kind = &upgrade_kinds[KIND_INDEX(work)];
    STATE(task)->texlist =
        upgrade_player_texlist(0, STATE(task)->kind->character);
    if (STATE(task)->texlist == NULL) {
        STATE(task)->texlist =
            upgrade_player_texlist(1, STATE(task)->kind->character);
    }
    if (flag_get(STATE(task)->kind->number)) {
        work->mode = MODE_TAKEN;
        work->timer = TAKEN_STEPS;
    }
    collision_init(task, upgrade_shapes, UPGRADE_SHAPES, COLLISION_OBJECT);
}

static void upgrade_destroy(struct task *task)
{
    if (STATE(task)->message != NULL) {
        task_destroy(STATE(task)->message);
    }
    syFree(task->motion.p);
    task->motion.p = NULL;
}

static void upgrade_step(struct task *task)
{
    struct task_work *work = task->work;
    struct task *player;
    int slot;
    char **messages;
    NJS_POINT3 pos;
    NJS_VECTOR spd;

    /* @hack spelling: the rest of the step in an else after the return, where
     * the return alone would do */
    if (placement_try_leave(task)) {
        return;
    } else {
        if (STATE(task)->kind->step != NULL && work->mode == MODE_HOLDING) {
            STATE(task)->kind->step(task);
        }
        switch (work->mode) {
        case MODE_HOLDING:
            /* @hack spelling: the assignment inside the test, where a
             * statement of its own would do */
            if ((player = collision_next_player(task)) != NULL) {
                /* @hack spelling: slot 1 under the test player !=
                 * player_tasks[0], where slot 0 under == would do */
                if (player != player_tasks[0]) {
                    slot = 1;
                } else {
                    slot = 0;
                }
                flag_set(STATE(task)->kind->number);
                player_records[slot]->upgrades |= STATE(task)->kind->bit;
                work->mode = MODE_TAKING;
                work->timer = 0;
                sound_play_at_for(SOUND_TAKE, work, SOUND_TAKE_PRIORITY,
                                  take_volume, SOUND_TAKE_DURATION, &work->pos);
                messages = messenger_upgrade_texts();
                if (STATE(task)->message != NULL) {
                    task_destroy(STATE(task)->message);
                }
                if (messages != NULL) {
                    /* @hack spelling: messages stepped to the upgrade's entry
                     * and read through, where messages[KIND_INDEX(work)]
                     * would do */
                    messages += KIND_INDEX(work);
                    message_window_open(MESSAGE_WINDOW_FADE, *messages,
                                        UNTIL_A(slot), game_state.text_language,
                                        &STATE(task)->message);
                }
                pos = work->pos;
                pos.y += PUFF_HEIGHT;
                spd.x = 0.0F;
                spd.y = PUFF_RISE;
                spd.z = 0.0F;
                dust_create(&pos, &spd, PUFF_SCALE);
                if (!(player_records[0]->flags & PLAYER_INVINCIBLE)) {
                    player_records[0]->flags |= PLAYER_INVINCIBLE;
                    MADE_INVINCIBLE(work) = TRUE;
                }
                player_request(0, PLAYER_REQUEST_ANIM, TAKE_ANIM);
                player_set_pos(0, work->pos.x, work->pos.y, work->pos.z);
                player_set_angles(0, work->ang.x, work->ang.y, work->ang.z);
                music_play_next(take_jingle);
                camera_push_mode(0, CAMERA_MODE_EASYSET);
                camera_set_adjust(0, camera_top(0), CAMERA_ADJUST_NONE);
                camera_easyset_states[0].type = CAMERA_TARGET_ANG_DIST;
                camera_easyset_states[0].unk_04 = 0;
                camera_easyset_states[0].trigger_push = FALSE;
                camera_easyset_states[0].dist = work->scl.y + CAMERA_MARGIN;
                camera_easyset_states[0].target = player_work[0]->pos;
                camera_easyset_states[0].target.y +=
                    player_records[0]->consts.target_height;
                camera_easyset_states[0].ang.y =
                    QUARTER_TURN - player_work[0]->ang.y;
                camera_easyset_states[0].ang.x = FULL_TURN - CAMERA_TILT;
                pause_disable();
                pad_disable_player(0);
                stage_clock_stop();
            }
            break;
        case MODE_TAKING:
            if (STATE(task)->message == NULL) {
                work->mode = MODE_TAKEN;
                if (MADE_INVINCIBLE(work)) {
                    player_records[0]->flags &= ~PLAYER_INVINCIBLE;
                    MADE_INVINCIBLE(work) = FALSE;
                }
                player_request(0, PLAYER_REQUEST_RELEASE, 0);
                camera_remove_mode(0, camera_top(0));
                music_play();
                pause_enable();
                pad_enable_player(0);
                stage_clock_resume();
            } else {
                camera_easyset_states[0].ang.y += CAMERA_TURN;
            }
            break;
        case MODE_TAKEN:
            if (work->timer < TAKEN_STEPS) {
                work->timer++;
            }
            break;
        /* @hack redundant: a default that does nothing */
        default:
            break;
        }
        collision_register(task);
        if (STATE(task)->texlist == NULL) {
            STATE(task)->texlist =
                upgrade_player_texlist(0, STATE(task)->kind->character);
            if (STATE(task)->texlist == NULL) {
                STATE(task)->texlist =
                    upgrade_player_texlist(1, STATE(task)->kind->character);
            }
        }
    }
}

static void upgrade_draw(struct task *task)
{
    struct task_work *work = task->work;
    NJS_COLOR glow;

    njPushMatrixEx();
    njTranslateEx(&work->pos);
    njRotateZ(NULL, work->ang.z);
    njRotateX(NULL, work->ang.x);
    njRotateY(NULL, work->ang.y);
    if (STATE(task)->kind->draw != NULL && work->mode == MODE_HOLDING) {
        njPushMatrixEx();
        STATE(task)->kind->draw(task);
        njPopMatrixEx();
    }
    njSetTexture(&stand_base_texlist);
    njCnkEasyDrawModel(&stand_base_model);
    if (work->mode == MODE_HOLDING || work->mode == MODE_TAKING) {
        njSetTexture(&stand_floor_texlist);
        njCnkEasyDrawModel(&stand_floor_model);
    }
    njFogDisable();
    texture_anim_apply(stand_column_model, &stand_column_anim,
                       game_state.steps);
    njSetTexture(&stand_column_texlist);
    if (work->mode == MODE_TAKING) {
        glow.color =
            color_blend(glow_from, glow_to,
                        (njSin(game_state.steps * GLOW_RATE) + 1.0F) * 0.5F);
        draw_control_save();
        draw_attr_save();
        draw_control_clear(DRAW_CONTROL_MATERIAL_EDITS);
        draw_control_set(DRAW_CONTROL_CONSTANT);
        njSetConstantAttr(CHUNK_TYPE_BITS, GLOW_ATTR);
        constant_material_set(
            glow.argb.a * COLOR_UNIT, glow.argb.r * COLOR_UNIT,
            glow.argb.g * COLOR_UNIT, glow.argb.b * COLOR_UNIT);
    }
    if (work->mode == MODE_HOLDING) {
        njCnkEasyDrawModel(stand_column_model);
    } else if (work->timer < COLUMN_STEPS) {
        njScale(NULL, COLUMN_X,
                (COLUMN_HEIGHT +
                 (COLUMN_PULSE * njSin(game_state.steps * COLUMN_PULSE_RATE))) *
                    (COLUMN_SHRINK_STEPS - work->timer) *
                    (1.0F / COLUMN_SHRINK_STEPS),
                COLUMN_Z);
        if (work->mode == MODE_TAKING) {
            njCnkSimpleDrawModel(stand_column_model);
        } else {
            njCnkEasyDrawModel(stand_column_model);
        }
    }
    /* the column's second draw, turned, and scaled on top of any scale the
     * first draw set */
    njRotateY(NULL, game_state.steps * COLUMN_TURN);
    if (work->mode == MODE_HOLDING) {
        njScale(NULL, COLUMN_X,
                column_scale * (COLUMN_HEIGHT +
                                (COLUMN_PULSE *
                                 njSin(game_state.steps * COLUMN_PULSE_RATE))),
                COLUMN_Z);
        njCnkEasyDrawModel(stand_column_model);
    } else if (work->timer < COLUMN_STEPS) {
        njScale(
            NULL, COLUMN_X,
            column_scale *
                (COLUMN_HEIGHT +
                 (COLUMN_PULSE * njSin(game_state.steps * COLUMN_PULSE_RATE))) *
                (COLUMN_SHRINK_STEPS - work->timer) *
                (1.0F / COLUMN_SHRINK_STEPS),
            COLUMN_Z);
        if (work->mode == MODE_TAKING) {
            njCnkSimpleDrawModel(stand_column_model);
        } else {
            njCnkEasyDrawModel(stand_column_model);
        }
    }
    if (work->mode == MODE_TAKING) {
        constant_material_set(0.0F, 0.0F, 0.0F, 0.0F);
        draw_attr_restore();
        draw_control_restore();
    }
    njFogEnable();
    njPopMatrixEx();
}
