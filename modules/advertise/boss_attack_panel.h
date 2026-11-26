/* the boss attack's panel: the courses, and the best times and emblem of
 * the course under the cursor. its task draws the panel as boss_attack.h's
 * menu sets the work */
#ifndef ADVERTISE_BOSS_ATTACK_PANEL_H
#define ADVERTISE_BOSS_ATTACK_PANEL_H
#include "advertise/menus.h"
#include "hud/hud.h"
#include "save/save.h"

/* the courses are hud.h's BOSS_COURSE_ values. course n can be chosen once
 * save_data.awards[BOSS_ATTACK_FIRST_AWARD + n] is given. those are the hero,
 * the dark and the last story's awards in turn. BOSS_COURSE_ALL shows only
 * once its award is given */
#define BOSS_ATTACK_COURSES (BOSS_COURSE_ALL + 1)
#define BOSS_ATTACK_FIRST_AWARD SAVE_AWARD_HERO_STORY

/* the work, menu_works.work_15 */
extern struct boss_attack_work boss_attack_work;

/* create the task, bossAttackExec, with the work cleared and the panel
 * hidden. the task's destructor releases the textures */
void boss_attack_panel_create(void);
/* clear the work but loaded, and hide the panel at once */
void boss_attack_panel_reset(void);
/* load the textures, or release them, and set loaded to match. neither
 * does anything where loaded matches already */
void boss_attack_panel_load(void);
void boss_attack_panel_release(void);

#endif
