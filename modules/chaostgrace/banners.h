/* the Chao race's banners: its start, its goal, and the win and retry
 * results */
#ifndef CHAOSTGRACE_BANNERS_H
#define CHAOSTGRACE_BANNERS_H

struct task;

void race_start_banner_create(void);
void race_goal_banner_create(void);
void race_win_banner_create(void);
void race_retry_banner_create(void);

/* a banner keeps its opacity in its work's scl.x and its scale in scl.y.
 * race_banner_reset sets them to 0 and BANNER_FIRST_SCALE. each
 * race_banner_expand step raises the opacity by BANNER_STEP to 1. it raises
 * the scale by BANNER_STEP up to 1, then by BANNER_SWELL. each step of a
 * hiding banner lowers its opacity by BANNER_STEP and raises its scale by
 * BANNER_SWELL */
#define BANNER_FIRST_SCALE 0.2F
#define BANNER_STEP 0.1F
#define BANNER_SWELL 0.005F
void race_banner_reset(struct task *task);
void race_banner_expand(struct task *task);

#endif
