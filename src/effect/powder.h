/* powder, waves and gunfire: specks, the two trails of a player's wave, and a
 * gun's flashes */
#ifndef EFFECT_POWDER_H
#define EFFECT_POWDER_H
#include <ninja.h>

/* make count specks of powder, each a task named PowderExecute in level 3.
 * none is made past the first the heap can't give. a speck starts within 0.25
 * of pos along each axis, moving at spd plus a random part. the part is up to
 * 0.5 either way along x and z, and up to 0.5 upward. world_gravity then pulls
 * the speck by 0.04 of itself a step. it draws as a point.
 *
 * speck i ends at its 15th, 31st, 15th or 61st step, as i mod 4 runs from 0 to
 * 3. each of those three lengths has an orange of its own, or a green on
 * STAGE_GREEN_FOREST and STAGE_WHITE_JUNGLE. a speck ends sooner once
 * player_work[player] is null */
void powder_create(int player, int count, NJS_POINT3 *pos, NJS_VECTOR *spd);

/* make the two trails of a player's wave, each a task named WaveExecute in
 * level 3. player is the player's slot. player_records[player] must not be
 * null. where the heap can't give the first trail, neither is made.
 *
 * a trail is a strip between two lines, drawn added to what lies behind it.
 * each line runs from a root toward the record's unk_230. the root is the
 * record's unk_200 for the first trail, and its unk_20c for the second. the
 * first line stays where the call finds the player. the second follows the
 * player until the trail's first draw. in a split screen it follows until the
 * first draw for the player's camera or a later one. the strip fades each step,
 * and once the second line stays, its lines draw back to their roots.
 *
 * from a trail's first step to its second, its draws add a model at the second
 * line's root. the model turns by the angles of player_work[player], which must
 * not be null then. no model draws on STAGE_GREEN_FOREST or STAGE_WHITE_JUNGLE.
 * a trail ends at its eighth step, or once player_records[player] is null */
void wave_create(int player);

/* make a flash at pos, a task named GunfireExecute in level 3. it turns to
 * the heading and pitch of dir, which must not be the zero vector. its scale
 * along each axis is random, from 0.7 up to 1.2. it takes one of two models.
 * its third step kills it. where the heap can't give the task, nothing is
 * made */
void gunfire_create(NJS_POINT3 *pos, NJS_VECTOR *dir);
/* gunfire_create, but with a scale from 0.5 up to 0.9, killed at its first
 * step */
void gunfire_create_brief(NJS_POINT3 *pos, NJS_VECTOR *dir);
/* gunfire_create_brief, but with a third model, turned at random about y
 * and about x */
void gunfire_create_unaimed(NJS_POINT3 *pos);

/* The three frames of a player wave, drawn in the sequence A, B, A, C. */
extern NJS_CNK_OBJECT wave_object_a;
extern NJS_CNK_OBJECT wave_object_b;
extern NJS_CNK_OBJECT wave_object_c;
/* Boss and player gunfire select the first two flashes by shot parity.
 * The third model is the unaimed flash. */
extern NJS_CNK_OBJECT gunfire_object_a;
/* @hack type: an array of one object, where one NJS_CNK_OBJECT would do. */
extern NJS_CNK_OBJECT gunfire_object_b[1];
extern NJS_CNK_OBJECT gunfire_object_c;

#endif
