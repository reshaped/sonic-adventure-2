/* what CHAO.PRS provides its garden areas */
#ifndef CHAO_GARDEN_SCENE_H
#define CHAO_GARDEN_SCENE_H
#include <ninja.h>
#include <sg_xpt.h>

struct set_head;
struct task;

extern NJS_TEXLIST neutral_garden_texlist;
extern NJS_TEXLIST hero_garden_texlist;
extern NJS_TEXLIST dark_garden_texlist;

extern NJS_TEXLIST garden_effect_texlist;

struct garden_water {
    NJS_CNK_OBJECT *object;
    NJS_TEXLIST *texlist;
    Sint32 first_texture;
    Sint32 texture_count;
    Float start_frame;
    Float frame;
    Float speed;
    Sint32 scroll;
    Float du, dv;
};

/* create a water task for each entry of surfaces, up to the one whose object
 * is null. each task keeps its entry, and writes its frame */
void garden_water_create(struct garden_water *surfaces);
void garden_camera_start(void);
struct task *garden_camera_create(void);
int garden_control_start(void);
void garden_animals_start(void);
/* Read a garden SET file and return its heap copy, or NULL on failure.
 * The port checks the complete transfer and entry count. In the original,
 * only whole 32-byte blocks are copied. Both reserve at least 16 KiB and
 * the caller releases the returned block with loader_free. */
struct set_head *garden_file_load(const char *name);
void garden_transfer_machine_create(NJS_POINT3 *position, Angle yaw);
void garden_ball_create(NJS_POINT3 *position, NJS_VECTOR *velocity);
void garden_television_create(NJS_POINT3 *position, Angle yaw);
void garden_jack_in_box_create(NJS_POINT3 *position, Angle yaw);
void garden_rocking_horse_create(NJS_POINT3 *position, Angle yaw);
void garden_radio_create(NJS_POINT3 *position, Angle yaw);

#endif
