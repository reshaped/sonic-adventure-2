/* CHAOSTGKINDER.PRS's texture lists, chunk models and land, from the disc.
 * its D lies between dat_0cab1714.c and form_field.c */
#include <ninja.h>
#include <sg_xpt.h>

#include "chaostgkinder/land_data.h"
#include "chaostgkinder/land_textures.h"
#include "collision/world.h"

static NJS_TEXNAME principal_textures[46] = {
#include "chaostgkinder/dat_0cac1504/principal_textures.inc"
};
NJS_TEXLIST kinder_principal_texlist = {
#include "chaostgkinder/dat_0cac1504/kinder_principal_texlist.inc"
};
static Sint16 principal_shelf_cup_and_books_polygons[365] = {
#include "chaostgkinder/dat_0cac1504/principal_shelf_cup_and_books_polygons.inc"
};
static Sint32 principal_shelf_cup_and_books_vertices[271] = {
#include "chaostgkinder/dat_0cac1504/principal_shelf_cup_and_books_vertices.inc"
};
static NJS_CNK_MODEL principal_shelf_cup_and_books_model = {
#include "chaostgkinder/dat_0cac1504/principal_shelf_cup_and_books_model.inc"
};
static NJS_CNK_OBJECT principal_shelf_cup_and_books = {
#include "chaostgkinder/dat_0cac1504/principal_shelf_cup_and_books.inc"
};
static Sint16 principal_ribboned_award_polygons[315] = {
#include "chaostgkinder/dat_0cac1504/principal_ribboned_award_polygons.inc"
};
static Sint32 principal_ribboned_award_vertices[939] = {
#include "chaostgkinder/dat_0cac1504/principal_ribboned_award_vertices.inc"
};
static NJS_CNK_MODEL principal_ribboned_award_model = {
#include "chaostgkinder/dat_0cac1504/principal_ribboned_award_model.inc"
};
static NJS_CNK_OBJECT principal_ribboned_award = {
#include "chaostgkinder/dat_0cac1504/principal_ribboned_award.inc"
};
static Sint16 principal_curtain_polygons[355] = {
#include "chaostgkinder/dat_0cac1504/principal_curtain_polygons.inc"
};
static Sint32 principal_curtain_vertices[267] = {
#include "chaostgkinder/dat_0cac1504/principal_curtain_vertices.inc"
};
static NJS_CNK_MODEL principal_curtain_model = {
#include "chaostgkinder/dat_0cac1504/principal_curtain_model.inc"
};
static NJS_CNK_OBJECT principal_curtain = {
#include "chaostgkinder/dat_0cac1504/principal_curtain.inc"
};
static Sint16 principal_desk_polygons[205] = {
#include "chaostgkinder/dat_0cac1504/principal_desk_polygons.inc"
};
static Sint32 principal_desk_vertices[175] = {
#include "chaostgkinder/dat_0cac1504/principal_desk_vertices.inc"
};
static NJS_CNK_MODEL principal_desk_model = {
#include "chaostgkinder/dat_0cac1504/principal_desk_model.inc"
};
static NJS_CNK_OBJECT principal_desk = {
#include "chaostgkinder/dat_0cac1504/principal_desk.inc"
};
static Sint16 principal_carpet_polygons[131] = {
#include "chaostgkinder/dat_0cac1504/principal_carpet_polygons.inc"
};
static Sint32 principal_carpet_vertices[99] = {
#include "chaostgkinder/dat_0cac1504/principal_carpet_vertices.inc"
};
static NJS_CNK_MODEL principal_carpet_model = {
#include "chaostgkinder/dat_0cac1504/principal_carpet_model.inc"
};
static NJS_CNK_OBJECT principal_carpet = {
#include "chaostgkinder/dat_0cac1504/principal_carpet.inc"
};
static Sint16 principal_desktop_accessories_polygons[1879] = {
#include "chaostgkinder/dat_0cac1504/principal_desktop_accessories_polygons.inc"
};
static Sint32 principal_desktop_accessories_vertices[1427] = {
#include "chaostgkinder/dat_0cac1504/principal_desktop_accessories_vertices.inc"
};
static NJS_CNK_MODEL principal_desktop_accessories_model = {
#include "chaostgkinder/dat_0cac1504/principal_desktop_accessories_model.inc"
};
static NJS_CNK_OBJECT principal_desktop_accessories = {
#include "chaostgkinder/dat_0cac1504/principal_desktop_accessories.inc"
};
static Sint16 principal_ceiling_lamp_polygons[987] = {
#include "chaostgkinder/dat_0cac1504/principal_ceiling_lamp_polygons.inc"
};
static Sint32 principal_ceiling_lamp_vertices[1377] = {
#include "chaostgkinder/dat_0cac1504/principal_ceiling_lamp_vertices.inc"
};
static NJS_CNK_MODEL principal_ceiling_lamp_model = {
#include "chaostgkinder/dat_0cac1504/principal_ceiling_lamp_model.inc"
};
static NJS_CNK_OBJECT principal_ceiling_lamp = {
#include "chaostgkinder/dat_0cac1504/principal_ceiling_lamp.inc"
};
static Sint16 principal_back_wall_photos_polygons[655] = {
#include "chaostgkinder/dat_0cac1504/principal_back_wall_photos_polygons.inc"
};
static Sint32 principal_back_wall_photos_vertices[631] = {
#include "chaostgkinder/dat_0cac1504/principal_back_wall_photos_vertices.inc"
};
static NJS_CNK_MODEL principal_back_wall_photos_model = {
#include "chaostgkinder/dat_0cac1504/principal_back_wall_photos_model.inc"
};
static NJS_CNK_OBJECT principal_back_wall_photos = {
#include "chaostgkinder/dat_0cac1504/principal_back_wall_photos.inc"
};
static Sint16 principal_side_wall_photos_polygons[603] = {
#include "chaostgkinder/dat_0cac1504/principal_side_wall_photos_polygons.inc"
};
static Sint32 principal_side_wall_photos_vertices[563] = {
#include "chaostgkinder/dat_0cac1504/principal_side_wall_photos_vertices.inc"
};
static NJS_CNK_MODEL principal_side_wall_photos_model = {
#include "chaostgkinder/dat_0cac1504/principal_side_wall_photos_model.inc"
};
static NJS_CNK_OBJECT principal_side_wall_photos = {
#include "chaostgkinder/dat_0cac1504/principal_side_wall_photos.inc"
};
static Sint16 principal_bookshelves_polygons[361] = {
#include "chaostgkinder/dat_0cac1504/principal_bookshelves_polygons.inc"
};
static Sint32 principal_bookshelves_vertices[343] = {
#include "chaostgkinder/dat_0cac1504/principal_bookshelves_vertices.inc"
};
static NJS_CNK_MODEL principal_bookshelves_model = {
#include "chaostgkinder/dat_0cac1504/principal_bookshelves_model.inc"
};
static NJS_CNK_OBJECT principal_bookshelves = {
#include "chaostgkinder/dat_0cac1504/principal_bookshelves.inc"
};
static Sint16 principal_walls_polygons[655] = {
#include "chaostgkinder/dat_0cac1504/principal_walls_polygons.inc"
};
static Sint32 principal_walls_vertices[463] = {
#include "chaostgkinder/dat_0cac1504/principal_walls_vertices.inc"
};
static NJS_CNK_MODEL principal_walls_model = {
#include "chaostgkinder/dat_0cac1504/principal_walls_model.inc"
};
static NJS_CNK_OBJECT principal_walls = {
#include "chaostgkinder/dat_0cac1504/principal_walls.inc"
};
static Sint16 principal_window_polygons[345] = {
#include "chaostgkinder/dat_0cac1504/principal_window_polygons.inc"
};
static Sint32 principal_window_vertices[363] = {
#include "chaostgkinder/dat_0cac1504/principal_window_vertices.inc"
};
static NJS_CNK_MODEL principal_window_model = {
#include "chaostgkinder/dat_0cac1504/principal_window_model.inc"
};
static NJS_CNK_OBJECT principal_window = {
#include "chaostgkinder/dat_0cac1504/principal_window.inc"
};
static Sint16 principal_statue_plinth_polygons[471] = {
#include "chaostgkinder/dat_0cac1504/principal_statue_plinth_polygons.inc"
};
static Sint32 principal_statue_plinth_vertices[343] = {
#include "chaostgkinder/dat_0cac1504/principal_statue_plinth_vertices.inc"
};
static NJS_CNK_MODEL principal_statue_plinth_model = {
#include "chaostgkinder/dat_0cac1504/principal_statue_plinth_model.inc"
};
static NJS_CNK_OBJECT principal_statue_plinth = {
#include "chaostgkinder/dat_0cac1504/principal_statue_plinth.inc"
};
struct land_entry kinder_principal_entries[13] = {
#include "chaostgkinder/dat_0cac1504/kinder_principal_entries.inc"
};
struct land_table kinder_principal_land = {
#include "chaostgkinder/dat_0cac1504/kinder_principal_land.inc"
};
