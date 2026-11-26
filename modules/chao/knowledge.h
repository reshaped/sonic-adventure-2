/* what a Chao knows: its memory of each player character, and its lessons */
#ifndef CHAO_KNOWLEDGE_H
#define CHAO_KNOWLEDGE_H
#include <sg_xpt.h>

struct task;

/* the character whose memory a Chao keeps for player 0. it is player 0's
 * own from PLAYER_SONIC to PLAYER_ROUGE, and PLAYER_SONIC for any other */
int chao_knowledge_player(void);
/* a Chao's memory of that character. liking runs from -100 to 100, and fear
 * from 0 to 200. time apart grows as chao_age_memories runs, to 50000, and
 * meetings stop at 65535 */
Sint8 chao_player_liking(struct task *chao);
Uint8 chao_player_fear(struct task *chao);
Uint16 chao_time_apart(struct task *chao);
Uint16 chao_meetings(struct task *chao);
/* add delta to the liking, and meet the character. a delta above 0 also
 * eases the fear, the more so the more aggressive and calm the Chao */
void chao_remember_player(struct task *chao, int delta);
/* add delta to the fear, the less so the more aggressive and calm the Chao */
void chao_fear_player(struct task *chao, int delta);
/* count a meeting, and cut the time apart to two thirds */
void chao_meet_player(struct task *chao);
/* lengthen the time apart from each character and each other Chao by
 * chao_tuning.time_apart_step. it does so once every
 * chao_tuning.memory_interval + 1 calls */
void chao_age_memories(struct task *chao);

/* a Chao's lessons, a set of bits for each subject. learn sets a lesson's
 * bit, forget clears the whole set, and knows returns whether the bit is
 * set */
void chao_learn_art(struct task *chao, int bit);
void chao_forget_art(struct task *chao);
Bool chao_knows_art(struct task *chao, int bit);
void chao_learn_dance(struct task *chao, int bit);
void chao_forget_dances(struct task *chao);
Bool chao_knows_dance(struct task *chao, int bit);
void chao_learn_song(struct task *chao, int bit);
void chao_forget_songs(struct task *chao);
Bool chao_knows_song(struct task *chao, int bit);
void chao_learn_instrument(struct task *chao, int bit);
void chao_forget_instruments(struct task *chao);
Bool chao_knows_instrument(struct task *chao, int bit);

/* learn, forget and knows for chao_knowledge.unk_06, a set of 16 bits */
void fun_0c50fd60(struct task *chao, int bit);
void fun_0c50fd74(struct task *chao);
Bool fun_0c50fd82(struct task *chao, int bit);

#endif
