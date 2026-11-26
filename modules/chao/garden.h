/* the Chao world's stage and its areas, and a texture list and calls that
 * other Chao sources define */
#ifndef CHAO_GARDEN_H
#define CHAO_GARDEN_H
#include <ninja.h>
#include <sg_xpt.h>

struct task;

/* the Chao world's areas. CHAO_AREA_NEUTRAL, _HERO and _DARK are the
 * gardens */
enum chao_area {
    CHAO_AREA_NONE = -1,
    CHAO_AREA_NEUTRAL = 1,
    CHAO_AREA_HERO = 2,
    CHAO_AREA_DARK = 3,
    CHAO_AREA_RACE = 4,
    CHAO_AREA_ENTRANCE = 5,
    CHAO_AREA_KINDERGARTEN = 6,
    CHAO_AREA_LOBBY = 7,
    /* CHAO_AREA_ENTRANCE's stage, as the first area. chao_init starts there,
     * not in the lobby, when a menu opened the Chao world for its races, as
     * game_state.chao_race_menu says */
    CHAO_AREA_RACE_ENTRY = 8
};

extern NJS_TEXLIST chao_common_texlist;

void chao_init(void);
void chao_exit(void);
void chao_step(struct task *task);

/* TRUE if chao_init found the garden file damaged. each call clears that
 * finding, so only the first after chao_init returns TRUE */
Bool chao_take_bad_file(void);
/* the area loaded now, an enum chao_area, or CHAO_AREA_NONE before the first
 * loads */
int chao_current_area(void);
/* the area loaded before the current one, or CHAO_AREA_NONE while the first
 * is current */
int chao_previous_area(void);
/* restart the stage in area, an enum chao_area. player 0 carries the egg or
 * Chao it holds into area. it carries none where area or the current area is
 * CHAO_AREA_ENTRANCE or CHAO_AREA_RACE. while a sequence runs, a move from a
 * garden to the lobby calls chao_leave instead */
void chao_change_area(int area);
/* leave the Chao world. the call copies its Chao, trees and races into the
 * garden file, and plays the leaving sound. while an area is loaded, the
 * stage's task then fades to white. last, it writes the file and ends the
 * stage as cleared */
void chao_leave(void);
/* garden_drop_headgear's kinds. a Chao that puts one on wears its kind as
 * parameters.hat */
enum garden_headgear {
    GARDEN_HEADGEAR_PUMPKIN = 1,
    GARDEN_HEADGEAR_SKULL,
    GARDEN_HEADGEAR_SHELL
};
/* the id of loose headgear's PICKUP_LOWER shape, by which a Chao finds it */
#define GARDEN_HEADGEAR_HIT 147
/* loose headgear's kind, which its task's work keeps in ang.x */
#define HEADGEAR_KIND(work) ((work)->ang.x)
#ifdef NONMATCHING
/* this side returns the headgear, or null where it can't be made */
struct task *garden_drop_headgear(int kind, NJS_POINT3 *position, Angle yaw,
                                  NJS_VECTOR *velocity);
#else
void garden_drop_headgear(int kind, NJS_POINT3 *position, Angle yaw,
                          NJS_VECTOR *velocity);
#endif

void fun_0c500be0(struct task *task);
void garden_pulse_scale(struct task *task);
#endif
