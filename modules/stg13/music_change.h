/* the music change, ADXCHG in City Escape's object list. it changes the
 * stage's music once a player enters its ball */
#ifndef STG13_MUSIC_CHANGE_H
#define STG13_MUSIC_CHANGE_H

struct task;

/* the object list's step for ADXCHG. unless placement_try_leave(task)
 * returns TRUE, it gives task the change's own step, draw and destructor.
 * each step makes the same test. task needs a work, whose pos and scl.x are
 * the center and radius of the ball. a player enters the ball where
 * player_near finds it nearer than the radius.
 *
 * the first entry stops the music and plays c_escap2.adx. until then, each
 * step in STAGE_PLAY names c_escap3.adx as the music that music_play starts.
 * a retry lets the next entry make the change again, and a miss doesn't.
 *
 * the destructor names the music that music_play next starts, by what the
 * steps saw last. after a retry, or in STAGE_FADE_IN, it names c_escap1.adx.
 * after play, an entry or a miss, it names c_escap3.adx where player 0's
 * restart point has order 0. it names c_escap1.adx where the point has
 * another order or there is none. before any of those it names nothing */
void music_change_init(struct task *task);

#endif
