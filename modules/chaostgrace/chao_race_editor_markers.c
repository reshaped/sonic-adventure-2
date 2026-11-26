/* CHAOSTGRACE.PRS's chunk models, from the disc. its D lies between script.c
 * and edit_menu.c */
#include <ninja.h>
#include <sg_xpt.h>

#include "chaostgrace/edit_menu.h"

static Sint16 editor_markers_reference_bar_polygons[41] = {
#include "chaostgrace/chao_race_editor_markers/editor_markers_reference_bar_polygons.inc"
};
static Sint32 editor_markers_reference_bar_vertices[147] = {
#include "chaostgrace/chao_race_editor_markers/editor_markers_reference_bar_vertices.inc"
};
static NJS_CNK_MODEL editor_markers_reference_bar_model = {
#include "chaostgrace/chao_race_editor_markers/editor_markers_reference_bar_model.inc"
};
static NJS_CNK_OBJECT editor_markers_reference_bar = {
#include "chaostgrace/chao_race_editor_markers/editor_markers_reference_bar.inc"
};
static Sint16 editor_markers_reference_symbol_polygons[55] = {
#include "chaostgrace/chao_race_editor_markers/editor_markers_reference_symbol_polygons.inc"
};
static Sint32 editor_markers_reference_symbol_vertices[219] = {
#include "chaostgrace/chao_race_editor_markers/editor_markers_reference_symbol_vertices.inc"
};
static NJS_CNK_MODEL editor_markers_reference_symbol_model = {
#include "chaostgrace/chao_race_editor_markers/editor_markers_reference_symbol_model.inc"
};
static NJS_CNK_OBJECT editor_markers_reference_symbol = {
#include "chaostgrace/chao_race_editor_markers/editor_markers_reference_symbol.inc"
};
static Sint16 editor_markers_eye_marker_polygons[67] = {
#include "chaostgrace/chao_race_editor_markers/editor_markers_eye_marker_polygons.inc"
};
static Sint32 editor_markers_eye_marker_vertices[249] = {
#include "chaostgrace/chao_race_editor_markers/editor_markers_eye_marker_vertices.inc"
};
static NJS_CNK_MODEL editor_markers_eye_marker_model = {
#include "chaostgrace/chao_race_editor_markers/editor_markers_eye_marker_model.inc"
};
NJS_CNK_OBJECT race_eye_marker = {
#include "chaostgrace/chao_race_editor_markers/race_eye_marker.inc"
};
static Sint16 editor_markers_chase_marker_polygons[79] = {
#include "chaostgrace/chao_race_editor_markers/editor_markers_chase_marker_polygons.inc"
};
static Sint32 editor_markers_chase_marker_vertices[159] = {
#include "chaostgrace/chao_race_editor_markers/editor_markers_chase_marker_vertices.inc"
};
static NJS_CNK_MODEL editor_markers_chase_marker_model = {
#include "chaostgrace/chao_race_editor_markers/editor_markers_chase_marker_model.inc"
};
NJS_CNK_OBJECT race_chase_marker = {
#include "chaostgrace/chao_race_editor_markers/race_chase_marker.inc"
};
static Sint16 editor_markers_fix_marker_polygons[41] = {
#include "chaostgrace/chao_race_editor_markers/editor_markers_fix_marker_polygons.inc"
};
static Sint32 editor_markers_fix_marker_vertices[147] = {
#include "chaostgrace/chao_race_editor_markers/editor_markers_fix_marker_vertices.inc"
};
static NJS_CNK_MODEL editor_markers_fix_marker_model = {
#include "chaostgrace/chao_race_editor_markers/editor_markers_fix_marker_model.inc"
};
NJS_CNK_OBJECT race_fix_marker = {
#include "chaostgrace/chao_race_editor_markers/race_fix_marker.inc"
};
static Sint16 editor_markers_move_marker_polygons[27] = {
#include "chaostgrace/chao_race_editor_markers/editor_markers_move_marker_polygons.inc"
};
static Sint32 editor_markers_move_marker_vertices[75] = {
#include "chaostgrace/chao_race_editor_markers/editor_markers_move_marker_vertices.inc"
};
static NJS_CNK_MODEL editor_markers_move_marker_model = {
#include "chaostgrace/chao_race_editor_markers/editor_markers_move_marker_model.inc"
};
NJS_CNK_OBJECT race_move_marker = {
#include "chaostgrace/chao_race_editor_markers/race_move_marker.inc"
};
