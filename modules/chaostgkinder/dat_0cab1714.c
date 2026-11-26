/* CHAOSTGKINDER.PRS's texture lists, chunk models and land, from the disc.
 * its D lies between dat_0caa7868.c and dat_0cac1504.c */
#include <ninja.h>
#include <sg_xpt.h>

#include "chaostgkinder/land_data.h"
#include "chaostgkinder/land_textures.h"
#include "collision/world.h"

static NJS_TEXNAME library_textures[46] = {
#include "chaostgkinder/dat_0cab1714/library_textures.inc"
};
NJS_TEXLIST kinder_library_texlist = {
#include "chaostgkinder/dat_0cab1714/kinder_library_texlist.inc"
};
static Sint16 library_bench_polygons[761] = {
#include "chaostgkinder/dat_0cab1714/library_bench_polygons.inc"
};
static Sint32 library_bench_vertices[419] = {
#include "chaostgkinder/dat_0cab1714/library_bench_vertices.inc"
};
static NJS_CNK_MODEL library_bench_model = {
#include "chaostgkinder/dat_0cab1714/library_bench_model.inc"
};
static NJS_CNK_OBJECT library_bench = {
#include "chaostgkinder/dat_0cab1714/library_bench.inc"
};
static Sint16 library_albums_polygons[435] = {
#include "chaostgkinder/dat_0cab1714/library_albums_polygons.inc"
};
static Sint32 library_albums_vertices[399] = {
#include "chaostgkinder/dat_0cab1714/library_albums_vertices.inc"
};
static NJS_CNK_MODEL library_albums_model = {
#include "chaostgkinder/dat_0cab1714/library_albums_model.inc"
};
static NJS_CNK_OBJECT library_albums = {
#include "chaostgkinder/dat_0cab1714/library_albums.inc"
};
static Sint16 library_back_bookshelf_polygons[639] = {
#include "chaostgkinder/dat_0cab1714/library_back_bookshelf_polygons.inc"
};
static Sint32 library_back_bookshelf_vertices[591] = {
#include "chaostgkinder/dat_0cab1714/library_back_bookshelf_vertices.inc"
};
static NJS_CNK_MODEL library_back_bookshelf_model = {
#include "chaostgkinder/dat_0cab1714/library_back_bookshelf_model.inc"
};
static NJS_CNK_OBJECT library_back_bookshelf = {
#include "chaostgkinder/dat_0cab1714/library_back_bookshelf.inc"
};
static Sint16 library_side_bookshelf_polygons[611] = {
#include "chaostgkinder/dat_0cab1714/library_side_bookshelf_polygons.inc"
};
static Sint32 library_side_bookshelf_vertices[555] = {
#include "chaostgkinder/dat_0cab1714/library_side_bookshelf_vertices.inc"
};
static NJS_CNK_MODEL library_side_bookshelf_model = {
#include "chaostgkinder/dat_0cab1714/library_side_bookshelf_model.inc"
};
static NJS_CNK_OBJECT library_side_bookshelf = {
#include "chaostgkinder/dat_0cab1714/library_side_bookshelf.inc"
};
static Sint16 library_curtain_trim_polygons[39] = {
#include "chaostgkinder/dat_0cab1714/library_curtain_trim_polygons.inc"
};
static Sint32 library_curtain_trim_vertices[35] = {
#include "chaostgkinder/dat_0cab1714/library_curtain_trim_vertices.inc"
};
static NJS_CNK_MODEL library_curtain_trim_model = {
#include "chaostgkinder/dat_0cab1714/library_curtain_trim_model.inc"
};
static NJS_CNK_OBJECT library_curtain_trim = {
#include "chaostgkinder/dat_0cab1714/library_curtain_trim.inc"
};
static Sint16 library_curtain_polygons[391] = {
#include "chaostgkinder/dat_0cab1714/library_curtain_polygons.inc"
};
static Sint32 library_curtain_vertices[327] = {
#include "chaostgkinder/dat_0cab1714/library_curtain_vertices.inc"
};
static NJS_CNK_MODEL library_curtain_model = {
#include "chaostgkinder/dat_0cab1714/library_curtain_model.inc"
};
static NJS_CNK_OBJECT library_curtain = {
#include "chaostgkinder/dat_0cab1714/library_curtain.inc"
};
static Sint16 library_writing_desk_polygons[203] = {
#include "chaostgkinder/dat_0cab1714/library_writing_desk_polygons.inc"
};
static Sint32 library_writing_desk_vertices[171] = {
#include "chaostgkinder/dat_0cab1714/library_writing_desk_vertices.inc"
};
static NJS_CNK_MODEL library_writing_desk_model = {
#include "chaostgkinder/dat_0cab1714/library_writing_desk_model.inc"
};
static NJS_CNK_OBJECT library_writing_desk = {
#include "chaostgkinder/dat_0cab1714/library_writing_desk.inc"
};
static Sint16 library_carpet_polygons[131] = {
#include "chaostgkinder/dat_0cab1714/library_carpet_polygons.inc"
};
static Sint32 library_carpet_vertices[99] = {
#include "chaostgkinder/dat_0cab1714/library_carpet_vertices.inc"
};
static NJS_CNK_MODEL library_carpet_model = {
#include "chaostgkinder/dat_0cab1714/library_carpet_model.inc"
};
static NJS_CNK_OBJECT library_carpet = {
#include "chaostgkinder/dat_0cab1714/library_carpet.inc"
};
static Sint16 library_computer_and_books_polygons[1505] = {
#include "chaostgkinder/dat_0cab1714/library_computer_and_books_polygons.inc"
};
static Sint32 library_computer_and_books_vertices[1119] = {
#include "chaostgkinder/dat_0cab1714/library_computer_and_books_vertices.inc"
};
static NJS_CNK_MODEL library_computer_and_books_model = {
#include "chaostgkinder/dat_0cab1714/library_computer_and_books_model.inc"
};
static NJS_CNK_OBJECT library_computer_and_books = {
#include "chaostgkinder/dat_0cab1714/library_computer_and_books.inc"
};
static Sint16 library_left_statue_head_polygons[1323] = {
#include "chaostgkinder/dat_0cab1714/library_left_statue_head_polygons.inc"
};
static Sint32 library_left_statue_head_vertices[847] = {
#include "chaostgkinder/dat_0cab1714/library_left_statue_head_vertices.inc"
};
static NJS_CNK_MODEL library_left_statue_head_model = {
#include "chaostgkinder/dat_0cab1714/library_left_statue_head_model.inc"
};
static NJS_CNK_OBJECT library_left_statue_head = {
#include "chaostgkinder/dat_0cab1714/library_left_statue_head.inc"
};
static Sint16 library_right_statue_head_polygons[1337] = {
#include "chaostgkinder/dat_0cab1714/library_right_statue_head_polygons.inc"
};
static Sint32 library_right_statue_head_vertices[823] = {
#include "chaostgkinder/dat_0cab1714/library_right_statue_head_vertices.inc"
};
static NJS_CNK_MODEL library_right_statue_head_model = {
#include "chaostgkinder/dat_0cab1714/library_right_statue_head_model.inc"
};
static NJS_CNK_OBJECT library_right_statue_head = {
#include "chaostgkinder/dat_0cab1714/library_right_statue_head.inc"
};
static Sint16 library_ceiling_light_polygons[389] = {
#include "chaostgkinder/dat_0cab1714/library_ceiling_light_polygons.inc"
};
static Sint32 library_ceiling_light_vertices[227] = {
#include "chaostgkinder/dat_0cab1714/library_ceiling_light_vertices.inc"
};
static NJS_CNK_MODEL library_ceiling_light_model = {
#include "chaostgkinder/dat_0cab1714/library_ceiling_light_model.inc"
};
static NJS_CNK_OBJECT library_ceiling_light = {
#include "chaostgkinder/dat_0cab1714/library_ceiling_light.inc"
};
static Sint16 library_album_stool_polygons[397] = {
#include "chaostgkinder/dat_0cab1714/library_album_stool_polygons.inc"
};
static Sint32 library_album_stool_vertices[415] = {
#include "chaostgkinder/dat_0cab1714/library_album_stool_vertices.inc"
};
static NJS_CNK_MODEL library_album_stool_model = {
#include "chaostgkinder/dat_0cab1714/library_album_stool_model.inc"
};
static NJS_CNK_OBJECT library_album_stool = {
#include "chaostgkinder/dat_0cab1714/library_album_stool.inc"
};
static Sint16 library_bookshelf_ladder_polygons[811] = {
#include "chaostgkinder/dat_0cab1714/library_bookshelf_ladder_polygons.inc"
};
static Sint32 library_bookshelf_ladder_vertices[779] = {
#include "chaostgkinder/dat_0cab1714/library_bookshelf_ladder_vertices.inc"
};
static NJS_CNK_MODEL library_bookshelf_ladder_model = {
#include "chaostgkinder/dat_0cab1714/library_bookshelf_ladder_model.inc"
};
static NJS_CNK_OBJECT library_bookshelf_ladder = {
#include "chaostgkinder/dat_0cab1714/library_bookshelf_ladder.inc"
};
static Sint16 library_flowers_polygons[1033] = {
#include "chaostgkinder/dat_0cab1714/library_flowers_polygons.inc"
};
static Sint32 library_flowers_vertices[935] = {
#include "chaostgkinder/dat_0cab1714/library_flowers_vertices.inc"
};
static NJS_CNK_MODEL library_flowers_model = {
#include "chaostgkinder/dat_0cab1714/library_flowers_model.inc"
};
static NJS_CNK_OBJECT library_flowers = {
#include "chaostgkinder/dat_0cab1714/library_flowers.inc"
};
static Sint16 library_walls_polygons[723] = {
#include "chaostgkinder/dat_0cab1714/library_walls_polygons.inc"
};
static Sint32 library_walls_vertices[707] = {
#include "chaostgkinder/dat_0cab1714/library_walls_vertices.inc"
};
static NJS_CNK_MODEL library_walls_model = {
#include "chaostgkinder/dat_0cab1714/library_walls_model.inc"
};
static NJS_CNK_OBJECT library_walls = {
#include "chaostgkinder/dat_0cab1714/library_walls.inc"
};
static Sint16 library_window_polygons[351] = {
#include "chaostgkinder/dat_0cab1714/library_window_polygons.inc"
};
static Sint32 library_window_vertices[367] = {
#include "chaostgkinder/dat_0cab1714/library_window_vertices.inc"
};
static NJS_CNK_MODEL library_window_model = {
#include "chaostgkinder/dat_0cab1714/library_window_model.inc"
};
static NJS_CNK_OBJECT library_window = {
#include "chaostgkinder/dat_0cab1714/library_window.inc"
};
static Sint16 library_left_statue_body_polygons[511] = {
#include "chaostgkinder/dat_0cab1714/library_left_statue_body_polygons.inc"
};
static Sint32 library_left_statue_body_vertices[375] = {
#include "chaostgkinder/dat_0cab1714/library_left_statue_body_vertices.inc"
};
static NJS_CNK_MODEL library_left_statue_body_model = {
#include "chaostgkinder/dat_0cab1714/library_left_statue_body_model.inc"
};
static NJS_CNK_OBJECT library_left_statue_body = {
#include "chaostgkinder/dat_0cab1714/library_left_statue_body.inc"
};
static Sint16 library_right_statue_body_polygons[483] = {
#include "chaostgkinder/dat_0cab1714/library_right_statue_body_polygons.inc"
};
static Sint32 library_right_statue_body_vertices[355] = {
#include "chaostgkinder/dat_0cab1714/library_right_statue_body_vertices.inc"
};
static NJS_CNK_MODEL library_right_statue_body_model = {
#include "chaostgkinder/dat_0cab1714/library_right_statue_body_model.inc"
};
static NJS_CNK_OBJECT library_right_statue_body = {
#include "chaostgkinder/dat_0cab1714/library_right_statue_body.inc"
};
struct land_entry kinder_library_entries[19] = {
#include "chaostgkinder/dat_0cab1714/kinder_library_entries.inc"
};
struct land_table kinder_library_land = {
#include "chaostgkinder/dat_0cab1714/kinder_library_land.inc"
};
