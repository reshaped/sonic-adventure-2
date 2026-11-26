/* the spheres' models and pulse, which each fight's spheres.c includes once,
 * after its sphere_models. the includer defines SPHERE_BUILD_MODELS, the name
 * of the call that compiles the models, and SPHERE_MESH_COUNT, their count. it
 * defines SPHERE_TEXTURES, their texture list, and SPHERE_MESHES, the array
 * the compiled meshes go to. the Biolizard's fight also defines
 * SPHERE_RESET_CLIP, which sets the direct clip again before each mesh */
#include <ninja.h>
#include <stdlib.h>
#include "draw/light.h"
#include "shared/last_boss_sphere.h"
#include "system/angle.h"
#include "system/areas.h"
#include "system/task.h"
#include "system/util.h"

/* how a sphere's scale pulses. it grows by growth, less up to growth_variation,
 * a step until it reaches peak. it then shrinks by shrink, less up to
 * shrink_variation, a step. at trough or below, it grows again with odds of
 * turn_chance a step, or at once below PULSE_FLOOR. PULSE_SPHERE reads
 * pulse_rates[PULSE_CHANNEL] alone */
#define PULSE_FLOOR 0.5F
#define PULSE_CHANNEL 0
struct sphere_pulse {
    Float peak;
    Float trough;
    Float unused;
    Float growth;
    Float growth_variation;
    Float shrink;
    Float shrink_variation;
    Float turn_chance;
};

static struct sphere_pulse pulse_rates[] = {
    {1.1F, 0.9F, 0, 0.04F, 0.01F, 0.005F, 0.003F, 0.2F},
    {1.05F, 0.95F, 0, 0.025F, 0, 0, 0, 0},
};

#define SPHERE_SHADOW (NJD_CONTROL_3D_SHADOW | NJD_CONTROL_3D_TRANS_MODIFIER)

#ifdef NONMATCHING
/* whether object holds a model with both a vertex and a polygon list. GCC's
 * side compiles no other */
static Bool sphere_has_mesh(const NJS_CNK_OBJECT *object)
{
    return object && object->model && object->model->vlist &&
           object->model->plist;
}
#endif

/* compile source alone, without its child and sibling, at *buffer, which
 * moves past it. it returns the mesh, or null where it would reach
 * DEMO_AREA */
static NJS_DIRECT_HEAD *sphere_compile_mesh(Uint32 *buffer,
                                            const NJS_CNK_OBJECT *source)
{
    NJS_CNK_OBJECT object;
    NJS_DIRECT_HEAD *compiled;
    Uint32 end;

#ifdef NONMATCHING
    /* this side compiles no mesh that sphere_has_mesh refuses, as the disc-free
     * build's are */
    if (!sphere_has_mesh(source)) {
        return NULL;
    }
#endif
    object = *source;
    object.child = NULL;
    object.sibling = NULL;
    compiled = NULL;
    end = *buffer + njCnkDirectObjectCompileSize(&object);
    if (end < (Uint32)DEMO_AREA) {
#ifdef SPHERE_RESET_CLIP
        njSetDirectClip();
#endif
        njCnkDirectObjectCompile(&object, (void *)*buffer);
        compiled = (NJS_DIRECT_HEAD *)*buffer;
        *buffer = end;
    }
    return compiled;
}

Uint32 SPHERE_BUILD_MODELS(Uint32 buffer)
{
    /* @hack type: mesh a Sint8, and the count cast to one, where an int would
     * do */
    Sint8 mesh;

#ifdef NONMATCHING
    /* this side builds no models while no sphere has a mesh, as in the
     * disc-free build */
    for (mesh = 0; mesh < SPHERE_MESH_COUNT; mesh++) {
        if (sphere_has_mesh(sphere_models[mesh])) {
            break;
        }
        SPHERE_MESHES[mesh] = NULL;
    }
    if (mesh == SPHERE_MESH_COUNT) {
        return buffer;
    }
#endif
    draw_control_set(SPHERE_SHADOW);
    njSetTexture(&SPHERE_TEXTURES);
    light_apply(LAST_BOSS_EFFECT_LIGHT);
    njSetDirectClip();
    for (mesh = 0; mesh < (Sint8)SPHERE_MESH_COUNT; mesh++) {
        SPHERE_MESHES[mesh] = sphere_compile_mesh(&buffer, sphere_models[mesh]);
    }
    draw_control_clear(SPHERE_SHADOW);
    return buffer;
}

