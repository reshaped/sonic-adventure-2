/* the anger of a Dark Chao at a player it dislikes */
#ifndef CHAO_AGGRESSION_H
#define CHAO_AGGRESSION_H
#include <sg_xpt.h>
struct task;
/* chao_choose_intention's bid from a Dark Chao's anger. it bids while the
 * Chao's kindness is 30 or less and its anger passes
 * chao_tuning.anger_threshold. a Chao that sees a player it dislikes bids to
 * confront or complain at it, by its aggression. otherwise it bids to sulk */
void score_player_dislike(struct task *chao, Float *best);
/* pull a face, CHAO_ANIM_AKANBE, and finish as it ends */
int chao_complain_at_player(struct task *chao);
/* walk at player 0, and within 5 of it strike and confront it again. each
 * strike takes 10 from the Chao's anger. past 10 away the Chao sulks. the
 * confrontation finishes on its 1002nd frame, or at once where the Chao's
 * anger is below 10 */
int chao_confront_player(struct task *chao);
#endif
