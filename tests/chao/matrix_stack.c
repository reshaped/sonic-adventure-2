/* Exercise Chao's public pose and draw entry points with translated joints.
 * Distinct translations expose a pop into a sibling's or caller's frame. */
#include <assert.h>
#include <string.h>

#include "chao/actor.h"
#include "chao/body_pose.h"
#include "chao/minimal.h"
#include "chao/movement.h"
#include "chao/tuning.h"
#include "chao/work.h"
#include "draw/lines.h"
#include "system/task.h"
#include "system/util.h"

NJS_TEXLIST chao_env_texlist, chao_body_texlist, chao_eye_texlist;
NJS_TEXLIST chao_jewel_texlist, chao_mini_parts_texlist, chao_mouth_texlist;
NJS_TEXLIST garden_animal_texlist;
NJS_CNK_OBJECT chao_hat_1, chao_hat_2, chao_hat_3;
NJS_CNK_OBJECT chao_special_model_1, chao_special_model_2;
NJS_CNK_OBJECT *chao_costume_models[CHAO_BODY_NODES], *chao_medal_models[32];
NJS_CNK_OBJECT *garden_minimal_models[ANIMAL_KINDS];
NJS_CNK_MODEL garden_shadow_model;
struct chao_tuning chao_tuning;
NJS_MATRIX _nj_unit_matrix_ = {1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1};
Sint32 _nj_stack_size_, _nj_max_stack_size_;

static NJS_MATRIX current, caller, entry, stack[128];
static struct task task;
static struct chao_work work;
static struct chao_node nodes[CHAO_BODY_NODES];
static NJS_MATRIX matrices[CHAO_BODY_NODES];
static NJS_MDATA2 playing[CHAO_BODY_NODES], linking[CHAO_BODY_NODES];
static NJS_MOTION clips[2];
static struct chao_mesh mesh;
static struct chunk_vertex_vn vertex;
static struct chao_vertex_spring spring;
static NJS_POINT3 rest;
static NJS_VECTOR normal;
static int textures, controls, attributes, icons, special_draws, model_draws,
    prop_draws;
static float prop_x, mesh_x;
static NJS_CNK_OBJECT *special_object, *drawn_medal;
static NJS_TEXLIST *bound_textures;
static int medal_bindings;
static struct motion_control *special_motion;

void njGetMatrix(NJS_MATRIX *m)
{
    memcpy(*m, current, sizeof(current));
}
void njSetMatrix(NJS_MATRIX *d, NJS_MATRIX *s)
{
    memcpy(d ? *d : current, *s, sizeof(current));
}
void njUnitMatrix(NJS_MATRIX *m)
{
    njSetMatrix(m, &_nj_unit_matrix_);
}

Bool njPushMatrixEx(void)
{
    if (_nj_stack_size_ == _nj_max_stack_size_) {
        return FALSE;
    }
    njGetMatrix(&stack[_nj_stack_size_++ - 1]);
    return TRUE;
}

Bool njPushMatrix(NJS_MATRIX *m)
{
    if (!njPushMatrixEx()) {
        return FALSE;
    }
    if (m) {
        njSetMatrix(NULL, m);
    }
    return TRUE;
}

Bool njPopMatrixEx(void)
{
    assert(_nj_stack_size_ > 1);
    njSetMatrix(NULL, &stack[--_nj_stack_size_ - 1]);
    return TRUE;
}