static Float sphere_pulse_scale(Float scale, Float *speed, Sint8 channel)
{
    struct sphere_pulse *rate = &pulse_rates[channel];
    Float *growth;
    Float *variation;
    Float result;
    /* @hack spelling: growth and variation set in the first test, where
     * rate->growth and rate->growth_variation would do */
    if (growth = &rate->growth, variation = &rate->growth_variation,
        *speed == 0.0F) {
        *speed = *growth - (rand() * *variation * RAND_SCALE);
    } else if (!(rate->peak > scale)) {
        *speed = -rate->shrink + (rand() * rate->shrink_variation * RAND_SCALE);
    } else if (!(scale > rate->trough)) {
        if (scale < PULSE_FLOOR || rand() * RAND_SCALE < rate->turn_chance) {
            *speed = *growth - (rand() * *variation * RAND_SCALE);
        }
    }
    /* @hack spelling: result, a local the return passes through, where
     * returning *speed + scale would do */
    result = *speed + scale;
    return result;
}

/* step sphere's pulse. scale takes sphere_pulse_scale's next value on every
 * axis, with wobble as its speed. scale_phase's x and z become 1 plus
 * PULSE_DEPTH times the sine of pulse. its y becomes the same half a turn
 * apart. pulse turns PULSE_STEP a step, and PULSE_STRETCH more for each unit
 * the old scale lay above 1 */
#define PULSE_STEP 4096.0F
#define PULSE_STRETCH 20480.0F
#define PULSE_DEPTH 0.02F
#define PULSE_SPHERE(sphere)                                                   \
    do {                                                                       \
        (sphere)->scale_phase = (sphere)->scale;                               \
        (sphere)->scale.x = sphere_pulse_scale(                                \
            (sphere)->scale.x, &(sphere)->wobble, PULSE_CHANNEL);              \
        (sphere)->scale.y = (sphere)->scale.z = (sphere)->scale.x;             \
        (sphere)->pulse +=                                                     \
            PULSE_STEP + (((sphere)->scale_phase.x - 1.0F) * PULSE_STRETCH);   \
        if ((sphere)->pulse >= FULL_TURN) {                                    \
            (sphere)->pulse -= FULL_TURN;                                      \
        }                                                                      \
        (sphere)->scale_phase.x =                                              \
            1.0F + (njSin((sphere)->pulse) * PULSE_DEPTH);                     \
        (sphere)->scale_phase.y =                                              \
            1.0F + (njSin((sphere)->pulse + HALF_TURN) * PULSE_DEPTH);         \
        (sphere)->scale_phase.z =                                              \
            1.0F + (njSin((sphere)->pulse) * PULSE_DEPTH);                     \
    } while (0)

#define SCALE_COMPONENTS(vector, factor)                                       \
    do {                                                                       \
        (vector).x *= (factor);                                                \
        (vector).y *= (factor);                                                \
        (vector).z *= (factor);                                                \
    } while (0)
#define SPHERE_SQ(value) ((value) * (value))

/* a sphere task's modes, in its work */
enum sphere_mode {
    SPHERE_RISE,
    SPHERE_HOVER,
    SPHERE_CHARGE,
    SPHERE_FLY,
    SPHERE_BURST,
    SPHERE_FADE,
    SPHERE_LIFT
};
/* the spheres' sounds. the first three are 10 to 12 of the list each fight
 * loads. SOUND_SPHERE_BREAK is 0x15 of sound list place 0, which an id's
 * 0x8000 picks */
enum sphere_sound {
    SOUND_SPHERE_IMPACT = 10,
    SOUND_SPHERE_FLIGHT = 11,
    SOUND_SPHERE_CHARGE = 12,
    SOUND_SPHERE_BREAK = 0x8015
};
