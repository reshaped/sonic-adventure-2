/* the bid of a Chao that sees the player */
#ifndef CHAO_PLAYER_VISIT_H
#define CHAO_PLAYER_VISIT_H
#include <sg_xpt.h>

struct task;
/* bid against *best to greet, complain at, follow or confront the player in
 * sight. the Chao's liking for player 0's character and their time apart
 * pick which. a confrontation also needs a dark Chao with an aggression above
 * 30. the Chao's loneliness sets the bid. a greeting or a complaint counts a
 * meeting */
void score_player_visit(struct task *chao, Float *best);
#endif
