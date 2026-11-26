/* Super Sonic and Super Shadow, the two players who take turns at the
 * Finalhazard's fight */
#ifndef BOSS_LAST2_SUPER_H
#define BOSS_LAST2_SUPER_H
/* make Super Sonic or Super Shadow player number player, 0 or 1, hovering.
 * the call makes no player where the heap can't give the form's task or
 * record. where finalhazard_players is null, it also starts the manager,
 * which hands the fight between the players. the manager allocates
 * finalhazard_players. where the heap can't give the manager or
 * that record, finalhazard_players stays null. the players' draws then read
 * through null. the call loads the forms' effect textures where they aren't
 * loaded. a task it creates then releases them as it dies. it doesn't check
 * the form's collision record, its loads, its quills' jiggle or that task */
void super_sonic_create(int player);
void super_shadow_create(int player);
#endif