void njTranslate(NJS_MATRIX *m, Float x, Float y, Float z)
{
    assert(m == NULL);
    current[12] += x;
    current[13] += y;
    current[14] += z;
}
void njTranslateV(NJS_MATRIX *m, NJS_VECTOR *v)
{
    njTranslate(m, v->x, v->y, v->z);
}
void njTranslateEx(NJS_VECTOR *v)
{
    njTranslateV(NULL, v);
}
void njGetTranslation(NJS_MATRIX *m, NJS_VECTOR *v)
{
    v->x = current[12];
    v->y = current[13];
    v->z = current[14];
}
void njAddVector(NJS_VECTOR *d, NJS_VECTOR *s)
{
    d->x += s->x;
    d->y += s->y;
    d->z += s->z;
}
void njCalcPoint(NJS_MATRIX *m, NJS_POINT3 *s, NJS_POINT3 *d)
{
    njGetTranslation(m, d);
    njAddVector(d, s);
}
void njCalcPoints(NJS_MATRIX *m, NJS_POINT3 *s, NJS_POINT3 *d, Int n)
{
    int i;
    for (i = 0; i < n; i++) {
        njCalcPoint(m, &s[i], &d[i]);
    }
}
void njCalcVector(NJS_MATRIX *m, NJS_VECTOR *s, NJS_VECTOR *d)
{
    *d = *s;
}
void garden_subtract_matrix(NJS_MATRIX *m, NJS_MATRIX *previous)
{
    int i;
    for (i = 12; i < 15; i++) {
        (*m)[i] -= (*previous)[i];
    }
}
void garden_clear_translation(NJS_MATRIX *m)
{
    memset((m ? *m : current) + 12, 0, 3 * sizeof(Float));
}
Bool njInvertMatrix(NJS_MATRIX *m)
{
    return TRUE;
}
void njMultiMatrix(NJS_MATRIX *d, NJS_MATRIX *s)
{
    njTranslate(d, (*s)[12], (*s)[13], (*s)[14]);
}
void njRotateXYZ(NJS_MATRIX *m, Angle x, Angle y, Angle z)
{
}
void njRotateY(NJS_MATRIX *m, Angle a)
{
}
void njRotateZ(NJS_MATRIX *m, Angle a)
{
}
void njRotateX(NJS_MATRIX *m, Angle a)
{
}
void njScale(NJS_MATRIX *m, Float x, Float y, Float z)
{
}
Float njSin(Angle a)
{
    return 0;
}
void garden_blend_angles(const Angle *a, const Angle *b, Float t, Angle *out)
{
    memcpy(out, a, 3 * sizeof(Angle));
}

void njCnkSimpleDrawModel(NJS_CNK_MODEL *m)
{
    mesh_x = current[12];
    model_draws++;
}
void njCnkSimpleDrawObject(NJS_CNK_OBJECT *o)
{
    if (bound_textures == &chao_jewel_texlist) {
        assert(o != NULL);
        drawn_medal = o;
    }
    prop_x = current[12];
    prop_draws++;
}
void njCnkEasyDrawModel(NJS_CNK_MODEL *m)
{
    assert(0);
}
void njCnkEasyDrawObject(NJS_CNK_OBJECT *o)
{
    assert(0);
}
void njCnkModDrawModel(NJS_CNK_MODEL *m)
{
    assert(0);
}
Sint32 njSetTexture(NJS_TEXLIST *t)
{
    bound_textures = t;
    if (t == &chao_jewel_texlist) {
        medal_bindings++;
    }
    textures++;
    return 0;
}
int chao_current_area(void)
{
    return 0;
}
void chao_bind_palette(struct task *t, int slot)
{
}
Angle chao_eyelid_angle(struct task *t)
{
    return 0;
}
Angle chao_eyelid_slope(struct task *t)
{
    return 0;
}
void draw_attr_save(void)
{
    assert(attributes == 0);
    attributes++;
}
void draw_attr_restore(void)
{
    assert(attributes == 1);
    attributes--;
}
void draw_control_save(void)
{
    assert(controls == 0);
    controls++;
}
void draw_control_restore(void)
{
    assert(controls == 1);
    controls--;
}
void draw_control_set(Uint32 bits)
{
    assert(controls == 1);
}
void draw_control_clear(Uint32 bits)
{
    assert(controls == 1);
}
void constant_material_set(Float a, Float r, Float g, Float b)
{
    assert(controls == 1);
}
Bool garden_visible_box(struct task *t, Float x, Float y, Float h)
{
    return TRUE;
}
void chao_draw_icon(struct task *t)
{
    icons++;
}
void motion_draw_simple(NJS_CNK_OBJECT *object, struct motion_control *motion)
{
    assert(object == special_object && motion == special_motion);
    assert(current[12] == entry[12] + work.base.pos.x);
    special_draws++;
}

