#ifndef ADVERTISE_TUTORIAL_ART_H
#define ADVERTISE_TUTORIAL_ART_H
#include <ninja.h>
#include <sg_xpt.h>
#include "advertise/tutorial_panel.h"
#include "draw/chunk_polygon.h"

enum { TUTORIAL_CELLS = 24 };

struct tutorial_picture {
    Sint32 unk_00;
    NJS_TEXLIST *texlist;
};

/* the picture's polygon list. tutorial_board_set_picture writes the texture id
 * of the picture shown into the low byte of texture.texture */
struct tutorial_picture_polygons {
    struct part_material_diffuse_ambient material;
    struct part_texture texture;
    struct part_strip_header strips;
    Sint16 length;
    struct part_uv strip[4];
    Uint16 end;
};

/* @hack type: the picture's polygon list declared as an array of one, where the
 * list alone would do */
extern struct tutorial_picture_polygons tutorial_picture_plist[1];
/* @hack type: the camera's motion declared as an array of one, where the motion
 * alone would do */
extern NJS_MOTION tutorial_camera_motion[1];
extern NJS_CNK_OBJECT tutorial_board_object;
extern NJS_CNK_OBJECT tutorial_picture_object;
extern NJS_CNK_OBJECT tutorial_model_object;
extern NJS_CNK_OBJECT tutorial_spin_y_object;
extern NJS_CNK_OBJECT tutorial_spin_z_object;
extern struct tutorial_picture tutorial_pictures[TUTORIAL_LESSONS];
extern NJS_TEXANIM tutorial_cells[TUTORIAL_CELLS];

extern NJS_TEXLIST tutorial_kihon_texlist;
extern NJS_TEXLIST tutorial_sonic_texlist;
extern NJS_TEXLIST tutorial_tails_texlist;
extern NJS_TEXLIST tutorial_knuckles_texlist;
extern NJS_TEXLIST tutorial_chao_texlist;

#endif
