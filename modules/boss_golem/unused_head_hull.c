/* a Ninja basic model of ten strips, which no code reads. its center lies near
 * that of the head's model */
#include <ninja.h>
#include <sg_xpt.h>

/* NOLINTBEGIN(clang-diagnostic-missing-braces) */
static NJS_MATERIAL upper_hull_materials[1] = {
#include "boss_golem/unused_head_hull/upper_hull_materials.inc"
};
/* NOLINTEND(clang-diagnostic-missing-braces) */

static Sint16 upper_hull_faces[134] = {
#include "boss_golem/unused_head_hull/upper_hull_faces.inc"
};

static NJS_MESHSET upper_hull_meshes[1] = {
#include "boss_golem/unused_head_hull/upper_hull_meshes.inc"
};

static NJS_POINT3 upper_hull_points[65] = {
#include "boss_golem/unused_head_hull/upper_hull_points.inc"
};

static NJS_VECTOR upper_hull_normals[65] = {
#include "boss_golem/unused_head_hull/upper_hull_normals.inc"
};

static NJS_MODEL upper_hull_model = {
#include "boss_golem/unused_head_hull/upper_hull_model.inc"
};

static NJS_OBJECT upper_hull = {
#include "boss_golem/unused_head_hull/upper_hull.inc"
};