static void check_caller(void)
{
    assert(_nj_stack_size_ == 2);
    assert(memcmp(stack[0], caller, sizeof(caller)) == 0);
    assert(memcmp(current, entry, sizeof(entry)) == 0);
    assert(controls == 0 && attributes == 0);
}

static void prepare_body(void)
{
    int i;
    task.work = &work.base;
    work.base.pos.x = 10;
    work.flags = CHAO_VISIBLE | CHAO_BODY_IN_VIEW | CHAO_BODY_SPRINGS;
    work.shape.root = nodes;
    for (i = 0; i < CHAO_BODY_NODES; i++) {
        nodes[i].object.pos[0] = i + 1;
        nodes[i].matrix = &matrices[i];
        nodes[i].object.evalflags = NJD_EVAL_UNIT_SCL;
        work.shape.nodes[i] = &nodes[i].object;
        if (i >= 7 && i + 1 < CHAO_BODY_NODES) {
            nodes[i].object.sibling = &nodes[i + 1].object;
        }
    }
    nodes[0].object.child = &nodes[1].object;
    nodes[1].object.child = &nodes[2].object;
    nodes[1].object.sibling = &nodes[7].object;
    nodes[2].object.child = &nodes[3].object;
    nodes[2].object.sibling = &nodes[5].object;
    nodes[3].object.child = &nodes[4].object;
    nodes[5].object.child = &nodes[6].object;
    nodes[4].prop.object = &chao_hat_1;
    nodes[4].placed = nodes[4].prop.fade_in = 1;
    nodes[4].prop_position.x = 20;
    nodes[6].object.model = (NJS_CNK_MODEL *)&mesh;
    nodes[6].swing = 1;
    nodes[6].velocity.x = 4;
    nodes[6].weight = 0.5F;
    mesh.vertex_count = 1;
    mesh.vertices = &vertex;
    mesh.rest_points = &rest;
    mesh.rest_normals = &normal;
    mesh.springs = &spring;
    vertex.point.x = 3;
    rest.x = -1;
    spring.weight = 1;
    clips[0].mdata = playing;
    clips[1].mdata = linking;
    work.motion.flags = MOTION_LINKING;
    work.motion.current.motion = &clips[0];
    work.motion.next_play.motion = &clips[1];
}

static void denied(int build_room, int pose_room, int draw_room)
{
    chao_pose_cursor = playing + 5;
    chao_pose.current.node = playing + 2;
    chao_pose.next.node = linking + 3;
    work.shape.hand_4.x = 57;
    work.flags &= ~CHAO_BODY_IN_VIEW;
    _nj_max_stack_size_ = _nj_stack_size_ + build_room;
    chao_build_node_matrices(&task);
    check_caller();
    _nj_max_stack_size_ = _nj_stack_size_ + pose_room;
    chao_move_body_tree(&task);
    check_caller();
    _nj_max_stack_size_ = _nj_stack_size_ + draw_room;
    chao_actor_draw(&task);
    check_caller();
    assert(chao_pose.current.node == playing + 2 &&
           chao_pose.next.node == linking + 3);
    assert(!(work.flags & CHAO_BODY_IN_VIEW));
    assert(chao_pose_cursor == playing + 5 && work.shape.hand_4.x == 57);
    assert(matrices[0][12] == 0 && matrices[6][12] == 0);
    assert(nodes[4].prop.scale == 0 && nodes[6].object.pos[0] == 7 &&
           vertex.point.x == 3);
    assert(textures == 0 && icons == 0 && model_draws == 0 && prop_draws == 0);
}

