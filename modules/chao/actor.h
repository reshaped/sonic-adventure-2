/* a garden Chao's task, from its creation to its removal */
#ifndef CHAO_ACTOR_H
#define CHAO_ACTOR_H
#include <ninja.h>
struct task;
struct chao_gene;
struct chao_profile;
struct chao_save;
struct garden_race_chao;
/* create a Chao at position facing yaw, or at the origin where position is
 * null. the Chao copies gene, or draws a new one where gene is null. it copies
 * profile, or hatches one from its gene where profile is null. where
 * chao_current_area() is CHAO_AREA_DARK or less, the profile's garden becomes
 * that area. a non-null appearance gives the Chao a recorded racer's form,
 * looks, name and stats. a non-null save becomes the Chao's save record.
 * chao_create clears the record and copies the Chao's profile and gene into it
 */
struct task *chao_create(const struct chao_gene *gene,
                         const struct chao_profile *profile,
                         const struct garden_race_chao *appearance,
                         NJS_POINT3 *position, Angle yaw,
                         struct chao_save *save);
/* draw the Chao by its profile's body */
void chao_actor_draw(struct task *chao);
/* clear the Chao's save record and kill its task */
void chao_remove_resident(struct task *chao);
#endif
