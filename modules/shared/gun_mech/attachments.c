/* the markers of the jets and of the pods' two banks, as chunk models. Flying
 * Dog's fdog_find_points reads their vertices as the points of the jets and
 * pods */
#include <ninja.h>
#include <sg_xpt.h>

#include "shared/gun_mech/effect_art.h"

static Sint16 jet_marker_polygons[15] = {
#include "shared/gun_mech/attachments/jet_marker_polygons.inc"
};

Sint32 mech_boss_jet_markers[21] = {
#include "shared/gun_mech/attachments/mech_boss_jet_markers.inc"
};

static NJS_CNK_MODEL jet_marker_mesh = {
#include "shared/gun_mech/attachments/jet_marker_mesh.inc"
};

static NJS_CNK_OBJECT jet_marker = {
#include "shared/gun_mech/attachments/jet_marker.inc"
};

static Sint16 pod_1_marker_polygons[15] = {
#include "shared/gun_mech/attachments/pod_1_marker_polygons.inc"
};

Sint32 mech_boss_pods_1_markers[27] = {
#include "shared/gun_mech/attachments/mech_boss_pods_1_markers.inc"
};

static NJS_CNK_MODEL pod_1_marker_mesh = {
#include "shared/gun_mech/attachments/pod_1_marker_mesh.inc"
};

static NJS_CNK_OBJECT pod_1_marker = {
#include "shared/gun_mech/attachments/pod_1_marker.inc"
};

static Sint16 pod_0_marker_polygons[15] = {
#include "shared/gun_mech/attachments/pod_0_marker_polygons.inc"
};

Sint32 mech_boss_pods_0_markers[27] = {
#include "shared/gun_mech/attachments/mech_boss_pods_0_markers.inc"
};

static NJS_CNK_MODEL pod_0_marker_mesh = {
#include "shared/gun_mech/attachments/pod_0_marker_mesh.inc"
};

static NJS_CNK_OBJECT pod_0_marker = {
#include "shared/gun_mech/attachments/pod_0_marker.inc"
};
