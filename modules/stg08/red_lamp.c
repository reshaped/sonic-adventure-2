/* REDLAMP in Security Hall's object list: a red lamp that no code draws. this
 * source holds its models, and security_red_lamp_step only tests the
 * placement's range */
#include <ninja.h>
#include <sg_xpt.h>

#include "object/set.h"
#include "stg08/security_props.h"

static const char miu64_ig011[12] = {
#include "stg08/red_lamp/miu64_ig011.inc"
};

static const char miu64_sh009[12] = {
#include "stg08/red_lamp/miu64_sh009.inc"
};

static const char pr128_27[12] = {
#include "stg08/red_lamp/pr128_27.inc"
};

static const char pr64_12[8] = {
#include "stg08/red_lamp/pr64_12.inc"
};

static NJS_TEXNAME red_lamp_materials[4] = {
#include "stg08/red_lamp/red_lamp_materials.inc"
};

static NJS_TEXLIST red_lamp_texlist = {
#include "stg08/red_lamp/red_lamp_texlist.inc"
};

static Sint16 red_lamp_reflector_polygons[119] = {
#include "stg08/red_lamp/red_lamp_reflector_polygons.inc"
};

static Sint32 red_lamp_reflector_vertices[105] = {
#include "stg08/red_lamp/red_lamp_reflector_vertices.inc"
};

static NJS_CNK_MODEL red_lamp_reflector_model = {
#include "stg08/red_lamp/red_lamp_reflector_model.inc"
};

static NJS_CNK_OBJECT red_lamp_reflector = {
#include "stg08/red_lamp/red_lamp_reflector.inc"
};

static Sint16 red_lamp_lens_polygons[27] = {
#include "stg08/red_lamp/red_lamp_lens_polygons.inc"
};

static Sint32 red_lamp_lens_vertices[27] = {
#include "stg08/red_lamp/red_lamp_lens_vertices.inc"
};

static NJS_CNK_MODEL red_lamp_lens_model = {
#include "stg08/red_lamp/red_lamp_lens_model.inc"
};

static NJS_CNK_OBJECT red_lamp_lens = {
#include "stg08/red_lamp/red_lamp_lens.inc"
};

static Sint16 red_lamp_housing_polygons[251] = {
#include "stg08/red_lamp/red_lamp_housing_polygons.inc"
};

static Sint32 red_lamp_housing_vertices[147] = {
#include "stg08/red_lamp/red_lamp_housing_vertices.inc"
};

static NJS_CNK_MODEL red_lamp_housing_model = {
#include "stg08/red_lamp/red_lamp_housing_model.inc"
};

static NJS_CNK_OBJECT red_lamp_housing = {
#include "stg08/red_lamp/red_lamp_housing.inc"
};

NJS_TEXLIST *security_red_lamp_textures[1] = {
#include "stg08/red_lamp/security_red_lamp_textures.inc"
};

/* copies of lamp.c's pulse for LIGHT's lens: its phase's turn a step, and the
 * colors it blends between */
static Angle pulse_speed = 512;
static Uint32 pulse_clear = 0x00ffffff;
static Uint32 pulse_white = 0xffffffff;

void security_red_lamp_step(struct task *task)
{
    /* @hack redundant: the test of placement_try_leave's result, with an
     * empty body, where the call alone would do */
    if (!placement_try_leave(task)) {
    }
}
