/* the dances a Chao learns at the Kindergarten, which other Chao watch or
 * join */
#ifndef CHAO_DANCE_H
#define CHAO_DANCE_H
#include <sg_xpt.h>
struct task;
/* set a dance the Chao knows, at random, and make the dance's field 8 ahead
 * of it. the field lasts 1800 to 4200 frames. FALSE where the Chao knows no
 * dance or stands in a dance's field already, which sets nothing */
Bool chao_start_dance(struct task *chao);
/* chao_choose_intention's bid from the Chao's boredom, while the Chao stands in
 * a dance's field. it bids nothing where *best is 1 or more. its bid is 0 where
 * boredom is chao_tuning.boredom_threshold or less. a bid above *best has the
 * Chao join the field owner's dance. it joins only a dance it knows, and never
 * the hand dance. otherwise the Chao sits and watches. *best then becomes 0.99
 */
void score_nearby_dance(struct task *chao, Float *best);
#endif
