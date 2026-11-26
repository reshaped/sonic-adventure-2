/* Ninja's 3D draw settings: its vertex buffer, NJD_CONTROL_3D_ bits,
 * constant attributes and material, and depth queue */
#include <ninja.h>
#include <sg_xpt.h>

/* ninja.h's njInit3D and njSetDepthQueue say what each of these holds */
extern Int _nj_vertex_num_;
extern Float _nj_depth_far_;
extern Float _nj_depth_range_;

void njInit3D(NJS_VERTEX_BUF *vbuf, Int vn)
{
    _nj_vertex_buf_ = vbuf;
    _nj_vertex_num_ = vn;
    _nj_control_3d_flag_ = 0;
    _nj_constant_attr_and_ = 0xffffffff;
    _nj_constant_attr_or_ = 0;
    _nj_constant_material_.a = 255.0F;
    _nj_constant_material_.r = 0.0F;
    _nj_constant_material_.g = 0.0F;
    _nj_constant_material_.b = 0.0F;
}

void njControl3D(Uint32 flag)
{
    _nj_control_3d_flag_ = flag;
}

void njSetConstantAttr(Uint32 and_attr, Uint32 or_attr)
{
    _nj_constant_attr_and_ = and_attr;
    _nj_constant_attr_or_ = or_attr;
}

void njSetConstantMaterial(NJS_ARGB *material)
{
    _nj_constant_material_.a = material->a;
    _nj_constant_material_.r = material->r;
    _nj_constant_material_.g = material->g;
    _nj_constant_material_.b = material->b;
}

void njSetDepthQueue(Float dnear, Float dfar)
{
    _nj_depth_near_ = -dnear;
    _nj_depth_far_ = -dfar;
    _nj_depth_range_ = 1.0F / (_nj_depth_far_ - _nj_depth_near_);
}
