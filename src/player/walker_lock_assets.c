/* the mechs' lock-on assets, which walker_lock_assets.h declares */
#include <ninja.h>
#include <sg_xpt.h>

#include "collision/collision.h"
#include "draw/texture_anim.h"
#include "player/walker_lock_assets.h"

struct collision_shape walker_lock_shot_shape = {
#include "player/walker_lock_assets/walker_lock_shot_shape.inc"
};

#define PART_VERTICES 27

struct blast_part {
    NJS_CNK_MODEL model;
    NJS_CNK_OBJECT object;
};

/* Each mech draws four blast meshes and changes the alpha of its own vertices.
 */
static Sint16 eggman_polygons_a[49] = {
#include "player/walker_lock_assets/eggman_polygons_a.inc"
};
static Sint32 eggman_vertices_a[111] = {
#include "player/walker_lock_assets/eggman_vertices_a.inc"
};
static struct blast_part eggman_part_a = {
#include "player/walker_lock_assets/eggman_part_a.inc"
};
static Sint16 eggman_polygons_b[49] = {
#include "player/walker_lock_assets/eggman_polygons_b.inc"
};
static Sint32 eggman_vertices_b[111] = {
#include "player/walker_lock_assets/eggman_vertices_b.inc"
};
static struct blast_part eggman_part_b = {
#include "player/walker_lock_assets/eggman_part_b.inc"
};
static Sint16 eggman_polygons_c[49] = {
#include "player/walker_lock_assets/eggman_polygons_c.inc"
};
static Sint32 eggman_vertices_c[111] = {
#include "player/walker_lock_assets/eggman_vertices_c.inc"
};
static struct blast_part eggman_part_c = {
#include "player/walker_lock_assets/eggman_part_c.inc"
};
static Sint16 eggman_polygons_d[51] = {
#include "player/walker_lock_assets/eggman_polygons_d.inc"
};
static Sint32 eggman_vertices_d[111] = {
#include "player/walker_lock_assets/eggman_vertices_d.inc"
};
static struct blast_part eggman_part_d = {
#include "player/walker_lock_assets/eggman_part_d.inc"
};
static Sint16 tails_polygons_a[49] = {
#include "player/walker_lock_assets/tails_polygons_a.inc"
};
static Sint32 tails_vertices_a[111] = {
#include "player/walker_lock_assets/tails_vertices_a.inc"
};
static struct blast_part tails_part_a = {
#include "player/walker_lock_assets/tails_part_a.inc"
};
static Sint16 tails_polygons_b[49] = {
#include "player/walker_lock_assets/tails_polygons_b.inc"
};
static Sint32 tails_vertices_b[111] = {
#include "player/walker_lock_assets/tails_vertices_b.inc"
};
static struct blast_part tails_part_b = {
#include "player/walker_lock_assets/tails_part_b.inc"
};
static Sint16 tails_polygons_c[49] = {
#include "player/walker_lock_assets/tails_polygons_c.inc"
};
static Sint32 tails_vertices_c[111] = {
#include "player/walker_lock_assets/tails_vertices_c.inc"
};
static struct blast_part tails_part_c = {
#include "player/walker_lock_assets/tails_part_c.inc"
};
static Sint16 tails_polygons_d[51] = {
#include "player/walker_lock_assets/tails_polygons_d.inc"
};
static Sint32 tails_vertices_d[111] = {
#include "player/walker_lock_assets/tails_vertices_d.inc"
};
static struct blast_part tails_part_d = {
#include "player/walker_lock_assets/tails_part_d.inc"
};

NJS_CNK_MODEL *walker_blast_parts[8] = {
#include "player/walker_lock_assets/walker_blast_parts.inc"
};

/* each cell draws a whole texture, 64 by 64, centered on the sprite's point */
static NJS_TEXANIM shot_cells[3] = {
#include "player/walker_lock_assets/shot_cells.inc"
};
NJS_SPRITE walker_shot_sprite[1] = {
#include "player/walker_lock_assets/walker_shot_sprite.inc"
};
NJS_TEXANIM walker_lock_cells[3] = {
#include "player/walker_lock_assets/walker_lock_cells.inc"
};
NJS_SPRITE walker_lock_marker = {
#include "player/walker_lock_assets/walker_lock_marker.inc"
};
Float walker_lock_scales[LOCK_SCALES] = {1.0F, 3.0F, 2.0F, 1.7F,
                                         1.4F, 1.2F, 1.1F, 1.0F};

/* the uvs of the beam's three models, one key each, which laser_anim_a,
 * laser_anim_b and laser_anim_c scroll. walker_weapons.c applies each
 * animation to its model before it draws it */
static Sint16 laser_a_uvs[180] = {
#include "player/walker_lock_assets/laser_a_uvs.inc"
};
struct texture_anim laser_anim_a = {
#include "player/walker_lock_assets/laser_anim_a.inc"
};

static Sint16 laser_b_uvs[168] = {
#include "player/walker_lock_assets/laser_b_uvs.inc"
};
struct texture_anim laser_anim_b = {
#include "player/walker_lock_assets/laser_anim_b.inc"
};

static Sint16 laser_c_uvs[308] = {
#include "player/walker_lock_assets/laser_c_uvs.inc"
};
struct texture_anim laser_anim_c = {
#include "player/walker_lock_assets/laser_anim_c.inc"
};
