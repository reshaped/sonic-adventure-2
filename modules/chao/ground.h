/* the floor under a garden mover, and the editor's height map. a mover's
 * floor comes from the stage's collision, the height map or a race course */
#ifndef CHAO_GROUND_H
#define CHAO_GROUND_H
#include <ninja.h>
#include <sg_xpt.h>
struct task;
struct angles;

/* the height map's samples along x, and along z */
#define GARDEN_HEIGHT_SIDE 1
extern Uint16 garden_height_map[GARDEN_HEIGHT_SIDE * GARDEN_HEIGHT_SIDE];

/* how chao_meet_floor finds a Chao's floor */
enum chao_ground_mode {
    CHAO_GROUND_COLLISION,
    CHAO_GROUND_HEIGHT_MAP,
    CHAO_GROUND_RACE
};

/* garden_floor_below's result where no surface is */
#define GARDEN_NO_NEARBY_FLOOR (-123400.0F)

/* the highest surface at or under position among the world's objects
 * within 10 of it, or GARDEN_NO_NEARBY_FLOOR */
Float garden_floor_below(NJS_POINT3 *position);
/* the highest surface at or under (x, y, z) among the world's solid objects
 * within 300. where none is, the result is NO_FLOOR. angles, unless NULL,
 * takes the surface's tilt about x and z. where there's no surface, angles
 * is undefined, and the GCC side leaves it as it was */
Float garden_floor_angles(Float x, Float y, Float z, struct angles *angles);
/* sample the floor into the height map */
void garden_build_height_map(void);
/* set height->y to the map's height at (x, z), between its four nearest
 * samples */
void garden_sample_height_map(Float x, Float z, NJS_POINT2 *height);
/* draw a sample of the map as a short line */
void garden_draw_height_map(void);
Float garden_map_floor(NJS_POINT3 *position);
/* land the mover of a Chao or of another garden object on its floor. the
 * mover takes its garden's water height. the result is TRUE for a Chao landed
 * on the height map, and FALSE for any other mover. for a resting mover the
 * result is undefined */
int chao_meet_floor(struct task *chao);
int garden_meet_floor(struct task *object);
void chao_set_ground_mode(struct task *chao, int mode);

#endif
