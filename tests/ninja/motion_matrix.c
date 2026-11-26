/* Each channel's node pointer must agree with its linear motion cursor. */
#include <assert.h>
#include <nj_cnk.h>
#include <nj_cnk_motlink.h>
#include <nj_cnk_shape.h>
#include <nj_motion.h>
#include <ninja.h>

void run_njCnkMotionS(NJS_CNK_OBJECT *node, int shaped);
void run_njCnkMotionE(NJS_CNK_OBJECT *node, int shaped);
void run_njCnkMotionEM(NJS_CNK_OBJECT *node, int shaped);
void run_njCnkMotLinkS(NJS_CNK_OBJECT *node, int shaped);
void run_njCnkMotLinkE(NJS_CNK_OBJECT *node, int shaped);

static NJS_CNK_OBJECT nodes[6];
static NJS_CNK_MODEL model;
static int depth, capacity, failed_pushes;
static int callbacks, poses[2], shapes[2], draws;
Uint16 _nj_pos_int_flag_, _nj_ang_int_flag_, _nj_scl_int_flag_;
Uint16 _nj_mot_current_data_;

Bool njPushMatrixEx(void)
{
    if (depth == capacity) {
        ++failed_pushes;
        return FALSE;
    }
    ++depth;
    return TRUE;
}

Bool njPopMatrixEx(void)
{
    assert(depth > 1);
    --depth;
    return TRUE;
}

static void pose(NJS_CNK_OBJECT *node)
{
    int slot = _nj_mot_current_data_;
    assert(slot == 0 || slot == 1);
    assert(node == &nodes[poses[slot]++]);
}

static void shape(NJS_CNK_OBJECT *node)
{
    int slot = _nj_mot_current_data_;
    assert(node == &nodes[shapes[slot]++]);
}

static void visit(NJS_CNK_OBJECT *node)
{
    assert(node == &nodes[callbacks++]);
}

static Sint32 draw(NJS_CNK_MODEL *mesh)
{
    assert(mesh == &model);
    ++draws;
    return 0;
}

void (*_nj_cnk_motion_data_func_)(NJS_CNK_OBJECT *) = pose;
void (*_nj_cnk_shape_data_func_)(NJS_CNK_OBJECT *) = shape;
void (*_nj_cnk_motion_callback_)(NJS_CNK_OBJECT *) = visit;
void (*_nj_cnk_motlink_data_func_[2])(NJS_CNK_OBJECT *) = {pose, pose};
void (*_nj_cnk_shapelink_data_func_[2])(NJS_CNK_OBJECT *) = {shape, shape};
Sint32 (*_nj_cnk_draw_model_)(NJS_CNK_MODEL *) = draw;

Sint32 _njCnkSimpleDrawModel(NJS_CNK_MODEL *mesh)
{
    return draw(mesh);
}
Sint32 _njCnkEasyDrawModel(NJS_CNK_MODEL *mesh)
{
    return draw(mesh);
}
void njCnkMakeMotionLink(void)
{
}
void njCnkMakeSimpleShapeLink(NJS_CNK_OBJECT *node)
{
}
void njCnkMakeEasyShapeLink(NJS_CNK_OBJECT *node)
{
}
void njTranslateEx(NJS_VECTOR *v)
{
    assert(0);
}
void njScaleEx(NJS_VECTOR *v)
{
    assert(0);
}
void njRotateEx(Angle *angles, Sint32 order)
{
    assert(0);
}

typedef void (*walker)(NJS_CNK_OBJECT *, int);
static void check(walker walk, int linked, int shaped, int available, int count)
{
    capacity = available;
    depth = 1;
    failed_pushes = 0;
    poses[0] = poses[1] = shapes[0] = shapes[1] = 0;
    callbacks = draws = 0;
    _nj_mot_current_data_ = 0;
    walk(nodes, shaped);
    assert(depth == 1 && failed_pushes == (count < 6));
    assert(callbacks == count);
    assert(poses[0] == count && poses[1] == (linked ? count : 0));
    assert(shapes[0] == (shaped ? count : 0));
    assert(shapes[1] == (shaped && linked ? count : 0));
    assert(draws == (shaped ? 0 : count));
}

int main(void)
{
    static walker const walkers[] = {run_njCnkMotionS, run_njCnkMotionE,
                                     run_njCnkMotionEM, run_njCnkMotLinkS,
                                     run_njCnkMotLinkE};
    int i, shaped;

    nodes[0].child = &nodes[1];
    nodes[1].child = &nodes[2];
    nodes[2].sibling = &nodes[3];
    nodes[1].sibling = &nodes[4];
    nodes[0].sibling = &nodes[5];
    for (i = 0; i < 6; i++) {
        nodes[i].model = &model;
    }
    for (i = 0; i < (int)(sizeof(walkers) / sizeof(walkers[0])); i++) {
        for (shaped = 0; shaped <= 1; shaped++) {
            check(walkers[i], i >= 3, shaped, 1, 0);
            check(walkers[i], i >= 3, shaped, 2, 1);
            check(walkers[i], i >= 3, shaped, 3, 2);
            check(walkers[i], i >= 3, shaped, 4, 6);
        }
    }
    return 0;
}
