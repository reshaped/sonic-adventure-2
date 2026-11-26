/* the chunk models that 43 images place, from the disc. its D lies between
 * upgrade.c's D and its DUPGRADE */
#include <ninja.h>
#include <sg_xpt.h>

#include "shared/upgrade_models.h"

static Sint16 sonic_rubberunit_polygons[353] = {
#include "shared/upgrade_models/sonic_rubberunit_polygons.inc"
};
static Sint32 sonic_rubberunit_vertices[267] = {
#include "shared/upgrade_models/sonic_rubberunit_vertices.inc"
};
static NJS_CNK_MODEL sonic_rubberunit_model = {
#include "shared/upgrade_models/sonic_rubberunit_model.inc"
};
NJS_CNK_OBJECT upgrade_sonic_rubberunit = {
#include "shared/upgrade_models/upgrade_sonic_rubberunit.inc"
};
static Sint16 sonic_shoes_polygons[1993] = {
#include "shared/upgrade_models/sonic_shoes_polygons.inc"
};
static Sint32 sonic_shoes_vertices[1395] = {
#include "shared/upgrade_models/sonic_shoes_vertices.inc"
};
static NJS_CNK_MODEL sonic_shoes_model = {
#include "shared/upgrade_models/sonic_shoes_model.inc"
};
NJS_CNK_OBJECT upgrade_sonic_shoes = {
#include "shared/upgrade_models/upgrade_sonic_shoes.inc"
};
static Sint16 sonic_flamering_child_polygons[45] = {
#include "shared/upgrade_models/sonic_flamering_child_polygons.inc"
};
static Sint32 sonic_flamering_child_vertices[45] = {
#include "shared/upgrade_models/sonic_flamering_child_vertices.inc"
};
static NJS_CNK_MODEL sonic_flamering_child_model = {
#include "shared/upgrade_models/sonic_flamering_child_model.inc"
};
static NJS_CNK_OBJECT sonic_flamering_child_object = {
#include "shared/upgrade_models/sonic_flamering_child_object.inc"
};
static Sint16 sonic_flamering_polygons[183] = {
#include "shared/upgrade_models/sonic_flamering_polygons.inc"
};
static Sint32 sonic_flamering_vertices[117] = {
#include "shared/upgrade_models/sonic_flamering_vertices.inc"
};
static NJS_CNK_MODEL sonic_flamering_model = {
#include "shared/upgrade_models/sonic_flamering_model.inc"
};
NJS_CNK_OBJECT upgrade_sonic_flamering = {
#include "shared/upgrade_models/upgrade_sonic_flamering.inc"
};
static Sint16 sonic_magicwrist_polygons[425] = {
#include "shared/upgrade_models/sonic_magicwrist_polygons.inc"
};
static Sint32 sonic_magicwrist_vertices[237] = {
#include "shared/upgrade_models/sonic_magicwrist_vertices.inc"
};
static NJS_CNK_MODEL sonic_magicwrist_model = {
#include "shared/upgrade_models/sonic_magicwrist_model.inc"
};
NJS_CNK_OBJECT upgrade_sonic_magicwrist = {
#include "shared/upgrade_models/upgrade_sonic_magicwrist.inc"
};
static Sint16 shadow_flamering_child_polygons[45] = {
#include "shared/upgrade_models/shadow_flamering_child_polygons.inc"
};
static Sint32 shadow_flamering_child_vertices[45] = {
#include "shared/upgrade_models/shadow_flamering_child_vertices.inc"
};
static NJS_CNK_MODEL shadow_flamering_child_model = {
#include "shared/upgrade_models/shadow_flamering_child_model.inc"
};
static NJS_CNK_OBJECT shadow_flamering_child_object = {
#include "shared/upgrade_models/shadow_flamering_child_object.inc"
};
static Sint16 shadow_flamering_polygons[183] = {
#include "shared/upgrade_models/shadow_flamering_polygons.inc"
};
static Sint32 shadow_flamering_vertices[117] = {
#include "shared/upgrade_models/shadow_flamering_vertices.inc"
};
static NJS_CNK_MODEL shadow_flamering_model = {
#include "shared/upgrade_models/shadow_flamering_model.inc"
};
NJS_CNK_OBJECT upgrade_shadow_flamering = {
#include "shared/upgrade_models/upgrade_shadow_flamering.inc"
};
static Sint16 shadow_shoes_polygons[1657] = {
#include "shared/upgrade_models/shadow_shoes_polygons.inc"
};
static Sint32 shadow_shoes_vertices[1143] = {
#include "shared/upgrade_models/shadow_shoes_vertices.inc"
};
static NJS_CNK_MODEL shadow_shoes_model = {
#include "shared/upgrade_models/shadow_shoes_model.inc"
};
NJS_CNK_OBJECT upgrade_shadow_shoes = {
#include "shared/upgrade_models/upgrade_shadow_shoes.inc"
};
static Sint16 twalker_booster_polygons[763] = {
#include "shared/upgrade_models/twalker_booster_polygons.inc"
};
static Sint32 twalker_booster_vertices[627] = {
#include "shared/upgrade_models/twalker_booster_vertices.inc"
};
static NJS_CNK_MODEL twalker_booster_model = {
#include "shared/upgrade_models/twalker_booster_model.inc"
};
NJS_CNK_OBJECT upgrade_twalker_booster = {
#include "shared/upgrade_models/upgrade_twalker_booster.inc"
};
static Sint16 twalker_hypercannon_polygons[875] = {
#include "shared/upgrade_models/twalker_hypercannon_polygons.inc"
};
static Sint32 twalker_hypercannon_vertices[669] = {
#include "shared/upgrade_models/twalker_hypercannon_vertices.inc"
};
static NJS_CNK_MODEL twalker_hypercannon_model = {
#include "shared/upgrade_models/twalker_hypercannon_model.inc"
};
NJS_CNK_OBJECT upgrade_twalker_hypercannon = {
#include "shared/upgrade_models/upgrade_twalker_hypercannon.inc"
};
static Sint16 twalker_lazerbalster_child_polygons[127] = {
#include "shared/upgrade_models/twalker_lazerbalster_child_polygons.inc"
};
static Sint32 twalker_lazerbalster_child_vertices[147] = {
#include "shared/upgrade_models/twalker_lazerbalster_child_vertices.inc"
};
static NJS_CNK_MODEL twalker_lazerbalster_child_model = {
#include "shared/upgrade_models/twalker_lazerbalster_child_model.inc"
};
static NJS_CNK_OBJECT twalker_lazerbalster_child_object = {
#include "shared/upgrade_models/twalker_lazerbalster_child_object.inc"
};
static Sint16 twalker_lazerbalster_polygons[763] = {
#include "shared/upgrade_models/twalker_lazerbalster_polygons.inc"
};
static Sint32 twalker_lazerbalster_vertices[591] = {
#include "shared/upgrade_models/twalker_lazerbalster_vertices.inc"
};
static NJS_CNK_MODEL twalker_lazerbalster_model = {
#include "shared/upgrade_models/twalker_lazerbalster_model.inc"
};
NJS_CNK_OBJECT upgrade_twalker_lazerbalster = {
#include "shared/upgrade_models/upgrade_twalker_lazerbalster.inc"
};
static Sint16 ewalker_jetengine_polygons[887] = {
#include "shared/upgrade_models/ewalker_jetengine_polygons.inc"
};
static Sint32 ewalker_jetengine_vertices[627] = {
#include "shared/upgrade_models/ewalker_jetengine_vertices.inc"
};
static NJS_CNK_MODEL ewalker_jetengine_model = {
#include "shared/upgrade_models/ewalker_jetengine_model.inc"
};
NJS_CNK_OBJECT upgrade_ewalker_jetengine = {
#include "shared/upgrade_models/upgrade_ewalker_jetengine.inc"
};
static Sint16 ewalker_extrashield_polygons[523] = {
#include "shared/upgrade_models/ewalker_extrashield_polygons.inc"
};
static Sint32 ewalker_extrashield_vertices[465] = {
#include "shared/upgrade_models/ewalker_extrashield_vertices.inc"
};
static NJS_CNK_MODEL ewalker_extrashield_model = {
#include "shared/upgrade_models/ewalker_extrashield_model.inc"
};
NJS_CNK_OBJECT upgrade_ewalker_extrashield = {
#include "shared/upgrade_models/upgrade_ewalker_extrashield.inc"
};
static Sint16 ewalker_bazooka_polygons[617] = {
#include "shared/upgrade_models/ewalker_bazooka_polygons.inc"
};
static Sint32 ewalker_bazooka_vertices[477] = {
#include "shared/upgrade_models/ewalker_bazooka_vertices.inc"
};
static NJS_CNK_MODEL ewalker_bazooka_model = {
#include "shared/upgrade_models/ewalker_bazooka_model.inc"
};
NJS_CNK_OBJECT upgrade_ewalker_bazooka = {
#include "shared/upgrade_models/upgrade_ewalker_bazooka.inc"
};
static Sint16 ewalker_powergun_child_polygons[169] = {
#include "shared/upgrade_models/ewalker_powergun_child_polygons.inc"
};
static Sint32 ewalker_powergun_child_vertices[147] = {
#include "shared/upgrade_models/ewalker_powergun_child_vertices.inc"
};
static NJS_CNK_MODEL ewalker_powergun_child_model = {
#include "shared/upgrade_models/ewalker_powergun_child_model.inc"
};
static NJS_CNK_OBJECT ewalker_powergun_child_object = {
#include "shared/upgrade_models/ewalker_powergun_child_object.inc"
};
static Sint16 ewalker_powergun_polygons[1045] = {
#include "shared/upgrade_models/ewalker_powergun_polygons.inc"
};
static Sint32 ewalker_powergun_vertices[783] = {
#include "shared/upgrade_models/ewalker_powergun_vertices.inc"
};
static NJS_CNK_MODEL ewalker_powergun_model = {
#include "shared/upgrade_models/ewalker_powergun_model.inc"
};
NJS_CNK_OBJECT upgrade_ewalker_powergun = {
#include "shared/upgrade_models/upgrade_ewalker_powergun.inc"
};
static Sint16 knuckles_groves_polygons[990] = {
#include "shared/upgrade_models/knuckles_groves_polygons.inc"
};
static Sint32 knuckles_groves_vertices[507] = {
#include "shared/upgrade_models/knuckles_groves_vertices.inc"
};
static NJS_CNK_MODEL knuckles_groves_model = {
#include "shared/upgrade_models/knuckles_groves_model.inc"
};
NJS_CNK_OBJECT upgrade_knuckles_groves = {
#include "shared/upgrade_models/upgrade_knuckles_groves.inc"
};
static Sint16 knuckles_claws_polygons[257] = {
#include "shared/upgrade_models/knuckles_claws_polygons.inc"
};
static Sint32 knuckles_claws_vertices[351] = {
#include "shared/upgrade_models/knuckles_claws_vertices.inc"
};
static NJS_CNK_MODEL knuckles_claws_model = {
#include "shared/upgrade_models/knuckles_claws_model.inc"
};
NJS_CNK_OBJECT upgrade_knuckles_claws = {
#include "shared/upgrade_models/upgrade_knuckles_claws.inc"
};
static Sint16 knuckles_air_polygons[215] = {
#include "shared/upgrade_models/knuckles_air_polygons.inc"
};
static Sint32 knuckles_air_vertices[99] = {
#include "shared/upgrade_models/knuckles_air_vertices.inc"
};
static NJS_CNK_MODEL knuckles_air_model = {
#include "shared/upgrade_models/knuckles_air_model.inc"
};
NJS_CNK_OBJECT upgrade_knuckles_air = {
#include "shared/upgrade_models/upgrade_knuckles_air.inc"
};
static Sint16 knuckles_sunglass_polygons[187] = {
#include "shared/upgrade_models/knuckles_sunglass_polygons.inc"
};
static Sint32 knuckles_sunglass_vertices[243] = {
#include "shared/upgrade_models/knuckles_sunglass_vertices.inc"
};
static NJS_CNK_MODEL knuckles_sunglass_model = {
#include "shared/upgrade_models/knuckles_sunglass_model.inc"
};
NJS_CNK_OBJECT upgrade_knuckles_sunglass = {
#include "shared/upgrade_models/upgrade_knuckles_sunglass.inc"
};
static Sint16 rouge_scope_polygons[261] = {
#include "shared/upgrade_models/rouge_scope_polygons.inc"
};
static Sint32 rouge_scope_vertices[159] = {
#include "shared/upgrade_models/rouge_scope_vertices.inc"
};
static NJS_CNK_MODEL rouge_scope_model = {
#include "shared/upgrade_models/rouge_scope_model.inc"
};
NJS_CNK_OBJECT upgrade_rouge_scope = {
#include "shared/upgrade_models/upgrade_rouge_scope.inc"
};
static Sint16 rouge_nails_polygons[1431] = {
#include "shared/upgrade_models/rouge_nails_polygons.inc"
};
static Sint32 rouge_nails_vertices[1347] = {
#include "shared/upgrade_models/rouge_nails_vertices.inc"
};
static NJS_CNK_MODEL rouge_nails_model = {
#include "shared/upgrade_models/rouge_nails_model.inc"
};
NJS_CNK_OBJECT upgrade_rouge_nails = {
#include "shared/upgrade_models/upgrade_rouge_nails.inc"
};
static Sint16 rouge_boots_polygons[1591] = {
#include "shared/upgrade_models/rouge_boots_polygons.inc"
};
static Sint32 rouge_boots_vertices[1563] = {
#include "shared/upgrade_models/rouge_boots_vertices.inc"
};
static NJS_CNK_MODEL rouge_boots_model = {
#include "shared/upgrade_models/rouge_boots_model.inc"
};
NJS_CNK_OBJECT upgrade_rouge_boots = {
#include "shared/upgrade_models/upgrade_rouge_boots.inc"
};
