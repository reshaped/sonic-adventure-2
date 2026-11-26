/* the course menus, one for each race group. the challenge, dark and hero
 * menus are the rivals' grid, which entrance_event_tiles and
 * entrance_event_info make */
#ifndef CHAOSTGENTRANCE_OPTIONS_H
#define CHAOSTGENTRANCE_OPTIONS_H

struct task;
void entrance_beginner_courses(struct task *parent);
void entrance_jewel_courses(struct task *parent);
void entrance_challenge_courses(struct task *parent);
void entrance_dark_courses(struct task *parent);
void entrance_hero_courses(struct task *parent);
void entrance_multiplayer_courses(struct task *parent);

void entrance_event_tiles(struct task *parent);
void entrance_event_info(struct task *parent);
#endif