static void retry(void)
{
    _nj_max_stack_size_ = 128;
    work.flags |= CHAO_BODY_IN_VIEW;
    chao_build_node_matrices(&task);
    check_caller();
    assert(matrices[4][12] == 25 && matrices[6][12] == 26 &&
           matrices[7][12] == 19);
    work.base.pos.x += 3;
    chao_move_body_tree(&task);
    check_caller();
    assert(chao_pose.current.node == playing + CHAO_BODY_NODES);
    assert(chao_pose.next.node == linking + CHAO_BODY_NODES);
    assert(work.shape.hand_4.x == 28 && work.shape.foot_6.x == 29.1F);
    assert(matrices[6][12] == 29 && nodes[6].object.pos[0] == 11.3F);
    assert(nodes[4].prop.scale == 0.1F && vertex.point.x == 3.6F);
    chao_actor_draw(&task);
    check_caller();
    assert(icons == 1 && model_draws == 1 && prop_draws == 1);
    assert(prop_x == 125 && mesh_x == 110.3F);
    assert(chao_pose.current.node == playing + CHAO_BODY_NODES);
}

static void exact_fit(void)
{
    _nj_max_stack_size_ = _nj_stack_size_ + 6;
    chao_build_node_matrices(&task);
    check_caller();
    assert(matrices[7][12] == 22);
    _nj_max_stack_size_ = _nj_stack_size_ + 8;
    chao_move_body_tree(&task);
    check_caller();
    assert(nodes[4].prop.scale == 0.2F);
    _nj_max_stack_size_ = _nj_stack_size_ + 7;
    chao_actor_draw(&task);
    check_caller();
    assert(icons == 2 && model_draws == 2 && prop_draws == 2);
}

static void draw_medals(void)
{
    unsigned int number;

    /* The normal node mesh remains visible when no medal is equipped. */
    nodes[35].object.model = (NJS_CNK_MODEL *)&mesh;
    for (number = 0; number <= 32; number++) {
        NJS_CNK_OBJECT *expected = NULL;
        int bindings = medal_bindings, models = model_draws;

        /* These two runs of model pointers come from the retail table. */
        if ((number >= 1 && number <= 15) || (number >= 17 && number <= 26)) {
            expected = &nodes[number].object;
            chao_medal_models[number] = expected;
        }
        drawn_medal = NULL;
        work.profile.parameters.medal = (Uint8)number;
        chao_actor_draw(&task);
        check_caller();
        assert(drawn_medal == expected);
        assert(medal_bindings == bindings + (expected != NULL));
        /* Node 6 also draws a mesh on every traversal. */
        assert(model_draws == models + 1 + (expected == NULL));
        assert(work.profile.parameters.medal == number);
    }
}

static void special_models(void)
{
    const int kinds[] = {CHAO_BODY_MODEL_1, CHAO_BODY_MODEL_2,
                         CHAO_BODY_MINIMAL};
    NJS_CNK_OBJECT *models[] = {&chao_special_model_1, &chao_special_model_2,
                                &chao_hat_1};
    int i;
    garden_minimal_models[0] = models[2];
    for (i = 0; i < 3; i++) {
        int bound = textures;
        special_object = models[i];
        special_motion = i == 2 ? &work.minimal_motion : &work.motion;
        work.profile.parameters.body = kinds[i];
        _nj_max_stack_size_ = _nj_stack_size_;
        chao_actor_draw(&task);
        check_caller();
        assert(textures == bound && special_draws == i);
        _nj_max_stack_size_ = 128;
        chao_actor_draw(&task);
        check_caller();
        assert(textures == bound + 1 && special_draws == i + 1);
    }
}

int main(void)
{
    njUnitMatrix(NULL);
    njTranslate(NULL, 77, 4, 9);
    njGetMatrix(&entry);
    njSetMatrix(&caller, &_nj_unit_matrix_);
    caller[12] = 999;
    njSetMatrix(&stack[0], &caller);
    _nj_stack_size_ = 2;
    prepare_body();
    denied(0, 0, 0);
    denied(1, 1, 1);
    /* Five joints share the deepest path. Each call is one slot short. */
    denied(5, 7, 6);
    retry();
    exact_fit();
    draw_medals();
    special_models();
    return 0;
}
