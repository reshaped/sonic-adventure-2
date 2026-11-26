/* the boss attack: MENU_MODE_BOSS_ATTACK's menu, whose courses each play a
 * sequence of bosses. boss_attack_panel.h's panel draws it */
#ifndef ADVERTISE_BOSS_ATTACK_H
#define ADVERTISE_BOSS_ATTACK_H
#include <sg_xpt.h>

/* start the menu, its textures and its music. the caller sets
 * menu_control.mode */
void boss_attack_start(void);
/* the menu's step under MENU_MODE_BOSS_ATTACK. Choose on a course whose
 * award is given sets menu_resume_target to MENU_RESUME_BOSS_ATTACK and
 * keeps the menus' snapshot. it also empties the bosses' times, by
 * boss_times_clear. once the panel has faded out, the step starts the
 * course's sequence and returns TRUE. it returns FALSE otherwise. Cancel
 * goes back to the new menu's single-player group. while the work's
 * from_emblems is 1, it goes back to the emblem results' menu instead, and
 * clears from_emblems */
Bool boss_attack_step(void);
/* restore the menus' snapshot, and start the menu under
 * MENU_MODE_BOSS_ATTACK. menus_resume calls it for MENU_RESUME_BOSS_ATTACK */
void boss_attack_resume(void);

#endif
