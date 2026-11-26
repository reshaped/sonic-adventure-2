/* a garden Chao singing, and the Chao that hear it */
#ifndef CHAO_SINGING_H
#define CHAO_SINGING_H
#include <sg_xpt.h>
struct task;
/* make chao sing and return TRUE. return FALSE where chao is in an in-tune
 * song's field already, or knows no song. a Chao that knows n classroom
 * songs sings the song of its nth lesson. one that knows none but has eaten
 * a parrot sings the parrot's song. it sings off key where its alignment is
 * below -0.5 */
int chao_start_singing(struct task *chao);
void score_hearing_song(struct task *chao, Float *best);
void score_hearing_bad_song(struct task *chao, Float *best);
/* start a voice, a task that keeps sound playing for frames frames. it plays
 * at the pos source has at the call, and keeps only sound's low byte. yaw
 * becomes the task's ang.y, which nothing reads */
void chao_create_voice(struct task *source, int sound, Sint16 yaw,
                       Sint16 frames);
#endif
